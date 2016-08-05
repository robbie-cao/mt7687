# MediaTek MT7687/7697 LinkIt Platform

## Introduction

MediaTek LinkIt(TM) Development Platform for RTOS provides a comprehensive software solution for devices based
on the MT76x7 and MT2523x SOCs.

1. Getting started

   The SDK package supports GCC, Keil, and IAR tool chains. Follow the instructions at <sdk_root>/doc/LinkIt_for_RTOS_Get_Started_Guide.pdf
   to build your first project and run it on the HDK. Release notes are also under the <sdk_root>/doc folder.

2. Folder structure

  ```
  \doc                      ->  SDK documents, including get started guide, developer's guide, software and environment guide,
                            ->  API reference manual, and SDK release notes.
  \doc\HDK                  ->  HDK documents, including user guide, layout information, schematics, daughterboard information,
                            ->  BOM list. The HDK relative application notes are also in this folder, such as power measurement guide.
  \doc\MCU                  ->  Chipset documents, such as chipset product brief and chipset datasheet.
  \driver\board             ->  Drivers for the development board
  \driver\chip              ->  Chipset modules, such as GPIO, I2C, Bluetooth Low Energy.
  \driver\CMSIS             ->  CMSIS interface drivers.
  \driver\inc               ->  HAL driver APIs
  \kernel                   ->  OS / system service
  \middleware\MTK           ->  MediaTek middleware. Read readme.txt in each module for details.
  \middleware\third_party   ->  Open source software, such as cjson, fatfs, httpd. Read readme.txt in each module for details.
  \project\<board>          ->  Example projects of the current SDK. Read <sdk_root>/project/readme.txt for more details.
  \tools                    ->  Script, generation script, gcc compiler. If there isn't a gcc compiler under the /tools/gcc folder,
                            ->  extract the tool package to the root folder of the SDK with the following command.
                            ->       tar -C <sdk_root> -xvzf SDK_VX.Y.Z_tool_chain.tar.gz
                            **  Please make sure you have /tools/gcc before you build the SDK under linux environment. **
  ```

## Overview

MediaTek LinkIt(TM) Development Platform for RTOS provides a comprehensive software solution for devices based on the MediaTek MT7687F SOC,
including the LinkIt 7687 hardware development kit (HDK). The platform supports hardware abstraction layers, peripheral drivers, FreeRTOS,
Wi-Fi and lwIP modules. This API reference describes the features of and how to use the APIs for each of the supported modules.
Each module has a readme.txt under the module root directory. It contains the information about the module dependency, feature options,
notes and brief introduction. Please refer to the readme.txt to learn the basic usage of the module.

![Architecture](http://home.labs.mediatek.com/media/apis/7687/MT7687_SDK_architecture.PNG)

The architecture layers are described below:

- BSP
  - **Hardware drivers** provide peripheral drivers for the development platform, such as ADC, I2S, I2C, SPI, RTC, GPIO, UART, Flash, Security Engine, TRNG, GDMA, PWM, WTD and IRDA TX/RX.
  - **Hardware Abstraction Layer (HAL)** provides driver Application Programming Interface (API) encapsulating the low-level functions of peripheral drivers for the operating system (OS), middleware features and applications.
  - **FreeRTOS** provides real-time OS with open source software for middleware features and projects.
- Middleware
  - **Wi-Fi** provides OS dependent function calls, including Wi-Fi APIs that control the bridge supplicant and network processor messages.
  - **Network** provides OS dependent features, such as IPv4, Hyper-Text Transfer Protocol (HTTP) client and the Simple Network Time Protocol (SNTP).
  - **Advanced features** include Non-Volatile Random Access Memory (NVRAM), JavaScript Object Notation (JSON) and other features that are dependent on HAL and the OS.
- Example project
  - Pre-configured projects, such as Wi-Fi station and Wi-Fi access point.

## Installation

### GCC

**Mac OS**

  ```
  $ brew install gcc-arm-none-eabi
  $ cd <linkit_sdk_root>
  $ ln -s /usr/local tools/gcc/mac/gcc-arm-none-eabi
  ```

> http://robbie-cao.github.io/blog/2016/05/17/install-gcc-arm-on-mac

**Linux**

  ```
  $ cd ~
  $ wget https://launchpad.net/gcc-arm-embedded/5.0/5-2016-q2-update/+download/gcc-arm-none-eabi-5_4-2016q2-20160622-linux.tar.bz2
  $ mkdir Tools
  $ tar jxvf gcc-arm-none-eabi-5_4-2016q2-20160622-linux.tar.bz2 -C Tools
  $ cd <linkit_sdk_root>
  $ ln -s ~/Tools/gcc-arm-none-eabi-5_4-2016q2 tools/gcc/linux/gcc-arm-none-eabi
  ```

**Cygwin**

  ```
  $ cd ~
  $ wget https://launchpad.net/gcc-arm-embedded/5.0/5-2016-q2-update/+download/gcc-arm-none-eabi-5_4-2016q2-20160622-win32.zip
  $ mkdir Tools
  $ unzip gcc-arm-none-eabi-5_4-2016q2-20160622-win32.zip -d Tools
  $ cd <linkit_sdk_root>
  $ ln -s ~/Tools/gcc-arm-none-eabi-5_4-2016q2 tools/gcc/win/gcc-arm-none-eabi
  ```

**Build in Cygwin NOT support yet.**

### IAR

## Flashing

## Misc

## Reference

- http://labs.mediatek.com/site/global/developer_tools/mediatek_linkit_rtos/sdk_intro/index.gsp

