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
-include $(SOURCE_DIR)/config/board/$(BOARD_CONFIG)/board.mk

RAM_BOOTING=0

DEBUG      = 0
FLOAT_TYPE = hard
BUILD_DIR  = $(PWD)/Build

# Project name
PROJ_NAME = cjson
PROJ_PATH = $(PWD)
OUTPATH = $(PWD)/Build

# Main APP files
APP_NAME = cjson
APP_PATH = project/mt7687_hdk/apps/cjson
APP_PATH_SRC = $(APP_PATH)/src
APP_FILES =    $(APP_PATH_SRC)/sys_init.c \
               $(APP_PATH_SRC)/system_mt7687.c \
               $(APP_PATH)/GCC/syscalls.c

C_FILES += $(APP_FILES)
C_FILES += $(APP_PATH_SRC)/main.c
C_FILES += $(APP_PATH_SRC)/cJSON_test.c

C_FILES += driver/board/mt76x7_hdk/ept/src/bsp_gpio_ept_config.c
C_FILES += $(APP_PATH_SRC)/ept_gpio_var.c
C_FILES += $(APP_PATH_SRC)/ept_eint_var.c

# add startup file to build
S_FILES += $(APP_PATH)/GCC/startup_mt7687.s

C_OBJS = $(C_FILES:%.c=$(BUILD_DIR)/%.o)
S_OBJS = $(S_FILES:%.s=$(BUILD_DIR)/%.o)


CFLAGS += -DMTK_FW_VERSION=\"$(MTK_FW_VERSION)\"

###################################################
# include path
CFLAGS += -I$(SOURCE_DIR)/$(APP_PATH)/inc
CFLAGS += -I$(SOURCE_DIR)/driver/chip/mt7687/inc
CFLAGS += -I$(SOURCE_DIR)/driver/chip/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/lwip/src/include
CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/lwip/ports/include
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

###################################################
# Libraries
LIBS += $(OUTPATH)/libnvdm_CM4_GCC.a
LIBS += $(OUTPATH)/libcjson.a
LIBS += $(OUTPATH)/liblwip.a
LIBS += $(OUTPATH)/libservice.a
LIBS += $(OUTPATH)/librtos.a
LIBS += $(OUTPATH)/libbsp.a
LIBS += $(OUTPATH)/libhal.a

MODULE_PATH += $(KRL_OS_PATH)
MODULE_PATH += $(MID_LWIP_PATH)
MODULE_PATH += $(DRV_CHIP_PATH)
MODULE_PATH += $(KRL_SRV_PATH)
MODULE_PATH += $(DRV_BSP_PATH)
MODULE_PATH += $(MID_CJSON_PATH)
MODULE_PATH += $(MID_NVDM_PATH)
###################################################
# Rules
.PHONY: proj clean $(MODULE_PATH)

all: proj
	@mkdir -p $(BUILD_DIR)
	@$(SIZE) $(OUTPATH)/$(PROJ_NAME).elf
	@echo "Generate Assembly from elf:"
	@$(OBJDUMP) -S $(OUTPATH)/$(PROJ_NAME).elf > $(OUTPATH)/$(PROJ_NAME).elf.s
	@$(SOURCE_DIR)/tools/scripts/build/copy_firmware.sh $(SOURCE_DIR) $(OUTPATH) $(IC_CONFIG) $(BOARD_CONFIG) $(PROJ_NAME).bin $(PWD)

MOD_EXTRA = BUILD_DIR=$(BUILD_DIR) OUTPATH=$(OUTPATH) PROJ_PATH=$(PROJ_PATH) CONFIG_PATH=$(APP_PATH)/inc

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
	if [ -e $(MID_MINISUPP_PATH) ]; then \
		make -C $(MID_MINISUPP_PATH) $(MOD_EXTRA) clean \
	;fi

print-%  : ; @echo $* = $($*)
