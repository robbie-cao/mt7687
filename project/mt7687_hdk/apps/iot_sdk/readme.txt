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
  * @page MT7687 IOT SDK example project
  * @{

@par Overview

- Example project description
  - This project demonstrates the connectivity features of the MT7687.
- Features of the application
  - Act as a Wi-Fi station, to connect to a Wi-Fi network.
  - Act as a Wi-Fi AP, to accept connection to the LinkIt 7687 HDK using a
    handheld device or a laptop computer.
  - Ping out/in the device.

@par Hardware and software environment

  - Supported platform
    - MediaTek LinkIt 7687 HDK.

  - HDK switches and pin configuration
    - J36 provides the pins of GPIOs, PWMs, SPI master chip select 0, SPI
      master, UART1 RX/TX.
    - J35 provides the pins of GPIOs, PWMs, UART2 RX/TX, UART1 RTS/CTS, SPI
      master chip select 1, IR TX, and IR RX.
    - J34 provides the pins of GPIOs, PWMs, UART2 RTS/CTS, I2S, SPI slave, and
      I2C0.
    - J33 provides the pins of GPIOs, PWMs, I2C0, ADC0~3.
    - J32 provides the pins of GND, 5V, 3.3V, and reset pin.
    - J25 controls the pins of the mode of MT7687. Place a jumper over it,
      during the next boot-up, the MT7687 will boot under Flash Recovery Mode
      (update firmware), otherwise, it is booted into Flash Normal Mode (Run
      Application).
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
    - iot_sdk/src/main.c                Entry point of the application program.
    - iot_sdk/src/sys_init.c            Aggregated initialization routines.
    - iot_sdk/src/system_mt7687.c       MT7687 system clock configuration file.

    - iot_sdk/inc/flash_map.h           MT7687 memory layout symbol file.
    - iot_sdk/inc/hal_feature_config.h  MT7687 HAL API feature configuration
                                        file.
    - iot_sdk/inc/FreeRTOSConfig.h      MT7687 FreeRTOS configuration file.
    - iot_sdk/inc/lwipopts.h            LwIP configuration.
    - iot_sdk/inc/sys_init.h            Prototype declaration for iot_sdk/src/sys_init.c.
    - iot_sdk/inc/ept_eint_drv.h        The EINT configuration file generated
                                        by Easy Pinmux Tool(EPT). Please do
                                        not edit the file.
    - iot_sdk/inc/ept_gpio_drv.h        The GPIO configuration file generated
                                        by Easy Pinmux Tool(EPT). Please do
                                        not edit the file.
    - iot_sdk/inc/cli_cmds.h            Declares the reference point of CLI
                                        commands of cli_cmds.c. To be used
                                        by cli_def.c

  - Project configuration files using GCC.
    - iot_sdk/GCC/Makefile              GNU Makefile for this project.
    - iot_sdk/GCC/feature.mk            Generic feature options configuration
                                        file.
    - iot_sdk/GCC/mt7687_flash.ld       Linker script.
    - iot_sdk/GCC/startup_mt7687.s      MT7687 startup file.
    - iot_sdk/GCC/syscalls.c            MT7687 syscalls implementation.
    - iot_sdk/src/ept_eint_var.c        EINT configuration file generated by
                                        Easy Pinmux Tool(EPT). Please do
                                        not edit the file.
    - iot_sdk/src/ept_gpio_var.c        GPIO configuration file generated by
                                        Easy Pinmux Tool(EPT). Please do not
                                        edit the file.
    - iot_sdk/src/cli_cmds.c            CLI commands of this project.
    - iot_sdk/src/cli_def.c             CLI initialize sequence code.
    - iot_sdk/src/network_init.c        Network initialize sequence code.
    - iot_sdk/src/user_config.c         Default user configuration file.

  - Project configuration files using Keil.
    - iot_sdk/MDK-ARM/RTE/Device/7687_32M_MXIC.FLM
    - iot_sdk/MDK-ARM/RTE/Device/flash.ini
    - iot_sdk/MDK-ARM/RTE/RTE_Components.h
    - iot_sdk/MDK-ARM/flash.sct          Scatter file.
    - iot_sdk/MDK-ARM/iot_sdk.uvoptx     uVision5 project options. Contains the
                                         settings for the debugger, trace
                                         configuration, breakpoints, currently
                                         open files, etc.
    - iot_sdk/MDK-ARM/iot_sdk.uvprojx    uVision5 Project File. Contains the
                                         project structure in XML format.
    - iot_sdk/MDK-ARM/startup_mt7687.s   MT7687 startup file.

@par Run the examples
  - Connect the board to PC with a serial port cable.
  - Build the example project and download the binary file to the MT7687.
  - Reboot the HDK, the console will show "FreeRTOS Running" message to
    indicate the HDK is booting up.
  - Use '?' and enter to query the available command line options.
    Note that the command line options are still under development and subject to
    change without notice.
  - Below are two examples to demonstrate the Wi-Fi station and Wi-Fi access
    point modes of MT7687.
  - Example 1. Wi-Fi station mode.
    - Find your Wi-Fi access point settings:
      Before connecting to a Wi-Fi access point, the following information
      needs to be collected:
      1. What is the SSID of your Wi-Fi access point?
      2. What is the authentication mode of your Wi-Fi access point?
         In general, it is WPA PSK or WPA2 PSK. If you want to use different
         settings, please read the table 'Table 1. Supported AuthMode(s)' at
         the bottom of this example.
      3. What is the password of your Wi-Fi access point?
      4. What is the encryption mode of your Wi-Fi access point?
         In general, AES or TKIP is used. If you want to use different settings,
         please read the table 'Table 2. Supported EncrypType(s)' at the bottom
         of this example.
    - Once you have this information, use the following commands to configure
      the HDK. This example assumes you want to use either WPA PSK or WPA2 PSK,
      your packets are encrypted with TKIP or AES, the access point SSID is
      'myhome' (length 6), and the password of WPA or WPA2 is '12345678'
      (length 8).

        config write STA AuthMode 9
        config write STA EncrypType 8
        config write STA Ssid myhome
        config write STA SsidLen 6
        config write STA WpaPsk 12345678
        config write STA WpaPskLen 8
        config write common OpMode 1

        press the reset button on the LinkIt 7687 HDK to restart the system.
    - Boot up with the new configuration.

      If everything is correct, similar messages will be shown in the console
      to notify your HDK has received an IP address.

        ************************
        DHCP got IP:10.10.10.101
        ************************

    - PING from the LinkIt 7687 HDK (SDK v3.1.0)
      If the IP address is fetched and the network is operating, the LinkIt
      7687 can ping other devices/computer on the network with the following
      command in the console.

        f 11 10.10.10.254 3 64

      The ping stops after sending three packets to 10.10.10.254.
      The ping usage is: f 11 <ip address> <times> <ping packet length>

    - Wi-Fi configuration options for AuthMode and EncrypType.
      +---+-------------------------------------+
      | 0 | open, no security                   |
      +---+-------------------------------------+
      | 4 | WPA PSK                             |
      +---+-------------------------------------+
      | 7 | WPA2 PSK                            |
      +---+-------------------------------------+
      | 9 | Support both WPA and WPA2 PSK       |
      +---+-------------------------------------+
      Table 1. Supported AuthMode(s)

      +---+------------------------+
      | 0 | WEP                    |
      +---+------------------------+
      | 1 | No encryption          |
      +---+------------------------+
      | 4 | TKIP                   |
      +---+------------------------+
      | 6 | AES                    |
      +---+------------------------+
      | 8 | Support TKIP and AES   |
      +---+------------------------+
      Table 2. Supported EncrypType(s)
  - Example 2. Wi-Fi access point mode.
    - Decide Wi-Fi access point settings:
      - SSID
      - Authentication Mode
      - Encryption Type.
      - Password

      You need the above settings before proceeding.
    - Once the information is collected, use the following commands to
      configure the LinkIt 7687 HDK. This example assumes WPA2 PSK is used for
      authentication, AES for encryption, 'iot_ap' (length 6) for the SSID,
      and the password of WPA2 as '87654321' (length 8).

        config write AP Ssid iot_ap
        config write AP SsidLen 6
        config write AP AuthMode 7
        config write AP EncrypType 6
        config write AP WpaPsk 87654321
        config write AP WpaPskLen 8
        config write common OpMode 2

        press reset button on the LinkIt 7687 HDK to restart the system.
    - Use a handheld device or a laptop computer to connect to the access point
      'iot_ap'. In the MT7687 console, the IP address assigned to the cellphone
      or laptop is shown as below.

        [DHCPD:DBG]lease_ip:10.10.10.2

* @}
*/
