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

#ifndef __HR_FET_H__
#define __HR_FET_H__
#ifdef _cpluscplus
extern "C"{
#endif



   
/**
 * @addtogroup API
 * @{
 * This document include all heart rate (HR) features extraction APIs for sleep staging. 
 * Details about heart rate (HR) features extraction APIs including terms and acronym,
 * supported features, software achitechture, how to use this driver, HR features extraction function groups, all enum, structure and functions are introduced.
 *
 * @section Terms_Chapter Terms and acronyms
 *
 * |Terms              |Details                                                                 |
 * |-------------------|------------------------------------------------------------------------|
 * |\b HR              | Heart Rate                                                             |
 * |\b PPG             | Photoplethysmography                                                   |
 * |\b BISI            | Beat Interval and Signal Integrity. This is a proprietary signal in MediaTek Clover. |
 *
 * @section Features_Chapter Supported features
 *
 * - The advanced APIs can extract HR-related features for sleep staging. \n 
 *
 * @section Driver_Usage_Chapter How to use this API
 *
 * - Use ST library \n
 *  - step1: Call #hr_fet_init to intialize HR features extraction.
 *  - step2: Call #hr_fet_ext to set HR and BISI data.
 *  - step3: Call #check_sleep_hr_fet to check if features are available. The return number means total available features.
 *  - step4: Allocate appropriate buffer to #get_one_hr_fet. Buffer size can be determined by #check_sleep_hr_fet.
 *           In the sample code, 100 elements are preallocated for features and corresponding timestamps.
 *  - step5: Call #rearrange_queue_for_hr_fet to manage internal buffers in HR features extraction APIs. It must be called after full features are accessed. 
 *  - sample code:
 *    @code
 *       int ret;
 *       int fet_count = 0;
 *       int          hr_fet[100];
 *       unsigned int hr_fet_time[100]; 
 *       ret = hr_fet_init();
 *       if (ret == HR_FET_STATUS_OK) {
 *         while (input != NULL) {
 *           if(input_list->sensor_type == PPG_TYPE) {
 *             hr_fet_ext(ppg_bisi_data, ppg_bisi_time, 0);
 *           }
 *           else if (input_list->sensor_type == BISI_TYPE) {
 *             hr_fet_ext(ppg_bisi_data, ppg_bisi_time, 1);
 *           }
 *           fet_count = check_sleep_hr_fet();             *
 *           if(fet_count > 0) {
 *             // loop to get heart rate features
 *             for (j=0;j<fet_count;j++) {            
 *               ret = get_one_hr_fet(&hr_fet[j], &hr_fet_time[j]);           
 *             }
 *             rearrange_queue_for_hr_fet();
 *           }
 *         }
 *       }        
 *    @endcode
 *
 * @section Function_Group_Chapter ST function groups description
 */
//---------------------------------------------------------
// Definitions of HR features extraction status 
//---------------------------------------------------------
/** @brief  This macro defines numeric value for error indication. HR_FET_STATUS_OK: Failure during functions call
  */
#define HR_FET_STATUS_OK               0
/** @brief  This macro defines numeric value for error indication. HR_FET_STATUS_ERROR: No error happen during functions call
  */
#define HR_FET_STATUS_ERROR           -1
//---------------------------------------------------------
// signal type definition
//---------------------------------------------------------
/** @brief  This macro defines numeric value for signal type. \n PPG_TYPE: PPG
  */
#define PPG_TYPE                       0
/** @brief  This macro defines numeric value for signal type. \n BISI_TYPE: BISI
  */
#define BISI_TYPE                      1
//---------------------------------------------------------
// functions for heart rate features extraction
//---------------------------------------------------------
/**
 * @brief Initialize parameters for heart rate feature extraction.
 * @return    Return #HR_FET_STATUS_OK as success and #HR_FET_STATUS_ERROR for failure indication.
 * Please refer to #hr_fet_ext for sample code.
 */
int  hr_fet_init(void);
/**
 * @brief This function is main function to extract heart rate features for sleep staging. Call #hr_fet_ext with one set of PPG/BISI sample\n
 * @param[in] ppg_bisi is the PPG/BISI data inputs.
 * @param[in] timestamp is the corresponding timestamp to PPG/BISI data.
 * @param[in] sensor_type is the type definition for event timestamp.
 * @return    Return #HR_FET_STATUS_OK as success and #HR_FET_STATUS_ERROR for failure indication.
 *  - sample code:
 *    @code
 *       int ret;
 *       int fet_count = 0;
 *       int          hr_fet[100];
 *       unsigned int hr_fet_time[100]; 
 *       ret = hr_fet_init();
 *       if (ret == HR_FET_STATUS_OK) {
 *         while (input != NULL) {
 *           if(input_list->sensor_type == PPG_TYPE) {
 *             hr_fet_ext(ppg_bisi_data, ppg_bisi_time, 0);
 *           }
 *           else if (input_list->sensor_type == BISI_TYPE) {
 *             hr_fet_ext(ppg_bisi_data, ppg_bisi_time, 1);
 *           }
 *           fet_count = check_sleep_hr_fet();             *
 *           if(fet_count > 0) {
 *             // loop to get heart rate features
 *             for (j=0;j<fet_count;j++) {            
 *               ret = get_one_hr_fet(&hr_fet[j], &hr_fet_time[j]);           
 *             }
 *             rearrange_queue_for_hr_fet();
 *           }
 *         }
 *       }        
 *    @endcode
 */  
int  hr_fet_ext(int ppg_bisi, unsigned int timestamp, uint8_t sensor_type);

/**
 * @brief This function is used to check if available features can be accessed. \n
 * @return    Return number is the available data. Return 0 if no available data. 
 * Sample code please refer to #hr_fet_ext
 */    
int  check_sleep_hr_fet(void);

/**
 * @brief This function is used to get one set of motion features and its corresponding timestamp if available\n
 * @param[in] results is the pointer for event label.
 *            Please call #check_sleep_hr_fet to allocate appropriate buffer size before calling #get_one_hr_fet.
 *            Loop N times to get full results and N can be obtained by calling #check_sleep_hr_fet. 
 * @param[in] results_timestamp is the pointer for event timestamp.
 * @return    Return #HR_FET_STATUS_OK as success and #HR_FET_STATUS_ERROR for failure indication.
 * Sample code please refer to #hr_fet_ext
 */  
int  get_one_hr_fet(int *results, unsigned int *results_timestamp);


/**
 * @brief This function is used to manage internal buffer in HR features extraction APIs. It must be called after full events are accessed.\n
 * @return    Return #HR_FET_STATUS_OK as success and #HR_FET_STATUS_ERROR for failure indication.
 * Please refer to #hr_fet_ext for sample code.
 */  
int  rearrange_queue_for_hr_fet(void);


#ifdef _cpluscplus
}
#endif

#endif /* __HR_FET_H__*/
