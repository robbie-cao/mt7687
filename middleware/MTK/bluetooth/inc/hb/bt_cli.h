#ifndef __BT_CLI_H__
#define __BT_CLI_H__

#if defined(MTK_MINICLI_ENABLE)


#ifdef __cplusplus
extern "C" {
#endif

#ifdef MTK_BLE_BQB_CLI_ENABLE
uint8_t bt_cli_bqb(uint8_t len, char *param[]);
#endif


#ifdef MTK_BLE_CLI_ENABLE
uint8_t bt_cli_ble(uint8_t len, char *param[]);
#endif


#if defined(MTK_BLE_BQB_CLI_ENABLE) || defined(MTK_BLE_CLI_ENABLE)
#include "bt_type.h"
#endif


/* Bluetooth Low Energy commands */


#ifdef MTK_BLE_CLI_ENABLE
#define BLE_CLI_ENTRY { "ble", "bluetooth ble related cmd", bt_cli_ble },
#else
#define BLE_CLI_ENTRY
#endif


#ifdef MTK_BLE_BQB_CLI_ENABLE
#define BLE_BQB_CLI_ENTRY { "bqb", "bluetooth bqb related cmd", bt_cli_bqb },
#else
#define BLE_BQB_CLI_ENTRY
#endif


#ifdef __cplusplus
}
#endif


#endif /* MTK_MINICLI_ENABLE */

#endif /* __BT_CLI_H__ */
