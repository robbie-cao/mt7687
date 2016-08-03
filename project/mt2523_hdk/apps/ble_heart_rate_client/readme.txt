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
  * @page ble_heart_rate_client EPT example project
  * @{

@par Overview

 - Example project description
   - This example shows how to use the APIs of GATTC module.
 - Features of the application
   - This example project demonstrates how to get HeartRate data from HeartRate sensor via BLE link.

@par Hardware and software environment

  - Supported platform
    MediaTek LinkIt 2523 HDK.

  - PC/environment configuration
   - A serial tool is required, such as hyper terminal, for UART logging.
   - COM port settings. baudrate: 115200, data bits: 8, stop bit: 1, parity: none and flow control: off.

@par Directory contents
  - Source and header files
   - apps/ble_heart_rate_client/src/main.c  		     Main program
   - apps/ble_heart_rate_client/src/system_mt2523.c	     MT2523 system clock configuration file
   - apps/ble_heart_rate_clientsrc/ble_gattc_connect.c       Connection manager
   - apps/ble_heart_rate_client/src/ble_gattc_handle_op.c    Discover procedure and other operation
   - apps/ble_heart_rate_client/src/ble_gattc_service.c	     GATTC service file
   - apps/ble_heart_rate_client/src/hr_app.c		     Show HeartRate data
   - apps/ble_heart_rate_client/src/hr_client.c		     Parse HeartRate data
   - apps/ble_heart_rate_client/GCC/startup_mt2523.s	     MT2523x's startup file for GCC.
   - apps/ble_heart_rate_client/GCC/syscalls.c		     MT2523x's syscalls for GCC.
  
  - Project configuration files using GCC
   - apps/ble_heart_rate_client/GCC/feature.mk		Feature configuration.
   - apps/ble_heart_rate_client/GCC/Makefile		Makefile.
   - apps/ble_heart_rate_client/GCC/flash.ld	        Linker script.

@par Run the example
  - Connect board to PC with serial port cable.
  - Build the example project with the command, "./build.sh mt2523_hdk ble_heart_rate_client bl" from the SDK root folder and download the bin file to LinkIt 2523 development board.
  - Run the example:
   - Input "s" to find the BLE devices and input "c address" to connect the HeartRate sensor.
   - After BLE connectted with HeartRate sensor, the HeartRate data will be shown in the log.
* @}
*/
