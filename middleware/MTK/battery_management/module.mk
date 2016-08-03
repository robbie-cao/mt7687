
###################################################

BMT_SRC = middleware/MTK/battery_management
BMT_FILES = $(BMT_SRC)/src/battery_management_driver.c  \
			$(BMT_SRC)/src/battery_management_meter.c   \
           	$(BMT_SRC)/src/battery_management_core.c


C_FILES += $(BMT_FILES) 

###################################################
# include path
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/battery_management/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/battery_management/lib/fuelgauge/inc

