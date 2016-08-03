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

#ifndef __BLE_SM_H__
#define __BLE_SM_H__

/**
 * @addtogroup BLE
 * @{
 * @addtogroup SM
 * @{
 * This section describes the SM APIs.
 * The Security Manager (SM) defines the protocol and behavior to manage pairing, authentication and
 * encryption between low energy devices.
 *
 * Terms and Acronyms
 * ======
 * |Terms                         |Details                                                                                                                              |
 * |------------------------------|-----------------------------------------------------------------------------------------------------------------------|
 * |\b LTK                        | Long Term Key, is a 128-bit key used to generate the contributory session key for an encrypted connection. |
 * |\b STK                        | Short Term Key, is a 128-bit key used to encrypt a connection following pairing. |
 * |\b EDIV                       | Encrypted Diversifier, is a 16-bit stored value used to identify the LTK distributed during the LE legacy pairing. |
 * |\b IRK                        | Identity Resolving Key, is a 128-bit key used to generate and resolve random addresses. |
 * |\b CSRK                       | Connection Signature Resolving Key, is a 128-bit key used to sign data and verify signatures on the receiving device. |
 * |\b MITM                       | Man-in-the-middle attack, is an attack where the attacker secretly relays and possibly alters the communication between two parties who believe they are directly communicating with each other. |
 * |\b Out-Of-Band                | The model is primarily designed for scenarios where an Out of Band mechanism is used to both discover the devices as well as to exchange or transfer cryptographic numbers used in the pairing process. |
 *
 * @section ble_sm_api_usage How to use this module
 *
 * - Step1: Mandatory, implement the API #ble_sm_common_callback() to handle the events sent by the SM.
 *  - Sample code:
 *     @code
 *        void ble_sm_common_callback(ble_event_t event_id, const void *param)
 *        {
 *            switch (event_id) {
 *                case BLE_SM_STORE_KEYS_REQ: {
 *                    //Store the keys into a flash memory or file system.
 *                }
 *                case BLE_SM_RETRIEVE_KEYS_REQ: {
 *                    //Get the keys from flash memory or file system then fill in the parameter.
 *                }
 *                default:
 *                    break;
 *            }
 *        }
 *     @endcode
 * - Step2: Mandatory, implement the API #ble_sm_get_io_capability(), SM will call this API to get the IO capability based on hardware features of the device. The value should be fixed in compile time.
 *  - Sample code:
 *     @code
 *        ble_sm_io_capability_t ble_sm_get_io_capability(void)
 *        {
 *            return BLE_SM_IO_CAPABILITY_NO_INPUT_NO_OUTPUT;
 *        }
 *     @endcode
 * - Step3: Optional, call #ble_sm_is_pairing_authorization_enabled() to check the old value, then call #ble_sm_set_pairing_authorization() to enable/disable pairing authorization.
 *          Once the pairing result is available, please reset to the old value.
 *  - Sample code:
 *     @code
 *        bool enabled;
 *        enabled = ble_sm_is_pairing_authorization_enabled();
 *        if (!enabled) {
 *            ble_sm_set_pairing_authorization(true);
 *        }
 *        //Reset the value after obtaining the pairing result.
 *        void ble_sm_common_callback(ble_event_t event_id, const void *param)
 *        {
 *            switch (event_id) {
 *                case BLE_SM_PAIRING_RESULT_IND: {
 *                    ble_sm_set_pairing_authorization(enabled);
 *                }
 *                default:
 *                    break;
 *        }
 *     @endcode
 * - Step4: Optional, call #ble_sm_bond() to pair with a remote device. This API performs the STK encryption procedure once the pairing is finished.
 *  - Sample code:
 *     @code
 *        ble_status_t status;
 *        ble_sm_pairing_attribute_t local_attributes = {true, true, false};//mitm, bond, oob
 *        U8 device_bt_addr[6] = {0x10, 0x06, 0x00, 0x86, 0x19, 0x00};
 *        ble_sm_set_pairing_authorization(true);
 *        status = ble_sm_bond((ble_address_t*)device_bt_addr, &local_attributes);
 *     @endcode
 * - Step5: Optional, call #ble_sm_encrypt() to only encrypt with a paired remote device using LTK. This is usually performed after reconnecting with the paired remote device.
 *  - Sample code:
 *     @code
 *        ble_status_t status;
 *        U8 device_bt_addr[6] = {0x10, 0x06, 0x00, 0x86, 0x19, 0x00};
 *        ble_sm_encrypt_keyset_t encrypt_keyset = {0};
 *
 *        encrypt_keyset.ediv = ediv; //EDIV is the key value of a remote device after bond.
 *        encrypt_keyset.rand = rand; //Rand is the key value of a remote device after bond.
 *        OS_MemCpy(encrypt_keyset.ltk, ltk, SM_LTK_VALUE_LENGTH); //LTK is the key value of remote device after bond.
 *        status = ble_sm_encrypt((ble_address_t*)device_bt_addr, &encrypt_keyset);
 *     @endcode
 */


#include <stdint.h>
#include <stdbool.h>

#include "ble_address.h"
#include "ble_status.h"
#include "ble_events.h"

/** @defgroup ble_sm_enum Enum
  * @{
  */

/**
 *  @brief Key type, is used to retrieve or store the SM keys.
 */
typedef enum {
    BLE_SM_KEY_LTK  = 0x01,
    BLE_SM_KEY_EDIV = 0x02,
    BLE_SM_KEY_RAND = 0x04,
    BLE_SM_KEY_IRK  = 0x08,
    BLE_SM_KEY_ADDRESS = 0x10,
    BLE_SM_KEY_CSRK = 0x20,
    BLE_SM_KEY_COUNTER = 0x40
} ble_sm_key_type_t;

/**
 *  @brief IO capability type, is fixed for each device.
 */
typedef enum {
    BLE_SM_IO_CAPABILITY_DISPLAY_ONLY,        /* 0x00 */
    BLE_SM_IO_CAPABILITY_DISPLAY_YES_NO,      /* 0x01 */
    BLE_SM_IO_CAPABILITY_KEYBOARD_ONLY,       /* 0x02 */
    BLE_SM_IO_CAPABILITY_NO_INPUT_NO_OUTPUT,  /* 0x03 */
    BLE_SM_IO_CAPABILITY_KEYBOARD_DISPLAY     /* 0x04 */
} ble_sm_io_capability_t;

/**
  * @}
  */



/** @defgroup ble_sm_struct Struct
  * @{
  */
/**
 *  @brief Call #ble_sm_reply_pairing_param() to provide the pairing attributes after receiving #BLE_SM_PAIRING_REQ.
 */
typedef struct {
    bool mitm;            /**< Indicate whether the device has requested MITM protection. */
    bool bonding_flag;    /**< Indicate whether there is a need to save the keys after pairing is finished. */
    bool oob_data_flag;   /**< Indicate whether out-of-band data authentication is available. */
} ble_sm_pairing_attribute_t;

/**
 *  @brief Substructure for #ble_sm_keyset_t.
 */
typedef struct {
    uint16_t ediv;                 /**< 16-bit EDIV used to identify the LTK, valid only if key_present contains BLE_SM_KEY_EDIV. */
    uint64_t rand;                 /**< 64-bit random number used to identify the LTK, valid only if key_present contains BLE_SM_KEY_RAND. */
    uint8_t ltk[16];               /**< 128-bit LTK/STK used to generate the contributory session key for an encrypted connection, valid only if key_present contains BLE_SM_KEY_LTK. */
    uint8_t encryption_key_size;   /**< Key size of LTK, min is 7 octets and max is 16 octets, valid only if key_present contains BLE_SM_KEY_LTK. */
    bool authenticated;            /**< Indicates whether the LTK is authenticated or not, for example, the LTK is unauthenticated if using Just Work pairing method, valid only if key_present contains BLE_SM_KEY_LTK. */
} ble_sm_encryption_info_t;

/**
 *  @brief Substructure for #ble_sm_keyset_t.
 */
typedef struct {
    uint8_t irk[16];    /**< 128-bit IRK used to generate and resolve random addresses, valid only if key_present contains BLE_SM_KEY_IRK. */
} ble_sm_identity_info_t;

/**
 *  @brief Substructure for #ble_sm_keyset_t.
 */
typedef struct {
    uint8_t csrk[16];         /**< 128-bit CSRK used to sign data and verify signature on the receiving device, valid only if key_present contains BLE_SM_KEY_CSRK. */
    uint32_t sign_counter;    /**< Counter used to sign data, increase for every message signed with a given CSRK, valid only if key_present contains BLE_SM_KEY_COUNTER. */
} ble_sm_signing_info_t;

/**
 *  @brief User shall store/provide the SM keys after receive #BLE_SM_STORE_KEYS_REQ or #BLE_SM_RETRIEVE_KEYS_REQ.
 */
typedef struct {
    ble_sm_key_type_t key_present;               /**< Bit mapping to indicate the valid field in the encryption_info/indentity_info/signing_info. */
    ble_sm_encryption_info_t encryption_info;    /**< Information to encrypt connections. */
    ble_sm_identity_info_t identity_info;        /**< Information to generate and resolve private resolvable random addresses. */
    ble_sm_signing_info_t signing_info;          /**< Information to sign data. */
} ble_sm_keyset_t;

/**
 *  @brief Call #ble_sm_encrypt() to provide the LTK relative keys to encrypt after reconnecting to a device.
 */
typedef struct {
    uint8_t ltk[16];  /**< 128-bit STK/LTK used to generate the contributory session key for an encrypted connection. */
    uint16_t ediv;    /**< 16-bit EDIV used to identify the LTK.*/
    uint64_t rand;    /**< 64-bit random number used to identify the LTK.*/
} ble_sm_encrypt_keyset_t;


/**
 *  @brief This event (#BLE_SM_PAIRING_REQ) structure indicates the SM security/pairing request is from a remote device.
 *         Provide the pairing attributes when #BLE_SM_PAIRING_REQ is received. For more information, please refer to #ble_sm_pairing_attribute_t.
 */
typedef struct {
    ble_address_t remote_address;    /**< For more details on the address of a remote device, please refer to #ble_address_t. */
} ble_sm_pairing_req_t;

/**
 *  @brief This event (#BLE_SM_PAIRING_RESULT_IND) structure indicates the SM pairing procedure is complete.
 *         User can get the pairing result from "result".
 */
typedef struct {
    ble_address_t remote_address;    /**< For more details on the address of a remote device, please refer to #ble_address_t. */
    bool result;                     /**< Pairing result, true means success while false means failure. */
} ble_sm_pairing_result_ind_t;

/**
 *  @brief This event (#BLE_SM_PAIRING_AUTHORIZATION_REQ) structure indicates the SM security request is from remote device and user should authorize if it's accepted or rejected.
 *         Notify the SM if the pairing needs to be enabled.
 */
typedef struct {
    ble_address_t remote_address;    /**< For more details on the address of a remote device, please refer to #ble_address_t. */
} ble_sm_pairing_authorization_req_t;

/**
 *  @brief This event (#BLE_SM_INPUT_PASSKEY_REQ) structure enables providing an input passkey.
 *         Provide the passkey to the SM after input or notify if it's cancelled.
 */
typedef struct {
    ble_address_t remote_address;    /**< For more details on the address of a remote device, please refer to #ble_address_t.*/
} ble_sm_input_passkey_req_t;

/**
 *  @brief This event (#BLE_SM_DISPLAY_PASSKEY_REQ) structure enables displaying a passkey.
 *         Provide the passkey to the SM after it's displayed or rejected. The passkey can be modified.
 */
typedef struct {
    ble_address_t remote_address;    /**< For more details on the address of a remote device, please refer to #ble_address_t. */
    uint32_t passkey;                /**< 6-bit decimal numeric passkey, the value is between 000000 and 999999. */
} ble_sm_display_passkey_req_t;

/**
 *  @brief This event (#BLE_SM_OOB_DATA_REQ) structure enables getting the out-of-band data.
 *         Notify the SM if out-of-band data is received or rejected.
 */
typedef struct {
    ble_address_t remote_address;    /**< For more details on the address of a remote device, please refer to #ble_address_t. */
} ble_sm_oob_data_req_t;

/**
 *  @brief This event (#BLE_SM_STORE_KEYS_REQ) structure enables to store the keys into the flash memory, such as file system.
 *         If remote_address is available, the keys are stored for each device, since they are different for each remote device.
 *         If remote_address is not available, only a single copy for a local device is stored, such as a local counter.
 *         The SM will query the user to store all keys except local IRK/CSRK/initial counter(0) keys, since these are provided by the user.
 */
typedef struct {
    ble_address_t remote_address;   /**< For more details on the address of a remote device, please refer to #ble_address_t. */
    ble_sm_keyset_t local_keyset;   /**< For more details on the keyset of local device, please refer to #ble_sm_keyset_t. */
    ble_sm_keyset_t remote_keyset;  /**< For more details on the keyset of remote device, please refer to #ble_sm_keyset_t. */
} ble_sm_store_keys_req_t;

/**
 *  @brief This event (#BLE_SM_RETRIEVE_KEYS_REQ) structure enables getting the keys stored in the flash memory, such as file system.
 *         If remote_address is available, the keys are stored for each device, since they are different for each remote device.
 *         If remote_address is not available, the keys are only for the local device, such as local IRK/CSRK/counter.
 *         Generate a unique local IRK/CSRK and set initial counter to 0, which will be retrieved by the SM.
 *         The SM updates the local counter with the new user provided counter.
 */
typedef struct {
    ble_address_t remote_address;   /**< For more details on the address of a remote device, please refer to #ble_address_t. */
    ble_sm_keyset_t local_keyset;   /**< For more details on the keyset of local device, please refer to #ble_sm_keyset_t. */
    ble_sm_keyset_t remote_keyset;  /**< For more details on the keyset of remote device, please refer to #ble_sm_keyset_t. */
} ble_sm_retrieve_keys_req_t;

/**
 *  @brief This event (#BLE_SM_ENCRYPT_RESULT_IND) structure indicates the SM encryption procedure is complete.
 *         User can get the encryption result from "result".
 */
typedef struct {
    ble_address_t remote_address;    /**< For more details on the address of a remote device, please refer to #ble_address_t. */
    ble_status_t encrypt_status;     /**< For more details on the encryption result, please refer to #ble_status_t, BLE_STATUS_SUCCESS means success, BLE_STATUS_PIN_OR_KEY_MISSING means encryption fail because remote device can't provide keys and user should invoke pairing by call #ble_sm_bond(). */
} ble_sm_encrypt_result_ind_t;

/**
  * @}
  */



#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief                       This function enables pairing as a peripheral or central role.
 * @param[in] remote_address    is the device address to bond.
 * @param[in] local_attributes  is the pair attributes to send a pair request.
 * @return                      If the operation completes successfully, the returned value is #BLE_STATUS_SUCCESS;
                                If the SM is busy, the returned value is #BLE_STATUS_GAP_BUSY. Try to bond later;
                                If pending, the returned value is #BLE_STATUS_GAP_PENDING, and will receive other events(#ble_event_t) later.
                                For more information about other errors, please refer to #ble_status_t.
 * @note                        Before using this function, please make sure ACL link is established.
 */
ble_status_t ble_sm_bond(const ble_address_t *remote_address, const ble_sm_pairing_attribute_t *local_attributes);


/**
 * @brief                       This function replies to the SM once #BLE_SM_PAIRING_REQ is received.
 * @param[in] remote_address    is the device address to bond.
 * @param[in] local_attributes  is the pair attributes to send a pair request/response.
 * @return                      None
 * @note                        Before using this function, please make sure ACL link is established.
 * @par                         Example
 * @code
 *        void ble_sm_common_callback(ble_event_t event_id, const void *param)
 *        {
 *            switch (event_id) {
 *                case BLE_SM_PAIRING_REQ: {
 *                    ble_sm_pairing_req_t *pairing_param = (ble_sm_pairing_req_t*)param;
 *                    ble_sm_pairing_attribute_t local_attributes = {true, true, false};//mitm, bond, oob
 *
 *                    ble_sm_reply_pairing_param(&pairing_param->remote_address, &local_attributes);
 *                    break;
 *                }
 *                default:
 *                    break;
 *            }
 *        }
 * @endcode
 */
void ble_sm_reply_pairing_param(const ble_address_t *remote_address, const ble_sm_pairing_attribute_t *local_attributes);

/**
 * @brief   This function verifies if an authorization is required.
 * @return  True means enable authorization procedure, while false means disable authorization procedure. Default value is false.
 * @note    It's a global attribute and once set, all are affected.
 */
bool ble_sm_is_pairing_authorization_enabled();

/**
 * @brief               Enable authorization procedure, if enable, user will receive #BLE_SM_PAIRING_AUTHORIZATION_REQ when get a pair/security request from peer device and ask if need authorization.
 * @param[in] enable    is true means enable authorization procedure, while false means disable authorization procedure. Default value is false.
 * @return              None.
 * @note                It's a global attribute and once set, all are affected.
 */
void ble_sm_set_pairing_authorization(bool enable);


/**
 * @brief                       This function replies to the SM once #BLE_SM_PAIRING_AUTHORIZATION_REQ is received.
 * @param[in] remote_address    is the device address to bond.
 * @param[in] authorized        is to accept or reject the pairing/slave request from peer device.
 * @return                      None.
 * @note                        Before using this function, please make sure ACL link is established.
 * @par                         Example
 * @code
 *        void ble_sm_common_callback(ble_event_t event_id, const void *param)
 *        {
 *            switch (event_id) {
 *                case BLE_SM_PAIRING_AUTHORIZATION_REQ: {
 *                    ble_sm_pairing_authorization_req_t *authorization_param = (ble_sm_pairing_authorization_req_t*)param;
 *
 *                    ble_sm_reply_pairing_authorization(&authorization_param->remote_address, true);
 *                    break;
 *                }
 *                default:
 *                    break;
 *            }
 *        }
 * @endcode
 */
void ble_sm_reply_pairing_authorization(const ble_address_t *remote_address, bool authorized);


/**
 * @brief                       This function replies to the SM once #BLE_SM_INPUT_PASSKEY_REQ is received.
 * @param[in] remote_address    is the device address to bond.
 * @param[in] accept            is to accept or reject the pairing procedure.
 * @param[in] passkey           is only valid when accept, set the passkey which user input here.
 * @return                      If the operation completes successfully, the returned value is #BLE_STATUS_SUCCESS, otherwise the returned value is less than zero.
 * @note                        Before using this function, please make sure the ACL link is established.
 * @par                         Example
 * @code
 *        void ble_sm_common_callback(ble_event_t event_id, const void *param)
 *        {
 *            switch (event_id) {
 *                case BLE_SM_INPUT_PASSKEY_REQ:
 *                {
 *                    ble_status_t status;
 *                    ble_sm_input_passkey_req_t *passkey_param = (ble_sm_input_passkey_req_t*)param;
 *                    uint32_t passkey = value; //Value is from user.
 *                    status = ble_sm_reply_input_passkey(&passkey_param->remote_address, true, passkey);
 *                    break;
 *                }
 *                default:
 *                    break;
 *            }
 *        }
 * @endcode
 */
ble_status_t ble_sm_reply_input_passkey(const ble_address_t *remote_address, bool accept, uint32_t passkey);


/**
 * @brief                       This function replies to the SM once #BLE_SM_DISPLAY_PASSKEY_REQ is received. If the application wishes to display a different passkey from the one suggested in
 *                              #BLE_SM_DISPLAY_PASSKEY_REQ, then can set the changed to TRUE and provide the new passkey.
 * @param[in] remote_address    is the device address to bond.
 * @param[in] changed           is to tell the SM if user use the passkey from #BLE_SM_DISPLAY_PASSKEY_REQ or make a new one.
 * @param[in] passkey           is only valid when changed, set the new passkey here.
 * @return                      If the operation completes successfully, the returned value is #BLE_STATUS_SUCCESS, otherwise the returned value is less than zero.
 * @note                        Before using this function, please make sure the ACL link is established.
 * @par                         Example
 * @code
 *        void ble_sm_common_callback(ble_event_t event_id, const void *param)
 *        {
 *            switch (event_id) {
 *                case BLE_SM_DISPLAY_PASSKEY_REQ: {
 *                    ble_status_t status;
 *                    ble_sm_display_passkey_req_t *passkey_param = (ble_sm_display_passkey_req_t*)param;
 *                    uint32_t passkey = value; //New passkey value.
 *                    status = ble_sm_reply_display_passkey(&passkey_param->remote_address, true, passkey);
 *                    break;
 *                }
 *                default:
 *                    break;
 *            }
 *        }
 * @endcode
 */
ble_status_t ble_sm_reply_display_passkey(const ble_address_t *remote_address, bool changed, uint32_t passkey);


/**
 * @brief                       This function replies to the SM once #BLE_SM_OOB_DATA_REQ is received. User might get data using the out-of-band mechanism, such as NFC.
 * @param[in] remote_address    is the device address to bond.
 * @param[in] accept            is to tell SM if user accept or reject the pairing procedure.
 * @param[in] oob_data          is only valid when accept, and it's 128-bit data.
 * @return                      If the operation completes successfully, the returned value is #BLE_STATUS_SUCCESS, otherwise the returned value is less than zero.
 * @note                        Before using this function, please make sure the ACL link is established.
 * @par                         Example
 * @code
 *        void ble_sm_common_callback(ble_event_t event_id, const void *param)
 *        {
 *            switch (event_id) {
 *                case BLE_SM_OOB_DATA_REQ: {
 *                    ble_status_t status;
 *                    ble_sm_oob_data_req_t *oob_param = (ble_sm_oob_data_req_t*)param;
 *                    uint8_t oob_data[16] = {1, 3, 5, 7, 9, 2, 4, 6, 8, 10, 88, 88, 88, 88, 88, 88};//Data from OOB mechanism.
 *                    status = ble_sm_reply_oob_data(&(oob_param->remote_address), true, oob_data);
 *                    break;
 *                }
 *                default:
 *                    break;
 *            }
 *        }
 * @endcode
 */
ble_status_t ble_sm_reply_oob_data(const ble_address_t *remote_address, bool accept, const uint8_t *oob_data);


/**
 * @brief                       Encrypt with the paired device after reconnect.
 * @param[in] remote_address    is the device address to encrypt.
 * @param[in] remote_keyset     is the keyset to encrypt.
 * @return                      If the operation completes successfully, the returned value is #BLE_STATUS_SUCCESS; if pending, the returned value is #BLE_STATUS_GAP_PENDING, otherwise the returned value is less than zero.
 * @note                        Before using this function, please make sure ACL link is established.
 */
ble_status_t ble_sm_encrypt(const ble_address_t *remote_address, ble_sm_encrypt_keyset_t *remote_keyset);


/**
 * @brief               This function is a static callback to capture the event. User defined implementation is enabled.
 * @param[in] event_id  is notification sent by the SM.
 * @param[in] param     is parameter pointer as per the events.
 * @return              None.
 */
void ble_sm_common_callback(ble_event_t event_id, const void *param);


/**
 * @brief   This function is to get the IO capability based on hardware features of the device. The SM will call it when Bluetooth powers on. User defined implementation is enabled.
 * @return  For more information about IO capability, please refer to #ble_sm_io_capability_t.
 */
ble_sm_io_capability_t ble_sm_get_io_capability(void);


#ifdef __cplusplus
}
#endif

/**
 * @}
 * @}
 */

#endif /* __BLE_SM_H__ */

