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

#include "sensor_alg_interface.h"
#include "stdio.h"
#include "algo_adaptor/algo_adaptor.h"
#include "FreeRTOS.h"
#include "stdint.h"

// Revision History:
// v.1.0.0.1: 2016/03/16 Shu-Yu Hsu

/* syslog */
#define LOGI(fmt,arg...)   LOG_I(sensor, "[BP]: "fmt,##arg)
#define LOGE(fmt,arg...)   LOG_E(sensor, "[BP]: "fmt,##arg)
/* printf*/
//#define LOGE(fmt, args...)    printf("[BP] ERR: "fmt, ##args)
//#define LOGD(fmt, args...)    printf("[BP] DBG: "fmt, ##args)

#define BLOOD_PRESSURE_MONITOR_PPG1_POLLING_TIME 16
#define BLOOD_PRESSURE_MONITOR_ECG_POLLING_TIME 16

static sensor_input_list_t input_blood_pressure_monitor_comp_ppg1;
static sensor_input_list_t input_blood_pressure_monitor_comp_ecg;

static uint32_t fusion_bp_prev_notify_time;

static int32_t get_blood_pressure_monitor_result(sensor_data_t *const output)
{
    if(output == NULL || output->data == NULL){
      return -1;
    }

    sensor_data_unit_t *bp_data = output->data;
    bp_data->sensor_type = SENSOR_TYPE_BLOOD_PRESSURE_MONITOR;
    bp_data->time_stamp = 0;

    // simulated data
    bp_data->blood_pressure_t.dbp = 80;
    bp_data->blood_pressure_t.sbp = 130;
    bp_data->blood_pressure_t.status = SENSOR_STATUS_NO_CONTACT;

    return 1;
}

int32_t accuCount = 0;
#define EXPECT_DATA_COUNT (512*3*45)


static int32_t blood_pressure_monitor_process_data(const sensor_data_t *input_list, void *reserve)
{


    int32_t ret = 1;
    int32_t count = input_list->data_exist_count;
    sensor_data_unit_t *data_start = input_list->data;
    uint32_t input_sensor_type = input_list->data->sensor_type;
    uint32_t input_time_stamp = input_list->data->time_stamp;

    accuCount += count;

    if (accuCount >= EXPECT_DATA_COUNT) {

	sensor_fusion_algorithm_notify(SENSOR_TYPE_BLOOD_PRESSURE_MONITOR);
	accuCount = 0;

    }

    if(input_sensor_type == SENSOR_TYPE_BIOSENSOR_PPG1) {
      //store input data
    	while (count != 0) {
        	if (data_start->time_stamp -fusion_bp_prev_notify_time > 2000UL) {
            		// notify sensor manager
            		sensor_fusion_algorithm_notify(SENSOR_TYPE_BLOOD_PRESSURE_MONITOR);
            		fusion_bp_prev_notify_time = data_start->time_stamp;
        	}

        	data_start++;
        	count--;

		}
    }
    else if(input_sensor_type == SENSOR_TYPE_BIOSENSOR_EKG) {
        while (count != 0) {
            data_start++;
            count--;
        }
    }


    return ret;
}


static int32_t blood_pressure_monitor_operate(int32_t command, void *buffer_out, int32_t size_out, \
                                 void *buffer_in, int32_t size_in)
{
    /* To handle user input of age, sex, height, weight information. */
    return 0;
}

const sensor_descriptor_t blood_pressure_monitor_desp = {
    SENSOR_TYPE_BLOOD_PRESSURE_MONITOR, //output_type
    1, /* version */
    SENSOR_REPORT_MODE_ON_CHANGE, /* report_mode */
    {0, 0}, /*sensor_capability_t */ //[SY]: ???
    &input_blood_pressure_monitor_comp_ppg1, /* sensor_input_list_t */
    blood_pressure_monitor_operate,
    get_blood_pressure_monitor_result,
    blood_pressure_monitor_process_data,
    0 /* accumulate */
};

int blood_pressure_monitor_register(void)
{
    int ret; /*return: fail=-1, pass>=0, which means the count of current register algorithm */

    input_blood_pressure_monitor_comp_ppg1.input_type = SENSOR_TYPE_BIOSENSOR_PPG1_512HZ;
    //input_blood_pressure_monitor_comp_ppg1.input_type = SENSOR_TYPE_BIOSENSOR_PPG1;
    input_blood_pressure_monitor_comp_ppg1.sampling_delay = BLOOD_PRESSURE_MONITOR_PPG1_POLLING_TIME; // ms

    input_blood_pressure_monitor_comp_ecg.input_type = SENSOR_TYPE_BIOSENSOR_EKG;
    input_blood_pressure_monitor_comp_ecg.sampling_delay = BLOOD_PRESSURE_MONITOR_ECG_POLLING_TIME; // ms

    input_blood_pressure_monitor_comp_ppg1.next_input = &input_blood_pressure_monitor_comp_ecg; // build as a signal linked list
    input_blood_pressure_monitor_comp_ecg.next_input = NULL;

    ret = sensor_fusion_algorithm_register_type(&blood_pressure_monitor_desp); //[SY]: ???
    if (ret < 0) {
        LOGE("fail to register blood pressure monitor \r\n");
    }
    ret = sensor_fusion_algorithm_register_data_buffer(SENSOR_TYPE_BLOOD_PRESSURE_MONITOR, 1);
    if (ret < 0) {
        LOGE("fail to register buffer \r\n");
    }
    return ret;
}

int blood_pressure_monitor_init(void)
{
    return 1;
}

