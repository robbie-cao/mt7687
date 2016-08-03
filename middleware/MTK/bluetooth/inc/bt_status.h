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

#ifndef __BT_STATUS_H__
#define __BT_STATUS_H__

/**
 * @addtogroup BT
 * @{
 * @addtogroup Common
 * @{
 * @addtogroup Common_3 Bluetooth status codes
 * @{
 * This section provides the Bluetooth status codes for Classic Bluetooth only.
 *
 * The Bluetooth status codes indicate success or failure of an API procedure. In case of failure, a comprehensive status code indicating
 * the cause or reason for the failure is provided.
 *
 * Terms and Acronyms
 * ======
 * |Terms                         |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b PDU                        | Protocol Data Unit, information that is delivered as a unit among peer entities of a network and that may contain control information, such as address information, or user data. |
 * |\b HCI                        | The host control interface (HCI) layer provides a means of communication between the host and controller via a standardized interface. |
 * |\b ACL                        | Asynchronous Connection-Less, a data transmission link in the Bluetooth system. |
 * |\b LMP                        | Link Management Protocol (LMP), a protocol for initiating and controlling the link between Bluetooth terminals. |
 * |\b LL                         | The link layer (LL) controls the RF state of the device. |
 * |\b SUL                        | Sound Unit Length (SUL) is one of the parameters that determine bit rate of the audio stream. |
 * |\b SEID                       | Stream End Point Identifier (SEID) is a SEP identifier. |
 * |\b SEP                        | Stream End Point (SEP) is a concept to expose available Transport Services and AV capabilities of the Application in order to negotiate a stream. |
 * |\b SDP                        | Service Discovery Protocol (SDP) is used to allow devices to discover what services are supported by each other, and what parameters to use to connect to them. |
 * |\b HFP                        | Hands-Free Profile, Bluetooth profiles allow hands-free kits to communicate with mobile phones in a car. |
 * |\b CRC                        | A cyclic redundancy check (CRC) is an error-detecting code commonly used in digital networks and storage devices to detect accidental changes to raw data. |
 * |\b AVCTP                      | Audio/Video Control Transport Protocol (AVCTP) is used to transport the command/response messages exchanged for the control of distant A/V devices over point-to-point connections. |
 * |\b PBAPC                      | The Phone Book Access Profile (PBAP) defines the procedures and protocols to exchange Phone Book objects between devices. |
 * |\b MAPC                       | Message Access Profile, a Bluetooth profile for exchange of messages between devices. |
 * |\b AVRCP                      | The Audio/Video Remote Control Profile (AVRCP) defines the features and procedures required in order to ensure interoperability between Bluetooth devices with audio/video control functions in the Audio/Video distribution scenarios. |
 * |\b GAVDP                      | Generic Audio/Video Distribution Profile (GAVDP) provides the basis for A2DP and VDP, the basis of the systems designed for distributing video and audio streams using Bluetooth technology. |
 * |\b HID                        | Human Interface Device Profile (HID) provides support for devices such as mice, joysticks, keyboards, as well as sometimes providing support for simple buttons and indicators on other types of devices. |
 * |\b SPP                        | Serial Port Profile, a Bluetooth wireless-interface profile. It emulates a serial cable to provide a simple substitute for existing RS-232, including the familiar control signals. |
 */

/**
 * @defgroup bt_status_enum Enum
 * @{
 */

/** @brief This enumeration defines the Bluetooth status codes. */
typedef enum {
    BT_STATUS_SUCCESS = 0,                              /**< The operation completed successfully. */
    BT_STATUS_UNKNOWN_HCI_CMD = -0x1,                   /**< The Controller does not understand the HCI Command Packet OpCode sent by the Host. */
    BT_STATUS_UNKNOWN_IDENTIFIER = -0x2,                /**< A command was sent from the Host to identify a connection, but that connection does not exist. */
    BT_STATUS_HARDWARE_FAILURE = -0x3,                  /**< It notifies the Host that the Controller operation has failed for an unknown reason and no error code is available. */
    BT_STATUS_PAGE_TIMEOUT = -0x4,                      /**< A page timed out because of the Page Timeout configuration parameter. */
    BT_STATUS_AUTHENTICATION_FAILURE = -0x5,            /**< Pairing or authentication failed due to incorrect results in the pairing or authentication procedure. */
    BT_STATUS_PIN_OR_KEY_MISSING = -0x6,                /**< It is used when pairing failed because of a missing PIN, or authentication failed because of a missing Key. */
    BT_STATUS_MEMORY_CAPACITY_EXCEEDED = -0x7,          /**< It notifies the Host that the Controller has run out of memory to store new parameters. */
    BT_STATUS_CONNECTION_TIMEOUT = -0x8,                /**< The link supervision timeout has expired for a given connection. */
    BT_STATUS_CONNECTION_LIMIT_EXCEEDED = -0x9,         /**< An attempt to create another connection failed because the Controller is already at its limit of the number of connections it can support. */
    BT_STATUS_SYNCHRONOUS_CONNECTION_LIMIT_TO_A_DEVICE_EXCEEDED = -0xA, /**< The Controller has reached the limit of the number of synchronous connections it supports for the device. */
    BT_STATUS_ACL_CONNECTION_ALREADY_EXISTS = -0xB,     /**< An attempt to create a new ACL Connection to a device when there is already an existing connection to this device. */
    BT_STATUS_COMMAND_DISALLOWED = -0xC,                /**< The Controller is unable to process the requested command. */
    BT_STATUS_REJECTED_BY_LIMITED_RESOURCES = -0xD,     /**< An incoming connection was rejected due to limited resources. */
    BT_STATUS_REJECTED_BY_SECURITY = -0xE,              /**< A connection was rejected due to security requirements not being fulfilled, such as authentication or pairing. */
    BT_STATUS_REJECTED_BY_UNACCEPTABLE_ADDRESS = -0xF,  /**< A connection was rejected because this device does not accept the BD_ADDR. */
    BT_STATUS_ACCEPT_TIMEOUT = -0x10,                   /**< The Connection Accept Timeout has been exceeded for this connection attempt. */
    BT_STATUS_UNSUPPORTED_FEATURE_OR_PARAM = -0x11,     /**< A feature or parameter value in the HCI command is not supported. */
    BT_STATUS_INVALID_HCI_CMD_PARAM = -0x12,            /**< At least one of the HCI command parameters is invalid. */
    BT_STATUS_REMOTE_USER_TERMINATED = -0x13,           /**< The user on the remote device terminated the connection. */
    BT_STATUS_REMOTE_DEVICE_LOW_RESOURCE = -0x14,       /**< The remote device terminated the connection because of low resource availability. */
    BT_STATUS_REMOTE_DEVICE_POWER_OFF = -0x15,          /**< The remote device terminated the connection because the device is about to power off. */
    BT_STATUS_LOCAL_HOST_TERMINATED = -0x16,            /**< The local device terminated the connection. */
    BT_STATUS_REPEATED_ATTEMPTS = -0x17,                /**< The Controller forbids an authentication or pairing procedure because too little
                                                             time has elapsed since the last failed attempt for authentication or pairing. */
    BT_STATUS_PAIRING_NOT_ALLOWED = -0x18,              /**< The device does not allow pairing. */
    BT_STATUS_UNKNOWN_LMP_PDU = -0x19,                  /**< The Controller has received an unknown LMP OpCode. */
    BT_STATUS_UNSUPPORTED_REMOTE_FEATURE_OR_LMP_FEATURE = -0x1A, /**< The remote device does not support a feature associated with the issued command or LMP PDU. */
    BT_STATUS_SCO_OFFSET_REJECTED = -0x1B,              /**< The offset requested in the LMP_SCO_link_req PDU is rejected. */
    BT_STATUS_SCO_INTERVAL_REJECTED = -0x1C,            /**< The interval requested in the LMP_SCO_link_req PDU is rejected. */
    BT_STATUS_SCO_AIR_MODE_REJECTED = -0x1D,            /**< The air mode requested in the LMP_SCO_link_req PDU is rejected. */
    BT_STATUS_INVALID_LMP_OR_LL_PARAM = -0x1E,          /**< Some LMP PDU / LL Control PDU parameters are invalid. */
    BT_STATUS_UNSPECIFIED_ERROR = -0x1F,                /**< No other error code specified is appropriate to use. */
    BT_STATUS_UNSUPPORTED_LMP_OR_LL_PARAM_VALUE = -0x20, /**< An LMP PDU or an LL Control PDU contains at least one parameter value that is not supported by the Controller at this time. */
    BT_STATUS_ROLE_CHANGE_NOT_ALLOWED = -0x21,          /**< A Controller will not allow a role change at this time. */
    BT_STATUS_LMP_OR_LL_RESPONSE_TIMEOUT = -0x22,       /**< An LMP transaction failed to respond within the LMP response timeout or an LL transaction failed to respond within the LL response timeout. */
    BT_STATUS_LMP_ERROR_TRANSACTION_COLLISION = -0x23,  /**< An LMP transaction has collided with the same transaction that is already in progress. */
    BT_STATUS_LMP_PDU_NOT_ALLOWED = -0x24,              /**< A Controller sent an LMP PDU with an OpCode that was not allowed. */
    BT_STATUS_ENCRYPTION_MODE_NOT_ACCEPTABLE = -0x25,   /**< The requested encryption mode is not acceptable at this time. */
    BT_STATUS_LINK_KEY_CANNOT_BE_CHANGED = -0x26,       /**< Link key cannot be changed because a fixed unit key is being used. */
    BT_STATUS_REQUESTED_QOS_NOT_SUPPORTED = -0x27,      /**< The requested Quality of Service is not supported. */
    BT_STATUS_INSTANT_PASSED = -0x28,                   /**< An LMP PDU or LL PDU that includes an instant cannot be performed because the instant when this would have occurred has passed. */
    BT_STATUS_PAIRING_WITH_UNIT_KEY_NOT_SUPPORTED = -0x29, /**< Impossible to establish pairing as the requested unit key is not supported. */
    BT_STATUS_DIFFERENT_TRANSACTION_COLLISION = -0x2A,  /**< An initiated LMP transaction collided with an ongoing transaction. */
    /* 0x2B Reserved */
    BT_STATUS_QOS_UNACCEPTABLE_PARAM = -0x2C,           /**< The specified Quality of Service parameters could not be accepted. */
    BT_STATUS_QOS_REJECTED = -0x2D,                     /**< The specified Quality of Service parameters cannot be accepted and Quality of Service negotiation should be terminated. */
    BT_STATUS_CHANNEL_CLASSIFICATION_NOT_SUPPORTED = -0x2E, /**< The Controller cannot perform channel assessment because it is not supported. */
    BT_STATUS_INSUFFICIENT_SECURITY = -0x2F,            /**< Sending the HCI command or LMP PDU is only possible on an encrypted link. */
    BT_STATUS_PARAM_OUT_OF_RANGE = -0x30,               /**< A parameter value requested is outside the mandatory range of parameters for the given HCI command or LMP PDU
                                                             and the recipient does not accept that value. */
    /* 0x31 Reserved */
    BT_STATUS_ROLE_SWITCH_PENDING = -0x32,              /**< A Role Switch is pending. */
    /* 0x33 Reserved */
    BT_STATUS_RESERVED_SLOT_VIOLATION = -0x34,          /**< The current Synchronous negotiation was terminated with the negotiation state set to Reserved Slot Violation. */
    BT_STATUS_ROLE_SWITCH_FAILED = -0x35,               /**< A role switch was attempted but it failed and the original piconet structure is restored. */
    BT_STATUS_EXTENDED_INQUIRY_RESPONSE_TOO_LARGE = -0x36, /**< The extended inquiry response with the requested requirements for FEC is too large to fit in any of the packet types
                                                             supported by the Controller. */
    BT_STATUS_SSP_NOT_SUPPORTED_BY_HOST = -0x37,        /**< The IO capabilities request or response was rejected because the sending Host does not support Secure Simple Pairing
                                                             even if the receiving Link Manager does. */
    BT_STATUS_HOST_PAIRING_BUSY = -0x38,                /**< The Host is busy with another pairing operation and unable to support the requested pairing. */
    BT_STATUS_NO_SUITABLE_CHANNEL_FOUND = -0x39,        /**< The Controller could not calculate an appropriate value for the Channel selection operation. */
    BT_STATUS_CONTROLLER_BUSY = -0x3A,                  /**< The operation was rejected because the Controller was busy and unable to process the request. */
    BT_STATUS_UNACCEPTABLE_CONNECTION_PARAM = -0x3B,    /**< The remote device either terminated the connection or rejected a request because of one
                                                             or more unacceptable connection parameters. */
    BT_STATUS_DIRECTED_ADVERTSING_TIMEOUT = -0x3C,      /**< Directed advertising completed without a connection being created. */
    BT_STATUS_MIC_FAILURE = -0x3D,                      /**< The connection was terminated because the Message Integrity Check (MIC) failed on a received packet. */
    BT_STATUS_CONNECTION_FAILED_TO_BE_ESTABLISHED = -0x3E, /**< The LL initiated a connection but the connection failed to be established. */
    BT_STATUS_MAC_CONNECTION_FAILED = -0x3F,            /**< The MAC of the 802.11 AMP was requested to connect to a peer, but the connection failed. */
    BT_STATUS_COARSE_CLOCK_ADJUSTMENT_REJECTED = -0x40, /**< The master is unable to make a coarse adjustment to the piconet clock at a moment using the supplied parameters. */

    BT_STATUS_HCI_BASE = -0x0100,                       /**< The base of the HCI status. */

    BT_STATUS_GAP_BASE = -0x0200,                       /**< The base of the GAP status. */
    BT_STATUS_GAP_FAILED = -0x0201,                     /**< GAP operation failed. */
    BT_STATUS_GAP_PENDING = -0x0202,                    /**< GAP operation successfully started but pending. */
    BT_STATUS_GAP_IN_USE = -0x0205,                     /**< GAP operation is already in use. */
    BT_STATUS_GAP_BUSY = -0x020B,                       /**< One GAP operation is ongoing, the new operation cannot be added. */
    BT_STATUS_GAP_NO_RESOURCES = -0x020C,               /**< There are no resources for the GAP operation. */
    BT_STATUS_GAP_NOT_FOUND = -0x020D,                  /**< There is no result found for the GAP operation. */
    BT_STATUS_GAP_DEVICE_NOT_FOUND = -0x020E,           /**< Device is not found for the GAP operation. */
    BT_STATUS_GAP_CONNECTION_FAILED = -0x020F,          /**< Failed to create connection. */
    BT_STATUS_GAP_TIMEOUT = -0x0210,                    /**< Timeout for the GAP operation. */
    BT_STATUS_GAP_NO_CONNECTION = -0x0211,              /**< There is no connection. */
    BT_STATUS_GAP_INVALID_PARAM = -0x0212,              /**< The parameter is invalid. */
    BT_STATUS_GAP_IN_PROGRESS = -0x0213,                /**< The current operation is ongoing. */
    BT_STATUS_GAP_RESTRICTED = -0x0214,                 /**< The remote device already has a restricted link. */
    BT_STATUS_GAP_INVALID_TYPE = -0x0215,               /**< The type is invalid. */
    BT_STATUS_GAP_HCI_INIT_ERROR = -0x0216,             /**< There is HCI initialization error. */
    BT_STATUS_GAP_NOT_SUPPORTED = -0x0217,              /**< The operation or the feature is not supported. */
    BT_STATUS_GAP_SDP_CONTINUE_STATE = -0x0218,         /**< The data cannot be sent in one packet. */
    BT_STATUS_GAP_CANCELLED = -0x0219,                  /**< The operation is cancelled. */
    BT_STATUS_GAP_NO_SERVICES = -0x021A,                /**< There are no services. */
    BT_STATUS_GAP_SCO_REJECTED = -0x021B,               /**< LMP_SCO_link_req PDU is rejected. */
    BT_STATUS_GAP_ERROR_CHIP_REASON = -0x021C,          /**< A failure occurred in the Controller and it cannot be described with any error code. */
    BT_STATUS_GAP_BLOCK_LIST = -0x021D,                 /**< Error code reserved. */
    BT_STATUS_GAP_SCATTERNET_REJECTED = -0x021E,        /**< Error code reserved. */
    BT_STATUS_GAP_REMOTE_REJECTED = -0x021F,            /**< Error code reserved. */
    BT_STATUS_GAP_KEY_ERROR = -0x0220,                  /**< Error code reserved. */
    BT_STATUS_GAP_CONNECTION_EXISTED = -0x0221,         /**< The connection already exists. */

    BT_STATUS_L2CAP_BASE = -0x0300,                     /**< The base of the L2CAP status */

    BT_STATUS_RFCOMM_BASE = -0x0400,                    /**< The base of the RFCOMM status */

    BT_STATUS_SDP_BASE = -0x0500,                       /**< The base of the SDP status */

    BT_STATUS_HS_BASE = -0x1000,                        /**< The base of the Headset (HS) status */

    BT_STATUS_HF_BASE = -0x1100,                        /**< The base of the Hands Free (HF) status */
    BT_STATUS_HF_PENDING = -0x1101,                     /**< HFP operation successfully started but pending. */
    BT_STATUS_HF_BUSY = -0x1102,                        /**< Currently there is another ongoing operation. */
    BT_STATUS_HF_NO_RESOURCES = -0x1103,                /**< There are no resources for the HFP operation. */
    BT_STATUS_HF_DEVICE_NOT_FOUND = -0x1105,            /**< Device is not found for the HFP operation. */
    BT_STATUS_HF_TIMEOUT = -0x1106,                     /**< Timeout for the HFP operation. */
    BT_STATUS_HF_NO_CONNECTION = -0x1107,               /**< The HFP is not connected successfully. */
    BT_STATUS_HF_INVALID_PARAM = -0x1108,               /**< The parameter is invalid. */
    BT_STATUS_HF_IN_PROGRESS = -0x1109,                 /**< The current operation is ongoing. */
    BT_STATUS_HF_NOT_SUPPORTED = -0x110A,               /**< The operation or the feature is not supported. */
    BT_STATUS_HF_IN_USE = -0x110B,                      /**< HFP operation is already in use. */
    BT_STATUS_HF_CANCELLED = -0x110C,                   /**< The operation is cancelled. */
    BT_STATUS_HF_NO_SERVICES = -0x110D,                 /**< There are no services. */
    BT_STATUS_HF_SCO_REJECTED = -0x110E,                /**< LMP_SCO_link_req PDU is rejected. */
    BT_STATUS_HF_CHIP_REASON = -0x110F,                 /**< A failure occurred in the Controller and it cannot be described with any error code. */
    BT_STATUS_HF_REMOTE_REJECTED = -0x1110,             /**< Error code reserved. */
    BT_STATUS_HF_CONNECTION_EXISTED = -0x1111,          /**< The connection already exists. */

    BT_STATUS_A2DP_BASE = -0x1200,                      /**< The base of the A2DP status. */
    BT_STATUS_A2DP_OK = 0,                              /**< The operation completed successfully. */
    BT_STATUS_A2DP_INVALID_CODEC_TYPE = -0x12C1,        /**< The Media Codec Type is not valid. */
    BT_STATUS_A2DP_NOT_SUPPORTED_CODEC_TYPE = -0x12C2,  /**< The Media Codec Type is not supported. */
    BT_STATUS_A2DP_INVALID_SAMPLING_FREQUENCY = -0x12C3, /**< The Sampling Frequency is not valid or multiple values are selected. */
    BT_STATUS_A2DP_NOT_SUPPORTED_SAMPLING_FREQUENCY = -0x12C4, /**< The Sampling Frequency is not supported. */
    BT_STATUS_A2DP_INVALID_CHANNEL_MODE = -0x12C5,      /**< The Channel Mode is not valid or multiple values are selected. */
    BT_STATUS_A2DP_NOT_SUPPORTED_CHANNEL_MODE = -0x12C6, /**< The Channel Mode is not supported. */
    BT_STATUS_A2DP_INVALID_SUBBANDS = -0x12C7,          /**< None or multiple values are selected for the Number of Subbands. */
    BT_STATUS_A2DP_NOT_SUPPORTED_SUBBANDS = -0x12C8,    /**< The Number of Subbands is not supported. */
    BT_STATUS_A2DP_INVALID_ALLOCATION_METHOD = -0x12C9, /**< None or multiple values are selected for the Allocation Method. */
    BT_STATUS_A2DP_NOT_SUPPORTED_ALLOCATION_METHOD = -0x12CA, /**< The Allocation Method is not supported. */
    BT_STATUS_A2DP_INVALID_MIN_BITPOOL_VALUE = -0x12CB, /**< The Minimum Bitpool Value is not valid. */
    BT_STATUS_A2DP_NOT_SUPPORTED_MIN_BITPOOL_VALUE = -0x12CC, /**< The Minimum Bitpool Value is not supported. */
    BT_STATUS_A2DP_INVALID_MAX_BITPOOL_VALUE = -0x12CD, /**< The Maximum Bitpool Value is not valid. */
    BT_STATUS_A2DP_NOT_SUPPORTED_MAX_BITPOOL_VALUE = -0x12CE, /**< The Maximum Bitpool Value is not supported. */
    BT_STATUS_A2DP_INVALID_LAYER = -0x12CF,             /**< None or multiple values are selected for the Layer. */
    BT_STATUS_A2DP_NOT_SUPPORTED_LAYER = -0x12D0,       /**< The Layer is not supported. */
    BT_STATUS_A2DP_NOT_SUPPORTED_CRC = -0x12D1,         /**< The CRC is not supported. */
    BT_STATUS_A2DP_NOT_SUPPORTED_MPF = -0x12D2,         /**< The Media Payload Format (MPF-2) is not supported. */
    BT_STATUS_A2DP_NOT_SUPPORTED_VBR = -0x12D3,         /**< The Variable Bit Rate is not supported. */
    BT_STATUS_A2DP_INVALID_BIT_RATE = -0x12D4,          /**< None or multiple values are selected for the the Bit Rate. */
    BT_STATUS_A2DP_NOT_SUPPORTED_BIT_RATE = -0x12D5,    /**< The Bit Rate is not supported. */
    BT_STATUS_A2DP_INVALID_OBJECT_TYPE = -0x12D6,       /**< 1) The Object type is not valid (b3-b0) or 2) None or multiple values are selected for the Object Type. */
    BT_STATUS_A2DP_NOT_SUPPORTED_OBJECT_TYPE = -0x12D7, /**< The Object Type is not supported. */
    BT_STATUS_A2DP_INVALID_CHANNELS = -0x12D8,          /**< None or multiple values are selected for Channels. */
    BT_STATUS_A2DP_NOT_SUPPORTED_CHANNELS = -0x12D9,    /**< The Channels are not supported. */
    BT_STATUS_A2DP_INVALID_VERSION = -0x12DA,           /**< The Version is not valid. */
    BT_STATUS_A2DP_NOT_SUPPORTED_VERSION = -0x12DB,     /**< The Version is not supported. */
    BT_STATUS_A2DP_NOT_SUPPORTED_MAX_SUL = -0x12DC,     /**< The Maximum SUL is not acceptable for the Decoder in the Sink (SNK). */
    BT_STATUS_A2DP_INVALID_BLOCK_LENGTH = -0x12DD,      /**< None or multiple values are selected for the Block Length. */
    BT_STATUS_A2DP_INVALID_CP_TYPE = -0x12E0,           /**< The requested Content Protection Type is not supported. */
    BT_STATUS_A2DP_INVALID_CP_FORMAT = -0x12E1,         /**< The format of Content Protection Service Capability/Content Protection Scheme Dependent Data is not correct. */
    BT_STATUS_A2DP_TIMEOUT = -0x12F0,                   /**< Timeout for the A2DP operation. */
    BT_STATUS_A2DP_BAD_STATE = -0x12F1,                 /**< The state for the operation is not correct. */
    BT_STATUS_A2DP_FATAL_ERROR = -0x12F2,               /**< An internal error occurred. */
    BT_STATUS_A2DP_NO_CONNECTION = -0x12F3,             /**< The A2DP failed to connect. */
    BT_STATUS_A2DP_SEP_IN_USE = -0x12F4,                /**< The A2DP does not disconnect after deactivation. */
    BT_STATUS_A2DP_BAD_ACP_SEID = -0x12F5,              /**< The Acceptor (ACP) SEID is invalid. */

    BT_STATUS_GAVDP_BASE = -0x1300,                     /**< The base of the GAVDP status. */
    BT_STATUS_GAVDP_OK = 0,                             /**< The operation completed successfully. */
    BT_STATUS_GAVDP_BAD_SERVICE = -0x1380,              /**< The Service Category is invalid. */
    BT_STATUS_GAVDP_INSUFFICIENT_RESOURCE = -0x1381,    /**< Lack of resource in the new Stream Context. */

    BT_STATUS_AVDTP_BASE = -0x1400,                     /**< The base of the GAVDP status. */
    BT_STATUS_AVDTP_OK = 0,                             /**< The operation completed successfully. */
    BT_STATUS_AVDTP_BAD_HEADER_FORMAT = -0x1401,        /**< The requested packet header format is not included in this specification. */
    BT_STATUS_AVDTP_BAD_LENGTH = -0x1411,               /**< The requested packet length does not match the assumed length. */
    BT_STATUS_AVDTP_BAD_ACP_SEID = -0x1412,             /**< The requested command indicates an invalid Acceptor (ACP) SEID (not addressable). */
    BT_STATUS_AVDTP_SEP_IN_USE = -0x1413,               /**< The SEP is in use. */
    BT_STATUS_AVDTP_SEP_NOT_IN_USE = -0x1414,           /**< The SEP is not in use. */
    BT_STATUS_AVDTP_BAD_SERVICE_CATEGORY = -0x1417,     /**< The value of Service Category in the requested packet is not defined in the AVDTP. */
    BT_STATUS_AVDTP_BAD_PAYLOAD_FORMAT = -0x1418,       /**< The requested command has an incorrect payload format (format errors are not specified in this ERROR_CODE). */
    BT_STATUS_AVDTP_NOT_SUPPORTED_COMMAND = -0x1419,    /**< The requested command is not supported by the device. */
    BT_STATUS_AVDTP_INVALID_CAPABILITIES = -0x141A,     /**< The reconfigure command is an attempt to reconfigure transport service capabilities of the SEP. */
    BT_STATUS_AVDTP_BAD_RECOVERY_TYPE = -0x1422,        /**< The requested Recovery Type is not defined in AVDTP. */
    BT_STATUS_AVDTP_BAD_MEDIA_TRANSPORT_FORMAT = -0x1423, /**< The format of Media Transport Capability is not correct. */
    BT_STATUS_AVDTP_BAD_RECOVERY_FORMAT = -0x1425,      /**< The format of Recovery Service Capability is not correct. */
    BT_STATUS_AVDTP_BAD_ROHC_FORMAT = -0x1426,          /**< The format of Header Compression Service Capability is not correct. */
    BT_STATUS_AVDTP_BAD_CP_FORMAT = -0x1427,            /**< The format of Content Protection Service Capability is not correct. */
    BT_STATUS_AVDTP_BAD_MULTIPLEXING_FORMAT = -0x1428,  /**< The format of Multiplexing Service Capability is not correct. */
    BT_STATUS_AVDTP_UNSUPPORTED_CONFIG = -0x1429,       /**< The Configuration is not supported. */
    BT_STATUS_AVDTP_BAD_STATE = -0x1431,                /**< The Acceptor state machine is in an invalid state in order to process the signal. */

    BT_STATUS_AVRCP_BASE = -0x1500,                     /**< The base of the AVRCP status. */
    BT_STATUS_AVRCP_OK = 0,                             /**< The operation completed successfully. */
    BT_STATUS_AVRCP_INVALID_CMD = -0x1500,              /**< Error code for an invalid command. The Target (TG) did not recognize the received PDU. */
    BT_STATUS_AVRCP_INVALID_PARAM = -0x1501,            /**< Error code for an invalid parameter. The Target (TG) did not recognize the received PDU with a given parameter ID. */
    BT_STATUS_AVRCP_NOT_FOUND = -0x1502,                /**< Error code for the specified parameter is not found. The parameter ID is identified, but the associated content is wrong or corrupted. */
    BT_STATUS_AVRCP_INTERNAL_ERROR = -0x1503,           /**< Error code for internal error. Send if other errors occurred. */
    BT_STATUS_AVRCP_INVALID_ROLE = -0x15F0,             /**< The AVRCP role for the operation is not correct. */
    BT_STATUS_AVRCP_CHANNEL_NOT_FOUND = -0x15F1,        /**< The AVRCP is not connected successfully. */
    BT_STATUS_AVRCP_BAD_STATE = -0x15F2,                /**< The state for the operation is not correct. */
    BT_STATUS_AVRCP_BUSY = -0x15F3,                     /**< Currently there is another ongoing operation. */

    BT_STATUS_AVCTP_BASE = -0x1600,                     /**< The base of the AVCTP status. */

    BT_STATUS_PBAPC_BASE = -0x1700,                     /**< The base of the PBAPC status. */
    BT_STATUS_PBAPC_OK = 0,                             /**< The operation completed successfully. */
    BT_STATUS_PBAPC_ERROR = -0x1701,                    /**< There is a PBAPC error. */
    BT_STATUS_PBAPC_INVALID_PARAM = -0x1702,            /**< The parameter is invalid. */
    BT_STATUS_PBAPC_PENGDING = -0x1703,                 /**< PBAPC operation successfully started but pending. */
    BT_STATUS_PBAPC_BUSY = -0x1704,                     /**< Currently there is another ongoing operation. */

    BT_STATUS_MAPC_BASE = -0x1800,                      /**< The base of the MAPC status. */

    BT_STATUS_HID_BASE = -0x1900,                       /**< The base of the HID status. */

    BT_STATUS_SPP_BASE = -0x1A00,                       /**< The base of the SPP status. */
    BT_STATUS_SPP_CONNECTION_FAIL = -0x1A01,            /**< The connection failed. */
    BT_STATUS_SPP_INVALID_PARAM = -0x1A02,              /**< Input invalid parameters. */
    BT_STATUS_SPP_NO_AVALIALBE_PORT = -0x1A03,          /**< All ports are exhausted, no other port is available for a new server or client application. */
    BT_STATUS_SPP_BUFFER_ASSIGN_FAIL = -0x1A04,         /**< The SPP TX/RX buffer set error, the size of assigned TX/RX buffer should be
                                                             larger than #BT_SPP_MIN_TX_BUFSIZE and #BT_SPP_MIN_RX_BUFSIZE. */
    BT_STATUS_SPP_ACL_LINK_CREATION_FAIL = -0x1A05,     /**< Create ACL link failed. */
    BT_STATUS_SPP_SDP_QUERY_REQUEST_FAIL = -0x1A06,     /**< SDP query failed. */
    BT_STATUS_SPP_DISCONNECTION_FAIL = -0x1A07,         /**< The disconnection failed. */
    BT_STATUS_SPP_ABNORAML_DISCONNECTION = -0x1A08,     /**< The disconnection attempt failed because the SPP connection doesn't exist. */
    BT_STATUS_SPP_INTERNAL_ERROR = -0x1AF0,             /**< Internal error occurred in the SPP library. */
    BT_STATUS_SPP_OTHER_ERROR = -0x1AF1,                /**< For other unknown reason. */

    BT_STATUS_MEDIA_BASE = -0x1B00,                     /**< The base of the Media status. */
    BT_STATUS_MEDIA_OK = 0,                             /**< Media status is successful. */
    BT_STATUS_MEDIA_ERROR = -0x1B01,                    /**< The Media status indicates an error. */
    BT_STATUS_MEDIA_INVALID_PARAM = -0x1B02,            /**< Configuration of Bluetooth codec is invalid. */

    BT_STATUS_END
} bt_status_t;

/**
 * @}
 */

/**
 * @}
 * @}
 * @}
 */

#endif /* __BT_STATUS_H__ */

