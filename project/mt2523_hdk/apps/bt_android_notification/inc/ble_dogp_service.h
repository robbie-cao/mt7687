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

/*****************************************************************************
 *
 * Description:
 * ------------
 * This file implements DOGP service service structures and functions
 *
 ****************************************************************************/

#ifndef __BLE_DOGP_SERVICE_H__
#define __BLE_DOGP_SERVICE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "ble_gatts_srv_common.h"
#include "ble_bds_app_util.h"
#include "ble_gatt.h"
#include "ble_gap.h"


/**
 * @brief DOGP service event type.
 */
typedef enum {
    BLE_DOGP_SERVICE_DOGP_SERVICE_READ_CHARACTERISTIC_EVT_CCCD_WRITE,    /**< DOGP service read characteristic CCCD write event. */
    BLE_DOGP_SERVICE_DOGP_SERVICE_READ_CHARACTERISTIC_EVT_CCCD_READ,     /**< DOGP service read characteristic CCCD read event. */
    BLE_DOGP_SERVICE_DOGP_SERVICE_READ_CHARACTERISTIC_EVT_READ,          /**< DOGP service read characteristic read event. */
    BLE_DOGP_SERVICE_DOGP_SERVICE_WRITE_CHARACTERISTIC_EVT_WRITE,        /**< DOGP service write characteristic write event. */
    BLE_DOGP_SERVICE_DOGP_SERVICE_WRITE_CHARACTERISTIC_EVT_EXCUTE_WRITE, /**< DOGP service write characteristic excute write event. */
} ble_dogp_service_evt_type_t;

/**
 * @brief Forward declaration of the ble_dogp_service_t type.
 */
typedef struct ble_dogp_service_s ble_dogp_service_t;

/**
 * @brief DOGP service read characteristic structure.
 */
typedef struct {
    ble_srv_utf8_str_t read__field;
} ble_dogp_service_dogp_service_read_characteristic_t;

/**
 * @brief DOGP service write characteristic structure.
 */
typedef struct {
    ble_srv_utf8_str_t write_field;
} ble_dogp_service_dogp_service_write_characteristic_t;

/**
 * @brief DOGP service Service event.
 */
typedef struct {
    ble_dogp_service_evt_type_t evt_type;                           /**< Type of event. */
    struct {
        uint16_t dogp_service_read_characteristic_cccd_value;       /**< Holds decoded data in Notify and Indicate event handler. */
        uint16_t dogp_service_read_characteristic_read_offset;      /**< offset for read long. */
        ble_dogp_service_dogp_service_write_characteristic_t dogp_service_write_characteristic; /**< Holds decoded data in Write event handler, not for write long. */
        uint8_t *dogp_service_write_characteristic_write_long_data; /**< for write long. */
        uint8_t dogp_service_write_characteristic_is_prepare;       /**< is prepare(TRUE) or not(FALSE). */
        uint16_t dogp_service_write_characteristic_write_offset;    /**< offset for write long. */
        uint16_t dogp_service_write_characteristic_write_length;    /**< the data length. */
        uint8_t dogp_service_write_characteristic_is_excute;        /**< is execute(TRUE) or cancel write(FALSE). */
    } params;
} ble_dogp_service_evt_t;

/**
 * @brief DOGP service Service event handler type.
 */
typedef int32_t(*ble_dogp_service_evt_handler_t) (ble_dogp_service_t *p_dogp_service, ble_dogp_service_evt_t *p_evt);

/**
 * @brief DOGP service Service init structure. This contains all options and data needed for initialization of the service
 */
typedef struct {
    ble_dogp_service_evt_handler_t     evt_handler; /**< Event handler to be called for handling events in the DOGP service Service. */
    ble_dogp_service_dogp_service_read_characteristic_t ble_dogp_service_dogp_service_read_characteristic_initial_value; /**< If not NULL, initial value of the DOGP service read characteristic characteristic. */
    ble_dogp_service_dogp_service_write_characteristic_t ble_dogp_service_dogp_service_write_characteristic_initial_value; /**< If not NULL, initial value of the DOGP service write characteristic characteristic. */
} ble_dogp_service_init_t;

/**
 * @brief DOGP service Service structure. This contains various status information for the service.
 */
struct ble_dogp_service_s {
    ble_dogp_service_evt_handler_t evt_handler; /**< Event handler to be called for handling events in the DOGP service Service. */
    uint16_t service_handle; /**< Handle of DOGP service Service (as provided by the BLE stack). */
    ble_gatts_char_handles_t dogp_service_read_characteristic_handles; /**< Handles related to the DOGP service read characteristic characteristic. */
    ble_gatts_char_handles_t dogp_service_write_characteristic_handles; /**< Handles related to the DOGP service write characteristic characteristic. */
    uint16_t conn_id; /**< Handle of the current connection (as provided by the BLE stack, is BLE_CONN_HANDLE_INVALID if not in a connection). */
};

/**
 * @brief Function for initializing the DOGP service.
 *
 * @param[out] p_dogp_service        DOGP service Service structure. This structure will have to be supplied by
 *                                   the application. It will be initialized by this function, and will later
 *                                   be used to identify this particular service instance.
 * @param[in]  p_dogp_service_init   Information needed to initialize the service.
 *
 * @return     0 means successful    initialization of service, otherwise an error code.
 */
ble_status_t ble_dogp_service_init(ble_dogp_service_t *p_dogp_service, const ble_dogp_service_init_t *p_dogp_service_init);

/**
 * @brief Function for handling the Application's BLE Stack events.
 */
void ble_dogp_service_on_ble_evt(ble_dogp_service_t *p_dogp_service, ble_evt_t *p_ble_evt);

/**
 * @brief Function for setting the DOGP service read characteristic.
 *
 * @details Sets a new value of the DOGP service read characteristic characteristic. The new value will be sent
 *          to the client by read characteristic response.
 *          This function is only generated if the characteristic's Read property is not 'Excluded'.
 *
 * @param[in] p_dogp_service                      DOGP service Service structure.
 * @param[in] p_dogp_service_read_characteristic  New DOGP service read characteristic.
 *
 * @return    0 on success, otherwise an error code.
 */
ble_status_t ble_dogp_service_dogp_service_read_characteristic_read_rsp(ble_dogp_service_t *p_dogp_service, ble_dogp_service_dogp_service_read_characteristic_t *p_dogp_service_read_characteristic, uint16_t offset);

/**
 * @brief Function for setting the DOGP service read characteristic.
 *
 * @details Sets a new value of the DOGP service read characteristic characteristic client configuration descriptor.
 *          The new value will be sent to the client by read response.
 *          This function is only generated if the characteristic's Read property is not 'Excluded'.
 *
 * @param[in]   p_dogp_service                      DOGP service Service structure.
 * @param[in]   p_dogp_service_read_characteristic  New DOGP service read characteristic.
 *
 * @return      0 on success, otherwise an error code.
 */
ble_status_t ble_dogp_service_dogp_service_read_characteristic_cccd_read_rsp(ble_dogp_service_t *p_dogp_service, uint16_t cccd_value);

/**
 * @brief Function for sending the DOGP service read characteristic.
 *
 * @details The application calls this function after having performed a dogp service read characteristic.
 *          The dogp service read characteristic data is encoded and sent to the client.
 *          This function is only generated if the characteristic's Notify or Indicate property is not 'Excluded'.
 *
 * @param[in]   p_dogp_service                      DOGP service Service structure.
 * @param[in]   p_dogp_service_read_characteristic  New dogp service read characteristic.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
ble_status_t ble_dogp_service_dogp_service_read_characteristic_send(ble_dogp_service_t *p_dogp_service, ble_dogp_service_dogp_service_read_characteristic_t *p_dogp_service_read_characteristic);


#ifdef __cplusplus
}
#endif

#endif /**_BLE_DOGP_SERVICE_H__*/



