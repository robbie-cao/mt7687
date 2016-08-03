SOURCE_DIR = ../../../../..
BINPATH = ~/gcc-arm-none-eabi/bin

PWD= $(shell pwd)

FEATURE ?= feature.mk
include $(FEATURE)

# Gloabl Config
-include $(SOURCE_DIR)/.config
# IC Config
-include $(SOURCE_DIR)/config/chip/$(IC_CONFIG)/chip.mk
# Board Config
-include $(SOURCE_DIR)/config/board/$(BOARD_CONFIG)/board.mk


ifeq ($(MTK_CLI_EXAMPLE_MODE_ENABLE),y)
CFLAGS         += -DMTK_CLI_EXAMPLE_MODE_ENABLE
endif


RAM_BOOTING=0

DEBUG = 0
FLOAT_TYPE = hard
BUILD_DIR = $(PWD)/Build

# Project name
PROJ_NAME = mt7697_iot_sdk
PROJ_PATH = $(PWD)
OUTPATH = $(PWD)/Build

# Main APP files
APP_PATH = project/mt7697_hdk/apps/iot_sdk
APP_PATH_SRC = $(APP_PATH)/src
BQB_PATH_SRC = $(SOURCE_DIR)/$(APP_PATH)/src_protected/bqb

# bqb files
ifneq ($(wildcard $(BQB_PATH_SRC)),)
include $(BQB_PATH_SRC)/module.mk
endif

APP_FILES       = $(APP_PATH_SRC)/main.c     \
                  $(APP_PATH_SRC)/sys_init.c \
                  $(APP_PATH_SRC)/network_init.c \
                  $(APP_PATH_SRC)/user_config.c  \
                  $(APP_PATH_SRC)/hci_log.c  \
                  $(APP_PATH_SRC)/bt_init.c \
                  $(APP_PATH_SRC)/ble_smtcn_app.c \
                  $(APP_PATH_SRC)/ut_app/ut_app.c \
                  $(APP_PATH_SRC)/ut_app/ut_app_gap.c \
                  $(APP_PATH_SRC)/ut_app/connection_info.c \
                  $(APP_PATH_SRC)/ut_app/gatt_service.c \
                  $(APP_PATH_SRC)/ut_app/gatt_command.c \
                  $(APP_PATH_SRC)/../../../../../middleware/MTK/bluetooth/src/hb/bt_cli.c \
                  $(APP_PATH_SRC)/../../../../../middleware/MTK/bluetooth/src/hb/bt_os_layer_api.c \
                  $(APP_PATH_SRC)/../../../../../middleware/MTK/bluetooth/src/hb/bt_debug.c \
                  $(APP_PATH_SRC)/../../../../../middleware/MTK/bluetooth/src/hb/bt_hci_log.c \
                  $(APP_PATH)/GCC/syscalls.c \
                  driver/board/mt76x7_hdk/init/src/io_def.c


CFLAGS += -DBT_DEBUG

SYS_FILES       = $(APP_PATH_SRC)/system_mt7687.c

ifeq ($(MTK_MINICLI_ENABLE),y)
APP_FILES       += $(APP_PATH_SRC)/cli_def.c
APP_FILES       += $(APP_PATH_SRC)/cli_cmds.c
endif

ifeq ($(MTK_HOMEKIT_ENABLE),y)
CFLAGS          += -I$(SOURCE_DIR)/middleware/MTK/homekit/inc
endif

C_FILES += $(APP_FILES)
C_FILES += $(APP_PATH_SRC)/system_mt7687.c

# add startup file to build
S_FILES += $(APP_PATH)/GCC/startup_mt7687.s

CFLAGS += -DMTK_BT_LWIP_ENABLE
C_FILES += $(APP_PATH_SRC)/ut_app/bt_lwip.c

# add ept file to build
C_FILES += driver/board/mt76x7_hdk/ept/src/bsp_gpio_ept_config.c
C_FILES += $(APP_PATH_SRC)/ept_gpio_var.c
C_FILES += $(APP_PATH_SRC)/ept_eint_var.c

#include BLE DEMO files

CFLAGS += -DMTK_FW_VERSION=\"$(MTK_FW_VERSION)\"

C_OBJS = $(C_FILES:%.c=$(BUILD_DIR)/%.o)
S_OBJS = $(S_FILES:%.s=$(BUILD_DIR)/%.o)



###################################################
# include path
CFLAGS += -I$(SOURCE_DIR)/$(APP_PATH)/inc
CFLAGS += -I$(SOURCE_DIR)/driver/chip/mt7687/inc
CFLAGS += -I$(SOURCE_DIR)/driver/chip/inc
# BLE include path
CFLAGS += -I$(SOURCE_DIR)/$(APP_PATH)/src/ut_app
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/bluetooth/inc/hb

ifeq ($(MTK_BLE_SMTCN_ENABLE),y)
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/ble_smtcn/inc
endif

CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/lwip/src/ports
CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/lwip/src/ports/include
CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/lwip/src/lwip-master/src/include
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/minisupp/src_protected/wpa_supplicant_8.jb4_1/wpa_supplicant

CFLAGS += -I$(SOURCE_DIR)/driver/board/mt76x7_hdk/ept/inc

###################################################
# LD Flags
LDFLAGS = $(ALLFLAGS) $(FPUFLAGS) --specs=nano.specs -lnosys -nostartfiles
ifeq ($(RAM_BOOTING), 1)
LDFLAGS += -Wl,-Tmt7687_sram.ld -Wl,--gc-sections
else
LDFLAGS += -Wl,-Tmt7687_flash.ld -Wl,--gc-sections
endif

ifneq ($(MTK_DEBUG_LEVEL), none)
LDFLAGS += -Wl,-wrap=printf
endif


##############################################################################
#
# Libraries
#
##############################################################################


ifeq ($(MTK_HOMEKIT_ENABLE),y)
LIBS += $(OUTPATH)/libhomekit.a
LIBS += $(OUTPATH)/libed25519.a
LIBS += $(OUTPATH)/libchacha20poly1305.a
LIBS += $(OUTPATH)/libcurve25519.a
LIBS += $(OUTPATH)/libsrp.a
endif
LIBS += $(OUTPATH)/libbspex.a
LIBS += $(OUTPATH)/libfota.a
LIBS += $(OUTPATH)/libtftp.a
LIBS += $(OUTPATH)/libcjson.a
LIBS += $(OUTPATH)/libhttpclient.a
LIBS += $(OUTPATH)/libmbedtls.a
LIBS += $(OUTPATH)/libminicli.a
ifeq ($(MTK_WIFI_WPS_ENABLE),y)
LIBS += $(OUTPATH)/libminisupp_wps.a
else
LIBS += $(OUTPATH)/libminisupp.a
endif
LIBS += $(OUTPATH)/libbsp.a
LIBS += $(OUTPATH)/libmqtt.a
LIBS += $(OUTPATH)/libnghttp2.a
LIBS += $(OUTPATH)/libnvdm_CM4_GCC.a
LIBS += $(OUTPATH)/libsntp.a
LIBS += $(OUTPATH)/libxml.a
LIBS += $(OUTPATH)/libiperf.a
# bluetooth files
LIBS += $(SOURCE_DIR)/middleware/MTK/bluetooth/lib/hb/libble_hb.a
LIBS += $(SOURCE_DIR)/middleware/MTK/bluetooth/lib/hb/libbtdriver_7697.a

LIBS += $(OUTPATH)/librtos.a
LIBS += $(OUTPATH)/libsmtcn.a
LIBS += $(OUTPATH)/libservice.a
LIBS += $(OUTPATH)/libping.a
LIBS += $(OUTPATH)/liblwip.a
LIBS += $(OUTPATH)/libhal.a
LIBS += $(OUTPATH)/libdhcpd.a
LIBS += $(SOURCE_DIR)/middleware/third_party/lzma_decoder/lib/liblzma.a
ifeq ($(MTK_BLE_SMTCN_ENABLE),y)
LIBS += $(OUTPATH)/libblesmtcn.a
endif

MODULE_PATH += $(KRL_OS_PATH)
MODULE_PATH += $(MID_TFTP_PATH)
MODULE_PATH += $(MID_LWIP_PATH)
MODULE_PATH += $(MID_CJSON_PATH)
MODULE_PATH += $(MID_DHCPD_PATH)
MODULE_PATH += $(MID_HTTPCLIENT_PATH)
MODULE_PATH += $(MID_MBEDTLS_PATH)
MODULE_PATH += $(MID_MINICLI_PATH)
MODULE_PATH += $(MID_MINISUPP_PATH)
MODULE_PATH += $(MID_MQTT_PATH)
MODULE_PATH += $(MID_NGHTTP2_PATH)
MODULE_PATH += $(MID_NVDM_PATH)
MODULE_PATH += $(MID_SMTCN_PATH)
MODULE_PATH += $(MID_SNTP_PATH)
MODULE_PATH += $(MID_XML_PATH)
MODULE_PATH += $(MID_BSPEXT_PATH)
MODULE_PATH += $(MID_BLE_SMTCN_PATH)
#MODULE_PATH += $(MID_BLE_PATH)
MODULE_PATH += $(DRV_CHIP_PATH)
MODULE_PATH += $(KRL_SRV_PATH)
MODULE_PATH += $(DRV_BSP_PATH)
MODULE_PATH += $(MID_PING_PATH)
MODULE_PATH += $(MID_IPERF_PATH)
MODULE_PATH += $(MID_FOTA_PATH)
ifeq ($(MTK_HOMEKIT_ENABLE),y)
MODULE_PATH += $(MID_HOMEKIT_PATH)
MODULE_PATH += $(MID_CURVE25519_PATH)
MODULE_PATH += $(MID_ED25519_PATH)
MODULE_PATH += $(MID_CHACHA20POLY1305_PATH)
MODULE_PATH += $(MID_SRP_PATH)
endif

###################################################
# Rules
.PHONY: proj clean $(MODULE_PATH)

all: proj
	@mkdir -p $(BUILD_DIR)
	@$(SIZE) $(OUTPATH)/$(PROJ_NAME).elf
	@echo "Generate Assembly from elf:"
	#@echo cp $(SOURCE_DIR)/$(APP_PATH)/download_config/* $(OUTPATH)/
	#@cp $(SOURCE_DIR)/$(APP_PATH)/download_config/flash_download.txt $(OUTPATH)/
	#@$(OBJDUMP) -S $(OUTPATH)/$(PROJ_NAME).elf > $(OUTPATH)/$(PROJ_NAME).elf.s
	#@cp $(SOURCE_DIR)/driver/board/mt76x7_hdk/bootloader/loader_inflash.bin $(OUTPATH)/
	@$(SOURCE_DIR)/tools/scripts/build/copy_firmware.sh $(SOURCE_DIR) $(OUTPATH) mt7697 $(BOARD_CONFIG) $(PROJ_NAME).bin $(PWD)
	@cp *.cmm $(OUTPATH)/


MOD_EXTRA = BUILD_DIR=$(BUILD_DIR) OUTPATH=$(OUTPATH) PROJ_PATH=$(PROJ_PATH)

$(LIBS): $(MODULE_PATH)

$(MODULE_PATH):
ifeq ($(MTK_HOMEKIT_ENABLE),y)
	@if [ ! -d "$(MID_HOMEKIT_PATH)" ]; then echo "\n####################################################################################################\nBuild Error: there isn't homekit folder in your codebase.\nYou should apply to the MTK for the related files of homekit. \n####################################################################################################\n"; exit 1;fi
endif
	@+make -C $@ $(MOD_EXTRA) $($@_EXTRA)


proj: $(OUTPATH)/$(PROJ_NAME).elf

$(OUTPATH)/$(PROJ_NAME).elf: $(C_OBJS) $(S_OBJS) $(LIBS)
	@echo Linking...
	@if [ -e "$@" ]; then rm -f "$@"; fi
	@if [ -e "$(OUTPATH)/$(PROJ_NAME).map" ]; then rm -f "$(OUTPATH)/$(PROJ_NAME).map"; fi
	@if [ -e "$(OUTPATH)/$(PROJ_NAME).hex" ]; then rm -f "$(OUTPATH)/$(PROJ_NAME).hex"; fi
	@if [ -e "$(OUTPATH)/$(PROJ_NAME).bin" ]; then rm -f "$(OUTPATH)/$(PROJ_NAME).bin"; fi
	@$(CC) $(LDFLAGS) -Wl,--start-group $^ -Wl,--end-group -Wl,-Map=$(OUTPATH)/$(PROJ_NAME).map -lm -o $@ 2>>$(ERR_LOG)
	@$(OBJCOPY) -O ihex $(OUTPATH)/$(PROJ_NAME).elf $(OUTPATH)/$(PROJ_NAME).hex
	@$(OBJCOPY) -O binary $(OUTPATH)/$(PROJ_NAME).elf $(OUTPATH)/$(PROJ_NAME).bin
	@echo Done

include $(SOURCE_DIR)/.rule.mk

clean:
	rm -rf $(OUTPATH)
	if [ -e $(MID_MINISUPP_PATH) ]; then \
		make -C $(MID_MINISUPP_PATH) $(MOD_EXTRA) clean \
	;fi
	#make -C $(MID_BLE_PATH) clean


print-%  : ; @echo $* = $($*)
