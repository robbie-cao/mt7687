/*
 * halsleepmanager.c
 *
 *  Created on: 2015?9?23?
 *      Author: MTK10267
 */


#include "hal_sleep_manager.h"
#ifdef HAL_SLEEP_MANAGER_ENABLED

#include "hal_sleep_manager_platform.h"
#include "hal_sleep_driver.h"
#include "hal_log.h"
#include "hal_gpt.h"
#include "string.h"
uint8_t at_cmd_lock_sleep_handle;

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

hal_sleep_manager_status_t hal_sleep_manager_init()
{
    sleep_driver_init();
    return HAL_SLEEP_MANAGER_OK;
}

void hal_sleep_manager_enter_sleep_mode(hal_sleep_mode_t mode)
{
}

hal_sleep_manager_status_t hal_sleep_manager_set_sleep_time(uint32_t sleep_time_ms)
{
    return HAL_SLEEP_MANAGER_OK;
}
#endif /* HAL_SLEEP_MANAGER_ENABLED */
