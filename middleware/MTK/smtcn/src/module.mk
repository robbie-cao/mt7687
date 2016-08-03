SMTCN_SRC := middleware/MTK/smtcn/src

C_FILES  += $(SMTCN_SRC)/elian.c \
            $(SMTCN_SRC)/smt_api.c \
            $(SMTCN_SRC)/smt_conn.c

#################################################################################
#include path
CFLAGS 	+= -I./inc
CFLAGS	+= -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/include 
CFLAGS  += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/portable/GCC/ARM_CM4F
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/mt7687/inc
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/inc
CFLAGS  += -I$(SOURCE_DIR)/kernel/service/inc 
CFLAGS 	+= -I$(SOURCE_DIR)/driver/board/mt76x7_hdk/wifi/inc
CFLAGS 	+= -I$(SOURCE_DIR)/driver/board/mt76x7_hdk/inc
