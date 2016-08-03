
MT7687_CHIP = driver/chip/mt7687
CFLAGS   += $(FPUFLAGS) -DUSE_HAL_DRIVER -DPRODUCT_VERSION=7687
			 
C_FILES  += $(MT7687_CHIP)/src_core/hal_efuse.c            
C_FILES  += $(MT7687_CHIP)/src_core/hal_sleep_manager.c
C_FILES  += $(MT7687_CHIP)/src_core/hal_sleep_driver.c
#################################################################################
#include path
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/inc
CFLAGS 	+= -I$(SOURCE_DIR)/driver/board/mt76x7_hdk/inc
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/mt7687/src/common/include
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/mt7687/inc
CFLAGS 	+= -I$(SOURCE_DIR)/kernel/service/inc
