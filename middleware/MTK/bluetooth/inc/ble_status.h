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

#ifndef __BLE_STATUS_H__
#define __BLE_STATUS_H__

/**
 * @addtogroup BLE
 * @{
 * @addtogroup CommonBLE Common
 * @{
 * @addtogroup CommonBLE_3 BLE status codes
 * @{
 * This section provides the BLE status codes.
 *
 * The BLE status codes indicate success or failure of an API procedure. In case of failure, a comprehensive status code indicating
 * the cause or reason for the failure is provided.
 *
 * Terms and Acronyms
 * ======
 * |Terms                         |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b MTU                        | The Maximum Transmission Unit (MTU) is the size of the largest protocol data unit that the communications layer can pass onwards. |
 * |\b PDU                        | Protocol Data Unit, information that is delivered as a unit among peer entities of a network and that may contain control information, such as address information, or user data. |
 */

/**
 * @defgroup ble_status_enum Enum
 * @{
 */

/** @brief This enumeration defines the BLE status codes. */
typedef enum {
    BLE_STATUS_SUCCESS = 0,                             /**< The operation completed successfully. */
    BLE_STATUS_UNKNOWN_HCI_CMD = -0x1,                  /**< The Controller does not understand the HCI Command Packet OpCode sent by the Host. */
    BLE_STATUS_UNKNOWN_IDENTIFIER = -0x2,               /**< A command was sent from the Host to identify a connection, but that connection does not exist. */
    BLE_STATUS_HARDWARE_FAILURE = -0x3,                 /**< It notifies the Host that the Controller operation has failed for an unknown reason and no error code is available. */
    BLE_STATUS_PAGE_TIMEOUT = -0x4,                     /**< A page timed out because of the Page Timeout configuration parameter. */
    BLE_STATUS_AUTHENTICATION_FAILURE = -0x5,           /**< Pairing or authentication failed due to incorrect results in the pairing or authentication procedure. */
    BLE_STATUS_PIN_OR_KEY_MISSING = -0x6,               /**< It is used when pairing failed because of a missing PIN, or authentication failed because of a missing Key. */
    BLE_STATUS_MEMORY_CAPACITY_EXCEEDED = -0x7,         /**< It notifies the Host that the Controller has run out of memory to store new parameters. */
    BLE_STATUS_CONNECTION_TIMEOUT = -0x8,               /**< The link supervision timeout has expired for a given connection. */
    BLE_STATUS_CONNECTION_LIMIT_EXCEEDED = -0x9,        /**< An attempt to create another connection failed because the Controller is already at its limit of the number of connections it can support. */
    BLE_STATUS_SYNCHRONOUS_CONNECTION_LIMIT_TO_A_DEVICE_EXCEEDED = -0xA, /**< The Controller has reached the limit of the number of synchronous connections it supports for the device. */
    BLE_STATUS_ACL_CONNECTION_ALREADY_EXISTS = -0xB,    /**< An attempt to create a new ACL Connection to a device when there is already an existing connection to this device. */
    BLE_STATUS_COMMAND_DISALLOWED = -0xC,               /**< The Controller is unable to process the requested command. */
    BLE_STATUS_REJECTED_BY_LIMITED_RESOURCES = -0xD,    /**< An incoming connection was rejected due to limited resources. */
    BLE_STATUS_REJECTED_BY_SECURITY = -0xE,             /**< A connection was rejected due to security requirements not being fulfilled, such as authentication or pairing. */
    BLE_STATUS_REJECTED_BY_UNACCEPTABLE_ADDRESS = -0xF, /**< A connection was rejected because this device does not accept the BD_ADDR. */
    BLE_STATUS_ACCEPT_TIMEOUT = -0x10,                  /**< The Connection Accept Timeout has been exceeded for this connection attempt. */
    BLE_STATUS_UNSUPPORTED_FEATURE_OR_PARAM = -0x11,    /**< A feature or parameter value in the HCI command is not supported. */
    BLE_STATUS_INVALID_HCI_CMD_PARAM = -0x12,           /**< At least one of the HCI command parameters is invalid. */
    BLE_STATUS_REMOTE_USER_TERMINATED = -0x13,          /**< The user on the remote device terminated the connection. */
    BLE_STATUS_REMOTE_DEVICE_LOW_RESOURCE = -0x14,      /**< The remote device terminated the connection because of low resources. */
    BLE_STATUS_REMOTE_DEVICE_POWER_OFF = -0x15,         /**< The remote device terminated the connection because the device is about to power off. */
    BLE_STATUS_LOCAL_HOST_TERMINATED = -0x16,           /**< The local device terminated the connection. */
    BLE_STATUS_REPEATED_ATTEMPTS = -0x17,               /**< The Controller forbids an authentication or pairing procedure because too little
                                                             time has elapsed since the last failed attempt for authentication or pairing. */
    BLE_STATUS_PAIRING_NOT_ALLOWED = -0x18,             /**< The device does not allow pairing. */
    BLE_STATUS_UNKNOWN_LMP_PDU = -0x19,                 /**< The Controller has received an unknown LMP OpCode. */
    BLE_STATUS_UNSUPPORTED_REMOTE_FEATURE_ORLMP_FEATURE = -0x1A, /**< The remote device does not support a feature associated with the issued command or LMP PDU. */
    BLE_STATUS_SCO_OFFSET_REJECTED = -0x1B,             /**< The offset requested in the LMP_SCO_link_req PDU is rejected. */
    BLE_STATUS_SCO_INTERVAL_REJECTED = -0x1C,           /**< The interval requested in the LMP_SCO_link_req PDU is rejected. */
    BLE_STATUS_SCO_AIR_MODE_REJECTED = -0x1D,           /**< The air mode requested in the LMP_SCO_link_req PDU is rejected. */
    BLE_STATUS_INVALID_LMP_OR_LL_PARAM = -0x1E,         /**< Some LMP PDU / LL Control PDU parameters are invalid. */
    BLE_STATUS_UNSPECIFIED_ERROR = -0x1F,               /**< No other error code specified is appropriate to use. */
    BLE_STATUS_UNSUPPORTED_LMP_OR_LL_PARAM_VALUE = -0x20, /**< An LMP PDU or an LL Control PDU contains at least one parameter value that is not supported by the Controller at this time. */
    BLE_STATUS_ROLE_CHANGE_NOT_ALLOWED = -0x21,         /**< A Controller will not allow a role change at this time. */
    BLE_STATUS_LMP_OR_LL_RESPONSE_TIMEOUT = -0x22,      /**< An LMP transaction failed to respond within the LMP response timeout or an LL transaction failed to respond within the LL response timeout. */
    BLE_STATUS_LMP_ERROR_TRANSACTION_COLLISION = -0x23, /**< An LMP transaction has collided with the same transaction that is already in progress. */
    BLE_STATUS_LMP_PDU_NOT_ALLOWED = -0x24,             /**< A Controller sent an LMP PDU with an OpCode that was not allowed. */
    BLE_STATUS_ENCRYPTION_MODE_NOT_ACCEPTABLE = -0x25,  /**< The requested encryption mode is not acceptable at this time. */
    BLE_STATUS_LINK_KEY_CANNOT_BE_CHANGED = -0x26,      /**< Link key cannot be changed because a fixed unit key is being used. */
    BLE_STATUS_REQUESTED_QOS_NOT_SUPPORTED = -0x27,     /**< The requested Quality of Service is not supported. */
    BLE_STATUS_INSTANT_PASSED = -0x28,                  /**< An LMP PDU or LL PDU that includes an instant cannot be performed because the instant when this would have occurred has passed. */
    BLE_STATUS_PAIRING_WITH_UNIT_KEY_NOT_SUPPORTED = -0x29, /**< Impossible to establish pairing as the requested unit key is not supported. */
    BLE_STATUS_DIFFERENT_TRANSACTION_COLLISION = -0x2A, /**< An initiated LMP transaction collided with an ongoing transaction. */
    /* 0x2B Reserved */
    BLE_STATUS_QOS_UNACCEPTABLE_PARAM = -0x2C,          /**< The specified Quality of Service parameters could not be accepted. */
    BLE_STATUS_QOS_REJECTED = -0x2D,                    /**< The specified Quality of Service parameters cannot be accepted and Quality of Service negotiation should be terminated. */
    BLE_STATUS_CHANNEL_CLASSIFICATION_NOT_SUPPORTED = -0x2E, /**< The Controller cannot perform channel assessment because it is not supported. */
    BLE_STATUS_INSUFFICIENT_SECURITY = -0x2F,           /**< Sending the HCI command or LMP PDU is only possible on an encrypted link. */
    BLE_STATUS_PARAM_OUT_OF_RANGE = -0x30,              /**< A parameter value requested is outside the mandatory range of parameters for the given HCI command or LMP PDU
                                                             and the recipient does not accept that value. */
    /* 0x31 Reserved */
    BLE_STATUS_ROLE_SWITCH_PENDING = -0x32,             /**< A Role Switch is pending. */
    /* 0x33 Reserved */
    BLE_STATUS_RESERVED_SLOT_VIOLATION = -0x34,         /**< The current Synchronous negotiation was terminated with the negotiation state set to Reserved Slot Violation. */
    BLE_STATUS_ROLE_SWITCH_FAILED = -0x35,              /**< A role switch was attempted but it failed and the original piconet structure is restored. */
    BLE_STATUS_EXTENDED_INQUIRY_RESPONSE_TOO_LARGE = -0x36, /**< The extended inquiry response with the requested requirements for FEC is too large to fit in any of the packet types
                                                             supported by the Controller. */
    BLE_STATUS_SSP_NOT_SUPPORTED_BY_HOST = -0x37,       /**< The IO capabilities request or response was rejected because the sending Host does not support Secure Simple Pairing
                                                             even if the receiving Link Manager does. */
    BLE_STATUS_HOST_PAIRING_BUSY = -0x38,               /**< The Host is busy with another pairing operation and unable to support the requested pairing. */
    BLE_STATUS_NO_SUITABLE_CHANNEL_FOUND = -0x39,       /**< The Controller could not calculate an appropriate value for the Channel selection operation. */
    BLE_STATUS_CONTROLLER_BUSY = -0x3A,                 /**< The operation was rejected because the Controller was busy and unable to process the request. */
    BLE_STATUS_UNACCEPTABLE_CONNECTION_PARAM = -0x3B,   /**< The remote device either terminated the connection or rejected a request because of one
                                                             or more unacceptable connection parameters. */
    BLE_STATUS_DIRECTED_ADVERTSING_TIMEOUT = -0x3C,     /**< Directed advertising completed without a connection being created. */
    BLE_STATUS_MIC_FAILURE = -0x3D,                     /**< The connection was terminated because the Message Integrity Check (MIC) failed on a received packet. */
    BLE_STATUS_CONNECTION_FAILED_TO_BE_ESTABLISHED = -0x3E, /**< The LL initiated a connection but the connection has failed to be established. */
    BLE_STATUS_MAC_CONNECTION_FAILED = -0x3F,           /**< The MAC of the 802.11 AMP was requested to connect to a peer, but the connection failed. */
    BLE_STATUS_COARSE_CLOCK_ADJUSTMENT_REJECTED = -0x1F, /**< The master is unable to make a coarse adjustment to the piconet clock at a moment using the supplied parameters. */

    BLE_STATUS_BASE_HCI = -0x0100,                      /**< The base of the HCI status. */

    BLE_STATUS_BASE_GAP = -0x0200,                      /**< The base of the GAP status. */
    BLE_STATUS_GAP_FAILED = -0x0201,                    /**< GAP operation failed. */
    BLE_STATUS_GAP_PENDING = -0x0202,                   /**< GAP operation successfully started but pending. */
    BLE_STATUS_GAP_IN_USE = -0x0205,                    /**< GAP operation is already in use. */
    BLE_STATUS_GAP_BUSY = -0x020B,                      /**< One GAP operation is ongoing, the new operation cannot be added. */
    BLE_STATUS_GAP_NO_RESOURCES = -0x020C,              /**< There are no resources for the GAP operation. */
    BLE_STATUS_GAP_NOT_FOUND = -0x020D,                 /**< There is no result found for the GAP operation. */
    BLE_STATUS_GAP_DEVICE_NOT_FOUND = -0x020E,          /**< Device is not found for the GAP operation. */
    BLE_STATUS_GAP_CONNECTION_FAILED = -0x020F,         /**< Failed to create connection. */
    BLE_STATUS_GAP_TIMEOUT = -0x0210,                   /**< Timeout for the GAP operation. */
    BLE_STATUS_GAP_NO_CONNECTION = -0x0211,             /**< There is no connection. */
    BLE_STATUS_GAP_INVALID_PARAM = -0x0212,             /**< The parameter is invalid. */
    BLE_STATUS_GAP_IN_PROGRESS = -0x0213,               /**< The current operation is on-going. */
    BLE_STATUS_GAP_RESTRICTED = -0x0214,                /**< The remote device already has a restricted link. */
    BLE_STATUS_GAP_INVALID_TYPE = -0x0215,              /**< The type is invalid. */
    BLE_STATUS_GAP_HCI_INIT_ERROR = -0x0216,            /**< There is HCI initialization error. */
    BLE_STATUS_GAP_NOT_SUPPORTED = -0x0217,             /**< The operation or the feature is not supported. */
    BLE_STATUS_GAP_CANCELLED = -0x0219,                 /**< The operation is cancelled. */
    BLE_STATUS_GAP_NO_SERVICES = -0x021A,               /**< There are no services. */
    BLE_STATUS_GAP_SCO_REJECTED = -0x021B,              /**< LMP_SCO_link_req PDU has been rejected. */
    BLE_STATUS_GAP_CHIP_REASON = -0x021C,               /**< Something in the Controller has failed in a manner that cannot be described with any other error code. */
    BLE_STATUS_GAP_BLOCK_LIST = -0x021D,                /**< Error code reserved. */
    BLE_STATUS_GAP_SCATTERNET_REJECTED = -0x021E,       /**< Error code reserved. */
    BLE_STATUS_GAP_REMOTE_REJECTED = -0x021F,           /**< Error code reserved. */
    BLE_STATUS_GAP_KEY_ERROR = -0x0220,                 /**< Error code reserved. */
    BLE_STATUS_GAP_CONNECTION_EXISTED = -0x0221,        /**< The connection already exists. */

    BLE_STATUS_BASE_L2CAP = -0x0300,                    /**< The base of the L2CAP status. */

    BLE_STATUS_BASE_ATT = -0x2000,                      /**< The base of the ATT status. */

    BLE_STATUS_BASE_GATT = -0x02100,                    /**< The base of the GATT status. */
    BLE_STATUS_GATT_SUCCESS = 0,                        /**< The operation completed successfully. */
    BLE_STATUS_GATT_FAILED = -0x02121,                  /**< Failed for current operation. */
    BLE_STATUS_GATT_PENDING = -0x02122,                 /**< Pending. */
    BLE_STATUS_GATT_NO_RESOURCES = -0x02123,            /**< No resource is available for a send request. */
    BLE_STATUS_GATT_NO_CONNECTION = -0x02124,           /**< No connection is established with current connection ID. */
    BLE_STATUS_GATT_INVALID_PARAM = -0x02125,           /**< An invalid parameter for this API. */
    BLE_STATUS_GATT_IN_PROGRESS = -0x02126,             /**< Exchange MTU size process is in progress. */
    BLE_STATUS_GATT_INVALID_HANDLE = -0x02101,          /**< An invalid attribute handler is provided for this server. */
    BLE_STATUS_GATT_READ_NOT_PERMITTED = -0x02102,      /**< The attribute cannot be read. */
    BLE_STATUS_GATT_WRITE_NOT_PERMITTED = -0x02103,     /**< The attribute cannot be written. */
    BLE_STATUS_GATT_INVALID_PDU = -0x02104,             /**< The attribute PDU is invalid. */
    BLE_STATUS_GATT_INSUFFICIENT_AUTHENTICATION = -0x02105, /**< The attribute requires authentication before it can be read or written. */
    BLE_STATUS_GATT_REQUEST_NOT_SUPPORTED = -0x02106,   /**< The attribute server does not support the request received from the client */
    BLE_STATUS_GATT_INVALID_OFFSET = -0x02107,          /**< Offset specified was past the end of the attribute. */
    BLE_STATUS_GATT_INSUFFICIENT_AUTHORIZATION = -0x02108, /**< The attribute requires authorization before it can be read or written. */
    BLE_STATUS_GATT_PREPARE_QUEUE_FULL = -0x02109,      /**< Too many prepare writes are queued. */
    BLE_STATUS_GATT_ATTRIBUTE_NOT_FOUND = -0x0210A,     /**< No attribute found within the given attribute handle range. */
    BLE_STATUS_GATT_ATTRIBUTE_NOT_LONG = -0x0210B,      /**< The attribute cannot be read or written using the read blob request. */
    BLE_STATUS_GATT_INSUFFICIENT_ENCRYPTION_KEY_SIZE = -0x0210C, /**< Insufficient encryption key size for the link encryption. */
    BLE_STATUS_GATT_INVALID_ATTRIBUTE_VALUE_LENGTH = -0x0210D, /**< The attribute value length is invalid for the operation. */
    BLE_STATUS_GATT_UNLIKELY_ERROR = -0x0210E,          /**< A requested attribute error occurred, and the operation couldn't be completed. */
    BLE_STATUS_GATT_INSUFFICIENT_ENCRYPTION = -0x0210F, /**< The attribute requires encryption before it can be read or written. */
    BLE_STATUS_GATT_UNSUPPORTED_GROUP_TYPE = -0x02110,  /**< The attribute type is not a supported grouping attribute as defined by a higher layer specification. */
    BLE_STATUS_GATT_INSUFFICIENT_RESOURCES = -0x02111,  /**< Insufficient resources to complete the request. */
    BLE_STATUS_GATT_CONNECTION_DISCONNECTED = -0x02181, /**< Connection failure, when a request is in progress. */

    BLE_STATUS_BASE_SM = -0x02200,                      /**< The base of the SM status. */

    BLE_STATUS_END
} ble_status_t;

/**
 * @}
 */

/**
 * @}
 * @}
 * @}
 */

#endif /* __BLE_STATUS_H__ */

