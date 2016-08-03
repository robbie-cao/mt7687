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
PROJ_NAME = atci_register_command
PROJ_PATH = $(PWD)
OUTPATH = $(PWD)/Build

#GNSS Bin Config

###################################################
# Sources or Lib
# RTOS source files
include $(SOURCE_DIR)/kernel/rtos/FreeRTOS/module.mk


ifndef FLAG_STORE_FOLDER
FLAG_STORE_FOLDER = $(OUTPATH)
endif

# mems driver files
include $(SOURCE_DIR)/driver/board/component/mems/module.mk

# clover driver files
#include $(SOURCE_DIR)/driver/board/component/clover/src/module.mk

# fatfs files
#include $(SOURCE_DIR)/middleware/third_party/fatfs/module.mk



# HAL driver files
LIBS += $(OUTPATH)/libbsp.a
LIBS += $(OUTPATH)/libhal.a
LIBS += $(SOURCE_DIR)/driver/chip/mt2523/lib/libcryptos_CM4_GCC.a

# NVDM files
LIBS += $(OUTPATH)/libnvdm_CM4_GCC.a

include $(SOURCE_DIR)/middleware/MTK/audio/module.mk


# kernel service files
LIBS += $(OUTPATH)/libservice.a

# add ATCI files
include $(SOURCE_DIR)/middleware/MTK/atci/module.mk

# Main APP files
APP_PATH = project/mt2523_hdk/apps/atci_register_command
APP_PATH_SRC = project/mt2523_hdk/apps/atci_register_command/src

APP_FILES = $(APP_PATH_SRC)/main.c \
            $(APP_PATH_SRC)/sys_init.c \
            $(APP_PATH_SRC)/atci_cmd.c \
            $(APP_PATH)/GCC/syscalls.c

APP_FILES += $(APP_PATH_SRC)/ept_gpio_var.c \
	     	 $(APP_PATH_SRC)/ept_eint_var.c

APP_FILES += $(APP_PATH_SRC)/system_mt2523.c

C_FILES += $(APP_FILES)
C_FILES += driver/board/mt25x3_hdk/ept/src/bsp_gpio_ept_config.c


# add startup file to build
S_FILES += $(APP_PATH)/GCC/startup_mt2523.s

C_OBJS = $(C_FILES:%.c=$(BUILD_DIR)/%.o)
S_OBJS = $(S_FILES:%.s=$(BUILD_DIR)/%.o)

# LD Flags
LDFLAGS = $(ALLFLAGS) $(FPUFLAGS) --specs=nano.specs -lnosys -nostartfiles
ifeq ($(RAM_BOOTING), 1)
CFLAGS+= -DRAM_BOOTING 
LDFLAGS += -Wl,-Tram.ld -Wl,--gc-sections
else
LDFLAGS += -Wl,-Tflash.ld -Wl,--gc-sections
endif
ifeq ($(DEBUG_IO), 1)
LDFLAGS += --specs=rdimon.specs -lrdimon
CFLAGS += -DDEBUG_IO
endif
#LDFLAGS = $(CFLAGS)

###################################################
# include path
CFLAGS += -I$(SOURCE_DIR)/$(APP_PATH)/inc -I$(SOURCE_DIR)/driver/CMSIS/Include
CFLAGS += -I$(SOURCE_DIR)/driver/CMSIS/Device/MTK/mt2523/Include
CFLAGS += -I$(SOURCE_DIR)/driver/chip/mt2523/inc
CFLAGS += -I$(SOURCE_DIR)/driver/chip/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/nvdm/inc
CFLAGS += -I$(SOURCE_DIR)/driver/board/mt25x3_hdk/ept/inc

###################################################

MODULE_PATH += $(DRV_CHIP_PATH)
MODULE_PATH += $(KRL_SRV_PATH)
MODULE_PATH += $(DRV_BSP_PATH)
MODULE_PATH += $(MID_NVDM_PATH)

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
	@echo Done


include $(SOURCE_DIR)/.rule.mk

clean:
	rm -rf $(OUTPATH)


