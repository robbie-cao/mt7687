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
#include "string.h"
#include "motion_st.h"
#include "algo_adaptor/algo_adaptor.h"
#include "motion_fet.h"
#include "hr_fet.h"

#define LOGE(fmt, args...)    printf("[Sleep] ERR: "fmt, ##args)
#define LOGD(fmt, args...)    printf("[Sleep] DBG: "fmt, ##args)

#ifdef SLEEP_RPOFILING
#define NANO_MEASUREMENT
#define MINI_MEASUREMENT
#endif

static sensor_input_list_t input_comp_acc;
static sensor_input_list_t input_comp_bio;
static sensor_input_list_t input_comp_bisi;

uint32_t ppg1_idx;
uint32_t ppg1_all_idx;

int32_t get_sleep_data(sensor_data_t * const output)
{
  int ret = IC_STATE;
  int j, result_count, motion_fet_count, hr_fet_count;

  if(output == NULL || output->data == NULL){
    return -1;
  }

  sensor_data_unit_t * pdata;
  pdata              = output->data; // cast to sleep unit structure

  result_count       = check_sleep_results();
  motion_fet_count   = check_sleep_motion_fet();
  hr_fet_count       = check_sleep_hr_fet();

  output->data_exist_count = result_count+motion_fet_count+hr_fet_count;

  int state;
  unsigned int timestamp;

  /* get sleep results first */
  for (j=0;j<result_count;j++) {
    ret = get_one_sleep_result(&state, &timestamp);
    pdata->sensor_type = SENSOR_TYPE_SLEEP;
    pdata->sleep_data_t.state = state;
    pdata->time_stamp = timestamp;
    pdata++;
  }

  /* if complete frame, then cache the sleep efficiency */
  if(ret == C_STATE) {
    pdata->sensor_type = SENSOR_TYPE_SLEEP;
    pdata->sleep_data_t.state = get_se();
    pdata->time_stamp         = get_bedtime();
    pdata++;
    output->data_exist_count++;
  }

  if(result_count>0) rearrange_queue_for_sleep();

  /* get motion features */
  for (j=0;j<motion_fet_count;j++) {
    ret = get_one_motion_fet(&state, &timestamp);
    pdata->sensor_type = SENSOR_TYPE_SLEEP;
    pdata->sleep_data_t.state = state;
    pdata->time_stamp = timestamp;
    pdata++;
}
  if(motion_fet_count>0) rearrange_queue_for_motion_fet();

  /* get hr features */
  for (j=0;j<hr_fet_count;j++) {
    ret = get_one_hr_fet(&state, &timestamp);
    pdata->sensor_type = SENSOR_TYPE_SLEEP;
    pdata->sleep_data_t.state = state;
    pdata->time_stamp = timestamp;
    pdata++;
  }
  if(hr_fet_count>0) rearrange_queue_for_hr_fet();

  return ret;
}

static int32_t set_sleep_data(const sensor_data_t *input_list, void *reserve)
{
    //store input data
    int count                      = input_list->data_exist_count;
    sensor_data_unit_t *data_start = input_list->data;
    int result_count, fet_count;

    if(input_list->data->sensor_type == SENSOR_TYPE_ACCELEROMETER){
    while (count != 0) {
        /* kernel of motion-based sleep tracker */
      motion_st((data_start)->accelerometer_t.x,
                (data_start)->accelerometer_t.y,
                (data_start)->accelerometer_t.z,
                (data_start)->time_stamp);
      data_start++;
      count--;
    }

      //get results
    result_count = check_sleep_results();
    if(result_count > 0) {
      /* Notify sensor manager to get result */
      sensor_fusion_algorithm_notify(SENSOR_TYPE_SLEEP);
    }

      //get features
      fet_count = check_sleep_motion_fet();
      if(fet_count > 0) {
        /* Notify sensor manager to get result */
        sensor_fusion_algorithm_notify(SENSOR_TYPE_SLEEP);
      }
    }
    else if(input_list->data->sensor_type == SENSOR_TYPE_BIOSENSOR_PPG1){
      while (count != 0) {

        if((ppg1_all_idx & 0x00000001) == 0){
          //0: real; 1: ambient
          /* kernel of motion-based sleep features extraction */
          hr_fet_ext((data_start)->bio_data.data, ppg1_idx, 0);
        data_start++;
          ppg1_idx++;
        }
        count--;
        ppg1_all_idx++;
      }

    }
    else if(input_list->data->sensor_type == SENSOR_TYPE_BIOSENSOR_BISI) {
      while (count != 0) {
        hr_fet_ext((data_start)->bio_data.data, (data_start)->time_stamp, 1);
        data_start++;
        count--;
      }

      //get HR features
      fet_count = check_sleep_hr_fet();
      if(fet_count > 0) {
        /* Notify sensor manager to get result */
        sensor_fusion_algorithm_notify(SENSOR_TYPE_SLEEP);
      }
    }

    return 1;
}


static int32_t sleep_operate(int32_t command, void* buffer_out, int32_t size_out, void* buffer_in, int32_t size_in)
{
  return 0;
}

const sensor_descriptor_t sleep_desp = {
    SENSOR_TYPE_SLEEP,    /* output_type */
    2,                    /* version */
    SENSOR_REPORT_MODE_ON_CHANGE,            /* report_mode */
    {0,0},                /* for physical sensor to register ability */
    &input_comp_acc,      /* sensor_input_list_t */
    sleep_operate,
    get_sleep_data,
    set_sleep_data,
    0                     /* accumulate */
};


int sleep_register(void)
{
  int ret = 0;
  input_comp_acc.input_type     = SENSOR_TYPE_ACCELEROMETER;
  input_comp_acc.sampling_delay = 100; /* 100 ms, default: 10Hz */
  input_comp_acc.next_input     = &input_comp_bio;

  input_comp_bio.input_type     = SENSOR_TYPE_BIOSENSOR_PPG1;
  input_comp_bio.sampling_delay = 8; /* 8 ms, default: 125Hz */
  input_comp_bio.next_input     = &input_comp_bisi;

  input_comp_bisi.input_type     = SENSOR_TYPE_BIOSENSOR_BISI;
  input_comp_bisi.sampling_delay = 100; /* 100 ms, default: 10Hz */
  input_comp_bisi.next_input     = NULL;

  ret = sensor_fusion_algorithm_register_type(&sleep_desp);
  if(ret < 0) printf("%s():fail to register sleep\r\n", __func__);

  ret = sensor_fusion_algorithm_register_data_buffer(SENSOR_TYPE_SLEEP, 100);
  if(ret<0) LOGE("fail to register value \r\n");


  return ret;
}

int sleep_init(void)
{
  int ret = 0;
  ppg1_all_idx = 0;
  ppg1_idx = 0;

  #if defined(NANO_MEASUREMENT) || defined(MINI_MEASUREMENT)
  timestamp_init();
  #endif

  //motion_st_init(&sleep_para);
  motion_st_init();
  hr_fet_init();
  return ret;
}
