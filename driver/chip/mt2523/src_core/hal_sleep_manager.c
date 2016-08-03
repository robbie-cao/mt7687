/*
 * halsleepmanager.c
 *
 *  Created on: 2015?9?23?
 *      Author: MTK10267
 */


#include "hal_sleep_manager.h"
#ifdef HAL_SLEEP_MANAGER_ENABLED

#include "hal_sys_topsm.h"
#include "hal_cm4_topsm.h"
#include "hal_dsp_topsm.h"
#include "hal_sleep_manager_platform.h"
#include "hal_dcxo.h"
#include "hal_ostd.h"
#include "hal_sleep_driver.h"
#include "hal_log.h"
#include "hal_gpt.h"
#include "hal_pmu.h"
#include "string.h"
#include "hal_rtc.h"
extern cm4_topsm_mtcmos_control mtcmos_control;
extern bool BT_workaround;
uint8_t at_cmd_lock_sleep_handle;

void Deep_Sleep_GPT_CB()
{
    hal_gpt_stop_timer(DEEP_SLEEP_GPT);
}

uint8_t hal_sleep_manager_set_sleep_handle(const char *handle_name)
{
    uint8_t index;
    index = sleepdrv_get_handle(handle_name, strlen(handle_name));
    return index;
}

hal_sleep_manager_status_t hal_sleep_manager_lock_sleep(uint8_t handle_index)
{
    sleepdrv_hold_sleep_lock(handle_index);
    return HAL_SLEEP_MANAGER_OK;
}

hal_sleep_manager_status_t hal_sleep_manager_unlock_sleep(uint8_t handle_index)
{
    sleepdrv_release_sleep_lock(handle_index);
    return HAL_SLEEP_MANAGER_OK;
}

hal_sleep_manager_status_t hal_sleep_manager_release_sleep_handle(
    uint8_t handle)
{
    sleepdrv_release_handle(handle);
    return HAL_SLEEP_MANAGER_OK;
}

uint32_t hal_sleep_manager_get_lock_status(void)
{
    return sleepdrv_get_lock_index();
}

uint32_t hal_sleep_manager_sleep_driver_dump_handle_name(void)
{
    return sleepdrv_get_lock_module();
}

bool hal_sleep_manager_is_sleep_locked(void)
{
    return sleepdrv_check_sleep_locks();
}

bool hal_sleep_manager_is_sleep_handle_alive(uint8_t handle_index)
{
    sleepdrv_get_handle_status(handle_index);
    return sleepdrv_get_handle_status(handle_index);
}

void hal_sleep_manager_enter_power_off_mode()
{
    hal_rtc_deinit();
    pmu_set_register_value(PMU_RG_PWRHOLD_ADDR, PMU_RG_PWRHOLD_MASK, PMU_RG_PWRHOLD_SHIFT, 0);
}

hal_sleep_manager_status_t hal_sleep_manager_set_sleep_time(uint32_t sleep_time_ms)
{
    hal_gpt_status_t         ret_status;

    hal_gpt_register_callback(DEEP_SLEEP_GPT, (hal_gpt_callback_t)Deep_Sleep_GPT_CB, NULL);
    ret_status = hal_gpt_start_timer_ms(DEEP_SLEEP_GPT, sleep_time_ms, HAL_GPT_TIMER_TYPE_ONE_SHOT);
    if (ret_status != HAL_GPT_STATUS_OK) {
        printf("ERROR : Deep Sleep GPT Start Fail");
        return HAL_SLEEP_MANAGER_ERROR;
    }

    return HAL_SLEEP_MANAGER_OK;
}

void hal_sleep_manager_enter_sleep_mode(hal_sleep_mode_t mode)
{
    if (mode == HAL_SLEEP_MODE_IDLE) {
        __asm volatile("dsb");
        __asm volatile("wfi");
        __asm volatile("isb");
    } else if ((mode == HAL_SLEEP_MODE_SLEEP) || (mode == HAL_SLEEP_MODE_DEEP_SLEEP)) {
        cm4_topsm_enter_suspend(mode);
        hal_gpt_stop_timer(DEEP_SLEEP_GPT);
    }
}

hal_sleep_manager_status_t hal_sleep_manager_enable_wakeup_pin(hal_sleep_manager_wakeup_source_t pin)
{
    cm4_ostd_set_one_irq_enable(pin);
    return HAL_SLEEP_MANAGER_OK;
}

hal_sleep_manager_status_t hal_sleep_manager_disable_wakeup_pin(hal_sleep_manager_wakeup_source_t pin)
{
    cm4_ostd_set_one_irq_disable(pin);
    return HAL_SLEEP_MANAGER_OK;
}

hal_sleep_manager_status_t hal_sleep_manager_init()
{
    log_hal_info("hal_sleep_manager_init start\n");
    pmu_init();
    cm4_topsm_init();
    mtcmos_control.BT_Count = 0;
    mtcmos_control.MD2G_Count = 0;
    mtcmos_control.MM_Count = 0;
    BT_workaround = true;
    dsp_topsm_init();
    sys_topsm_init();
    sleep_driver_init();
    cm4_ostd_init();
    cm4_topsm_sleep_default_callback_function();
    char AT_Cmd_Test[] = "AT_Cmd_Test";
    at_cmd_lock_sleep_handle = hal_sleep_manager_set_sleep_handle(AT_Cmd_Test);
    hal_sleep_manager_lock_sleep(at_cmd_lock_sleep_handle);

    if (hal_gpt_init(DEEP_SLEEP_GPT) != HAL_GPT_STATUS_OK) {
        printf("ERROR : Deep Sleep GPT Init Fail");
        return HAL_SLEEP_MANAGER_ERROR;
    }

    return HAL_SLEEP_MANAGER_OK;
}

#endif /* HAL_SLEEP_MANAGER_ENABLED */
