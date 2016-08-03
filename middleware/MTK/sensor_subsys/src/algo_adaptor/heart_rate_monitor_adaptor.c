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
 /**
 * @file    heart_rate_monitor_adapter.c
 * @brief   OHRM adapter
 * @author  Mediatek
 * @version 1.0.0.1
 * @date    2016.06.21
 */
#include "sensor_alg_interface.h"
#include "stdio.h"
#include "ohrm.h"
#include "ppg_control.h"
#include "algo_adaptor/algo_adaptor.h"
#include "FreeRTOS.h"
#include "vsm_driver.h"
#include "stdint.h"

// Function configuration
#if defined(MTK_SENSOR_BIO_USE_MT2511)
#define OHRM_ON            // HR algorithm enable
#define OHRM_DEBUG_ADAPTER // check system correctness
#define OHRM_PPG_CTRL_ON   // PPG control enable
#endif //#if defined(MTK_SENSOR_BIO_USE_MT2511)

#if defined(OHRM_ON)
// Input signal information definition
#define PPG_BITWIDTH 23
#define ACC_BITWIDTH 32 //signed from BMA255 driver
#define FS_PPG_HZ 125
#define FS_ACC_HZ 125
#define REAL_PPG_SAMPLE_INTERVAL_NANOSEC 8007050  //round(10^9 / 124.8899475940924) ns
//#define REAL_PPG_SAMPLE_INTERVAL_NANOSEC 7812500  //ns: 10^9 / 128
//#define REAL_PPG_SAMPLE_INTERVAL_NANOSEC 8000000  //ns: 10^9 / 125

// buffer control
#define ACC_BUF_SIZE 10
#define PPG_BUF_SIZE 10
#define OHRM_OUT_LENGTH 2
#define OHRM_PPG_CTRL_OUT_LENGTH 2

// OHRM config
uint32_t ohrm_adapter_timer;
uint32_t ohrm_adapter_ts;
int32_t ohrm_adapt_pre_ts_ppg;
int32_t ohrm_adapt_pre_ts_motion;

int32_t ohrm_adap_init_motion;

int32_t ohrm_adapt_buf_ppg[PPG_BUF_SIZE];
int32_t ohrm_adapt_buf_motion[ACC_BUF_SIZE];
int32_t ohrm_adapt_buf_ts_ppg[PPG_BUF_SIZE];
int32_t ohrm_adapt_buf_ts_motion[ACC_BUF_SIZE];
int32_t ohrm_adapt_idx_ppg;
int32_t ohrm_adapt_idx_motion;

// Calc. FS_MOTION/FS_PPG (T_PPG/T_MOTION)
#define MOTION_PPG_RATIO_DECIMAL_POINT_BITS 12
uint32_t ohrm_fs_acc_div_ppg; // fs_acc / fs_ppg
uint32_t motion_ppg_freq_ratio = 1 << MOTION_PPG_RATIO_DECIMAL_POINT_BITS;  // 12-bit decimal representation ==> 0x1000 means 1.0
uint32_t start_motion_tick; //ms
uint64_t motion_total_sample_interval; //ns
uint64_t motion_accumulated_count;

void ms_fps_statistic_reset(int32_t first_motion_timestamp, int32_t sample_amount); // calls when ms_init
void ms_fps_statistic_update(uint32_t count, uint32_t curr_timestamp);
uint32_t update_motion_ppg_sample_time_ratio(void);
// End --Calc. FS_MOTION/FS_PPG --

// debug info
#if defined(OHRM_DEBUG_ADAPTER)
#define OHRM_DBG_ERR_TS_MAX 5
#define OHRM_DBG_NEG_TS_MAX 1
uint32_t dbg_ohrm_switch = 0;
uint32_t dbg_cnt_err_ts_ppg = 0;
uint32_t dbg_cnt_neg_ts_ppg = 0;
uint32_t dbg_cnt_err_ts_acc = 0;
uint32_t dbg_cnt_neg_ts_acc = 0;
uint32_t dbg_cnt_ppg_ip = 0;
uint32_t dbg_cnt_acc_ip = 0;
uint32_t dbg_cnt_5sec = 0;
#endif //#if defined(OHRM_DEBUG_ADAPTER)

#endif //#if defined(OHRM_ON)

/* syslog */
#define LOGI(fmt,arg...)   LOG_I(sensor, "[HeartRateM]: "fmt,##arg)
#define LOGE(fmt,arg...)   LOG_E(sensor, "[HeartRateM]: "fmt,##arg)
/* printf*/
//#define LOGE(fmt, args...)    printf("[HeartRateM] ERR: "fmt, ##args)
//#define LOGD(fmt, args...)    printf("[HeartRateM] DBG: "fmt, ##args)

#define HEART_RATE_MONITOR_ACC_DELAY 8
#define HEART_RATE_MONITOR_PPG1_DELAY 8
#define HEART_RATE_MONITOR_PPG1_POLLING_TIME 64 // align ACC irq: 8 samples

static sensor_input_list_t input_heart_rate_monitor_comp_acc;
static sensor_input_list_t input_heart_rate_monitor_comp_bio;

static int32_t get_heart_rate_monitor_result(sensor_data_t *const output)
{
    if(output == NULL || output->data == NULL){
      return -1;
    }
    sensor_data_unit_t *hrm_data = output->data;
    hrm_data->sensor_type = SENSOR_TYPE_HEART_RATE_MONITOR;

    #if defined(OHRM_ON)
    hrm_data->time_stamp = ohrm_adapter_ts;
    hrm_data->heart_rate_t.bpm = ohrm_get_bpm()>>16;

    switch(dbg_ohrm_switch){
    case 1: // PPG1 config status
        // 2'b01 + 3'b ambdac_ppg + 3'b ambdac_amb + 2'b0 + 3'b tia2 + 3'b tia1 + 8'b led  + 8'b confidence
        hrm_data->heart_rate_t.status = 0x40000000;
        hrm_data->heart_rate_t.status += ohrm_get_confidence_level() & 0x000000FF;

        bus_data_t ppg_reg_info;
        uint32_t ohrm_reg_tx;
        uint32_t ohrm_reg_rx;
        ppg_reg_info.reg  = (0x332C & 0x00FF);
        ppg_reg_info.addr = (0x332C & 0xFF00) >> 8;
        ppg_reg_info.data_buf = (uint8_t *)&ohrm_reg_tx;
        ppg_reg_info.length = sizeof(ohrm_reg_tx);
        vsm_driver_read_register(&ppg_reg_info);

        ppg_reg_info.reg  = (0x3318 & 0x00FF);
        ppg_reg_info.addr = (0x3318 & 0xFF00) >> 8;
        ppg_reg_info.data_buf = (uint8_t *)&ohrm_reg_rx;
        ppg_reg_info.length = sizeof(ohrm_reg_rx);
        vsm_driver_read_register(&ppg_reg_info);

        hrm_data->heart_rate_t.status += (ohrm_reg_tx & 0x000000FF)<<8;//LED1
        hrm_data->heart_rate_t.status += (ohrm_reg_rx & 0x00000007)<<16;//tia1
        hrm_data->heart_rate_t.status += ((ohrm_reg_rx & 0x00000038)>>3)<<19;//tia2
        hrm_data->heart_rate_t.status += ((ohrm_reg_rx & 0x0E000000)>>25)<<24;//ambdac_amb
        hrm_data->heart_rate_t.status += ((ohrm_reg_rx & 0x01C00000)>>22)<<27;//ambdac_ppg
        break;
    case 2: //SW status from adapter
        // 2'b10 + 6'b SW flag + 3'b0 + 13'b fs_ratio + 8'b confidence
        hrm_data->heart_rate_t.status = 0x80000000;
        hrm_data->heart_rate_t.status += ohrm_get_confidence_level() & 0x000000FF;
        hrm_data->heart_rate_t.status += (ohrm_fs_acc_div_ppg & 0x00001FFF) << 8;
        #if defined(OHRM_DEBUG_ADAPTER)
        hrm_data->heart_rate_t.status += (((dbg_cnt_ppg_ip-(dbg_cnt_ppg_ip>>2))<dbg_cnt_acc_ip)?1:0)<<24;
        hrm_data->heart_rate_t.status += ((dbg_cnt_ppg_ip>((dbg_cnt_acc_ip<<1)+dbg_cnt_acc_ip))?1:0)<<25;
        hrm_data->heart_rate_t.status += ((dbg_cnt_neg_ts_acc>OHRM_DBG_NEG_TS_MAX)?1:0)<<26;
        hrm_data->heart_rate_t.status += ((dbg_cnt_neg_ts_ppg>OHRM_DBG_NEG_TS_MAX)?1:0)<<27;
        hrm_data->heart_rate_t.status += ((dbg_cnt_err_ts_acc>OHRM_DBG_ERR_TS_MAX)?1:0)<<28;
        hrm_data->heart_rate_t.status += ((dbg_cnt_err_ts_ppg>OHRM_DBG_ERR_TS_MAX)?1:0)<<29;
        break;
        #endif //#if defined(OHRM_DEBUG_ADAPTER)
    case 3: // called at initial stage
        //2'b11 + 6'b0 + 8b ohrm ver + 8b ppg control ver + 8'b
        hrm_data->heart_rate_t.status = 0xC0000000;
        hrm_data->heart_rate_t.status += 0;
        hrm_data->heart_rate_t.status += (ohrm_get_version() & 0x000000FF)<<16;
        hrm_data->heart_rate_t.status += (ppg_control_get_version() & 0x000000FF)<<8;
        break;
    default: //ohrm lib status
        hrm_data->heart_rate_t.status = ohrm_get_confidence_level();
    }
    #else
    hrm_data->time_stamp = 0;
    hrm_data->heart_rate_t.bpm = 80;
    #endif //#if defined(OHRM_ON)

    return 1;
}

static int32_t heart_rate_monitor_process_data(const sensor_data_t *input_list, void *reserve)
{
    #if defined(OHRM_ON)
    int32_t var;
    int32_t count = input_list->data_exist_count;
    sensor_data_unit_t *data_start = input_list->data;
    uint32_t input_sensor_type = input_list->data->sensor_type;
    uint32_t input_time_stamp = input_list->data->time_stamp;

    //OHRM lib
    ohrm_data_t param_ppg1;
    ohrm_data_t param_motion;
    int32_t ohrm_out[OHRM_OUT_LENGTH];

    //ppg control
    #if defined(OHRM_PPG_CTRL_ON)
    ppg_control_t ppg1_ctrl_input;
    int32_t ppg_control_output[OHRM_PPG_CTRL_OUT_LENGTH];
    #endif //#if defined(OHRM_PPG_CTRL_ON)

    if(input_sensor_type == SENSOR_TYPE_ACCELEROMETER){
        //check init
        if(ohrm_adap_init_motion==0){
            ohrm_adap_init_motion = 1;
            ms_fps_statistic_reset(input_time_stamp, 0);
            ohrm_adapter_ts = input_time_stamp;

            dbg_ohrm_switch = 3;
            sensor_fusion_algorithm_notify(SENSOR_TYPE_HEART_RATE_MONITOR);
        }

        //store input data
        while (count != 0) {
            input_time_stamp = input_list->data->time_stamp;
            // combine motion signal, be aware of overflow for bitwudth != 32
            var = (data_start)->accelerometer_t.x + (data_start)->accelerometer_t.y + (data_start)->accelerometer_t.z;
            //ts_input_motion_buf[ohrm_adapt_idx_motion] = (data_start)->time_stamp;
            ohrm_adapt_buf_motion[ohrm_adapt_idx_motion] = var;
            ohrm_adapt_idx_motion++;

            //check time stamp
            #if defined(OHRM_DEBUG_ADAPTER)
            dbg_cnt_acc_ip++;
            if((data_start)->time_stamp <ohrm_adapt_pre_ts_motion){
                dbg_cnt_neg_ts_acc++;
            }else if(((data_start)->time_stamp - ohrm_adapt_pre_ts_motion) > (HEART_RATE_MONITOR_ACC_DELAY<<1)){
                dbg_cnt_err_ts_acc++;
            }
            ohrm_adapt_pre_ts_motion = input_time_stamp;
            #endif

            // Call OHRM: acc (motion) part
            if(ohrm_adapt_idx_motion >= (ACC_BUF_SIZE)){
                param_motion.data = ohrm_adapt_buf_motion;
                param_motion.data_fs = FS_ACC_HZ;
                param_motion.data_length = ohrm_adapt_idx_motion;
                param_motion.data_bit_width = ACC_BITWIDTH;
                param_motion.data_config = 0;
                param_motion.data_source = 0; // motion
                ohrm_process(&param_motion, ohrm_fs_acc_div_ppg, ohrm_out);

                if(((data_start)->time_stamp) >0){
                    ms_fps_statistic_update(ohrm_adapt_idx_motion, (data_start)->time_stamp);
                }
                ohrm_adapt_idx_motion -= ACC_BUF_SIZE;
            }
            data_start++;
            count--;
        }
    }else if(input_sensor_type == SENSOR_TYPE_BIOSENSOR_PPG1){
        //store input data
        while (count != 0) {
            //ts_input_ppg_buf[ohrm_adapt_idx_ppg] = (data_start)->time_stamp;
            ohrm_adapt_buf_ppg[ohrm_adapt_idx_ppg] = (data_start)->bio_data.data;

            #if defined(OHRM_DEBUG_ADAPTER)
            if(dbg_cnt_ppg_ip > (1<<30)){
                dbg_cnt_ppg_ip = 0;
                dbg_cnt_acc_ip = 0;
            }
            dbg_cnt_ppg_ip++;
            if((ohrm_adapt_idx_ppg & 0x00000001) == 0){
                if((data_start)->time_stamp < ohrm_adapt_pre_ts_ppg){
                    dbg_cnt_neg_ts_ppg++;
                }else if(((data_start)->time_stamp - ohrm_adapt_pre_ts_ppg) >(HEART_RATE_MONITOR_PPG1_DELAY<<1)){
                    dbg_cnt_err_ts_ppg++;
                }
                ohrm_adapt_pre_ts_ppg = (data_start)->time_stamp;
            }
            #endif

            ohrm_adapt_idx_ppg++;
            ohrm_adapter_timer++;

            if(ohrm_adapt_idx_ppg >= (PPG_BUF_SIZE)){

                // Call PPG Control
                #if defined(OHRM_PPG_CTRL_ON)
                ppg1_ctrl_input.input = ohrm_adapt_buf_ppg;
                ppg1_ctrl_input.input_fs = FS_PPG_HZ;
                ppg1_ctrl_input.input_length = ohrm_adapt_idx_ppg;
                ppg1_ctrl_input.input_bit_width = PPG_BITWIDTH;
                ppg1_ctrl_input.input_config = 1;
                ppg1_ctrl_input.input_source = 1;
                //ppg_adjust_flag = ppg_control_process(&ppg1_ctrl_input, PPG_CONTROL_MODE_DUAL1, ppg_control_output);
                ppg_control_process(&ppg1_ctrl_input, PPG_CONTROL_MODE_DUAL1, ppg_control_output);
                #endif //#if defined(OHRM_PPG_CTRL_ON)

                // Call OHRM: PPG part
                ohrm_fs_acc_div_ppg = update_motion_ppg_sample_time_ratio();
                param_ppg1.data = ohrm_adapt_buf_ppg;
                param_ppg1.data_fs = FS_PPG_HZ;
                param_ppg1.data_length = ohrm_adapt_idx_ppg;
                param_ppg1.data_bit_width = PPG_BITWIDTH;
                param_ppg1.data_config = 1;
                param_ppg1.data_source = 1; // ppg
                ohrm_process(&param_ppg1, ohrm_fs_acc_div_ppg, ohrm_out);
                ohrm_adapt_idx_ppg -= PPG_BUF_SIZE;
            }

            if(ohrm_adapter_timer >= (FS_PPG_HZ<<1)){
                ohrm_adapter_ts = (data_start)->time_stamp;
                ohrm_adapter_timer -= (FS_PPG_HZ<<1);
                dbg_cnt_5sec++;
                // notify sensor manager
                dbg_ohrm_switch = 0;
                sensor_fusion_algorithm_notify(SENSOR_TYPE_HEART_RATE_MONITOR);
                dbg_ohrm_switch = 1;
                sensor_fusion_algorithm_notify(SENSOR_TYPE_HEART_RATE_MONITOR);
            }

            if(dbg_cnt_5sec==5){
                dbg_cnt_5sec = 0;
                dbg_ohrm_switch = 2;
                sensor_fusion_algorithm_notify(SENSOR_TYPE_HEART_RATE_MONITOR);
            }

            data_start++;
            count--;
        }
    }
    else{
    // error handling
    }
    #endif //#if defined(OHRM_ON)

    return 1;
}

static int32_t heart_rate_monitor_operate(int32_t command, void *buffer_out, int32_t size_out, \
                                 void *buffer_in, int32_t size_in)
{
    return 0;
}

const sensor_descriptor_t heart_rate_monitor_desp = {
    SENSOR_TYPE_HEART_RATE_MONITOR, //output_type
    1, /* version */
    SENSOR_REPORT_MODE_ON_CHANGE, /* report_mode */
    {0, 0}, /*sensor_capability_t */ //[SY]: ???
    &input_heart_rate_monitor_comp_acc, /* sensor_input_list_t */
    heart_rate_monitor_operate,
    get_heart_rate_monitor_result,
    heart_rate_monitor_process_data,
    0 /* accumulate */
};

int heart_rate_monitor_register(void)
{
    int ret; /*return: fail=-1, pass>=0, which means the count of current register algorithm */

    input_heart_rate_monitor_comp_acc.input_type = SENSOR_TYPE_ACCELEROMETER;
    input_heart_rate_monitor_comp_acc.sampling_delay = HEART_RATE_MONITOR_ACC_DELAY; // ms

    input_heart_rate_monitor_comp_bio.input_type = SENSOR_TYPE_BIOSENSOR_PPG1;
    input_heart_rate_monitor_comp_bio.sampling_delay = HEART_RATE_MONITOR_PPG1_POLLING_TIME; // ms

    input_heart_rate_monitor_comp_acc.next_input = &input_heart_rate_monitor_comp_bio; // build as a signal linked list
    input_heart_rate_monitor_comp_bio.next_input = NULL;

    ret = sensor_fusion_algorithm_register_type(&heart_rate_monitor_desp);
    if (ret < 0) {
        LOGE("fail to register heart rate monitor \r\n");
    }
    ret = sensor_fusion_algorithm_register_data_buffer(SENSOR_TYPE_HEART_RATE_MONITOR, 1);
    if (ret < 0) {
        LOGE("fail to register buffer \r\n");
    }
    return ret;
}

int heart_rate_monitor_init(void)
{
    // Call PPG control lib init
    #if defined(OHRM_PPG_CTRL_ON)
        ppg_control_init();
    #endif //#if defined(OHRM_PPG_CTRL_ON)

    // Call HR lib init
    #if defined(OHRM_ON)
    ohrm_init(9800); // 1g = 9800 code

    ohrm_adapter_timer = 0;
    ohrm_adapter_ts = 0;

    ohrm_adap_init_motion = 0;
    ohrm_adapt_idx_ppg = 0;
    ohrm_adapt_idx_motion = 0;

    // 0x1000 means 1X (motion fs = PPG fs)
    ohrm_fs_acc_div_ppg   = 1 << MOTION_PPG_RATIO_DECIMAL_POINT_BITS;
    motion_ppg_freq_ratio = 1 << MOTION_PPG_RATIO_DECIMAL_POINT_BITS;
    #endif //#if defined(OHRM_ON)

    return 1;
}

#if defined(OHRM_ON)
// ACC/PPG fs_ratio calculation
void ms_fps_statistic_reset(int32_t first_motion_timestamp, int32_t sample_amount) // calls when ms_init
{
    start_motion_tick = first_motion_timestamp;
    motion_accumulated_count = sample_amount;
}

void ms_fps_statistic_update(uint32_t count, uint32_t curr_timestamp)
{
    uint64_t count_64bit = count;
    uint32_t curr_tick = curr_timestamp; //get millisecond number from the system power on till now
    motion_accumulated_count += count_64bit;
    motion_total_sample_interval = (((uint64_t)(curr_tick - start_motion_tick))*1000000); //ns
}

uint32_t update_motion_ppg_sample_time_ratio(void)
{
    // Default fps, need to get info from sensor driver if changed
    uint64_t real_ppg_sample_time = REAL_PPG_SAMPLE_INTERVAL_NANOSEC;
    motion_ppg_freq_ratio = (uint32_t)( (real_ppg_sample_time<<MOTION_PPG_RATIO_DECIMAL_POINT_BITS) * motion_accumulated_count / motion_total_sample_interval);

    // Set boundary
    if(motion_ppg_freq_ratio>4915){
        motion_ppg_freq_ratio = 4915;
    }else if(motion_ppg_freq_ratio<3600){
        motion_ppg_freq_ratio = 3600;
    }
    return motion_ppg_freq_ratio;
}
#endif //#if defined(OHRM_ON)

