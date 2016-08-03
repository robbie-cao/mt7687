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
  * @page hal_ept EPT example project
  * @{

@par Overview

 - Example project description
   - This example project shows how to use the EPT (Easy Pinmux Tool) to configure and initialize the I/O pins on the target board.
 - Features of the application
   - This example project demonstrates how to use the EPT generated files to configure and initialize the I/O pins on the target board.

@par Hardware and software environment

  - Supported platform
    - LinkIt 7687 HDK.


  - PC/environment configuration
   - A serial tool is required, such as hyper terminal, for UART logging.
   - COM port settings. baudrate: 115200, data bits: 8, stop bit: 1, parity: none and flow control: off.

@par Directory contents
  - Source and header files
   - hal_examples/ept_configure_pins_by_ept/src/main.c  		Main program
   - hal_examples/ept_configure_pins_by_ept/src/ept_gpio_var.c        	GPIO pin assignment with variable names.
   - hal_examples/ept_configure_pins_by_ept/src/ept_eint_var.c        	EINT assignment file with variable names.
   - hal_examples/ept_configure_pins_by_ept/src/system_mt7687.c		MT76x7's system clock configuration file.
   - hal_examples/ept_configure_pins_by_ept/inc/hal_feature_config.h 	MT76x7's feature configuration file.
   - hal_examples/ept_configure_pins_by_ept/inc/ept_eint_drv.h		EINT basic configuration header file.
   - hal_examples/ept_configure_pins_by_ept/inc/ept_gpio_drv.h		GPIO pin assignment and basic configuration file.
   - hal_examples/ept_configure_pins_by_ept/inc/flash_map.h	        MT76x7's memory layout symbol file.
   - hal_examples/ept_configure_pins_by_ept/GCC/startup_mt7687.s	MT76x7's startup file for GCC.
   - hal_examples/ept_configure_pins_by_ept/GCC/syscalls.c		MT76x7's syscalls for GCC.
   - hal_examples/ept_configure_pins_by_ept/MDK-ARM/startup_mt7687.s    MT76x7's startup file for Keil.

  - Project configuration files using GCC.
   - hal_examples/ept_configure_pins_by_ept/GCC/feature.mk		Feature configuration.
   - hal_examples/ept_configure_pins_by_ept/GCC/Makefile		Makefile.
   - hal_examples/ept_configure_pins_by_ept/GCC/flash.ld		Linker script.

  - Project configuration files using Keil.
   - hal_examples/ept_configure_pins_by_ept/MDK-ARM/ept_configure_pins_by_ept.uvprojx		uVision5 project File. Contains the project structure in XML format.
   - hal_examples/ept_configure_pins_by_ept/MDK-ARM/ept_configure_pins_by_ept.uvoptx		uVision5 project options. Contains the settings for the debugger, trace configuration, breakpoints, currently open files, etc.
   - hal_examples/ept_configure_pins_by_ept/MDK-ARM/flash.sct			                Linker script.
   
  - Project configuration files using IAR
   - hal_examples/ept_configure_pins_by_ept/EWARM/ept_configure_pins_by_ept.ewd	    IAR project options. Contains the settings for the debugger.
   - hal_examples/ept_configure_pins_by_ept/EWARM/ept_configure_pins_by_ept.ewp     IAR project file. Contains the project structure in XML format.
   - hal_examples/ept_configure_pins_by_ept/EWARM/ept_configure_pins_by_ept.eww     IAR workspace file. Contains project information.
   - hal_examples/ept_configure_pins_by_ept/EWARM/flash.icf			    Linker script.

@par Run the example
  - Copy files generated by EPT to the path of target project and make sure that the files can be built(This step has to be done).
   - Copy ept_gpio_var.c and ept_eint_var.c to /project/mt7687_hdk/hal_examples/ept_configure_pins_by_ept/src/.
   - Copy ept_eint_drv.h and ept_gpio_drv.h to /project/mt7687_hdk/hal_examples/ept_configure_pins_by_ept/inc/.
  - Copy program files for pin initialization to the path of target project and make sure that the files can be built, and then call ept_gpio_setting_init() whitch is implemented in the program file in main.c to make configuration of GPIO by EPT go into effect(This step has to be done).
   - Call the API of bsp_ept_gpio_setting_init() to take configuration of EPT into effect to initialize pins of the platform.
  - Connect board to the PC with serial port cable.
  - Build the example project with the command, "./build.sh mt7687_hdk ept_configure_pins_by_ept" under the SDK root folder and download the binary file to LinkIt 7687 development board.
  - Run the example.


@par Configuration
  - There are several variable names that can be assigned to different pins according to hardware and software design with help of EPT. All the GPIO and EINT variable names and the role assignment are provided in the table below. The software can use variable names to configure the corresponding pin without the need to know the pin number in the low level. This simplifies the work of porting the software to different platform.

    As for the GPIO variable name, it is necessary to assign the GPIO variable name to the correct pin according to the current pinmux setting or the corresponding peripheral may not work properly or the platform cannot boot. Taking variable name of "HAL_KEYPAD_COL2_PIN" for example, if one pin works in KCOL1 mode for keypad, variable name of "HAL_KEYPAD_COL2_PIN" should be assigned to this pin, or something will be wrong with keypad module. The role assignment of all the GPIO variable names can be obtained from the table below.

    As for the EINT variable name, it has the same effect as GPIO variable name other than that it is assigned to a different EINTx on the EINT page of EPT if the EINT has been assigned to a pin. Taking EINT variable name of "HAL_ACCDET_EINT" for example, if one pin is configured to work in EINT mode for ACCDET, variable name of "HAL_ACCDET_EINT" should be assigned to this pin, or something will be wrong with ACCDET module or the platform cannot boot.

    All the variable name and role assignment can be obtained from the table below where "comment" means extra attention to be paid to.

    | Module|Role of variable names assigned to pins |  GPIO variable name           | EINT variable name |  Comment                                     |
    |-------|----------------------------------------|-------------------------------|--------------------|----------------------------------------------|
    |keypad |Chosen as KCOL2 for keypad              |HAL_KEYPAD_COL2_PIN            |                    |                                              |
    |keypad |Chosen as KCOL1 for keypad              |HAL_KEYPAD_COL1_PIN            |                    |                                              |
    |keypad |Chosen as KCOL0 for keypad              |HAL_KEYPAD_COL0_PIN            |                    |                                              |
    |keypad |Chosen as KROW2 for keypad              |HAL_KEYPAD_ROW2_PIN            |                    |                                              |
    |keypad |Chosen as KROW1 for keypad              |HAL_KEYPAD_ROW1_PIN            |                    |                                              |
    |keypad |Chosen as KROW0 for keypad              |HAL_KEYPAD_ROW0_PIN            |                    |                                              |
    |CTP    |Chosen as GPIOx for TP_RST              |BSP_CTP_RST_PIN                |                    |                                              |
    |CTP    |Chosen as EINTx for TP_EINT             |BSP_CTP_EINT_PIN               |BSP_CTP_EINT        |Pull up the pin corresponding to this EINTx.  |
    |CTP    |Chosen as SCLx for TP_I2C_SCL           |BSP_CTP_SCL_PIN                |                    |                                              |
    |CTP    |Chosen as SDAx for TP_I2C_SDA           |BSP_CTP_SDA_PIN                |                    |                                              |
    |ACCDET |Chosen as EINTx for ACCDET              |                               |HAL_ACCDET_EINT     |Disable the pin corresponding to this EINTx.  |
    |AUDIO  |Chosen as "SPEAKER" for audio           |BSP_SPEAKER_EBABLE_PIN         |                    |                                              |
    |AUDIO  |Chosen as "AUXADC" for audio            |BSP_AUXADC_ENABLE_PIN          |                    |                                              |
    |MSDC   |Chosen as EINTx for MSDC                |                               |HAL_MSDC_EINT       |Pull up the pin corresponding to this EINTx.  |
    |GNSS   |Chosen as GPIOx for GNSS_LDO_EN         |BSP_GNSS_POWER_PIN             |                    |                                              |
    |GNSS   |Chosen as EINTx for GNSS_EINT           |                               |BSP_GNSS_EINT       |                                              |
    |Senser |Chosen as GPIOx for PPG_VDRV_EN         |BSP_BIO_SENSOR_PPG_VDRV_EN     |                    |                                              |
    |Senser |Chosen as 32k clock for bio sensor      |BSP_BIO_SENSOR_32K             |                    |                                              |
    |Senser |Chosen as GPIOx for reset control       |BSP_BIO_SENSOR_RST_PORT_PIN    |                    |                                              |
    |Senser |Chosen as GPIOx for power down control  |BSP_BIO_SENSOR_AFE_PWD_PIN     |                    |                                              |
* @}
*/
