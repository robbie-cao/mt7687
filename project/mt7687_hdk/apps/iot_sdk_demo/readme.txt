/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

/**
  * @page MT7687 IoT SDK Example Project
  * @{

@par Overview

- Example project description
  - This project demonstrates the connectivity features of the MT7687.
- Application features
  - Act as a Wi-Fi station to connect to a Wi-Fi network.
  - Act as a Wi-Fi AP to accept connection to the LinkIt 7687 HDK from a
    handheld device or a laptop computer.
  - Ping out/in the device.
  - Wi-Fi Command Line Interface (CLI), please refer to "cli_readme.txt" for more details.
  
@par Hardware and software environment

  - Supported platform
    - LinkIt 7687 HDK.

  - HDK switches and pin configuration
    - J36 provides the pins of GPIOs, PWMs, SPI master chip select 0, SPI
      master, UART1 RX/TX.
    - J35 provides the pins of GPIOs, PWMs, UART2 RX/TX, UART1 RTS/CTS, SPI
      master chip select 1, IR TX, and IR RX.
    - J34 provides the pins of GPIOs, PWMs, UART2 RTS/CTS, I2S, SPI slave and
      I2C0.
    - J33 provides the pins of GPIOs, PWMs, I2C0, ADC0~3.
    - J32 provides the pins of GND, 5V, 3.3V and reset pin.
    - J25 controls the pins of the Flash Normal Mode vs Flash Recovery mode of HDK. 
      Place a jumper over it,during the next boot-up, the HDK will boot under 
      Flash Recovery Mode(update firmware), otherwise, it is booted into Flash 
      Normal Mode (Run Application).
    - There are three buttons on the board:
      - RST - reset.
      - EINT - external interrupt trigger.
      - RTC_INT - RTC interrupt trigger.

  - PC/environment configuration
   - A serial tool is required, such as hyper terminal or TeraTerm for UART
     logging.
   - COM port settings. baudrate: 115200, data bits: 8, stop bit: 1, parity:
     none and flow control: off.

@par Directory contents
  - Source and header files
   - iot_sdk_demo/src/main.c                 Entry point of the application program.
   - iot_sdk_demo/src/sys_init.c             Aggregated initialization routines.
   - iot_sdk_demo/src/system_mt7687.c        MT7687 system clock configuration file.
   - iot_sdk_demo/src/app_common.c           Project user entry.
   - iot_sdk_demo/src/cli_cmds.c             CLI table.
   - iot_sdk_demo/src/lwip_network.c         lwIP configuration.
   - iot_sdk_demo/src/cli_def.c              CLI definition.

   - iot_sdk_demo/inc/flash_map.h            MT7687 memory layout symbol file.
   - iot_sdk_demo/inc/hal_feature_config.h   MT7687 HAL API feature configuration
                                             file.
   - iot_sdk_demo/inc/FreeRTOSConfig.h       MT7687 FreeRTOS configuration file.
   - iot_sdk_demo/inc/lwipopts.h             lwIP configuration.
   - iot_sdk_demo/inc/sys_init.h             Prototype declaration for iot_sdk_demo/src/sys_init.c.
   - iot_sdk_demo/inc/app_cli_table.h        CLI table entry.
   - iot_sdk_demo/inc/cli_cmds.h             Prototype declaration for iot_sdk_demo/src/cli_cmds.c.
   - iot_sdk_demo/inc/lwip_network.h         Prototype declaration for iot_sdk_demo/src/lwip_network.c.
   - iot_sdk_demo/inc/app_common.h           Prototype declaration for iot_sdk_demo/src/app_common.c.

  - Project configuration files using GCC.
   - iot_sdk_demo/GCC/Makefile               GNU Makefile for this project.
   - iot_sdk_demo/GCC/feature.mk             Generic feature options configuration
                                             file.
   - iot_sdk_demo/GCC/mt7687_flash.ld        Linker script.
   - iot_sdk_demo/GCC/startup_mt7687.s       MT7687 startup file.
   - iot_sdk_demo/GCC/syscalls.c             MT7687 syscalls implementation.

  - Project configuration files using Keil.
   - iot_sdk_demo/MDK-ARM/RTE/Device/7687_32M_MXIC.FLM
   - iot_sdk_demo/MDK-ARM/RTE/Device/flash.ini
   - iot_sdk_demo/MDK-ARM/RTE/RTE_Components.h
   - iot_sdk_demo/MDK-ARM/flash.sct               Scatter file.
   - iot_sdk_demo/MDK-ARM/iot_sdk_demo.uvoptx     uVision5 project options. Contains the
                                                  settings for the debugger, trace
                                                  configuration, breakpoints, currently
                                                  open files, etc.
   - iot_sdk_demo/MDK-ARM/iot_sdk_demo.uvprojx    uVision5 Project File. Contains the
                                                  project structure in XML format.
   - iot_sdk_demo/MDK-ARM/startup_mt7687.s        MT7687 startup file.

@par Run the examples
  - Below are two examples to demonstrate the Wi-Fi station and Wi-Fi access
    point modes of MT7687.
  - Example 1. Wi-Fi station mode.
    - Find your Wi-Fi access point settings:
      Before connecting to a Wi-Fi access point, the following information
      needs to be collected:
      1. What is the SSID of your Wi-Fi access point?
      2. What is the password of your Wi-Fi access point?
    - Once you have this information, use the following settings to initialize
      the HDK. This example code in main.c assumes the access point SSID is
      'myhome' (length 6) and the password is '12345678'
      (length 8).

        // User-defined settings.
        wifi_config_t config = {0};
        config.opmode = WIFI_MODE_STA_ONLY;
        strcpy((char *)config.sta_config.ssid, "myhome");
        config.sta_config.ssid_length = strlen("myhome");
        strcpy((char *)config.sta_config.password, "12345678");
        config.sta_config.password_length = strlen("12345678");

        // Wi-Fi initialization.
        wifi_init(&config, NULL);

  - Connect the board to PC with a serial port cable.
  - Build the example project and download the binary file to the HDK.
  - Reboot the HDK, the console will show "FreeRTOS Running" message to
    indicate the HDK is booting up.

      If everything is correct, similar messages will be shown in the console
      to notify your HDK has received an IP address.

        ************************
        DHCP got IP:10.10.10.101
        ************************

    - PING from the LinkIt 7687 HDK.
      If the IP address is fetched and the network is operating, the LinkIt
      7687 HDK can ping other devices/computer on the network with the following
      command in the console.

        ping 10.10.10.254 3 64

      The ping stops after sending three packets to 10.10.10.254.
      The ping usage is: ping <ip address> <ping count> <ping packet length>

  - Example 2. Wi-Fi access point mode.
    - Apply Wi-Fi access point settings before proceeding":
      - SSID
      - Authentication Mode
      - Encryption Type
      - Password
      - Channel

    - Once the information is collected, use the following statements to
      configure the LinkIt 7687 HDK. The example code in main.c assumes WPA2 PSK is used for
      authentication, AES for encryption, 'iot_ap' (length 6) for the SSID,
      the password of WPA2 as '87654321' (length 8) and the channel 6 is assigned to the channel.

        // User-defined settings.
        wifi_config_t config = {0};
        wifi_config.opmode = WIFI_MODE_AP_ONLY;
        strcpy((char *)wifi_config.ap_config.ssid, "iot_ap");
        wifi_config.ap_config.ssid_length = strlen("iot_ap");
        wifi_config.ap_config.auth_mode = WIFI_AUTH_MODE_WPA2_PSK;
        wifi_config.ap_config.encrypt_type = WIFI_ENCRYPT_TYPE_AES_ENABLED;
        strcpy((char *)wifi_config.ap_config.password, "87654321");
        wifi_config.ap_config.password_length = strlen("87654321");
        wifi_config.ap_config.channel = 6;

        // Wi-Fi initialization.
        wifi_init(&config, NULL);

  - Connect the board to PC with a serial port cable.
  - Build the example project and download the binary file to the HDK.
  - Reboot the HDK, the console will show "FreeRTOS Running" message to
    indicate the HDK is booting up.
    - Use a handheld device or a laptop computer to connect to the access point
      'iot_ap'. On the HDK console, the IP address assigned to the handheld device
      or laptop is shown below.

        [DHCPD:DBG]lease_ip:10.10.10.2

* @}
*/
