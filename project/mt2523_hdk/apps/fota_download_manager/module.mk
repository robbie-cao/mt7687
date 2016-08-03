# source files

FOTA_APP_PATH = project/mt2523_hdk/apps/fota_download_manager
BT_NOTIFY_SRC = $(FOTA_APP_PATH)/src/btnotify_src
GNSS_DL_SRC = $(FOTA_APP_PATH)/src/gnss_src
C_FILES += $(FOTA_APP_PATH)/src/fota_main.c
C_FILES += $(FOTA_APP_PATH)/src/fota_dummy.c
C_FILES += $(BT_NOTIFY_SRC)/ble_dogp_service.c
C_FILES += $(BT_NOTIFY_SRC)/ble_dogp_adp_service.c
C_FILES += $(BT_NOTIFY_SRC)/ble_gatts_srv_common.c
C_FILES += $(BT_NOTIFY_SRC)/bt_notify_conversion.c
C_FILES += $(BT_NOTIFY_SRC)/bt_notify_data_parse.c
C_FILES += $(BT_NOTIFY_SRC)/ble_bds_app_util.c
C_FILES += $(BT_NOTIFY_SRC)/xml_main.c
C_FILES += $(GNSS_DL_SRC)/brom_base.c
C_FILES += $(GNSS_DL_SRC)/brom_mt3301.c
C_FILES += $(GNSS_DL_SRC)/da_cmd.c
C_FILES += $(GNSS_DL_SRC)/flashtool.c
C_FILES += $(GNSS_DL_SRC)/gnss_uart.c
C_FILES += $(GNSS_DL_SRC)/bl_debug.c
C_FILES += $(GNSS_DL_SRC)/bl_dbguart.c
#C_FILES += $(GNSS_DL_SRC)/epo_download.c
#C_FILES ++ $(GNSS_DL_SRC)/gnss_app.c

LIBS += $(SOURCE_DIR)/$(FOTA_APP_PATH)/lib/libbtnotify.a

# include files

CFLAGS += -I$(SOURCE_DIR)/$(FOTA_APP_PATH)/inc/fota_inc
CFLAGS += -I$(SOURCE_DIR)/$(FOTA_APP_PATH)/inc/btnotify_inc
CFLAGS += -I$(SOURCE_DIR)/$(FOTA_APP_PATH)/inc/gnss_inc
CFLAGS += -I$(SOURCE_DIR)/driver/chip/mt2523/inc
CFLAGS += -I$(SOURCE_DIR)/driver/chip/mt7687/inc
CFLAGS += -I$(SOURCE_DIR)/driver/chip/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/nvdm/inc

