BLE WIFI smart connection module usage guide 

Brief:          This module is able to receive the SSID, password and security mode from the APK by BLE connection, and 
                then connect to the WIFI AP.
Usage:          Define the MTK_BLE_SMTCN_ENABLE compile option in project/mt7697_hdk/apps/iot_sdk to enable this 
                feature. It's same for Keil/IAR.
Dependency:     None. 
Notice:         The GATT service of this feature is registered in project/mt7697_hdk/apps/iot_sdk/src/ut_app/gatt_service.c. 
                Get the APK in SDK packet in tools/ble_smart_connection/ble_smart_connection.apk and install it on Android Smartphone
Relative doc:   None.
Example project:project/mt7697_hdk/apps/iot_sdk. 
