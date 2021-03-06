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

#include "hal_i2c_master.h"
#if defined(HAL_I2C_MASTER_MODULE_ENABLED)
#include <stddef.h>
#include <string.h>
#include "i2c.h"
#include "mt7687.h"
#include "core_cm4.h"
#include "dma_sw.h"
#include "hal_log.h"
#include "hal_I2C.h"
#include "nvic.h"
#include "hal_nvic.h"


#ifdef HAL_SLEEP_MANAGER_ENABLED
#include "hal_sleep_manager.h"
#include "hal_sleep_driver.h"

const static char *i2c_lock_sleep_name[HAL_I2C_MASTER_MAX] = {"I2C0", "I2C1"};
static uint8_t i2c_lock_sleep_handle[HAL_I2C_MASTER_MAX];
#define I2C_GET_SLEEP_HANDLE(i2c_port) \
do{ \
    i2c_lock_sleep_handle[i2c_port] = hal_sleep_manager_set_sleep_handle(i2c_lock_sleep_name[i2c_port]); \
    if(i2c_lock_sleep_handle[i2c_port] == INVALID_SLEEP_HANDLE ) { \
        log_hal_error("[I2C%d][init]:get sleep handle failed\r\n",i2c_port); \
        return HAL_I2C_STATUS_ERROR; \
    } \
}while(0)

#define I2C_RELEASE_SLEEP_HANDLE(i2c_port) \
do{ \
    hal_sleep_manager_release_sleep_handle(i2c_lock_sleep_handle[i2c_port]); \
}while(0)

#define I2C_LOCK_SLEEP(i2c_port) \
do{ \
    hal_sleep_manager_lock_sleep(i2c_lock_sleep_handle[i2c_port]); \
}while(0)

#define I2C_UNLOCK_SLEEP(i2c_port) \
do{ \
    hal_sleep_manager_unlock_sleep(i2c_lock_sleep_handle[i2c_port]); \
}while(0)

#else
#define I2C_GET_SLEEP_HANDLE(i2c_port)
#define I2C_RELEASE_SLEEP_HANDLE(i2c_port)
#define I2C_LOCK_SLEEP(i2c_port)
#define I2C_UNLOCK_SLEEP(i2c_port)
#endif


typedef struct {
    bool basic_config_present;
    hal_i2c_config_t basic_config;

    bool callback_present;
    hal_i2c_callback_t callback;
    void *user_data;
} hal_i2c_inner_config_t;

typedef struct {
    uint8_t slave_address;
    hal_i2c_port_t master_port;
} hal_i2c_current_user_t;

typedef struct {
    hal_i2c_current_user_t current_user;
    hal_i2c_inner_config_t config[HAL_I2C_MASTER_MAX];
} hal_i2c_context_t;

static hal_i2c_context_t g_i2c_context;
volatile static uint8_t s_i2c_master_status[HAL_I2C_MASTER_MAX] = {0};


extern uint32_t save_and_set_interrupt_mask(void);
extern void restore_interrupt_mask(uint32_t mask);



static inline bool hal_i2c_is_port_valid(hal_i2c_port_t port)
{
    return port < HAL_I2C_MASTER_MAX;
}

static inline uint8_t hal_i2c_translate_port(hal_i2c_port_t port)
{
    return (HAL_I2C_MASTER_0 == port) ? 0 : 1;
}

static inline uint8_t hal_i2c_translate_frequency(hal_i2c_frequency_t frequency)
{
    return (uint8_t)frequency;
}

static inline uint32_t hal_i2c_index_irq(hal_i2c_port_t port)
{
    return (HAL_I2C_MASTER_0 == port) ? CM4_I2C1_IRQ : CM4_I2C2_IRQ;
}

static void hal_i2c_callback(hal_nvic_irq_t irq_number)
{
    hal_i2c_port_t current_port = g_i2c_context.current_user.master_port;
    uint8_t current_slave_address = g_i2c_context.current_user.slave_address;
    hal_i2c_callback_t callback = g_i2c_context.config[current_port].callback;
    void *parameter = g_i2c_context.config[current_port].user_data;
    if (g_i2c_context.config[current_port].callback_present && NULL != callback) {
        callback(current_slave_address, HAL_I2C_EVENT_SUCCESS, parameter);
    }

    /* unlock sleep */
    I2C_UNLOCK_SLEEP(current_port);
}

static inline bool hal_i2c_is_frequency_valid(hal_i2c_frequency_t frequency)
{
    return frequency < HAL_I2C_FREQUENCY_MAX;
}

static inline bool hal_i2c_is_config_valid(const hal_i2c_config_t *config)
{
    if (NULL == config) {
        return false;
    }
    if (!hal_i2c_is_frequency_valid(config->frequency)) {
        return false;
    }
    return true;
}

hal_i2c_status_t hal_i2c_master_init(hal_i2c_port_t i2c_port, hal_i2c_config_t *i2c_config)
{
    hal_i2c_status_t busy_status;
    if (!hal_i2c_is_port_valid(i2c_port)) {
        log_hal_error("Wrong I2C port: %d", i2c_port);
        return HAL_I2C_STATUS_INVALID_PORT_NUMBER;
    }
    if (!hal_i2c_is_config_valid(i2c_config)) {
        log_hal_error("Config is invalid");
        return HAL_I2C_STATUS_INVALID_PARAMETER;
    }

    i2c_check_and_set_busy(i2c_port, busy_status);
    if (HAL_I2C_STATUS_ERROR_BUSY == busy_status) {
        return HAL_I2C_STATUS_ERROR_BUSY;
    }

    memset(&g_i2c_context.config[i2c_port], 0, sizeof(hal_i2c_inner_config_t));
    g_i2c_context.config[i2c_port].basic_config_present = true;
    g_i2c_context.config[i2c_port].basic_config.frequency = i2c_config->frequency;

    int32_t ret_val = i2c_configure(hal_i2c_translate_port(i2c_port), hal_i2c_translate_frequency(i2c_config->frequency));
    if (ret_val < 0) {
        log_hal_error("i2c_configure failed.");
        return HAL_I2C_STATUS_ERROR;
    }
    ret_val = i2c_enable(hal_i2c_translate_port(i2c_port));
    if (ret_val < 0) {
        log_hal_error("i2c_enable failed.");
        return HAL_I2C_STATUS_ERROR;
    }

    /*get sleep handler*/
    I2C_GET_SLEEP_HANDLE(i2c_port);


    return HAL_I2C_STATUS_OK;
}

hal_i2c_status_t hal_i2c_master_deinit(hal_i2c_port_t i2c_port)
{
    if (!hal_i2c_is_port_valid(i2c_port)) {
        log_hal_error("Wrong I2C port: %d", i2c_port);
        return HAL_I2C_STATUS_INVALID_PORT_NUMBER;
    }

    memset(&g_i2c_context.config[i2c_port], 0, sizeof(hal_i2c_inner_config_t));

    /*release sleep hander*/
    I2C_RELEASE_SLEEP_HANDLE(i2c_port);
    /* unlock i2c */
    i2c_set_idle(i2c_port);
    return HAL_I2C_STATUS_OK;
}

hal_i2c_status_t hal_i2c_master_register_callback(hal_i2c_port_t i2c_port, hal_i2c_callback_t i2c_callback, void *user_data)
{
    if (!hal_i2c_is_port_valid(i2c_port)) {
        log_hal_error("Wrong I2C port: %d", i2c_port);
        return HAL_I2C_STATUS_INVALID_PORT_NUMBER;
    }
    g_i2c_context.config[i2c_port].callback_present = true;
    g_i2c_context.config[i2c_port].callback = i2c_callback;
    g_i2c_context.config[i2c_port].user_data = user_data;
    hal_nvic_register_isr_handler((hal_nvic_irq_t)hal_i2c_index_irq(i2c_port), hal_i2c_callback);
    NVIC_SetPriority((IRQn_Type)hal_i2c_index_irq(i2c_port), (uint32_t)DEFAULT_PRI);
    NVIC_EnableIRQ((IRQn_Type)hal_i2c_index_irq(i2c_port));
    return HAL_I2C_STATUS_OK;
}

hal_i2c_status_t hal_i2c_master_send_dma(hal_i2c_port_t i2c_port, uint8_t slave_address, const uint8_t *data, uint32_t size)
{
    if (!hal_i2c_is_port_valid(i2c_port)) {
        log_hal_error("Wrong I2C port: %d", i2c_port);
        return HAL_I2C_STATUS_INVALID_PORT_NUMBER;
    }

    if (1 == halI2CGetBusyStatus(hal_i2c_translate_port(i2c_port))) {
        log_hal_error("[I2C]send_dma busy\r\n");
        return HAL_I2C_STATUS_ERROR_BUSY;
    }
    /* lock sleep */
    I2C_LOCK_SLEEP(i2c_port);

    g_i2c_context.current_user.master_port = i2c_port;
    g_i2c_context.current_user.slave_address = slave_address;
    int32_t ret_val = i2c_write(hal_i2c_translate_port(i2c_port), slave_address, data, (uint16_t)size);
    return (ret_val >= 0) ? HAL_I2C_STATUS_OK : HAL_I2C_STATUS_ERROR;
}

hal_i2c_status_t hal_i2c_master_receive_dma(hal_i2c_port_t i2c_port, uint8_t slave_address, uint8_t *buffer, uint32_t size)
{
    if (!hal_i2c_is_port_valid(i2c_port)) {
        log_hal_error("Wrong I2C port: %d", i2c_port);
        return HAL_I2C_STATUS_INVALID_PORT_NUMBER;
    }

    if (1 == halI2CGetBusyStatus(hal_i2c_translate_port(i2c_port))) {
        log_hal_error("[I2C]read_dma busy\r\n");
        return HAL_I2C_STATUS_ERROR_BUSY;
    }

    /* lock sleep */
    I2C_LOCK_SLEEP(i2c_port);

    g_i2c_context.current_user.master_port = i2c_port;
    g_i2c_context.current_user.slave_address = slave_address;
    int32_t ret_val = i2c_read(hal_i2c_translate_port(i2c_port), slave_address, buffer, (uint16_t)size);
    return (ret_val >= 0) ? HAL_I2C_STATUS_OK : HAL_I2C_STATUS_ERROR;
}

hal_i2c_status_t hal_i2c_master_send_polling(hal_i2c_port_t i2c_port, uint8_t slave_address, const uint8_t *data, uint32_t size)
{
    if (!hal_i2c_is_port_valid(i2c_port)) {
        log_hal_error("Wrong I2C port: %d", i2c_port);
        return HAL_I2C_STATUS_INVALID_PORT_NUMBER;
    }
    if (I2C_FIFO_MAX_LEN < size) {
        log_hal_error("[I2C]send_polling length error\r\n");
        return HAL_I2C_STATUS_INVALID_PARAMETER;
    }

    if (1 == halI2CGetBusyStatus(hal_i2c_translate_port(i2c_port))) {
        log_hal_error("[I2C]send_polling busy\r\n");
        return HAL_I2C_STATUS_ERROR_BUSY;
    }
    /* lock sleep */
    I2C_LOCK_SLEEP(i2c_port);

    g_i2c_context.current_user.master_port = i2c_port;
    g_i2c_context.current_user.slave_address = slave_address;
    int32_t ret_val = i2c_write_via_mcu(hal_i2c_translate_port(i2c_port), slave_address, data, (uint16_t)size);
    /* unlock sleep */
    I2C_UNLOCK_SLEEP(i2c_port);

    return (ret_val >= 0) ? HAL_I2C_STATUS_OK : HAL_I2C_STATUS_ERROR;
}

hal_i2c_status_t hal_i2c_master_receive_polling(hal_i2c_port_t i2c_port, uint8_t slave_address, uint8_t *buffer, uint32_t size)
{
    if (!hal_i2c_is_port_valid(i2c_port)) {
        log_hal_error("Wrong I2C port: %d", i2c_port);
        return HAL_I2C_STATUS_INVALID_PORT_NUMBER;
    }

    if (I2C_FIFO_MAX_LEN < size) {
        log_hal_error("[I2C]read_polling length error\r\n");
        return HAL_I2C_STATUS_INVALID_PARAMETER;
    }

    if (1 == halI2CGetBusyStatus(hal_i2c_translate_port(i2c_port))) {
        log_hal_error("[I2C]read_polling busy\r\n");
        return HAL_I2C_STATUS_ERROR_BUSY;
    }
    /* lock sleep */
    I2C_LOCK_SLEEP(i2c_port);

    g_i2c_context.current_user.master_port = i2c_port;
    g_i2c_context.current_user.slave_address = slave_address;
    int32_t ret_val = i2c_read_via_mcu(hal_i2c_translate_port(i2c_port), slave_address, buffer, (uint16_t)size);

    /* unlock sleep */
    I2C_UNLOCK_SLEEP(i2c_port);


    return (ret_val >= 0) ? HAL_I2C_STATUS_OK : HAL_I2C_STATUS_ERROR;
}

hal_i2c_status_t hal_i2c_master_set_frequency(hal_i2c_port_t i2c_port, hal_i2c_frequency_t frequency)
{
    if (!hal_i2c_is_port_valid(i2c_port)) {
        log_hal_error("Wrong I2C port: %d", i2c_port);
        return HAL_I2C_STATUS_INVALID_PORT_NUMBER;
    }
    if (!hal_i2c_is_frequency_valid(frequency)) {
        log_hal_error("Wrong frequency: %d", frequency);
        return HAL_I2C_STATUS_INVALID_PARAMETER;
    }

    if (1 == halI2CGetBusyStatus(hal_i2c_translate_port(i2c_port))) {
        return HAL_I2C_STATUS_ERROR_BUSY;
    }

    int32_t ret_val = i2c_configure(hal_i2c_translate_port(i2c_port), hal_i2c_translate_frequency(frequency));
    return (ret_val >= 0) ? HAL_I2C_STATUS_OK : HAL_I2C_STATUS_ERROR;
}

hal_i2c_status_t hal_i2c_master_get_running_status(hal_i2c_port_t i2c_port, hal_i2c_running_status_t *running_status)
{
    /* parameter check */
    if (HAL_I2C_MASTER_MAX <= i2c_port) {
        return HAL_I2C_STATUS_INVALID_PORT_NUMBER;
    }
    running_status->running_status = (hal_i2c_running_type_t)halI2CGetBusyStatus(hal_i2c_translate_port(i2c_port));
    return HAL_I2C_STATUS_OK;
}

#endif
