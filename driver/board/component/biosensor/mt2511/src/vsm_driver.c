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
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "vsm_driver.h"
#include "vsm_signal_reg.h"
#include "hal_gpio.h"
#include "hal_gpt.h"
#include "syslog.h"
#ifdef MT2511_USE_SPI
#include "vsm_spi_operation.h"
#else
#include "vsm_i2c_operation.h"
#endif


log_create_module(clover_driver, PRINT_LEVEL_INFO);

#define LOGE(fmt,arg...)   LOG_E(clover_driver, "[clover]"fmt,##arg)
#define LOGW(fmt,arg...)   LOG_W(clover_driver, "[clover]"fmt,##arg)
#define LOGI(fmt,arg...)   LOG_I(clover_driver, "[clover]"fmt,##arg)

#define DBG                 0
#define DBG_READ            0
#define DBG_WRITE           0
#define DBG_SRAM            0

#if DBG
#define MT2511_LOG_MSG
#endif

#ifdef MT2511_LOG_MSG
#define LOGD(fmt,arg...)   LOG_I(clover_driver, "[clover]"fmt,##arg)
#else
#define LOGD(fmt,arg...)
#endif

//two slave addr
#define MT2511_SLAVE_I 	0x23
#define MT2511_SLAVE_II 0x33

//sram type addr
#define SRAM_EKG_ADDR	0xC8
#define SRAM_PPG1_ADDR	0xD8
#define SRAM_PPG2_ADDR	0xE8
#define SRAM_BISI_ADDR	0xF8

//read counter addr
#define SRAM_EKG_READ_COUNT_ADDR	0xC0
#define SRAM_PPG1_READ_COUNT_ADDR 	0xD0
#define SRAM_PPG2_READ_COUNT_ADDR 	0xE0
#define SRAM_BISI_READ_COUNT_ADDR 	0xF0

//write counter addr
#define SRAM_EKG_WRITE_COUNT_ADDR	0xCC
#define SRAM_PPG1_WRITE_COUNT_ADDR 	0xDC
#define SRAM_PPG2_WRITE_COUNT_ADDR 	0xEC
#define SRAM_BISI_WRITE_COUNT_ADDR 	0xFC

#define SRAM_COUNTER_RESET_MASK     0x20000000 
#define SRAM_COUNTER_OFFSET         29

//extern gpio var by name
extern const char BSP_BIO_SENSOR_PPG_VDRV_EN;
extern const char BSP_BIO_SENSOR_32K;
extern const char BSP_BIO_SENSOR_AFE_PWD_PIN;
extern const char BSP_BIO_SENSOR_RST_PORT_PIN;
#define GPIO_MT2511_PPG_VDRV_EN     (hal_gpio_pin_t)BSP_BIO_SENSOR_PPG_VDRV_EN
#define GPIO_MT2511_32K             (hal_gpio_pin_t)BSP_BIO_SENSOR_32K
#define GPIO_MT2511_AFE_PWD_PIN     (hal_gpio_pin_t)BSP_BIO_SENSOR_AFE_PWD_PIN
#define GPIO_MT2511_RST_PORT_PIN    (hal_gpio_pin_t)BSP_BIO_SENSOR_RST_PORT_PIN


#define UPDATE_COMMAND_ADDR     0x2328
#define SEC_UPDATE_COMMAND_ADDR 0x2728

#define PPG1_GAIN_ADDR		0x3318
#define PPG1_GAIN_MASK      0x00000007
#define PPG1_GAIN_OFFSET    0

#define PPG2_GAIN_ADDR      PPG1_GAIN_ADDR
#define PPG2_GAIN_MASK      0x00000038
#define PPG2_GAIN_OFFSET    3

#define PPG_AMDAC_ADDR      PPG1_GAIN_ADDR
#define PPG_AMDAC_MASK      0x3C00000
#define PPG_AMDAC_OFFSET    22

#define PPG_AMDAC1_MASK      0x1C00000
#define PPG_AMDAC1_OFFSET    22
#define PPG_AMDAC2_MASK      0xE000000
#define PPG_AMDAC2_OFFSET    25


#define PPG_PGA_GAIN_ADDR      PPG1_GAIN_ADDR
#define PPG_PGA_GAIN_MASK      0x1C0000
#define PPG_PGA_GAIN_OFFSET    18

#define PPG1_CURR_ADDR      0x332C
#define PPG1_CURR_MASK      0x000000FF
#define PPG1_CURR_OFFSET    0
#define PPG2_CURR_ADDR      PPG1_CURR_ADDR
#define PPG2_CURR_MASK      0x0000FF00
#define PPG2_CURR_OFFSET    8

#define CHIP_VERSION_ADDR       0x23AC
#define CHIP_VERSION_E1         0X1
#define CHIP_VERSION_E2         0X2
#define CHIP_VERSION_UNKNOWN    0XFF

#define DIGITAL_START_ADDR 0x3360

#define EKG_SAMPLE_RATE_ADDR1 0x3364
#define EKG_SAMPLE_RATE_ADDR2 0x3310
#define EKG_DEFAULT_SAMPLE_RATE 256

#define PPG_SAMPLE_RATE_ADDR 0x232C
#define PPG_FSYS             1048576
#define PPG_DEFAULT_SAMPLE_RATE 125

#define VSM_BISI_SRAM_LEN   256

#define MAX_WRITE_LENGTH 4
#define DUMMY_EKGPPG_REG 0x23A8

#define time_after(a,b)  ((long)(b) - (long)(a) < 0)
#define time_before(a,b) time_after(b,a) 

static signal_data_t EKG_PPG1_clk_setting[]={
    {0x3344,0x7CC00048},
    {0x3348,0x800000C3},
};

// Assign default capabilities setting
static int32_t combo_usage = 0;
uint32_t pre_ppg1_timestamp = 0;
uint32_t pre_ppg2_timestamp = 0;
uint32_t pre_ekg_timestamp = 0;
static uint32_t vsm_curr_tick;
static uint32_t vsm_curr_ms;
static uint32_t vsm_tick_remainder;
static int32_t   vsm_chip_version = -1;
static bool vsm_init_driver = true;
int32_t vsm_driver_chip_version_get(void);

vsm_status_t vsm_driver_read_register(bus_data_t *data)
{
    vsm_status_t ret = VSM_STATUS_ERROR;
    int32_t i = 0;

    if(data == NULL){
        LOGE("NULL data parameter\n");
        return VSM_STATUS_INVALID_PARAMETER;
    }

    if (vsm_init_driver == false) {
        LOGE("%s():addr 0x%x, reg 0x%x, len %d", 
            __func__, data->addr, data->reg, data->length);
        return ret;
    }

#ifdef MT2511_USE_SPI
    for (i = 0; i < 5; i ++)  {
        ret = vsm_spi_write_read(data->addr, data->reg, data->data_buf, data->length);
        if (ret == VSM_STATUS_OK) {
            break;
        }
        vTaskDelay(1);
    }
#else
    #if 1 /*send to receive*/
    for (i = 0; i < 5; i ++)  {
        ret = vsm_i2c_write_read(data->addr, data->reg, data->data_buf, data->length);
        if (ret == VSM_STATUS_OK) {
            break;
        }
        vTaskDelay(1);
    }
    #else /*write first then read*/
    for (i = 0; i < 5; i ++)  {
        ret = vsm_i2c_write(data->addr, data->reg, 1);

        if (ret == VSM_STATUS_OK) {
            ret = vsm_i2c_read(data->addr, data->data_buf, data->length);

            if (ret == VSM_STATUS_OK) {
                break;
            }
        }        
    }
    #endif
#endif
    if (DBG_READ) {
        LOGD("%s():addr 0x%x, reg 0x%x, len %d, value 0x%x", 
            __func__, data->addr, data->reg, data->length, *(uint32_t *)data->data_buf);
    }

    if (ret < VSM_STATUS_OK) {
        LOGE("vsm_driver_read_register error (%ld)\r\n", ret);
    }
    return ret;
}

vsm_status_t vsm_driver_write_register(bus_data_t *data)
{
	unsigned char txbuffer[MAX_WRITE_LENGTH * 2];
    unsigned char reg_addr;
    unsigned char data_len;
    int32_t ret, i = 0;

    if(data == NULL){
        LOGE("NULL data parameter\n");
        return VSM_STATUS_INVALID_PARAMETER;
    } else {
        reg_addr = data->reg;
        data_len = data->length;
    }

    if (vsm_init_driver == false) {
        LOGE("%s():addr 0x%x, reg 0x%x, len %d", 
            __func__, data->addr, data->reg, data->length);
        return VSM_STATUS_ERROR;
    }
    
    if (DBG_WRITE) {
        LOGD("%s():addr 0x%x, reg 0x%x, value 0x%x, len %d", 
            __func__, data->addr, reg_addr, *(uint32_t *)(data->data_buf), data_len);
    }
    
#ifdef MT2511_USE_SPI
    for (i = 0; i < 5; i ++)  {
        ret = vsm_spi_write(data->addr, reg_addr, data->data_buf, data_len);
        if (ret == VSM_STATUS_OK) {
            break;
        }
    }
#else
    txbuffer[0] = reg_addr;
    memcpy(txbuffer + 1, data->data_buf, data_len);
    for (i = 0; i < 5; i ++)  {
        ret = vsm_i2c_write(data->addr, txbuffer, data_len + 1);
        if (ret == VSM_STATUS_OK) {
            break;
        }
    }
#endif
    if (ret < 0) {
        LOGE("I2C Trasmit error(%ld)\n", ret);
        return VSM_STATUS_ERROR;
    } else {
        return VSM_STATUS_OK;   
    }
}

vsm_status_t vsm_driver_write_signal(signal_data_t *reg_addr, int32_t len, uint32_t *enable_data)
{
    bus_data_t data;
    uint32_t data_buf;
    int32_t i = 0;
    vsm_status_t err = VSM_STATUS_OK;

    if(!reg_addr || !enable_data) {
        err = VSM_STATUS_INVALID_PARAMETER;
        return err;
    }

    for( i = 0; i < len; i ++){
        if ((reg_addr+i)->addr != DIGITAL_START_ADDR){
            data.addr = ((reg_addr+i)->addr & 0xFF00) >> 8;
            data.reg = ((reg_addr+i)->addr & 0xFF);
            data.length = sizeof(data_buf);
            data.data_buf = (uint8_t *)&data_buf;
            err = vsm_driver_read_register(&data);
    		//ms_delay(5);
            if(DBG){
                LOGI( "%ld:vsm_driver_read1, addr:0x%x, reg:0x%x, data 0x%lx \r\n",
                        i, data.addr, data.reg, *(uint32_t *)(data.data_buf));
            }
            /* process with combo signal */
            if (combo_usage) {
                if ((reg_addr+i)->addr == 0x3360 || (reg_addr+i)->addr == 0x334C || (reg_addr+i)->addr == 0x3300 || 
                    (reg_addr+i)->addr == 0x3334) {
                    data_buf |= (reg_addr+i)->value;
                } else {
                    data_buf = (reg_addr+i)->value;
                }	
            } else {
                data_buf = (reg_addr+i)->value;
            }
            data.length = sizeof(data_buf);
            if(DBG){
    			LOGI( "%ld:vsm_driver_write, addr:0x%x, reg:0x%x, data 0x%lx, length %d \r\n",
            							i, data.addr, data.reg, data_buf, data.length);
            }
            err = vsm_driver_write_register(&data);
            //ms_delay(5);
            data_buf = 0;
            err = vsm_driver_read_register(&data);
            if(DBG){
    			LOGI( "%ld:vsm_driver_read2, addr:0x%x, reg:0x%x, data 0x%lx \r\n",
            							i, data.addr, data.reg, *(uint32_t *)(data.data_buf));
            }
        } else { /*address equal to 0x3360*/
            *enable_data = (reg_addr+i)->value;
        }
    }
    return err;
}

vsm_status_t vsm_driver_set_signal(vsm_signal_t signal)
{
    uint32_t enable_data, signal_enable_data = 0;
    uint32_t temp_combo_signal = 0;
    vsm_status_t err = VSM_STATUS_OK;
    int32_t len, i = 0;
    signal_data_t *temp;
    bus_data_t data;

    if (signal & VSM_SIGNAL_EKG) {
        len = sizeof(VSM_SIGNAL_EKG_array)/sizeof(VSM_SIGNAL_EKG_array[0]);
        temp = VSM_SIGNAL_EKG_array;
        vsm_driver_write_signal(temp, len, &enable_data);
        signal_enable_data |= enable_data;
    }

    if (signal & VSM_SIGNAL_EEG) {
        len = sizeof(VSM_SIGNAL_EEG_array)/sizeof(VSM_SIGNAL_EEG_array[0]);
        temp = VSM_SIGNAL_EEG_array;
        vsm_driver_write_signal(temp, len, &enable_data);
        signal_enable_data |= enable_data;
    }

    if (signal & VSM_SIGNAL_EMG) {
        len = sizeof(VSM_SIGNAL_EMG_array)/sizeof(VSM_SIGNAL_EMG_array[0]);
        temp = VSM_SIGNAL_EMG_array;
        vsm_driver_write_signal(temp, len, &enable_data);
        signal_enable_data |= enable_data;
    }

    if (signal & VSM_SIGNAL_GSR) {
        len = sizeof(VSM_SIGNAL_GSR_array)/sizeof(VSM_SIGNAL_GSR_array[0]);
        temp = VSM_SIGNAL_GSR_array;
        vsm_driver_write_signal(temp, len, &enable_data);
        signal_enable_data |= enable_data;
    }

    if (signal & VSM_SIGNAL_PPG1) {
        len = sizeof(VSM_SIGNAL_PPG1_array)/sizeof(VSM_SIGNAL_PPG1_array[0]);
        temp = VSM_SIGNAL_PPG1_array;
        vsm_driver_write_signal(temp, len, &enable_data);
        signal_enable_data |= enable_data;
    }

    if (signal & VSM_SIGNAL_PPG2) {
        len = sizeof(VSM_SIGNAL_PPG2_array)/sizeof(VSM_SIGNAL_PPG2_array[0]);
        temp = VSM_SIGNAL_PPG2_array;
        vsm_driver_write_signal(temp, len, &enable_data);
        signal_enable_data |= enable_data;
    }

    if (signal & VSM_SIGNAL_BISI) {
        len = sizeof(VSM_SIGNAL_BISI_array)/sizeof(VSM_SIGNAL_BISI_array[0]);
        temp = VSM_SIGNAL_BISI_array;
        vsm_driver_write_signal(temp, len, &enable_data);
        signal_enable_data |= enable_data;
    }

    //if (signal & VSM_SIGNAL_BL) {
    if (signal & VSM_SIGNAL_PPG1_512HZ) {
#ifdef FUSION_BLOOD_PRESSURE_USE
        vsm_driver_init();
        combo_usage |= VSM_SIGNAL_EKG;
#endif
        len = sizeof(VSM_SIGNAL_PPG1_512HZ_array)/sizeof(VSM_SIGNAL_PPG1_512HZ_array[0]);
        temp = VSM_SIGNAL_PPG1_512HZ_array;
        vsm_driver_write_signal(temp, len, &enable_data);
        signal_enable_data |= enable_data;
#ifdef FUSION_BLOOD_PRESSURE_USE
        len = sizeof(VSM_SIGNAL_EKG_array)/sizeof(VSM_SIGNAL_EKG_array[0]);
        temp = VSM_SIGNAL_EKG_array;
        vsm_driver_write_signal(temp, len, &enable_data);        
        signal_enable_data |= enable_data;
#endif
    }
    LOGI("signal_enable_data 0x%x, signal %d\r\n", signal_enable_data, signal);

    if (combo_usage ) {
        temp_combo_signal = (combo_usage | signal);
        /*set clock gating register setting patch for combo signal*/
        if ( ((temp_combo_signal & VSM_SIGNAL_EKG) && (temp_combo_signal & VSM_SIGNAL_PPG1)) ||
             ((temp_combo_signal & VSM_SIGNAL_EKG) && (temp_combo_signal & VSM_SIGNAL_PPG1_512HZ))) {
            data.length = sizeof(enable_data);
            data.data_buf = (uint8_t *)&enable_data;
            len = sizeof(EKG_PPG1_clk_setting)/sizeof(EKG_PPG1_clk_setting[0]);
            for (i = 0; i < len; i ++) {
                data.addr = (EKG_PPG1_clk_setting[i].addr& 0xFF00) >> 8;
                data.reg = (EKG_PPG1_clk_setting[i].addr & 0xFF);
                enable_data = EKG_PPG1_clk_setting[i].value;
                vsm_driver_write_register(&data);
            }
        } 
    }
    #ifndef BIO_TUNNING_TOOL_USE
    /*wirte 0x3360 to enable signal*/
    /*enable signal, read first then write*/
    data.addr = (DIGITAL_START_ADDR & 0xFF00) >> 8;
    data.reg = (DIGITAL_START_ADDR & 0xFF);
    data.length = sizeof(enable_data);
    data.data_buf = (uint8_t *)&enable_data;
    if (combo_usage) {    
        vsm_driver_read_register(&data);
        enable_data |= signal_enable_data;
    }    
    err = vsm_driver_write_register(&data);
    #endif 
    combo_usage |= signal;
    LOGI("enable_data 0x%x, combo_usage 0x%x\r\n", enable_data, combo_usage);
    return err;
}

static vsm_status_t vsm_driver_read_counter(vsm_sram_type_t sram_type, uint32_t *counter)
{
	vsm_status_t err = VSM_STATUS_OK;
	bus_data_t data;
	if (counter == NULL) {
		err = VSM_STATUS_INVALID_PARAMETER;
		return err;
	}

	switch(sram_type){
		case VSM_SRAM_EKG:
			data.reg = SRAM_EKG_READ_COUNT_ADDR;
			break;

		case VSM_SRAM_PPG1:
			data.reg = SRAM_PPG1_READ_COUNT_ADDR;
			break;

		case VSM_SRAM_PPG2:
			data.reg = SRAM_PPG2_READ_COUNT_ADDR;
			break;

		case VSM_SRAM_BISI:
			data.reg = SRAM_BISI_READ_COUNT_ADDR;
			break;

		case VSM_SRAM_NUMBER:
		default:
			err = VSM_STATUS_INVALID_PARAMETER;
			return err;
	}

	data.addr = MT2511_SLAVE_II;
	data.data_buf = (uint8_t *)counter;
	data.length  = sizeof(uint32_t);
	err = vsm_driver_read_register(&data);
	if (err == VSM_STATUS_OK){
		*counter = ((*counter & 0x01ff0000) >> 16) > VSM_SRAM_LEN ? 0 : ((*counter & 0x01ff0000) >> 16);
		if(DBG){
			LOGD( "vsm_driver_read_counter 0x%lx \r\n", *counter);
		}
	}
	return err;
}

static vsm_status_t vsm_driver_write_counter(vsm_sram_type_t sram_type, uint32_t *write_counter)
{
	vsm_status_t err = VSM_STATUS_OK;
	bus_data_t data;
	uint32_t counter = 0;

	if (write_counter == NULL) {
		err = VSM_STATUS_INVALID_PARAMETER;
		return err;
	}

	switch(sram_type){
	case VSM_SRAM_EKG:
		data.reg = SRAM_EKG_WRITE_COUNT_ADDR;
		break;

	case VSM_SRAM_PPG1:
		data.reg = SRAM_PPG1_WRITE_COUNT_ADDR;
		break;

	case VSM_SRAM_PPG2:
		data.reg = SRAM_PPG2_WRITE_COUNT_ADDR;
		break;

	case VSM_SRAM_BISI:
		data.reg = SRAM_BISI_WRITE_COUNT_ADDR;
		break;

	case VSM_SRAM_NUMBER:
	default:
		err = VSM_STATUS_INVALID_PARAMETER;
		return err;
	}

	data.addr = MT2511_SLAVE_II;
	data.data_buf = (uint8_t *)&counter;
	data.length  = sizeof(uint32_t);
	err = vsm_driver_read_register(&data);

	if (err == VSM_STATUS_OK){
		counter = (counter & 0x01ff0000) >> 16;
		if(DBG){
			LOGD( "vsm_driver_write_counter 0x%lx \r\n", counter);
		}
	}
	*write_counter = counter;

	return err;
}

int32_t vsm_driver_check_sample_rate(vsm_sram_type_t sram_type)
{
    int32_t sample_rate;
    vsm_status_t err = VSM_STATUS_OK;
    bus_data_t data;
    
    switch(sram_type){
        case VSM_SRAM_EKG:
            {
                int32_t ekg_sample_data1, ekg_sample_data2;
                data.addr = (EKG_SAMPLE_RATE_ADDR1 & 0xFF00) >> 8;
                data.reg  = (EKG_SAMPLE_RATE_ADDR1 & 0xFF);
                data.data_buf = (uint8_t *)&ekg_sample_data1;
                data.length = sizeof(ekg_sample_data1);
                err = vsm_driver_read_register(&data);
                if (err == VSM_STATUS_OK) {
                    data.addr = (EKG_SAMPLE_RATE_ADDR2 & 0xFF00) >> 8;
                    data.reg  = (EKG_SAMPLE_RATE_ADDR2 & 0xFF);
                    data.data_buf = (uint8_t *)&ekg_sample_data2;
                    err = vsm_driver_read_register(&data);
                    //LOGI("ekg_sample_data1 0x%x, ekg_sample_data2 0x%x", ekg_sample_data1, ekg_sample_data2);
                    if (err == VSM_STATUS_OK) {
                        if ((ekg_sample_data1 & 0x7 ) == 0 && ((ekg_sample_data2 & 0x1C0000) >> 18 ) == 0) {
                            sample_rate = 64;
                        } else if ((ekg_sample_data1 & 0x7 ) == 1 && ((ekg_sample_data2 & 0x1C0000) >> 18 ) == 1) {
                            sample_rate = 128;
                        } else if ((ekg_sample_data1 & 0x7 ) == 2 && ((ekg_sample_data2 & 0x1C0000) >> 18 ) == 2) {
                            sample_rate = 256;
                        } else if ((ekg_sample_data1 & 0x7 ) == 3 && ((ekg_sample_data2 & 0x1C0000) >> 18 ) == 3) {
                            sample_rate = 512;
                        } else if ((ekg_sample_data1 & 0x7 ) == 4 && ((ekg_sample_data2 & 0x1C0000) >> 18 ) == 3) {
                            sample_rate = 1024;
                        } else if ((ekg_sample_data1 & 0x7 ) == 5 && ((ekg_sample_data2 & 0x1C0000) >> 18 ) == 3) {
                            sample_rate = 2048;
                        } else if ((ekg_sample_data1 & 0x7 ) == 6 && ((ekg_sample_data2 & 0x1C0000) >> 18 ) == 3) {
                            sample_rate = 4096;
                        } else {
                            sample_rate = EKG_DEFAULT_SAMPLE_RATE;
                        }
                    } else {
                        sample_rate = EKG_DEFAULT_SAMPLE_RATE;
                    }
                } else {
                    sample_rate = EKG_DEFAULT_SAMPLE_RATE;
                }
            }
        	break;

        case VSM_SRAM_PPG1:
        case VSM_SRAM_PPG2:
            {
                uint32_t ppg_sample_data;
        	    data.addr = (PPG_SAMPLE_RATE_ADDR & 0xFF00) >> 8;
                data.reg  = (PPG_SAMPLE_RATE_ADDR & 0xFF);
        	    data.data_buf = (uint8_t *)&ppg_sample_data;
                data.length = sizeof(ppg_sample_data);
                err = vsm_driver_read_register(&data);
                if (err == VSM_STATUS_OK) {
                    LOGD("ppg_sample_data 0x%x\r\n", ppg_sample_data);
                    sample_rate = PPG_FSYS/((ppg_sample_data & 0x3FFF) + 1);
                } else {
                    LOGE("ppg_sample_data i2c error, err %d\r\n", err);
                    sample_rate = PPG_DEFAULT_SAMPLE_RATE;
                }
            }
            break;
        case VSM_SRAM_BISI:
        case VSM_SRAM_NUMBER:
        default:
        	return VSM_STATUS_INVALID_PARAMETER;
    }
    return sample_rate;
}

uint32_t vsm_driver_get_ms_tick()
{
    uint32_t new_curr_tick;
    hal_gpt_status_t ret;

    ret = hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &new_curr_tick);
    if (ret != HAL_GPT_STATUS_OK) {
        LOGE("hal_gpt_get_free_run_count err (%d) \r\n", ret);
    }

    vsm_curr_ms += (uint32_t)((((uint64_t)(new_curr_tick - vsm_curr_tick))*1000 + vsm_tick_remainder)/32768);
    vsm_tick_remainder = (((uint64_t)(new_curr_tick - vsm_curr_tick))*1000%32768);
    vsm_curr_tick = new_curr_tick;

    return vsm_curr_ms;
}

vsm_status_t vsm_driver_check_write_counter(vsm_sram_type_t sram_type, uint32_t read_counter, uint32_t *write_counter)
{
    int i = 0;
    vsm_status_t err = VSM_STATUS_OK;
    uint32_t *pre_timestamp = NULL, cur_timestamp, sample_rate = 0;
    uint32_t expected_counter = 0, real_counter = 0;
    if (write_counter == NULL) {
        return VSM_STATUS_INVALID_PARAMETER;
    }

    switch(sram_type){
        case VSM_SRAM_EKG:
            pre_timestamp = &pre_ekg_timestamp;
            sample_rate = vsm_driver_check_sample_rate(sram_type);
            break;

        case VSM_SRAM_PPG1:
            pre_timestamp = &pre_ppg1_timestamp;
            sample_rate = vsm_driver_check_sample_rate(sram_type);
            break;

        case VSM_SRAM_PPG2:
            pre_timestamp = &pre_ppg2_timestamp;
            sample_rate = vsm_driver_check_sample_rate(sram_type);
            break;

        case VSM_SRAM_BISI:
            vsm_driver_write_counter(sram_type,write_counter);
            break;

        default:
            err = VSM_STATUS_INVALID_PARAMETER;
            break;
    }
    if (sram_type < VSM_SRAM_BISI) {
        //cur_timestamp = xTaskGetTickCount();
        cur_timestamp = vsm_driver_get_ms_tick();
        if (*pre_timestamp == 0) {
            err = vsm_driver_write_counter(sram_type,write_counter);
        } else {
            expected_counter = (uint32_t)(((cur_timestamp - *pre_timestamp) * sample_rate)/ 1000);
            for(i = 0; i < 10; i++) {
                vsm_driver_write_counter(sram_type,write_counter);
                real_counter = (*write_counter >= read_counter)? (*write_counter - read_counter) : (VSM_SRAM_LEN - read_counter + *write_counter);
                if (sram_type == VSM_SRAM_PPG1 || sram_type == VSM_SRAM_PPG2) {
                    real_counter /=2;
                }
                LOGD("sample_rate %lu, real_counter %lu,expected_counter %lu", sample_rate, real_counter, expected_counter);
                if (real_counter < (20 + expected_counter) && 
                    real_counter > (expected_counter > 20?(expected_counter - 20):0)) {
                    break;
                } else {
                    LOGD("try %d times", i);
                }
            }
        }
        LOGD("%s():cur_timestamp %lu, pre_timestamp %lu", __func__, cur_timestamp, *pre_timestamp);
        *pre_timestamp = cur_timestamp;
    }
    return err;
}

vsm_status_t vsm_driver_read_sram(vsm_sram_type_t sram_type, uint32_t *data_buf, int32_t *len)
{
    vsm_status_t err = VSM_STATUS_OK;
    int i = 0;
    uint32_t read_counter = 0, write_counter = 0, sram_len;
    bus_data_t data;
    if (data_buf == NULL || len == NULL){
    	err = VSM_STATUS_INVALID_PARAMETER;
    	return err;
    }
    //1. compute how many sram data
    //read counter
    vsm_driver_read_counter(sram_type, &read_counter);

    //write counter
    //vsm_driver_write_counter(sram_type, &write_counter);

    vsm_driver_check_write_counter(sram_type, read_counter, &write_counter);    

    //sram_write_counter = ((register_val & 0x01ff0000) >> 16);	// bit 25 ~ bit 16
    //LOGI("sram_type %d, write_counter %d, read_counter %d\r\n", sram_type, write_counter, read_counter);
    //irq_th = register_val & 0x1ff;
    //only get even number to prevent the reverse of ppg and ambient
    if (sram_type != VSM_SRAM_BISI) {
        //compute sram length
        sram_len = (write_counter >= read_counter)? (write_counter - read_counter) : (VSM_SRAM_LEN - read_counter + write_counter);
        sram_len = ((sram_len%2) == 0) ? sram_len : sram_len - 1;
    } else {
        sram_len = (write_counter >= read_counter)? (write_counter - read_counter) : (VSM_BISI_SRAM_LEN - read_counter + write_counter);
    }
    *len = sram_len;
    LOGD("sram_type %d, sram_len %d\r\n", sram_type, sram_len);
    if (sram_len > 0) {
    	//2. get sram data to data_buf
    	switch(sram_type){
    		case VSM_SRAM_EKG:
    			data.reg = SRAM_EKG_ADDR;
    			break;

    		case VSM_SRAM_PPG1:
    			data.reg = SRAM_PPG1_ADDR;
    			break;

    		case VSM_SRAM_PPG2:
    			data.reg = SRAM_PPG2_ADDR;
    			break;

    		case VSM_SRAM_BISI:
    			data.reg = SRAM_BISI_ADDR;
    			break;

    		case VSM_SRAM_NUMBER:
    		default:
    			err = VSM_STATUS_INVALID_PARAMETER;
    			return err;
    	}

    	data.addr = MT2511_SLAVE_II;

    	//get sram data
    	for (i = 0; i < sram_len; i ++){
    		//assign buffer location
    		data.data_buf = (uint8_t *)(data_buf + i);
    		data.length  = 4;
    		err = vsm_driver_read_register(&data);
    		if (err != VSM_STATUS_OK) {
    			break;
    		}
            //ms_delay(10);
    		#if DBG_SRAM
    		LOGD("%s():(%d)read sram data:0x%x\r\n" , __func__, i, *(uint32_t *)(data.data_buf));
    		#endif
    	}

    }else {
    	err = VSM_STATUS_INVALID_PARAMETER;
    }

    return err;
}

vsm_status_t vsm_driver_update_register(void)
{
	uint32_t write_data;
	bus_data_t data;
	vsm_status_t err = VSM_STATUS_OK;

	data.addr = (UPDATE_COMMAND_ADDR & 0xFF00) >> 8;
	data.reg  = (UPDATE_COMMAND_ADDR & 0xFF);
	write_data = (uint32_t )0xFFFF0002;
	data.data_buf = (uint8_t *)&write_data;
	data.length = sizeof(write_data);

	err = vsm_driver_write_register(&data);
	if (err == VSM_STATUS_OK) {
		write_data = 0xFFFF0000;
		err = vsm_driver_write_register(&data);
	}
	return err;
}

vsm_status_t vsm_driver_set_tia_gain(vsm_signal_t ppg_type, vsm_tia_gain_t input)
{
    vsm_status_t err = VSM_STATUS_OK;
    bus_data_t data;
    uint32_t read_data;

    if (ppg_type == VSM_SIGNAL_PPG1 || ppg_type == VSM_SIGNAL_PPG2) {

        switch(ppg_type){
        case VSM_SIGNAL_PPG1:
            data.reg  = (PPG1_GAIN_ADDR & 0xFF);
            data.addr = (PPG1_GAIN_ADDR & 0xFF00) >> 8;
            break;
        case VSM_SIGNAL_PPG2:
            data.reg  = (PPG2_GAIN_ADDR  & 0xFF);
            data.addr = (PPG2_GAIN_ADDR & 0xFF00) >> 8;
            break;
        default:
            break;
        }

        data.data_buf = (uint8_t *)&read_data;
        data.length = sizeof(read_data);
        err = vsm_driver_read_register(&data);

        if (err == VSM_STATUS_OK) {
            if (ppg_type == VSM_SIGNAL_PPG1) {
                read_data &= ~PPG1_GAIN_MASK;
                read_data |= (input & 0x7) << PPG1_GAIN_OFFSET;
            } else if (ppg_type == VSM_SIGNAL_PPG2) {
                read_data &= ~PPG2_GAIN_MASK;
                read_data |= (input & 0x7) << PPG2_GAIN_OFFSET;
            }
            err = vsm_driver_write_register(&data);
            /* update register setting */
            vsm_driver_update_register();
        }
    }else {
    	err = VSM_STATUS_INVALID_PARAMETER;
    }

    return err;
}

vsm_status_t vsm_driver_set_pga_gain(vsm_pga_gain_t input)
{
    vsm_status_t err = VSM_STATUS_OK;
    bus_data_t data;
    uint32_t read_data;

    data.reg  = (PPG_PGA_GAIN_ADDR & 0xFF);
    data.addr = (PPG_PGA_GAIN_ADDR & 0xFF00) >> 8;
    data.data_buf = (uint8_t *)&read_data;
    data.length = sizeof(read_data);
    err = vsm_driver_read_register(&data);

    if (err == VSM_STATUS_OK) {
        if (input > VSM_PGA_GAIN_6) {
            input = VSM_PGA_GAIN_6;
        }
        read_data &= ~PPG_PGA_GAIN_MASK;
        read_data |= (input & 0x7) << PPG_PGA_GAIN_OFFSET;
        err = vsm_driver_write_register(&data);
        /* update register setting */
        vsm_driver_update_register();
    }

	return err;
}

vsm_status_t vsm_driver_set_led_current(vsm_led_type_t led_type, uint32_t input)
{
    vsm_status_t err = VSM_STATUS_OK;
    bus_data_t data;
    uint32_t read_data;

    if (led_type == VSM_LED_1 || led_type == VSM_LED_2) {
        switch(led_type){
            case VSM_LED_1:
                data.reg  = (PPG1_CURR_ADDR & 0xFF);
                data.addr = (PPG1_CURR_ADDR & 0xFF00) >> 8;
                break;
            case VSM_LED_2:
                data.reg  = (PPG2_CURR_ADDR  & 0xFF);
                data.addr = (PPG2_CURR_ADDR & 0xFF00) >> 8;
                break;
            default:
                break;
        }

    	data.data_buf = (uint8_t *)&read_data;
    	data.length = sizeof(read_data);
    	err = vsm_driver_read_register(&data);

    	if (err == VSM_STATUS_OK) {
            if (led_type == VSM_LED_1) {
                read_data &= ~PPG1_CURR_MASK;
                read_data |= (input & 0xFF) << PPG1_CURR_OFFSET;
            } else if (led_type == VSM_LED_2) {
                read_data &= ~PPG2_CURR_MASK;
                read_data |= (input & 0xFF) << PPG2_CURR_OFFSET;
            }
            err = vsm_driver_write_register(&data);
            /* update register setting */
            vsm_driver_update_register();
    	}
    } else {
        err = VSM_STATUS_INVALID_PARAMETER;
    }

    return err;
}

vsm_status_t vsm_driver_set_ambdac_current(vsm_ambdac_type_t ambdac_type, vsm_ambdac_current_t current)
{
    vsm_status_t err = VSM_STATUS_OK;
    bus_data_t data;
    uint32_t read_data;

    data.reg  = (PPG_AMDAC_ADDR & 0xFF);
    data.addr = (PPG_AMDAC_ADDR & 0xFF00) >> 8;
    data.data_buf = (uint8_t *)&read_data;
    data.length = sizeof(read_data);
    err = vsm_driver_read_register(&data);

    if (err == VSM_STATUS_OK) {
        if (current > VSM_AMBDAC_CURR_06_MA) {
            current = VSM_AMBDAC_CURR_06_MA;
        }
        if (vsm_driver_chip_version_get() == CHIP_VERSION_E1) {            
            read_data &= ~PPG_AMDAC_MASK;
            read_data |= (current & 0xF) << PPG_AMDAC_OFFSET;
            err = vsm_driver_write_register(&data);
        } else if (vsm_driver_chip_version_get() == CHIP_VERSION_E2) {
            if (ambdac_type == VSM_AMBDAC_1) {
                read_data &= ~PPG_AMDAC1_MASK;
                read_data |= (current & 0x7) << PPG_AMDAC1_OFFSET;
                err = vsm_driver_write_register(&data);
            } else if (ambdac_type == VSM_AMBDAC_2) {
                read_data &= ~PPG_AMDAC2_MASK;
                read_data |= (current & 0x7) << PPG_AMDAC2_OFFSET;
                err = vsm_driver_write_register(&data);
            }
        }
        /* update register setting */
        vsm_driver_update_register();
    }
    return err;
}

int32_t vsm_driver_chip_version_get()
{
    int err = VSM_STATUS_OK;
	bus_data_t data;
	uint32_t read_data;

    if (vsm_chip_version == -1) {
        data.reg  = (CHIP_VERSION_ADDR & 0xFF);
        data.addr = (CHIP_VERSION_ADDR & 0xFF00) >> 8;
        data.data_buf = (uint8_t *)&read_data;
        data.length = sizeof(read_data);
        err = vsm_driver_read_register(&data);
        if (err == VSM_STATUS_OK) {
            LOGI("read back chip version:0x%x", read_data);
            if (read_data == 0x25110000) {
                vsm_chip_version = CHIP_VERSION_E2;
            } else if(read_data == 0xFFFFFFFF) {
                vsm_chip_version = CHIP_VERSION_E1;
            } else {
                vsm_chip_version = CHIP_VERSION_UNKNOWN;
            } 
        } else {
            vsm_chip_version = CHIP_VERSION_UNKNOWN;
        }
    }

    return vsm_chip_version;    
}

void ms_delay(uint32_t ms)
{
#if 0
    uint32_t end, current; 
    current = xTaskGetTickCount();
    end = xTaskGetTickCount() + ms + 1;

    do{
        current = xTaskGetTickCount();
    }while(time_before(current, end));
#else
    hal_gpt_delay_ms(ms);
#endif
    
}

vsm_status_t vsm_driver_set_register_map(vsm_signal_t signal, int32_t addr, uint32_t reg_data)
{
    vsm_status_t err = VSM_STATUS_ERROR;
    int32_t len, i = 0;
    signal_data_t *temp = NULL;

    //check which signal and length    
    if (signal & VSM_SIGNAL_EKG) {
        len = sizeof(VSM_SIGNAL_EKG_array)/sizeof(VSM_SIGNAL_EKG_array[0]);
        temp = VSM_SIGNAL_EKG_array;
    }

    if (signal & VSM_SIGNAL_EEG) {
        len = sizeof(VSM_SIGNAL_EEG_array)/sizeof(VSM_SIGNAL_EEG_array[0]);
        temp = VSM_SIGNAL_EEG_array;
    }

    if (signal & VSM_SIGNAL_EMG) {
        len = sizeof(VSM_SIGNAL_EMG_array)/sizeof(VSM_SIGNAL_EMG_array[0]);
        temp = VSM_SIGNAL_EMG_array;
    }

    if (signal & VSM_SIGNAL_GSR) {
        len = sizeof(VSM_SIGNAL_GSR_array)/sizeof(VSM_SIGNAL_GSR_array[0]);
        temp = VSM_SIGNAL_GSR_array;
    }

    if (signal & VSM_SIGNAL_PPG1) {
        len = sizeof(VSM_SIGNAL_PPG1_array)/sizeof(VSM_SIGNAL_PPG1_array[0]);
        temp = VSM_SIGNAL_PPG1_array;
    }

    if (signal & VSM_SIGNAL_PPG2) {
        len = sizeof(VSM_SIGNAL_PPG2_array)/sizeof(VSM_SIGNAL_PPG2_array[0]);
        temp = VSM_SIGNAL_PPG2_array;
    }

    if (signal & VSM_SIGNAL_BISI) {
        len = sizeof(VSM_SIGNAL_BISI_array)/sizeof(VSM_SIGNAL_BISI_array[0]);
        temp = VSM_SIGNAL_BISI_array;
    }
    
    //signal not exist
    if (temp == NULL) {
        err = VSM_STATUS_INVALID_PARAMETER;
    } else {
        // find the register and modify its content
        for (i = 0; i < len; i++) {
            if (temp->addr == addr) {
                temp->value = reg_data;
                err = VSM_STATUS_OK;
                break;
            }
            temp++;
        }
    }

    return err;
}

vsm_status_t vsm_driver_enable_signal(vsm_signal_t signal)
{
    vsm_status_t err = VSM_STATUS_OK;
    bus_data_t data;
    uint32_t enable_data, reg_data;

    //{0x3360,0x00000187}   //Enable Mode[0:5]=[018:124:187:164:33C:3FF]
    switch(signal){
        case VSM_SIGNAL_EKG:
        case VSM_SIGNAL_EEG:
        case VSM_SIGNAL_EMG:
        case VSM_SIGNAL_GSR:
            enable_data = 0x18;
            break;
        case VSM_SIGNAL_PPG1:
        case VSM_SIGNAL_PPG1_512HZ:
            //enable_data = 0x124;
            enable_data = 0x124;
            break;
        case VSM_SIGNAL_PPG2:
            //enable_data = 0x144;
            enable_data = 0x144;
            break;
        case VSM_SIGNAL_BISI:
            //enable_data = 0x187;
            enable_data = 0x187;
            break;
        default:
            enable_data = 0x00;
            break;
    }
    data.addr = (DIGITAL_START_ADDR & 0xFF00) >> 8;
    data.reg  = (DIGITAL_START_ADDR & 0xFF);
    data.data_buf = (uint8_t *)&reg_data;
    data.length = sizeof(reg_data);
    err = vsm_driver_read_register(&data);
    if (err == VSM_STATUS_OK) {
        reg_data |= (enable_data);        
        err = vsm_driver_write_register(&data);
    }

    return err;
}

vsm_status_t vsm_driver_disable_signal(vsm_signal_t signal)
{
    vsm_status_t err = VSM_STATUS_OK;
    bus_data_t data;
    uint32_t enable_data, reg_data;

    //{0x3360,0x00000187}   //Enable Mode[0:5]=[018:124:187:164:33C:3FF]
    switch(signal){
        case VSM_SIGNAL_EKG:
        case VSM_SIGNAL_EEG:
        case VSM_SIGNAL_EMG:
        case VSM_SIGNAL_GSR:
            enable_data = 0x18;
            break;
        case VSM_SIGNAL_PPG1:
        case VSM_SIGNAL_PPG1_512HZ:
            //enable_data = 0x124;
            enable_data = 0x24;
            break;
        case VSM_SIGNAL_PPG2:
            //enable_data = 0x144;
            enable_data = 0x44;
            break;
        case VSM_SIGNAL_BISI:
            //enable_data = 0x187;
            enable_data = 0x87;
            break;
        default:
            enable_data = 0x00;
            break;
    }
    data.addr = (DIGITAL_START_ADDR & 0xFF00) >> 8;
    data.reg  = (DIGITAL_START_ADDR & 0xFF);
    data.data_buf = (uint8_t *)&reg_data;
    data.length = sizeof(reg_data);
    err = vsm_driver_read_register(&data);
    if (err == VSM_STATUS_OK) {
        reg_data &= (~enable_data);
        err = vsm_driver_write_register(&data);
    }

    return err;
}

vsm_status_t vsm_driver_reset_PPG_counter(void)
{
    vsm_status_t err = VSM_STATUS_OK;
    bus_data_t data;
    uint32_t reg_data = 0;
    /* step 1: (disable PPG function and reset PPG1 and PPG2 write counters to 0) */
    data.addr = (DIGITAL_START_ADDR & 0xFF00) >> 8;
    data.reg  = (DIGITAL_START_ADDR & 0xFF);  
    data.data_buf = (uint8_t *)&reg_data;
    data.length = sizeof(reg_data);
    err = vsm_driver_write_register(&data);
    if (err == VSM_STATUS_OK) {
        /* step 2: 0x33D0 = 0x2000_0000 (reset PPG1 read counter to 0) */
        data.reg  = SRAM_PPG1_READ_COUNT_ADDR;
        reg_data = 0x60000000;
        err = vsm_driver_write_register(&data);
        if (err == VSM_STATUS_OK) {
            /* step 3: 0x33E0 = 0x2000_0000 (reset PPG2 read counter to 0) */
                data.reg  = SRAM_PPG2_READ_COUNT_ADDR;
                reg_data = 0x60000000;
                err = vsm_driver_write_register(&data);
        }
    }
    return err;
}

vsm_status_t vsm_driver_upgrade_i2c(void)
{
    bus_data_t data;
    uint32_t enable_data = 0;
    vsm_status_t err = VSM_STATUS_ERROR;

    data.addr = (DUMMY_EKGPPG_REG & 0xFF00) >> 8;
    data.reg  = (DUMMY_EKGPPG_REG & 0xFF);  
    data.data_buf = (uint8_t *)&enable_data;
    data.length = sizeof(enable_data);
    enable_data |= (uint32_t)(1U<<31);
    err = vsm_driver_write_register(&data);
    return err;
}

static vsm_status_t vsm_driver_idle(void)
{
    bus_data_t data;
    uint32_t enable_data = 0, len = 0;
    int i = 0;
    vsm_status_t err = VSM_STATUS_OK;
    len = sizeof(VSM_SIGNAL_IDLE_array)/sizeof(VSM_SIGNAL_IDLE_array[0]);
    data.data_buf = (uint8_t *)&enable_data;
    data.length = sizeof(enable_data);
    for (i = 0; i < len; i ++) {
        if (vsm_driver_chip_version_get() == CHIP_VERSION_E1) {
            if ((VSM_SIGNAL_IDLE_array[i].addr == 0x3300 && VSM_SIGNAL_IDLE_array[i].value == 0xA8C71555) ||
                (VSM_SIGNAL_IDLE_array[i].addr == 0x331C && VSM_SIGNAL_IDLE_array[i].value == 0x0048C429)) {
                continue;
            } 
            #ifndef USE_EXTERNAL_BOOST
            else if ((VSM_SIGNAL_IDLE_array[i].addr == 0x3334 && VSM_SIGNAL_IDLE_array[i].value == 0x00480000)) {
                VSM_SIGNAL_IDLE_array[i].value = 0x004C0000;
            } else if ((VSM_SIGNAL_IDLE_array[i].addr == 0x3334 && VSM_SIGNAL_IDLE_array[i].value == 0x00480100)) {
                VSM_SIGNAL_IDLE_array[i].value = 0x004C0100;
            }
            #endif
        }
        data.addr = (VSM_SIGNAL_IDLE_array[i].addr & 0xFF00) >> 8;
        data.reg  = (VSM_SIGNAL_IDLE_array[i].addr & 0xFF);          
        enable_data = VSM_SIGNAL_IDLE_array[i].value;
        err = vsm_driver_write_register(&data);

        if (err != VSM_STATUS_OK) {
            break;
        }

        #ifndef USE_EXTERNAL_BOOST
        if ((VSM_SIGNAL_IDLE_array[i].addr == 0x3334)){
            if ( (VSM_SIGNAL_IDLE_array[i].value == 0x00480000) || (VSM_SIGNAL_IDLE_array[i].value == 0x004C0000)) {
                ms_delay(45);
            }
        }
        #endif
        if (vsm_driver_chip_version_get() == CHIP_VERSION_E2) {
            if (VSM_SIGNAL_IDLE_array[i].addr == 0x3300 && VSM_SIGNAL_IDLE_array[i].value == 0xA9C71555) {
                ms_delay(50);
            }
        }
    }
    return err;
}


vsm_status_t vsm_driver_init(void)
{
    vsm_status_t ret = VSM_STATUS_OK;
    hal_gpio_direction_t dir;
    hal_gpio_data_t gpio_data;

#ifdef MT2511_USE_SPI
    hal_spi_master_config_t spi_config;
    spi_config.clock_frequency = 1875000;
    spi_config.slave_port = HAL_SPI_MASTER_SLAVE_0;
    //spi_config.bit_order = HAL_SPI_MASTER_MSB_FIRST;
    spi_config.bit_order = HAL_SPI_MASTER_LSB_FIRST;
    spi_config.polarity = HAL_SPI_MASTER_CLOCK_POLARITY0;
    spi_config.phase = HAL_SPI_MASTER_CLOCK_PHASE0;
    vsm_spi_init(HAL_SPI_MASTER_3, &spi_config, HAL_SPI_MASTER_MACRO_GROUP_A);
#else
    i2c_init(HAL_I2C_MASTER_0, HAL_I2C_FREQUENCY_200K);
#endif
    combo_usage = 0;
    hal_gpio_init(GPIO_MT2511_AFE_PWD_PIN);
    hal_gpio_init(GPIO_MT2511_RST_PORT_PIN);
    hal_gpio_init(GPIO_MT2511_32K);
    hal_gpio_init(GPIO_MT2511_PPG_VDRV_EN);

    //turn on 32k
    hal_pinmux_set_function(GPIO_MT2511_32K, 2);
    
    //config power pin
    hal_pinmux_set_function(GPIO_MT2511_AFE_PWD_PIN, 0);

    hal_gpio_set_direction(GPIO_MT2511_AFE_PWD_PIN, HAL_GPIO_DIRECTION_OUTPUT);

    if (DBG) {
        LOGD("%s():hal_gpio_set_direction GPIO_MT2511_AFE_PWD_PIN ret ", __func__);
    }

    hal_gpio_get_direction(GPIO_MT2511_AFE_PWD_PIN, &dir);

    if (DBG) {
        LOGD("%s():GPIO_MT2511_AFE_PWD_PIN dir:%d,ret ", __func__, dir);
    }

    //config reset pin
    hal_pinmux_set_function(GPIO_MT2511_RST_PORT_PIN, 0);

    hal_gpio_set_direction(GPIO_MT2511_RST_PORT_PIN, HAL_GPIO_DIRECTION_OUTPUT);

    if (DBG) {
        LOGD("%s():hal_gpio_set_direction ret GPIO_MT2511_RST_PORT_PIN ", __func__);
    }

    hal_gpio_get_direction(GPIO_MT2511_RST_PORT_PIN, &dir);

    if (DBG) {
        LOGD("%s():GPIO_MT2511_RST_PORT_PIN dir:%d,ret ", __func__, dir);
    }

    //power down pin, 0:active, 1:inactive
    //high
    hal_gpio_set_output(GPIO_MT2511_AFE_PWD_PIN, HAL_GPIO_DATA_HIGH);
    if (DBG) {
        LOGD("%s():hal_gpio_set_direction ret GPIO_MT2511_AFE_PWD_PIN HAL_GPIO_DATA_HIGH ", __func__);
    }

    hal_gpio_get_output(GPIO_MT2511_AFE_PWD_PIN, &gpio_data);
    if (DBG) {
        LOGD("%s():GPIO_MT2511_AFE_PWD_PIN data:%d", __func__, gpio_data);
    }
    ms_delay(5);

    //reset pin, 1:active, 0:inactive
    //high
    hal_gpio_set_output(GPIO_MT2511_RST_PORT_PIN, HAL_GPIO_DATA_HIGH);
    if (DBG) {
        LOGD("%s():hal_gpio_set_direction ret GPIO_MT2511_RST_PORT_PIN HAL_GPIO_DATA_HIGH ", __func__);
    }

    hal_gpio_get_output(GPIO_MT2511_RST_PORT_PIN, &gpio_data);
    if (DBG) {
        LOGD("%s():GPIO_MT2511_RST_PORT_PIN data:%d", __func__, gpio_data);
    }
    ms_delay(15);
    
    // config PPG_VDRV_EN and only for customer project
    hal_pinmux_set_function(GPIO_MT2511_PPG_VDRV_EN, 0);
    hal_gpio_set_direction(GPIO_MT2511_PPG_VDRV_EN, HAL_GPIO_DIRECTION_OUTPUT);
    if (DBG) {
        LOGD("%s():hal_gpio_set_direction ret GPIO_MT2511_PPG_VDRV_EN ", __func__);
    }
    hal_gpio_get_direction(GPIO_MT2511_PPG_VDRV_EN, &dir);
    if (DBG) {
        LOGD("%s():GPIO_MT2511_PPG_VDRV_EN dir:%d", __func__, dir);
    }
    //config high
    hal_gpio_set_output(GPIO_MT2511_PPG_VDRV_EN, HAL_GPIO_DATA_HIGH);
    if (DBG) {
        LOGD("%s():hal_gpio_set_direction ret GPIO_MT2511_PPG_VDRV_EN HAL_GPIO_DATA_HIGH ", __func__);
    }    
    hal_gpio_get_output(GPIO_MT2511_PPG_VDRV_EN, &gpio_data);
    if (DBG) {
        LOGD("%s():GPIO_MT2511_PPG_VDRV_EN data:%d", __func__, gpio_data);
    }
    //delay 10ms
    ms_delay(5);
    

    //low
    hal_gpio_set_output(GPIO_MT2511_RST_PORT_PIN, HAL_GPIO_DATA_LOW);
    if (DBG) {
        LOGD("%s():hal_gpio_set_direction ret GPIO_MT2511_RST_PORT_PIN HAL_GPIO_DATA_LOW ", __func__);
    }

    hal_gpio_get_output(GPIO_MT2511_RST_PORT_PIN, &gpio_data);
    if (DBG) {
        LOGD("%s():GPIO_MT2511_RST_PORT_PIN data:%d", __func__, gpio_data);
    } 
    //delay 5ms
    ms_delay(5);

    //high
    hal_gpio_set_output(GPIO_MT2511_RST_PORT_PIN, HAL_GPIO_DATA_HIGH);
    if (DBG) {
        LOGD("%s():hal_gpio_set_direction ret GPIO_MT2511_RST_PORT_PIN HAL_GPIO_DATA_HIGH ", __func__);
    }

    hal_gpio_get_output(GPIO_MT2511_RST_PORT_PIN, &gpio_data);
    if (DBG) {
        LOGD("%s():GPIO_MT2511_RST_PORT_PIN data:%d", __func__, gpio_data);
    }
    
    ms_delay(5);

    //low
    //hw_gpio_configure_pin(GPIO_MT2511_PORT, GPIO_MT2511_AFE_PWD_PIN,
    //						HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, 0);
    hal_gpio_set_output(GPIO_MT2511_AFE_PWD_PIN, HAL_GPIO_DATA_LOW);
    if (DBG) {
        LOGD("%s():hal_gpio_set_direction ret GPIO_MT2511_AFE_PWD_PIN HAL_GPIO_DATA_LOW ", __func__);
    }

    hal_gpio_get_output(GPIO_MT2511_AFE_PWD_PIN, &gpio_data);
    if (DBG) {
        LOGD("%s():GPIO_MT2511_AFE_PWD_PIN data:%d", __func__, gpio_data);
    }
    ms_delay(45);
    //*****************************************************************************    

/*check if speed up to 400KHz*/
#ifdef MT2511_USE_I2C
    if (vsm_driver_chip_version_get() == CHIP_VERSION_E2) {
        if (vsm_driver_upgrade_i2c() != VSM_STATUS_OK) {
            LOGE("%s():upgrade i2c fail", __func__);
        } else {
            i2c_init(HAL_I2C_MASTER_0, HAL_I2C_FREQUENCY_400K);
        }
    }
#endif
    ret = vsm_driver_idle();
    if (ret != VSM_STATUS_OK) {
        vsm_init_driver = false;
    }

    if (DBG) {
        LOGI("%s():--", __func__);
    }
    return ret;
}

vsm_status_t vsm_driver_deinit()
{
	vsm_status_t err = VSM_STATUS_OK;

	//power pin, 0:active, 1:inactive
	//high
	hal_gpio_set_output(GPIO_MT2511_AFE_PWD_PIN, HAL_GPIO_DATA_HIGH);
	ms_delay(25);
	

    //reset pin, 1:active, 0:inactive
	//high
	hal_gpio_set_output(GPIO_MT2511_RST_PORT_PIN, HAL_GPIO_DATA_LOW);
    hal_gpio_set_output(GPIO_MT2511_AFE_PWD_PIN, HAL_GPIO_DATA_LOW);
    
	hal_gpio_deinit(GPIO_MT2511_RST_PORT_PIN);
    hal_gpio_deinit(GPIO_MT2511_AFE_PWD_PIN);
	return err;
}


