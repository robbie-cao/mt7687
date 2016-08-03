###################################################
# Sources
###################################################

G2D_HAL_SRC = driver/chip/mt2523/src
G2D_SRC = driver/board/component/g2d/src
G2D_FILES = $(G2D_HAL_SRC)/hal_g2d_common_api.c \
	$(G2D_HAL_SRC)/hal_g2d_drv_common.c \
	$(G2D_HAL_SRC)/hal_g2d_rectfill_api.c \
	$(G2D_HAL_SRC)/hal_g2d_drv_rectfill.c \
	$(G2D_HAL_SRC)/hal_g2d_overlay_api.c \
	$(G2D_HAL_SRC)/hal_g2d_drv_overlay.c \
	$(G2D_SRC)/c_model/bitblt.c \
	$(G2D_SRC)/c_model/govl_func.c \
	$(G2D_SRC)/c_model/share_func.c \
	$(G2D_SRC)/c_model/g2d_drv_c_model.c \
	$(G2D_SRC)/g2d_sqc.c	
#	$(G2D_SRC)/rgb565_240x320.c \
#	$(G2D_SRC)/g2d_overlay_test.c \
#	$(G2D_SRC)/g2d_test.c 

C_FILES += $(G2D_FILES)

###################################################
# include path
###################################################

CFLAGS += -I$(SOURCE_DIR)/$(G2D_SRC)/../inc
CFLAGS += -I$(SOURCE_DIR)/$(G2D_SRC)/../inc/c_model
CFLAGS += -I$(SOURCE_DIR)/chip/mt2523/inc
CFLAGS += -I$(SOURCE_DIR)/driver/chip/inc
CFLAGS += -I$(SOURCE_DIR)/driver/chip/mt7687/src/common/include

# definition
CFLAGS += -D__MTK_TARGET__ -DHAL_G2D_MODULE_ENABLED
# G2D_FPGA have be enable when hw test
CFLAGS += -DG2D_FPGA
# Dream's G2D Test
CFLAGS += -DG2D_SUPPORT_MUTEX -DG2D_FreeRTOS

