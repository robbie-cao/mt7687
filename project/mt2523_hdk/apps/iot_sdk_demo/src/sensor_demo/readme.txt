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
  * @page sensor_demo Sensor application
  * @{

@par Overview

  - Application description
    - This application demonstrates how to obtain HR(Heart rate) and HRV(Heart rate variability) data from MT2511 on sensor DTB(daughter board). The HR and HRV data is displayed on the UI display and transferred to Android smartphone via Bluetoth.
  - Features of the application
    - This application demonstrates how to use the subscribe API to get the HR and HRV data.
    - UI displays the HR and HRV data.
    - Send the HR and HRV data to Android smartphone via Bluetooth.
  - Output from the application
    - Display the HR and HRV data.
    - Send the HR and HRV data from LinkIt 2523 HDK to Android smartphone via Bluetooth.
    - Show the HR and HRV data on Android smartphone.

@par Hardware and software environment

  - Supported platform
    - LinkIt 2523 HDK.
    - Sensor DTB. (Model number: WS3437)
    - PPG(photoplethysmography) module board. (Model number: WS3499)

  - PC/environment configuration
    - Require a patient cable.
    - Require an Android smartphone.
    - Install Sensors_DataShow.apk on the Android smartphone. (Find Sensors_DataShow.apk in the released apps package.)
    - Sensor DTB needs to be connected with LinkIt 2523 HDK. The interface is near the Audio jack with "Sensor" labeled on LinkIt 2523 HDK.
    - The patient cable needs to be connected to the sensor DTB.
    - PPG module board needs to be connected to the sensor DTB. PPG module board use a light-based technology to sense the rate of blood flow as controlled by the heart’s pumping action.

@par Directory contents
  - Source and header files
   - iot_sdk_demo/src/sensor_demo/sensor_demo.c                 The source file of the sensor initialization functions.
   - iot_sdk_demo/src/sensor_demo/sensor_demo_screen.c          The display configuration file of the sensor demo.
   - iot_sdk_demo/inc/sensor_demo/sensor_demo.h                 The header file of the sensor initialization functions.

@par Run the application
  - Press "Sensor Subsystem" on the main display.
  - The patient cable has three leads with electrode patch. Stick the green and black electrode patches to the left arm, stick the red electrode patch to the right arm.
  - Put a hand on the PPG module board.
  - Observe the HR and HRV data change on the display.
    - Sensor subsystem display will show the HR data changes.
    - Press "HRV" on the sensor subsystem display.
      - Sensor subsystem display will show the HRV data measuring.
      - Wait a few minutes for the HRV data measuring.
      - Sensor subsystem display will show the HRV measuring result.
  - Observe the HR and HRV data change on Android smartphone.
    - Bluetooth pairing between Android smartphone and LinkIt 2523 HDK. (Bluetooth device name is "MTK Device".)
    - Launch Sensors_DataShow App on Android smartphone.
      - Sensors_DataShow App displays a list of Bluetooth devices.
      - Press "MTK Device" on Sensors_DataShow App.
      - Receive the HR and HRV data from LinkIt 2523 HDK after few seconds.
    - Observe the HR and HRV data change on Sensors_DataShow App.
      - Sensors_DataShow App will show HR data change.
      - Press "HRV" on the sensor subsystem display.
        - Sensor subsystem display will show the HRV data measurements.
        - Wait for a few minutes for the HRV data measuring.
        - Sensors_DataShow App will show the HRV measured result.

* @}
*/
