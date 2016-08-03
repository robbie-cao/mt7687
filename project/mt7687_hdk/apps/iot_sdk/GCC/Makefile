##############################################################################
#
# Project settings
#
##############################################################################


SOURCE_DIR  = ../../../../..
BINPATH     = ~/gcc-arm-none-eabi/bin
PWD         = $(shell pwd)
DATIME      = $(shell date --iso=seconds)
V          ?= 0

##############################################################################
#
# build log output
#
##############################################################################


ifeq ($(V),0)
Q           = @
else
Q           =
endif


##############################################################################
#
# config files
#
##############################################################################


FEATURE    ?= feature.mk

include $(FEATURE)

# Gloabl Config
-include $(SOURCE_DIR)/.config
# IC Config
-include $(SOURCE_DIR)/config/chip/$(IC_CONFIG)/chip.mk
# Board Config
-include $(SOURCE_DIR)/config/board/$(BOARD_CONFIG)/board.mk


##############################################################################
#
# Project specific features
#
##############################################################################


ifeq ($(MTK_CLI_EXAMPLE_MODE_ENABLE),y)
CFLAGS         += -DMTK_CLI_EXAMPLE_MODE_ENABLE
endif


##############################################################################
#
# config
#
##############################################################################


RAM_BOOTING     = 0

DEBUG           = 0
FLOAT_TYPE      = hard
BUILD_DIR       = $(PWD)/Build

# Project name
PROJ_NAME       = $(IC_CONFIG)_iot_sdk
PROJ_PATH       = $(PWD)
OUTPATH         = $(PWD)/Build

# Main APP files
APP_PATH        = project/$(IC_CONFIG)_hdk/apps/iot_sdk
APP_PATH_SRC    = $(APP_PATH)/src


##############################################################################
#
# SDK source files
#
##############################################################################


APP_FILES      += $(APP_PATH_SRC)/main.c
APP_FILES      += $(APP_PATH_SRC)/sys_init.c
APP_FILES      += $(APP_PATH_SRC)/network_init.c
APP_FILES      += $(APP_PATH_SRC)/user_config.c
APP_FILES      += $(APP_PATH_SRC)/ept_gpio_var.c
APP_FILES      += $(APP_PATH_SRC)/ept_eint_var.c
APP_FILES      += $(APP_PATH)/GCC/syscalls.c

ifeq ($(MTK_MINICLI_ENABLE),y)
APP_FILES      += $(APP_PATH_SRC)/cli_def.c
APP_FILES      += $(APP_PATH_SRC)/cli_cmds.c
endif

APP_FILES      += driver/board/mt76x7_hdk/ept/src/bsp_gpio_ept_config.c
APP_FILES      += driver/board/mt76x7_hdk/init/src/io_def.c

ifeq ($(MTK_LED_ENABLE),y)
APP_FILES      += driver/board/mt76x7_hdk/led/src/bsp_led_internal.c
APP_FILES      += driver/board/component/led/bsp_led.c
endif

SYS_FILES      += $(APP_PATH_SRC)/system_mt7687.c

C_FILES        += $(APP_FILES) $(SYS_FILES)

S_FILES        += $(APP_PATH)/GCC/startup_mt7687.s

##########################################
#
# Apple HomeKit
#
##########################################

ifeq ($(MTK_HOMEKIT_ENABLE),y)
CFLAGS          += -I$(SOURCE_DIR)/middleware/MTK/homekit/inc
CFLAGS          += -DDEBUG
endif

##############################################################################
#
# SDK object files
#
##############################################################################


C_OBJS          = $(C_FILES:%.c=$(BUILD_DIR)/%.o)
S_OBJS          = $(S_FILES:%.s=$(BUILD_DIR)/%.o)
CXX_OBJS        = $(CXX_FILES:%.cpp=$(BUILD_DIR)/%.o)


##############################################################################
#
# CFLAGS
#
##############################################################################


CFLAGS += -DMTK_FW_VERSION=\"$(MTK_FW_VERSION)\"


###################################################
# include path
CFLAGS += -I$(SOURCE_DIR)/$(APP_PATH)/inc
CFLAGS += -I$(SOURCE_DIR)/driver/chip/$(IC_CONFIG)/inc
CFLAGS += -I$(SOURCE_DIR)/driver/chip/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/lwip/src/include
CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/lwip/ports/include
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/minisupp/src_protected/wpa_supplicant_8.jb4_1/wpa_supplicant

ifeq ($(MTK_LED_ENABLE),y)
CFLAGS += -I$(SOURCE_DIR)/driver/board/component/led
CFLAGS += -I$(SOURCE_DIR)/driver/board/mt76x7_hdk/led/inc
endif

CFLAGS += -I$(SOURCE_DIR)/driver/board/mt76x7_hdk/ept/inc

##############################################################################
#
# LDFLAGS
#
##############################################################################


LDFLAGS += $(ALLFLAGS) $(FPUFLAGS) --specs=nano.specs -lnosys -nostartfiles
LDFLAGS += -u _printf_float -u _scanf_float

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
LIBS += $(OUTPATH)/libcjson.a
LIBS += $(OUTPATH)/libfota.a
LIBS += $(OUTPATH)/libtftp.a
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
LIBS += $(OUTPATH)/librtos.a
LIBS += $(OUTPATH)/libsmtcn.a
LIBS += $(OUTPATH)/libservice.a
LIBS += $(OUTPATH)/libping.a
LIBS += $(OUTPATH)/liblwip.a
LIBS += $(OUTPATH)/libhal.a
LIBS += $(OUTPATH)/libdhcpd.a
ifeq ($(MTK_MPERF_ENABLE),y)
LIBS += $(OUTPATH)/libmperf.a
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
ifeq ($(MTK_MPERF_ENABLE),y)
MODULE_PATH += $(MID_MPERF_PATH)
endif
##############################################################################
#
# Target Rules
#
##############################################################################


.PHONY: proj clean $(MODULE_PATH)

all: proj
	$(Q)mkdir -p $(BUILD_DIR)
	$(Q)$(SIZE) $(OUTPATH)/$(PROJ_NAME).elf
	$(Q)echo "Generate Assembly from elf:"
	#$(Q)$(OBJDUMP) -S $(OUTPATH)/$(PROJ_NAME).elf > $(OUTPATH)/$(PROJ_NAME).elf.s
	$(Q)$(SOURCE_DIR)/tools/scripts/build/copy_firmware.sh $(SOURCE_DIR) $(OUTPATH) $(IC_CONFIG) $(BOARD_CONFIG) $(PROJ_NAME).bin $(PWD)
	$(Q)cp *.cmm $(OUTPATH)/

MOD_EXTRA = BUILD_DIR=$(BUILD_DIR) OUTPATH=$(OUTPATH) PROJ_PATH=$(PROJ_PATH)

$(LIBS): $(MODULE_PATH)

$(MODULE_PATH):
ifeq ($(MTK_HOMEKIT_ENABLE),y)
	@if [ ! -d "$(MID_HOMEKIT_PATH)" ]; then echo "\n####################################################################################################\nBuild Error: there isn't homekit folder in your codebase.\nYou should apply to the MTK for the related files of homekit. \n####################################################################################################\n"; exit 1;fi
endif
	$(Q)+make -C $@ $(MOD_EXTRA) $($@_EXTRA)


proj: $(OUTPATH)/$(PROJ_NAME).elf

$(OUTPATH)/$(PROJ_NAME).elf: $(C_OBJS) $(CXX_OBJS) $(S_OBJS) $(LIBS)
	$(Q)echo Linking...
	$(Q)if [ -e "$@" ]; then rm -f "$@"; fi
	$(Q)if [ -e "$(OUTPATH)/$(PROJ_NAME).map" ]; then rm -f "$(OUTPATH)/$(PROJ_NAME).map"; fi
	$(Q)if [ -e "$(OUTPATH)/$(PROJ_NAME).hex" ]; then rm -f "$(OUTPATH)/$(PROJ_NAME).hex"; fi
	$(Q)if [ -e "$(OUTPATH)/$(PROJ_NAME).bin" ]; then rm -f "$(OUTPATH)/$(PROJ_NAME).bin"; fi
	$(Q)$(CXX) $(LDFLAGS) -Wl,--start-group $^ -Wl,--end-group -Wl,-Map=$(OUTPATH)/$(PROJ_NAME).map -lm -o $@ 2>>$(ERR_LOG)
	$(Q)$(OBJCOPY) -O ihex $(OUTPATH)/$(PROJ_NAME).elf $(OUTPATH)/$(PROJ_NAME).hex
	$(Q)$(OBJCOPY) -O binary $(OUTPATH)/$(PROJ_NAME).elf $(OUTPATH)/$(PROJ_NAME).bin
	$(Q)echo Done


include $(SOURCE_DIR)/.rule.mk

clean:
	$(Q)rm -rf $(OUTPATH)
	$(Q)if [ -e $(MID_MINISUPP_PATH) ]; then \
		make -C $(MID_MINISUPP_PATH) $(MOD_EXTRA) clean \
	;fi

.PHONY: options

options:
	@for opt in `sed 's/[()]/\n/g' $(SOURCE_DIR)/config/chip/$(IC_CONFIG)/chip.mk|sed 's/-D/ /'|sort|uniq|grep ^MTK_|grep _ENABLE$$`; do echo $$opt; done
print-%  : ; @echo $* = $($*)
