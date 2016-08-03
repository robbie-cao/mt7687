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
  * @page hal_i2c_master I2C example project
  * @{

@par Overview

 - Example project description
   - This example project shows the useage of APIs of I2C module and it runs without FreeRTOS.
 - Features of the example project
   - This example project demonstrates how to use I2C API to communicate with an I2C M24C01 EEPROM device with I2C polling mode. 
 - Output from the example
   - Log will show whether the data transaction is correct or not .

@par Hardware and software environment

  - Supported platform
    - LinkIt 2523 HDK

  - HDK switches and pin configuration
    - I2C0 pin mapping is listed below.
  |GPIOx	|PINx	|
  |---------|---------|
  |  36	    |J1041.2 |
  |  37	    |J1041.1 |


  - PC/environment configuration
   - A serial tool is required, such as hyper terminal, for UART logging.
   - COM port settings. baudrate: 115200, data bits: 8, stop bit: 1, parity: none and flow control: off.

@par Directory contents
  - Source and header files
   - hal_examples/i2c_communication_with_EEPROM_polling/src/main.c  			Main program.
   - hal_examples/i2c_communication_with_EEPROM_polling/src/system_mt2523.c			MT2523x's clock configuration file.
   - hal_examples/i2c_communication_with_EEPROM_polling/inc/hal_feature_config.h 		MT2523x's feature configuration file.
   - hal_examples/i2c_communication_with_EEPROM_polling/inc/memory_map.h			MT2523x's memory layout symbol file.
   - hal_examples/i2c_communication_with_EEPROM_polling/GCC/startup_mt2523.s		MT2523x's startup file for GCC.
   - hal_examples/i2c_communication_with_EEPROM_polling/GCC/syscalls.c			MT2523x's syscalls for GCC.
   - hal_examples/i2c_communication_with_EEPROM_polling/MDK-ARM/startup_mt2523.s		MT2523x's startup file for Keil.
   - hal_examples/i2c_communication_with_EEPROM_polling/EWARM/startup_mt2523.s		MT2523x's startup file for IAR.   

  - Project configuration files using GCC
   - hal_examples/i2c_communication_with_EEPROM_polling/GCC/feature.mk		    	Feature configuration.
   - hal_examples/i2c_communication_with_EEPROM_polling/GCC/Makefile		 	Makefile.
   - hal_examples/i2c_communication_with_EEPROM_polling/GCC/flash.ld			Linker script.

  - Project configuration files using Keil
   - hal_examples/i2c_communication_with_EEPROM_polling/MDK-ARM/i2c_communication_with_EEPROM_polling.uvprojx		uVision5 project file. Contains the project structure in XML format.
   - hal_examples/i2c_communication_with_EEPROM_polling/MDK-ARM/i2c_communication_with_EEPROM_polling.uvoptx		uVision5 project options. Contains the settings for the debugger, trace configuration, breakpoints, currently open files, etc.
   - hal_examples/i2c_communication_with_EEPROM_polling/MDK-ARM/flash.sct					Linker script.
   
  - Project configuration files using IAR
   - hal_examples/i2c_communication_with_EEPROM_polling/EWARM/i2c_communication_with_EEPROM_polling.ewd		IAR project options. Contains the settings for the debugger.
   - hal_examples/i2c_communication_with_EEPROM_polling/EWARM/i2c_communication_with_EEPROM_polling.ewp     IAR project file. Contains the project structure in XML format
   - hal_examples/i2c_communication_with_EEPROM_polling/EWARM/i2c_communication_with_EEPROM_polling.eww     IAR workspace file. Contains project information.
   - hal_examples/i2c_communication_with_EEPROM_polling/EWARM/flash.icf					                    Linker script.

@par Run the example
  - Connect board to PC with serial port cable.
  - Build the example project with the command, "./build.sh mt2523_hdk i2c_communication_with_EEPROM_polling bl" from the SDK root folder and download the bin file to LinkIt 2523 development board.
  - Connect J1041.2 and J1041.1 to M24C01 EEPROM.
  - Run the example. The log will display the result of the transaction.

* @}
*/
