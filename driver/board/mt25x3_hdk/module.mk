
BOARD_SRC = driver/board/mt25x3_hdk
COMPONENT_SRC = driver/board/component
BT_CODEC_SRC = driver/board/mt25x3_hdk/bt_codec

C_FILES  = $(BOARD_SRC)/lcd/mt25x3_hdk_lcd.c
C_FILES  += $(COMPONENT_SRC)/lcm/ST7789H2/lcd.c
C_FILES  += $(COMPONENT_SRC)/lcm/RM69032/lcd_RM69032.c
C_FILES  += $(BOARD_SRC)/backlight/mt25x3_hdk_backlight.c

#bt codec driver source
ifneq ($(wildcard $(strip $(SOURCE_DIR))/$(BOARD_SRC)/bt_codec/),)
  C_FILES  += $(BT_CODEC_SRC)/src/bt_a2dp_codec.c
  C_FILES  += $(BT_CODEC_SRC)/src/bt_hfp_codec.c
endif

#ctp driver source
C_FILES  += $(COMPONENT_SRC)/touch_panel/common/src/bsp_ctp.c
C_FILES  += $(COMPONENT_SRC)/touch_panel/common/src/ctp.c
C_FILES  += $(COMPONENT_SRC)/touch_panel/common/src/ctp_i2c.c
C_FILES  += $(COMPONENT_SRC)/touch_panel/ctp_goodix_gt9xxx/gt9137/ctp_goodix_gt9xxx.c
C_FILES  += $(COMPONENT_SRC)/touch_panel/ctp_ite_it7xxx/it7258/ctp_ite_it7xxx.c
C_FILES  += $(COMPONENT_SRC)/touch_panel/ctp_ite_it7xxx/it7258/ctp_ite_it7xxx_download.c
C_FILES  += $(BOARD_SRC)/touch_panel/it7258/touch_panel_custom_ite.c
C_FILES  += $(BOARD_SRC)/touch_panel/gt9137/touch_panel_custom_goodix.c

#keypad custome file
C_FILES  += $(BOARD_SRC)/keypad/src/keypad_custom.c

ifeq ($(MTK_LED_ENABLE),y)
  C_FILES  += $(BOARD_SRC)/led/src/bsp_led_internal.c
  C_FILES  += $(COMPONENT_SRC)/led/bsp_led.c
endif

#################################################################################
#include path
CFLAGS 	+= -Iinclude
CFLAGS  += -I$(SOURCE_DIR)/kernel/service/inc
CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/nvdm/inc
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/inc
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/mt2523/inc
CFLAGS  += -I$(BOARD_SRC)/lcd

#ctp driver include path
CFLAGS  += -I$(SOURCE_DIR)/driver/board/component/common
CFLAGS  += -I$(SOURCE_DIR)/driver/board/component/touch_panel/common/inc
CFLAGS  += -I$(SOURCE_DIR)/driver/board/component/touch_panel/ctp_goodix_gt9xxx/gt9137
CFLAGS  += -I$(SOURCE_DIR)/driver/board/component/touch_panel/ctp_ite_it7xxx/it7258
CFLAGS  += -I$(SOURCE_DIR)/driver/board/mt25x3_hdk/touch_panel/gt9137
CFLAGS  += -I$(SOURCE_DIR)/driver/board/mt25x3_hdk/touch_panel/it7258

#ctp driver include path
CFLAGS  += -I$(SOURCE_DIR)/driver/board/mt25x3_hdk/keypad/inc

#bt codec driver include path
CFLAGS  += -I$(SOURCE_DIR)/$(BT_CODEC_SRC)/inc
CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/bluetooth/inc
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/mt2523/inc
CFLAGS  += -I$(SOURCE_DIR)/driver/board/component/bt_codec/inc

#led driver include path
ifeq ($(MTK_LED_ENABLE),y)
  CFLAGS  += -I$(SOURCE_DIR)/driver/board/component/led
  CFLAGS  += -I$(SOURCE_DIR)/driver/board/mt25x3_hdk/led/inc
endif


