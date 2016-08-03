/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

/* system includes */
#include "kal_release.h"
#include "kal_trace.h"
#include "stack_common.h"
#include "stack_msgs.h"

#include "camera_nvram_def.h"
#include "nvram_struct.h"

// CAL API include
#include "cal_api.h"
#include "cal_if.h"
#include "cal_drv_features.h"

#include "Isp_hw_feature_def.h"

#if defined(__MM_DCM_SUPPORT__)
#pragma arm section code = "DYNAMIC_CODE_CAMCAL_ROCODE", rodata = "DYNAMIC_CODE_CAMCAL_ROCODE"
#endif

kal_uint32 MainSensorListIdx, CurrentSensorListIdx;
#ifdef __CAL_NVRAM_NEED_READ__
ISP_FEATURE_NVRAM_INFO_STRUCT CameraNvramInfo;
ISP_FEATURE_NVRAM_DATA_STRUCT CameraNvramData;
kal_uint32 CurrentNvramFileLid=0, LastNvramFileLid=0;
kal_bool CctAccessNvram=KAL_FALSE;
#endif
#ifdef __CAL_DUAL_CAMERA_SUPPORT__
kal_uint32 SubSensorListIdx;
#endif
#ifdef __CAL_MATV_SUPPORT__
kal_uint32 MatvSensorListIdx;
#endif

kal_uint32 GetSensorListIdx(kal_bool BySensorId, kal_uint32 SensorId, IMAGE_SENSOR_INDEX_ENUM SensorIdx)
{
    kal_uint32 i=0;

    if (KAL_TRUE==BySensorId)
    {
        for (i=0; i<MAX_SENSOR_SUPPORT_NUMBER; i++)
        {
            if (SensorId == (pImageSensorList+i)->SensorId)
            {
                    return (i);
            }
        }
    }

    if ((KAL_FALSE==BySensorId)||
        ((KAL_TRUE==BySensorId) && (MAX_SENSOR_SUPPORT_NUMBER==i)))
    { // by SensorIdx
        for (i=0; i<MAX_SENSOR_SUPPORT_NUMBER; i++)
        {
            if (SensorIdx == (pImageSensorList+i)->SensorIdx)
            {
                return (i);
            }
        }
    }

    if (MAX_SENSOR_SUPPORT_NUMBER==i)
        ASSERT(0);

    return (i);
} /* GetSensorListIdx() */

void CalSendMsgToNvram(msg_type MsgId, kal_uint16 ef_id, void *DataPtr, kal_uint16 Length)
{
    peer_buff_struct *pPeerBuff;
    local_para_struct *pLocalPara;
    kal_uint16 PduLen;

    switch (MsgId)
    {
        case MSG_ID_NVRAM_WRITE_REQ:
            pLocalPara = construct_local_para(sizeof(nvram_write_req_struct), TD_CTRL);
            pPeerBuff = construct_peer_buff(Length, 0, 0, TD_CTRL);

            ((nvram_write_req_struct*) pLocalPara)->file_idx = (kal_uint16) ef_id;
            ((nvram_write_req_struct*) pLocalPara)->para = 1;
            ((nvram_write_req_struct*) pLocalPara)->access_id = 0;

            PduLen = Length;
            kal_mem_cpy(get_pdu_ptr(pPeerBuff, &PduLen), DataPtr, Length);

            CalSendIlm(MOD_CAL, MOD_NVRAM, MSG_ID_NVRAM_WRITE_REQ, pLocalPara, pPeerBuff, KAL_TRUE);
        break;
        case MSG_ID_NVRAM_READ_REQ:
            pLocalPara = construct_local_para(sizeof(nvram_read_req_struct), TD_CTRL);

            ((nvram_read_req_struct*) pLocalPara)->file_idx = (kal_uint16) ef_id;
            ((nvram_read_req_struct*) pLocalPara)->para = 1;

            CalSendIlm(MOD_CAL, MOD_NVRAM, MSG_ID_NVRAM_READ_REQ, pLocalPara, NULL, KAL_TRUE);
        break;
    }
} /* CalSendMsgToNvram() */

void CalNvramReadCnfHandle(ilm_struct *pIlmPtr)
{
    nvram_read_cnf_struct *pLocalPara;
    kal_uint8 *pPduPtr;
    kal_uint16 PduLen;

#ifdef __CAL_NVRAM_NEED_READ__
    kal_uint32 FeatureOutLen;
#endif

    pLocalPara = (nvram_read_cnf_struct *) pIlmPtr->local_para_ptr;
    pPduPtr = get_pdu_ptr(pIlmPtr->peer_buff_ptr, &PduLen);

    switch (pLocalPara->file_idx)
    {
        case NVRAM_EF_CAMERA_MAIN_SENSOR_ID_LID:
            MainSensorId = *((kal_uint32 *) pPduPtr);
            if ((NULL_SENSOR_ID!=MainSensorId)&&(0!=MainSensorId))
                MainSensorListIdx=GetSensorListIdx(KAL_TRUE, MainSensorId, IMAGE_SENSOR_MAIN);
            else
                MainSensorListIdx=GetSensorListIdx(KAL_FALSE, 0, IMAGE_SENSOR_MAIN);

        #ifdef __CAL_DUAL_CAMERA_SUPPORT__
            if (KAL_TRUE==SubSensorSupport)
            {
                CalSendMsgToNvram(MSG_ID_NVRAM_READ_REQ, NVRAM_EF_CAMERA_SUB_SENSOR_ID_LID,0,0 );
            }
            else
        #endif
            {   // start to read ISP related NVRAM data.
                // 1. query ISP driver for Nvram LID
                // 2. send meeage to read nvram
            #ifdef __CAL_NVRAM_NEED_READ__

                pfIspFunc->IspFeatureCtrl(ISP_FEATURE_GET_NVRAM_INFO, (void *) &MainSubCameraStatus,
                                          (void *) &CameraNvramInfo, 0, &FeatureOutLen);

                CurrentSensorListIdx = MainSensorListIdx;
                CurrentNvramFileLid = CameraNvramInfo.CameraNvramStartFileLid[(pImageSensorList+CurrentSensorListIdx)->SensorIdx];
                LastNvramFileLid = CameraNvramInfo.CameraNvramEndFileLid[(pImageSensorList+CurrentSensorListIdx)->SensorIdx];
                CalSendMsgToNvram(MSG_ID_NVRAM_READ_REQ, CurrentNvramFileLid, 0, 0);
            #else
                if (KAL_TRUE==WaitStartupCnfFlag)
                {
                    CalSendIlm(MOD_CAL, MOD_MED, MSG_ID_CAL_STARTUP_CNF, NULL, NULL, KAL_TRUE);
                }

                if (CalEventId!=NULL)
                    kal_set_eg_events(CalEventId,CAL_NVRAM_READ_EVENT,KAL_OR);
            #endif
            }
        break;
    #ifdef __CAL_DUAL_CAMERA_SUPPORT__
        case NVRAM_EF_CAMERA_SUB_SENSOR_ID_LID:
            SubSensorId = *((kal_uint32 *) pPduPtr);
            if ((NULL_SENSOR_ID!=SubSensorId)&&(0!=SubSensorId))
                SubSensorListIdx=GetSensorListIdx(KAL_TRUE, SubSensorId, IMAGE_SENSOR_SUB);
            else
                SubSensorListIdx=GetSensorListIdx(KAL_FALSE, 0, IMAGE_SENSOR_SUB);

        #ifdef __CAL_NVRAM_NEED_READ__
            // start to read ISP related NVRAM.
            pfIspFunc->IspFeatureCtrl(ISP_FEATURE_GET_NVRAM_INFO, (void *) &MainSubCameraStatus,
                                      (void *) &CameraNvramInfo, 0, &FeatureOutLen);

            CurrentSensorListIdx = MainSensorListIdx;
            CurrentNvramFileLid = CameraNvramInfo.CameraNvramStartFileLid[(pImageSensorList+CurrentSensorListIdx)->SensorIdx];
            LastNvramFileLid = CameraNvramInfo.CameraNvramEndFileLid[(pImageSensorList+CurrentSensorListIdx)->SensorIdx];
            CalSendMsgToNvram(MSG_ID_NVRAM_READ_REQ, CurrentNvramFileLid, 0, 0);
        #else
            if (KAL_TRUE==WaitStartupCnfFlag)
            {
                CalSendIlm(MOD_CAL, MOD_MED, MSG_ID_CAL_STARTUP_CNF, NULL, NULL, KAL_TRUE);
            }

            if (CalEventId!=NULL)
                kal_set_eg_events(CalEventId,CAL_NVRAM_READ_EVENT,KAL_OR);
        #endif
        break;
    #endif
        default:
            // for ISP Nvram LID
            // 1. pass the Nvram content to ISP driver
            // 2. if not the end of Nvram read request, read next one or stop nvram read
        #ifdef __CAL_NVRAM_NEED_READ__
            CameraNvramData.CameraNvramFileLid=CurrentNvramFileLid;
            CameraNvramData.pCameraNvramFileData=(void *) pPduPtr;
            CameraNvramData.CameraNvramDataSize=PduLen;
            pfIspFunc->IspFeatureCtrl(ISP_FEATURE_SET_NVRAM_DATA, (void *) &CameraNvramData, NULL, 0, &FeatureOutLen);

            if (CurrentNvramFileLid<LastNvramFileLid)
            {
                CurrentNvramFileLid++;
                CalSendMsgToNvram(MSG_ID_NVRAM_READ_REQ, CurrentNvramFileLid, 0, 0);
            }
            else
            {
            #ifdef __CAL_DUAL_CAMERA_SUPPORT__
                if ((CurrentSensorListIdx==MainSensorListIdx) && (KAL_TRUE==SubSensorSupport))
                { // start to read NVRAM for sub sensor
                    CurrentSensorListIdx = SubSensorListIdx;
                    CurrentNvramFileLid = CameraNvramInfo.CameraNvramStartFileLid[(pImageSensorList+CurrentSensorListIdx)->SensorIdx];
                    LastNvramFileLid = CameraNvramInfo.CameraNvramEndFileLid[(pImageSensorList+CurrentSensorListIdx)->SensorIdx];
                    CalSendMsgToNvram(MSG_ID_NVRAM_READ_REQ, CurrentNvramFileLid, 0, 0);
                }
                else
            #endif
                {
                    if ((KAL_TRUE==WaitStartupCnfFlag) && (KAL_FALSE==CctAccessNvram))
                    {
                        WaitStartupCnfFlag=KAL_FALSE;
                        CalSendIlm(MOD_CAL, MOD_MED, MSG_ID_CAL_STARTUP_CNF, NULL, NULL, KAL_TRUE);
                    }
                    if (CalEventId!=NULL)
                        kal_set_eg_events(CalEventId,CAL_NVRAM_READ_EVENT,KAL_OR);
                }
            }
        #endif
        break;
    }
} /* CalNvramReadCnfHandle() */

void CalNvramWriteCnfHandle(ilm_struct *pIlmPtr)
{
    nvram_write_cnf_struct *pLocalPara;

    pLocalPara = (nvram_write_cnf_struct *) pIlmPtr->local_para_ptr;

    switch (pLocalPara->file_idx)
    {
        case NVRAM_EF_CAMERA_MAIN_SENSOR_ID_LID:
    #ifdef __CAL_DUAL_CAMERA_SUPPORT__
        case NVRAM_EF_CAMERA_SUB_SENSOR_ID_LID:
    #endif
            if (CalEventId!=NULL)
                kal_set_eg_events(CalEventId,CAL_NVRAM_READ_EVENT,KAL_OR);
        break;
        default:
        break;
    }
} /* CalNvramWriteCnfHandle() */

#if defined(__MM_DCM_SUPPORT__)
#pragma arm section code, rodata
#endif



