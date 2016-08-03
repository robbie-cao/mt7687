MINISUPP_SRC = middleware/MTK/minisupp

#C_FILES += $(MINISUPP_SRC)/src/dummy.c

#################################################################################
#include path
#CFLAGS  += -I$(SOURCE_DIR)/$(MINISUPP_SRC)/inc_protected
CFLAGS  += -I$(SOURCE_DIR)/middleware/util/include
CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/minicli/include
CFLAGS  += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/include
CFLAGS  += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/include 
CFLAGS  += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/include/portable/GCC/ARM_CM4F
CFLAGS  += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source
CFLAGS  += -I$(SOURCE_DIR)/middleware/mlog/include
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/mt7687/include
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/inc
