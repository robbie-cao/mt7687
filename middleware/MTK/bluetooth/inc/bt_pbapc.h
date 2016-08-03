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

#ifndef __BT_PBAPC_H__
#define __BT_PBAPC_H__

/**
 * @addtogroup BT
 * @{
 * @addtogroup PBAPC
 * @{
 * This section introduces the PBAPC APIs including terms and acronyms, supported features, details on how to use this module, enums, structures and functions.
 * The Phonebook Access Profile (PBAP) defines the protocols and procedures to exchange Phonebook objects between two peer devices.
 * PBAP client (PBAPC) is implemented for a client side connection according to PBAP specification. It can
 * initiate a connection to a remote device.
 *
 * Terms and Acronyms
 * ======
 * |Terms                         |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b AUTH                       | Authentication. |
 * |\b MCH                        | Missed Call History. |
 *
 * @section bt_pbapc_api_usage How to use this module
 * - PBAP connection in Client mode.
 *  - Step1: Mandatory, call #bt_pbapc_connect() to initiate a connection to a remote server.
 *   - Sample code:
 *    @code
 *       ret = bt_pbapc_connect(const bt_address_t *device_addr);
 *    @endcode
 *  - Step2: Optional, call #bt_pbapc_connect_auth_response() to process authentication in this connection.
 *  - Step3: A #BT_PBAPC_CONNECT_CNF event notifies the #bt_pbapc_common_callback() if the connection is established.
 *    - Sample code:
 *     @code
 *       void bt_pbapc_common_callback(bt_event_t event_id, const void *param)
 *       {
 *           switch (event_id)
 *           {
 *               case BT_PBAPC_CONNECT_CNF:
 *               {
 *                   bt_pbapc_connect_cnf_t *data = (bt_pbapc_connect_cnf_t*)param;
 *                   // Check the connection result and save conn_id if the connection is successful.
 *                   break;
 *               }
 *           }
 *       }
 *     @endcode
 * .
 * - Get data. Provides two methods, download and browsing.
 *  -  Download.
 *   - Step1: Optional, call #bt_pbapc_pull_phonebook() to retrieve the phonebook objects from a remote device.
 *    - Sample code:
 *     @code
 *        ret = bt_pbapc_pull_phonebook(conn_id, name, param);
 *     @endcode
 *   - Step2: A #BT_PBAPC_DATA_IND event notifies the #bt_pbapc_common_callback() if there is data received from a remote device.
 *   - Step3: A #BT_PBAPC_PULL_PHONEBOOK_CNF event notifies the #bt_pbapc_common_callback() as a result of a call to #bt_pbapc_pull_phonebook().
 *    - Sample code:
 *     @code
 *        void bt_pbapc_common_callback(bt_event_t event_id, const void *param)
 *        {
 *            switch (event_id)
 *            {
 *                 case BT_PBAPC_PULL_PHONEBOOK_CNF:
 *                 {
 *                     bt_pbapc_pull_phonebook_cnf_t *data = (bt_pbapc_pull_phonebook_cnf_t *)param;
 *                     // Check the result.
 *                     break;
 *                 }
 *                 case BT_PBAPC_DATA_IND:
 *                 {
 *                     bt_pbapc_data_ind_t *data = (bt_pbapc_data_ind_t*)param;
 *                     // Save the data if the result is successful.
 *                     break;
 *                 }
 *            }
 *        }
 *     @endcode
 *  - Browsing.
 *   - Step1: Optional, call #bt_pbapc_set_phonebook() to set current phonebook to a destination path.
 *    - Sample code:
 *     @code
 *        ret = bt_pbapc_set_phonebook(conn_id, path_name, type);
 *     @endcode
 *   - Step2: A #BT_PBAPC_SET_PHONEBOOK_CNF event notifies the #bt_pbapc_common_callback() as a result of a call to #bt_pbapc_set_phonebook().
 *    - Sample code:
 *     @code
 *        void bt_pbapc_common_callback(bt_event_t event_id, const void *param)
 *        {
 *            switch (event_id)
 *            {
 *                 case BT_PBAPC_SET_PHONEBOOK_CNF:
 *                 {
 *                     bt_pbapc_set_phonebook_cnf_t *data = (bt_pbapc_set_phonebook_cnf_t *)param;
 *                     // Check the result.
 *                     break;
 *                 }
 *            }
 *        }
 *     @endcode
 *   - Step3: Optional, once the path is set, call #bt_pbapc_pull_vcard_listing() to retrieve vCard-listing of the current phonebook.
 *    - Sample code:
 *     @code
 *        ret = bt_pbapc_pull_vcard_listing(conn_id, name, param);
 *     @endcode
 *   - Step4: A #BT_PBAPC_DATA_IND event notifies the #bt_pbapc_common_callback() if there is data received from a remote device.
 *   - Step5: A #BT_PBAPC_PULL_VCARD_LISTING_CNF event notifies the #bt_pbapc_common_callback() as a result of a call to #bt_pbapc_pull_vcard_listing().
 *    - Sample code:
 *     @code
 *        void bt_pbapc_common_callback(bt_event_t event_id, const void *param)
 *        {
 *            switch (event_id)
 *            {
 *                case BT_PBAPC_PULL_VCARD_LISTING_CNF:
 *                {
 *                    bt_pbapc_pull_vcard_list_cnf_t *data = (bt_pbapc_pull_vcard_list_cnf_t *)param;
 *                    // Check the result.
 *                    break;
 *                }
 *                case BT_PBAPC_DATA_IND:
 *                {
 *                    bt_pbapc_data_ind_t *data = (bt_pbapc_data_ind_t*)param;
 *                    // Save the data if the result is successful.
 *                    break;
 *                }
 *            }
 *        }
 *     @endcode
 *   - Step3: Optional, once the path is set, call #bt_pbapc_pull_vcard_entry() to retrieve a specific vCard object.
 *    - Sample code:
 *     @code
 *        ret = bt_pbapc_pull_vcard_entry(conn_id, name, param);
 *     @endcode
 *   - Step4: A #BT_PBAPC_DATA_IND event notifies the #bt_pbapc_common_callback() if there is data received from a remote device.
 *   - Step5: A #BT_PBAPC_PULL_VCARD_ENTRY_CNF event notifies the #bt_pbapc_common_callback() as a result of a call to #bt_pbapc_pull_vcard_entry().
 *    - Sample code:
 *     @code
 *        void bt_pbapc_common_callback(bt_event_t event_id, const void *param)
 *        {
 *            switch (event_id)
 *            {
 *                case BT_PBAPC_PULL_VCARD_ENTRY_CNF:
 *                {
 *                    bt_pbapc_pull_vcard_entry_cnf_t *data = (bt_pbapc_pull_vcard_entry_cnf_t *)param;
 *                    // Check the result.
 *                    break;
 *                }
 *                case BT_PBAPC_DATA_IND:
 *                {
 *                    bt_pbapc_data_ind_t *data = (bt_pbapc_data_ind_t*)param;
 *                    // Save the data if the result is successful.
 *                    break;
 *                }
 *            }
 *        }
 *     @endcode
 * -  Disconnect PBAPC connection.
 *  - Step1: Call #bt_pbapc_disconnect() to disconnect the PBAPC connection from a remote device.
 *    @code
 *       ret = bt_pbapc_disconnect(port);
 *    @endcode
 *  - Step2: #BT_PBAPC_DISCONNECT_IND event notifies the #bt_pbapc_common_callback() to indicate the result of the disconnection.
 *    @code
 *       bt_pbapc_common_callback(bt_event_t event_id, void* parameter)
 *       {
 *           switch (event_id)
 *           {
 *               case BT_PBAPC_DISCONNECT_IND:
 *               {
 *                   if (parameter->result == BT_STATUS_SUCCESS) {
 *                       // PBAPC disconnected.
 *                   }
 *                   break;
 *               }
 *           }
 *       }
 *    @endcode
 */

#include <stdint.h>
#include <stdbool.h>
#include "bt_address.h"
#include "bt_status.h"
#include "bt_events.h"

/**
 * @defgroup bt_pbapc_define Define
 * @{
 */

#define MAX_BT_PBAPC_SEARCH_VALUE_LEN   32                /**< Maximum search value length. */
#define BT_PBAPC_MAX_NAME_LEN           128               /**< Maximum path name length. */
#define BT_PBAPC_VCARD_FILTER_VER       (1 << (0 + 1))    /**< Version (Bit 0). */
#define BT_PBAPC_VCARD_FILTER_FN        (1 << (1 + 1))    /**< Formatted Name (Bit 1). */
#define BT_PBAPC_VCARD_FILTER_N         (1 << (2 + 1))    /**< Structured Presentation of Name (Bit 2). */
#define BT_PBAPC_VCARD_FILTER_PHOTO     (1 << (3 + 1))    /**< Associated Image or Photo (Bit 3). */
#define BT_PBAPC_VCARD_FILTER_BDAY      (1 << (4 + 1))    /**< Birthday (Bit 4). */
#define BT_PBAPC_VCARD_FILTER_ADR       (1 << (5 + 1))    /**< Delivery Address (Bit 5). */
#define BT_PBAPC_VCARD_FILTER_LABEL     (1 << (6 + 1))    /**< Delivery (Bit 6). */
#define BT_PBAPC_VCARD_FILTER_TEL       (1 << (7 + 1))    /**< Telephone (Bit 7). */
#define BT_PBAPC_VCARD_FILTER_EMAIL     (1 << (8 + 1))    /**< Electronic Mail Address (Bit 8). */
#define BT_PBAPC_VCARD_FILTER_MAILER    (1 << (9 + 1))    /**< Electronic Mail (Bit 9). */
#define BT_PBAPC_VCARD_FILTER_TZ        (1 << (10 + 1))   /**< Time Zone (Bit 10). */
#define BT_PBAPC_VCARD_FILTER_GEO       (1 << (11 + 1))   /**< Geographic Position (Bit 11). */
#define BT_PBAPC_VCARD_FILTER_TITLE     (1 << (12 + 1))   /**< Job (Bit 12). */
#define BT_PBAPC_VCARD_FILTER_ROLE      (1 << (13 + 1))   /**< Role within the Organization (Bit 13). */
#define BT_PBAPC_VCARD_FILTER_LOGO      (1 << (14 + 1))   /**< Organization Logo (Bit 14). */
#define BT_PBAPC_VCARD_FILTER_AGENT     (1 << (15 + 1))   /**< vCard of the Representing Person (Bit 15). */
#define BT_PBAPC_VCARD_FILTER_ORG       (1 << (16 + 1))   /**< Name of the Organization (Bit 16). */
#define BT_PBAPC_VCARD_FILTER_NOTE      (1 << (17 + 1))   /**< Comments (Bit 17). */
#define BT_PBAPC_VCARD_FILTER_REV       (1 << (18 + 1))   /**< Revision (Bit 18). */
#define BT_PBAPC_VCARD_FILTER_SOUND     (1 << (19 + 1))   /**< Pronunciation of the Name (Bit 19). */
#define BT_PBAPC_VCARD_FILTER_URL       (1 << (20 + 1))   /**< Uniform Resource Locator (Bit 20). */
#define BT_PBAPC_VCARD_FILTER_UID       (1 << (21 + 1))   /**< Unique ID (Bit 21). */
#define BT_PBAPC_VCARD_FILTER_KEY       (1 << (22 + 1))   /**< Public Encryption Key (Bit 22). */
#define BT_PBAPC_VCARD_FILTER_NICK      (1 << (23 + 1))   /**< Nickname (Bit 23). */
#define BT_PBAPC_VCARD_FILTER_CAT       (1 << (24 + 1))   /**< Categories (Bit 24). */
#define BT_PBAPC_VCARD_FILTER_PRODID    (1 << (25 + 1))   /**< Product ID (Bit 25). */
#define BT_PBAPC_VCARD_FILTER_CLASS     (1 << (26 + 1))   /**< Class Information (Bit 26). */
#define BT_PBAPC_VCARD_FILTER_SORT_STR  (1 << (27 + 1))   /**< Sort string (Bit 27). */
#define BT_PBAPC_VCARD_FILTER_TIMESTAMP (1 << (28 + 1))   /**< Timestamp (Bit 28). */
#define BT_PBAPC_VCARD_FILTER_TOTAL     (28 + 1)          /**< Reserved. */
#define BT_PBAPC_VCARD_FILTER_ALL       (0xffffffff)      /**< All. */

/**
 * @}
 */

/**
 * @defgroup bt_pbapc_enum Enum
 * @{
 */

/**
 * @brief This enum defines folder types.
 */
typedef enum {
    BT_PBAPC_FOLDER_TYPE_FORWARD  = 0,          /**< Set the child folder. */
    BT_PBAPC_FOLDER_TYPE_BACKWARD = 1,          /**< Set the parent folder. */
    BT_PBAPC_FOLDER_TYPE_ROOT     = 2,          /**< Set the root. */
} bt_pbapc_folder_type_t;

/**
 *  @brief This enum defines vCard format types.
 */
typedef enum {
    BT_PBAPC_VCARD_FORMAT_21 = 0x00,            /**< Version 2.1 format. */
    BT_PBAPC_VCARD_FORMAT_30 = 0x01             /**< Version 3.0 format. */
} bt_pbapc_vcard_format_t;

/**
 * @brief This enum defines sort order types.
 */
typedef enum {
    BT_PBAPC_SORT_ORDER_BY_INDEXED = 0x00,      /**< Indexed sorting. */
    BT_PBAPC_SORT_ORDER_BY_ALPHABETIC = 0x01,   /**< Alphabetical sorting. */
    BT_PBAPC_SORT_ORDER_BY_PHONETIC = 0x02,     /**< Phonetic sorting. */
    BT_PBAPC_SORT_ORDER_TOTAL
} bt_pbapc_sort_order_t;

/**
*  @brief This enum defines search attribute types.
*/
typedef enum {
    BT_PBAPC_SEARCH_ATTRIBUTE_NAME = 0x00,      /**< Name. */
    BT_PBAPC_SEARCH_ATTRIBUTE_NUMBER = 0x01,    /**< Number. */
    BT_PBAPC_SEARCH_ATTRIBUTE_SOUND = 0x02,     /**< Sound. */
    BT_PBAPC_SEARCH_ATTRIBUTE_TOTAL
} bt_pbapc_search_attribute_t;

/**
 * @}
 */

/**
 * @defgroup bt_pbapc_struct Struct
 * @{
 */

/**
 *  @brief This structure defines details of the pull phonebook request.
 */
typedef struct {
    uint16_t max_list_count;    /**< Maximum list count. */
    uint16_t list_start_offset; /**< List start offset. */
    uint32_t filter;            /**< vCard filter. */
    bt_pbapc_vcard_format_t format; /**< vCard format. */
} bt_pbapc_pull_phonebook_params_t;

/**
 *  @brief This structure defines details of the pull vCard-listing request.
 */
typedef struct {
    bt_pbapc_sort_order_t order;                    /**< Sort order. */
    bt_pbapc_search_attribute_t search_attribute;   /**< Search attribute. */
    uint8_t search_value_length;                    /**< Search value length. */
    uint16_t max_list_count;                        /**< Maximum list count. */
    uint16_t list_start_offset;                     /**< List start offset. */
    uint8_t search_value[MAX_BT_PBAPC_SEARCH_VALUE_LEN + 1]; /**< Search value. */
} bt_pbapc_pull_vcard_listing_t;

/**
 *  @brief This structure defines details of the pull vCard object request.
 */
typedef struct {
    uint32_t filter;            /**< vCard filter */
    bt_pbapc_vcard_format_t format; /**< vCard format */
} bt_pbapc_pull_vcard_entry_t;

/**
 * @brief This structure defines the #BT_PBAPC_CONNECT_CNF event result.
 */
typedef struct {
    uint16_t conn_id;           /**< Connection ID. */
    bt_address_t dev_addr;      /**< Bluetooth address of a remote device. */
    bt_status_t status;         /**< Connection result. */
} bt_pbapc_connect_cnf_t;

/**
 * @brief This structure defines the #BT_PBAPC_DISCONNECT_IND event result.
 */
typedef struct {
    uint16_t conn_id;           /**< Connection ID. */
    bt_status_t status;         /**< Disconnection result. */
} bt_pbapc_disconnect_ind_t;

/**
 * @brief This structure defines the #BT_PBAPC_SET_PHONEBOOK_CNF event result.
 */
typedef struct {
    uint16_t conn_id;           /**< Connection ID. */
    bt_status_t status;         /**< Set phonebook result. */
} bt_pbapc_set_phonebook_cnf_t;

/**
 * @brief This structure defines the #BT_PBAPC_PULL_PHONEBOOK_CNF event result.
 */
typedef struct {
    uint16_t conn_id;           /**< Connection ID. */
    bt_status_t status;         /**< Pull phonebook result. */
    uint16_t phone_book_size;   /**< 0xFFFF indicateing the parameter can be ignored. */
    uint16_t new_missed_calls;  /**< 0xFFFF indicateing the type is not MCH. */
} bt_pbapc_pull_phonebook_cnf_t;

/**
 * @brief This structure defines the #BT_PBAPC_PULL_VCARD_ENTRY_CNF event result.
 */
typedef struct {
    uint16_t conn_id;           /**< Connection ID. */
    bt_status_t status;         /**< Pull vCard object result. */
} bt_pbapc_pull_vcard_entry_cnf_t;

/**
 *  @brief This structure defines the #BT_PBAPC_PULL_VCARD_LISTING_CNF event result.
 */
typedef struct {

    uint16_t conn_id;           /**< Connection ID. */
    bt_status_t status;         /**< Pull vCard-listing result. */
    uint16_t phone_book_size;   /**< 0xFFFF indicateing the parameter can be ignored. */
    uint16_t new_missed_calls;  /**< 0xFFFF indicateing the type is not MCH. */
} bt_pbapc_pull_vcard_list_cnf_t;

/**
 *  @brief This structure defines the #BT_PBAPC_ABORT_CNF event result.
 */
typedef struct {
    uint16_t conn_id;           /**< Connection ID. */
    bt_status_t status;         /**< Abort result. */
} bt_pbapc_abort_cnf_t;

/**
 *  @brief This structure defines the #BT_PBAPC_DATA_IND event result.
 */
typedef struct {
    uint16_t conn_id;           /**< Connection ID. */
    uint32_t op_type;           /**< Current operation type. */
    uint8_t *data_buffer;       /**< Objects data. */
    uint16_t data_len;          /**< Data length. */
} bt_pbapc_data_ind_t;

/**
 *  @brief This structure defines the #BT_PBAPC_AUTH_RESPONSE_CNF event result.
 */
typedef struct {
    bt_address_t addr;          /**< Remote device's address. */
    bt_status_t status;         /**< AUTH response result. */
} bt_pbapc_auth_response_cnf_t;

/**
 *  @brief This structure defines the #BT_PBAPC_AUTH_IND event result.
 */
typedef struct {
    bt_address_t addr;          /**< Remote device's address. */
    uint8_t auth_option;        /**< Additional information about authentication challenge. */
} bt_pbapc_auth_challenge_ind_t;

/**
 * @}
 */

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief               This function connects to the specified remote device.
 *                      #BT_PBAPC_CONNECT_CNF event is sent to the upper layer with the connection result.
 * @param[in] dev_addr  is the Bluetooth address of the remote device.
 * @return              If the operation was successful, the returned value is #BT_STATUS_PBAPC_OK, otherwise the returned value is less than zero.
 * @sa                  #bt_pbapc_disconnect()
 */
bt_status_t bt_pbapc_connect(const bt_address_t *dev_addr);

/**
 * @brief               This function processes authentication in this connection.
 *                      #BT_PBAPC_AUTH_RESPONSE_CNF event is sent to the upper layer with the authentication response.
 * @param[in] addr      is the address of the remote device.
 * @param[in] password  is authentication password.
 * @param[in] user_id   is authentication user ID.
 * @param[in] cancel    is whether accept or reject the request of remote device's connection.
 * @return              If the operation was successful, the returned value is #BT_STATUS_PBAPC_OK, otherwise the returned value is less than zero.
 */
bt_status_t bt_pbapc_connect_auth_response(bt_address_t *dev_addr, uint8_t *password, uint8_t *user_id, uint8_t cancel);

/**
 * @brief               This function is disconnects from the specified remote device.
 *                      #BT_PBAPC_DISCONNECT_IND event is sent to the
 *                      upper layer with the result of the disconnection.
 * @param[in] conn_id   is the identifier of the remote device.
 * @return              If the operation was successful, the returned value is #BT_STATUS_PBAPC_OK, otherwise the returned value is less than zero.
 */
bt_status_t bt_pbapc_disconnect(uint16_t conn_id);

/**
 * @brief               This function downloads the phonebook objects from the specified remote device.
 *                      #BT_PBAPC_DATA_IND and #BT_PBAPC_PULL_PHONEBOOK_CNF events
 *                      is sent to the upper layer with the data and the result.
 * @param[in] conn_id   is the identifier of the remote device.
 * @param[in] name      is the absolute file name of the phonebook objects.
 * @param[in] param     is the details of the pull phonebook request.
 * @return              If the operation was successful, the returned value is #BT_STATUS_PBAPC_OK, otherwise the returned value is less than zero.
 */
bt_status_t bt_pbapc_pull_phonebook(uint16_t conn_id, uint8_t *name, bt_pbapc_pull_phonebook_params_t *param);

/**
 * @brief               This function changes the path used with browsing phonebook objects from specified remote device.
 *                      #BT_PBAPC_SET_PHONEBOOK_CNF event is sent to the upper layer with the result of the request.
 * @param[in] conn_id   is the identifier of the remote device.
 * @param[in] path_name is the path to the virtual folder.
 * @param[in] type      is the type of set path.
 * @return              If the operation was successful, the returned value is #BT_STATUS_PBAPC_OK, otherwise the returned value is less than zero.
 */
bt_status_t bt_pbapc_set_phonebook(uint16_t conn_id, uint8_t *path_name, bt_pbapc_folder_type_t type);

/**
 * @brief               This function pulls the vCard-listing of the current phonebook from the specified remote device.
 *                      #BT_PBAPC_DATA_IND and #BT_PBAPC_PULL_VCARD_LISTING_CNF events are sent to the upper layer with the data and the result.
 * @param[in] conn_id   is the identifier of the remote device.
 * @param[in] name      is the name of the folder without any path information.
 * @param[in] param     is the details of the pull vCard-listing request.
 * @return              If the operation was successful, the returned value is #BT_STATUS_PBAPC_OK, otherwise the returned value is less than zero.
 */
bt_status_t bt_pbapc_pull_vcard_listing(uint16_t conn_id, uint8_t *name, bt_pbapc_pull_vcard_listing_t *param);

/**
 * @brief               This function pulls a specific vCard object from the specified remote device.
 *                      #BT_PBAPC_DATA_IND and #BT_PBAPC_PULL_VCARD_ENTRY_CNF events are sent to the upper layer with the data and the result.
 * @param[in] conn_id   is the identifier of the remote device.
 * @param[in] name      is the name of the object.
 * @param[in] param     is the details of the pull vCard object request.
 * @return              If the operation was successful, the returned value is #BT_STATUS_PBAPC_OK, otherwise the returned value is less than zero.
 */
bt_status_t bt_pbapc_pull_vcard_entry(uint16_t conn_id, uint8_t *name, bt_pbapc_pull_vcard_entry_t *param);

/**
 * @brief               This function stops the current pull operations.
 *                      #BT_PBAPC_ABORT_CNF event is sent to the upper layer with the result.
 * @param[in] conn_id   is the identifier of the remote device.
 * @return              If the operation was successful, the returned value is #BT_STATUS_PBAPC_OK, otherwise the returned value is less than zero.
 */
bt_status_t bt_pbapc_pull_complete(uint16_t conn_id);

/**
 * @brief               This function gets the current PBAPC links number.
 * @return              The link number.
 */
uint8_t  bt_pbapc_get_link_num(void);

/**
 * @brief               This function is the common callback with indication or response result to the upper layer.
 * @param[in] event_id  is the related PBAPC event ID.
 * @param[in] param     is the result.
 * @return              None
 */
void bt_pbapc_common_callback(bt_event_t event_id, const void *param);

#ifdef __cplusplus
}
#endif

/**
 * @}
 * @}
 */

#endif /* __BT_PBAPC_H__ */

