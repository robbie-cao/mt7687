SOURCE_DIR = ../../../../..
BINPATH = ~/gcc-arm-none-eabi/bin

PWD= $(shell pwd)
FEATURE ?= feature.mk
include $(FEATURE)
# Gloabl Config
include $(SOURCE_DIR)/.config
# IC Config
include $(SOURCE_DIR)/config/chip/$(IC_CONFIG)/chip.mk
# Board Config
include $(SOURCE_DIR)/config/board/$(BOARD_CONFIG)/board.mk


RAM_BOOTING=0

DEBUG = 0
FLOAT_TYPE = hard
BUILD_DIR = $(PWD)/Build

# Project name
PROJ_NAME = ble_ancs_ios_notification
PROJ_PATH = $(PWD)
OUTPATH = $(PWD)/Build

#GNSS Bin Config
GNSS_BIN = $(SOURCE_DIR)/middleware/MTK/gnss/gnss_chip_fw.bin

# Main APP files
APP_PATH = project/mt2523_hdk/apps/ble_ancs_ios_notification
APP_PATH_SRC = $(APP_PATH)/src
APP_FILES = $(APP_PATH_SRC)/main.c \
	    $(APP_PATH_SRC)/hci_log.c \
	    $(APP_PATH)/GCC/syscalls.c

APP_FILES += $(APP_PATH_SRC)/ept_gpio_var.c
APP_FILES += $(APP_PATH_SRC)/ept_eint_var.c

C_FILES += $(APP_PATH_SRC)/ble_gattc_connect.c
C_FILES += $(APP_PATH_SRC)/ble_gattc_handle_op.c
C_FILES += $(APP_PATH_SRC)/ble_gattc_utils_db.c
C_FILES += $(APP_PATH_SRC)/gattc_utils.c
C_FILES += $(APP_PATH_SRC)/ble_gattc_srv.c
C_FILES += $(APP_PATH_SRC)/ble_ancs_app.c
C_FILES += $(APP_PATH_SRC)/ble_common_callback.c
C_FILES += $(APP_FILES)
C_FILES += $(APP_PATH_SRC)/system_mt2523.c
C_FILES += driver/board/mt25x3_hdk/ept/src/bsp_gpio_ept_config.c
# add startup file to build
S_FILES += $(APP_PATH)/GCC/startup_mt2523.s


###################################################
# Check for valid float argument
# NOTE that you have to run make clan after
# changing these as hardfloat and softfloat are not
# binary compatible
ifneq ($(FLOAT_TYPE), hard)
ifneq ($(FLOAT_TYPE), soft)
override FLOAT_TYPE = hard
#override FLOAT_TYPE = soft
endif
endif

###################################################
# CC Flags
ifeq ($(DEBUG), 1)
ALLFLAGS = -g -O0
else
ALLFLAGS = -g -Os
endif
ALLFLAGS += -Wall -mlittle-endian -mthumb -mcpu=cortex-m4
CFLAGS += $(ALLFLAGS) -flto -ffunction-sections -fdata-sections -fno-builtin

ifeq ($(FLOAT_TYPE), hard)
FPUFLAGS = -fsingle-precision-constant -Wdouble-promotion
FPUFLAGS += -mfpu=fpv4-sp-d16 -mfloat-abi=hard
#CFLAGS += -mfpu=fpv4-sp-d16 -mfloat-abi=softfp
else
FPUFLAGS = -msoft-float
endif

# Definitions
CFLAGS += -D_REENT_SMALL
CFLAGS += -DARM_MATH_CM4
CFLAGS += -D__FPU_PRESENT
CFLAGS += $(FPUFLAGS)

# LD Flags
LDFLAGS = $(ALLFLAGS) $(FPUFLAGS) --specs=nano.specs -lnosys -nostartfiles
ifeq ($(RAM_BOOTING), 1)
LDFLAGS += -Wl,-Tsram.ld -Wl,--gc-sections
else
LDFLAGS += -Wl,-Tflash.ld -Wl,--gc-sections
endif
ifeq ($(DEBUG), 1)
LDFLAGS += --specs=rdimon.specs -lrdimon
endif
#LDFLAGS = $(CFLAGS)

LDFLAGS += -Wl,-wrap=printf

###################################################
# include path
CFLAGS += -I$(SOURCE_DIR)/$(APP_PATH)/inc -I$(SOURCE_DIR)/driver/CMSIS/Include
CFLAGS += -I$(SOURCE_DIR)/driver/CMSIS/Device/MTK/mt2523/Include
CFLAGS += -I$(SOURCE_DIR)/driver/chip/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/sensor_subsys/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/bluetooth/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/nvdm/inc
CFLAGS += -I$(SOURCE_DIR)/driver/board/mt25x3_hdk/ept/inc

CXXFLAGS = $(CFLAGS) -fno-rtti -fno-exceptions

###################################################

MODULE_PATH += $(DRV_CHIP_PATH)
MODULE_PATH += $(MID_NVDM_PATH)
MODULE_PATH += $(KRL_SRV_PATH)

###################################################
# Sources or Lib
# RTOS source files
include $(SOURCE_DIR)/kernel/rtos/FreeRTOS/module.mk

# mems driver files
include $(SOURCE_DIR)/driver/board/component/mems/module.mk

# sensor subsys files
#include $(SOURCE_DIR)/middleware/MTK/sensor_subsys/module.mk

# clover driver files
#include $(SOURCE_DIR)/driver/board/component/biosensor/mt2511/src/module.mk

# bluetooth files
LIBS += $(SOURCE_DIR)/middleware/MTK/bluetooth/lib/libbt_2523.a

LIBS += $(SOURCE_DIR)/$(APP_PATH)/lib/libble_ancs.a
# lzma files
#LIBS += $(SOURCE_DIR)/middleware/third_party/lzma_decoder/lib/liblzma.a

# HAL driver files
LIBS += $(OUTPATH)/libhal.a
LIBS += $(OUTPATH)/libnvdm_CM4_GCC.a

# kernel service files
LIBS += $(OUTPATH)/libservice.a

C_OBJS = $(C_FILES:%.c=$(BUILD_DIR)/%.o)
S_OBJS = $(S_FILES:%.s=$(BUILD_DIR)/%.o)
###################################################
# Rules
.PHONY: proj clean $(MODULE_PATH)

all: proj
	@mkdir -p $(BUILD_DIR)
	@$(SIZE) $(OUTPATH)/$(PROJ_NAME).elf
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
	@if [ -e "$(OUTPATH)/$(PROJ_NAME).dis" ]; then rm -f "$(OUTPATH)/$(PROJ_NAME).dis"; fi
	@if [ -e "$(OUTPATH)/$(PROJ_NAME).hex" ]; then rm -f "$(OUTPATH)/$(PROJ_NAME).hex"; fi
	@if [ -e "$(OUTPATH)/$(PROJ_NAME).bin" ]; then rm -f "$(OUTPATH)/$(PROJ_NAME).bin"; fi
	@$(CC) $(LDFLAGS) -Wl,--start-group $^ -Wl,--end-group -Wl,-Map=$(OUTPATH)/$(PROJ_NAME).map -lm -o $@ 2>>$(ERR_LOG)
	@$(OBJDUMP) -D $(OUTPATH)/$(PROJ_NAME).elf > $(OUTPATH)/$(PROJ_NAME).dis
	@$(OBJCOPY) -O ihex $(OUTPATH)/$(PROJ_NAME).elf $(OUTPATH)/$(PROJ_NAME).hex
	@$(OBJCOPY) -O binary $(OUTPATH)/$(PROJ_NAME).elf $(OUTPATH)/$(PROJ_NAME).bin
#	@cp $(GNSS_BIN) $(OUTPATH)/gnss_firmware.bin
	@echo Done


include $(SOURCE_DIR)/.rule.mk

clean:
	rm -rf $(OUTPATH)

ifneq ($(MAKECMDGOALS),clean)
-include $(C_OBJS:.o=.d)
endif
