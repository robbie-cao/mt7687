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
 * This file defines Immediate Alert service main structures and functions
 *
 ****************************************************************************/

#ifndef __BLE_IAS_H__
#define __BLE_IAS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "ble_gatt.h"
#include "ble_gap.h"
#include "ble_gatts_srv_common.h"
#include "ble_bds_app_util.h"


/**@brief Immediate Alert event type. */
typedef enum
{ 
    BLE_IAS_EVT_READ_MULTIPLE,              /**< ias read multiple event. */
    BLE_IAS_ALERT_LEVEL_EVT_WRITE,          /**< Alert Level write event. */
    BLE_IAS_ALERT_LEVEL_EVT_EXCUTE_WRITE,   /**< Alert Level excute write event. */
} ble_ias_evt_type_t;

/**@brief Forward declaration of the ble_ias_t type.*/
typedef struct ble_ias_s ble_ias_t;

typedef enum
{ 
    BLE_IAS_ALERT_LEVEL_NO_ALERT = 0, 
    BLE_IAS_ALERT_LEVEL_MILD_ALERT = 1, 
    BLE_IAS_ALERT_LEVEL_HIGH_ALERT = 2, 
} ble_ias_enum_alert_level_t; 

typedef struct
{
    ble_ias_enum_alert_level_t alert_level; 
} ble_ias_alert_level_alert_level_t; 

/**@brief Alert Level structure. */
typedef struct
{
    ble_ias_alert_level_alert_level_t alert_level;
} ble_ias_alert_level_t;

/**@brief Immediate Alert Service event. */
typedef struct
{
    uint16_t conn_id;                                                              /**< Handle of the current connection (is 0 if not in a connection). */
    ble_ias_evt_type_t evt_type;                                                   /**< Type of event. */
    struct {
        uint16_t *handle_list;                                                     /**< Characteristic handle list, for read multiple */
        ble_ias_alert_level_t alert_level;                                         /**< Holds decoded data in Write event handler, not for write long. */
        uint8_t *alert_level_write_long_data;                                      /**< Holds data only for write long. */
        uint8_t alert_level_is_prepare;                                            /**< Prepare write (TRUE) or not(FALSE). */
        uint16_t alert_level_write_offset;                                         /**< The offset for write long. */
        uint16_t alert_level_write_length;                                         /**< The data length of write value. */
        uint8_t alert_level_is_excute;                                             /**< Execute write(TRUE) or cancel(FALSE). */
    } params;
} ble_ias_evt_t;

/**@brief Immediate Alert Service event handler type. */
typedef void (*ble_ias_evt_handler_t) (ble_ias_t *p_ias, ble_ias_evt_t *p_evt);

/**@brief Immediate Alert Service init structure. This contains all options and data needed for initialization of the service */
typedef struct
{
    ble_ias_evt_handler_t     evt_handler;        /**< Event handler to be called for handling events in the Immediate Alert Service. */
} ble_ias_init_t;

/**@brief Immediate Alert Service structure. This contains various status information for the service.*/
struct ble_ias_s
{
    ble_ias_evt_handler_t evt_handler;            /**< Event handler to be called for handling events in the Immediate Alert Service. */
    uint16_t service_handle;                      /**< Handle of Immediate Alert Service (as provided by the BLE stack). */
    ble_gatts_char_handles_t alert_level_handles; /**< Handles related to the Alert Level characteristic. */
};

/**
 * @brief Function for initializing the Immediate Alert.
 *
 * @param[out] p_ias        Immediate Alert Service structure. This structure will have to be supplied by
 *                                       the application. It will be initialized by this function, and will later
 *                                       be used to identify this particular service instance.
 * @param[in]  p_ias_init   Information needed to initialize the service.
 *
 * @return     0 means successful        initialization of service, otherwise an error code.
 */
ble_status_t ble_ias_init(ble_ias_t *p_ias, const ble_ias_init_t *p_ias_init);

/**@brief Function for handling the Application's BLE Stack events.*/
void ble_ias_on_ble_evt(ble_ias_t *p_ias, ble_evt_t *p_ble_evt);

/**
 * @brief Function for response the ias read multiple characteristic event, if your app not care read multiple, please do not call this API.
 *
 * @details Sets a set of characteristic value of the ias. The values will be sent
 *          to the client by read multiple characteristic response.
 *
 * @param[in] p_ias                  Immediate Alert Service structure.
 * @param[in] uint8_t *data          data to send                 
 *
 * @return    0 on success, otherwise an error code.
 */
ble_status_t ble_ias_read_multiple_rsp(ble_ias_t *p_ias, uint16_t conn_id, uint16_t length, const uint8_t *data);



#ifdef __cplusplus
}
#endif
	
#endif /**_BLE_IAS_H__*/



