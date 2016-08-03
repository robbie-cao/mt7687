minisupp module usage guide

Brief:          This module is the Wi-Fi supplicant implementation.
Usage:          GCC: Include the module with "include $(SOURCE_DIR)/middleware/MTK/minisupp/module.mk" in your GCC project Makefile and  MTK_MINISUPP_ENABLE is forced to "y".
                     To support Repeater, set MTK_WIFI_REPEATER_ENABLE to "y"
                     To support WPS, set MTK_WIFI_WPS_ENABLE to "y"
                     To support Wi-Fi Direct, set MTK_WIFI_DIRECT_ENABLE to "y"(not ready yet)

                KEIL: Drag the middleware/MTK/minisupp/lib/libminisupp_wps_CM4_Keil.lib file to your project and define the MTK_MINISUPP_ENABLE macro. Add middleware/MTK/minisupp/inc to INCLUDE_PATH.
                     Repeater is forced on
                     For WPS feature , There are two methods to  enable/disable it
                     method-1: enable/disable WPS in the "project" window of KEIL
                     1) Enable WPS:  drag the middleware/MTK/minisupp/lib/libminisupp_wps_CM4_Keil.lib instead of libminisupp_CM4_Keil.lib to your project window in KEIL IDE and add the MTK_WIFI_WPS_ENABLE macro in "C/C++" tab in the KEIL dialog "options for target".
                     2) Disable WPS: drag the middleware/MTK/minisupp/lib/libminisupp_CM4_Keil.lib instead of libminisupp_wps_CM4_Keil.lib to your project window in KEIL IDE and delete the MTK_WIFI_WPS_ENABLE macro in "C/C++" tab in the KEIL dialog "options for target".

                     method-2: enable/disable WPS in KEIL pack installer
                     1) Enable WPS: open the pdsc file, replace the middleware/MTK/minisupp/lib/libminisupp_CM4_Keil.lib with libminisupp_wps_CM4_Keil.lib and add the MTK_WIFI_WPS_ENABLE macro defination in MiniSupp RTE_Components_h
                     2) Disable WPS: open the pdsc file, replace the middleware/MTK/minisupp/lib/libminisupp_wps_CM4_Keil.lib with libminisupp_CM4_Keil.lib and delete the MTK_WIFI_WPS_ENABLE macro defination in MiniSupp RTE_Components_h
                     Wi-Fi Direct is not supported

                IAR: Drag the middleware/MTK/minisupp/lib/libminisupp_wps_CM4_IAR.lib file to your project and define the MTK_MINISUPP_ENABLE macro. Add middleware/MTK/minisupp/inc to INCLUDE_PATH.
                     Repeater is forced on
                     WPS is forced on, if you want to switch WPS off, drag the middleware/MTK/minisupp/lib/libminisupp_CM4_IAR.lib instead of libminisupp_wps_CM4_IAR.lib to your project and delete the MTK_WIFI_WPS_ENABLE macro.
                     Wi-Fi Direct is not supported
Dependency:     libbsp, liblwip, libhal, libnvdm, librtos, libservice, libdhcpd
Notice:         None
Relative doc:   Please refer to the website "http://w1.fi" for more detail.
Example project:Please find the project under project folder with iot_sdk prefix



<< How to enable Wi-Fi feature >>
Step1: include the relative librarys in your project.
           libminisupp, libbsp, liblwip,libdhcpd
           libhal, libnvdm, librtos, libservice

Step2: Enable the compile option
           MTK_MINISUPP_ENABLE
           MTK_WIFI_REPEATER_ENABLE

Step3: If need the Wi-Fi advanced feature,
       the following compile option can be selected and enabled.
           MTK_WIFI_WPS_ENABLE
           MTK_WIFI_DIRECT_ENABLE (not ready yet)
       if enable MTK_WIFI_WPS_ENABLE compile option, should include library
       "libminisupp_wps" to instead of "libminisupp"

Step4: If need WiFi CLI command feature,
       the following compile option can be selected and be enabled.
          MTK_BSPEXT_ENABLE
          MTK_MINICLI_ENABLE
       and the following librarys should be included.
          libbspex, libminicli

Step5: If need Mideatek WiFi Smart Connection feature,
       the following compile option can be selected and be enabled.
          MTK_SMTCN_ENABLE
       and the following librarys should be included.
          libsmtcn

Step6: include the module path which belongs to the above librarys.
