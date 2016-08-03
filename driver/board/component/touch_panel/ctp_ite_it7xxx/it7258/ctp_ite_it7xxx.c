/*
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

#ifdef MTK_CTP_ENABLE
#include <string.h>
#include "ctp.h"
#include "ctp_ite_it7xxx.h"
#include "ctp_ite_it7xxx_download.h"
#include "hal_gpt.h"
#include "touch_panel_custom_ite.h"
#include "hal_log.h"
#include "hal_nvic_internal.h"
#include <assert.h>
#include "FreeRTOS.h"
#include "task.h"

#define MAX_BUFFER_SIZE 10

static bool ctp_ite_7258_get_data(bsp_ctp_multiple_event_t *tpes);

ctp_parameters_struct_t CTP_parameters;

uint32_t CTP_DELAY_TIME = 3;

ctp_custom_information_struct  ctp_ite_7258_custom_information_def = {
    "ITE     ",
    "7250AFN ",
    "UNKNOWN ",
};

static bool ctp_ite_read_query_buffer(uint8_t *rdata)
{
    return CTP_I2C_READ(QUERY_BUFFER_INDEX, rdata, 1);
}

static bool ctp_ite_read_command_response_buffer(uint8_t *rdata, uint32_t data_length)
{
    return CTP_I2C_READ(COMMAND_RESPONSE_BUFFER_INDEX, rdata, data_length);
}

static bool ctp_ite_read_point_buffer(uint8_t *rdata, uint32_t data_length)
{
    return CTP_I2C_READ(POINT_BUFFER_INDEX, rdata, 14);
}

static bool ctp_ite_write_command_buffer(uint8_t *wdata, uint32_t data_length)
{
    return CTP_I2C_send(COMMAND_BUFFER_INDEX, wdata, data_length);
}

void ctp_ite_7258_power(bool ON)
{
    uint8_t set_sleep[3] = {COMMAND_SET_POWER_MODE, 0x0, COMMAND_POWER_SLEEP_MODE};
    uint8_t query;

    log_hal_info("Enter ctp_ite_7258_power");

    if (ON == true) {
        ctp_ite_read_query_buffer(&query);
        log_hal_info("ctp_ite_7258_power : true");

    } else {
        ctp_ite_write_command_buffer(set_sleep, 3);
        log_hal_info("ctp_ite_7258_power : false");
    }
}

static bool waitCommandDone(void)
{
    uint8_t ucQuery = 0xFF;
    uint32_t count = 0;

    do {
        if (!CTP_I2C_READ(QUERY_BUFFER_INDEX, &ucQuery, 1)) {
            ucQuery = 0x01;
        }
        count++;
    } while ((ucQuery & 0x01) && (count < 500));
    log_hal_info("waitCommandDone: ucQuery=%x, count=%d\r\n", ucQuery, count);
    if (ucQuery == 0) {
        return  true;
    } else {
        return  false;
    }
}

bool ctp_ite_7258_upgrade(void)
{
    if (fnFirmwareDownload() == true) {
        log_hal_info("fnFirmwareDownload Success!!!!!\r\n");
        return true;
    } else {
        log_hal_info("fnFirmwareDownload Fail!!!!!\r\n");
        return false;
    }
}

//If controller has the register store these informations
//Read out the informations from controller through I2C
void ctp_ite_7258_get_information(void)
{
    uint8_t value[32], respond[8];
    uint8_t read_fw_cmd[] = {COMMAND_BUFFER_INDEX, COMMAND_READ_REGISTER, 0x08, COMMAND_DATA_TYPE_BYTE, 0x08, 0x00, 0x00, 0xd0};

    log_hal_info("Enter ctp_ite_7258_get_information\r\n");

    CTP_I2C_MULTI_WRITE(read_fw_cmd, 8);

    ctp_ite_read_command_response_buffer(&respond[0], 8);

    log_hal_info("[CTP]product ID:%s\r\n", &respond[0]);

    CTP_I2C_READ(POINT_BUFFER_INDEX, &value[0], 8);
    CTP_I2C_READ(POINT_BUFFER_INDEX + 8, &value[8], 8);
}


bool ctp_ite_get_firmware_information(ctp_custom_information_struct *str)
{
    uint8_t buffer[SIGNATURE_LENGTH];
    uint32_t i;

    waitCommandDone();
    if (!fnAdvanceReadFlash(SIGNATURE_LENGTH, 0x0000, buffer)) {
        return false;
    }

    //Get vendor
    str->vendor[0] = buffer[0];
    str->vendor[1] = buffer[1];

    //Get product
    for (i = 0; i < 6; i++) {
        str->product[i] = buffer[i + 2];
    }

    //Get firmware version
    for (i = 0; i < 4; i++) {
        str->firmware_version[i] = buffer[8 + i];
    }

    if ((buffer[0] != 'I') || \
            (buffer[1] != 'T') || \
            (buffer[2] != '7') || \
            (buffer[3] != '2') || \
            (buffer[4] != '6') || \
            (buffer[5] != '0') || \
            (buffer[6] != 'F') || \
            (buffer[7] != 'W')) {
        return false;
    }

    log_hal_info("[ITE]firmware info:%c%c%c%c%c%c%c%c  version:%x%x%x%x size:%x%x checksum:%x%x\r\n", \
                 buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7], \
                 buffer[8], buffer[9], buffer[10], buffer[11], \
                 buffer[12], buffer[13], \
                 buffer[14], buffer[15]);

    return true;
}


ctp_pen_state_enum_t ctp_ite_7258_hisr(void)
{
    uint8_t query, count = 0;
    uint8_t point[14];

    log_hal_info("Enter ctp_ite_7258_hisr");

    do {
        CTP_I2C_READ(QUERY_BUFFER_INDEX, &query, 1);
        count++;
        if (count >= 0xF) {
            return CTP_UP;
        }
    } while (query & 0x1); //busy

    CTP_I2C_READ(POINT_BUFFER_INDEX, point, 14);
    if ((query & 0x88) && (point[0] & POINT_BUFFER_POINT_INFO_MASK)) {
        return CTP_DOWN;
    }
    return CTP_UP;
}

bool ctp_ite_7258_device_mode(ctp_device_mode_enum_t set_device_mode)
{
    log_hal_info("Enter ctp_ite_7258_device_mode");
    return true;
}

bool ctp_ite_7258_parameters(ctp_parameters_struct_t *para, uint32_t get_para, uint32_t set_para)
{
    bool result = true;

    log_hal_info("Enter ctp_ite_7258_parameters");

    if (get_para & set_para) { //get and set the same parameter in the same time.
        assert(0);
    }

    result &= false;

    /*
    if(set_para & CTP_PARA_RESOLUTION)
    {
        result &= false; //not implement
    }
    if(set_para & CTP_PARA_THRESHOLD)
    {
        result &= false; //not implement
    }
    if(set_para & CTP_PARA_REPORT_INTVAL)
    {
        result &= false; //not implement
    }
    if(set_para & CTP_PARA_IDLE_INTVAL)
    {
        result &= false; //not implement
    }
    if(set_para & CTP_PARA_SLEEP_INTVAL)
    {
        result &= false; //not implement
    }
    if(get_para & CTP_PARA_RESOLUTION)
    {
        result &= false; //not implement
    }
    if(get_para & CTP_PARA_THRESHOLD)
    {
        result &= false; //not implement
    }
    if(get_para & CTP_PARA_REPORT_INTVAL)//write only, not support read
    {
        result = true;
    }
    if(get_para & CTP_PARA_IDLE_INTVAL)
    {
        result &= false; //not implement
    }
    if(get_para & CTP_PARA_SLEEP_INTVAL)
    {
        result &= false; //not implement
    }
    */

    return result;
}

bool ctp_ite_7258_check_busy(uint8_t *query_info)
{
    uint32_t count = 0;

    do {
        CTP_I2C_READ(QUERY_BUFFER_INDEX, query_info, 1);
        count++;
        if (count >= 0xF) {
            return true;
        }
    } while (*query_info & 0x1); //busy

    return false;
}

bool ctp_ite_7258_data_valid(void)
{
    uint8_t lvalue;
    bool    ret;

    ret = CTP_I2C_READ(QUERY_BUFFER_INDEX, &lvalue, 1);
    log_hal_info("QUERY_BUFFER_INDEX = %x\r\n", lvalue);
    if ((ret == true) && (lvalue & 0x80)) {
    	return true;
    }
    else {
    	return false;
    }

}

bool ctp_ite_7258_get_data(bsp_ctp_multiple_event_t *tpes)
{
    uint8_t     query_info[1];
    uint8_t     point_info[14];
    uint32_t    model = 0;
    uint32_t    temp_data;
    bool        busy;

    //log_hal_info("Enter ctp_ite_7258_get_data");

    assert(tpes);

    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &temp_data);
    tpes->time_stamp =  temp_data;

    tpes->padding = CTP_PATTERN;
    tpes->model = 0;

    tpes->points[0].x = 0;
    tpes->points[0].y = 0;
    tpes->points[1].x = 0;
    tpes->points[1].y = 0;

    busy = ctp_ite_7258_check_busy(query_info);
    if (busy) {
        log_hal_info("ctp_ite_7258_check_busy = TRUE");
        //log_hal_info("===Return FALSE===\r\n");
        return false;
    }

    ctp_ite_read_point_buffer(point_info, 14);

    //log_hal_info("ctp_ite_7258_get_data point_info[0]=%x %x %x %x %x %x %x %x %x %x %x %x %x %x",  point_info[0], point_info[1], point_info[2], point_info[3], point_info[4], point_info[5], point_info[6], point_info[7], point_info[8], point_info[9], point_info[10], point_info[11], point_info[12], point_info[13] );

    tpes->points[4].x = (point_info[1] & 0x01);//return false;
    //log_hal_info("ctp_ite_7258_get_data P5.Palm = %d ", tpes->points[4].x);

    if (point_info[0] & POINT_BUFFER_POINT_INFO_0) {
        tpes->points[0].x += point_info[2] + ((point_info[3] & 0x0F) << 8);
        tpes->points[0].y += point_info[4] + ((point_info[3] & 0xF0) << 4);
        model |= 0x1;
        //log_hal_info("ctp_ite_7258_get_data P1.x=%d  P1.y=%d  ", tpes->points[0].x, tpes->points[0].y);
    }
    if (point_info[0] & POINT_BUFFER_POINT_INFO_1) {
        tpes->points[1].x += point_info[6] + ((point_info[7] & 0x0F) << 8);
        tpes->points[1].y += point_info[8] + ((point_info[7] & 0xF0) << 4);
        model |= 0x2;
        //log_hal_info("ctp_ite_7258_get_data P2.x=%d  P2.y=%d  ", tpes->points[1].x, tpes->points[1].y);
    }
    //currently ITE not support 3 points

    if (model == 2) { //only point 2 have valid value
        tpes->points[0].x = tpes->points[1].x;
        tpes->points[0].y = tpes->points[1].y;
        tpes->model = 1;
    }

    tpes->model = (uint16_t)((model > 2) ? 2 : model);

    //log_hal_info("ctp_ite_7258_get_data x=%d y=%d",  tpes->points[0].x, tpes->points[0].y);
    //log_hal_info("ctp_ite_7258_get_data tpes->model=%d",  tpes->model);

    if (model > 0) {
        return true;
    } else {
        return false;
    }
}

bool ctp_ite_7258_set_interrupt_trigger(void)
{
    uint8_t data[4];
    uint16_t wCommandResponse;

    waitCommandDone();

    data[0] = 0x02;
    data[1] = 0x04;
    data[2] = 0x01;
    data[3] = 0x00;

    if (ctp_ite_write_command_buffer(data, 4) == false) {
        log_hal_info("[CTP][ITE]Setting interrupt trigger level false\r\n");
    }

    waitCommandDone();

    if (!ctp_ite_read_command_response_buffer((uint8_t*)(&wCommandResponse), 2)) {
        return false;
    }

    if (wCommandResponse != COMMAND_RESPONSE_SUCCESS) {
        return false;
    }

    log_hal_info("QUERY_BUFFER_INDEX = %x\r\n", wCommandResponse);

    return true;

}

bool ctp_ite_7258_init(void)
{
    uint8_t     lvalue;
    bool        ret_bool;
    uint32_t    count[3];
    uint32_t    time_count;
    uint32_t    time_start, time_total[10];
    ctp_custom_information_struct info;

    time_start   = ctp_get_time_us();
    log_hal_info("[CTP][ITE][init]Enter ctp_ite_7258_init\r\n");

    /* CTP reset flow start , need to control pinmux **/
    /***************************  reset flow **************************************/
    log_hal_info("[CTP][ITE][init]start reset sequence\r\n");
    /* set reset pin to gpio mode and output high */
    hal_pinmux_set_function(ctp_config_info.ctp_eint.eint_pin, 0);          //set GPIO mode
    hal_pinmux_set_function(ctp_config_info.reset_pin, 0);

    hal_gpio_set_direction(ctp_config_info.reset_pin, HAL_GPIO_DIRECTION_OUTPUT);
    hal_gpio_set_output(ctp_config_info.reset_pin, HAL_GPIO_DATA_HIGH);

    hal_gpio_set_direction(ctp_config_info.ctp_eint.eint_pin, HAL_GPIO_DIRECTION_OUTPUT);
    hal_gpio_set_output(ctp_config_info.ctp_eint.eint_pin, HAL_GPIO_DATA_LOW);

    ctp_delay_ms(10);

    hal_gpio_set_output(ctp_config_info.reset_pin, HAL_GPIO_DATA_LOW);
    ctp_delay_ms(50);

    hal_gpio_set_output(ctp_config_info.reset_pin, HAL_GPIO_DATA_HIGH);

    /* set enit pin to eint mode */
    hal_pinmux_set_function(ctp_config_info.ctp_eint.eint_pin, 1);          //set eint mode
    log_hal_info("[CTP][ITE][init]end reset sequence\r\n");
    time_total[0] = ctp_get_duration_time_us_to_ms(time_start);
    /***************************  end flow **************************************/

    /**** Get I2C and wait TP ready ***/
    count[0]   = ctp_get_time_us();
    time_count = 0;
    while (1) {
        ctp_delay_ms(10);
        if (ctp_hw_i2c_init(&ctp_config_info.ctp_i2c) == false) {
            log_hal_info("[CTP][ITE][init]ctp_hw_i2c_init, cannot get i2c\r\n");
        } else {
            ret_bool = CTP_I2C_READ(QUERY_BUFFER_INDEX, &lvalue, 1);
            count[1] = 10;
            if (ret_bool == false) {
                count[1] = ctp_get_duration_time_us_to_ms(count[0]);
            }

            /*if ctp is ready, no need to release i2c*/
            if (((lvalue & 0x1) == 0) && (ret_bool == true)) {
                count[2] = ctp_get_duration_time_us_to_ms(count[0]);
                break;
            }
        }
        ctp_hw_i2c_deinit(); /*if ctp not ready, should release i2c for other users*/
        time_count++;

        /*with 50ms cannot communicate with i2c, this is not ite*/
        if ((time_count >= 3) && ((ret_bool == false))) {
            log_hal_info("[CTP][ITE]communicate with i2c timeout\r\n");
            return false;
        }

        /*over 300ms, the ite command status always be busy*/
        if (time_count >= 30) {
            log_hal_info("[CTP][ITE]ite always busy error\r\n");
            return false;
        }
    }

    log_hal_info("QUERY_BUFFER_INDEX=%x, ret=%d, i2c_ready_at:%d ms, finish_busy_at:%d ms\r\n", \
                 lvalue, ret_bool, count[1], count[2]);

    time_total[1] = ctp_get_duration_time_us_to_ms(time_start);

    ctp_ite_7258_power(true);
    //ctp_ite_7258_set_interrupt_trigger();

    if (ctp_ite_get_firmware_information(&info) != true) {
        log_hal_info("[CTP]this touch is not ITE\r\n");
        ctp_hw_i2c_deinit();
        return false;
    } else {
        log_hal_info("[CTP]this touch is ITE\r\n");
    }

#ifdef CTP_ITE_UPGRADE_FIRMWARE
    ctp_ite_7258_upgrade();
    ctp_ite_get_firmware_information(&info);
#endif

    log_hal_info("[ITE]Leave ctp_ite_7258_init\r\n");

    ctp_hw_i2c_deinit();

    time_total[2] = ctp_get_duration_time_us_to_ms(time_start);
    log_hal_info("[ITE]init flow total time=%d ms;  after reset=%d ms;  before power=%d ms\r\n", \
                 time_total[2], time_total[1], time_total[0]);

    return true;

}

static uint32_t ctp_ite_7258_command(ctp_control_command_enum_t cmd, void *p1, void *p2)
{
    return false;
}

static ctp_customize_function_struct ctp_ite_7258_custom_function = {
    //ctp_Get_Data,
    ctp_ite_7258_init,
    ctp_ite_7258_device_mode,
    ctp_ite_7258_hisr,
    ctp_ite_7258_get_data,
    ctp_ite_7258_parameters,
    ctp_ite_7258_power,
    ctp_ite_7258_command,
    ctp_ite_7258_data_valid
};

ctp_customize_function_struct *ctp_ite_7258_get_custom_function(void)
{
    return (&ctp_ite_7258_custom_function);
}
#endif /*MTK_CTP_ENABLE*/

