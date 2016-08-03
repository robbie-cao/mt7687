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
 * This file implements Link Loss service main function
 *
 ****************************************************************************/

#ifndef __BLE_LLS_H__
#define __BLE_LLS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "ble_gatts_srv_common.h"
#include "ble_bds_app_util.h"
#include "ble_gatt.h"
#include "ble_gap.h"


/**@brief Link Loss event type. */
typedef enum
{ 
    BLE_LLS_EVT_READ_MULTIPLE,            /**< lls read multiple event. */
    BLE_LLS_ALERT_LEVEL_EVT_READ,         /**< Alert Level read event. */
    BLE_LLS_ALERT_LEVEL_EVT_WRITE,        /**< Alert Level write event. */
    BLE_LLS_ALERT_LEVEL_EVT_EXCUTE_WRITE, /**< Alert Level excute write event. */
} ble_lls_evt_type_t;

/**@brief Forward declaration of the ble_lls_t type. */
typedef struct ble_lls_s ble_lls_t;

typedef enum
{ 
    BLE_LLS_ALERT_LEVEL_NO_ALERT = 0, 
    BLE_LLS_ALERT_LEVEL_MILD_ALERT = 1, 
    BLE_LLS_ALERT_LEVEL_HIGH_ALERT = 2, 
} ble_lls_enum_alert_level_t; 

typedef struct
{
    ble_lls_enum_alert_level_t alert_level; 
} ble_lls_alert_level_alert_level_t; 

/**@brief Alert Level structure. */
typedef struct
{
    ble_lls_alert_level_alert_level_t alert_level;
} ble_lls_alert_level_t;

/**@brief Link Loss Service event. */
typedef struct
{
    uint16_t conn_id;                           /**< Handle of the current connection (is 0 if not in a connection). */
    ble_lls_evt_type_t evt_type;                /**< Type of event. */
    struct {
        uint16_t *handle_list;                  /**< characteristic handle list, for read multiple */
        ble_lls_alert_level_t alert_level;      /**< Holds decoded data in Write event handler, not for write long. */
        uint8_t *alert_level_write_long_data;   /**< Holds data only for write long*/
        uint8_t alert_level_is_prepare;         /**< is prepare(TRUE) or not(FALSE)*/
        uint16_t alert_level_write_offset;      /**< offset for write long*/
        uint16_t alert_level_write_length;      /**< the data length of write value */
        uint8_t alert_level_is_excute;          /**< is execute(TRUE) or cancel write(FALSE)*/
        uint16_t alert_level_read_offset;       /**< offset for read long*/
    } params;
} ble_lls_evt_t;

/**@brief Link Loss Service event handler type. */
typedef void (*ble_lls_evt_handler_t) (ble_lls_t *p_lls, ble_lls_evt_t *p_evt);

/**@brief Link Loss Service init structure. This contains all options and data needed for initialization of the service */
typedef struct
{
    ble_lls_evt_handler_t     evt_handler;       /**< Event handler to be called for handling events in the Link Loss Service. */
} ble_lls_init_t;

/**@brief Link Loss Service structure. This contains various status information for the service.*/
struct ble_lls_s
{
    ble_lls_evt_handler_t evt_handler;           /**< Event handler to be called for handling events in the Link Loss Service. */
    uint16_t service_handle;                     /**< Handle of Link Loss Service (as provided by the BLE stack). */
    ble_gatts_char_handles_t alert_level_handles;/**< Handles related to the Alert Level characteristic. */
};

/**
 * @brief Function for initializing the Link Loss.
 *
 * @param[out] p_lls              Link Loss Service structure. This structure will have to be supplied by
 *                                the application. It will be initialized by this function, and will later
 *                                be used to identify this particular service instance.
 * @param[in]  p_lls_init         Information needed to initialize the service.
 *
 * @return     0 means successful initialization of service, otherwise an error code.
 */
ble_status_t ble_lls_init(ble_lls_t *p_lls, const ble_lls_init_t *p_lls_init);

/**@brief Function for handling the Application's BLE Stack events.*/
void ble_lls_on_ble_evt(ble_lls_t *p_lls, ble_evt_t *p_ble_evt);

/**
 * @brief Function for response the Alert Level read event.
 *
 * @details Sets a new value of the Alert Level characteristic. The new value will be sent
 *          to the client by read characteristic response.
 *          This function is only generated if the characteristic's Read property is not 'Excluded'.
 *
 * @param[in] p_lls          Link Loss Service structure.
 * @param[in] p_alert_level  New Alert Level.
 * @param[in] conn_id        Connection ID.
 *
 * @return    0 on success, otherwise an error code.
 */
ble_status_t ble_lls_alert_level_read_rsp(ble_lls_t *p_lls, ble_lls_alert_level_t *p_alert_level, uint16_t conn_id, uint16_t offset);


/**
 * @brief Function for response the lls read multiple characteristic event, if your app not care read multiple, please do not call this API.
 *
 * @details Sets a set of characteristic value of the lls. The values will be sent
 *          to the client by read multiple characteristic response.
 *
 * @param[in] p_lls          Link Loss Service structure.
 * @param[in] uint8_t *data  data to send                 
 *
 * @return    0 on success, otherwise an error code.
 */
ble_status_t ble_lls_read_multiple_rsp(ble_lls_t *p_lls, uint16_t conn_id, uint16_t length, const uint8_t *data);

#ifdef __cplusplus
}
#endif
	
#endif /**_BLE_LLS_H__*/





