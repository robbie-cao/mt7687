NVDM module usage guide

Brief:          This module is the non-volatile data management implementation.
Usage:          GCC: Please add "LIBS += $(OUTPATH)/libnvdm_CM4_GCC.a", "MODULE_PATH += $(MID_NVDM_PATH)" and "CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/nvdm/inc" in your GCC project Makefile.
                KEIL: Please drag the middleware/MTK/nvdm/lib/libnvdm_xxx_KEIL.lib and middleware/MTK/nvdm/src folder to your project. Please define the PRODUCT_VERSION macro in KEIL, and add middleware/MTK/nvdm/inc to include paths.
                IAR: Please drag the middleware/MTK/nvdm/lib/libnvdm_xxx_IAR.a and middleware/MTK/nvdm/src folder to your project. Please define the PRODUCT_VERSION macro in IAR, and add middleware/MTK/nvdm/inc to include paths.
Dependency:     Please define HAL_FLASH_MODULE_ENABLED in hal_feature_config.h under the project inc folder.
Notice:         To customize the nvdm setting, please configure nvdm_port.c in middleware/MTK/nvdm/src folder.
Relative doc:   Please refer to the section of HAL/nvdm in LinkIt SDK for xxxx API Reference Manual.html under the doc folder for more detail.
Example project:none.
