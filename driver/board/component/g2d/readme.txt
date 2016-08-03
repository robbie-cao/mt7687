G2D module usage guide

Brief:          This module is the Graphic 2D engine. You could use the 2D engine by calling the G2D driver APIs directly.
Usage:          GCC: Include the module with "include $(SOURCE_DIR)/driver/board/component/g2d/module.mk" in your GCC project Makefile and set MTK_G2D_ENABLE to "y".
                KEIL: Drag the driver/board/component/g2d folder to your project and define the MTK_G2D_ENABLE macro. Add driver/board/component/g2d/inc to INCLUDE_PATH.
                IAR: ???
Dependency:     HAL_G2D_MODULE_ENABLED must also defined in the hal_feature_config.h  under the project inc folder. If MTK_G2D_ENABLE defined in the module.mk is "y", please also include the driver/board/component/g2d/module.mk. 
Notice:         Please use the driver/CMSIS/Device/MTK/mt2523/linkerscript/gcc/default/flash.ld for gcc build environment. 
                Please use the driver/CMSIS/Device/MTK/mt2523/linkerscript/rvct/default/flash.sct for KEIL build environment. 
                IAR???
Relative doc:   Please refer to the G2D developer guide under the doc folder for more detail.
Example project:Please find the project under project folder with g2d_ prefix. 
