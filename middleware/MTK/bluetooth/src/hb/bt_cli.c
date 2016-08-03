
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "os.h"

#include "bt_type.h"

bool is_cli_bqb_mode = false;//default is cli mode


#ifdef MTK_BLE_BQB_CLI_ENABLE
bt_status_t bt_app_io_callback(void *input, void *output);


uint8_t bt_cli_bqb(uint8_t len, char *param[])
{
    int32_t i;
    uint8_t cmd[140];
    uint32_t tt = 0;
    for (i = 0; i < len; i++) {
        os_memcpy(cmd + tt, param[i], strlen(param[i]));
        tt += strlen(param[i]);
        *(cmd + tt) = ' ';
        tt += 1;
    }
    *(cmd + tt - 1) = '\0';
    printf("CMD: %s\n", cmd);
    is_cli_bqb_mode = true;
    return bt_app_io_callback(cmd, NULL);

}
#endif /* MTK_BLE_BQB_CLI_ENABLE */

#ifdef MTK_BLE_CLI_ENABLE
bt_status_t bt_app_io_callback(void *input, void *output);

uint8_t bt_cli_ble(uint8_t len, char *param[])
{
    int32_t i;
    uint8_t cmd[140];
    uint32_t tt = 0;
    for (i = 0; i < len; i++) {
        os_memcpy(cmd + tt, param[i], strlen(param[i]));
        tt += strlen(param[i]);
        *(cmd + tt) = ' ';
        tt += 1;
    }
    *(cmd + tt - 1) = '\0';
    printf("CMD: %s\n", cmd);
    is_cli_bqb_mode = false;
    return bt_app_io_callback(cmd, NULL);

}
#endif /* MTK_BLE_CLI_ENABLE */

