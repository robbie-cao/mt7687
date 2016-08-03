#include "fota.h"
#include "hal_flash.h"
#include "hal_wdt_retention.h"

#if 0
hal_flash_status_t hal_flash_init(void)
{
    printf( "[FOTA] trace: hal_flash_init\n");
    return HAL_FLASH_STATUS_OK;
}


hal_flash_status_t hal_flash_deinit(void)
{
    printf( "[FOTA] trace: hal_flash_deinit\n");
    return HAL_FLASH_STATUS_OK;
}


hal_flash_status_t hal_flash_erase(uint32_t start_address,  hal_flash_block_t block)
{
    //printf( "[FOTA] trace: hal_flash_erase\n");
    return HAL_FLASH_STATUS_OK;
}


hal_flash_status_t hal_flash_read(uint32_t start_address, uint8_t *buffer, uint32_t length)
{
    printf( "[FOTA] trace: hal_flash_read\n");
    return HAL_FLASH_STATUS_OK;
}


hal_flash_status_t hal_flash_write(uint32_t address, const uint8_t *data, uint32_t length)
{
    printf( "[FOTA] trace: hal_flash_write\n");
    return HAL_FLASH_STATUS_OK;
}

fota_error_code_t fota_trigger_update(void)
{
    printf( "[FOTA] trace: fota_trigger_update\n");
    return FOTA_TRIGGER_SUCCESS;
}

void wdt_set_retention_flag(WDT_RETENTION_FLAG_USER_ID_T user_id, uint32_t register_number, uint32_t bit_offset, bool value)
{
}

int8_t wdt_read_retention_flag(uint32_t register_number, uint32_t bit_offset)
{
    return 0;
}

#endif
