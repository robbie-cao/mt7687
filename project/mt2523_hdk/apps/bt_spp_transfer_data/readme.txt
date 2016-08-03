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
  * @page spp_example data transmission via BT SPP example project
  * @{

@par Overview
 - Example project description
This example project describes how to use BT SPP API to create a SPP server and a SPP client.

@par Hardware and software environment

  - Supported platform
   - LinkIt 2523 HDK.

@par Directory contents
  - Source and header files
   - apps/bt_spp_transfer_data/src/main.c				   Main program
   - apps/bt_spp_transfer_data/src/system_mt2523.c			   MT2523x's system clock configuration file.
   - apps/bt_spp_transfer_data/src/bt_spp_main.c                           BT SPP main file.
   - apps/bt_spp_transfer_data/src/bt_spp_client.c                         BT SPP client example file.
   - apps/bt_spp_transfer_data/src/bt_spp_server.c                         BT SPP server example file.
   - apps/bt_spp_transfer_data/inc/bt_spp_main.h                           BT SPP main header file.
   - apps/bt_spp_transfer_data/inc/bt_spp_client.h                         BT SPP client example header file.
   - apps/bt_spp_transfer_data/inc/bt_spp_server.h                         BT SPP server example header file.

@par Prepare the example

  - PC/environment configuration
    Use SecureCRT or other hyperterminal to output system log.
  - Remote device configuration
    Install any Bluetooth SPP application that supports both the SPP server and the SPP client on the Smartphone from App Store.

@par Run the example

  - Connect board to PC with serial port cable.
  - Set the correct Smartphone Bluetooth address in the BT SPP client example file, then build the example project and download the binary file to the LinkIt 2523 development board.
  - Launch SPP application and enable SPP server on the Smartphone.
  - Power on the LinkIt 2523 development board, the SPP client example will automatically connect to the Smartphone.
  - The SPP client example will send string of "Hello SPP Server!" to the Smartphone.
  - Disconnects the SPP connection from the Smartphone.
  - Initiate the connection to SPP server example from the Smartphone.
  - The SPP server example will send string of "Hello SPP Client!" to the Smartphone.
  - Disconnect the SPP connection from the Smartphone.
* @}
*/
