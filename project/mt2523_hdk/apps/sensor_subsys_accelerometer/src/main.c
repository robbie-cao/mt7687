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

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

/* device.h includes */
#include "mt2523.h"

/* hal includes */
#include "hal_uart.h"
#include "hal_clock.h"

#include "hal_flash.h"

/* system init */
#include "sys_init.h"

/**
* @brief Callback function by each tick interrupt according to configUSE_TICK_HOOK in FreeRTOSConfig.h.
* @return None.\n
*/
void vApplicationTickHook(void)
{

}

#include "hal_eint.h"
#include "hal_i2c_master.h"
#include "hal_spi_master.h"

/* sensor subsys includes */
#include "mems_bus.h"
#include "sensor_manager.h" /* sensor manager task */
#include "sensor_alg_interface.h"

#ifdef MTK_SENSOR_ACCELEROMETER_GYROSCOPE_USE_BMI160
#include "bmi160.h"
#endif

#ifdef MTK_SENSOR_PROXIMITY_USE_CM36672
#include "cm36672.h"
#endif

#ifdef MTK_SENSOR_ACCELEROMETER_USE_BMA255
#include "bma255_sensor_adaptor.h"
#endif

//#define SENSORS_ON_EVB

/* pin mux setting of EVB */
#ifdef SENSORS_ON_EVB
#include "hal_gpio.h"

/**
* @brief This funciton initializes the peripherals for sensors.
* @return None.\n
*/
void sensor_peripheral_init()
{
    printf("\n\r sensorhub_peripheral_init ++\n\r");

    /* 1. Config SPI3 */
    hal_pinmux_set_function(HAL_GPIO_32, HAL_GPIO_32_MA_SPI3_B_CS);
    hal_pinmux_set_function(HAL_GPIO_33, HAL_GPIO_33_MA_SPI3_B_SCK);
    hal_pinmux_set_function(HAL_GPIO_34, HAL_GPIO_34_MA_SPI3_B_MOSI);
    hal_pinmux_set_function(HAL_GPIO_35, HAL_GPIO_35_MA_SPI3_B_MISO);

    hal_spi_master_config_t spi_config;
    spi_config.clock_frequency = 0x400000;
    spi_config.slave_port = HAL_SPI_MASTER_SLAVE_0;
    spi_config.bit_order = HAL_SPI_MASTER_MSB_FIRST;
    spi_config.polarity = HAL_SPI_MASTER_CLOCK_POLARITY0;
    spi_config.phase = HAL_SPI_MASTER_CLOCK_PHASE0;

    mems_spi_init(SPI_SELECT_BMI160, HAL_SPI_MASTER_3, &spi_config, HAL_SPI_MASTER_MACRO_GROUP_B);

    /* 2. Config I2C0 */
    mems_i2c_init(HAL_I2C_MASTER_0, HAL_I2C_FREQUENCY_400K);

    /*3. Config EINT*/
  #if  defined(MTK_SENSOR_ACCELEROMETER_GYROSCOPE_USE_BMI160) || defined(MTK_SENSOR_ACCELEROMETER_USE_BMA255)
    /* GPIO[14] function1 for EINT12, BMI160 */
    hal_pinmux_set_function(HAL_GPIO_14, HAL_GPIO_14_EINT12);
    hal_eint_config_t eint1_config;
    eint1_config.trigger_mode = HAL_EINT_EDGE_RISING;
    eint1_config.debounce_time = 1;
    hal_eint_init(HAL_EINT_NUMBER_12, &eint1_config);

#ifdef MTK_SENSOR_ACCELEROMETER_GYROSCOPE_USE_BMI160
    bmi160_eint_set(HAL_EINT_NUMBER_12);
    hal_eint_register_callback(HAL_EINT_NUMBER_12, bmi160_eint_handler, NULL);
#endif

#ifdef MTK_SENSOR_ACCELEROMETER_USE_BMA255
    bma255_eint_set(HAL_EINT_NUMBER_12);
    hal_eint_register_callback(HAL_EINT_NUMBER_12, bma255_eint_handler, NULL);
#endif

#ifdef HAL_EINT_FEATURE_MASK
    hal_eint_unmask(HAL_EINT_NUMBER_12);
#endif
  #endif

  #ifdef MTK_SENSOR_PROXIMITY_USE_CM36672
    /* GPIO[11] function1 for EINT9, PS_EINT */
    hal_pinmux_set_function(HAL_GPIO_11, HAL_GPIO_11_EINT9);
    hal_eint_config_t eint3_config;
    eint3_config.trigger_mode = HAL_EINT_EDGE_FALLING;
    eint3_config.debounce_time = 1;
    cm36672_eint_set(HAL_EINT_NUMBER_9);
    hal_eint_init(HAL_EINT_NUMBER_9, &eint3_config);
    hal_eint_register_callback(HAL_EINT_NUMBER_9, cm36672_eint_handler, NULL);
#ifdef HAL_EINT_FEATURE_MASK
    hal_eint_unmask(HAL_EINT_NUMBER_9);
#endif
  #endif
    printf("\n\r sensorhub_peripheral_init --\n\r");
}

/* pin mux setting of HDK */
#else
void sensor_peripheral_init()
{
    printf("\n\r sensorhub_peripheral_init ++\n\r");

    /* 1. Config SPI3 */
    hal_spi_master_config_t spi_config;
    spi_config.clock_frequency = 0x400000;
    spi_config.slave_port = HAL_SPI_MASTER_SLAVE_0;
    spi_config.bit_order = HAL_SPI_MASTER_MSB_FIRST;
    spi_config.polarity = HAL_SPI_MASTER_CLOCK_POLARITY0;
    spi_config.phase = HAL_SPI_MASTER_CLOCK_PHASE0;

    mems_spi_init(SPI_SELECT_BMI160, HAL_SPI_MASTER_3, &spi_config, HAL_SPI_MASTER_MACRO_GROUP_A);

    /* 2. Config I2C2 */
    mems_i2c_init(HAL_I2C_MASTER_0, HAL_I2C_FREQUENCY_400K);

    /*3. Config EINT*/
  #if defined(MTK_SENSOR_ACCELEROMETER_GYROSCOPE_USE_BMI160)  || defined(MTK_SENSOR_ACCELEROMETER_USE_BMA255)
    /* GPIO[5] function1 for EINT4, BMI160 */
    hal_eint_config_t eint1_config;
    eint1_config.trigger_mode = HAL_EINT_EDGE_RISING;
    eint1_config.debounce_time = 1;
    hal_eint_init(HAL_EINT_NUMBER_4, &eint1_config);

#ifdef MTK_SENSOR_ACCELEROMETER_GYROSCOPE_USE_BMI160
    bmi160_eint_set(HAL_EINT_NUMBER_4);
    hal_eint_register_callback(HAL_EINT_NUMBER_4, bmi160_eint_handler, NULL);
#endif

#ifdef MTK_SENSOR_ACCELEROMETER_USE_BMA255
    bma255_eint_set(HAL_EINT_NUMBER_4);
    hal_eint_register_callback(HAL_EINT_NUMBER_4, bma255_eint_handler, NULL);
#endif

#ifdef HAL_EINT_FEATURE_MASK
    hal_eint_unmask(HAL_EINT_NUMBER_4);
#endif
  #endif

  #ifdef MTK_SENSOR_PROXIMITY_USE_CM36672
    /* GPIO[6] function1 for EINT5, PS_EINT */
    hal_eint_config_t eint3_config;
    eint3_config.trigger_mode = HAL_EINT_EDGE_FALLING;
    eint3_config.debounce_time = 1;
    cm36672_eint_set(HAL_EINT_NUMBER_5);
    hal_eint_init(HAL_EINT_NUMBER_5, &eint3_config);
    hal_eint_register_callback(HAL_EINT_NUMBER_5, cm36672_eint_handler, NULL);
#ifdef HAL_EINT_FEATURE_MASK
    hal_eint_unmask(HAL_EINT_NUMBER_5);
#endif
  #endif
    printf("\n\r sensorhub_peripheral_init --\n\r");
}
#endif

static TaskHandle_t sensor_task_handle;
int32_t acc_sensor_send_digest_callback(sensor_data_t *const output);

sensor_subscriber_t aacc_subscriber = {
    "ap0", 0, SENSOR_TYPE_ACCELEROMETER, 1000, acc_sensor_send_digest_callback
};

/**
* @brief The callback function for accerometer data.
* @param[in] output, the accerometer data.
* @return 0, if the operation completed successfully.\n
*/
int32_t acc_sensor_send_digest_callback(sensor_data_t *const output)
{
    switch (output->data[0].sensor_type) {

        case SENSOR_TYPE_ACCELEROMETER:
            printf("acc sensor type = %lu , value = ( %ld , %ld , %ld ) (%ld) , timestamp = %lu \r\n",
                   output->data[0].sensor_type,
                   output->data[0].accelerometer_t.x,
                   output->data[0].accelerometer_t.y,
                   output->data[0].accelerometer_t.z,
                   output->data[0].accelerometer_t.status,
                   output->data[0].time_stamp
                  );
            break;

        default:
            printf("type = %lu,  timestamp = %lu \r\n",
                   output->data[0].sensor_type,
                   output->data[0].time_stamp);
            break;
    }
    return 0;
}

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
    system_init();

    int idx = 0;
    sensor_manager_init();
    int32_t subscription;
    subscription = sensor_subscribe_sensor(&aacc_subscriber);
    if (subscription < 0) {
        printf("acc subscription fail\r\b");
    }
    xTaskCreate( sensor_mgr_task, "SensorMgr", 2048, (void *)idx, 3, &sensor_task_handle);

    /* Start the scheduler. */
    vTaskStartScheduler();

    /* If all is well, the scheduler will now be running, and the following line
    will never be reached.  If the following line does execute, then there was
    insufficient FreeRTOS heap memory available for the idle and/or timer tasks
    to be created.  See the memory management section on the FreeRTOS web site
    for more details. */
    for ( ;; );
}

