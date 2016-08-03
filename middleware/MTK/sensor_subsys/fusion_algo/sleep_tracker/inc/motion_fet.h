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

#ifndef __MOTION_FET_H__
#define __MOTION_FET_H__
#ifdef _cpluscplus
extern "C"{
#endif

/**
 * @addtogroup API
 * @{
 * This document include all APIs for motion features extraction.
 * @addtogroup ST
 * @{
 * This document mainly introduce all details about the ST APIs including terms and acronym,
 * supported features, software achitechture, how to use this APIs, ST function groups, all enum, structure and functions.
 *
 * @section Terms_Chapter Terms and acronyms
 *
 * |Terms              |Details                                                                 |
 * |-------------------|------------------------------------------------------------------------|
 * |\b ST              | Sleep Tracker
 *
 * @section Features_Chapter Supported features
 *
 * - Based on moton_st library, this advanced APIs can extract motion-related features for sleep staging. \n 
 *
 * @section Usage_Chapter How to use this API
 *
 * - Use ST library \n
 *  - step1: Call motion_st_init() to intialize sleep tracking algorithm.
 *  - step2: Call motion_st() to set motion data.
 *  - step3: Call #check_sleep_motion_fet to check if features are available. The return number means total available features.
 *  - step4: Allocate appropriate buffer to #get_one_motion_fet. Buffer size can be determined by #check_sleep_motion_fet.
 *           In the sample code, 100 elements are preallocated for features and corresponding timestamps.
 *  - step5: Call #rearrange_queue_for_motion_fet to manage internal buffer in sleep tracker APIs. It must be called after full features are accessed. 
 *  - sample code:
 *    @code
 *       int ret;
 *       int fet_count = 0;
 *       int          motion_fet[100];
 *       unsigned int motion_fet_time[100]; 
 *       ret = motion_st_init();//initialize ST hardware, which power on ST.
 *       if (ret == SD_STATUS_OK) {
 *         // infinite-loop to set data   
 *         while (input != NULL) {
 *           motion_st( input->data->accelerometer_t.x,
 *                      input->data->accelerometer_t.y,
 *                      input->data->accelerometer_t.z, 
 *                      input->data->time_stamp); 
 *           fet_count = check_sleep_motion_fet();
 *           if(fet_count > 0) {
 *             // loop to get motion features
 *             for (j=0;j<fet_count;j++) {            
 *               ret = get_one_motion_fet(&motion_fet[j], &motion_fet_time[j]);           
 *             }
 *             rearrange_queue_for_motion_fet();
 *           }
 *         }
 *       }        
 *    @endcode
 *
 * @section Function_Group_Chapter ST function groups description
 */
//---------------------------------------------------------
// sleep tracker status definition
//---------------------------------------------------------
/** @brief  This macro defines numeric value for error indication. SD_STATUS_OK: Failure during functions call
  */
#define SD_STATUS_OK               0
/** @brief  This macro defines numeric value for error indication. SD_STATUS_ERROR: No error happen during functions call
  */
#define SD_STATUS_ERROR           -1

//---------------------------------------------------------
// functions for sleep tracker
//---------------------------------------------------------
/**
 * @brief This function is used to check if available features can be accessed. \n
 * @return    Return number is the available data. Return 0 if no available data. 
 *  - sample code:
 *    @code
 *       int ret;
 *       int fet_count = 0;
 *       int          motion_fet[100];
 *       unsigned int motion_fet_time[100]; 
 *       ret = motion_st_init();//initialize ST hardware, which power on ST.
 *       if (ret == SD_STATUS_OK) {
 *         // infinite-loop to set data   
 *         while (input != NULL) {
 *           motion_st( input->data->accelerometer_t.x,
 *                      input->data->accelerometer_t.y,
 *                      input->data->accelerometer_t.z, 
 *                      input->data->time_stamp); 
 *           fet_count = check_sleep_motion_fet();
 *           if(fet_count > 0) {
 *             // loop to get motion features
 *             for (j=0;j<fet_count;j++) {            
 *               ret = get_one_motion_fet(&motion_fet[j], &motion_fet_time[j]);           
 *             }
 *             rearrange_queue_for_motion_fet();
 *           }
 *         }
 *       }        
 *    @endcode
 */    
int  check_sleep_motion_fet(void);

/**
 * @brief This function is used to get one set of motion features and its corresponding timestamp if available\n
 * @param[in] results is the pointer for features.
 *            Please call #check_sleep_motion_fet to allocate appropriate buffer size before calling #get_one_motion_fet.
 *            Loop N times to get full results and N can be obtained by calling #check_sleep_motion_fet. 
 * @param[in] results_timestamp is the pointer for features timestamp.
 * @return    Return #SD_STATUS_OK as success and #SD_STATUS_ERROR for failure indication.
 * Sample code please refer to #check_sleep_motion_fet
 */  
int  get_one_motion_fet(int *results, unsigned int *results_timestamp);

/**
 * @brief This function is used to manage internal buffer in sleep tracker APIs. It must be called after full features are accessed.\n
 * @return    Return #SD_STATUS_OK as success and #SD_STATUS_ERROR for failure indication.
 * Please refer to #check_sleep_motion_fet for sample code.
 */  
int  rearrange_queue_for_motion_fet(void);

#ifdef _cpluscplus
}
#endif

#endif /* __MOTION_FET_H__*/
