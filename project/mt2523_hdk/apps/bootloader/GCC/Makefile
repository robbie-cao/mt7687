SOURCE_DIR = ../../../../..

BINPATH=~/gcc-arm-none-eabi/bin

PWD= $(shell pwd)

FEATURE ?= feature.mk
include $(FEATURE)
# Gloabl Config
-include $(SOURCE_DIR)/.config
# IC Config
-include $(SOURCE_DIR)/config/chip/$(IC_CONFIG)/chip.mk
# Board Config
-include $(SOURCE_DIR)/config/board/$(BOARD_CONFIG)/board.mk

export WITH_BOOTLOADER=n
#CC=$(BINPATH)/arm-none-eabi-gcc
#CXX=$(BINPATH)/arm-none-eabi-g++
#OBJCOPY=$(BINPATH)/arm-none-eabi-objcopy
#SIZE=$(BINPATH)/arm-none-eabi-size

#SOURCE_DIR = ../../../../..
BUILD_DIR = $(PWD)/Build

# Project name
PROJ_NAME=mt2523_bootloader
OUTPATH = $(PWD)/Build
PROJ_PATH = $(PWD)

#PLATFORM
PLATFORM = mt25x3

include $(SOURCE_DIR)/driver/board/mt25x3_hdk/bootloader/module.mk

# lzma files
LIBS += $(OUTPATH)/liblzma.a

C_FILES = $(BOOTLOADER_FILES) $(DRIVER_FILES)
S_FILES += $(APP_PATH)/GCC/startup_bootloader.s
C_FILES += $(APP_PATH)/GCC/syscalls.c

C_OBJS = $(C_FILES:%.c=$(BUILD_DIR)/%.o)
S_OBJS = $(S_FILES:%.s=$(BUILD_DIR)/%.o)

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
ALLFLAGS = -g -Os
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
CFLAGS += $(FPUFLAGS)

ifeq ($(MTK_BL_FPGA_LOAD_ENABLE),y)
CFLAGS += -D__FPGA_TARGET__
endif

ifeq ($(MTK_BL_DEBUG_ENABLE),y)
CFLAGS += -DBL_DEBUG
endif

ifeq ($(MTK_BL_FLASH_DIST_ENABLE),y)
CFLAGS += -D__SERIAL_FLASH_DISTURBANCE_CHECK_EN__
endif

ifeq ($(MTK_BL_FLASH_STT_ENABLE),y)
CFLAGS += -D__SERIAL_FLASH_STT_EN__
CFLAGS += -D__ADV_DBG_PRINT__
# add uart driver files
STT_DRIVER_PATH = project/mt2523_hdk/test_loads/HQA/hqa_flash_test
STT_DRIVER_PATH_SRC = $(STT_DRIVER_PATH)/src/mvg
C_FILES +=	$(STT_DRIVER_PATH_SRC)/bl_STT_SF.c
CFLAGS += -I$(SOURCE_DIR)/$(STT_DRIVER_PATH)/inc/mvg
endif

ifeq ($(MTK_FOTA_ENABLE),y)
CFLAGS += -DBL_FOTA_ENABLE
endif

ifeq ($(MTK_NO_PSRAM_ENABLE),y)
CFLAGS += -DMTK_NO_PSRAM_ENABLE
endif


ifeq ($(MTK_BL_LOAD_ENABLE),y)
CFLAGS += -DBL_LOAD_ENABLE
endif

CFLAGS += -D__UBL__ -D__EXT_BOOTLOADER__

# LD Flags
LDFLAGS = $(ALLFLAGS) $(FPUFLAGS) --specs=nano.specs -lnosys -nostartfiles
LDFLAGS += -Wl,-Tbootloader.ld -Wl,--gc-sections
ifeq ($(DEBUG), 1)
LDFLAGS += --specs=rdimon.specs -lrdimon
endif
#LDFLAGS = $(CFLAGS)

###################################################

MODULE_PATH += $(MID_LZMA_PATH)

###################################################

.PHONY: proj clean $(MODULE_PATH)

all: proj
	@mkdir -p $(BUILD_DIR)
	@echo ""
	@echo "-----------  bin size  ---------------------"
	@$(SIZE) $(OUTPATH)/$(PROJ_NAME).elf
	@$(SOURCE_DIR)/tools/scripts/build/copy_firmware.sh $(SOURCE_DIR) $(OUTPATH) $(IC_CONFIG) $(BOARD_CONFIG) $(PROJ_NAME).bin $(PWD)


MOD_EXTRA = BUILD_DIR=$(BUILD_DIR) OUTPATH=$(OUTPATH) PROJ_PATH=$(PROJ_PATH)

$(LIBS): $(MODULE_PATH)

$(MODULE_PATH):
	@+make -C $@ $(MOD_EXTRA) $($@_EXTRA)

proj: $(OUTPATH)/$(PROJ_NAME).elf

$(OUTPATH)/$(PROJ_NAME).elf: $(C_OBJS) $(CXX_OBJS) $(S_OBJS) $(LIBS)
	@echo Linking...
	@if [ -e "$@" ]; then rm -f "$@"; fi
	@if [ -e "$(OUTPATH)/$(PROJ_NAME).map" ]; then rm -f "$(OUTPATH)/$(PROJ_NAME).map"; fi
	@if [ -e "$(OUTPATH)/$(PROJ_NAME).hex" ]; then rm -f "$(OUTPATH)/$(PROJ_NAME).hex"; fi
	@if [ -e "$(OUTPATH)/$(PROJ_NAME).bin" ]; then rm -f "$(OUTPATH)/$(PROJ_NAME).bin"; fi
	@$(CC) $(LDFLAGS) -Wl,--start-group $^ -Wl,--end-group -Wl,-Map=$(OUTPATH)/$(PROJ_NAME).map -o $@ 2>>$(ERR_LOG)
	@$(OBJCOPY) -O ihex $(OUTPATH)/$(PROJ_NAME).elf $(OUTPATH)/$(PROJ_NAME).hex
	@$(OBJCOPY) -O binary $(OUTPATH)/$(PROJ_NAME).elf $(OUTPATH)/$(PROJ_NAME).bin
	@echo Done


include $(SOURCE_DIR)/.rule.mk

clean:
	rm -rf $(OUTPATH)

ifneq ($(MAKECMDGOALS),clean)
-include $(C_OBJS:.o=.d)
-include $(CXX_OBJS:.o=.d)
endif
