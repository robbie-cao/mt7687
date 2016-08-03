
LIBGRAPHIC   = project/mt2523_hdk/apps/iot_sdk_demo/src/graphicLib

C_FILES  += $(LIBGRAPHIC)/FontRes.c    \
            $(LIBGRAPHIC)/gdi_font.c    \
            $(LIBGRAPHIC)/gdi_layer.c    \
            $(LIBGRAPHIC)/gdi_primitive.c    \
            $(LIBGRAPHIC)/gd_primitive16.c    \
            $(LIBGRAPHIC)/main_screen.c    \
            $(LIBGRAPHIC)/PixcomFontEngine.c    \
            $(LIBGRAPHIC)/ui_demo_task.c    \
            $(LIBGRAPHIC)/gdi_lcd.c    



CFLAGS  += -I$(SOURCE_DIR)/project/mt2523_hdk/apps/iot_sdk_demo/inc/graphicLib
CFLAGS  += -I$(SOURCE_DIR)/project/mt2523_hdk/apps/iot_sdk_demo/inc/gnss_screen
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/inc
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/mt2523/inc
CFLAGS  += -I$(SOURCE_DIR)/driver/board/component/lcm/ST7789H2
CFLAGS  += -I$(SOURCE_DIR)/driver/board/component/common
