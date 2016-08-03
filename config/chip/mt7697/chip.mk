PRODUCT_VERSION=7697

# Default common setting

MTK_LWIP_ENABLE                             ?= y
MTK_WIFI_API_TEST_CLI_ENABLE                ?= y
MTK_WIFI_PROFILE_ENABLE                     ?= n
MTK_CLI_TEST_MODE_ENABLE                    ?= n
ifeq ($(MTK_WIFI_TGN_VERIFY_ENABLE),y)
MTK_HIF_GDMA_ENABLE                         ?= y
else
MTK_HIF_GDMA_ENABLE                         ?= n
endif
MTK_MAIN_CONSOLE_UART2_ENABLE               ?= n
MTK_BSP_LOOPBACK_ENABLE                     ?= n
MTK_OS_CPU_UTILIZATION_ENABLE               ?= n
MTK_SUPPORT_HEAP_DEBUG                      ?= n
MTK_HEAP_SIZE_GUARD_ENABLE                  ?= n

AR      = $(BINPATH)/arm-none-eabi-ar
CC      = $(BINPATH)/arm-none-eabi-gcc
CXX     = $(BINPATH)/arm-none-eabi-g++
OBJCOPY = $(BINPATH)/arm-none-eabi-objcopy
SIZE    = $(BINPATH)/arm-none-eabi-size
OBJDUMP = $(BINPATH)/arm-none-eabi-objdump


ALLFLAGS = -mlittle-endian -mthumb -mcpu=cortex-m4
FPUFLAGS = -fsingle-precision-constant -Wdouble-promotion -mfpu=fpv4-sp-d16 -mfloat-abi=hard

COM_CFLAGS += $(ALLFLAGS) $(FPUFLAGS) -ffunction-sections -fdata-sections -fno-builtin
COM_CFLAGS += -gdwarf-2 -Os -fno-strict-aliasing -fno-common
COM_CFLAGS += -Wall -Wimplicit-function-declaration -Werror=uninitialized -Wno-error=maybe-uninitialized -Werror=return-type -Wno-switch
COM_CFLAGS += -DPCFG_OS=2 -D_REENT_SMALL
COM_CFLAGS += -DPRODUCT_VERSION=$(PRODUCT_VERSION)

ifeq ($(MTK_SYS_TRNG_ENABLE),y)
COM_CFLAGS         += -DMTK_SYS_TRNG_ENABLE
endif

#
# MTK_CLI_TEST_MODE_ENABLE enables testing and under development features
# related CLI commands.
#

ifeq ($(MTK_CLI_TEST_MODE_ENABLE),y)
COM_CFLAGS         += -DMTK_CLI_TEST_MODE_ENABLE
endif

#
# MTK_WIFI_API_TEST_CLI_ENABLE contains test CLI commands used by RD, most of
# which are same as standard MTK CLI commands. Default should be disabled.
#
# OBSOLETE, to be removed soon
#

ifeq ($(MTK_WIFI_API_TEST_CLI_ENABLE),y)
COM_CFLAGS          += -DMTK_WIFI_API_TEST_CLI_ENABLE
endif

#
# MTK_MINISUPP_ENABLE enables built-in MTK mini-supplicant of CM4 version.
# All the security state machine and handshaking are processed by supplicant
# on CM4. Default should be enabled.
#

COM_CFLAGS         += -DMTK_MINISUPP_ENABLE

##
## MTK_SMTCN_ENABLE
## Brief:       This option is to enable/disable WIFI smart connection.
## Usage:       If the value is "y", the MTK_SMTCN_ENABLE compile option will be defined. You must also include the middleware/MTK/smtcn/module.mk 
##              and libsmtcn.a, libbsp.a in your Makefile before setting the option to "y". 
## Path:        middleware/MTK/smtcn/src
## Dependency:  libbsp.
## Relative doc:Please refer to wifi dev guide under the doc folder for more detail.
##
ifeq ($(MTK_SMTCN_ENABLE),y)
COM_CFLAGS         += -DMTK_SMTCN_ENABLE
endif

##
## MTK_MINICLI_ENABLE
## Brief:       This option is to enable and disable CLI (command line interface) engine.
## Usage:       If the value is "y", the MTK_MINICLI_ENABLE compile option will be defined. You must also include the middleware/MTK/minicli/module.mk in your Makefile before setting the option to "y". 
## Path:        middleware/MTK/minicli
## Dependency:  HAL_UART_MODULE_ENABLED must also defined in hal_feature_config.h under project inc folder.
## Notice:      N/A
## Realted doc: N/A
##

ifeq ($(MTK_MINICLI_ENABLE),y)
COM_CFLAGS         += -DMTK_MINICLI_ENABLE
endif

##
## MTK_CLI_FORK_ENABLE
## Brief:       This option enables the CLI command 'fork' to allow users to run their commands in a separate task.
## Usage:       If the value is "y", the MTK_CLI_FORK_ENABLE compile option will be defined. You must also include the middleware/MTK/minicli/module.mk in your Makefile before setting the option to "y". 
## Path:        middleware/MTK/minicli
## Dependency:  MTK_MINICLI_ENABLE must also defined in feature.mk under project inc folder.
## Notice:      N/A
## Related doc: N/A
##

ifeq ($(MTK_CLI_FORK_ENABLE), y)
COM_CFLAGS         += -DMTK_CLI_FORK_ENABLE
endif

#
# DEPRECATED (MTK mini-supplicant default support STA and SoftAP).
#

ifeq ($(MTK_WIFI_AP_ENABLE),y)
COM_CFLAGS         += -DMTK_WIFI_AP_ENABLE
endif

##
## MTK_PING_OUT_ENABLE
## Brief:       It supports MTK lite-ping tool to issue ping request toward the other peer of the connection. It's used by RD for debugging.
## Usage:       If the value is "y", the MTK_PING_OUT_ENABLE compile option will be defined. You must also include the middleware\third_party\ping\module.mk in your Makefile before setting the option to "y". 
## Path:        middleware/third_party/ping
## Dependency:  MTK_MINICLI_ENABLE must be defined in the project.
##
ifeq ($(MTK_PING_OUT_ENABLE),y)
COM_CFLAGS         += -DMTK_PING_OUT_ENABLE
endif

##
## MTK_IPERF_ENABLE
## Brief:       It supports supports iperf client and server service run on CM4. other peer of the connection. It supports almost all the major iPerf v2.0 features and most used for RD development and debug.
## Usage:       If the value is "y", the MTK_IPERF_ENABLE compile option will be defined. You must also include the middleware\third_party\ping\module.mk in your Makefile before setting the option to "y". 
## Path:        middleware/MTK/iperf
## Dependency:  MTK_MINICLI_ENABLE must be defined in the project.
##
ifeq ($(MTK_IPERF_ENABLE),y)
COM_CFLAGS         += -DMTK_IPERF_ENABLE
endif

##
## MTK_BSPEXT_ENABLE
## Brief:       This option is to enable and disable the Wi-Fi relevant CLI (command line interface).
## Usage:       If the value is "y", the MTK_BSPEXT_ENABLE  compile option will be defined. You must also include the project/common/bsp_ex/module.mk 
##              in your Makefile before setting the option to "y". 
## Path:        driver/board/mt76x7_hdk/wifi     project/common/bsp_ex
## Dependency:  MTK_MINICLI_ENABLE must also defined in feature.mk in your project£¬ 
##              HAL_UART_MODULE_ENABLED must also defined in hal_feature_config.h under project inc folder.
##              libbsp
##

ifeq ($(MTK_BSPEXT_ENABLE),y)
COM_CFLAGS         += -DMTK_BSPEXT_ENABLE
endif

#
# MTK_WIFI_TGN_VERIFY_ENABLE is an option to support TGn verification,
# which use large packet memory (64KB at SYSRAM) to pass TGn pass criteria.
# Once disable this flag, packet memory will shrink to 29KB and placed on TCM
# memory. This could release 64KB SYSRAM for customer use and speed up packet
# memory access.
#

ifeq ($(MTK_WIFI_TGN_VERIFY_ENABLE),y)
COM_CFLAGS         += -DMTK_WIFI_TGN_VERIFY_ENABLE
endif

#
# This is for an early stage IC and hardware that has been delivered.
#
# DO NOT USE, if you do not know what this is.
#

ifeq ($(MT7687_E1), 1)
COM_CFLAGS         += -D__EVB_E1__=1
endif

#
# DO NOT USE, software not available.
#

ifeq ($(IC_CONFIG),mt7687)
COM_CFLAGS         += -DMTK_FOTA_ON_7687
endif

ifeq ($(MTK_FOTA_ENABLE), y)
COM_CFLAGS         += -DMTK_FOTA_ENABLE
endif

##
## MTK_OS_CPU_UTILIZATION_ENABLE
## Brief:       This option is to enable and disable cpu utilization function.
## Usage:     If the value is "y", the MTK_OS_CPU_UTILIZATION_ENABLE compile option will be defined and supports the 'os 2' MTK CLI commands to show/get statistics of CM4 CPU utilizations of all the tasks running on.You must also include the gva3\kernel\service\module.mk in your Makefile before setting the option to "y". 
## Path:       kernel/service
## Dependency:  MTK_MINICLI_ENABLE must be enabled in this file.
## Notice:      None
## Realted doc: None
##
ifeq ($(MTK_OS_CPU_UTILIZATION_ENABLE),y)
COM_CFLAGS         += -DMTK_OS_CPU_UTILIZATION_ENABLE
endif

##
## MTK_HAL_LOWPOWER_ENABLE
## Brief:       This option is to enable CM4 deep low power support.
## Usage:       If the value is "y", the MTK_HAL_LOWPOWER_ENABLE compile option will be defined, the related code under driver\chip\mt7687\src\sdio_gen3, middleware\third_party\lwip\ports, project\mt7687_hdk\apps\iot_sdk\src, and middleware\mtk\minisupp\src_protected\wpa_supplicant_8.jb4_1\wpa_supplicant will be included.
## Path:        driver\chip\mt7687\src\sdio_gen3, middleware\third_party\lwip\ports, project\mt7687_hdk\apps\iot_sdk\src, middleware\mtk\minisupp\src_protected\wpa_supplicant_8.jb4_1\wpa_supplicant
## Dependency:  None
## Notice:      The default setting is ON. Enable this option will handle CM4 low power related interrupt and wakeup flow to protect CM4 when N9 in sleep state.  
##              When this option is OFF, CM4 disallow enter deep low power mode.
## Realted doc: None
##

ifeq ($(MTK_HAL_LOWPOWER_ENABLE),y)
COM_CFLAGS         += -DMTK_HAL_LOWPOWER_ENABLE
endif

##
## MTK_LWIP_ENABLE
## Brief:       This option provide the LWIPs CLI for user.
## Usage:       If the value is "y", the MTK_LWIP_ENABLE compile option will be defined. You must also include the middleware\third_party\lwip\module.mk in your Makefile before setting the option to "y". 
## Path:        middleware/third_party/lwip
## Dependency:  MTK_MINICLI_ENABLE must be defined in the project.
##
ifeq ($(MTK_LWIP_ENABLE),y)
COM_CFLAGS         += -DMTK_LWIP_ENABLE
endif

##
## MTK_MBEDTLS_CONFIG_FILE
## Brief:       This option is to configure mbedTLS features.
## Usage:       If the value is "*.h", mbedTLS module will use *.h as the configuration file. For example, if its value is "config-mtk-basic.h",
##              config-mtk-basic.h will be used as the configuration file. MTK_MBEDTLS_CONFIG_FILE compile option will be defined. You must also
##              include the gva/middleware/third_party/mbedtls/module.mk in your Makefile before setting the option to "*.h".
## Path:        middleware/third_party/mbedtls
## Dependency:  LWIP module must be enabled.
## Realted doc: Please refer to internet and open source software guide under the doc folder for more detail.
##
ifneq ($(MTK_MBEDTLS_CONFIG_FILE),)
COM_CFLAGS         += -DMBEDTLS_CONFIG_FILE=\"$(MTK_MBEDTLS_CONFIG_FILE)\"
endif

##
## MTK_HTTPCLIENT_SSL_ENABLE
## Brief:       This option is to switch SSL/TLS support in SSL client module.
## Usage:       To enable HTTP client support in a project, use "LIBS += $(OUTPATH)/libhttpclient.a" to include the middleware.
##              Additionally, switch this option to configure SSL support in HTTP client.
## Path:        middleware/third_party/httpclient
## Dependency:  LWIP and mbedTLS module must be enabled.
## Realted doc: Please refer to internet and open source software guide under the doc folder for more detail.
##
ifeq ($(MTK_HTTPCLIENT_SSL_ENABLE),y)
CFLAGS += -DMTK_HTTPCLIENT_SSL_ENABLE
endif

##
## MTK_HOMEKIT_ENABLE
## Brief:       This option is to enable homekit under middleware/MTK/homekit folder.
## Usage:       If the value is "y", the MTK_HOMEKIT_ENABLE compile option will be defined, the sources and header files under middleware/MTK/homekit/inc be included by middleware/MTK/homekit/MakeFile.
## Path:        gva/middleware/MTK/homekit
## Dependency:  None, more dependency for lower module be defined in middleware/MTK/homekit/readme.txt.
## Notice:      middleware/MTK/homekit is only for SLA customer with MFi license
## Relative doc: pls refer to middleware/MTK/homekit/doc/Getting_Started_with_IoT_Homekit_v1.2_on_MT7687 about how to start homekit
##
ifeq ($(MTK_HOMEKIT_ENABLE),y)
COM_CFLAGS += -DMTK_HOMEKIT_ENABLE
export HOMEKIT_DIR = middleware/MTK/homekit
endif

##
## MTK_HOMEKIT_HAP_MOCK
## Brief:       This option is to enable homekit profile using a mocked implementation under middleware/MTK/homekit folder.
## Usage:       If the value is "y", the MTK_HOMEKIT_HAP_MOCK compile option will be defined, the sources and header files under
##              middleware/MTK/homekit/src_protected/hkap be included by middleware/MTK/homekit/src_protected/hkap/module.mk.
## Path:        gva/middleware/MTK/homekit/src_protected/hkap/
## Dependency:  MTK_HOMEKIT_ENABLE.
## Notice:      middleware/MTK/homekit is only for SLA customer with MFi license
## Relative doc: none
##
ifeq ($(MTK_HOMEKIT_HAP_MOCK),y)
COM_CFLAGS += -DMTK_HOMEKIT_HAP_MOCK
endif

ifeq ($(MTK_HIF_GDMA_ENABLE), y)
COM_CFLAGS += -DMTK_HIF_GDMA_ENABLE
endif

ifeq ($(MTK_WIFI_FORCE_AUTOBA_DISABLE),y)
COM_CFLAGS += -DMTK_WIFI_FORCE_AUTOBA_DISABLE
endif

##
## MTK_WIFI_DIRECT_ENABLE
## Brief:       This option is to enable/disable Wi-Fi Direct features.
## Usage:       If the value is "y", the MTK_WIFI_DIRECT_ENABLE will be defined,You must include some module in your Makefile before setting
##              the option to "y".
##              include the middleware/MTK/minisupp/module.mk 
##              include the libminisupp_wps.a
##              include the driver/board/mt76x7_hdk/wifi/src/module.mk
##              include the libbsp.a
## Path:        middleware/MTK/minisupp
## Dependency:  MTK_WIFI_WPS_ENABLE must be defined,  libbsp
## Notice:      This feature is not ready yet, default turn it off.
## Relative doc:None
##

ifeq ($(MTK_WIFI_DIRECT_ENABLE),y)
COM_CFLAGS += -DMTK_WIFI_DIRECT_ENABLE
CFLAGS += -I$(DIR)/wpa_supplicant
CFLAGS += -I$(DIR)/src/p2p
CONFIG_P2P=y
MTK_WIFI_WPS_ENABLE=y
endif

##
## MTK_WIFI_WPS_ENABLE
## Brief:       This option is to enable/disable Wi-Fi WPS features.
## Usage:       If the value is "y", the MTK_WIFI_WPS_ENABLE will be defined,You must include some module in your Makefile before setting
##              the option to "y".
##              include the middleware/MTK/minisupp/module.mk 
##              include the libminisupp_wps.a
##              include the driver/board/mt76x7_hdk/wifi/src/module.mk
##              include the libbsp.a
## Path:        middleware/MTK/minisupp
## Dependency:  libbsp.
## Relative doc:Please refer to wifi dev guide under the doc folder for more detail.
##

ifeq ($(MTK_WIFI_WPS_ENABLE),y)
COM_CFLAGS += -DMTK_WIFI_WPS_ENABLE
CONFIG_WPS=y
CONFIG_WPS2=y
endif

##
## MTK_WIFI_REPEATER_ENABLE
## Brief:       This option is to enable/disable MTK Wi-Fi Repeator mode.
## Usage:       If the value is "y", the MTK_WIFI_REPEATER_ENABLE will be defined,You must include some module in your Makefile before setting
##              the option to "y".
##              include the middleware/MTK/minisupp/module.mk 
##              include the libminisupp_wps.a or libminisupp.a which base on whether the MTK_WIFI_WPS_ENABLE is defined
##              include the driver/board/mt76x7_hdk/wifi/src/module.mk
##              include the libbsp.a
## Path:        middleware/MTK/minisupp
## Dependency:  libbsp.
## Notice:      This feature is enabled by default in the libminisupp_wps.a and libminisupp.a
## Realted doc: Please refer to wifi dev guide under the doc folder for more detail.
##

ifeq ($(MTK_WIFI_REPEATER_ENABLE),y)
COM_CFLAGS += -DMTK_WIFI_REPEATER_ENABLE
CONFIG_REPEATER=y
endif

##
## MTK_DEBUG_LEVEL
## Brief:       This option is to configure system log debug level.
## Usage:       The valid values are empty, error, warning, info, and none.
##              The setting will determine whether a debug log will be compiled.
##              However, the setting has no effect on the prebuilt library.
##              empty   : All debug logs are compiled.
##              error   : Only error logs are compiled.
##              warning : Only warning and err logs are compiled.
##              info    : All debug logs are compiled.
##              none    : All debugs are disabled.
## Path:        kernel/service
## Dependency:  None
## Notice:      None
## Realted doc: Please refer to doc/LinkIt_for_RTOS_System_Log_Developers_Guide.pdf
##
ifeq ($(MTK_DEBUG_LEVEL),)
  COM_CFLAGS += -DMTK_DEBUG_LEVEL_INFO
  COM_CFLAGS += -DMTK_DEBUG_LEVEL_WARNING
  COM_CFLAGS += -DMTK_DEBUG_LEVEL_ERROR
endif

ifeq ($(MTK_DEBUG_LEVEL),error)
  COM_CFLAGS += -DMTK_DEBUG_LEVEL_ERROR
endif

ifeq ($(MTK_DEBUG_LEVEL),warning)
  COM_CFLAGS += -DMTK_DEBUG_LEVEL_WARNING
  COM_CFLAGS += -DMTK_DEBUG_LEVEL_ERROR
endif

ifeq ($(MTK_DEBUG_LEVEL),info)
  COM_CFLAGS += -DMTK_DEBUG_LEVEL_INFO
  COM_CFLAGS += -DMTK_DEBUG_LEVEL_WARNING
  COM_CFLAGS += -DMTK_DEBUG_LEVEL_ERROR
endif

ifeq ($(MTK_DEBUG_LEVEL),none)
  COM_CFLAGS += -DMTK_DEBUG_LEVEL_NONE
endif

##
## The following makefile options are not configurable and may be removed in the future. 
## Please do not set the makefile options to "y" in your GCC feature configuration.
##

ifeq ($(MTK_HEAP_GUARD_ENABLE),y)
ALLFLAGS       += -Wl,-wrap=pvPortMalloc -Wl,-wrap=vPortFree
COM_CFLAGS         += -DHEAP_GUARD_ENABLE
endif

ifeq ($(MTK_HEAP_SIZE_GUARD_ENABLE),y)
ALLFLAGS       += -Wl,-wrap=pvPortMalloc -Wl,-wrap=vPortFree
COM_CFLAGS         += -DMTK_HEAP_SIZE_GUARD_ENABLE
endif

ifeq ($(MTK_MET_TRACE_ENABLE),y)
COM_CFLAGS         += -DMET_TRACE_ENABLE
endif

ifeq ($(MTK_CODE_COVERAGE_ENABLE),y)
AR = $(BINPATH)/arm-none-eabi-cov-ar
CC = $(BINPATH)/arm-none-eabi-cov-gcc
CXX= $(BINPATH)/arm-none-eabi-cov-g++
export GCOV_DIR=$(SOURCE_DIR)
endif

#
# MTK_MAIN_CONSOLE_UART2_ENABLE supports CM4 console output via UART2 (UART1
# in HW spec.) instead of UART1 (UART0 in HW spec). It needs the board circuit
# supports, and only applied in MTK EVB only so far. Default should be off.
#

ifeq ($(MTK_MAIN_CONSOLE_UART2_ENABLE),y)
COM_CFLAGS         += -DMTK_MAIN_CONSOLE_UART2_ENABLE
endif

ifeq ($(MTK_SUPPORT_HEAP_DEBUG),y)
COM_CFLAGS         += -DMTK_SUPPORT_HEAP_DEBUG
endif

##
## MTK_AP_SNIFFER_ENABLE enables built-in MTK AP-sniffer concurrent mode.
## Default should be enabled.
##
ifeq ($(MTK_AP_SNIFFER_ENABLE),y)
COM_CFLAGS         += -DMTK_AP_SNIFFER_ENABLE
endif

ifeq ($(MTK_HCI_CONSOLE_MIX_ENABLE),y)
COM_CFLAGS += -DMTK_HCI_CONSOLE_MIX_ENABLE
endif

ifeq ($(MTK_BLE_BQB_CLI_ENABLE),y)
COM_CFLAGS += -DMTK_BLE_BQB_CLI_ENABLE
endif

ifeq ($(MTK_BT_BQB_TEST_ENABLE),y)
COM_CFLAGS += -DMTK_BT_BQB_ENABLE
endif

ifeq ($(MTK_BLE_BQB_ENABLE),y)
COM_CFLAGS += -DMTK_BLE_BQB_ENABLE
endif

ifeq ($(MTK_BLE_BQB_ENABLE),y)
COM_CFLAGS += -DMTK_BLE_BQB_ENABLE
endif

#
# MTK_BSP_LOOPBACK_ENABLE supports WiFi throughput loopback test from CM4 to
# N9, without round trip to LMAC and the air. Default should be disabled.
#
ifeq ($(MTK_BSP_LOOPBACK_ENABLE),y)
COM_CFLAGS         += -DMTK_BSP_LOOPBACK_ENABLE
endif

##
## MTK_WIFI_PROFILE_ENABLE
## Brief:       This option is to enable/disable the wifi profile features.  
##              when the feature is enabled, the wifi profile APIs are available to get and set the setting into NVDM.
##              when the feature is disabled, the wifi profile APIs are unavailable
## Usage:       If the value is "y", the MTK_WIFI_PROFILE_ENABLE will be defined,You must include the driver/board/mt76x7_hdk/wifi/src/module.mk
##              in your Makefile before setting the option to "y".
## Path:        driver/board/mt76x7_hdk/wifi/src
## Dependency:  NVDM
## Notice:      This feature is deprecated. 
## Realted doc: None.
##
ifeq ($(MTK_WIFI_PROFILE_ENABLE),y)
COM_CFLAGS += -DMTK_WIFI_PROFILE_ENABLE
endif

##
## MTK_BLE_CLI_ENABLE
## Brief:       This option is to enable/disable the BLE commands.  
## Usage:       If the value is "y", the MTK_BLE_CLI_ENABLE will be defined, the related code in the file bt_cli.c/.h and the file minicli_cmd_table.h will be included.
## Path:        middleware/MTK/bluetooth/src/hb, middleware/MTK/bluetooth/inc/hb, middleware/MTK/minicli/inc
## Dependency:  MTK_MINICLI_ENABLE
## Notice:      None.
## Relative doc:None.
##
ifeq ($(MTK_BLE_CLI_ENABLE),y)
COM_CFLAGS += -DMTK_BLE_CLI_ENABLE
endif

##
## MTK_LOAD_MAC_ADDR_FROM_EFUSE
## Brief:       This option is to configure the wifi MAC address initialization source .  
##              when the feature is enabled, if wifi MAC address in NVDM is NULL, system will read it from EFUSE then calculating STA MAC address 
##              and AP MAC address, then stored them into NVDM.   if Efuse also has no valid MAC address. system will read the MAC address from 
##              the array table in user_config.c
##              when the feature is disabled, if wifi MAC address in NVDM is NULL, system will read the MAC address from the array table in user_config.c
## Usage:       If the value is "y", the MTK_LOAD_MAC_ADDR_FROM_EFUSE will be defined.
## Path:        driver/board/mt76x7_hdk/wifi/src
## Dependency:  NVDM
## Notice:      This feature is deprecated. 
## Realted doc: None.
##

ifeq ($(MTK_LOAD_MAC_ADDR_FROM_EFUSE),y)
COM_CFLAGS += -DMTK_LOAD_MAC_ADDR_FROM_EFUSE
endif

#Incldue Path
COM_CFLAGS += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS
COM_CFLAGS += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/include
COM_CFLAGS += -I$(SOURCE_DIR)/driver/CMSIS/Device/MTK/mt7687/Include
COM_CFLAGS += -I$(SOURCE_DIR)/driver/CMSIS/Include
COM_CFLAGS += -I$(SOURCE_DIR)/driver/chip/mt7687/inc
COM_CFLAGS += -I$(SOURCE_DIR)/driver/chip/inc
COM_CFLAGS += -I$(SOURCE_DIR)/driver/chip/mt7687/src/common/include
COM_CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/tftp/inc
COM_CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/fota/inc
COM_CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/fota/inc/76x7
ifeq ($(MTK_JOYLINK_ENABLE),y)
COM_CFLAGS += -DMTK_JOYLINK_ENABLE
COM_CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/cloud/jd_joylink/inc
else
COM_CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/cjson/inc
endif
COM_CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/lwip/ports/include
COM_CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/lwip/src/include
COM_CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/lwip/src/include/lwip
COM_CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/dhcpd/inc
COM_CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/httpclient/inc
COM_CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/mbedtls/include
COM_CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/mbedtls/configs
COM_CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/minicli/inc
COM_CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/minisupp/inc
COM_CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/mqtt/MQTTClient-C/src/mediatek
COM_CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/mqtt/MQTTClient-C/src
COM_CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/mqtt/MQTTPacket/src
COM_CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/nghttp2/lib/includes
COM_CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/nghttp2/lib/includes/nghttp2
COM_CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/nvdm/inc
COM_CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/smtcn/inc
COM_CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/sntp/inc
COM_CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/xml/inc
COM_CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/httpd/inc
COM_CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/iperf/inc
COM_CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/ping/inc
COM_CFLAGS += -I$(SOURCE_DIR)/project/common/bsp_ex/inc
COM_CFLAGS += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/portable/GCC/ARM_CM4F
COM_CFLAGS += -I$(SOURCE_DIR)/kernel/service/inc
#Homekit & Security
COM_CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/curve25519/inc
COM_CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/ed25519/inc
COM_CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/chacha20poly1305/inc
COM_CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/srp/inc

CFLAGS     += -std=gnu99 $(COM_CFLAGS)
CPPFLAGS   += -std=c++11 $(COM_CFLAGS)

#Middleware Module Path
MID_TFTP_PATH 		= $(SOURCE_DIR)/middleware/MTK/tftp
MID_FOTA_PATH 		= $(SOURCE_DIR)/middleware/MTK/fota
MID_LWIP_PATH 		= $(SOURCE_DIR)/middleware/third_party/lwip
MID_DHCPD_PATH 		= $(SOURCE_DIR)/middleware/third_party/dhcpd
MID_HTTPCLIENT_PATH = $(SOURCE_DIR)/middleware/third_party/httpclient
ifeq ($(MTK_JOYLINK_ENABLE),y)
MID_JOYLINK_PATH 	= $(SOURCE_DIR)/middleware/third_party/cloud/jd_joylink
else
MID_CJSON_PATH 		= $(SOURCE_DIR)/middleware/third_party/cjson
endif
MID_MBEDTLS_PATH 	= $(SOURCE_DIR)/middleware/third_party/mbedtls
MID_MINICLI_PATH 	= $(SOURCE_DIR)/middleware/MTK/minicli
MID_MINISUPP_PATH 	= $(SOURCE_DIR)/middleware/MTK/minisupp
MID_MQTT_PATH 		= $(SOURCE_DIR)/middleware/third_party/mqtt
MID_NGHTTP2_PATH 	= $(SOURCE_DIR)/middleware/third_party/nghttp2
MID_NVDM_PATH       = $(SOURCE_DIR)/middleware/MTK/nvdm
MID_SMTCN_PATH 		= $(SOURCE_DIR)/middleware/MTK/smtcn
MID_BLE_SMTCN_PATH	= $(SOURCE_DIR)/middleware/MTK/ble_smtcn
MID_AIRKISS_ADAPTER_PATH = $(SOURCE_DIR)/middleware/third_party/cloud/tencent_weixin
MID_SNTP_PATH 		= $(SOURCE_DIR)/middleware/third_party/sntp
MID_XML_PATH 		= $(SOURCE_DIR)/middleware/third_party/xml
MID_HTTPD_PATH 		= $(SOURCE_DIR)/middleware/third_party/httpd
MID_PING_PATH 		= $(SOURCE_DIR)/middleware/third_party/ping
MID_IPERF_PATH 		= $(SOURCE_DIR)/middleware/MTK/iperf
MID_BSPEXT_PATH		= $(SOURCE_DIR)/project/common/bsp_ex
DRV_CHIP_PATH 		= $(SOURCE_DIR)/driver/chip/mt7687
DRV_BSP_PATH 		= $(SOURCE_DIR)/driver/board/mt76x7_hdk
KRL_OS_PATH 		= $(SOURCE_DIR)/kernel/rtos/FreeRTOS
KRL_SRV_PATH		= $(SOURCE_DIR)/kernel/service
#Homekit & Security
MID_HOMEKIT_PATH        =  $(SOURCE_DIR)/$(HOMEKIT_DIR)
MID_CURVE25519_PATH        =  $(SOURCE_DIR)/middleware/third_party/curve25519
MID_ED25519_PATH        =  $(SOURCE_DIR)/middleware/third_party/ed25519
MID_CHACHA20POLY1305_PATH        =  $(SOURCE_DIR)/middleware/third_party/chacha20poly1305
MID_SRP_PATH        =  $(SOURCE_DIR)/middleware/third_party/srp
