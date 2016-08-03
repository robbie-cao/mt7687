# BT sink source files
BT_SINK_SRC = project/mt2523_hdk/apps/iot_sdk_demo/src/bt_audio/sink
BT_SINK_FILES = $(BT_SINK_SRC)/bt_sink_utils.c \
                $(BT_SINK_SRC)/bt_sink_hf_multipoint.c \
                $(BT_SINK_SRC)/bt_sink_hf_call_manager.c \
                $(BT_SINK_SRC)/bt_sink_hf.c \
                $(BT_SINK_SRC)/bt_sink_event.c \
                $(BT_SINK_SRC)/bt_sink_db.c \
                $(BT_SINK_SRC)/bt_sink_custom_db.c \
                $(BT_SINK_SRC)/bt_sink_conmgr.c \
                $(BT_SINK_SRC)/bt_sink_music.c \
                $(BT_SINK_SRC)/bt_sink_avrcp.c \
                $(BT_SINK_SRC)/bt_sink_a2dp.c \
                $(BT_SINK_SRC)/bt_sink_task.c \
                $(BT_SINK_SRC)/bt_sink_queue.c \
                $(BT_SINK_SRC)/bt_sink_common_cb.c \
                $(BT_SINK_SRC)/bt_sink_am_task.c \
                $(BT_SINK_SRC)/bt_sink_ami.c
C_FILES += $(BT_SINK_FILES)

# Include bt sink path
CFLAGS += -I$(SOURCE_DIR)/project/mt2523_hdk/apps/iot_sdk_demo/inc/bt_audio/sink
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/bluetooth/inc
CFLAGS += -I$(SOURCE_DIR)/driver/chip/inc
CFLAGS += -I$(SOURCE_DIR)/driver/board/mt25x3_hdk/bt_codec/inc
CFLAGS += -mno-unaligned-access
LIBS += $(OUTPATH)/libhal.a
