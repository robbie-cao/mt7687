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
  * @page bt_pbapc_get_phonebook_objects EPT example project
  * @{

@par Overview

 - Example project description
   - This example shows how to use the APIS of PBAPC module.
 - Features of the application
   - This example project demonstrates how to access the phonebook objects from a remote device.

@par Hardware and software environment

  - Supported platform
    - MediaTek LinkIt 2523 HDK.


  - PC/environment configuration
   - A serial tool is required, such as hyper terminal, for UART logging.
   - COM port settings. baudrate: 115200, data bits: 8, stop bit: 1, parity: none and flow control: off.

@par Directory contents
  - Source and header files
   - apps/bt_pbapc_get_phonebook_objects/src/main.c  		Main program
   - apps/bt_pbapc_get_phonebook_objects/src/app_pbapc.c	PBAPC app file
   - apps/bt_pbapc_get_phonebook_objects/GCC/startup_mt2523.s	MT2523x's startup file for GCC
   - apps/bt_pbapc_get_phonebook_objects/GCC/syscalls.c		MT2523x's syscalls for GCC

  - GCC version project configuration files
   - apps/bt_pbapc/GCC/feature.mk		    	     Feature configuration file of GCC version project
   - apps/bt_pbapc/GCC/Makefile		 	             Make file of GCC version project
   - apps/bt_pbapc/GCC/flash.ld			             Linker script of GCC version project
  - Project configuration files using GCC.
   - hal_examples/ept_configure_pins_by_ept/GCC/feature.mk		Feature configuration.
   - hal_examples/ept_configure_pins_by_ept/GCC/Makefile		Makefile.
   - hal_examples/ept_configure_pins_by_ept/GCC/flash.ld		Linker script.

@par Run the example
  - Connect board to the PC with serial port cable.
  - Build the example project with the command, "./build.sh mt2523_hdk bt_pbapc_get_phonebook_objects bl" under the SDK root folder and download the binary file to LinkIt 2523 development board.
   -Power on the development board, and input "pb address" to connect to a remote device.
   -Get phonebook objects, the data will be shown in the log.
* @}
*/
