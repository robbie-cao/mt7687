
BSPEXT_SRC = project/common/bsp_ex

C_FILES  += $(BSPEXT_SRC)/src/wifi_ex_cli.c     \
            $(BSPEXT_SRC)/src/wifi_ex_config.c  \
            $(BSPEXT_SRC)/src/wifi_ex_connect.c \
            $(BSPEXT_SRC)/src/wifi_ex_profile.c

C_FILES  += $(BSPEXT_SRC)/src/lp_ex_sleep.c

#################################################################################
#include path
CFLAGS 	+= -I./inc
CFLAGS	+= -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/include 
CFLAGS  += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/portable/GCC/ARM_CM4F
CFLAGS  += -I$(SOURCE_DIR)/driver/board/mt76x7_hdk/wifi/inc
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/mt7687/inc

