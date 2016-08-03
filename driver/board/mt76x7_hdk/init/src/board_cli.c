

#include <stdio.h>
#include <stdint.h>

#include "os.h"

#include "toi.h"
#include "cli.h"
#include "hal_sys.h"
#include "hal_cache.h"
#include "connsys_util.h"

#ifndef MTK_FW_VERSION

#ifdef __CC_ARM
#define MTK_FW_VERSION "Not supported by Keil"
#endif

#ifdef __ICCARM__
#define MTK_FW_VERSION "Not supported by IAR"
#endif

#endif



/****************************************************************************
 * Public Functions
 ****************************************************************************/


uint8_t board_cli_ver(uint8_t len, char *param[])
{
    char fw_ver[32];
    char patch_ver[32];

    os_memset(fw_ver, 0, 32);
    os_memset(patch_ver, 0, 32);

    cli_puts("CM4 Image Ver: ");
    cli_puts(MTK_FW_VERSION);
    cli_putln();

    connsys_util_get_n9_fw_ver(fw_ver);
    connsys_util_get_ncp_patch_ver(patch_ver);

    cli_puts("N9 Image  Ver: ");
    cli_puts(fw_ver);
    cli_putln();

    cli_puts("HW Patch  Ver: ");
    cli_puts(patch_ver);
    cli_putln();

    return 0;
}


uint8_t board_cli_reboot(uint8_t len, char *param[])
{
    cli_puts("Reboot Bye Bye Bye!!!!\n");

    hal_cache_disable();
    hal_cache_deinit();
    hal_sys_reboot(HAL_SYS_REBOOT_MAGIC, WHOLE_SYSTEM_REBOOT_COMMAND);

    return 0;
}


uint8_t board_cli_reg_read(uint8_t len, char *param[])
{
    uint32_t reg;
    uint32_t val;
    uint8_t  type;

    if (len != 1) {
        printf("reg#\n");
        return 0;
    }

    reg = toi(param[0], &type);

    if (type == TOI_ERR) {
        printf("reg#\n");
    } else {
        val = *((volatile uint32_t *)reg);
        printf("read register 0x%08x (%u) got 0x%08x\n", (unsigned int)reg, (unsigned int)reg, (unsigned int)val);
    }

    return 0;
}


uint8_t board_cli_reg_write(uint8_t len, char *param[])
{
    uint32_t reg;
    uint32_t val;
    uint8_t  type;

    if (len == 2) {
        reg = toi(param[0], &type);
        if (type == TOI_ERR) {
            printf("reg#\n");
            return 0;
        }
        val = toi(param[1], &type);
        if (type == TOI_ERR) {
            printf("val#\n");
            return 0;
        }

        *((volatile uint32_t *)reg) = val;
        printf("written register 0x%08x (%u) as 0x%08x\n", (unsigned int)reg, (unsigned int)reg, (unsigned int)val);
    }

    return 0;
}


