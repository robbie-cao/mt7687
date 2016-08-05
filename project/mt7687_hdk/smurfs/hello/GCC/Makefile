##############################################################################
#
# Project settings
#
##############################################################################


SOURCE_DIR = ../../../../../

ifneq ($(findstring CYGWIN, ${UNAME}), )
	SOURCE_DIR  := ${ROOT_DIR}
endif

BINPATH    = ~/gcc-arm-none-eabi/bin
PWD        = $(shell pwd)
DATIME      = $(shell date --iso=seconds)
V          ?= 0

##############################################################################
#
# Build log output
#
##############################################################################


ifeq ($(V),0)
	Q           = @
else
	Q           =
endif


##############################################################################
#
# Config files
#
##############################################################################


FEATURE    = feature.mk

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
# Config
#
##############################################################################


RAM_BOOTING=0

DEBUG = 0
ifeq ($(DEBUG), 1)
DEBUG_IO = 1
endif

FLOAT_TYPE = hard
BUILD_DIR  = $(PWD)/Build

# Project name
PROJ_NAME = hello
PROJ_PATH = $(PWD)
OUTPATH = $(PWD)/Build

FULL_TEST = 0


# Main APP files
APP_PATH        = project/$(IC_CONFIG)_hdk/smurfs/hello
APP_PATH_SRC 	= $(APP_PATH)/src


##############################################################################
#
# Source files
#
##############################################################################

APP_FILES  = $(APP_PATH_SRC)/main.c
APP_FILES += $(APP_PATH_SRC)/ept_gpio_var.c
APP_FILES += $(APP_PATH_SRC)/ept_eint_var.c
APP_FILES += $(APP_PATH_SRC)/system_mt7687.c
APP_FILES += $(APP_PATH_SRC)/sys_init.c
APP_FILES += $(APP_PATH)/GCC/syscalls.c
APP_FILES += driver/board/mt76x7_hdk/init/src/io_def.c

ifeq ($(MTK_MINICLI_ENABLE),y)
	APP_FILES       += $(APP_PATH_SRC)/cli_def.c
	APP_FILES       += $(APP_PATH_SRC)/cli_cmds.c
endif

C_FILES += $(APP_FILES)
C_FILES += driver/board/mt76x7_hdk/ept/src/bsp_gpio_ept_config.c



# Add startup file to build
S_FILES += $(APP_PATH)/GCC/startup_mt7687.s

C_OBJS = $(C_FILES:%.c=$(BUILD_DIR)/%.o)
S_OBJS = $(S_FILES:%.s=$(BUILD_DIR)/%.o)

CFLAGS += -DMTK_FW_VERSION=\"$(MTK_FW_VERSION)\"
# Include path
CFLAGS += -I$(SOURCE_DIR)/$(APP_PATH)/inc
CFLAGS += -I$(SOURCE_DIR)/driver/chip/$(IC_CONFIG)/inc
CFLAGS += -I$(SOURCE_DIR)/driver/chip/inc
CFLAGS += -I$(SOURCE_DIR)/$(APP_PATH)/inc
CFLAGS += -I$(SOURCE_DIR)/driver/CMSIS/Include
CFLAGS += -I$(SOURCE_DIR)/driver/CMSIS/Device/MTK/mt7687/Include
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

ifeq ($(DEBUG_IO), 1)
LDFLAGS += --specs=rdimon.specs -lrdimon
CFLAGS += -DDEBUG_IO
endif

###################################################
# Libraries
LIBS += $(OUTPATH)/libminicli.a
LIBS += $(OUTPATH)/librtos.a
LIBS += $(OUTPATH)/libservice.a
LIBS += $(OUTPATH)/libhal.a

MODULE_PATH += $(KRL_OS_PATH)
MODULE_PATH += $(MID_MINICLI_PATH)
MODULE_PATH += $(KRL_SRV_PATH)
MODULE_PATH += $(DRV_CHIP_PATH)

###################################################
# Target Rules
.PHONY: proj clean $(MODULE_PATH)

all: proj
	@mkdir -p $(BUILD_DIR)
	@$(SIZE) $(OUTPATH)/$(PROJ_NAME).elf
	@echo "Generate Assembly from elf:"
	@$(OBJDUMP) -S $(OUTPATH)/$(PROJ_NAME).elf > $(OUTPATH)/$(PROJ_NAME).elf.s
	@$(SOURCE_DIR)/tools/scripts/build/copy_firmware.sh $(SOURCE_DIR) $(OUTPATH) $(IC_CONFIG) $(BOARD_CONFIG) $(PROJ_NAME).bin $(PWD)
	@cp *.cmm $(OUTPATH)/

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

.PHONY: options

options:
	@for opt in `sed 's/[()]/\n/g' $(SOURCE_DIR)/config/chip/$(IC_CONFIG)/chip.mk|sed 's/-D/ /'|sort|uniq|grep ^MTK_|grep _ENABLE$$`; do echo $$opt; done
print-%  : ; @echo $* = $($*)
