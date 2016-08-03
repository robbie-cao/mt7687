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

#ifndef __BT_GAP_H__
#define __BT_GAP_H__


/**
 * @addtogroup BT
 * @{
 * @addtogroup BTGAP GAP
 * @{
 * This section describes the GAP APIs for Bluetooth with enhanced data rate (EDR) only.
 * The Generic Access Profile (GAP) defines the generic procedures related to discovery of
 * Bluetooth devices and link management aspects of connecting to Bluetooth devices.
 *
 * Terms and Acronyms
 * ======
 * |Terms                         |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b PIN                        | Personal Identification Number. A user-friendly number that can be used to authenticate connections to a device before pairing takes place. |
 * |\b COD                        | Class of Device, indicating the type of device. |
 * |\b OOB                        | Out of Band. One association mode, primarily designed for scenarios where an Out of Band mechanism is used to both discover the devices as well as to exchange or transfer cryptographic numbers used in the pairing process. |
 * |\b RSSI                       | Received Signal Strength Indication, received signal power. |
 * |\b SCO                        | Synchronous logical link. |
 * |\b ACL                        | Asynchronous Connection-oriented [logical transport]. |
 * |\b LMP                        | Link Manager Protocol. |
 * |\b PDU                        | Protocol Data Unit. |
 * |\b ISDN                       | Integrated Services Digital Network. |
 * |\b EIR                        | Extended Inquiry Reponse data. |
 * |\b FHS                        | Frequency Hop Synchronization. |
 * |\b HCI                        | Host Controller Interface. |
 * |\b MITM                       | Man-in-the-middle. |
 *
 * @section bt_gap_api_usage How to use this module
 *
 * - Step1: Mandatory, implement the API #bt_gap_common_callback() to handle the events sent by GAP.
 *  - Sample code:
 *     @code
 *        void bt_gap_common_callback(ble_event_t event_id, void *param)
 *        {
 *            bt_address_t addr;
 *            CONVERT_BDADDR2ARRAY((U8 *)addr.address, 0x424B64, 0x71, 0x6805);
 *            switch (event_id) {
 *
 *                case BT_GAP_IO_CAPABILITY_REQUEST_IND:{
 *                    bt_gap_reply_io_capability_request((bt_gap_io_capability_request_ind_t *)param, BT_GAP_IO_NO_INPUT_NO_OUTPUT, true, BT_GAP_BONDING_MODE_DEDICATED, false, NULL, NULL);
 *                }
 *                case BT_GAP_USER_CONFIRM_REQUEST_IND: {
 *                    bt_gap_reply_user_confirm_request((bt_gap_user_confirm_request_ind_t *)param, true);
 *                }
 *                case BT_GAP_DISCOVERY_CNF:
 *                case BT_GAP_CANCEL_INQUIRY_CNF: {
 *                    bt_gap_bond(&addr);
 *                }
 *                default:
 *                    break;
 *            }
 *        }
 *     @endcode
 * - Step2: Mandatory, implement the API #bt_gap_inquiry() to discover other nearby devices.
 *  - Sample code:
 *     @code
 *        void main(void)
 *        {
 *            bt_gap_set_scan_mode(BT_GAP_MODE_GENERAL_ACCESSIBLE);
 *            bt_gap_inquiry(10, 0x10);
 *        }
 *     @endcode
 * - Step3: Mandatory, implement the API #bt_gap_disconnect() to disconnect from the device.
 *  - Sample code:
 *     @code
 *        void main(void)
 *        {
 *            bt_gap_disconnect(&addr, BT_GAP_DISCONNECTION_REASON_REMOTE_USER_TERMINATED);
 *        }
 *     @endcode
 */


#include <stdint.h>
#include <stdbool.h>
#include "bt_address.h"
#include "bt_status.h"
#include "bt_events.h"


/** @defgroup bt_gap_define Define
  * @{
  */

/**
 *  @brief The maximum length of the Bluetooth device's name.
 */
#define BT_GAP_MAX_NAME_LEN 80

/**
 *  @brief The length of Link Key.
 */
#define BT_GAP_LINK_KEY_LEN 16

/**
  * @}
  */


/** @defgroup bt_gap_enum Enum
  * @{
  */

/**
 *  @brief Device roles in a piconet.
 */
typedef enum {
    BT_GAP_ROLE_MASTER,               /**< The Bluetooth Clock and Bluetooth Device Address of this device in a piconet define the physical channel characteristics of the piconet. */
    BT_GAP_ROLE_SLAVE,                /**< Any device in a piconet that is not the Piconet Master, but is connected to the Piconet Master. */
    BT_GAP_ROLE_UNKNOWN               /**< Not Master or Slave role. */
} bt_gap_role_t;

/**
 *  @brief Connection link types.
 */
typedef enum {
    BT_GAP_LINK_TYPE_SCO,             /**< SCO connection. */
    BT_GAP_LINK_TYPE_ACL,             /**< ACL connection. */
    BT_GAP_LINK_TYPE_UNKNOWN          /**< Not an SCO or ACL connection. */
} bt_gap_link_type_t;

/**
 *  @brief Encryption status for the connection.
 */
typedef enum {
    BT_GAP_ENCRYPTION_STATUS_DISABLED,       /**< Disable encryption for the connection. */
    BT_GAP_ENCRYPTION_STATUS_ENABLED,        /**< Enable encryption for the connection. */
    BT_GAP_ENCRYPTION_STATUS_UNKNOWN         /**< Unknown encryption status. */
} bt_gap_encryption_status_t;

/**
 *  @brief Scan modes.
 */
typedef enum {
    BT_GAP_MODE_NOT_ACCESSIBLE = 0x00,         /**< Impossible to discover or connect. */
    BT_GAP_MODE_DISCOVERABLE_ONLY = 0x01,      /**< Discoverable but not connectable. */
    BT_GAP_MODE_CONNECTABLE_ONLY = 0x02,       /**< Connectable but not discoverable. */
    BT_GAP_MODE_GENERAL_ACCESSIBLE = 0x03,     /**< Discoverable and connectable. */
} bt_gap_mode_t;

/**
 *  @brief Input and output capabilities.
 */
typedef enum {
    BT_GAP_IO_DISPLAY_ONLY,              /**< The device has the ability to display or communicate a 6 digit decimal number, but does not have the ability to indicate 'yes' or 'no'. */
    BT_GAP_IO_DISPLAY_YES_NO,            /**< The device has the ability to display or communicate a 6 digit decimal number.
                                              Also, the device has at least two buttons that are mapped easily to 'yes' and 'no' or the device has a mechanism whereby the user can indicate either 'yes' or 'no'. */
    BT_GAP_IO_KEYBOARD_ONLY,             /**< The device has a numeric keyboard that can input the numbers '0' through '9' and a confirmation.
                                              But, the device does not have the ability to display or communicate a 6 digit decimal number. */
    BT_GAP_IO_NO_INPUT_NO_OUTPUT         /**< The device does not have the ability to indicate 'yes' or 'no', and does not have the ability to display or communicate a 6 digit decimal number. */
} bt_gap_io_capability_t;

/**
 *  @brief Bonding modes.
 */
typedef enum {
    BT_GAP_BONDING_MODE_NO,              /**< Used when the device is performing a Secure Simple Pairing procedure, but does not intend to retain the link key after the physical link is disconnected. */
    BT_GAP_BONDING_MODE_DEDICATED,       /**< A process where one device connects to another only to perform pairing without accessing a particular service. */
    BT_GAP_BONDING_MODE_GENERAL          /**< A process of performing bonding during connection setup or channel establishment procedures as a precursor to accessing a service. */
} bt_gap_bonding_mode_t;

/**
 *  @brief Device types.
 */
typedef enum {
    BT_GAP_DEVICE_TYPE_UNKNOWN,                   /**< Neither Bluetooth+EDR nor Low Energy type of the device. */
    BT_GAP_DEVICE_TYPE_LE,                        /**< Devices that support the Low Energy Core Configuration. */
    BT_GAP_DEVICE_TYPE_BR_EDR,                    /**< Devices that support the Basic Rate Core Configuration. */
    BT_GAP_DEVICE_TYPE_BR_EDR_LE                  /**< Devices that support the Basic Rate and Low Energy Combined Core Configuration. */
} bt_gap_device_type_t;

/**
 *  @brief Class of Device/Service.
 */
typedef enum {
    /**< Bluetooth GAP major service class. */
    BT_GAP_COD_LIMITED_DISCOVERABLE_MODE = 0x00002000,    /**< Limited discoverable mode. */
    BT_GAP_COD_POSITIONING = 0x00010000,                  /**< Positioning. */
    BT_GAP_COD_NETWORKING = 0x00020000,                   /**< Networking. */
    BT_GAP_COD_RENDERING = 0x00040000,                    /**< Rendering. */
    BT_GAP_COD_CAPTURING = 0x00080000,                    /**< Capturing. */
    BT_GAP_COD_OBJECT_TRANSFER = 0x00100000,              /**< Object transfer. */
    BT_GAP_COD_AUDIO = 0x00200000,                        /**< Audio. */
    BT_GAP_COD_TELEPHONY = 0x00400000,                    /**< Telephony. */
    BT_GAP_COD_INFORMATION = 0x00800000,                  /**< Information. */

    /**< Bluetooth GAP major device class. */
    BT_GAP_COD_MAJOR_MISCELLANEOUS = 0x00000000,          /**< Miscellaneous. */
    BT_GAP_COD_MAJOR_COMPUTER = 0x00000100,               /**< Computer. */
    BT_GAP_COD_MAJOR_PHONE = 0x00000200,                  /**< Phone. */
    BT_GAP_COD_MAJOR_LAN_ACCESS_POINT = 0x00000300,       /**< LAN access point. */
    BT_GAP_COD_MAJOR_AUDIO = 0x00000400,                  /**< Audio. */
    BT_GAP_COD_MAJOR_PERIPHERAL = 0x00000500,             /**< Peripheral. */
    BT_GAP_COD_MAJOR_IMAGING = 0x00000600,                /**< Imaging. */
    BT_GAP_COD_MAJOR_UNCLASSIFIED = 0x00001F00,           /**< Unclassified. */

    /**< Bluetooth GAP minor device class of the computer major device class. */
    BT_GAP_COD_MINOR_COMP_UNCLASSIFIED = 0x00000000,      /**< Unclassified. */
    BT_GAP_COD_MINOR_COMP_DESKTOP = 0x00000004,           /**< Desktop. */
    BT_GAP_COD_MINOR_COMP_SERVER = 0x00000008,            /**< Server. */
    BT_GAP_COD_MINOR_COMP_LAPTOP = 0x0000000C,            /**< Laptop. */
    BT_GAP_COD_MINOR_COMP_HANDHELD = 0x00000010,          /**< Handheld. */
    BT_GAP_COD_MINOR_COMP_PALM = 0x00000014,              /**< Palm. */
    BT_GAP_COD_MINOR_COMP_WEARABLE = 0x00000018,          /**< Wearable. */

    /**< Bluetooth GAP minor device class of the phone major device Class. */
    BT_GAP_COD_MINOR_PHONE_UNCLASSIFIED = 0x00000000,     /**< Unclassified. */
    BT_GAP_COD_MINOR_PHONE_CELLULAR = 0x00000004,         /**< Cellular. */
    BT_GAP_COD_MINOR_PHONE_CORDLESS = 0x00000008,         /**< Cordless. */
    BT_GAP_COD_MINOR_PHONE_SMART = 0x0000000C,            /**< Smart. */
    BT_GAP_COD_MINOR_PHONE_MODEM = 0x00000010,            /**< Modem. */
    BT_GAP_COD_MINOR_PHONE_ISDN = 0x00000014,             /**< ISDN. */

    /**< Bluetooth GAP minor device class of the LAN access point major device class. */
    BT_GAP_COD_MINOR_LAN_0 = 0x00000000,                  /**< Fully available. */
    BT_GAP_COD_MINOR_LAN_17 = 0x00000020,                 /**< 1-17% utilized. */
    BT_GAP_COD_MINOR_LAN_33 = 0x00000040,                 /**< 17-33% utilized. */
    BT_GAP_COD_MINOR_LAN_50 = 0x00000060,                 /**< 33-50% utilized. */
    BT_GAP_COD_MINOR_LAN_67 = 0x00000080,                 /**< 50-67% utilized. */
    BT_GAP_COD_MINOR_LAN_83 = 0x000000A0,                 /**< 67-83% utilized. */
    BT_GAP_COD_MINOR_LAN_99 = 0x000000C0,                 /**< 83-99% utilized. */
    BT_GAP_COD_MINOR_LAN_NO_SERVICE = 0x000000E0,         /**< 100% utilized. */

    /**< Bluetooth GAP minor device class of the audio major device class. */
    BT_GAP_COD_MINOR_AUDIO_UNCLASSIFIED = 0x00000000,     /**< Unclassified. */
    BT_GAP_COD_MINOR_AUDIO_HEADSET = 0x00000004,          /**< Headset. */
    BT_GAP_COD_MINOR_AUDIO_HANDSFREE = 0x00000008,        /**< Handsfree. */
    BT_GAP_COD_MINOR_AUDIO_MICROPHONE = 0x00000010,       /**< Microphone. */
    BT_GAP_COD_MINOR_AUDIO_LOUDSPEAKER = 0x00000014,      /**< Loudspeaker. */
    BT_GAP_COD_MINOR_AUDIO_HEADPHONES = 0x00000018,       /**< Headphones. */
    BT_GAP_COD_MINOR_AUDIO_PORTABLE = 0x0000001C,         /**< Portable. */
    BT_GAP_COD_MINOR_AUDIO_CAR = 0x00000020,              /**< Car. */
    BT_GAP_COD_MINOR_AUDIO_SET_TOP_BOX = 0x00000024,      /**< Set top box. */
    BT_GAP_COD_MINOR_AUDIO_HIGH_FIDELITY = 0x00000028,    /**< High fidelity. */
    BT_GAP_COD_MINOR_AUDIO_VCR = 0x0000002C,              /**< VCR. */
    BT_GAP_COD_MINOR_AUDIO_VIDEO_CAMERA = 0x00000030,     /**< Video Camera. */
    BT_GAP_COD_MINOR_AUDIO_CAMCORDER = 0x00000034,        /**< Camcorder. */
    BT_GAP_COD_MINOR_AUDIO_VIDEO_MONITOR = 0x00000038,    /**< Video monitor. */
    BT_GAP_COD_MINOR_AUDIO_VIDEO_SPEAKER = 0x0000003C,    /**< Video speaker. */
    BT_GAP_COD_MINOR_AUDIO_CONFERENCING = 0x00000040,     /**< Conferencing. */
    BT_GAP_COD_MINOR_AUDIO_GAMING = 0x00000048,           /**< Gaming. */

    /**< Bluetooth GAP minor device class of the peripheral major device class. */
    BT_GAP_COD_MINOR_PERIPH_UNCLASSIFIED = 0x00000000,    /**< Unclassified. */
    BT_GAP_COD_MINOR_PERIPH_JOYSTICK = 0x00000004,        /**< Joystick. */
    BT_GAP_COD_MINOR_PERIPH_GAMEPAD = 0x00000008,         /**< Camepad. */
    BT_GAP_COD_MINOR_PERIPH_REMOTE_CONTROL = 0x0000000C,  /**< Remote control. */
    BT_GAP_COD_MINOR_PERIPH_SENSING = 0x00000010,         /**< Sensing. */
    BT_GAP_COD_MINOR_PERIPH_DIGITIZER = 0x00000014,       /**< Digitizer. */
    BT_GAP_COD_MINOR_PERIPH_CARD_READER = 0x00000018,     /**< Card reader. */
    BT_GAP_COD_MINOR_PERIPH_KEYBOARD = 0x00000040,        /**< Keyboard. */
    BT_GAP_COD_MINOR_PERIPH_POINTING = 0x00000080,        /**< Pointing. */
    BT_GAP_COD_MINOR_PERIPH_COMBO_KEY = 0x000000C0,       /**< Combo key. */

    /**< Bluetooth GAP minor device class of the imaging major device class. */
    BT_GAP_COD_MINOR_IMAGE_UNCLASSIFIED = 0x00000000,     /**< Unclassified. */
    BT_GAP_COD_MINOR_IMAGE_DISPLAY = 0x00000010,          /**< Display. */
    BT_GAP_COD_MINOR_IMAGE_CAMERA = 0x00000020,           /**< Camera. */
    BT_GAP_COD_MINOR_IMAGE_SCANNER = 0x00000040,          /**< Scanner. */
    BT_GAP_COD_MINOR_IMAGE_PRINTER = 0x00000080,          /**< Printer. */

    BT_GAP_COD_SERVICE_MASK = 0x00ffC000,                 /**< Mask used to isolate the major service class of the class of device/service. */
    BT_GAP_COD_MAJOR_MASK = 0x00001F00,                   /**< Mask used to isolate the major device class of the class of device/service. */
    BT_GAP_COD_MINOR_MASK = 0x000000FC,                   /**< Mask used to isolate the minor device class of the major device class. */
    BT_GAP_COD_LIMITED_DISC_MASK = 0x00002000,            /**< Mask used to isolate the limited discoverable mode major service class. */
} bt_gap_cod_t;

/**
 *  @brief Reason for ending the connection.
 */
typedef enum {
    BT_GAP_DISCONNECTION_REASON_AUTHENTICATION_FAILURE = 0x05,                /**< Authentication failure. */
    BT_GAP_DISCONNECTION_REASON_REMOTE_USER_TERMINATED = 0x13,                /**< The user on the remote device terminated the connection. */
    BT_GAP_DISCONNECTION_REASON_REMOTE_DEVICE_LOW_RESOURCE = 0x14,            /**< The remote device terminated the connection because of low resource availability. */
    BT_GAP_DISCONNECTION_REASON_REMOTE_DEVICE_POWER_OFF = 0x15,               /**< The remote device terminated the connection because the device is about to power off. */
    BT_GAP_DISCONNECTION_REASON_UNSUPPORTED_REMOTE_FEATURE = 0x1A,            /**< The remote device does not support a feature associated with the issued command or LMP PDU. */
    BT_GAP_DISCONNECTION_REASON_PAIRING_WITH_UNIT_KEY_NOT_SUPPORTED = 0x29,   /**< Impossible to establish pairing as the requested unit key is not supported. */
    BT_GAP_DISCONNECTION_REASON_UNACCEPTABLE_CONNECTION_PARAM = 0x3B,         /**< The remote device either terminated the connection or rejected a request because of one or more unacceptable connection parameters. */
} bt_gap_disconnection_reason_t;

/**
  * @}
  */


/** @defgroup bt_gap_typedef Typedef
  * @{
  */

/**
 *  @brief This type defines a parameter of the #BT_GAP_USER_CONFIRM_REQUEST_IND event.
 */
typedef bt_address_t bt_gap_user_confirm_request_ind_t;
/**
 *  @brief This type defines a parameter of the #BT_GAP_IO_CAPABILITY_REQUEST_IND event.
 */
typedef bt_address_t bt_gap_io_capability_request_ind_t;
/**
 *  @brief This type defines a parameter of the #BT_GAP_PIN_CODE_REQUEST_IND event.
 */
typedef bt_address_t bt_gap_pin_code_request_ind_t;
/**
 *  @brief This type defines a parameter of the #BT_GAP_SEND_KEYPRESS_COMPLETE_IND event.
 */
typedef bt_address_t bt_gap_send_keypress_complete_ind_t;
/**
 *  @brief This type defines a parameter of the #BT_GAP_USER_PASSKEY_REQUEST_IND event.
 */
typedef bt_address_t bt_gap_user_passkey_request_ind_t;
/**
 *  @brief This type defines a parameter of the #BT_GAP_DELETE_LINK_KEY_IND event.
 */
typedef bt_address_t bt_gap_delete_link_key_ind_t;
/**
 *  @brief This type defines a parameter of the #BT_GAP_DISCOVERY_CNF event.
 */
typedef uint8_t bt_gap_discovery_cnf_t;
/**
 *  @brief This type defines a parameter of the #BT_GAP_CANCEL_INQUIRY_CNF event.
 */
typedef uint8_t bt_gap_cancel_inquiry_cnf_t;
/**
 *  @brief This type defines a parameter of the #BT_GAP_WRITE_INQUIRY_TX_CNF event.
 */
typedef uint8_t bt_gap_write_inquiry_tx_cnf_t;
/**
 *  @brief This type defines a parameter of the #BT_GAP_SET_SCAN_MODE_CNF event.
 */
typedef uint8_t bt_gap_set_scan_mode_cnf_t;
/**
 *  @brief This type defines a parameter of the #BT_GAP_POWER_ON_CNF event.
 */
typedef uint8_t bt_gap_power_on_cnf_t;
/**
 *  @brief This type defines a parameter of the #BT_GAP_POWER_OFF_CNF event.
 */
typedef uint8_t bt_gap_power_off_cnf_t;
/**
 *  @brief This type defines a parameter of the #BT_GAP_SET_SNIFF_SUBRATE_POLICY_CNF event.
 */
typedef uint8_t bt_gap_set_sniff_subrate_policy_cnf_t;
/**
 *  @brief This type defines a parameter of the #BT_GAP_ENTER_TEST_MODE_CNF event.
 */
typedef uint8_t bt_gap_enter_test_mode_cnf_t;
/**
 *  @brief This type defines a parameter of the #BT_GAP_PIN_CODE_RESPONSE_CNF event.
 */
typedef uint8_t bt_gap_pin_code_response_cnf_t;
/**
 *  @brief This type defines a parameter of the #BT_GAP_IO_CAPABILITY_RESPONSE_CNF event.
 */
typedef uint8_t bt_gap_io_capability_response_cnf_t;
/**
 *  @brief This type defines a parameter of the #BT_GAP_USER_CONFIRM_RESPONSE_CNF event.
 */
typedef uint8_t bt_gap_user_confirm_response_cnf_t;

/**
  * @}
  */


/** @defgroup bt_gap_struct Struct
  * @{
  */

/**
 *  @brief Sniff information.
 */
typedef struct {
    uint16_t max_interval;          /**< The maximum acceptable interval between each consecutive sniff periods. */
    uint16_t min_interval;          /**< The minimum acceptable interval between each consecutive sniff periods. */
    uint16_t attempt;               /**< The number of master-to-slave transmission slots during which a device should listen for traffic. */
    uint16_t timeout;               /**< The amount of time before a sniff timeout occurs. */
} bt_gap_sniff_info_t;

/**
 *  @brief Page scan information.
 */
typedef struct {
    uint8_t repeat_mode;            /**< Page scan repetition mode supported by the remote basic rate (BR)/EDR Controller. */
    uint8_t mode;                   /**< Page scan mode used by the remote device to establish a link. */
    uint16_t clock_offset;          /**< The difference between the local basic rate (BR)/EDR Controller's own clock and the clock of the remote basic rate (BR)/EDR Controller. */
#ifdef __A2DP_MULTI_LINK_FEATURE__
    uint32_t clock_offset_ext;
#endif
} bt_gap_page_scan_info_t;

/**
 *  @brief This event structure provides parameters of the #BT_GAP_BONDING_CNF event.
 */
typedef struct {
    bt_status_t result;          /**< The status indicates if the bonding completed successfully or not. */
    bt_address_t dev_addr;       /**< Bluetooth address of the remote device. */
} bt_gap_bonding_cnf_t;

/**
 *  @brief This event structure provides parameters of the #BT_GAP_AUTHENTICATE_RESULT_IND event.
 */
typedef struct {
    bt_status_t result;          /**< The status indicates if the authentication completed successfully or not. */
    bt_address_t dev_addr;       /**< Bluetooth address of the remote device. */
} bt_gap_authenticate_result_ind_t;

/**
 *  @brief This event structure provides parameters of the #BT_GAP_USER_PASSKEY_NOTIFICATION_IND event.
 */
typedef struct {
    bt_address_t dev_addr;      /**< Bluetooth address of the device involved in simple pairing process. */
    uint32_t passkey;           /**< Passkey to be displayed. Valid values are decimal 000000 - 999999. */
} bt_gap_user_passkey_notification_ind_t;

/**
 *  @brief This event structure provides parameters of the #BT_GAP_KEYPRESS_NOTIFICATION_IND event.
 */
typedef struct {
    bt_address_t dev_addr;      /**< Bluetooth address of the remote device involved in simple pairing process. */
    uint32_t notification_type; /**< Keypress notification type. */
} bt_gap_keypress_notification_ind_t;

/**
 *  @brief This event structure provides parameters of the #BT_GAP_BONDING_RESULT_IND event.
 */
typedef struct {
    bt_status_t result;                       /**< The status indicates if the bonding completed successfully or not. */
    bt_address_t dev_addr;                    /**< Bluetooth address of the remote device. */
    uint8_t link_key[BT_GAP_LINK_KEY_LEN];    /**< Link key. */
    uint8_t key_type;                         /**< Link key type. */
} bt_gap_bond_result_ind_t;

/**
 *  @brief Connection information.
 */
typedef struct {
    uint16_t conn_id;                                  /**< Connection identifier. */
    bt_address_t dev_addr;                             /**< Bluetooth address of the connected device. */
    bt_gap_link_type_t link_type;                      /**< Connection link type, please refer to #bt_gap_link_type_t. */
    bt_gap_encryption_status_t encryption_status;      /**< Encryption status for the connection, please refer to #bt_gap_encryption_status_t. */
    bt_gap_role_t role;                                /**< The role of the connected device, please refer to #bt_gap_role_t. */
} bt_gap_connection_info_t;

/**
 *  @brief This event structure provides parameters of the #BT_GAP_DISCOVERY_RESULT_IND event.
 */
typedef struct {
    bt_address_t dev_addr;          /**< Bluetooth address of the responding device. */
    bt_gap_page_scan_info_t psi;    /**< Page scan information, please refer to #bt_gap_page_scan_info_t. */
    uint8_t ps_period_mode;         /**< Page scan period mode. */
    uint32_t cod;                   /**< COD of the device. */
    int8_t rssi;                    /**< RSSI. */
    uint8_t supported_eir;          /**< The flag indicates whether the responding device supports EIR. */
    uint8_t *eir_response;          /**< EIR data. */
} bt_gap_discovery_result_ind_t;

/**
 *  @brief This event structure provides parameters of the #BT_GAP_READ_RSSI_CNF event.
 */
typedef struct {
    bt_status_t result;            /**< The status indicates if the read RSSI completed successfully or not. */
    int8_t rssi_value;             /**< RSSI value. */
    bt_address_t dev_addr;         /**< Bluetooth address of the remote device. */
} bt_gap_read_rssi_cnf_t;

/**
 *  @brief This event structure provides parameters of the #BT_GAP_READ_REMOTE_NAME_CNF event.
 */
typedef struct {
    bt_status_t result;                   /**< The status indicates if the remote name request completed successfully or not. */
    bt_address_t dev_addr;                /**< Bluetooth address of the remote device. */
    uint8_t name_len;                     /**< Name length of the remote device. */
    uint8_t name[BT_GAP_MAX_NAME_LEN];    /**< Name of the remote device. */
} bt_gap_read_remote_name_cnf_t;

/**
 *  @brief This event structure provides parameters of the #BT_GAP_READ_INQUIRY_RESPONSE_TX_CNF event.
 */
typedef struct {
    bt_status_t result;                   /**< The status indicates if the read inquiry response transmit power level completed successfully or not. */
    int8_t tx_pwr;                        /**< Power level used to transmit the FHS and EIR data packets in the inquiry response procedure. */
} bt_gap_read_inquiry_response_tx_cnf_t;

/**
 *  @brief Connection link result.
 */
typedef struct {
    bt_status_t result;                   /**< Connection link result. */
    uint8_t active_link_number;           /**< The number of active links. */
    bt_address_t dev_addr;                /**< Bluetooth address of the connected device. */
    uint16_t conn_id;                     /**< Connection identifier. */
    bt_gap_device_type_t dev_type;        /**< Device type of the connected device, please refer to #bt_gap_device_type_t. */
} bt_gap_link_connection_result_t;

/**
 *  @brief This event structure provides parameters of the #BT_GAP_SET_ROLE_CNF event.
 */
typedef struct {
    bt_status_t result;                  /**< The status indicates if the set role completed successfully or not. */
    bt_address_t dev_addr;               /**< Bluetooth address of the device with an assigned role. */
    bt_gap_role_t role;                  /**< Newly switched role. */
} bt_gap_set_role_cnf_t;

/**
 *  @brief This event structure provides parameters of the #BT_GAP_DETACH_SINGLE_LINK_CNF event.
 */
typedef bt_gap_link_connection_result_t bt_gap_detach_single_link_cnf_t;

/**
  * @}
  */


#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief                 This function switches roles of master and slave devices.
 * @param[in] remote_addr is the Bluetooth address of a remote device.
 * @param[in] role        is the newly switched role of a remote device.
 * @return                If the operation is pending, the returned value is #BT_STATUS_GAP_PENDING;
 *                        if the remote device is not found, the returned value is #BT_STATUS_GAP_NOT_FOUND;
 *                        if HCI initialization error occurred, the returned value is #BT_STATUS_GAP_HCI_INIT_ERROR;
 *                        if the link is performing an operation, the returned value is #BT_STATUS_GAP_IN_PROGRESS,
 *                        otherwise the returned value is #BT_STATUS_GAP_FAILED.
 */
bt_status_t bt_gap_set_role(const bt_address_t *remote_addr, bt_gap_role_t role);

/**
 * @brief                 This function gets the role of a remote device.
 * @param[in] remote_addr is the Bluetooth address of a remote device.
 * @return                If the operation completes successfully, the returned value is the role of the remote device: #BT_GAP_ROLE_MASTER or #BT_GAP_ROLE_SLAVE;
 *                        if the remote device is not found, the returned value is #BT_GAP_ROLE_UNKNOWN.
 */
bt_gap_role_t bt_gap_get_role(const bt_address_t *remote_addr);

/**
 * @brief                  This function causes the BR/EDR controller to enter the inquiry mode. It is used to discover other nearby BR/EDR devices.
 * @param[in] max_response is the maximum number of responses from the inquiry before the inquiry halted. Range is from 0x01 to 0xFF.
 * @param[in] timeout      is the maximum amount of time specified before the inquiry halted. Range is from 0x01 to 0x30, Time = timeout * 1.28 sec.
 * @return                 If the operation is pending, the returned value is #BT_STATUS_GAP_PENDING;
 *                         if HCI initialization error occurred, the returned value is #BT_STATUS_GAP_HCI_INIT_ERROR;
 *                         if the inquiry is already in progress, the returned value is #BT_STATUS_GAP_IN_PROGRESS.
 */
bt_status_t bt_gap_inquiry(uint8_t max_response, uint8_t timeout);

/**
 * @brief                  This function causes the BR/EDR controller to stop the current inquiry.
 * @return                 If the inquiry operation completes successfully, the returned value is #BT_STATUS_SUCCESS;
 *                         if the inquiry operation is in progress, the returned value is #BT_STATUS_GAP_PENDING;
 *                         if a cancel inquiry operation is in progress, the returned value is #BT_STATUS_GAP_IN_PROGRESS;
 *                         if there is no inquiry operation, the returned value is #BT_STATUS_GAP_FAILED.
 */
bt_status_t bt_gap_cancel_inquiry(void);

/**
 * @brief                 This function obtains the user-friendly name of a remote device.
 * @param[in] remote_addr is the Bluetooth address of a remote device.
 * @return                If the operation is pending, the returned value is #BT_STATUS_GAP_PENDING, otherwise the returned value is #BT_STATUS_GAP_FAILED.
 */
bt_status_t bt_gap_read_remote_name(const bt_address_t *remote_addr);

/**
 * @brief              This function writes the inquiry transmit power level used to transmit the inquiry data packets during the inquiry.
 * @param[in] tx_power is the inquiry transmit power level.
 * @return             If the operation is pending, the returned value is #BT_STATUS_GAP_PENDING, otherwise the returned value is #BT_STATUS_GAP_FAILED.
 */
bt_status_t bt_gap_write_inquiry_tx(int8_t tx_power);

/**
 * @brief              This function reads the power level used to transmit the FHS and EIR data packets during the inquiry response.
 * @return             If the operation is pending, the returned value is #BT_STATUS_GAP_PENDING, otherwise the returned value is #BT_STATUS_GAP_FAILED.
 */
bt_status_t bt_gap_read_inquiry_response_tx(void);

/**
 * @brief          This function sets the scan mode. Call this API after the #BT_GAP_POWER_ON_CNF event is received.
 * @param[in] mode is the scan mode to be set.
 * @return         If the mode is the same as the current mode, the returned value is #BT_STATUS_SUCCESS;
 *                 if HCI initialization error occurred, the returned value is #BT_STATUS_GAP_HCI_INIT_ERROR;
 *                 if the operation is pending, the returned value is #BT_STATUS_GAP_PENDING.
 */
bt_status_t bt_gap_set_scan_mode(bt_gap_mode_t mode);

/**
 * @brief                 This function reads the RSSI value of a remote device.
 * @param[in] remote_addr is the Bluetooth address of a remote device.
 * @return                If the operation is pending, the returned value is #BT_STATUS_GAP_PENDING, otherwise the returned value is #BT_STATUS_GAP_FAILED.
 */
bt_status_t bt_gap_read_rssi(const bt_address_t *remote_addr);

/**
 * @brief          This function enables the local BR/EDR controller to enter test mode.
 * @return         If the operation is pending, the returned value is #BT_STATUS_GAP_PENDING, otherwise the returned value is #BT_STATUS_GAP_FAILED.
 */
bt_status_t bt_gap_enter_test_mode(void);

/**
 * @brief                        This function specifies the parameters for sniff sub-rating for a given link.
 * @param[in] remote_addr        is the Bluetooth address of a remote device.
 * @param[in] max_remote_latency is used to calculate the maximum_sniff subrate that the remote device may use.
 * @param[in] min_remote_timeout is the minimum base sniff subrate timeout that the remote device may use.
 * @param[in] min_local_timeout  is the minimum base sniff subrate timeout that the local device may use.
 * @return                       If the operation is pending, the returned value is #BT_STATUS_GAP_PENDING, otherwise the returned value is #BT_STATUS_GAP_FAILED.
 */
bt_status_t bt_gap_set_sniff_subrate_policy(const bt_address_t *remote_addr, uint16_t max_remote_latency, uint16_t min_remote_timeout, uint16_t min_local_timeout);

/**
 * @brief                 This function replies to a PIN code request event from the BR/EDR controller, and specifies the PIN code to use for a connection.
 * @param[in] remote_addr is the Bluetooth address of the remote device which the PIN code is for.
 * @param[in] pin_code    is the PIN code for the device that is to be connected.
 * @param[in] pin_len     is the length, in octets, of the PIN code to be used. The range is from 0x01 to 0x10.
 * @return                If the operation is pending, the returned value is #BT_STATUS_GAP_PENDING, otherwise the returned value is #BT_STATUS_GAP_FAILED.
 */
bt_status_t bt_gap_reply_pin_code_request(const bt_address_t *remote_addr, const uint8_t *pin_code, uint8_t pin_len);

/**
 * @brief                 This function terminates an existing connection.
 * @param[in] remote_addr is the Bluetooth address of the remote device to be disconnected.
 * @param[in] reason      is the reason to end the connection.
 * @return                If the operation is pending, the returned value is #BT_STATUS_GAP_PENDING, otherwise the returned value is #BT_STATUS_GAP_FAILED.
 */
bt_status_t bt_gap_disconnect(const bt_address_t *remote_addr, bt_gap_disconnection_reason_t reason);

/**
 * @brief                      This function replies to an I/O capability request event from the controller, and specifies the current I/O capability of the host.
 * @param[in] remote_addr      is the Bluetooth address of the remote device involved in simple pairing process.
 * @param[in] io_cap           is the current I/O capability of the host.
 * @param[in] force_mitm       is whether the MITM protection is required.
 * @param[in] mode             is bonding mode.
 * @param[in] oob_data_present is whether OOB is used.
 * @param[in] oob_hash_c       is simple pairing hash C.
 * @param[in] oob_rand_r       is simple pairing randomizer R.
 * @return                     If the operation is pending, the returned value is #BT_STATUS_GAP_PENDING; if cann't get a security operation token, the returned value is BT_STATUS_NO_RESOURCES.
 */
bt_status_t bt_gap_reply_io_capability_request(
    const bt_address_t *remote_addr,
    bt_gap_io_capability_t io_cap,
    bool force_mitm,
    bt_gap_bonding_mode_t mode,
    bool oob_data_present,
    const uint8_t *oob_hash_c,
    const uint8_t *oob_rand_r);

/**
 * @brief                 This function replies to a user confirmation request event.
 * @param[in] remote_addr is the Bluetooth address of a remote device.
 * @param[in] confirm     is the user's confirmation, either yes or no.
 * @return                If the operation is pending, the returned value is #BT_STATUS_GAP_PENDING; if cann't get a security operation token, the returned value is BT_STATUS_NO_RESOURCES.
 */
bt_status_t bt_gap_reply_user_confirm_request(const bt_address_t *remote_addr, bool confirm);

/**
 * @brief                   This function replies to a user passkey request event.
 * @param[in] remote_addr   is the Bluetooth address of a remote device.
 * @param[in] numeric_value is the numeric value entered by the user.
 * @return                  If the operation completes successfully, the returned value is #BT_STATUS_SUCCESS, otherwise the returned value is #BT_STATUS_GAP_FAILED.
 */
bt_status_t bt_gap_reply_user_passkey_request(const bt_address_t *remote_addr, uint32_t numeric_value);

/**
 * @brief          This function powers on the Bluetooth and initializes the profiles.
 * @return         If the operation completes successfully, the returned value is #BT_STATUS_SUCCESS;
 *                 if the operation is pending, the returned value is #BT_STATUS_GAP_PENDING;
 *                 otherwise the returned value is #BT_STATUS_GAP_FAILED.
 */
bt_status_t bt_gap_power_on(void);

/**
 * @brief          This function powers off the Bluetooth and de-initializes the profiles. The user should terminate the existing connection before powers off the Bluetooth.
 * @return         If the operation completes successfully, the returned value is #BT_STATUS_SUCCESS;
 *                 if the operation is pending, the returned value is #BT_STATUS_GAP_PENDING.
 */
bt_status_t bt_gap_power_off(void);

/**
 * @brief                 This function reads the connection ID.
 * @param[in] remote_addr is the Bluetooth address of remote device.
 * @return                If the operation completes successfully, the returned value is id of the connection, otherwise the returned value is 0.
 */
uint16_t bt_gap_get_connection_id(const bt_address_t *remote_addr);

/**
 * @brief             This function reads the connection information.
 * @param[in] conn_id is the connection ID.
 * @return            If the operation completes successfully, the returned value is the connection information, otherwise the returned value is NULL.
 */
const bt_gap_connection_info_t *bt_gap_get_connection_info(uint16_t conn_id);

/**
 * @brief          This function is a user-defined API to provide the name. The GAP will read the name and modify the user-friendly name for the BR/EDR controller.
 * @param[in] name is the user-friendly name for the device.
 * @return         If the operation completes successfully, the returned value is #BT_STATUS_SUCCESS, otherwise the returned value is #BT_STATUS_GAP_FAILED.
 */
bt_status_t bt_gap_get_local_name(char *name);

/**
 * @brief          This function is a user-defined API to provide the COD. The GAP will read the COD and modify the COD value in the BR/EDR controller.
 * @return         The returned value is the COD of the device.
 */
uint32_t bt_gap_get_local_cod(void);

/**
 * @brief                This function is a user-defined API to provide the address. The GAP will read the address and modify the Bluetooth controller address.
 * @param[in] local_addr is the address of the device.
 * @return               If the operation completes successfully, the returned value is #BT_STATUS_SUCCESS, otherwise the returned value is #BT_STATUS_GAP_FAILED.
 */
bt_status_t bt_gap_get_local_address(bt_address_t *local_addr);

/**
 * @brief                 This function connects two Bluetooth devices based on a common link key.
 * @param[in] remote_addr is the Bluetooth address of a remote device.
 * @return                If the operation is pending, the returned value is #BT_STATUS_GAP_PENDING, otherwise the returned value is #BT_STATUS_GAP_FAILED.
 */
bt_status_t bt_gap_bond(const bt_address_t *remote_addr);

/**
 * @brief              This function is a static callback for a user to listen to the event. It is a user-defined API.
 * @param[in] event_id is notification sent by the GAP.
 * @param[in] param    is parameter pointer of the events.
 * @return             None
 */
void bt_gap_common_callback(bt_event_t event_id, void *param);

/**
 * @brief                 This function is a user-defined API to provide the link key. The GAP will read the link key and use it in the pairing process.
 * @param[in] remote_addr is the Bluetooth address of a remote device.
 * @param[out] key        is link key for the address.
 * @param[in] key_len     is the length of a link key.
 * @return                If the operation completes successfully, the returned value is #BT_STATUS_SUCCESS, otherwise the returned value is #BT_STATUS_GAP_FAILED.
 */
bt_status_t bt_gap_request_link_key(const bt_address_t *remote_addr, uint8_t *key, uint32_t key_len);

/**
 * @brief                This function reads the local Bluetooth address of the device.
 * @param[in] local_addr is buffer to store the address of the device.
 * @return               If the operation completes successfully, the returned value is #BT_STATUS_SUCCESS, otherwise the returned value is #BT_STATUS_GAP_CANCELLED.
 */
bt_status_t bt_gap_read_local_address(bt_address_t *local_addr);

/**
 * @brief                This function sets the sniff requirements in the sniff mode.
 * @param[in] info       is the sniff configuration to set in the controller.
 * @return               None
 */
void bt_gap_set_sniff_info(const bt_gap_sniff_info_t *info);

#ifdef __cplusplus
}
#endif

/**
 * @}
 * @}
 */

#endif /* #ifndef __BT_GAP_H__ */
