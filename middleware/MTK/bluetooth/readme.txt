Bluetooth module usage guide

Brief:          This module is the bluetooth stack implementation. It supports EDR/BLE stack on MT2523x and BLE only stack on MT7697x.
Usage:          GCC:  For EDR/BLE stack, include the module with "add $(SOURCE_DIR)/middleware/MTK/bluetooth/lib/libbt_2523.a" and "include $(SOURCE_DIR)/middleware/MTK/bluetooth/inc/" in your GCC project Makefile.
                      For BLE only stack, include the module with "add $(SOURCE_DIR)/middleware/MTK/bluetooth/lib/hb/libble_hb.a, $(SOURCE_DIR)/middleware/MTK/bluetooth/lib/hb/libbtdriver_7697.a", "add all source files under $(SOURCE_DIR)/middleware/MTK/bluetooth/src/hb" and "include $(SOURCE_DIR)/middleware/MTK/bluetooth/inc/hb/" in your GCC project Makefile.
                KEIL: For EDR/BLE stack, add the library middleware/MTK/bluetooth/lib/libbt_2523_CM4_keil.a to your project and add middleware/MTK/bluetooth/inc to the "include paths" of the C/C++ options.
                      For BLE only stack, add the library middleware/MTK/bluetooth/lib/libble_hb_CM4_keil.a, the library middleware/MTK/bluetooth/lib/libbtdriver_7697_CM4_keil.a and all source files under middleware/MTK/bluetooth/src/hb to your project and add middleware/MTK/bluetooth/inc/hb to the "include paths" of the C/C++ options.
                IAR:  For EDR/BLE stack, add the library middleware/MTK/bluetooth/lib/libbt_2523_iar.a to your project and add middleware/MTK/bluetooth/inc to the "Additional include directories" of the preprocessor.
                      For BLE only stack, add the library middleware/MTK/bluetooth/lib/libble_hb_iar.a, the library middleware/MTK/bluetooth/lib/libbtdriver_7697_iar.a and all source files under middleware/MTK/bluetooth/src/hb to your project and add middleware/MTK/bluetooth/inc/hb to the "Additional include directories" of the preprocessor.
Dependency:     Please define HAL_DVFS_MODULE_ENABLED in the hal_feature_config.h under the project inc folder. 
Notice:         None
Relative doc:   For EDR/BLE stack, please refer to the bluetooth developer's guide on MT2523 under the doc folder for more detail.
                For BLE only stack, please refer to the bluetooth developer's guide on mt7697x under the doc folder for more detail.
Example project:For EDR/BLE stack, please find the project under project folder with bt_ or ble_ prefix.
                For BLE only stack, please find the iot_sdk project under the project folder.
