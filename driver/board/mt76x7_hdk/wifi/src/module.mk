
BOARD_SRC = driver/board/mt76x7_hdk/wifi

C_FILES  += $(BOARD_SRC)/src/get_profile_string.c
C_FILES  += $(BOARD_SRC)/src/misc.c
C_FILES  += $(BOARD_SRC)/src/os_util.c
C_FILES  += $(BOARD_SRC)/src/os_utils.c
C_FILES  += $(BOARD_SRC)/src/wifi_profile.c
C_FILES  += $(BOARD_SRC)/src/wifi_wps.c
C_FILES  += $(BOARD_SRC)/src/wifi_init.c
C_FILES  += $(BOARD_SRC)/src/wifi_default_config.c
ifeq ($(MTK_WIFI_CONFIGURE_FREE_ENABLE),y)
C_FILES  += $(BOARD_SRC)/src/sie.c
endif

ifeq ($(MTK_MINICLI_ENABLE),y)
C_FILES  += $(BOARD_SRC)/src/wifi_cli.c
C_FILES  += $(BOARD_SRC)/src/wifi_inband_cli.c
ifeq ($(MTK_WIFI_CONFIGURE_FREE_ENABLE),y)
C_FILES  += $(BOARD_SRC)/src/sie_cli.c
endif
endif

#################################################################################
#include path
CFLAGS 	+= -I../inc
CFLAGS  += -I$(SOURCE_DIR)/kernel/service/inc
CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/nvdm/inc

