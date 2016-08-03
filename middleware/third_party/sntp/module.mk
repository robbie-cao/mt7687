
SNTP_SRC = middleware/third_party/sntp

C_FILES  += 	$(SNTP_SRC)/src/sntp.c 
			 

#################################################################################
#include path
CFLAGS 	+= -I./include
CFLAGS	+= -I$(SOURCE_DIR)/middleware/third_party/lwip/src/include
CFLAGS  += -I$(SOURCE_DIR)/middleware/third_party/lwip/src/include/lwip
CFLAGS  += -I$(SOURCE_DIR)/middleware/third_party/lwip/ports/include
CFLAGS  += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/include 
CFLAGS  += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/portable/GCC/ARM_CM4F
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/mt7687/inc
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/inc
