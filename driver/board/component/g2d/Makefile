SOURCE_DIR = ../../../..
BINPATH = ~/gcc-arm-none-eabi/bin
PROJ_PATH = ../../../../project/mt2523_hdk/apps/iot_sdk_demo/GCC
CONFIG_PATH ?= .

CFLAGS += -I$driver/board/component/g2d/inc
CFLAGS += -I$(PROJ_PATH)/../inc
CFLAGS += -I$(SOURCE_DIR)/$(CONFIG_PATH)


FEATURE    ?= feature.mk

include $(PROJ_PATH)/$(FEATURE)

# Gloabl Config
-include $(SOURCE_DIR)/.config
# IC Config
-include $(SOURCE_DIR)/config/chip/$(IC_CONFIG)/chip.mk
# Board Config
-include $(SOURCE_DIR)/config/board/$(BOARD_CONFIG)/board.mk

# Project name
TARGET_LIB=libg2d

BUILD_DIR = Build
OUTPATH = Build


# Sources
 include module.mk
 -include src_core/module.mk

C_OBJS = $(C_FILES:%.c=$(BUILD_DIR)/%.o)


.PHONY: $(TARGET_LIB).a

all: $(TARGET_LIB).a 
	if [ -e  ./Build/$(TARGET_LIB).a ]; then \
	cp ./Build/$(TARGET_LIB).a ./lib/.; \
	fi

include $(SOURCE_DIR)/.rule.mk

clean:
	rm -rf $(BUILD_DIR)



