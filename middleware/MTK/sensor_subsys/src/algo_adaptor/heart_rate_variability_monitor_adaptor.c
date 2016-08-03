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

#include <stdio.h>
#include <stdlib.h>
#include "heart_rate_variability.h"
#include "sensor_alg_interface.h"

#define _HEART_RATE_VARIABILITY_ADAPTOR_
#define HEART_RATE_VARIABILITY_ADAPTOR_LUSPER 1

#ifdef _HEART_RATE_VARIABILITY_ADAPTOR_
#define HEART_RATE_MONITOR_EKG_POLLING_TIME 100


#if HEART_RATE_VARIABILITY_ADAPTOR_LUSPER
#include "stdio.h"
#include "algo_adaptor/algo_adaptor.h"
//#include "FreeRTOS.h"
#include "stdint.h"
#endif

#if !HEART_RATE_VARIABILITY_ADAPTOR_LUSPER
#include "sensor_input_ecg_256.h"
#endif


int32_t heart_rate_variability_monitor_init(void);

static sensor_input_list_t input_heart_rate_variability_monitor_comp_bio;
int32_t beatPos[BEAT_POS_BUFFER_SIZE]; // beat position
int32_t numBeats;
int32_t SDNN;
int32_t LF;
int32_t HF;
int32_t LF_HF;
int32_t bufferData[FRAME_SIZE];
int32_t dataIdx;
int32_t bufferDataCount;
int32_t isFinishHRV;

#if HEART_RATE_VARIABILITY_ADAPTOR_LUSPER

/* syslog */
#define LOGI(fmt,arg...)   printf("[HeartRateM]: "fmt,##arg) //[SY]?
#define LOGE(fmt,arg...)   printf("[HeartRateM]: "fmt,##arg) //[SY]?
#endif

static int32_t get_heart_rate_variability_monitor_result(sensor_data_t *const output)
{
  if(output == NULL || output->data == NULL)
  {
    return -1;
  }
  else
  {
    sensor_data_unit_t *hrvm_data = output->data;
    hrvm_data->sensor_type = SENSOR_TYPE_HEART_RATE_VARIABILITY_MONITOR;
    hrvm_data->time_stamp = 0;
    hrvm_data->heart_rate_variability_t.LF = LF;
    hrvm_data->heart_rate_variability_t.HF = HF;
    hrvm_data->heart_rate_variability_t.LF_HF = LF_HF;
    hrvm_data->heart_rate_variability_t.SDNN = SDNN;
    return 1;
  }

}
static int32_t heart_rate_variability_monitor_operate(int32_t command, void *buffer_out, int32_t size_out, \
                                          void *buffer_in, int32_t size_in)
{
  return 0;
}
#endif


static int32_t heart_rate_variability_monitor_process_data(const sensor_data_t *input_list, void *reserve)
{

  int32_t m;
  int32_t x;
  int32_t frameSize = FRAME_SIZE;

  if(input_list->data->sensor_type == SENSOR_TYPE_BIOSENSOR_EKG)
  {
    if (isFinishHRV)
    {
      return 0;
    }
    x = input_list->data->bio_data.data;
    m = x>>22;
    x = (~m & x) | (((x & 0x800000) - x) & m);
    if (bufferDataCount >= 0)
    {
      bufferData[bufferDataCount] = x;
      dataIdx++;
    }
    bufferDataCount++;
    if ( bufferDataCount==frameSize )
    {
      bufferDataCount = 0;
      HRV_preprocessing(bufferData, frameSize, dataIdx, &numBeats, beatPos);
      if (dataIdx > 512*60*5)
      {
        HRV_estimate(&SDNN, &LF, &HF, &LF_HF);
        //notify sensor manager
#if HEART_RATE_VARIABILITY_ADAPTOR_LUSPER
        sensor_fusion_algorithm_notify(SENSOR_TYPE_HEART_RATE_VARIABILITY_MONITOR);
#else
        printf("%d,%d,%d,%d\n",SDNN,LF,HF,LF_HF);
        isFinishHRV = 1;
#endif
//#if DEBUG_MODE
//        cmf_output_waveform(dataIdx);
//        cmf_output_beat_pos(numBeats, beatPos);
//        cmf_output_hrv(SDNN,LF,HF,LF_HF);
//#endif
      }
    }
  }
  else{
    // error handling
  }

  return 1;

}

sensor_descriptor_t heart_rate_variation_monitor_desp  = {
    SENSOR_TYPE_HEART_RATE_VARIABILITY_MONITOR, //output_type
    1, /* version */
    SENSOR_REPORT_MODE_ON_CHANGE, /* report_mode */
    {0, 0}, /*sensor_capability_t */ //[SY]: ???
    &input_heart_rate_variability_monitor_comp_bio, /* sensor_input_list_t */
    heart_rate_variability_monitor_operate,
    get_heart_rate_variability_monitor_result,
    heart_rate_variability_monitor_process_data,
    0 /* accumulate */
};

int32_t heart_rate_variability_monitor_register(void)
{
  int ret; /*return: fail=-1, pass>=0, which means the count of current register algorithm */

  input_heart_rate_variability_monitor_comp_bio.input_type = SENSOR_TYPE_BIOSENSOR_EKG;
  input_heart_rate_variability_monitor_comp_bio.sampling_delay = HEART_RATE_MONITOR_EKG_POLLING_TIME; // ms
  input_heart_rate_variability_monitor_comp_bio.next_input = NULL;
#if HEART_RATE_VARIABILITY_ADAPTOR_LUSPER
  ret = sensor_fusion_algorithm_register_type(&heart_rate_variation_monitor_desp);
  if (ret < 0) {
    LOGE("fail to register heart rate monitor \r\n");
  }
  ret = sensor_fusion_algorithm_register_data_buffer(SENSOR_TYPE_HEART_RATE_VARIABILITY_MONITOR, 1); // [SY]: what's this ??
  if (ret < 0) {
    LOGE("fail to register buffer \r\n");
  }
#endif
  return ret;
}

int32_t heart_rate_variability_monitor_init(void)
{
  dataIdx =0;
  bufferDataCount=-100;
  isFinishHRV = 0;

  HRV_initialization(ECG_FS);
  return 1;
}

#if !HEART_RATE_VARIABILITY_ADAPTOR_LUSPER
void main(int32_t argc, char* argv[])
{
  int32_t dataLength = DATA_LENGTH;
  int32_t fs = ECG_FS;
  int32_t i;
  sensor_data_t input_list[1];
  sensor_data_unit_t data;

  heart_rate_variability_monitor_init();
  input_list->data = &data;
  input_list->data->time_stamp = 0;
  for (i=0;i<dataLength;i++)
  {
    input_list->data_exist_count = 1;
    input_list->data->sensor_type = SENSOR_TYPE_BIOSENSOR_EKG;
    input_list->data->time_stamp += 4;
    input_list->data->bio_data.data = sensor_input[i];
    heart_rate_variability_monitor_process_data(input_list, NULL);
  }
}
#endif
