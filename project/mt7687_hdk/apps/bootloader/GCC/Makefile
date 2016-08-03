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
PROJ_NAME = mt7687_bootloader
PROJ_PATH = $(PWD)
OUTPATH = $(PWD)/Build

Q ?= @

include $(SOURCE_DIR)/driver/board/mt76x7_hdk/bootloader/module.mk


# Main APP files
APP_PATH = project/mt7687_hdk/apps/bootloader
APP_PATH_SRC = $(APP_PATH)/src



C_FILES += $(APP_PATH_SRC)/system_mt7687.c
C_FILES += driver/board/mt76x7_hdk/ept/src/bsp_gpio_ept_config.c
C_FILES += $(APP_PATH_SRC)/ept_gpio_var.c
C_FILES += $(APP_PATH_SRC)/ept_eint_var.c
C_FILES += $(APP_PATH)/GCC/syscalls.c

C_OBJS = $(C_FILES:%.c=$(BUILD_DIR)/%.o)
S_OBJS = $(S_FILES:%.s=$(BUILD_DIR)/%.o)


CFLAGS += -DMTK_FW_VERSION=\"$(MTK_FW_VERSION)\"

ifeq ($(MTK_FOTA_ENABLE),y)
CFLAGS += -DMOD_CFG_FOTA_DISABLE_CLI
CFLAGS += -DMOD_CFG_FOTA_DISABLE_OS
DISABLE_CLI_MACRO = MOD_CFG_FOTA_DISABLE_CLI
DISABLE_OS_MACRO = MOD_CFG_FOTA_DISABLE_OS
else
CFLAGS += -DMOD_CFG_LOADER_DISABLE_FOTA
endif

ifeq ($(MTK_BL_LOAD_ENABLE),y)
CFLAGS  += -DBL_FOTA_DEBUG
endif

ifeq ($(MTK_BL_DEBUG_ENABLE),y)
CFLAGS += -DBL_DEBUG
endif

ifeq ($(MTK_BL_BOOT_MENU_ENABLE),y)
CFLAGS += -DMTK_BOOTLOADER_MENU_ENABLE
endif

ifeq ($(MTK_BL_RESTORE_DEFAULT_ENABLE),y)
CFLAGS += -DMTK_BOOTLOADER_RESTORE_DEFAULT_ENABLE
endif

###################################################
# include path
CFLAGS += -I$(SOURCE_DIR)/$(APP_PATH)/inc
ifeq ($(MTK_BL_LOAD_ENABLE),y)
CFLAGS  += -I$(SOURCE_DIR)/driver/board/mt76x7_hdk/bootloader/src/mt7687
endif
CFLAGS += -I$(SOURCE_DIR)/driver/board/mt76x7_hdk/ept/inc
###################################################
# LD Flags
LDFLAGS  = $(ALLFLAGS) $(FPUFLAGS) --specs=nano.specs -lnosys -nostartfiles
LDFLAGS_FLASH = $(LDFLAGS) -Wl,-Tbootloader.ld -Wl,--gc-sections
LDFLAGS_RAM   = $(LDFLAGS) -Wl,-Tbootloader-ram.ld -Wl,--gc-sections

###################################################
# Libraries

ifeq ($(MTK_FOTA_ENABLE),y)
LIBS += $(OUTPATH)/libfota.a
LIBS += $(OUTPATH)/liblzma.a
endif
ifeq ($(MTK_MPERF_ENABLE),y)
LIBS += $(OUTPATH)/libmperf.a
endif

LIBS += $(OUTPATH)/libhal.a


MODULE_PATH += $(DRV_CHIP_PATH)
ifeq ($(MTK_FOTA_ENABLE),y)
MODULE_PATH += $(MID_FOTA_PATH)
MODULE_PATH += $(MID_LZMA_PATH)
endif
ifeq ($(MTK_MPERF_ENABLE),y)
MODULE_PATH += $(MID_MPERF_PATH)
endif
###################################################
# Rules
.PHONY: clean proj $(MODULE_PATH)

all: proj
	@mkdir -p $(BUILD_DIR)
	@$(SIZE) $(OUTPATH)/$(PROJ_NAME).elf
	@$(SOURCE_DIR)/tools/scripts/build/copy_firmware.sh $(SOURCE_DIR) $(OUTPATH) $(IC_CONFIG) $(BOARD_CONFIG) $(PROJ_NAME).bin $(PWD)


MOD_EXTRA = BUILD_DIR=$(BUILD_DIR) OUTPATH=$(OUTPATH) PROJ_PATH=$(PROJ_PATH) DISABLE_CLI_MACRO=$(DISABLE_CLI_MACRO) DISABLE_OS_MACRO=$(DISABLE_OS_MACRO)

$(LIBS): $(MODULE_PATH)

$(MODULE_PATH):
	@+make -C $@ $(MOD_EXTRA) $($@_EXTRA)


proj: $(OUTPATH)/$(PROJ_NAME).elf
#proj: $(OUTPATH)/$(PROJ_NAME).elf $(OUTPATH)/$(PROJ_NAME)-ram.elf

$(OUTPATH)/$(PROJ_NAME)-ram.elf: $(C_OBJS) $(CXX_OBJS) $(S_OBJS) $(LIBS)
	@echo Linking...
	@if [ -e "$@" ]; then rm -f "$@"; fi
	@if [ -e "$(OUTPATH)/$(PROJ_NAME)-ram.map" ]; then rm -f "$(OUTPATH)/$(PROJ_NAME)-ram.map"; fi
	@if [ -e "$(OUTPATH)/$(PROJ_NAME)-ram.hex" ]; then rm -f "$(OUTPATH)/$(PROJ_NAME)-ram.hex"; fi
	@if [ -e "$(OUTPATH)/$(PROJ_NAME)-ram.bin" ]; then rm -f "$(OUTPATH)/$(PROJ_NAME)-ram.bin"; fi
	$(Q)$(CC) $(LDFLAGS_RAM) -Wl,--start-group $^ -Wl,--end-group -Wl,-Map=$(OUTPATH)/$(PROJ_NAME)-ram.map -lm -o $@ 2>>$(ERR_LOG)
	$(Q)$(OBJCOPY) -O ihex $(OUTPATH)/$(PROJ_NAME)-ram.elf $(OUTPATH)/$(PROJ_NAME)-ram.hex
	$(Q)$(OBJCOPY) -O binary $(OUTPATH)/$(PROJ_NAME)-ram.elf $(OUTPATH)/$(PROJ_NAME)-ram.bin
	@echo Done

$(OUTPATH)/$(PROJ_NAME).elf: $(C_OBJS) $(CXX_OBJS) $(S_OBJS) $(LIBS)
	@echo Linking...
	@if [ -e "$@" ]; then rm -f "$@"; fi
	@if [ -e "$(OUTPATH)/$(PROJ_NAME).map" ]; then rm -f "$(OUTPATH)/$(PROJ_NAME).map"; fi
	@if [ -e "$(OUTPATH)/$(PROJ_NAME).hex" ]; then rm -f "$(OUTPATH)/$(PROJ_NAME).hex"; fi
	@if [ -e "$(OUTPATH)/$(PROJ_NAME).bin" ]; then rm -f "$(OUTPATH)/$(PROJ_NAME).bin"; fi
	$(Q)$(CC) $(LDFLAGS_FLASH) -Wl,--start-group $^ -Wl,--end-group -Wl,-Map=$(OUTPATH)/$(PROJ_NAME).map -lm -o $@ 2>>$(ERR_LOG)
	$(Q)$(OBJCOPY) -O ihex $(OUTPATH)/$(PROJ_NAME).elf $(OUTPATH)/$(PROJ_NAME).hex
	$(Q)$(OBJCOPY) -O binary $(OUTPATH)/$(PROJ_NAME).elf $(OUTPATH)/$(PROJ_NAME).bin
	@echo Done

include $(SOURCE_DIR)/.rule.mk

clean:
	rm -rf $(OUTPATH)

print-%  : ; @echo $* = $($*)
