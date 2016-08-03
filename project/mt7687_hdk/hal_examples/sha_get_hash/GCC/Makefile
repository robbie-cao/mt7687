SOURCE_DIR = ../../../../..
BINPATH    = ~/gcc-arm-none-eabi/bin

PWD        = $(shell pwd)

FEATURE    = feature.mk

include $(FEATURE)

# Gloabl Config
-include $(SOURCE_DIR)/.config
# IC Config
-include $(SOURCE_DIR)/config/chip/$(IC_CONFIG)/chip.mk
# Board Config
#-include $(SOURCE_DIR)/config/board/$(BOARD_CONFIG)/board.mk

RAM_BOOTING=0

DEBUG      = 0
FLOAT_TYPE = hard
BUILD_DIR  = $(PWD)/Build

# Project name
PROJ_NAME = sha_get_hash
PROJ_PATH = $(PWD)
OUTPATH = $(PWD)/Build

# Main APP files
APP_NAME = sha_get_hash
APP_PATH = project/mt7687_hdk/hal_examples


C_FILES += $(APP_PATH)/$(APP_NAME)/src/main.c
C_FILES += $(APP_PATH)/$(APP_NAME)/src/system_mt7687.c
C_FILES += $(APP_PATH)/$(APP_NAME)/GCC/syscalls.c

# add startup file to build
S_FILES += $(APP_PATH)/$(APP_NAME)/GCC/startup_mt7687.s

C_OBJS = $(C_FILES:%.c=$(BUILD_DIR)/%.o)
S_OBJS = $(S_FILES:%.s=$(BUILD_DIR)/%.o)


CFLAGS += -DMTK_FW_VERSION=\"$(MTK_FW_VERSION)\"

###################################################
# include path
CFLAGS += -I$(SOURCE_DIR)/$(APP_PATH)/$(APP_NAME)/inc
CFLAGS += -I$(SOURCE_DIR)/driver/chip/mt7687/inc
CFLAGS += -I$(SOURCE_DIR)/driver/chip/mt7687/include
CFLAGS += -I$(SOURCE_DIR)/driver/chip/inc
CFLAGS += -I$(APP_PATH)/$(APP_NAME)/inc/

###################################################
# LD Flags
LDFLAGS = $(ALLFLAGS) $(FPUFLAGS) --specs=nano.specs -lnosys -nostartfiles
ifeq ($(RAM_BOOTING), 1)
LDFLAGS += -Wl,-T./mt7687_sram.ld -Wl,--gc-sections
else
LDFLAGS += -Wl,-T./mt7687_flash.ld -Wl,--gc-sections
endif

###################################################
# Libraries
LIBS += $(OUTPATH)/libhal.a

MODULE_PATH += $(DRV_CHIP_PATH)

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
	@if [ -e "$(OUTPATH)/$(PROJ_NAME).hex" ]; then rm -f "$(OUTPATH)/$(PROJ_NAME).hex"; fi
	@if [ -e "$(OUTPATH)/$(PROJ_NAME).bin" ]; then rm -f "$(OUTPATH)/$(PROJ_NAME).bin"; fi
	@$(CC) $(LDFLAGS) -Wl,--start-group $^ -Wl,--end-group -Wl,-Map=$(OUTPATH)/$(PROJ_NAME).map -lm -o $@ 2>>$(ERR_LOG)
	@$(OBJCOPY) -O ihex $(OUTPATH)/$(PROJ_NAME).elf $(OUTPATH)/$(PROJ_NAME).hex
	@$(OBJCOPY) -O binary $(OUTPATH)/$(PROJ_NAME).elf $(OUTPATH)/$(PROJ_NAME).bin
	@echo Done

include $(SOURCE_DIR)/.rule.mk

clean:
	rm -rf $(OUTPATH)

print-%  : ; @echo $* = $($*)
