MINICLI module usage guide

Brief:          This module is a Command Line Interface engine implementation.
Usage:          GCC: Include the module with "include $(SOURCE_DIR)/middleware/MTK/minicli/module.mk" in your GCC project Makefile and set MTK_MINICLI_ENABLE to "y".
                KEIL: Drag the middleware/MTK/minicli folder to your project and define the MTK_MINICLI_ENABLE macro. Add middleware/MTK/minicli/inc to INCLUDE_PATH.
Dependency:     HAL_UART_MODULE_ENABLED must also defined in hal_feature_config.h under project inc folder.
Notice:         N/A
Relative doc:   N/A
Example project:Please find the project under project folder with iot_sdk prefix. 

