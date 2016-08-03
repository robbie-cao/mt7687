SOURCE_DIR = ../../../../..
BINPATH = ~/gcc-arm-none-eabi/bin

-include $(SOURCE_DIR)/.config

CC = $(BINPATH)/arm-none-eabi-gcc
AR = $(BINPATH)/arm-none-eabi-ar

# Project name
TARGET_LIB=libcamsensor

DEBUG=1
FLOAT_TYPE=hard

BUILD_DIR = Build
OUTPATH=Build

SENSOR_NAME = GC0310_SERIAL

include $(SOURCE_DIR)/kernel/rtos/FreeRTOS/module.mk

# CamSensor files
SOURCE_SRC = driver/board/component/camera/sensor/$(SENSOR_NAME)
SOURCE_FILES = $(SOURCE_SRC)/image_sensor_$(SENSOR_NAME).c

C_FILES = $(SOURCE_FILES)
C_OBJS = $(C_FILES:%.c=$(BUILD_DIR)/%.o)

###################################################

# Check for valid float argument
# NOTE that you have to run make clan after
# changing these as hardfloat and softfloat are not
# binary compatible
ifneq ($(FLOAT_TYPE), hard)
ifneq ($(FLOAT_TYPE), soft)
override FLOAT_TYPE = hard
#override FLOAT_TYPE = soft
endif
endif

###################################################
# CC Flags
ifeq ($(DEBUG), 1)
ALLFLAGS = -g -O0
else
ALLFLAGS = -Os
endif
ALLFLAGS += -Wall -mlittle-endian -mthumb -mcpu=cortex-m4
CFLAGS += $(ALLFLAGS) -flto -ffunction-sections -fdata-sections -fno-builtin

ifeq ($(FLOAT_TYPE), hard)
FPUFLAGS = -fsingle-precision-constant -Wdouble-promotion
FPUFLAGS += -mfpu=fpv4-sp-d16 -mfloat-abi=hard
#CFLAGS += -mfpu=fpv4-sp-d16 -mfloat-abi=softfp
else
FPUFLAGS = -fsingle-precision-constant -Wdouble-promotion
FPUFLAGS += -msoft-float
endif

# Definitions
CFLAGS += -D_REENT_SMALL
CFLAGS += -DARM_MATH_CM4
CFLAGS += -D__FPU_PRESENT
CFLAGS += $(FPUFLAGS)

###################################################
# include path
CFLAGS += -I$(SOURCE_DIR)/$(SOURCE_SRC)
CFLAGS += -I$(SOURCE_DIR)/driver/board/component/camera/isp/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/camera/interface/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/camera/Camera_common/inc
CFLAGS += -I$(SOURCE_DIR)/driver/chip/inc
CFLAGS += -I$(SOURCE_DIR)/driver/chip/mt2523/inc
CFLAGS += -I$(SOURCE_DIR)/driver/CMSIS/Device/MTK/mt2523/Include
CFLAGS += -I$(SOURCE_DIR)/driver/CMSIS/Include
CFLAGS += -I$(SOURCE_DIR)/project/mt2523_hdk/apps/iot_sdk_dev/inc

CFLAGS += -D$(SENSOR_NAME)_YUV -DDRV_ISP_MT6250_HW_SUPPORT -DDRV_ISP_6235_SERIES

.PHONY: $(TARGET_LIB).a

all: $(TARGET_LIB).a
	mv ./Build/$(TARGET_LIB).a $(SOURCE_DIR)/middleware/MTK/camera/lib/$(TARGET_LIB).a

include $(SOURCE_DIR)/.rule.mk

clean:
	rm -rf $(OUTPATH) $(TARGET_LIB).a $(SOURCE_DIR)/middleware/MTK/camera/lib/$(TARGET_LIB).a

