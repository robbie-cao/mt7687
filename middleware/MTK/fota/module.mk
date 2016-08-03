
FOTA_SRC = middleware/MTK/fota

C_FILES  += $(FOTA_SRC)/src/fota.c 

ifeq ($(IC_CONFIG), $(filter $(IC_CONFIG),mt7687 mt7697))
C_FILES  += $(FOTA_SRC)/src/76x7/fota_76x7.c
C_FILES  += $(FOTA_SRC)/src/76x7/fota_config.c
C_FILES  += $(FOTA_SRC)/src/76x7/fota_port.c

ifeq ($(MTK_FOTA_ENABLE),y)
C_FILES  += $(FOTA_SRC)/src/76x7/fota_cli.c
C_FILES  += $(FOTA_SRC)/src/76x7/fota_download_interface.c
endif

endif
#################################################################################
# include path

CFLAGS 	+= -I$(SOURCE_DIR)/middleware/MTK/fota/inc
CFLAGS 	+= -I$(SOURCE_DIR)/middleware/MTK/fota/inc/76x7
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/inc
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/mt7687/inc
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/mt2523/inc

CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/tftp/include

ifeq ($(MTK_BL_LOAD_ENABLE),y)
CFLAGS  += -I$(SOURCE_DIR)/driver/board/mt76x7_hdk/bootloader/src/mt7687
CFLAGS  += -DBL_FOTA_DEBUG
endif