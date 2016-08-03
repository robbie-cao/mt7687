SOURCE_DIR = ../../..
BINPATH = ~/gcc-arm-none-eabi/bin
PROJ_PATH = ../../../project/mt7687_hdk/apps/iot_sdk/GCC

FEATURE    ?= feature.mk

include $(PROJ_PATH)/$(FEATURE)

# Gloabl Config
-include $(SOURCE_DIR)/.config
# IC Config
-include $(SOURCE_DIR)/config/chip/$(IC_CONFIG)/chip.mk
# Board Config
-include $(SOURCE_DIR)/config/board/$(BOARD_CONFIG)/board.mk

CFLAGS += -I$(PROJ_PATH)/../inc
CFLAGS += -I../../../$(CONFIG_PATH)
# Project name
TARGET_LIB=libbspex

BUILD_DIR = Build
OUTPATH = Build


# Sources
include module.mk
           
C_OBJS = $(C_FILES:%.c=$(BUILD_DIR)/%.o)


.PHONY: $(TARGET_LIB).a

all: $(TARGET_LIB).a
	@echo Build $< Done

$(TARGET_LIB).a: $(C_OBJS) $(S_OBJS)
	@echo Gen $(TARGET_LIB).a
	@echo Gen $(TARGET_LIB).a >>$(BUILD_LOG)
	@$(AR) -r $(OUTPATH)/$@ $(C_OBJS) $(S_OBJS) >>$(BUILD_LOG) 2>/dev/null

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo Build... $$(basename $@)
	@echo Build... $@ >>$(BUILD_LOG)
	@$(CC) $(CFLAGS) -c $< -o $@ 2>>$(ERR_LOG)

$(BUILD_DIR)/%.d: $(SOURCE_DIR)/%.c
	@mkdir -p $(dir $@)
	@set -e; rm -f $@; \
	export D_FILE="$@"; \
	export B_NAME=`echo $$D_FILE | sed 's/\.d//g'`; \
	$(CC) -MM $(CFLAGS) $< > $@.$$$$; \
	sed 's@\(.*\)\.o@'"$$B_NAME\.o $$B_NAME\.d"'@g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

clean:
	rm -rf $(OUTPATH)/$(TARGET_LIB).a
	rm -rf $(BUILD_DIR)

ifneq ($(MAKECMDGOALS),clean)
-include $(C_OBJS:.o=.d)
endif
