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
  * @page pxp_sample General pxp project guidelines
  * @{
  
@par Overview

  - Application description 
    The application defines the behavior when a device moves away from a peer device 
    so that the connection is dropped or the path loss increases above a preset level, 
    causing an immediate alert or interupt display. This alert can be used to notify 
    the user that the devices have become separated. 
  
  - Features of the application
    - "Unconnected" display when a device disconnect with a peer device; 
    - "Connected" display when a device connect with a peer device; 
    - "Out of range" display when a device moves away from a peer device so that the path 
      loss increases above a preset level;   
  
@par Hardware and Software environment  

  - Supported Chipset/HW  
    - This application runs on mt2523 or mt7687 or mt7697 devices.
    
    - This application has been tested with mt7687 and mt7697 evB and 
      can be easily tailored to any other supported device and development board.

@par Directory contents 

  - mt2523_hdk/apps/iot_sdk_demo/src/pxp_screen/ble_gatts_srv_common.c        gatt server common function file
  - mt2523_hdk/apps/iot_sdk_demo/src/pxp_screen/ble_message.c                 gatt and gap message file
  - mt2523_hdk/apps/iot_sdk_demo/src/pxp_screen/ble_static_callback.c         gatt and gap static callback file
  - mt2523_hdk/apps/iot_sdk_demo/src/pxp_screen/ble_ias.c                     gatt service ias header file 	
  - mt2523_hdk/apps/iot_sdk_demo/src/pxp_screen/ble_lls.c                     gatt service lls header file
  - mt2523_hdk/apps/iot_sdk_demo/src/pxp_screen/ble_tps.c                     gatt service tps header file
  - mt2523_hdk/apps/iot_sdk_demo/src/pxp_screen/ble_pxp_main.c                PXP prfile and UI Main program file   
  - mt2523_hdk/apps/iot_sdk_demo/inc/pxp_screen/ble_pxp_main.h                PXP prfile and UI Main program header file
  - mt2523_hdk/apps/iot_sdk_demo/inc/pxp_screen/ble_ias.h                     gatt service ias header file
  - mt2523_hdk/apps/iot_sdk_demo/inc/pxp_screen/ble_lls.h                     gatt service lls header file
  - mt2523_hdk/apps/iot_sdk_demo/inc/pxp_screen/ble_tps.h                     gatt service tps header file
  - mt2523_hdk/apps/iot_sdk_demo/inc/pxp_screen/ble_message.h                 ble message header file
  - mt2523_hdk/apps/iot_sdk_demo/inc/pxp_screen/ble_gatts_srv_common.h        gatt server header file 
  - mt2523_hdk/apps/iot_sdk_demo/inc/pxp_screen/ble_app_utils.h               ble app utils header file 
  - mt2523_hdk/apps/iot_sdk_demo/inc/pxp_screen/ble_bds_app_util.h            bds tool header file 
  
  
@par Run the demo

  - Step by step to illustrate how to use the demo application, include PC/other board's interoperabilities.  
  
  
  * @}
*/