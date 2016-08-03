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

#include "hal_eint.h"
#ifdef HAL_EINT_MODULE_ENABLED

#include <string.h>
#include "hal_eint.h"
#include "hal_gpio.h"
#include "hal_log.h"
#include "low_hal_gpio.h"
#include "hal_gpio_7687.h"
#include "nvic.h"

typedef struct {
    bool callback_present;
    hal_eint_callback_t callback;
    void *user_data;
} hal_eint_callback_context_t;


#define HAL_EINT_COUNT (11)

static hal_eint_callback_context_t g_eint_callback_context[HAL_EINT_COUNT];

typedef struct {
    hal_eint_number_t eint_number;
    ENUM_IOT_GPIO_NAME_T gpio_pin;
} hal_eint_gpio_map_t;


static const hal_eint_gpio_map_t eint_gpio_table[] = {
    {HAL_EINT_NUMBER_0,  eIOT_GPIO_IDX_0},
    {HAL_EINT_NUMBER_1,  eIOT_GPIO_IDX_1},
    {HAL_EINT_NUMBER_2,  eIOT_GPIO_IDX_3},
    {HAL_EINT_NUMBER_3,  eIOT_GPIO_IDX_4},
    {HAL_EINT_NUMBER_4,  eIOT_GPIO_IDX_5},
    {HAL_EINT_NUMBER_5,  eIOT_GPIO_IDX_6},
    {HAL_EINT_NUMBER_6,  eIOT_GPIO_IDX_7},
    {HAL_EINT_NUMBER_19, eIOT_GPIO_IDX_35},
    {HAL_EINT_NUMBER_20, eIOT_GPIO_IDX_37},
    {HAL_EINT_NUMBER_21, eIOT_GPIO_IDX_38},
    {HAL_EINT_NUMBER_22, eIOT_GPIO_IDX_39}
};

static uint32_t get_index_from_gpio_pin(ENUM_IOT_GPIO_NAME_T gpio_pin)
{
    uint32_t count = sizeof(eint_gpio_table) / sizeof(eint_gpio_table[0]);
    uint32_t index = 0;
    for (index = 0; index < count; index++) {
        if (eint_gpio_table[index].gpio_pin == gpio_pin) {
            return index;
        }
    }
    return (0xFFFFFFFF);
}

static int32_t hal_eint_convert_for_gpio(hal_eint_number_t eint_number, ENUM_IOT_GPIO_NAME_T *gpio_pin)
{
    uint32_t count = sizeof(eint_gpio_table) / sizeof(eint_gpio_table[0]);
    uint32_t index = 0;
    for (index = 0; index < count; index++) {
        if (eint_gpio_table[index].eint_number == eint_number) {
            *gpio_pin = eint_gpio_table[index].gpio_pin;
            return 0;
        }
    }
    return -1;
}

static void hal_eint_calc_debounce_parameters(uint32_t debounce_time,
        ENUM_DEBOUNCE_PRESCALER_T *sample_rate,
        uint32_t *sample_count)
{
    /*
        We prefer a higher sample rate for debouncing, i.e. check a number of times
        to make sure any interference to be detected and filtered.
        But checking too many times doesn't help much, so a threshold is defined,
        to scale among different sample rates.
    */
    uint32_t const threshold = 10;
    uint32_t target_count = 0;
    if ((target_count = debounce_time * 8) <= threshold) {
        *sample_rate = eDebounce_Prescaler_8K;
    } else if ((target_count = debounce_time * 4) <= threshold) {
        *sample_rate = eDebounce_Prescaler_4K;
    } else if ((target_count = debounce_time * 2) <= threshold) {
        *sample_rate = eDebounce_Prescaler_2K;
    } else if ((target_count = debounce_time * 1) <= threshold) {
        *sample_rate = eDebounce_Prescaler_1K;
    } else if ((target_count = debounce_time / 2) <= threshold) {
        *sample_rate = eDebounce_Prescaler_Dot5K;
    } else if ((target_count = debounce_time / 4) <= threshold) {
        *sample_rate = eDebounce_Prescaler_Dot25K;
    } else if ((target_count = debounce_time / 8) <= threshold) {
        *sample_rate = eDebounce_Prescaler_Dot125K;
    } else {
        *sample_rate = eDebounce_Prescaler_Dot0625K;
        *sample_count = debounce_time / 16;
    }
    *sample_count = target_count;
}

static void hal_eint_isr(ENUM_IOT_GPIO_NAME_T gpio_name)
{
    hal_eint_callback_context_t *callback_context = &g_eint_callback_context[get_index_from_gpio_pin(gpio_name)];
    if ((true == callback_context->callback_present)
            && (NULL != callback_context->callback)) {
        callback_context->callback(callback_context->user_data);
    }
}

static hal_eint_status_t hal_eint_apply_config(ENUM_IOT_GPIO_NAME_T gpio_pin,
        const hal_eint_config_t *eint_config)
{
    uint32_t sample_count;
    ENUM_NVIC_SENSE_T trigger_mode;
    ENUM_DEBOUNCE_POL_T trigger_polarity;
    ENUM_DEBOUNCE_PRESCALER_T sample_rate;
    ENUM_DEBOUNCE_DUAL_T dual_edge;
    
    dual_edge = eDebounce_Dual_No;
    switch (eint_config->trigger_mode) {
        case  HAL_EINT_LEVEL_LOW : {
            trigger_mode = eLevel_Trigger;
            trigger_polarity = eDebounce_Positive_Pol;
            break;
        }
        case  HAL_EINT_LEVEL_HIGH : {
            trigger_mode = eLevel_Trigger;
            trigger_polarity = eDebounce_Negative_Pol;
            break;
        }
        case  HAL_EINT_EDGE_FALLING : {
            trigger_mode = eEdge_Trigger;
            trigger_polarity = eDebounce_Positive_Pol;
            break;
        }
        case  HAL_EINT_EDGE_RISING : {
            trigger_mode = eEdge_Trigger;
            trigger_polarity = eDebounce_Negative_Pol;
            break;
        }
        case  HAL_EINT_EDGE_FALLING_AND_RISING : {
            trigger_mode = eEdge_Trigger;
            trigger_polarity = eDebounce_Negative_Pol;
            dual_edge = eDebounce_Dual_Yes;
            break;
        }
        default :
            return HAL_EINT_STATUS_ERROR;
    }

    hal_eint_calc_debounce_parameters(eint_config->debounce_time,
                                      &sample_rate,
                                      &sample_count);

    int32_t ret_value = gpio_int(gpio_pin,
                                 trigger_mode,
                                 eDebounce_Enable,
                                 trigger_polarity,
                                 dual_edge,
                                 sample_rate,
                                 sample_count,
                                 hal_eint_isr);
    return (ret_value >= 0) ? HAL_EINT_STATUS_OK : HAL_EINT_STATUS_ERROR;
}

hal_eint_status_t hal_eint_init(hal_eint_number_t eint_number, const hal_eint_config_t *eint_config)
{
    ENUM_IOT_GPIO_NAME_T gpio_pin;
    if (0 != hal_eint_convert_for_gpio(eint_number, &gpio_pin)) {
        log_hal_error("hal_eint_convert_for_gpio fail.");
        return HAL_EINT_STATUS_ERROR;
    }
    if (NULL == eint_config) {
        log_hal_error("eint_config is NULL.");
        return HAL_EINT_STATUS_INVALID_PARAMETER;
    }
    hal_eint_callback_context_t *callback_context = &g_eint_callback_context[get_index_from_gpio_pin(gpio_pin)];
    memset(callback_context, 0, sizeof(hal_eint_callback_context_t));
    return hal_eint_apply_config(gpio_pin, eint_config);
}

hal_eint_status_t hal_eint_deinit(hal_eint_number_t eint_number)
{
    return HAL_EINT_STATUS_OK;
}


hal_eint_status_t hal_eint_register_callback(hal_eint_number_t eint_number,
        hal_eint_callback_t callback,
        void *user_data)
{
    ENUM_IOT_GPIO_NAME_T gpio_pin;
    if (0 != hal_eint_convert_for_gpio(eint_number, &gpio_pin)) {
        log_hal_error("hal_eint_convert_for_gpio fail.");
        return HAL_EINT_STATUS_ERROR;
    }
    hal_eint_callback_context_t *callback_context = &g_eint_callback_context[get_index_from_gpio_pin(gpio_pin)];
    callback_context->callback = callback;
    callback_context->user_data = user_data;
    callback_context->callback_present = true;
    return HAL_EINT_STATUS_OK;
}

hal_eint_status_t hal_eint_set_trigger_mode(hal_eint_number_t eint_number, hal_eint_trigger_mode_t trigger_mode)
{
    ENUM_IOT_GPIO_NAME_T gpio_pin;
    if (0 != hal_eint_convert_for_gpio(eint_number, &gpio_pin)) {
        log_hal_error("hal_eint_convert_for_gpio fail.");
        return HAL_EINT_STATUS_ERROR;
    }

    NVIC_DisableIRQ((IRQn_Type)eint_number);

    switch (trigger_mode) {
        case  HAL_EINT_LEVEL_LOW : {
            mSetHWEntry(IOT_GPIO0_DEBOUNCE_POLARITY, eLevel_Trigger);
            mSetHWEntry(IOT_GPIO0_NVIC_SENSE, eDebounce_Positive_Pol);
            mSetHWEntry(IOT_GPIO0_DEBOUNCE_DUAL, eDebounce_Dual_No);
            break;
        }
        case  HAL_EINT_LEVEL_HIGH : {
            mSetHWEntry(IOT_GPIO0_DEBOUNCE_POLARITY, eLevel_Trigger);
            mSetHWEntry(IOT_GPIO0_NVIC_SENSE, eDebounce_Negative_Pol);
            mSetHWEntry(IOT_GPIO0_DEBOUNCE_DUAL, eDebounce_Dual_No);
            break;
        }
        case  HAL_EINT_EDGE_FALLING : {
            mSetHWEntry(IOT_GPIO0_DEBOUNCE_POLARITY, eEdge_Trigger);
            mSetHWEntry(IOT_GPIO0_NVIC_SENSE, eDebounce_Positive_Pol);
            mSetHWEntry(IOT_GPIO0_DEBOUNCE_DUAL, eDebounce_Dual_No);
            break;
        }
        case  HAL_EINT_EDGE_RISING : {
            mSetHWEntry(IOT_GPIO0_DEBOUNCE_POLARITY, eEdge_Trigger);
            mSetHWEntry(IOT_GPIO0_NVIC_SENSE, eDebounce_Negative_Pol);
            mSetHWEntry(IOT_GPIO0_DEBOUNCE_DUAL, eDebounce_Dual_No);
            break;
        }
        case  HAL_EINT_EDGE_FALLING_AND_RISING : {
            mSetHWEntry(IOT_GPIO0_NVIC_SENSE, eDebounce_Negative_Pol);
            mSetHWEntry(IOT_GPIO0_DEBOUNCE_DUAL, eDebounce_Dual_Yes);
            break;
        }
        default : {
            return HAL_EINT_STATUS_ERROR;
        }
    }

    mSetHWEntry(IOT_GPIO0_NVIC_CLR_PEND, 1);
    NVIC_EnableIRQ((IRQn_Type)eint_number);

    return HAL_EINT_STATUS_OK;

}

hal_eint_status_t hal_eint_set_debounce_time(hal_eint_number_t eint_number, uint32_t time_ms)
{
    ENUM_IOT_GPIO_NAME_T gpio_pin;
    uint32_t sample_count;
    ENUM_DEBOUNCE_PRESCALER_T sample_rate;
    if (0 != hal_eint_convert_for_gpio(eint_number, &gpio_pin)) {
        log_hal_error("hal_eint_convert_for_gpio fail.");
        return HAL_EINT_STATUS_ERROR;
    }

    /*
          7687 EINT spec
           3 bit          |                      |      4bit counter(1 to 16)
           pre-scaler      clock(KHz)      |   min(ms)           max (ms)
              000                8                      0.13                  2.00
              001                4                      0.25                  4.00
              010                2                      0.50                  8.00
              011                1                      1.00                  16.00
              100                0.5                   2.00                  32.00
              101                0.25                 4.00                   64.00
              110                0.125                8.00                  128.00
              111                0.0625              16.00                 256.00
    */
    hal_eint_calc_debounce_parameters(time_ms, &sample_rate, &sample_count);

    NVIC_DisableIRQ((IRQn_Type)eint_number);
    mSetHWEntry(IOT_GPIO0_DEBOUNCE_EN, eDebounce_Disable);
    mSetHWEntry(IOT_GPIO0_DEBOUNCE_CNT, sample_count);
    mSetHWEntry(IOT_GPIO0_DEBOUNCE_PRESCALER, sample_rate);
    mSetHWEntry(IOT_GPIO0_DEBOUNCE_EN, eDebounce_Enable);
    mSetHWEntry(IOT_GPIO0_NVIC_CLR_PEND, 1);
    NVIC_EnableIRQ((IRQn_Type)eint_number);
    return HAL_EINT_STATUS_OK;
}
#endif

