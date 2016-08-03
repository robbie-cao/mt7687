smtcn module usage guide

Brief:          This module is the MediaTek smart connection implementation.
Usage:          GCC: Include the module with "include $(SOURCE_DIR)/middleware/MTK/smtcn/module.mk" in your GCC project Makefile and set MTK_SMTCN_ENABLE to "y".
                KEIL: 1. Drag the middleware/MTK/smtcn folder to your project and define the MTK_SMTCN_ENABLE macro.
                      2. Add middleware/MTK/smtcn/inc to INCLUDE_PATH.
                IAR:  1.Drag the middleware/MTK/smtcn folder to your project and define the MTK_SMTCN_ENABLE macro.
                      2. Add middleware/MTK/smtcn/inc to INCLUDE_PATH.
Dependency:     None 
Notice:         None
Relative doc:   Please refer to the WiFi API reference under the doc folder for more detail.
Example project:Please refer to any project which calls mtk_smart_connect(). 
