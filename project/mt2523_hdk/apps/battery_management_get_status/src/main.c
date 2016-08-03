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
#include "queue.h"
/* device.h includes */
#include "mt2523.h"

/* hal includes */
#include "hal.h"
#include "hal_charger.h"
#include "hal_sleep_manager.h"
#include "hal_gpt.h"
#include "cust_battery_meter.h"

/* system init */
#include "sys_init.h"

#include "battery_message.h"

/* battery management includes */
#include "battery_management.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void vTestTask(void *pvParameters);

/* Create the log control block for battery management demo module.
 * The initialization of the log is in the sys_init.c.
 * Please refer to the log dev guide under /doc folder for more details.
 */
log_create_module(bmt_demo, PRINT_LEVEL_INFO);

/**
* @brief       This function is use to get the battery information.
* @return      None.
*/
void get_battery_information(void)
{
    int32_t capacity, charger_current, charger_status, charger_type, battery_temperature, battery_volt, capacity_level;

    capacity = battery_management_get_battery_property(BATTERY_PROPERTY_CAPACITY);
    capacity_level = battery_management_get_battery_property(BATTERY_PROPERTY_CAPACITY_LEVEL);
    charger_current = battery_management_get_battery_property(BATTERY_PROPERTY_CHARGING_CURRENT);
    charger_status = battery_management_get_battery_property(BATTERY_PROPERTY_CHARGER_EXIST);
    charger_type = battery_management_get_battery_property(BATTERY_PROPERTY_CHARGER_TYPE);
    battery_temperature = battery_management_get_battery_property(BATTERY_PROPERTY_TEMPERATURE);
    battery_volt = battery_management_get_battery_property(BATTERY_PROPERTY_VOLTAGE);

    if (charger_status) {
        LOG_I(bmt_demo, "Battery capacity = %d(%%)", capacity);
        LOG_I(bmt_demo, "Battery capacity level = %d", capacity_level);
        LOG_I(bmt_demo, "Charger current = %d(mA)", charger_current);
        LOG_I(bmt_demo, "Charger status = %d(0:charge not exist; 1:charger exist)", charger_status);
        LOG_I(bmt_demo, "Charger type = %d(0:unknown; 1:standard host; 2:non-standard AC charger; 3:standard AC charger 4:charging host)", charger_type);
        LOG_I(bmt_demo, "Battery temperature = %d(Celsius degrees)", battery_temperature);
        LOG_I(bmt_demo, "Battery voltage = %d(mV)\n", battery_volt);
    } else {
        LOG_I(bmt_demo, "Battery capacity = %d(%%)", capacity);
        LOG_I(bmt_demo, "Battery capacity level = %d", capacity_level);
        LOG_I(bmt_demo, "Battery temperature = %d(Celsius degrees)", battery_temperature);
        LOG_I(bmt_demo, "Battery voltage = %d(mV)\n", battery_volt);
    }

}

/**
* @brief       This function is battery management message handler.
* @param[in]   message: The message should be process.
* @return      None.
*/
static void battery_management_message_handler(battery_message_context_t *message)
{
    LOG_I(bmt_demo, "battery_management_message_handler event  = 0x%X", (int) message->event);

    switch (message->event) {
        case BATTERY_MANAGEMENT_EVENT_BATTERY_UPDATE: {

            get_battery_information();

        }
        break;
        default: {

        }
        break;
    }

}

/**
* @brief       Task main function
* @param[in]   pvParameters: Pointer that will be used as the parameter for the task being created.
* @return      None.
*/
static void vTestTask(void *pvParameters)
{
    uint32_t handle;
    battery_message_context_t message;

    battery_message_allocate(&handle);

    while (1) {
        if (battery_message_receive(handle, &message)) {

            battery_management_message_handler(&message);
        }
    }
}

/**
* @brief       This function is to pre-charging battery and check the battery voltage.
* @return      None.
*/
static void check_battery_voltage(void)
{
    /* Check power on battery voltage and pre-charging battery sample code */
    hal_charger_init();
    while (1) {
        int32_t battery_voltage = 0;
        bool charger_status;

        hal_charger_meter_get_battery_voltage_sense(&battery_voltage);
        LOG_I(bmt_demo, "Check battery_voltage = %d mV \n\r", battery_voltage);
        /* Check battery voltage  > SHUTDOWN_SYSTEM_VOLTAGE (3400mV)  + 100mV */
        if (battery_voltage >= SHUTDOWN_SYSTEM_VOLTAGE + 100) {
            break;
        } else {
            hal_charger_get_charger_detect_status(&charger_status);
            if (charger_status == true) {
                LOG_I(bmt_demo, "SW charging battery_voltage = %d mV \n\r", battery_voltage);
                hal_charger_init();
                /* Setup pre-charging current. It depends on the battery specifications */
                hal_charger_set_charging_current(HAL_CHARGE_CURRENT_70_MA);
                hal_charger_enable(true);
                /* Reset watchdog timer */
                hal_charger_reset_watchdog_timer();
                hal_gpt_delay_ms(1 * 1000);
            } else {
                LOG_I(bmt_demo, "Low battery power off !! battery_voltage = %d mV \n\r", battery_voltage);
                hal_sleep_manager_enter_power_off_mode();
            }

        }
    }

}

int main(void)
{
    battery_management_status_t status;

    system_init();

    /* Check battery voltage */
    check_battery_voltage();

    /* Initialize battery management service, as early as possible */
    status = battery_management_init();
    if (status != BATTERY_MANAGEMENT_STATUS_OK) {
        LOG_E(bmt_demo, "battery_management_init fail!! status = %d\n\r", (int)status);
    }

    LOG_I(bmt_demo, "battery_management test main!!\n\r");

    /* Create task */
    xTaskCreate(vTestTask, "battery_monitor_task", 200, NULL, 4, NULL);


    /* Start the scheduler. */
    vTaskStartScheduler();

    /* If all is well, the scheduler will now be running, and the following line
    will never be reached.  If the following line does execute, then there was
    insufficient FreeRTOS heap memory available for the idle and/or timer tasks
    to be created.  See the memory management section on the FreeRTOS web site
    for more details. */
    for (;;);

}

