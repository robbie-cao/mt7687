LIBBTAUDIO = project/mt2523_hdk/apps/low_power_with_psram/src/bt_audio

C_FILES += $(LIBBTAUDIO)/bt_audio.c
C_FILES += $(LIBBTAUDIO)/bt_common_dispatch.c

CFLAGS += -I$(SOURCE_DIR)/project/mt2523_hdk/apps/low_power_with_psram/inc/bt_audio
CFLAGS += -I$(SOURCE_DIR)/project/mt2523_hdk/apps/low_power_with_psram/inc/graphicLib

# BT Sink
include $(SOURCE_DIR)/project/mt2523_hdk/apps/low_power_with_psram/src/bt_audio/sink/module.mk

