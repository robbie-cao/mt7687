
###################################################

SENSOR_SUBSYS_SRC = middleware/MTK/sensor_subsys
SENSOR_SUBSYS_FILES = $(SENSOR_SUBSYS_SRC)/src/sensor_manager_driver.c \
                      $(SENSOR_SUBSYS_SRC)/src/sensor_manager.c

# Sensor fusion algorithm adaptor files
SENSOR_FUSION_ALGO_ADAPTOR_FILES = $(SENSOR_SUBSYS_SRC)/src/algo_adaptor/algo_adaptor.c                      

ifeq ($(FUSION_PEDOMETER_USE),M_INHOUSE_PEDOMETER)
SENSOR_FUSION_ALGO_ADAPTOR_FILES += $(SENSOR_SUBSYS_SRC)/src/algo_adaptor/pedometer_adaptor.c
endif

ifeq ($(FUSION_SLEEP_TRACKER_USE),M_INHOUSE_SLEEP_TRACKER)
SENSOR_FUSION_ALGO_ADAPTOR_FILES += $(SENSOR_SUBSYS_SRC)/src/algo_adaptor/sleep_adaptor.c
endif

ifeq ($(FUSION_HEART_RATE_MONITOR_USE),M_INHOUSE_HEART_RATE_MONITOR)
SENSOR_FUSION_ALGO_ADAPTOR_FILES += $(SENSOR_SUBSYS_SRC)/src/algo_adaptor/heart_rate_monitor_adaptor.c
endif

ifeq ($(FUSION_HEART_RATE_VARIABILITY_USE),M_INHOUSE_HEART_RATE_VARIABILITY)
SENSOR_FUSION_ALGO_ADAPTOR_FILES += $(SENSOR_SUBSYS_SRC)/src/algo_adaptor/heart_rate_variability_monitor_adaptor.c
endif

ifeq ($(FUSION_BLOOD_PRESSURE_USE),M_INHOUSE_BLOOD_PRESSURE)
SENSOR_FUSION_ALGO_ADAPTOR_FILES += $(SENSOR_SUBSYS_SRC)/src/algo_adaptor/blood_pressure_monitor_adaptor.c
endif

C_FILES += $(SENSOR_SUBSYS_FILES) $(SENSOR_FUSION_ALGO_ADAPTOR_FILES)

###################################################
# include path
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/sensor_subsys/inc

# fusion algorithm libraries
ifeq ($(FUSION_PEDOMETER_USE),M_INHOUSE_PEDOMETER)
  ifneq ($(wildcard $(SOURCE_DIR)/middleware/MTK/sensor_subsys/fusion_algo/pedometer/lib_core/libpedometer_CM4_GCC.a),)
    LIBS += $(SOURCE_DIR)/middleware/MTK/sensor_subsys/fusion_algo/pedometer/lib_core/libpedometer_CM4_GCC.a
  endif
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/sensor_subsys/fusion_algo/pedometer/inc
endif

ifeq ($(FUSION_SLEEP_TRACKER_USE),M_INHOUSE_SLEEP_TRACKER)
  ifneq ($(wildcard $(SOURCE_DIR)/middleware/MTK/sensor_subsys/fusion_algo/sleep_tracker/lib_core/libsleeptracker_CM4_GCC.a),)
    LIBS += $(SOURCE_DIR)/middleware/MTK/sensor_subsys/fusion_algo/sleep_tracker/lib_core/libsleeptracker_CM4_GCC.a
  endif
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/sensor_subsys/fusion_algo/sleep_tracker/inc
endif

ifeq ($(FUSION_HEART_RATE_MONITOR_USE),M_INHOUSE_HEART_RATE_MONITOR)
  ifneq ($(wildcard $(SOURCE_DIR)/middleware/MTK/sensor_subsys/fusion_algo/heart_rate/lib_core/libhr_CM4_GCC.a),)
    LIBS += $(SOURCE_DIR)/middleware/MTK/sensor_subsys/fusion_algo/heart_rate/lib_core/libhr_CM4_GCC.a
  endif
  ifneq ($(wildcard $(SOURCE_DIR)/middleware/MTK/sensor_subsys/fusion_algo/ppg_control/lib_core/libppgctrl_CM4_GCC.a),)
    LIBS += $(SOURCE_DIR)/middleware/MTK/sensor_subsys/fusion_algo/ppg_control/lib_core/libppgctrl_CM4_GCC.a
  endif
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/sensor_subsys/fusion_algo/heart_rate/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/sensor_subsys/fusion_algo/ppg_control/inc
endif

ifeq ($(FUSION_HEART_RATE_VARIABILITY_USE),M_INHOUSE_HEART_RATE_VARIABILITY)
  ifneq ($(wildcard $(SOURCE_DIR)/middleware/MTK/sensor_subsys/fusion_algo/heart_rate_variability/lib_core/libhrv_CM4_GCC.a),)
    LIBS += $(SOURCE_DIR)/middleware/MTK/sensor_subsys/fusion_algo/heart_rate_variability/lib_core/libhrv_CM4_GCC.a
  endif
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/sensor_subsys/fusion_algo/heart_rate_variability/inc
endif

ifeq ($(FUSION_BLOOD_PRESSURE_USE),M_INHOUSE_BLOOD_PRESSURE)
  ifneq ($(wildcard $(SOURCE_DIR)/middleware/MTK/sensor_subsys/fusion_algo/blood_pressure/lib_core/libbp_CM4_GCC.a),)
    #LIBS += $(SOURCE_DIR)/middleware/MTK/sensor_subsys/fusion_algo/blood_pressure/lib_core/libbp_CM4_GCC.a
  endif
#CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/sensor_subsys/fusion_algo/blood_pressure/inc
endif

# Sensor Fusion Task Support
ifeq ($(MTK_SENSOR_SUBSYS_SINGLE_TASK),y)
# Only sensor manager task
else
CFLAGS += -DSENSOR_FUSTION_TASK_ENABLED
endif
