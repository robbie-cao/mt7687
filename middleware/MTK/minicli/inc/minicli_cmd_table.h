/**
 * @file minicli_cmd_table.h
 *
 * This file contains a list of all CLI commands that are globally availabe
 * to each project. If a project enables the corresponding feature and turns
 * on it, then the CLI command will be turned on in the list of table in this
 * file. Two sets of commands are listed at the end of this file, namely:
 *
 * NORMAL mode - MINICLI_NORMAL_MODE_CLI_CMDS
 * TEST mode - MINICLI_TEST_MODE_CLI_CMDS
 *
 * Normal and test are enabled by default. TEST mode commands can be turned
 * off by setting MTK_CLI_TEST_MODE_ENABLE to 'n' in feature.mk.
 */

#if defined(MTK_MINICLI_ENABLE)


#ifdef __cplusplus
extern "C" {
#endif

#ifdef MTK_CLI_FORK_ENABLE
/**
 * FOTA_CLI_ENTRY
 */
#include "cli_cli.h"
#else
#define CLI_CLI_FORK_ENTRY
#endif


#ifdef MTK_FOTA_ENABLE
/**
 * FOTA_CLI_ENTRY
 */
#include "fota_cli.h"
#else
#define FOTA_CLI_ENTRY
#endif


/**
 * INBAND_CLI_ENTRY
 */
#include "wifi_inband_cli.h"


/**
 * IP_CLI_ENTRY
 */
#include "lwip_cli.h"


/**
 * IPERF_CLI_ENTRY
 */
#include "iperf_cli.h"


/**
 * IPERF_CLI_SPI_TPUT_ENTRY.
 */
#ifdef MTK_SPI_THROUGHPUT
#include "iperf_test.h"
#else
#define IPERF_CLI_SPI_TPUT_ENTRY
#endif

/**
 * LP_CLI_ENTRY
 */
#include "wifi_ex.h"


/**
 * MINISUPP_CLI_ENTRY
 */
#include "minisupp_cli.h"


/**
 * MOD_HAP_CLI_ENTRY
 */
#ifdef MTK_HOMEKIT_ENABLE
#include "HAP_test.h"
#else
#define MOD_HAP_CLI_ENTRY
#endif


/**
 * MOD_HKMOCK_CLI_ENTRY
 */
#if defined(MTK_HOMEKIT_HAP_MOCK) && defined(MTK_HOMEKIT_ENABLE)
#include "hkmock_cli.h"
#else
#define MOD_HKMOCK_CLI_ENTRY
#endif


#ifdef MTK_HOMEKIT_ENABLE
/**
 * MOD_MDNS_CLI_ENTRY
 */
#include "mdns_cli.h"
#else
#define MOD_MDNS_CLI_ENTRY
#endif


/**
 * MOD_MFI_CLI_ENTRY
 */
#ifdef MTK_HOMEKIT_ENABLE
#include "mfi_cli.h"
#else
#define MOD_MFI_CLI_ENTRY
#endif


/**
 * MOD_WAC_CLI_ENTRY
 */
#ifdef MTK_HOMEKIT_ENABLE
#include "wac_cli.h"
#else
#define MOD_WAC_CLI_ENTRY
#endif


/**
 * MPERF_CLI_ENTRY
 */
#ifdef MTK_MPERF_ENABLE
#include "mperf.h"
#else
#define MPERF_CLI_ENTRY
#endif


/**
 * NVDM_CLI_ENTRY
 */
#include "nvdm_cli.h"


/**
 * OS_CLI_ENTRY
 */
#include "os_cli.h"


/**
 * PING_CLI_ENTRY
 */
#include "ping_cli.h"


/**
 * REBOOT_CLI_ENTRY
 */
#include "board_cli.h"


/**
 * SDK_CLI_ENTRY
 */
#include "board_cli.h"


/**
 * SIE_CLI_ENTRY
 */
#include "sie_cli.h"


/**
 * SYSLOG_CLI_ENTRY
 */
#include "syslog_cli.h"


/**
 * TEST_REG_CLI_ENTRY
 */
#include "board_cli.h"


/**
 * BLE_CLI_ENTRY
 */
#ifdef MTK_BLE_CLI_ENABLE
#include "bt_cli.h"
#else
#define BLE_CLI_ENTRY
#endif


/**
 * BLE_BQB_CLI_ENTRY
 */
#ifdef MTK_BLE_BQB_CLI_ENABLE
#include "bt_cli.h"
#else
#define BLE_BQB_CLI_ENTRY
#endif


#define MINICLI_TEST_MODE_CLI_CMDS      INBAND_CLI_ENTRY         \
                                        SIE_CLI_ENTRY            \
                                        MINISUPP_CLI_ENTRY       \
                                        WIFI_CONNSYS_CLI_ENTRY   \
                                        MOD_MFI_CLI_ENTRY        \
                                        MOD_WAC_CLI_ENTRY        \
                                        MOD_HAP_CLI_ENTRY        \
                                        MOD_MDNS_CLI_ENTRY       \
                                        MOD_HKMOCK_CLI_ENTRY     \
                                        IP_CLI_ENTRY             \
                                        CLI_CLI_FORK_ENTRY       \
                                        WIFI_SCAN_CLI_ENTRY      \
                                        WIFI_SUPP_CLI_ENTRY      \
                                        WIFI_CLI_WIFI_ENTRY      \
                                        TEST_REG_CLI_ENTRY       \
                                        LP_CLI_ENTRY             \
                                        OS_CLI_ENTRY             \
                                        REBOOT_CLI_ENTRY         \
                                        MPERF_CLI_ENTRY          \
                                        N9_LOG_CLI_ENTRY         \


#define MINICLI_NORMAL_MODE_CLI_CMDS    FOTA_CLI_ENTRY           \
                                        IP_CLI_ENTRY             \
                                        PING_CLI_ENTRY           \
                                        IPERF_CLI_ENTRY          \
                                        IPERF_CLI_SPI_TPUT_ENTRY \
                                        NVDM_CLI_ENTRY           \
                                        WIFI_CLI_SMNT_ENTRY      \
                                        AWS_CLI_ENTRY            \
                                        WIFI_CLI_ENTRY           \
                                        BLE_CLI_ENTRY            \
                                        BLE_BQB_CLI_ENTRY        \
                                        SYSLOG_CLI_ENTRY         \
                                        SDK_CLI_ENTRY

#ifdef __cplusplus
}
#endif

#endif /* #if defined(MTK_MINICLI_ENABLE) */

