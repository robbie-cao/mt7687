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

/*****************************************************************************
 *
 * Filename:
 * ---------
 *   cal_task_main.c
 *
 * Project:
 * --------
 *   All
 *
 * Description:
 * ------------
 *   Interface between CAL and UI. To receive messages from UI.
 *
 *
 *****************************************************************************/

/* system includes */
// CAL API include
#include "cal_api.h"
#include "cal_if.h"
#include "cal_drv_features.h"
#include "cal_task_msg_if.h"
#include "cal_mem_def.h"
#include "isp_if.h"

kal_bool WaitStartupCnfFlag=KAL_FALSE;

#if defined(__MM_DCM_SUPPORT__)
#pragma arm section code, rodata
#endif

/*****************************************************************************
 * FUNCTION
 *  CalTaskMain
 * DESCRIPTION
 *  This function is main function of CAL task. use to receive msgs from APP.
 * PARAMETERS
 *  task_entry_ptr      [?]
 * RETURNS
 *  void
 *****************************************************************************/
void CalTaskMain(void *pvParameters)
{
    MM_ERROR_CODE_ENUM ret = MM_ERROR_NONE;
    CAL_MSG msgs;

    while(1)
    { 
        if(xQueueReceive(cal_queue_handle, &msgs, portMAX_DELAY)) {
            switch(msgs.msg_id)
            {
                case MSG_ID_MEDIA_CAM_PREVIEW_REQ:
                    if(CalState == CAL_IDLE_STATE) {
                        LOG_I(hal, "Msg from source module:%s\r\n", msgs.src_mod);

                        ret = CalOpen();
                        if(ret != MM_ERROR_NONE) {
                            LOG_E(hal, "Calopen error!\r\n");
                            break;
                        }

                        ret = CalCameraPreviewReq();
                        if(ret != MM_ERROR_NONE) {
                            LOG_E(hal, "Cam preview error!\r\n");
                            CalClose();
                        }
                    } else {
                        LOG_E(hal, "Cam is in wrong state!\r\n");
                    }
                break;
                case MSG_ID_MEDIA_CAM_POWER_DOWN_REQ:
                    if(CalState == CAL_CAMERA_PREVIEW_STATE || CalState == CAL_CAMERA_CAPTURE_STATE) {
                        LOG_I(hal, "Msg from source module:%s\r\n", msgs.src_mod);

                        ret = CalCameraPowerDownReq();
                        if(ret != MM_ERROR_NONE) {
                            LOG_E(hal, "Cam power down error!\r\n");
                            configASSERT(0);
                        }

                        CalClose();
                    } else {
                        CalClose();
                    }
                break;
                case MSG_ID_MEDIA_CAM_STOP_PREVIEW_REQ:
                break;
                default:
                break;
            }
        }
    }
} /* CalTaskMain() */

