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

#ifndef __MOTION_ST_H__
#define __MOTION_ST_H__
#ifdef _cpluscplus
extern "C"{
#endif

/**
 * @addtogroup API
 * @{
 * This document include all APIs for sleep tracker.
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
 * |\b SE              | Sleep Efficiency (%) is the ratio of time spent asleep (total sleep time) to the amount of time spent on bed.
 *
 * @section Features_Chapter Supported features
 *
 * - The function can detect sleep by continuously feeding in 3-axis accelerometers data in 10-Hz sampling rate. \n
 * - SE (%) and sleep states with corresponding timestamp (AWAKE, ASLEEP, and RESTLESS) are reported if a complete sleep duration is detected. \n
 *
 * @section Usage_Chapter How to use this API
 *
 * - Use ST library \n
 *  - step1: Call #motion_st_init to intialize sleep tracking algorithm.
 *  - step2: Call #motion_st to set motion data.
 *  - step3: Call #check_sleep_results to check if sleep events are available. The return number means total available events.
 *  - step4: Allocate appropriate buffer to #get_one_sleep_result. Buffer size can be determined by #check_sleep_results.
 *           In the sample code, 100 elements are preallocated for sleep events and corresponding timestamps.
 *  - step5: Call #rearrange_queue_for_sleep to manage internal buffer in sleep tracker APIs. It must be called after full events are accessed. 
 *  - sample code:
 *    @code
 *       int ret;
 *       int result_count = 0;
 *       sensor_data_t output;
 *       int          results[100];
 *       unsigned int results_timestamp[100]; 
 *       int se;
 *       unsigned int onbed_time; 
 *       ret = motion_st_init();//initialize ST hardware, which power on ST.
 *       if (ret == SD_STATUS_OK) {
 *         // infinite-loop to set data   
 *         while (input != NULL) {
 *           motion_st( input->data->accelerometer_t.x,
 *                      input->data->accelerometer_t.y,
 *                      input->data->accelerometer_t.z, 
 *                      input->data->time_stamp); 
 *           result_count = check_sleep_results();
 *           if(result_count > 0) {
 *             // loop to get sleep events
 *             for (j=0;j<result_count;j++) {            
 *               ret = get_one_sleep_result(&sleep_evt[j], &sleep_evt_time[j]);           
 *             }
 *             // if complete frame, then cache the sleep efficiency
 *             if(ret == C_STATE) {
 *               se         = get_se();
 *               onbed_time = get_bedtime();
 *             }
 *             rearrange_queue_for_sleep();
 *           }
 *         }
 *       }        
 *    @endcode
 *
 * @section Function_Group_Chapter ST function groups description
 *
 * For easily using, we group the APIs by its functionality.
 * You can refer to the details of each function by clicking the function name.
 * - ST hardware initialization \n 
 *   #motion_st_init function mainly initialize parameters on ST.
 *   Before running ST, you should call this firstly. 
 * 
 * - Run ST \n 
 *   #motion_st function will run algorithm by assigning a set of motion value and corresponding timestamp;
 *
 * - Check results \n 
 *   #check_sleep_results can return the available number of data which can be accessed. \n
 *   System software should get data as soon as possible OR the results will be overwritten.
 * 
 * - Get results \n 
 *   System software should allocate apporiate results buffer and results_timestamp buffer before calling #get_one_sleep_result, #get_se, #get_bedtime, and #rearrange_queue_for_sleep. 
 *   #check_sleep_results can return the available number of data which can be accessed. \n
 */
//---------------------------------------------------------
// sleep state definition
//---------------------------------------------------------
/** @brief  This macro defines numeric value for sleep state. \n AWAKE: Active or not sleep
  */
#define AWAKE                      0 

/** @brief  This macro defines numeric value for sleep state. \n RESTLESS: Significant movement or turn over during sleep, but not AWAKE
  */
#define RESTLESS                  -2

/** @brief  This macro defines numeric value for sleep state.\n ASLEEP: Inactive state
  */
#define ASLEEP                    -3
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
// state for frame control
//---------------------------------------------------------
/** @brief  This macro defines numeric value for frame control. IC_STATE: Incomplete sleep duration
  */
#define IC_STATE                  -99 
/** @brief  This macro defines numeric value for frame control. C_STATE: Complete sleep duration
  */
#define C_STATE                    0
//---------------------------------------------------------
// functions for sleep tracker
//---------------------------------------------------------
/**
 * @brief Initialize parameters for sleep tracker.
 * @return    Return #SD_STATUS_OK as success and #SD_STATUS_ERROR for failure indication.
 * Please refer to #motion_st for sample code.
 */
int  motion_st_init(void);

/**
 * @brief This function is main function to detect sleep. Call #motion_st with one set of motion sample.
 * @return    Return #SD_STATUS_OK as success and #SD_STATUS_ERROR for failure indication. 
 * @param[in] ax is the x-axis value from accelerometer (unit: 0.001 m/s^2).
 * @param[in] ay is the y-axis value from accelerometer (unit: 0.001 m/s^2).
 * @param[in] az is the z-axis value from accelerometer (unit: 0.001 m/s^2).
 * @param[in] timestamp is the corresponding time (unit: milli-second) to accelerometer.
 *  - sample code:
 *    @code
 *       int ret;
 *       int result_count = 0;
 *       sensor_data_t output;
 *       int          results[100];
 *       unsigned int results_timestamp[100]; 
 *       int se;
 *       unsigned int onbed_time; 
 *       ret = motion_st_init();//initialize ST hardware, which power on ST.
 *       if (ret == SD_STATUS_OK) {
 *         // infinite-loop to set data   
 *         while (input != NULL) {
 *           motion_st( input->data->accelerometer_t.x,
 *                      input->data->accelerometer_t.y,
 *                      input->data->accelerometer_t.z, 
 *                      input->data->time_stamp); 
 *           result_count = check_sleep_results();
 *           if(result_count > 0) {
 *             // loop to get sleep events
 *             for (j=0;j<result_count;j++) {            
 *               ret = get_one_sleep_result(&sleep_evt[j], &sleep_evt_time[j]);           
 *             }
 *             // if complete frame, then cache the sleep efficiency
 *             if(ret == C_STATE) {
 *               se         = get_se();
 *               onbed_time = get_bedtime();
 *             }
 *             rearrange_queue_for_sleep();
 *           }
 *         }
 *       }        
 *    @endcode
 */  
int  motion_st(int ax, int ay, int az, int timestamp);

/**
 * @brief This function is used to check if available data can be accessed. \n
          Call #check_sleep_results once system get one set of motion samples. \n
 * @return    Return number is the available data. Return 0 if no available data. 
 * Please refer to #motion_st for sample code.
 */    
int  check_sleep_results();

/**
 * @brief This function is used to get one sleep evnet and its corresponding timestamp if available\n
 * @param[in] results is the pointer for event label.
 *            Please call #check_sleep_results to allocate appropriate buffer size before calling #get_one_sleep_result.
 *            Loop N times to get full results and N can be obtained by calling #check_sleep_results. 
 * @param[in] results_timestamp is the pointer for event timestamp.
 * @return    Return #C_STATE as complete sleep duration and #IC_STATE for incomplete sleep duration. 
 *            If #C_STATE is returned, calling #get_se and #get_bedtime to get sleep efficiency (%) and bedtime timestamp, respectively.
 *            If #IC_STATE is returned, it typically means internal buffers are full but the sleep duration is incomplete.
 calling #get_se and #get_bedtime do not provide meaningful infomation. This typically means buffers are out of 
 * Sample code please refer to #motion_st
 */  
int   get_one_sleep_result(int *results, unsigned int *results_timestamp);
/**
 * @brief This function is used to get sleep efficiency(%). \n
 * @return    Return sleep efficiency(%) if the sleep duration is complete.
 * Please refer to #motion_st for sample code.
 */  
int   get_se(void);
/**
 * @brief This function is used to get bedtime timestamp. \n
 * @return    Return bedtime timestamp if the sleep duration is complete.
 * Sample code please refer to #motion_st
 */  
unsigned int get_bedtime(void);
/**
 * @brief This function is used to manage internal buffer in sleep tracker APIs. It must be called after full events are accessed.\n
 * @return    Return #SD_STATUS_OK as success and #SD_STATUS_ERROR for failure indication.
 * Please refer to #motion_st for sample code.
 */  
int  rearrange_queue_for_sleep(void);

#ifdef _cpluscplus
}
#endif

#endif /* __MOTION_ST_H__*/
