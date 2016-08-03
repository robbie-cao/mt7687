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
 * This file implements Tx Power service main function
 *
 ****************************************************************************/

#ifndef __BLE_TPS_H__
#define __BLE_TPS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "ble_gatts_srv_common.h"
#include "ble_bds_app_util.h"
#include "ble_gatt.h"
#include "ble_gap.h"


/**@brief Tx Power event type. */
typedef enum
{ 
    BLE_TPS_EVT_READ_MULTIPLE,               /**< tps read multiple event. */
    BLE_TPS_TX_POWER_LEVEL_EVT_READ,         /**< Tx Power Level read event. */
} ble_tps_evt_type_t;

/**@brief Forward declaration of the ble_tps_t type.*/
typedef struct ble_tps_s ble_tps_t;

/**@brief Tx Power Level structure. */
typedef struct
{
    int8_t tx_power;
} ble_tps_tx_power_level_t;

/**@brief Tx Power Service event. */
typedef struct
{
    uint16_t conn_id;                        /**< Handle of the current connection (is 0 if not in a connection). */
    ble_tps_evt_type_t evt_type;             /**< Type of event. */
    struct {
        uint16_t *handle_list;               /**< characteristic handle list, for read multiple */
        uint16_t tx_power_level_read_offset; /**< offset for read long*/
    } params;
} ble_tps_evt_t;

/**@brief Tx Power Service event handler type. */
typedef void (*ble_tps_evt_handler_t) (ble_tps_t *p_tps, ble_tps_evt_t *p_evt);

/**@brief Tx Power Service init structure. This contains all options and data needed for initialization of the service */
typedef struct
{
    ble_tps_evt_handler_t     evt_handler;   /**< Event handler to be called for handling events in the Tx Power Service. */
} ble_tps_init_t;

/**@brief Tx Power Service structure. This contains various status information for the service.*/
struct ble_tps_s
{
    ble_tps_evt_handler_t evt_handler;               /**< Event handler to be called for handling events in the Tx Power Service. */
    uint16_t service_handle;                         /**< Handle of Tx Power Service (as provided by the BLE stack). */
    ble_gatts_char_handles_t tx_power_level_handles; /**< Handles related to the Tx Power Level characteristic. */
};

/**
 * @brief Function for initializing the Tx Power.
 *
 * @param[out] p_tps               Tx Power Service structure. This structure will have to be supplied by
 *                                 the application. It will be initialized by this function, and will later
 *                                 be used to identify this particular service instance.
 * @param[in]  p_tps_init          Information needed to initialize the service.
 *
 * @return     0 means successful  initialization of service, otherwise an error code.
 */
ble_status_t ble_tps_init(ble_tps_t *p_tps, const ble_tps_init_t *p_tps_init);

/**@brief Function for handling the Application's BLE Stack events.*/
void ble_tps_on_ble_evt(ble_tps_t *p_tps, ble_evt_t *p_ble_evt);

/**
 * @brief Function for response the Tx Power Level read event.
 *
 * @details Sets a new value of the Tx Power Level characteristic. The new value will be sent
 *          to the client by read characteristic response.
 *          This function is only generated if the characteristic's Read property is not 'Excluded'.
 *
 * @param[in] p_tps             Tx Power Service structure.
 * @param[in] p_tx_power_level  New Tx Power Level.
 * @param[in] conn_id           Connection ID.
 *
 * @return    0 on success, otherwise an error code.
 */
ble_status_t ble_tps_tx_power_level_read_rsp(ble_tps_t *p_tps, ble_tps_tx_power_level_t *p_tx_power_level, uint16_t conn_id, uint16_t offset);


/**
 * @brief Function for response the tps read multiple characteristic event, if your app not care read multiple, please do not call this API.
 *
 * @details Sets a set of characteristic value of the tps. The values will be sent
 *          to the client by read multiple characteristic response.
 *
 * @param[in] p_tps             Tx Power Service structure.
 * @param[in] uint8_t *data     data to send                       
 *
 * @return    0 on success, otherwise an error code.
 */
ble_status_t ble_tps_read_multiple_rsp(ble_tps_t *p_tps, uint16_t conn_id, uint16_t length, const uint8_t *data);

#ifdef __cplusplus
}
#endif
	
#endif /**_BLE_TPS_H__*/





