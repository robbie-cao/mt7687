
FATFS_SRC = middleware/third_party/fatfs

C_FILES  += $(FATFS_SRC)/src/ff.c
C_FILES  += $(FATFS_SRC)/src/diskio.c
C_FILES  += $(FATFS_SRC)/src/option/unicode.c
C_FILES  += $(FATFS_SRC)/src/option/syscall.c



#################################################################################
# include path

CFLAGS 	+= -I$(SOURCE_DIR)/middleware/util/include
CFLAGS 	+= -I$(SOURCE_DIR)/middleware/MTK/minicli/inc
CFLAGS  += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/include
CFLAGS  += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/portable/GCC/ARM_CM4F
CFLAGS  += -I$(SOURCE_DIR)/kernel/service/inc
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/inc
CFLAGS  += -I$(SOURCE_DIR)/middleware/third_party/fatfs/src

