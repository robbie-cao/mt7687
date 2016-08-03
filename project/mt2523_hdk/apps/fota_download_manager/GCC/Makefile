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

RAM_BOOTING=0

DEBUG = 0
FLOAT_TYPE = hard
BUILD_DIR = $(PWD)/Build

# Project name
PROJ_NAME = fota_download_manager
PROJ_PATH = $(PWD)
OUTPATH = $(PWD)/Build

#GNSS Bin Config
GNSS_BIN = $(SOURCE_DIR)/middleware/MTK/gnss/gnss_chip_fw.bin

# Main APP files
APP_PATH = project/mt2523_hdk/apps/fota_download_manager

#include module makefile
include $(SOURCE_DIR)/$(APP_PATH)/module.mk

# GNSS files
include $(SOURCE_DIR)/middleware/MTK/gnss/module.mk

# kernel service files
#LIBS += $(OUTPATH)/libservice.a

APP_PATH_SRC = $(APP_PATH)/src
BT_NOTIFY_SRC = $(APP_PATH_SRC)/btnotify_src
APP_FILES = $(APP_PATH_SRC)/main.c \
			$(APP_PATH)/GCC/syscalls.c \

C_FILES += $(APP_FILES)
C_FILES += driver/board/mt25x3_hdk/ept/src/bsp_gpio_ept_config.c

C_FILES += $(APP_PATH_SRC)/system_mt2523.c
C_FILES += $(APP_PATH_SRC)/ept_gpio_var.c
C_FILES += $(APP_PATH_SRC)/ept_eint_var.c
C_FILES += $(APP_PATH_SRC)/gnss_src/epo_download.c
C_FILES += $(APP_PATH_SRC)/hci_log.c

# add startup file to build
S_FILES += $(APP_PATH)/GCC/startup_mt2523.s

C_OBJS = $(C_FILES:%.c=$(BUILD_DIR)/%.o)
S_OBJS = $(S_FILES:%.s=$(BUILD_DIR)/%.o)

CFLAGS += -DMTK_FW_VERSION=\"$(MTK_FW_VERSION)\" 

###################################################
# include path
CFLAGS += -I$(SOURCE_DIR)/$(APP_PATH)/inc
CFLAGS += -I$(SOURCE_DIR)/driver/chip/mt2523/include
CFLAGS += -I$(SOURCE_DIR)/driver/chip/mt7687/inc
CFLAGS += -I$(SOURCE_DIR)/driver/chip/inc
CFLAGS += -I$(SOURCE_DIR)/driver/board/mt25x3_hdk/ept/inc
CFLAGS += -I$(SOURCE_DIR)/driver/CMSIS/Device/MTK/mt7687/Include

# BLE include path
CFLAGS += -I$(SOURCE_DIR)/$(APP_PATH)/inc/ble_app
CFLAGS += -I$(SOURCE_DIR)/$(APP_PATH)/inc/commandLine
CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/lwip/src/ports
CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/lwip/src/ports/include
CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/lwip/src/lwip-master/src/include
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/bluetooth/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/nvdm/inc

###################################################
# Libraries
LIBS += $(SOURCE_DIR)/$(APP_PATH)/lib/libbtnotify.a
LIBS += $(SOURCE_DIR)/middleware/MTK/bluetooth/lib/libbt_2523.a
LIBS += $(OUTPATH)/libfota.a
LIBS += $(OUTPATH)/libhal.a
LIBS += $(OUTPATH)/librtos.a
LIBS += $(OUTPATH)/libbsp.a
LIBS += $(OUTPATH)/libservice.a
# NVDM files
LIBS += $(OUTPATH)/libnvdm_CM4_GCC.a

###################################################
# Module folder path
MODULE_PATH += $(KRL_OS_PATH)
MODULE_PATH += $(DRV_CHIP_PATH)
MODULE_PATH += $(KRL_SRV_PATH)
MODULE_PATH += $(DRV_BSP_PATH)
MODULE_PATH += $(MID_FOTA_PATH)
MODULE_PATH += $(MID_NVDM_PATH)

###################################################
# LD Flags
LDFLAGS = $(ALLFLAGS) $(FPUFLAGS) --specs=nano.specs -lnosys -nostartfiles
ifeq ($(RAM_BOOTING), 1)
LDFLAGS += -Wl,-Tram.ld -Wl,--gc-sections
else
LDFLAGS += -Wl,-Tflash.ld -Wl,--gc-sections
endif


###################################################
# Rules
.PHONY: proj clean $(MODULE_PATH)

all: proj
	@mkdir -p $(BUILD_DIR)
	@$(SIZE) $(OUTPATH)/$(PROJ_NAME).elf
	@echo "Generate Assembly from elf:"
	@$(OBJDUMP) -D -S $(OUTPATH)/$(PROJ_NAME).elf > $(OUTPATH)/$(PROJ_NAME).elf.s
	@$(SOURCE_DIR)/tools/scripts/build/copy_firmware.sh $(SOURCE_DIR) $(OUTPATH) $(IC_CONFIG) $(BOARD_CONFIG) $(PROJ_NAME).bin $(PWD)


MOD_EXTRA = BUILD_DIR=$(BUILD_DIR) OUTPATH=$(OUTPATH) PROJ_PATH=$(PROJ_PATH)

$(LIBS): $(MODULE_PATH)

$(MODULE_PATH):
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
	@cp $(GNSS_BIN) $(OUTPATH)/gnss_firmware.bin
	@echo Done


include $(SOURCE_DIR)/.rule.mk

clean:
	rm -rf $(OUTPATH)
	+make -C $(MID_MINISUPP_PATH) $(MOD_EXTRA) clean
	#+make -C $(MID_BLE_PATH) clean

print-%  : ; @echo $* = $($*)
