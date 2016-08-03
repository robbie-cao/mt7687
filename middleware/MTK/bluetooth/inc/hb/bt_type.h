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

#ifndef __BT_PRIMARY_TYPE_H__
#define __BT_PRIMARY_TYPE_H__

/**
 * @addtogroup Bluetooth
 * @{
 * @defgroup Common Common
 * @{
 * @defgroup type Type
 * @{
 * This section defines the basic types.
 *
 * Terms and Acronyms
 * ======
 * |Terms                         |Details                                                                  |
 * |------------------------------|-------------------------------------------------------------------------|
 * |\b ATT                        | Attribute Protocol. For more information, please refer to <a href="https://en.wikipedia.org/wiki/List_of_Bluetooth_protocols#Low_Energy_Attribute_Protocol_.28ATT.29">Wikipedia</a>.|
 * |\b GAP                        | Generic Access Profile defines the generic procedures related to the discovery of Bluetooth devices and link management aspects of connecting to Bluetooth devices. It also defines procedures related to use of different security levels. |
 * |\b GATT                       | Generic Attribute Profile defines a service framework using the attribute Protocol for discovering services, and for reading and writing characteristic value on a remote device. |
 * |\b HCI                        | Host Controller Interface. For more information, please refer to <a href="https://en.wikipedia.org/wiki/List_of_Bluetooth_protocols#HCI">Wikipedia</a>.|
 * |\b L2CAP                      | Logical Link Control And Adaptation Protocol Specification. For more information, please refer to <a href="https://en.wikipedia.org/wiki/List_of_Bluetooth_protocols#Logical_link_control_and_adaption_protocol_.28L2CAP.29">Wikipedia</a>. |
 * |\b SM                         | Security Manager. For more information, please refer to <a href="https://en.wikipedia.org/wiki/List_of_Bluetooth_protocols#Low_Energy_Security_Manager_Protocol_.28SMP.29">Wikipedia</a>. |
 *
 */

#include "bt_platform.h"

/**
 * @defgroup bt_primary_type_define Define
 * @{
 */

/**
 *  @brief      Bluetooth connection handle.
 */
#define BT_HANDLE_INVALID       0xFFFF  /**< Invalid connection handle. */
typedef uint16_t bt_handle_t; /**< Define the connection handle type. */

/**
 *  @brief      Bluetooth SM key size, please refer to the <a href="https://www.bluetooth.org/DocMan/handlers/DownloadDoc.ashx?doc_id=286439&_ga=1.241778370.1195131350.1437993589">Bluetooth core specification version 4.2 [VOL 3, part H] Session 2.3.4</a>.
 */
#define BT_KEY_SIZE  16
typedef uint8_t bt_key_t[BT_KEY_SIZE]; /**< Define the SM key type. */

/**
 *  @brief      Bluetooth GAP role, please refer to the <a href="https://www.bluetooth.org/DocMan/handlers/DownloadDoc.ashx?doc_id=286439&_ga=1.241778370.1195131350.1437993589">Bluetooth core specification version 4.2 [VOL 6, part B] Session 4.5</a>.
 */
#define BT_ROLE_MASTER                  0x00 /**< Master or Central. */
#define BT_ROLE_SLAVE                   0x01 /**< Slave or Peripheral. */
typedef uint8_t bt_role_t;  /**< Define the role type. */

/*
 * +---------+-----------------------------+
 * |Module ID  | Data                      |
 * +---------+-----------------------------+
 */
#define BT_MODULE_OFFSET                26          /**< Module range: 0x00 ~ 0x3F, maximum module: 64. */
#define BT_MODULE_MASK                  0x03FFFFFFU /**< Mask for Bluetooth module. */

#define BT_MODULE_GENERAL               (0x00U<<BT_MODULE_OFFSET) /**< Prefix of the general module. */
#define BT_MODULE_TIMER                 (0x01U<<BT_MODULE_OFFSET) /**< Prefix of the timer module. */
#define BT_MODULE_MM                    (0x02U<<BT_MODULE_OFFSET) /**< Prefix of the Memory Management module. */
#define BT_MODULE_HCI                   (0x03U<<BT_MODULE_OFFSET) /**< Prefix of the HCI module. */
#define BT_MODULE_GAP                   (0x04U<<BT_MODULE_OFFSET) /**< Prefix of the GAP module. */
#define BT_MODULE_L2CAP                 (0x05U<<BT_MODULE_OFFSET) /**< Prefix of the L2CAP module. */
#define BT_MODULE_SM                    (0x06U<<BT_MODULE_OFFSET) /**< Prefix of the SM module. */
#define BT_MODULE_ATT                   (0x07U<<BT_MODULE_OFFSET) /**< Prefix of the ATT module. */
#define BT_MODULE_GATT                  (0x08U<<BT_MODULE_OFFSET) /**< Prefix of the GATT module. */
#define BT_MODULE_SYSTEM                (0x09U<<BT_MODULE_OFFSET) /**< Prefix of the System module. */
#define BT_MODULE_GENERAL_ERROR         (0x3FU<<BT_MODULE_OFFSET) /**< Prefix of the general error module. */
#define BT_MODULE_NUM                   BT_MODULE_GENERAL_ERROR   /**< The maximum prefix value of the module. */
typedef uint32_t bt_msg_type_t;  /**< Define the message type. */

/**
 *  @brief      Bluetooth status definitions. For the ATT or HCI error codes, please refer to \ref GATT \ref HCI .
 */
#define BT_STATUS_SUCCESS               (BT_MODULE_GENERAL|(BT_MODULE_MASK&0x00))        /**< General Success. */

#define BT_STATUS_TIMER_FULL            (BT_MODULE_TIMER|(BT_MODULE_MASK&0x1))           /**< Timer Out-of-memory. */
#define BT_STATUS_TIMER_NOT_FOUND       (BT_MODULE_TIMER|(BT_MODULE_MASK&0x2))           /**< Timer not found in the timer manager. */

#define BT_STATUS_FAIL                  (BT_MODULE_GENERAL|(BT_MODULE_MASK&0x03FFFFF1))  /**< General failed. */
#define BT_STATUS_OUT_OF_MEMORY         (BT_MODULE_GENERAL|(BT_MODULE_MASK&0x03FFFFF2))  /**< General Out-of-memory. */
#define BT_STATUS_TIMEOUT               (BT_MODULE_GENERAL|(BT_MODULE_MASK&0x03FFFFF3))  /**< General Timeout. */
#define BT_STATUS_BUFFER_USED           (BT_MODULE_GENERAL|(BT_MODULE_MASK&0x03FFFFF4))  /**< General Buffer-used. */
#define BT_STATUS_UNSUPPORTED           (BT_MODULE_GENERAL|(BT_MODULE_MASK&0x03FFFFF5))  /**< General Unsupported. */
#define BT_STATUS_CONNECTION_IN_USE     (BT_MODULE_GENERAL|(BT_MODULE_MASK&0x03FFFFF6))  /**< General Connection-in-use. */

#define BT_STATUS_HCI_FLOW_CONTROLLED   (BT_MODULE_HCI|0x01)            /**< The packet is flow-controlled. */
#define BT_STATUS_RX_PENDING            (BT_MODULE_HCI|0x02)            /**< Internal use of the HCI. */

#define BT_STATUS_DIRECTED_ADVERTISING_TIMEOUT    (BT_MODULE_GAP|0x03)  /**< Directed Advertising Timeout defined in the specification. */

typedef int32_t bt_status_t;    /**< Define the status type, return from the APIs, please refer to the corresponding status BT_STATUS_XXX. */

#define BT_BD_ADDR_LEN      6   /**< Bluetooth Device Address Length defined in the specification. */

/**
 *  @brief      Address type, defined in spec.
 */
typedef uint8_t bt_bd_addr_t[BT_BD_ADDR_LEN];   /**< Bluetooth Device Address defined in the specification. */
typedef uint8_t const *bt_bd_addr_ptr_t;        /**< A pointer to the #bt_bd_addr_t. */

/**
 *  @brief      Address type, defined in spec.
 */
#define BT_ADDR_PUBLIC          0x00 /**< Public address type. */
#define BT_ADDR_RANDOM          0x01 /**< Random address type. */
#define BT_ADDR_PUBLIC_IDENTITY 0x02 /**< Public identity address type. */
#define BT_ADDR_RANDOM_IDENTITY 0x03 /**< Random identity address type. */
typedef uint8_t bt_addr_type_t;      /**< Define the address type. */

/**
 *  @brief      Boolean.
 */
#define BT_TRUE     1       /**< Boolean value: true. */
#define BT_FALSE    0       /**< Boolean value: false. */
typedef uint32_t bt_bool_t; /**< Define the Boolean type. */

#define BT_NULL     0   /**< NULL */

/**
 *  @brief      Variable length data type.
 */
typedef uint8_t bt_data_t[1];

/**
 * @}
 */

/**
 * @defgroup bt_primary_type_struct Struct
 * @{
 */

/**
 *  @brief      Address type
 */
BT_PACKED (
typedef struct {
    bt_addr_type_t  type;   /**< Address Type. */
    bt_bd_addr_t    addr;   /**< Bluetooth Device Address. */
}) bt_addr_t;

/**
 * @}
 */

/**
* @}
* @}
* @}
*/

#endif /*__BT_PRIMARY_TYPE_H__*/

