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
 * This file implements gatt server profile common function
 *
 ****************************************************************************/
#include "ble_gatts_srv_common.h"
#include "ble_gatt.h"
#include "ble_gap.h"
#include "ble_status.h"
#include "ble_app_utils.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "syslog.h"

/************************************************
*   static params 
*************************************************/
static ble_gap_adv_interval_enum_t g_ble_gap_curr_adv = BLE_GAP_ADV_INTERVAL_DEFAULT;
static ble_gap_conn_param_priority_t g_ble_gap_curr_conn_param = BLE_GAP_CONN_PARAM_PRIORITY_DEFAULT;
const uint8_t ble_base_uuid[16] = 
{
    0xFB, 0x34, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80,
    0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


/************************************************
*   static utilities
*************************************************/
static uint8_t ble_me_util_write_eir_name(uint8_t *buf, uint8_t bufLen, const uint8_t *name, uint8_t nameLen);
static uint8_t ble_me_util_write_eir_ad_flag(uint8_t *buf, uint8_t bufLen, ble_advertise_flags_t flags);
static uint8_t ble_me_util_write_eir_appearance(uint8_t *buf, uint8_t bufLen, uint16_t appearance);
static uint8_t ble_me_util_write_eir_service_list(uint8_t *buf, uint8_t bufLen, service_uuid_t *service);
static uint8_t ble_me_util_write_eir_manu_data(uint8_t *buf, uint8_t bufLen, adv_manufacturer_data_t *manu);
static uint8_t ble_me_util_write_eir_txpower(uint8_t *buf, uint8_t bufLen, int8_t dBm);
static uint8_t ble_me_util_write_eir_internal_range(uint8_t *buf, uint8_t bufLen, slave_conn_interval_range_t *value);
static uint8_t ble_me_util_write_eir_servie_data(uint8_t *buf, uint8_t bufLen, uint16_t mask, service_data_t *data);
static uint8_t ble_util_compose_advertise_data(uint8_t *buf, uint32_t bufLen, ble_adv_data_type_mask_t mask, ble_adv_data_type_t *advData);
static ble_status_t ble_util_update_advertise_data(ble_adv_data_type_mask_t advMask, ble_adv_data_type_t *advData);

log_create_module(GATTS_SRV, PRINT_LEVEL_INFO);

/************************************************
*   Functions
*************************************************/
/**
 * @brief Function for get the MTU size.
 *
 * @param[in]  conn_id	 connection id.
 * 
 */
uint16_t ble_gatts_get_mtu_size(uint16_t conn_id)
{
    uint16_t mtu_size;
    ble_gatt_get_mtu_size(conn_id, &mtu_size);
    return mtu_size;
}

/**
 * @brief Function for convert 16-bits UUID to 128-bits.
 *
 * @param[in]  *uuid16	         16-bits UUID.
 * @param[in]  *uuid128_base	 128-bits GATT Base UUID.
 *
 * @param[out] *uuid128          128-bits UUID.
 */
void ble_gatt_uuid128_generate(uint8_t *uuid128, uint8_t *uuid128_base, const uint8_t *uuid16)
{
    if (uuid128 && uuid16) {
        if (uuid128_base) {
            memcpy(uuid128, uuid128_base, 16);
        } else {
	    memcpy(uuid128, ble_base_uuid, 16);
	}

    	uuid128[13] = uuid16[1];
    	uuid128[12] = uuid16[0];	
    }
}

/**
 * @brief Function for alloc memory.
 *
 * @param[in]  size	 memory size.
 * 
 * @param[out] the point of memory.
 */
void *ble_gatts_alloc_memory(unsigned int size)
{
#ifndef WIN32
    return pvPortMalloc(size);
#else
    return malloc(size);
#endif
}

/**
 * @brief Function for free memory.
 *
 * @param[in]  *pv	 the point of memory to free.
 * 
 */
void ble_gatts_free_memory(void *pv)
{
#ifndef WIN32
    vPortFree(pv);
#else
    free(pv);
#endif
}

uint8_t ble_me_util_write_eir_name(uint8_t *buf, uint8_t bufLen, const uint8_t *name, uint8_t nameLen)
{
    uint8_t offset = 0;

    if (name && buf && bufLen > 2) {
        nameLen = MIN(nameLen, strlen((const char*)name));
        if (nameLen) {
            if (nameLen > bufLen - 2) {
                buf[offset] = bufLen - 1;
                offset++;
                buf[offset++] = BLE_EIR_NAME_SHORTENED;
                memcpy(buf + offset, name, bufLen - offset);
                offset = bufLen;
            } else {
                buf[offset++] = nameLen + 1;
                buf[offset++] = BLE_EIR_NAME_COMPLETE;
                memcpy(buf + offset, name, nameLen);
                offset += nameLen;
            }
        }
    }
    return offset;
}

uint8_t ble_me_util_write_eir_ad_flag(uint8_t *buf, uint8_t bufLen, ble_advertise_flags_t flags)
{
    uint8_t offset = 0;
	
    buf[offset++] = 2;
    buf[offset++] = BLE_EIR_FLAGS;
    buf[offset++] = flags;

    return offset;
}

uint8_t ble_me_util_write_eir_appearance(uint8_t *buf, uint8_t bufLen, uint16_t appearance)
{
    uint8_t offset = 0;

    buf[offset++] = 3;
    buf[offset++] = BLE_EIR_APPEARANCE;
    le_to_store_host16(&buf[offset], appearance);
    offset+=2;
	
    return offset;
}

uint8_t ble_me_util_write_eir_service_list(uint8_t *buf, uint8_t bufLen, service_uuid_t *service)
{
    uint8_t offset = 0, index;
    uint8_t len = 0;
    uint8_t uuid_list[BLE_MAX_EIR_SDP_SIZE * 2];
    uint8_t uuid16_num = service->uuid16.serviceNum;
    uint8_t uuid32_num = service->uuid32.serviceNum;
    uint8_t uuid128_num = service->uuid128.serviceNum;
    uint8_t *uuid16_ptr = (uint8_t*)service->uuid16.uuidList;

    /** UUID service order: 16 uuid block | 32 uuid block | 128 uuid block */
    LOG_I(GATTS_SRV, "[BLE] BLE service list: 16uuid[%d], 32uuid[%d], 128uuid[%d] \r\n", uuid16_num, uuid32_num, uuid128_num);
    if (uuid16_num && uuid16_ptr && bufLen >= 4) {
        for (index = 0; index < uuid16_num; index++) {
            le_to_store_host16(&uuid_list[len], be_to_retrieve_host16(uuid16_ptr + len));
            len += 2;
        }

        if (len + 2 > bufLen) {
            /** EIR UUID is not completed */
            len = bufLen - 2;
            buf[1] = BLE_EIR_SERVICE_CLASS_16UUID_MORE;
        } else {
            /** EIR UUID is completed */
            buf[1] = BLE_EIR_SERVICE_CLASS_16UUID_COMPLETE;
        }
        
        buf[0] = len + 1;
        offset += 2;
        memcpy(buf + offset, uuid_list, len);
        offset += len;
    }    
    return offset;
}

uint8_t ble_me_util_write_eir_manu_data(uint8_t *buf, uint8_t bufLen, adv_manufacturer_data_t *manu)
{
    uint8_t offset = 0;

    if (bufLen < manu->manuDataLen) {
        return offset;
    }
    buf[offset++] = 1 + manu->manuDataLen;
    buf[offset++] = BLE_EIR_MANUFACTURER;
    memcpy(buf + offset, manu->manuData, manu->manuDataLen);
    offset += manu->manuDataLen;
    return offset;
}

uint8_t ble_me_util_write_eir_txpower(uint8_t *buf, uint8_t bufLen, int8_t dBm)
{
    uint8_t offset = 0;

    buf[offset++] = 2;
    buf[offset++] = BLE_EIR_TX_POWER;
    buf[offset++] = (uint8_t)dBm;   /** need consider conversion. */
    return offset;
}

uint8_t ble_me_util_write_eir_internal_range(uint8_t *buf, uint8_t bufLen, slave_conn_interval_range_t *value)
{
    uint8_t offset = 0;
    uint32_t interval = 0;

    /** Spec Min ConnInterval: 0x0006 ~ 0x0C80, *1.25ms. */
    /** Spec Max ConnInterval: 0x0006 ~ 0x0C80, *1.25ms. */
    if (value->minInterval < 6 || 
        value->minInterval > 3200 ||
        value->maxInterval < 6 ||
        value->maxInterval > 3200) {
        return offset;
    }
        
    buf[offset++] = 5;
    buf[offset++] = BLE_EIR_SLAVE_CONNINT_RANGE;
    interval = value->minInterval << 16;
    interval |= value->maxInterval;
    le_to_store_host32(&buf[offset], interval);
    offset += 4;

    return offset;
}

uint8_t ble_me_util_write_eir_servie_data(uint8_t *buf, uint8_t bufLen, uint16_t mask, service_data_t *data)
{
    uint8_t offset = 0;

    /** Consider each UUID data only one instance */
    if (mask & BLE_ADV_DATA_TYPE_SERVICE_DATA_16UUID) {
        if (data->uuid16.uuidDataLen > 0 && 
            data->uuid16.uuidDataLen < bufLen &&
            data->uuid16.uuidData != NULL) {
            buf[offset++] = 1 + data->uuid16.uuidDataLen;
            buf[offset++] = BLE_EIR_SERVICE_DATA_16UUID;
            memcpy(buf + offset, data->uuid16.uuidData, data->uuid16.uuidDataLen);
            offset += data->uuid16.uuidDataLen;
        }
    }
    
    if (mask & BLE_ADV_DATA_TYPE_SERVICE_DATA_32UUID) {
        if (data->uuid32.uuidDataLen > 0 && 
            data->uuid32.uuidDataLen < bufLen &&
            data->uuid32.uuidData != NULL) {
            buf[offset++] = 1 + data->uuid32.uuidDataLen;
            buf[offset++] = BLE_EIR_SERVICE_DATA_32UUID;
            memcpy(buf + offset, data->uuid32.uuidData, data->uuid32.uuidDataLen);
            offset += data->uuid32.uuidDataLen;
        }
    }    
    
    if (mask & BLE_ADV_DATA_TYPE_SERVICE_DATA_128UUID) {
        if (data->uuid128.uuidDataLen > 0 && 
            data->uuid128.uuidDataLen < bufLen &&
            data->uuid128.uuidData != NULL) {
            buf[offset++] = 1 + data->uuid128.uuidDataLen;
            buf[offset++] = BLE_EIR_SERVICE_DATA_32UUID;
            memcpy(buf + offset, data->uuid128.uuidData, data->uuid128.uuidDataLen);
            offset += data->uuid128.uuidDataLen;
        }
    }          
    return offset;
}


uint8_t ble_util_compose_advertise_data(uint8_t *buf, uint32_t bufLen, ble_adv_data_type_mask_t mask, ble_adv_data_type_t *advData)
{
    uint8_t offset = 0;

    LOG_I(GATTS_SRV, "[BLE] Compose ADV Data, mask:0x%04x \r\n", mask);
	
    memset(buf, 0x00, BLE_ADV_DATA_SIZE);
    
    /** Flags */
    if (mask & BLE_ADV_DATA_TYPE_FLAGS) {
	offset += ble_me_util_write_eir_ad_flag(buf + offset, BLE_ADV_DATA_SIZE - offset, advData->flag);
        LOG_I(GATTS_SRV, "[BLE] Use ADType flags:0x%x \r\n", advData->flag);
    }

    /** Local name */
    if (mask & BLE_ADV_DATA_TYPE_LOCAL_NAME) {
        LOG_I(GATTS_SRV, "[BLE] Use Local name,len:%d, name:%s \r\n", advData->name.nameLen, advData->name.localName);
        /** maybe no dev name from caller, stack need set default devname. */
        if (advData->name.nameLen != 0) {
            offset += ble_me_util_write_eir_name(buf + offset, 
                                                 BLE_ADV_DATA_SIZE - offset, 
                                                 advData->name.localName,
                                                 advData->name.nameLen);
        } else {
            offset += ble_me_util_write_eir_name(buf + offset, 
                                                 BLE_ADV_DATA_SIZE - offset, 
                                                 NULL,/** default name */
                                                 0);            
        }	
	  }

    /** Appearance */
    if (mask & BLE_ADV_DATA_TYPE_APPEARANCE) {
        offset += ble_me_util_write_eir_appearance(buf + offset, BLE_ADV_DATA_SIZE - offset, advData->appearance);
        LOG_I(GATTS_SRV, "[BLE] Use appearance:0x%x \r\n", advData->appearance);
    }

    /** current support 16 bit Service */
    if (mask & BLE_ADV_DATA_TYPE_SERVICE_UUID) {
        offset += ble_me_util_write_eir_service_list(buf + offset, BLE_ADV_DATA_SIZE - offset, &(advData->service));
    }

    /** Manufacturer specific data */
    if (mask & BLE_ADV_DATA_TYPE_MANU_DATA) {
        offset += ble_me_util_write_eir_manu_data(buf + offset, BLE_ADV_DATA_SIZE - offset, &(advData->manu));
        LOG_I(GATTS_SRV, "[BLE] Use Manu data[%d][%s] \r\n", advData->manu.manuDataLen, advData->manu.manuData);
    }

    /** Slave connection interval range */
    if (mask & BLE_ADV_DATA_TYPE_SLAVE_CONN_INTERVAL) {
        offset += ble_me_util_write_eir_internal_range(buf + offset, BLE_ADV_DATA_SIZE - offset, &(advData->conn_int_range));
        LOG_I(GATTS_SRV, "[BLE] Use connInterval min:%02x, max:%02x \r\n", 
               advData->conn_int_range.minInterval,
               advData->conn_int_range.maxInterval);
    }    

    /** Tx power level */
    if (mask & BLE_ADV_DATA_TYPE_TX_POWER_LEVEL) {
        offset += ble_me_util_write_eir_txpower(buf + offset, BLE_ADV_DATA_SIZE - offset, advData->txpower);
        LOG_I(GATTS_SRV, "[BLE] Use Txpower:%d \r\n", advData->txpower);
    }

    /** Service Data */
    if ((mask & BLE_ADV_DATA_TYPE_SERVICE_DATA_16UUID) ||
        (mask & BLE_ADV_DATA_TYPE_SERVICE_DATA_32UUID) ||
        (mask & BLE_ADV_DATA_TYPE_SERVICE_DATA_128UUID)) {
        offset += ble_me_util_write_eir_servie_data(buf + offset, BLE_ADV_DATA_SIZE - offset, mask, &(advData->service_data));
        LOG_I(GATTS_SRV, "[BLE] Use 16 service data, len:%d \r\n", advData->service_data.uuid16.uuidDataLen);
        LOG_I(GATTS_SRV, "[BLE] Use 32 service data, len:%d \r\n", advData->service_data.uuid32.uuidDataLen);
        LOG_I(GATTS_SRV, "[BLE] Use 128 service data, len:%d \r\n", advData->service_data.uuid128.uuidDataLen);
    }
    
    return offset;
}

ble_status_t ble_util_update_advertise_data(ble_adv_data_type_mask_t advMask, ble_adv_data_type_t *advData)
{
    uint8_t pdu[BLE_ADV_DATA_SIZE];
    uint8_t len;

 
    len = ble_util_compose_advertise_data(pdu, 
                                          BLE_ADV_DATA_SIZE,
                                          advMask, 
                                          advData);

    ble_gap_set_advertising_data((const void *) pdu,
		                 (uint16_t) len,
		                 (const void *) pdu,
		                 (uint16_t) len);

    return BLE_STATUS_SUCCESS;
}

/**
 * @brief Function for setting advertising data.
 *
 * @param[in]  req	 the data structures of advertising data.
 *
 * @param[out] status    result of setting advertising data request.
 */
ble_status_t ble_gap_set_advertising_data_reqest(bt_gap_set_adv_data_req_struct_t *req)
{
    ble_status_t status = BLE_STATUS_SUCCESS;
    ble_adv_data_type_mask_t mask = 0;
    ble_adv_data_type_t data;	  
 
    memset((uint8_t *)&data, 0, sizeof(ble_adv_data_type_t));
    if (!req->set_scan_rsp) {
        mask |= BLE_ADV_DATA_TYPE_FLAGS;
        data.flag = BEF_LE_GENERAL_DISCOVERABLE_MODE;
    }
 
    if (req->include_name) {
        mask |= BLE_ADV_DATA_TYPE_LOCAL_NAME;
    }
 
    if (req->include_txpower) {
        mask |= BLE_ADV_DATA_TYPE_TX_POWER_LEVEL;
    }
 
    if ((req->min_interval > 0) && (req->max_interval > 0) && (req->max_interval > req->min_interval)) {
        mask |= BLE_ADV_DATA_TYPE_SLAVE_CONN_INTERVAL;	
        data.conn_int_range.minInterval = req->min_interval;
        data.conn_int_range.maxInterval = req->max_interval;
    }
 
    if (req->appearance != 0) {
        mask |= BLE_ADV_DATA_TYPE_APPEARANCE; 	 
        data.appearance = req->appearance;
    }
 
    if ((req->manufacturer_len > 0) && (req->manufacturer_data != NULL)) {
        mask |= BLE_ADV_DATA_TYPE_MANU_DATA;		
        data.manu.manuDataLen = req->manufacturer_len;
        data.manu.manuData = (uint8_t *)req->manufacturer_data;
    }
 
    if ((req->service_data_len > 0) && (req->service_data != NULL)) { 
        mask |= BLE_ADV_DATA_TYPE_SERVICE_DATA_16UUID; 
        data.service_data.uuid16.uuidDataLen = req->service_data_len;
        data.service_data.uuid16.uuidData = (uint8_t *)req->service_data;		  
    }
 
    if ((req->service_uuid_len > 0) && (req->service_uuid != NULL)) { 
        mask |= BLE_ADV_DATA_TYPE_SERVICE_UUID;	  
        data.service.uuid16.serviceNum = (req->service_uuid_len / 2);
        data.service.uuid16.uuidList = (uint16_t *)req->service_uuid;		  
    }
 
    status = ble_util_update_advertise_data(mask, &data);
    return status;

}

/**
 * @brief Function for updating connection params to client.
 *
 * @param[in]  conn_id	 connection id.
 * @param[in]  app_name	 application name, who trigger the request.
 *
 * @param[out] status    result of updating connection params request.
 */
ble_status_t ble_gap_update_connection_interval(uint16_t conn_id, uint8_t *app_name, ble_gap_conn_param_priority_t conn_priority)
{
    ble_status_t status;
    ble_gap_connection_params_t conn_params;
    if (g_ble_gap_curr_conn_param == conn_priority) {
        return BLE_STATUS_GAP_FAILED;
    }
    g_ble_gap_curr_conn_param = conn_priority;
    conn_params.conn_timeout = 0x0258;            /** TBC: 6000ms : 600 * 10 ms. */

    switch (conn_priority) {
        case BLE_GAP_CONN_PARAM_HIGH_SPEED_ANDROID:
        {
            conn_params.min_conn_interval = 0x0010;/** TBC: 20ms : 16 * 1.25 ms. */
            conn_params.max_conn_interval = 0x0010;/** TBC: 20ms : 16 * 1.25 ms. */
            conn_params.conn_latency = 0;
        }
            break;
        case BLE_GAP_CONN_PARAM_HIGH_SPEED_IOS:
        {
            conn_params.min_conn_interval = 0x0010;/** TBC: 20ms : 16 * 1.25 ms. */
            conn_params.max_conn_interval = 0x0020;/** TBC: 40ms : 32 * 1.25 ms. */
            conn_params.conn_latency = 0;
        }
            break;
        case BLE_GAP_CONN_PARAM_LOW_POWER:
        case BLE_GAP_CONN_PARAM_PRIORITY_DEFAULT:
        {
            conn_params.min_conn_interval = 0x0120;/** TBC: 360ms : 288 * 1.25 ms. */
            conn_params.max_conn_interval = 0x0130;/** TBC: 380ms : 304 * 1.25 ms. */
            conn_params.conn_latency = 4;
        }
            break;
        default:
        {
            conn_params.min_conn_interval = 0x0120;/*TBC: 360ms : 288 * 1.25 ms*/
            conn_params.max_conn_interval = 0x0130;/*TBC: 380ms : 304 * 1.25 ms*/
            conn_params.conn_latency = 4;
        }
        break;
    }

    status = ble_gap_update_connection_params(conn_id, &conn_params);
    return status;
}

/**
 * @brief Function for getting the current connection interval.
 *
 * @param[in]  conn_updated	 the information of BLE_GAP_CONNECTION_PARAM_UPDATED_IND event.
 *
 * @param[out] priority      the current connection interval.
 */
ble_gap_conn_param_priority_t ble_gap_get_current_connection_interval(ble_gap_connection_param_updated_ind_t *conn_updated)
{
    if (conn_updated->conn_interval == 0x0010) {
        return BLE_GAP_CONN_PARAM_HIGH_SPEED_ANDROID;
    } else if ((conn_updated->conn_interval >= 0x0010) && (conn_updated->conn_interval <= 0x0020)) {
        return BLE_GAP_CONN_PARAM_HIGH_SPEED_IOS;
    } else if ((conn_updated->conn_interval >= 0x0120) && (conn_updated->conn_interval <= 0x0130)) {
        return BLE_GAP_CONN_PARAM_LOW_POWER;
    }
    return BLE_GAP_CONN_PARAM_PRIORITY_DEFAULT;
}

/**
 * @brief Function for setting advertising interval.
 *          
 * @param[in]  conn_id	 connection id.
 * @param[in]  app_name	 application name, who trigger the request.
 *
 * @param[out] status    result of setting advertising interval request.
 */
ble_status_t ble_gap_set_advertising_interval(uint16_t conn_id, uint8_t *app_name, ble_gap_adv_interval_enum_t adv_interval)
{
    ble_status_t status = BLE_STATUS_SUCCESS;
    uint16_t adv_min_interval;
    uint16_t adv_max_interval;
    const ble_gap_connection_info_t *conn_info = NULL;

    if (g_ble_gap_curr_adv == adv_interval) {
        return BLE_STATUS_GAP_FAILED;
    }
    conn_info = ble_gap_get_connection_info(conn_id);
    if (conn_info == NULL) {
        return BLE_STATUS_GAP_FAILED;
    }
    g_ble_gap_curr_adv = adv_interval;

    switch (adv_interval) {
        case BLE_GAP_ADV_INTERVAL_FAST_CONNECT:
        {
            adv_min_interval = 0xC0;/** 192 * 0.625 ms = 120ms. */
            adv_max_interval = 0xC0;/** 192 * 1.25 ms = 120ms. */
        }
            break;

        case BLE_GAP_ADV_INTERVAL_LOW_POWER:
        case BLE_GAP_ADV_INTERVAL_DEFAULT:
        {
            adv_min_interval = 0x0320;/** 800* 0.625 ms = 500ms. */
            adv_max_interval = 0x07D0;/** 2000* 0.625 ms = 1250ms. */
        }
        break;

        default:
        {
            adv_min_interval = 0x0320;/** 800* 0.625 ms = 500ms. */
            adv_max_interval = 0x07D0;/** 2000* 0.625 ms = 1250ms. */
        }
        break;
    }

    status = ble_gap_set_advertising_params(adv_min_interval, 
                                   adv_max_interval, 
                                   BLE_GAP_ADVERTISING_CONNECTABLE_DIRECTED, 
                                   BLE_ADDRESS_TYPE_RANDOM, 
                                   BLE_ADDRESS_TYPE_RANDOM, 
                                   &conn_info->peer_addr, 
                                   0x07, 
                                   0);
    return status;
}






