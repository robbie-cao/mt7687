
LIBSENSORSREEN   = project/mt2523_hdk/apps/low_power_with_psram/src/sensor_demo

C_FILES  += $(LIBSENSORSREEN)/sensor_demo_screen.c \
            $(LIBSENSORSREEN)/sensor_demo.c

CFLAGS  += -I$(SOURCE_DIR)/project/mt2523_hdk/apps/low_power_with_psram/inc/graphicLib
CFLAGS  += -I$(SOURCE_DIR)/project/mt2523_hdk/apps/low_power_with_psram/inc/sensor_demo