LWIP module usage guide

Brief:          This module is the implementation of TCP/IP stack.
Usage:          GCC: Include the module with "include $(SOURCE_DIR)/middleware/third_party/lwip/module.mk" in your GCC project Makefile.
                KEIL:Drag the middleware/third_party/lwip folder to your project. Add middleware/third_party/lwip/src/include and middleware/third_party/lwip/ports/include to include paths.
                IAR: Drag the middleware/third_party/lwip folder to your project. Add middleware/third_party/lwip/src/include and middleware/third_party/lwip/ports/include to include paths.
Dependency:     For mt76x7, Please enable wifi module. For mt2523x, please enable modem module.
Notice:         N/A.
Relative doc:   Please refer to the Open source user guide under the doc folder for more detail.
Example project:Please find the project under project folder with lwip_ prefix.