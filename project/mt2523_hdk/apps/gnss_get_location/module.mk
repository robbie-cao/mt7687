# source files

GNSS_APP_PATH = project/mt2523_hdk/apps/gnss_get_location
BT_NOTIFY_SRC = $(GNSS_APP_PATH)/src/btnotify_src
C_FILES += $(BT_NOTIFY_SRC)/ble_dogp_service.c
C_FILES += $(BT_NOTIFY_SRC)/ble_dogp_adp_service.c
C_FILES += $(BT_NOTIFY_SRC)/ble_gatts_srv_common.c
C_FILES += $(BT_NOTIFY_SRC)/bt_notify_conversion.c
C_FILES += $(BT_NOTIFY_SRC)/bt_notify_data_parse.c
C_FILES += $(BT_NOTIFY_SRC)/xml_main.c
C_FILES += $(BT_NOTIFY_SRC)/ble_bds_app_util.c
C_FILES += $(GNSS_APP_PATH)/src/epo_demo.c
C_FILES += $(GNSS_APP_PATH)/src/epo_download.c
C_FILES += $(GNSS_APP_PATH)/src/gnss_app.c
C_FILES += $(GNSS_APP_PATH)/src/gnss_bridge_task.c
C_FILES += $(GNSS_APP_PATH)/src/gnss_demo.c
C_FILES += $(GNSS_APP_PATH)/src/gnss_ring_buffer.c
C_FILES += $(GNSS_APP_PATH)/src/gnss_timer.c
C_FILES += $(GNSS_APP_PATH)/src/gnss_uart_bridge.c
C_FILES += $(GNSS_APP_PATH)/src/hci_log.c
LIBS += $(SOURCE_DIR)/$(GNSS_APP_PATH)/lib/libbtnotify.a

# include files

CFLAGS += -I$(SOURCE_DIR)/$(GNSS_APP_PATH)/inc
CFLAGS += -I$(SOURCE_DIR)/$(GNSS_APP_PATH)/inc/btnotify_inc
CFLAGS += -I$(SOURCE_DIR)/driver/chip/mt2523/inc
CFLAGS += -I$(SOURCE_DIR)/driver/chip/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/nvdm/inc

