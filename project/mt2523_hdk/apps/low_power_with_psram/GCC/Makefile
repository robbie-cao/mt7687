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
PROJ_NAME = low_power_with_psram
PROJ_PATH = $(PWD)
OUTPATH = $(PWD)/Build

#GNSS Bin Config
GNSS_BIN = $(SOURCE_DIR)/middleware/MTK/gnss/gnss_chip_fw.bin

###################################################
# Sources or Lib
# RTOS source files
include $(SOURCE_DIR)/kernel/rtos/FreeRTOS/module.mk

include graphicLib.mk
# mems driver files
include $(SOURCE_DIR)/driver/board/component/mems/module.mk

# sensor subsys files
include $(SOURCE_DIR)/middleware/MTK/sensor_subsys/module.mk
include sensor_demo.mk

# GNSS files
include $(SOURCE_DIR)/middleware/MTK/gnss/module.mk
include gnss_screen.mk

# fatfs files
#include $(SOURCE_DIR)/middleware/third_party/fatfs/module.mk


# bluetooth files
LIBS += $(SOURCE_DIR)/middleware/MTK/bluetooth/lib/libbt_2523.a
#LIBS += $(SOURCE_DIR)/middleware/MTK/bluetooth/lib/libbtif_e2.a


# HAL driver files
LIBS += $(OUTPATH)/libbsp.a
LIBS += $(OUTPATH)/libhal.a
LIBS += $(SOURCE_DIR)/driver/chip/mt2523/lib/libcryptos_CM4_GCC.a

# NVDM files
LIBS += $(OUTPATH)/libnvdm_CM4_GCC.a

# BT codec files
LIBS += $(SOURCE_DIR)/driver/board/component/bt_codec/lib/arm_cm4/libcvsd_codec.a
LIBS += $(SOURCE_DIR)/driver/board/component/bt_codec/lib/arm_cm4/libmsbc_codec.a
LIBS += $(SOURCE_DIR)/driver/board/component/bt_codec/lib/arm_cm4/libplc.a

# audio middleware files
include $(SOURCE_DIR)/middleware/MTK/audio/module.mk

# kernel service files
LIBS += $(OUTPATH)/libservice.a

# add ATCI files
include $(SOURCE_DIR)/middleware/MTK/atci/module.mk

#BT audio files
include bt_audio.mk


# Main APP files
APP_PATH = project/mt2523_hdk/apps/low_power_with_psram
APP_PATH_SRC = project/mt2523_hdk/apps/low_power_with_psram/src
APP_PATH_SRC_PXP = project/mt2523_hdk/apps/low_power_with_psram/src/pxp_screen
APP_PATH_SRC_DISP = project/mt2523_hdk/apps/low_power_with_psram/src/display_demo
APP_PATH_SRC_WF = $(APP_PATH_SRC)/watch_face

APP_FILES = $(APP_PATH_SRC)/main.c \
            $(APP_PATH)/GCC/syscalls.c \
            $(APP_PATH_SRC)/hci_log.c \
            driver/board/mt25x3_hdk/ept/src/bsp_gpio_ept_config.c \
            $(APP_PATH_SRC)/task_def.c \
            $(APP_PATH_SRC)/system_mt2523.c

APP_FILES += $(APP_PATH_SRC_PXP)/ble_ias.c \
             $(APP_PATH_SRC_PXP)/ble_lls.c \
             $(APP_PATH_SRC_PXP)/ble_tps.c \
             $(APP_PATH_SRC_PXP)/ble_gatts_srv_common.c \
             $(APP_PATH_SRC_PXP)/ble_message.c \
             $(APP_PATH_SRC_PXP)/ble_static_callback.c \
             $(APP_PATH_SRC_PXP)/ble_pxp_main.c

APP_FILES += $(APP_PATH_SRC_WF)/watch_face_app.c \
             $(APP_PATH_SRC_WF)/wf_image.c

APP_FILES += $(APP_PATH_SRC)/ept_gpio_var.c \
	     $(APP_PATH_SRC)/ept_eint_var.c

APP_FILES += $(APP_PATH_SRC)/bt_at_cmd/bt_at_cmd.c

# MVG flash test files
ifeq ($(MTK_MVG_FLASH_TEST_ENABLE), y)
MVG_SRC = project/common/test_cases/mvg/
C_FILES += $(MVG_SRC)/src/flash_mtd_sf_ut.c
endif


#APP_FILES += $(APP_PATH_SRC_DISP)/display_demo.c \
	           $(APP_PATH_SRC_DISP)/logo.c \
	           $(APP_PATH_SRC_DISP)/linkit.c

C_FILES += $(APP_FILES)

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
CFLAGS += -I$(SOURCE_DIR)/$(APP_PATH)/inc -I$(SOURCE_DIR)/driver/CMSIS/Include -I$(SOURCE_DIR)/$(APP_PATH)/inc/watch_face
CFLAGS += -I$(SOURCE_DIR)/driver/CMSIS/Device/MTK/mt2523/Include
CFLAGS += -I$(SOURCE_DIR)/project/mt2523_hdk/apps/low_power_with_psram/inc/pxp_screen
CFLAGS += -I$(SOURCE_DIR)/driver/chip/mt2523/include
CFLAGS += -I$(SOURCE_DIR)/driver/chip/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/sensor_subsys/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/bluetooth/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/nvdm/inc
CFLAGS += -I$(SOURCE_DIR)/driver/board/mt25x3_hdk/ept/inc

###################################################
# Module folder path
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
	@cp $(GNSS_BIN) $(OUTPATH)/gnss_firmware.bin
	@echo Done



include $(SOURCE_DIR)/.rule.mk

clean:
	rm -rf $(OUTPATH)

