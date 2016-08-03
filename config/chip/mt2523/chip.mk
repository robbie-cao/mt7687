PRODUCT_VERSION=2523

# Default common setting
MTK_BUILD_SMT_LOAD ?= n

AR      = $(BINPATH)/arm-none-eabi-ar
CC      = $(BINPATH)/arm-none-eabi-gcc
CXX     = $(BINPATH)/arm-none-eabi-g++
OBJCOPY = $(BINPATH)/arm-none-eabi-objcopy
SIZE    = $(BINPATH)/arm-none-eabi-size
OBJDUMP = $(BINPATH)/arm-none-eabi-objdump


ALLFLAGS = -mlittle-endian -mthumb -mcpu=cortex-m4
FPUFLAGS = -fsingle-precision-constant -Wdouble-promotion -mfpu=fpv4-sp-d16 -mfloat-abi=hard

#CFLAGS += $(ALLFLAGS) $(FPUFLAGS) -flto -ffunction-sections -fdata-sections -fno-builtin
CFLAGS += $(ALLFLAGS) $(FPUFLAGS) -ffunction-sections -fdata-sections -fno-builtin
CFLAGS += -gdwarf-2 -Os -fno-strict-aliasing -fno-common
CFLAGS += -Wimplicit-function-declaration -Wall -Werror=uninitialized -Wno-error=maybe-uninitialized -Werror=return-type
CFLAGS += -std=gnu99 -Wno-switch
CFLAGS += -DPCFG_OS=2 -D_REENT_SMALL
CFLAGS += -DPRODUCT_VERSION=$(PRODUCT_VERSION)

## MTK_GNSS_ENABLE
## Brief:       This option is to enable and disable GNSS.
## Usage:       To enable GNSS support in a project, use "include $(SOURCE_DIR)/middleware/MTK/gnss/module.mk" to include the middleware.
##              Additionally, switch on this option to make build script to copy GNSS firmware image.
## Path:        middleware/MTK/gnss
## Dependency:  Add "include $(SOURCE_DIR)/middleware/MTK/gnss/module.mk" in the Makefile to link GNSS middleware.
## Notice:      Keep the copy_firmware.sh statement in Makefile to copy GNSS firmware image.
## Relative doc:Find the GNSS section in API reference.
##
ifeq ($(MTK_GNSS_ENABLE),y)
CFLAGS         += -DMTK_GNSS_ENABLE
export MTK_GNSS_ENABLE=y
endif

ifeq ($(MTK_BT_AT_COMMAND_ENABLE),y)
CFLAGS         += -DMTK_BT_AT_COMMAND_ENABLE
endif

##
## MTK_CTP_ENABLE
## Brief:       This option is to enable BSP CTP APIs under driver/board/component/touch_panel folder.
## Usage:       If the value is "y", the MTK_CTP_ENABLE compile option will be defined, the sources and header files under driver/board/component/touch_panel,
##				driver/board/component/common, driver/board/mt25x3_hdk/touch_panel will be included by driver/board/mt25x3_hdk/module.mk.
## Path:        driver/board/component/touch_panel, driver/board/component/common, driver/board/mt25x3_hdk/touch_panel.
## Dependency:  Must enable DHAL_GPT_MODULE_ENABLED, HAL_I2C_MASTER_MODULE_ENABLED, HAL_EINT_MODULE_ENABLED, HAL_NVIC_MODULE_ENABLED, HAL_GPIO_MODULE_ENABLED in the hal_feature_config.h under the project inc folder.
## Notice:      The default implementation of CTP under driver/board/mt25x3_hdk/touch_panel is none.
##              Please configure the GPIO for CTP by EPT tool. If you want to modify I2C parametr, please refer to driver/board/mt25x3_hdk/touch_panel.
## Relative doc:None
##
ifeq ($(MTK_CTP_ENABLE),y)
CFLAGS         += -DMTK_CTP_ENABLE
endif


##
## MTK_LED_ENABLE
## Brief:       This option is to enable BSP LED APIs under driver/board/component/led folder.
## Usage:       If the value is "y", the MTK_LED_ENABLE compile option will be defined, the sources and header files under driver/board/component/led and driver/board/mt25x3_hdk/led will be included by driver/board/mt25x3_hdk/module.mk.
## Path:        driver/board/component/led, driver/board/mt25x3_hdk/led.
## Dependency:  Must enable HAL_GPIO_MODULE_ENABLED in the hal_feature_config.h under the project inc folder.
## Notice:      The default implementation of LED under driver/board/mt25x3_hdk/led is none.
##              Please configure the GPIO for LED under driver/board/mt25x3_hdk/led. For more information, please refer to driver/board/mt76x7_hdk/led.
## Relative doc:None
##
ifeq ($(MTK_LED_ENABLE),y)
CFLAGS         += -DMTK_LED_ENABLE
endif


##
## MTK_SMART_BATTERY_ENABLE
## Brief:       This option is to enable and disable battery_management feature.
##              The battery_management include the charging and gauge function.
## Usage:       If the value is "y", the MTK_SMART_BATTERY_ENABLE compile option will be defined.
##              You must also include the gva/middleware/MTK/battery_management/module.mk and gva/middleware/MTK/battery_management/lib/libfgauge.a
##              in your Makefile before setting the option to "y".
## Path:        middleware/MTK/battery_management
## Dependency:  HAL_CHARGER_MODULE_ENABLED,HAL_SLEEP_MANAGER_ENABLED,HAL_RTC_MODULE_ENABLED,HAL_GPT_MODULE_ENABLED,HAL_PMU_MODULE_ENABLED,HAL_PMU_AUXADC_MODULE_ENABLED
##              must also defined in the hal_feature_config.h under the project inc folder.
##              Please also refer each module dependency.
## Notice:      Please ensure the following files are in the project inc folder.
##              Such as:cust_charging.h,cust_battery_meter.h,cust_battery_meter_table.h,cust_battery_temperature_table.h
## Relative doc:None
##
ifeq ($(MTK_SMART_BATTERY_ENABLE), y)
CFLAGS         += -DMTK_SMART_BATTERY_ENABLE
endif

ifeq ($(MTK_USB_DEMO_ENABLED),y)
CFLAGS         += -DMTK_USB_DEMO_ENABLED
endif

ifeq ($(MTK_MINICLI_ENABLE),y)
LDFLAGS        += -lminicli
CFLAGS         += -DMTK_MINICLI_ENABLE
endif

##
## MTK_FOTA_ENABLE
## Brief:       This option is to enable and disable FOTA.
## Usage:       If the value is "y", the MTK_FOTA_ENABLE compile option will be defined. You must also include the gva3\middleware\MTK\fota\module.mk in your Makefile before setting the option to "y". 
## Path:        middleware/MTK/fota
## Dependency:  HAL_FLASH_MODULE_ENABLED and HAL_WDT_MODULE_ENABLED must also defined in the hal_feature_config.h  under the project inc folder. If MTK_FOTA_CLI_ENABLE defined in the module.mk is "y", please also include the middleware/MTK/tftp/module.mk. 
## Notice:      Please use the driver/CMSIS/Device/MTK/mt2523/linkerscript/gcc/full_bin_fota/flash.ld for gcc build environment. 
##              Please use the driver/CMSIS/Device/MTK/mt2523/linkerscript/rvct/full_bin_fota/flash.sct for KEIL build environment. 
##              Please use the driver/CMSIS/Device/MTK/mt2523/linkerscript/iar/full_bin_fota/flash.icf for IAR build environment.
## Relative doc:Please refer to the firmware update developer's guide under the doc folder for more detail.
##
ifeq ($(MTK_FOTA_ENABLE),y)
CFLAGS += -DMTK_FOTA_ENABLE
endif

ifeq ($(MTK_BSP_LOOPBACK_ENABLE),y)
CFLAGS += -DMTK_BSP_LOOPBACK_ENABLE
endif

##
## MTK_OS_CPU_UTILIZATION_ENABLE
## Brief:       This option is to enable and disable cpu utilization function.
## Usage:     If the value is "y", the MTK_OS_CPU_UTILIZATION_ENABLE compile option will be defined and supports the 'os 2' MTK CLI commands to show/get statistics of CM4 CPU utilizations of all the tasks running on.You must also include the gva3\kernel\service\module.mk in your Makefile before setting the option to "y". 
## Path:       kernel/service
## Dependency:  MTK_MINICLI_ENABLE must be enabled in this file.
## Notice:      None
## Relative doc:None
##
ifeq ($(MTK_OS_CPU_UTILIZATION_ENABLE),y)
CFLAGS += -DMTK_OS_CPU_UTILIZATION_ENABLE
endif

##
## MTK_SENSOR_ACCELEROMETER_USE
## Brief:        This option is to enable and select accelerometer driver under driver/board/component/mems folder.
## Usage:        If the value is in the supported list (e.g. BMA255), the MTK_SENSOR_ACCELEROMETER_USE and correspoding selection (e.g. MTK_SENSOR_ACCELEROMETER_USE_BMA255) compile options will be defined, 
##               and the corresponding sources and header files under driver/board/component/mems will be included by driver/board/component/mems/module.mk.
## Path:         driver/board/component/mems
## Dependency:   Must enable HAL_SPI_MASTER_MODULE_ENABLED, HAL_I2C_MASTER_MODULE_ENABLED (dependent on the sensor hardware connection), and HAL_EINT_MODULE_ENABLED 
##               in the hal_feature_config.h under the project inc folder.
## Notice:       None
## Relative doc: None
##
ifeq ($(MTK_SENSOR_ACCELEROMETER_USE),BMA255)
  CFLAGS += -DMTK_SENSOR_ACCELEROMETER_USE
  CFLAGS += -DMTK_SENSOR_ACCELEROMETER_USE_BMA255
endif

ifeq ($(MTK_SENSOR_ACCELEROMETER_USE),BMI160)
  CFLAGS += -DMTK_SENSOR_ACCELEROMETER_GYROSCOPE_USE
  CFLAGS += -DMTK_SENSOR_ACCELEROMETER_GYROSCOPE_USE_BMI160
endif

##
## MTK_SENSOR_MAGNETIC_USE
## Brief:        This option is to enable and select magnetic field sensor driver under driver/board/component/mems folder.
## Usage:        If the value is in the supported list (e.g. YAS33), the MTK_SENSOR_MAGNETIC_USE and correspoding selection (e.g. MTK_SENSOR_MAGNETIC_USE_YAS533) compile options will be defined, 
##               and the corresponding sources and header files under driver/board/component/mems will be included by driver/board/component/mems/module.mk.
## Path:         driver/board/component/mems
## Dependency:   Must enable HAL_SPI_MASTER_MODULE_ENABLED, HAL_I2C_MASTER_MODULE_ENABLED (dependent on the sensor hardware connection), and HAL_EINT_MODULE_ENABLED 
##               in the hal_feature_config.h under the project inc folder.
## Notice:       None
## Relative doc: None
##
ifeq ($(MTK_SENSOR_MAGNETIC_USE),YAS533)
  CFLAGS += -DMTK_SENSOR_MAGNETIC_USE
  CFLAGS += -DMTK_SENSOR_MAGNETIC_USE_YAS533
endif

##
## MTK_SENSOR_BAROMETER_USE
## Brief:        This option is to enable and select barometric sensor driver under driver/board/component/mems folder.
## Usage:        If the value is in the supported list (e.g. BMP280), the MTK_SENSOR_BAROMETER_USE and correspoding selection (e.g. MTK_SENSOR_BAROMETER_USE_BMP280) compile options will be defined, 
##               and the corresponding sources and header files under driver/board/component/mems will be included by driver/board/component/mems/module.mk.
## Path:         driver/board/component/mems
## Dependency:   Must enable HAL_SPI_MASTER_MODULE_ENABLED, HAL_I2C_MASTER_MODULE_ENABLED (dependent on the sensor hardware connection), and HAL_EINT_MODULE_ENABLED 
##               in the hal_feature_config.h under the project inc folder.
## Notice:       None
## Relative doc: None
##
ifeq ($(MTK_SENSOR_BAROMETER_USE),BMP280)
  CFLAGS += -DMTK_SENSOR_BAROMETER_USE
  CFLAGS += -DMTK_SENSOR_BAROMETER_USE_BMP280
endif

##
## MTK_SENSOR_PROXIMITY_USE
## Brief:        This option is to enable and select proximity sensor driver under driver/board/component/mems folder.
## Usage:        If the value is in the supported list (e.g. CM36672), the MTK_SENSOR_PROXIMITY_USE and correspoding selection (e.g. MTK_SENSOR_PROXIMITY_USE_CM36672) compile options will be defined, 
##               and the corresponding sources and header files under driver/board/component/mems will be included by driver/board/component/mems/module.mk.
## Path:         driver/board/component/mems
## Dependency:   Must enable HAL_SPI_MASTER_MODULE_ENABLED, HAL_I2C_MASTER_MODULE_ENABLED (dependent on the sensor hardware connection), and HAL_EINT_MODULE_ENABLED 
##               in the hal_feature_config.h under the project inc folder.
## Notice:       None
## Relative doc: None
##
ifeq ($(MTK_SENSOR_PROXIMITY_USE),CM36672)
  CFLAGS += -DMTK_SENSOR_PROXIMITY_USE
  CFLAGS += -DMTK_SENSOR_PROXIMITY_USE_CM36672
endif

##
## MTK_SENSOR_BIO_USE
## Brief:        This option is to enable and select biosensor driver under driver/board/component/biosensor folder.
## Usage:        If the value is in the supported list (e.g. MT2511), the MTK_SENSOR_BIO_USE and correspoding selection (e.g. MTK_SENSOR_BIO_USE_MT2511) compile options will be defined, 
##               and the corresponding sources and header files under driver/board/component/mems will be included by driver/board/component/biosensor/mt2511/module.mk.
## Path:         driver/board/component/biosensor
## Dependency:   Must enable HAL_SPI_MASTER_MODULE_ENABLED, HAL_I2C_MASTER_MODULE_ENABLED (dependent on the sensor hardware connection), and HAL_EINT_MODULE_ENABLED 
##               in the hal_feature_config.h under the project inc folder.
## Notice:       None
## Relative doc: None
##
ifeq ($(MTK_SENSOR_BIO_USE),MT2511)
  CFLAGS += -DMTK_SENSOR_BIO_USE
  CFLAGS += -DMTK_SENSOR_BIO_USE_MT2511
endif


##
## FUSION_HEART_RATE_MONITOR_USE
## Brief:        This option is to enable MediaTek heart rate monitor fusion algorithm library under middleware/MTK/sensor_subsys/lib/fusion_algo folder.
## Usage:        If the value is M_INHOUSE_HEART_RATE_MONITOR, the FUSION_HEART_RATE_MONITOR_USE compile option will be defined as M_INHOUSE_HEART_RATE_MONITOR, 
##               and the corresponding library and header files under middleware/MTK/sensor_subsys/lib/fusion_algo will be included by middleware/MTK/sensor_subsys/src/module.mk.
## Path:         middleware/MTK/sensor_subsys/lib/fusion_algo
## Dependency:   Must enable MTK_SENSOR_BIO_USE and MTK_SENSOR_ACCELEROMETER_USE in this file.
## Notice:       None
## Relative doc: None
##
ifeq ($(FUSION_HEART_RATE_MONITOR_USE),M_INHOUSE_HEART_RATE_MONITOR)
  CFLAGS += -DFUSION_HEART_RATE_MONITOR_USE=M_INHOUSE_HEART_RATE_MONITOR
endif

##
## FUSION_HEART_RATE_VARIABILITY_USE
## Brief:        This option is to enable MediaTek heart rate variability monitor fusion algorithm library under middleware/MTK/sensor_subsys/lib/fusion_algo folder.
## Usage:        If the value is M_INHOUSE_HEART_RATE_VARIABILITY, the FUSION_HEART_RATE_VARIABILITY_USE compile option will be defined as M_INHOUSE_HEART_RATE_VARIABILITY, 
##               and the corresponding library and header files under middleware/MTK/sensor_subsys/lib/fusion_algo will be included by middleware/MTK/sensor_subsys/src/module.mk.
## Path:         middleware/MTK/sensor_subsys/lib/fusion_algo
## Dependency:   Must enable MTK_SENSOR_BIO_USE in this file.
## Notice:       None
## Relative doc: None
##
ifeq ($(FUSION_HEART_RATE_VARIABILITY_USE),M_INHOUSE_HEART_RATE_VARIABILITY)
  CFLAGS += -DFUSION_HEART_RATE_VARIABILITY_USE=M_INHOUSE_HEART_RATE_VARIABILITY
endif


##
## MTK_MBEDTLS_CONFIG_FILE
## Brief:       This option is to configure mbedTLS features.
## Usage:       If the value is "*.h", mbedTLS module will use *.h as the configuration file. For example, if its value is "config-mtk-basic.h",
##              config-mtk-basic.h will be used as the configuration file. MTK_MBEDTLS_CONFIG_FILE compile option will be defined. You must also
##              include the gva/middleware/third_party/mbedtls/module.mk in your Makefile before setting the option to "*.h".
## Path:        middleware/third_party/mbedtls
## Dependency:  LWIP module must be enabled.
## Relative doc:Please refer to internet and open source software guide under the doc folder for more detail.
##
ifneq ($(MTK_MBEDTLS_CONFIG_FILE),)
CFLAGS += -DMBEDTLS_CONFIG_FILE=\"$(MTK_MBEDTLS_CONFIG_FILE)\"
endif

##
## MTK_HTTPCLIENT_SSL_ENABLE
## Brief:       This option is to switch SSL/TLS support in SSL client module.
## Usage:       To enable HTTP client support in a project, use "LIBS += $(OUTPATH)/libhttpclient.a" to include the middleware.
##              Additionally, switch this option to configure SSL support in HTTP client.
## Path:        middleware/third_party/httpclient
## Dependency:  LWIP and mbedTLS module must be enabled.
## Relative doc:Please refer to internet and open source software guide under the doc folder for more detail.
##
ifeq ($(MTK_HTTPCLIENT_SSL_ENABLE),y)
CFLAGS += -DMTK_HTTPCLIENT_SSL_ENABLE
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
## Relative doc:Please refer to doc/LinkIt_for_RTOS_System_Log_Developers_Guide.pdf
##
ifeq ($(MTK_DEBUG_LEVEL),)
  CFLAGS += -DMTK_DEBUG_LEVEL_INFO
  CFLAGS += -DMTK_DEBUG_LEVEL_WARNING
  CFLAGS += -DMTK_DEBUG_LEVEL_ERROR
endif

ifeq ($(MTK_DEBUG_LEVEL),error)
  CFLAGS += -DMTK_DEBUG_LEVEL_ERROR
endif

ifeq ($(MTK_DEBUG_LEVEL),warning)
  CFLAGS += -DMTK_DEBUG_LEVEL_WARNING
  CFLAGS += -DMTK_DEBUG_LEVEL_ERROR
endif

ifeq ($(MTK_DEBUG_LEVEL),info)
  CFLAGS += -DMTK_DEBUG_LEVEL_INFO
  CFLAGS += -DMTK_DEBUG_LEVEL_WARNING
  CFLAGS += -DMTK_DEBUG_LEVEL_ERROR
endif

ifeq ($(MTK_DEBUG_LEVEL),none)
  CFLAGS += -DMTK_DEBUG_LEVEL_NONE
endif
  
ifeq ($(MTK_BT_A2DP_AAC_ENABLE),y)
CFLAGS         += -DMTK_BT_A2DP_AAC_ENABLE
endif


##
## MTK_AUDIO_TUNING_ENABLED
## Brief:       This option is to enable Middleware audio tuning tool functions under middleware/MTK/atci/at_command/ folder.
## Usage:       If the value is "y", the MTK_AUDIO_TUNING_ENABLED compile option will be defined, the audio tuning tool sources and header files under middleware/MTK/atci/at_command/ and middleware/MTK/atci/inc/ will be included by middleware/MTK/atci/module.mk.
## Path:        middleware/MTK/atci/at_command/, middleware/MTK/atci/inc/
## Dependency:  Must enable MTK_AUDIO_TUNING_ENABLED in the hal_feature_config.h under the project inc folder.
## Notice:      The default implementation of audio tuning tool under middleware/MTK/atci/at_command/ is none. 
## Relative doc:None
##
ifeq ($(MTK_AUDIO_TUNING_ENABLED), y)
CFLAGS += -DMTK_AUDIO_TUNING_ENABLED
endif


#
# MTK_DEVELOPMENT_BOARD_TYPE is an option to specify the type of development board.
#  For example, EVB, HDK, FPGA, and so on.
#

ifeq ($(MTK_DEVELOPMENT_BOARD_TYPE),)
  CFLAGS += -DMTK_DEVELOPMENT_BOARD_HDK
else
  ifeq ($(MTK_DEVELOPMENT_BOARD_TYPE),EVB)
    CFLAGS += -DMTK_DEVELOPMENT_BOARD_EVB
  else ifeq ($(MTK_DEVELOPMENT_BOARD_TYPE),HDK)
    CFLAGS += -DMTK_DEVELOPMENT_BOARD_HDK
  else ifeq ($(MTK_DEVELOPMENT_BOARD_TYPE),FPGA)
    CFLAGS += -DMTK_DEVELOPMENT_BOARD_FPGA
  else
    CFLAGS += -DMTK_DEVELOPMENT_BOARD_HDK
  endif
endif

##
## MTK_XOSC_MODE
## Brief:       This option is to enable or disable XOSC mode in the RTC module.
## Usage:       If the value is "XOSC", the MTK_XOSC_MODE compile option will be defined, and the RTC module will use XOSC mode instead of SCXO mode.
## Path:        driver/chip/mt2523
## Dependency:  Must enable HAL_RTC_MODULE_ENABLED in the hal_feature_config.h under the project inc folder.
## Notice:      The default implementation of RTC under driver/chip/mt2523 is using SCXO mode. 
##              Please configure MTK_XOSC_MODE value as XOSC in project/mt2523_hdk/apps/<project>/gcc/feature.mk if user must use XOSC mode.
## Relative doc:None
##
ifeq ($(MTK_XOSC_MODE),XOSC)
  CFLAGS += -DUSE_XOSC_MODE
endif

##
## The following makefile options are not configurable and may be removed in the future. 
## Please do not set the makefile options to "y" in your GCC feature configuration.
##

ifeq ($(MTK_SYSLOG_DMA),y)
  CFLAGS += -DSYSLOG_DMA_ENABLE
endif

ifeq ($(MTK_MVG_FLASH_TEST_ENABLE), y)
CFLAGS += -D__NOR_FLASH_HARDWARE_TEST__
MVG_SRC = project/mt2523_hdk/test_loads/HQA/hqa_flash_test
CFLAGS += -I$(SOURCE_DIR)/$(MVG_SRC)/inc/mvg
endif

ifeq ($(MTK_SUPPORT_SFC_DVT_ENABLE),y)
  CFLAGS += -D__SF_DVT__
endif

ifeq ($(MTK_BUILD_SMT_LOAD),y)
CFLAGS         += -DMTK_BUILD_SMT_LOAD
endif

ifeq ($(MTK_BLE_BQB_TEST_ENABLE),y)
CFLAGS += -DMTK_BLE_BQB_ENABLE
endif

ifeq ($(MTK_BT_BQB_TEST_ENABLE),y)
CFLAGS += -DMTK_BT_BQB_ENABLE
endif

ifeq ($(MTK_BT_NORMAL_MODE_ENABLE),y)
CFLAGS += -DMTK_BT_NORMAL_MODE_ENABLE
CFLAGS += -DBT_SINK_DEMO
CFLAGS += -DSUPPORT_BT_SINK_TEST_CASE
endif

ifeq ($(MTK_HEAP_GUARD_ENABLE),y)
ALLFLAGS       += -Wl,-wrap=pvPortMalloc -Wl,-wrap=vPortFree
CFLAGS         += -DHEAP_GUARD_ENABLE
endif

ifeq ($(MTK_HEAP_SIZE_GUARD_ENABLE),y)
ALLFLAGS       += -Wl,-wrap=pvPortMalloc -Wl,-wrap=vPortFree
CFLAGS         += -DMTK_HEAP_SIZE_GUARD_ENABLE
endif

ifeq ($(MTK_MET_TRACE_ENABLE),y)
CFLAGS         += -DMET_TRACE_ENABLE
endif

ifeq ($(MTK_NO_PSRAM_ENABLE),y)
CFLAGS         += -DMTK_NO_PSRAM_ENABLE
endif

ifeq ($(MTK_CODE_COVERAGE_ENABLE),y)
CC = $(BINPATH)/arm-none-eabi-cov-gcc
CXX= $(BINPATH)/arm-none-eabi-cov-g++
AR = $(BINPATH)/arm-none-eabi-cov-ar
export GCOV_DIR=$(SOURCE_DIR)
endif

ifeq ($(MTK_MAIN_CONSOLE_UART2_ENABLE),y)
CFLAGS         += -DMTK_MAIN_CONSOLE_UART2_ENABLE
endif

ifeq ($(FUSION_PEDOMETER_USE),M_INHOUSE_PEDOMETER)
  CFLAGS += -DFUSION_PEDOMETER_USE=M_INHOUSE_PEDOMETER
endif

ifeq ($(FUSION_SLEEP_TRACKER_USE),M_INHOUSE_SLEEP_TRACKER)
  CFLAGS += -DFUSION_SLEEP_TRACKER_USE=M_INHOUSE_SLEEP_TRACKER
endif

ifeq ($(FUSION_BLOOD_PRESSURE_USE),M_INHOUSE_BLOOD_PRESSURE)
  CFLAGS += -DFUSION_BLOOD_PRESSURE_USE=M_INHOUSE_BLOOD_PRESSURE
endif

#Incldue Path
CFLAGS += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS
CFLAGS += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/include
CFLAGS += -I$(SOURCE_DIR)/driver/CMSIS/Device/MTK/mt2523/Include
CFLAGS += -I$(SOURCE_DIR)/driver/CMSIS/Include
CFLAGS += -I$(SOURCE_DIR)/driver/chip/mt2523/inc
CFLAGS += -I$(SOURCE_DIR)/driver/chip/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/tftp/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/fota/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/gnss/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/cjson/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/lwip/src/include
CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/lwip/src/include/lwip
CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/lwip/ports/include
CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/dhcpd/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/httpclient/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/mbedtls/include
CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/mbedtls/configs
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/minicli/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/minisupp/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/mqtt/MQTTClient-C/src/mediatek
CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/mqtt/MQTTClient-C/src
CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/nghttp2/lib/includes
CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/nghttp2/lib/includes/nghttp2
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/nvdm/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/sntp/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/xml/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/httpd/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/iperf/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/ping/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/fatfs/src
CFLAGS += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/portable/GCC/ARM_CM4F
CFLAGS += -I$(SOURCE_DIR)/kernel/service/inc
CFLAGS += -I$(SOURCE_DIR)/kernel/service/src_protected/inc

#Middleware Module Path
MID_TFTP_PATH 		= $(SOURCE_DIR)/middleware/MTK/tftp
MID_FOTA_PATH 		= $(SOURCE_DIR)/middleware/MTK/fota
MID_GNSS_PATH 		= $(SOURCE_DIR)/middleware/MTK/gnss
MID_LWIP_PATH 		= $(SOURCE_DIR)/middleware/third_party/lwip
MID_CJSON_PATH 		= $(SOURCE_DIR)/middleware/third_party/cjson
MID_DHCPD_PATH 		= $(SOURCE_DIR)/middleware/third_party/dhcpd
MID_HTTPCLIENT_PATH = $(SOURCE_DIR)/middleware/third_party/httpclient
MID_MBEDTLS_PATH 	= $(SOURCE_DIR)/middleware/third_party/mbedtls
MID_MINICLI_PATH 	= $(SOURCE_DIR)/middleware/MTK/minicli
MID_MINISUPP_PATH 	= $(SOURCE_DIR)/middleware/MTK/minisupp
MID_MQTT_PATH 		= $(SOURCE_DIR)/middleware/third_party/mqtt
MID_NGHTTP2_PATH 	= $(SOURCE_DIR)/middleware/third_party/nghttp2
MID_NVDM_PATH 		= $(SOURCE_DIR)/middleware/MTK/nvdm
MID_SNTP_PATH 		= $(SOURCE_DIR)/middleware/third_party/sntp
MID_XML_PATH 		= $(SOURCE_DIR)/middleware/third_party/xml
MID_HTTPD_PATH 		= $(SOURCE_DIR)/middleware/third_party/httpd
MID_PING_PATH 		= $(SOURCE_DIR)/middleware/third_party/ping
MID_IPERF_PATH 		= $(SOURCE_DIR)/middleware/MTK/iperf
MID_FATFS_PATH 		= $(SOURCE_DIR)/middleware/third_party/fatfs
DRV_CHIP_PATH 		= $(SOURCE_DIR)/driver/chip/mt2523
DRV_BSP_PATH 		= $(SOURCE_DIR)/driver/board/mt25x3_hdk
DRV_G2D_PATH 		= $(SOURCE_DIR)/driver/board/component/g2d
KRL_OS_PATH 		= $(SOURCE_DIR)/kernel/rtos/FreeRTOS
KRL_SRV_PATH		= $(SOURCE_DIR)/kernel/service
MID_LZMA_PATH           = $(SOURCE_DIR)/middleware/third_party/lzma_decoder
