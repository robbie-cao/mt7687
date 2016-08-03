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
  * @page BT Audio example application
  * @{

@par Overview

 - Example application description
   The BT Sink application works as a Bluetooth headset
   which supports Bluetooth Hands-Free Profile,
   Advanced Audio Distribution Profile, and Audio/Video Remote Control Profile.
   The BT Sink application supports Bluetooth dual mode as well,
   which allows both BR/EDR and BLE PXP connections at the same time.
 - Features of the application
   1. Handle incoming calls, for example, answer or reject an incoming call.
   2. Dial last dialed number on the smartphone.
   3. Stream music from a music-playing device.
   4. Streaming control, such as play or pause music, move to previous or next song.
   5. Reconnect the last connected device.
   6. Connect BLE PXP.

@par Hardware and software environment

  - Refer to mt2523_hdk/apps/iot_sdk_demo/Readme.txt.

@par Directory contents
  - Source and header files
   - mt2523_hdk/apps/iot_sdk_demo/src/bt_audio/bt_audio.c                           bt audio app UI display file
   - mt2523_hdk/apps/iot_sdk_demo/src/bt_audio/bt_common_dispatch.c                 bt audio app common callback dispatch file
   - mt2523_hdk/apps/iot_sdk_demo/src/bt_audio/sink/bt_sink_a2dp.c                  bt audio app a2dp handler main file
   - mt2523_hdk/apps/iot_sdk_demo/src/bt_audio/sink/bt_sink_am_task.c               bt audio app audio manager task file
   - mt2523_hdk/apps/iot_sdk_demo/src/bt_audio/sink/bt_sink_ami.c                   bt audio app audio manager interface file
   - mt2523_hdk/apps/iot_sdk_demo/src/bt_audio/sink/bt_sink_avrcp.c                 bt audio app avrcp handler main file
   - mt2523_hdk/apps/iot_sdk_demo/src/bt_audio/sink/bt_sink_common_cb.c             bt audio app common callback wrapper file
   - mt2523_hdk/apps/iot_sdk_demo/src/bt_audio/sink/bt_sink_conmgr.c                bt audio app connection manager file
   - mt2523_hdk/apps/iot_sdk_demo/src/bt_audio/sink/bt_sink_custom_db.c             bt audio app customize database store file
   - mt2523_hdk/apps/iot_sdk_demo/src/bt_audio/sink/bt_sink_db.c                    bt audio app database control file
   - mt2523_hdk/apps/iot_sdk_demo/src/bt_audio/sink/bt_sink_event.c                 bt audio app event handler main file
   - mt2523_hdk/apps/iot_sdk_demo/src/bt_audio/sink/bt_sink_hf.c                    bt audio app hfp hander main file
   - mt2523_hdk/apps/iot_sdk_demo/src/bt_audio/sink/bt_sink_hf_call_manager.c       bt audio app call manager file
   - mt2523_hdk/apps/iot_sdk_demo/src/bt_audio/sink/bt_sink_hf_multipoint.c         bt audio app multipoint handler file
   - mt2523_hdk/apps/iot_sdk_demo/src/bt_audio/sink/bt_sink_music.c                 bt audio app a2dp/avrcp common file
   - mt2523_hdk/apps/iot_sdk_demo/src/bt_audio/sink/bt_sink_queue.c                 bt audio app event queue file
   - mt2523_hdk/apps/iot_sdk_demo/src/bt_audio/sink/bt_sink_task.c                  bt audio app main task file
   - mt2523_hdk/apps/iot_sdk_demo/src/bt_audio/sink/bt_sink_utils.c                 bt audio app utils main file
   - mt2523_hdk/apps/iot_sdk_demo/inc/bt_audio/bt_audio.h                           bt audio app UI display header file
   - mt2523_hdk/apps/iot_sdk_demo/inc/bt_audio/bt_common_dispatch.h                 bt audio app common callback dispatch header file
   - mt2523_hdk/apps/iot_sdk_demo/inc/bt_audio/sink/bt_sink_a2dp.h                  bt audio app a2dp main header file
   - mt2523_hdk/apps/iot_sdk_demo/inc/bt_audio/sink/bt_sink_am_task.h               bt audio app audio manager task main header file
   - mt2523_hdk/apps/iot_sdk_demo/inc/bt_audio/sink/bt_sink_ami.h                   bt audio app audio manager interface header file
   - mt2523_hdk/apps/iot_sdk_demo/inc/bt_audio/sink/bt_sink_avrcp.h                 bt audio app avrcp main header file
   - mt2523_hdk/apps/iot_sdk_demo/inc/bt_audio/sink/bt_sink_conmgr.h                bt audio app connection manager header file
   - mt2523_hdk/apps/iot_sdk_demo/inc/bt_audio/sink/bt_sink_custom_db.h             bt audio app customize database header file
   - mt2523_hdk/apps/iot_sdk_demo/inc/bt_audio/sink/bt_sink_db.h                    bt audio app database control header file
   - mt2523_hdk/apps/iot_sdk_demo/inc/bt_audio/sink/bt_sink_event.h                 bt audio app event interface header file
   - mt2523_hdk/apps/iot_sdk_demo/inc/bt_audio/sink/bt_sink_hf.h                    bt audio app hfp main header file
   - mt2523_hdk/apps/iot_sdk_demo/inc/bt_audio/sink/bt_sink_music.h                 bt audio app a2dp/avrcp main header file
   - mt2523_hdk/apps/iot_sdk_demo/inc/bt_audio/sink/bt_sink_queue.h                 bt audio app queue header file
   - mt2523_hdk/apps/iot_sdk_demo/inc/bt_audio/sink/bt_sink_resource.h              bt audio app event id, app status definition
   - mt2523_hdk/apps/iot_sdk_demo/inc/bt_audio/sink/bt_sink_task.h                  bt audio app task main header file
   - mt2523_hdk/apps/iot_sdk_demo/inc/bt_audio/sink/bt_sink_utils.h                 bt audio app utils interface header file

@par Run the application
  - Build source code of this demo application
    - Build the example project with the command, "./build.sh mt2523_hdk iot_sdk_demo bl" from the SDK root folder.
  - Download the bin file of the application to Linkit 2523 development board using Flash Tool.
  - Pair the Bluetooth between smartphone and Linkit 2523 development board.
  - Touch BT Audio app on the screen
  - BT Audio app show "Connecting" status and "reconnect" option
  - Touch "reconnect", Linkit 2523 development board will reconnect the last connected device
  - Use smartphone search, bond and connect Linkit 2523 development board(whose name is "BT_Audio_Demo")
  - Touch "play music", smartphone will play a music.
  - Touch "Next" or "Previous" to switch to the next or the prevous song.
  - Make an incoming calling to the smartphone and Linkit 2523 development board will show the caller number.
  - Touch "Answer call" or "Reject call" to accept or reject the incoming call.
  - After the call was answered, touch "Hang up" to terminate the active call.
  - To connect BLE PXP, please refer to mt2523_hdk/apps/iot_sdk_demo/src/pxp_screen/Readme.txt, step 2 and step 3.

* @}
*/
