###################################################
# Sources
###################################################

CAMERA_SRC = middleware/MTK/camera
CAMERA_FILES = $(CAMERA_SRC)/cal/src/cal_main.c \
               $(CAMERA_SRC)/cal/src/cal_task_main.c \
               $(CAMERA_SRC)/cal/src/cal_feature_ctrl.c \
               $(CAMERA_SRC)/cal/src/cal_cb.c \
               $(CAMERA_SRC)/Camera_common/src/sensor_frame_rate_lut.c \
               $(CAMERA_SRC)/cal/src/cal_scenario_ctrl.c \
               $(CAMERA_SRC)/mdp_cam_if/src/idp_cam_if.c \
               $(CAMERA_SRC)/Camera_common/src/image_sensor.c \
               $(CAMERA_SRC)/Camera_common/src/camera_sccb.c \
               $(CAMERA_SRC)/Camera_common/src/camera_hw.c

C_FILES += $(CAMERA_FILES)

###################################################
# include path
###################################################

CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/camera/interface/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/camera/Camera_common/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/camera/mdp_cam_if/inc
CFLAGS += -I$(SOURCE_DIR)/driver/board/component/camera/isp/inc
CFLAGS += -I$(SOURCE_DIR)/driver/board/component/camera/sensor/SP0820_SERIAL
CFLAGS += -I$(SOURCE_DIR)/driver/board/component/camera/sensor/BF3901_SERIAL
CFLAGS += -I$(SOURCE_DIR)/driver/board/component/camera/sensor/GC0310_SERIAL
CFLAGS += -I$(SOURCE_DIR)/driver/board/component/mdp/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/camera/cal/inc
CFLAGS += -I$(SOURCE_DIR)/driver/chip/inc
CFLAGS += -I$(SOURCE_DIR)/driver/chip/mt7687/src/common/include
CFLAGS += -I$(SOURCE_DIR)/kernel/service/inc

# definition
CFLAGS += -D__MTK_TARGET__ -DMT2523
# SENSOR's definitions
CFLAGS += -DGC0310_SERIAL_YUV_MAIN -DGC0310_SERIAL_YUV
# ISP's definitions
CFLAGS += -DISP_SUPPORT -D__ISP_SERIAL_SENSOR_SUPPORT__ -D__SERIAL_SENSOR_V1_SUPPORT__ -D__ISP_UPLL_SUPPORT__

