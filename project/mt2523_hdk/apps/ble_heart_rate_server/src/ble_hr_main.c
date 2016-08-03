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
 * This file implements Heart rate App main function
 *
 ****************************************************************************/
#include <stdio.h>
#include "syslog.h"
#include "ble_gatt.h"
#include "ble_gap.h"
#include "ble_message.h"
#include "ble_gatts_srv_common.h"
#include "ble_hr_main.h"
#include "ble_hrs.h"
#include "ble_dis.h"
#include <string.h>


/************************************************
*   Structures
*************************************************/

typedef struct
{
    ble_hrs_heart_rate_control_point_t heart_rate_control_point; /**< Holds decoded data in Write event handler, not for write long. */
    uint8_t *heart_rate_control_point_write_long_data;           /**< Holds data only for write long*/
    uint8_t heart_rate_control_point_is_prepare;                 /**< is prepare(TRUE) or not(FALSE)*/
    uint8_t heart_rate_control_point_is_excute;
    uint16_t heart_rate_control_point_write_offset;              /**< offset for write long*/
    uint16_t heart_rate_control_point_write_length;              /**< he data length of write value */
    uint16_t *handle_list;                                       /**< characteristic handle list, for read multiple */
    uint16_t heart_rate_measurement_cccd_value;                  /**< Holds decoded data in Notify and Indicate event handler. */
    uint16_t body_sensor_location_read_offset;                   /**< offset for read long*/
} ble_gatts_hrs_cntx_t;

typedef struct
{
    uint16_t *handle_list;                                       /**< characteristic handle list, for read multiple */
    uint16_t manufacturer_name_string_read_offset;               /**< offset for read long*/
    uint16_t model_number_string_read_offset;                    /**< offset for read long*/
    uint16_t serial_number_string_read_offset;                   /**< offset for read long*/
    uint16_t hardware_revision_string_read_offset;               /**< offset for read long*/
    uint16_t firmware_revision_string_read_offset;               /**< offset for read long*/
    uint16_t software_revision_string_read_offset;               /**< offset for read long*/
    uint16_t system_id_read_offset;                              /**< offset for read long*/
    uint16_t ieee_11073_20601_regulatory_certification_data_list_read_offset;                                          /*offset for read long*/
    uint16_t pnp_id_read_offset;
} ble_gatts_dis_cntx_t;


typedef struct
{
    uint16_t conn_id;
    ble_gatts_hrs_cntx_t hrs_cntx;
    ble_gatts_dis_cntx_t dis_cntx;
    ble_gap_conn_param_priority_t current_priority;
} ble_gatts_hr_cntx_t;

/************************************************
*   Global variables
*************************************************/
#define HRP_MAX_MULTIPLE_HANDLE_NUMBER 8
#define HRP_MAX_WRITE_DATA_LENGTH 158
ble_evt_t p_ble_evt_t = {{0}};
ble_hrs_t p_hrs_t;
ble_dis_t p_dis_t;
ble_gatts_hr_cntx_t g_hr_cntx_t[MAX_LE_DEV_NUM];
uint16_t read_handle[HRP_MAX_MULTIPLE_HANDLE_NUMBER];
uint8_t write_data[HRP_MAX_WRITE_DATA_LENGTH];
/************************************************
*   static utilities
*************************************************/
static void ble_hr_services_init(void);
static void ble_hr_evt_dispatch(ble_evt_t *p_ble_evt);
static void ble_hr_on_ble_evt(ble_evt_t *p_ble_evt);
static ble_status_t ble_gap_advertise_data_init(void);
static uint16_t ble_hr_get_null_cntx_index(void);
static uint16_t ble_hr_get_link_number(void);
static bool ble_hr_have_existed_in_cntx(uint16_t conn_id);
static uint16_t ble_hr_get_cntx_index_by_conn_id(uint16_t conn_id);

log_create_module(HR_MAIN, PRINT_LEVEL_INFO);

/************************************************
*   Functions
*************************************************/
void ble_hr_start_advertise(void)
{
    /* you can rename address name, address and advertise data here*/
    //uint8_t buff[31] = {2, 1, 0x1A};
    //char* LE_name = "my_hj_dev";
    //uint8_t len = 0;
    ble_address_t addr;
    addr.address[0] = 0x03;
    addr.address[1] = 9;
    addr.address[2] = 8;
    addr.address[3] = 7;
    addr.address[4] = 6;
    addr.address[5] = 0xC3;
    ble_gap_stop_advertiser();
    ble_gap_set_random_address(&addr);
    #if 0
    len = strlen(LE_name);
    buff[3] = len + 1;
    buff[4] = 9;
    memcpy(buff+5, LE_name, len);
    ble_gap_set_advertising_data(
        buff, 
        len + 5, 
        NULL, 
        0
    );
    #endif
    ble_gap_advertise_data_init();
    ble_gap_set_advertising_params(
        0xC0, 
        0xC0, 
        BLE_GAP_ADVERTISING_CONNECTABLE_UNDIRECTED, 
        BLE_ADDRESS_TYPE_RANDOM, 
        BLE_ADDRESS_TYPE_RANDOM, 
        NULL, 
        0x7, 
        0
    );
    ble_gap_start_advertiser();
    
    LOG_I(HR_MAIN, "[BLE_HR] ble_hr_start_advertise\r\n");
}

/**
 * @brief Function for getting the context index of link with connection id is conn_id.
 */
static uint16_t ble_hr_get_cntx_index_by_conn_id(uint16_t conn_id)
{
    uint16_t i;
    uint16_t index = 0;
    
    for (i = 0; i < MAX_LE_DEV_NUM; i++) {
        if (conn_id == g_hr_cntx_t[i].conn_id) {
            index = i;
            break;
        }
    }
	LOG_I(HR_MAIN, "[BLE_HR] ble_hr_get_cntx_index_by_conn_id: index[%d]\n", index);
    return index;
}

static bool ble_hr_have_existed_in_cntx(uint16_t conn_id)
{
    uint16_t i;
    bool is_existed = false;
    
    for (i = 0; i < MAX_LE_DEV_NUM; i++) {
        if (conn_id == g_hr_cntx_t[i].conn_id) {
            is_existed = true;
            break;
        }
    } 
	LOG_I(HR_MAIN, "[BLE_HR] ble_hr_have_existed_in_cntx: is_existed[%d],conn_id[%d]\n", is_existed, conn_id);
    return is_existed;
}

static uint16_t ble_hr_get_null_cntx_index(void)
{
    uint16_t i;
    uint16_t index = 0;
     
    for (i = 0; i < MAX_LE_DEV_NUM; i++) {
        if (0 == g_hr_cntx_t[i].conn_id) {
            index = i;
            break;
        }
    }
    LOG_I(HR_MAIN, "[BLE_HR] ble_hr_get_null_cntx_index: null index[%d]\n", index);
    return index;
}

static uint16_t ble_hr_get_link_number(void)
{
    uint16_t i;
    uint16_t number = 0;
     
    for (i = 0; i < MAX_LE_DEV_NUM; i++) {
        if (0 != g_hr_cntx_t[i].conn_id) {
            number ++;
        }
    }
    LOG_I(HR_MAIN, "[BLE_HR] ble_hr_get_link_number: linked number[%d]\n", number);
    return number;
}

/**
 * @brief Function for handling the HRS Service events.
 *
 * @details     This function will be called for all HRS Service events which are passed to
 *			    the application.
 *
 * @param[in]	p_hrs	HRS Service structure.
 * @param[in]	p_evt	Event received from the HRS Service.
 */
void ble_hr_hrs_evt_handler(ble_hrs_t *p_hrs, ble_hrs_evt_t *p_evt)
{
    if (false == ble_hr_have_existed_in_cntx(p_evt->conn_id)) {
        LOG_I(HR_MAIN, "[BLE_HR] ble_hr_hrs_evt_handler, invalid conn id \n");
        return;
    } else {
        uint16_t conn_id = p_evt->conn_id;
        uint16_t index = ble_hr_get_cntx_index_by_conn_id(conn_id);
        LOG_I(HR_MAIN, "[BLE_HR] ble_hr_hrs_evt_handler: conn_id[%d]\n", conn_id);

        switch (p_evt->evt_type) {
            case BLE_HRS_HEART_RATE_MEASUREMENT_EVT_CCCD_WRITE:
            {
                ble_hrs_heart_rate_measurement_t p_heart_rate_measurement;
                memset(&p_heart_rate_measurement, 0, sizeof(ble_hrs_heart_rate_measurement_t));	
                g_hr_cntx_t[index].hrs_cntx.heart_rate_measurement_cccd_value = p_evt->params.heart_rate_measurement_cccd_value;
                /*the value is virtual, not real value from sensor*/
                p_heart_rate_measurement.flags.heart_rate_value_format_bit = (ble_hrs_enum_flags_heart_rate_value_format_bit_t)1;
                p_heart_rate_measurement.heart_rate_measurement_value__uint16_ = 0x49;
                ble_hrs_heart_rate_measurement_send(p_hrs, &p_heart_rate_measurement, conn_id);
                LOG_I(HR_MAIN, "[BLE_HR] ble_hr_hrs_evt_handler, heart_rate_measurement_value[%d], conn_id[%d] \n", p_heart_rate_measurement.heart_rate_measurement_value__uint16_, conn_id);
            }
                break;
            case BLE_HRS_HEART_RATE_MEASUREMENT_EVT_CCCD_READ:
            { 
                LOG_I(HR_MAIN, "[BLE_HR]heart_rate_measurement_read[%d]\n", g_hr_cntx_t[index].hrs_cntx.heart_rate_measurement_cccd_value);
                ble_hrs_heart_rate_measurement_cccd_read_rsp(p_hrs, conn_id, g_hr_cntx_t[index].hrs_cntx.heart_rate_measurement_cccd_value);
            }
                break;

            case BLE_HRS_BODY_SENSOR_LOCATION_EVT_READ:
            {
                ble_hrs_body_sensor_location_t hrs_location;
                g_hr_cntx_t[index].hrs_cntx.body_sensor_location_read_offset = p_evt->params.body_sensor_location_read_offset;
                hrs_location.body_sensor_location.body_sensor_location = (ble_hrs_enum_body_sensor_location_t)3;

                LOG_I(HR_MAIN, "[BLE_HR]body_sensor_location read[%d]\n", hrs_location.body_sensor_location.body_sensor_location);
                ble_hrs_body_sensor_location_read_rsp(p_hrs, &hrs_location, conn_id, g_hr_cntx_t[index].hrs_cntx.body_sensor_location_read_offset);

            }
                break;
            case BLE_HRS_HEART_RATE_CONTROL_POINT_EVT_WRITE:
            {
                LOG_I(HR_MAIN, "[BLE_HR]heart_rate_control_point write[%d]\n", p_evt->params.heart_rate_control_point.heart_rate_control_point.heart_rate_control_point);
                g_hr_cntx_t[index].hrs_cntx.heart_rate_control_point.heart_rate_control_point.heart_rate_control_point = 
                p_evt->params.heart_rate_control_point.heart_rate_control_point.heart_rate_control_point;
            }
                break;
            case BLE_HRS_HEART_RATE_CONTROL_POINT_EVT_EXCUTE_WRITE:
            {
                /**< No implementation needed.*/
            }
                break;
            default:
                break;
        }
    }
}

/**
 * @brief Function for handling the DIS Service events.
 *
 * @details     This function will be called for allDIS Service events which are passed to
 *			    the application.
 *
 * @param[in]	p_dis  	DIS Service structure.
 * @param[in]	p_evt	Event received from the DIS Service.
 */
void ble_hr_dis_evt_handler(ble_dis_t *p_dis, ble_dis_evt_t *p_evt)
{
    /*Note: all followed values are virtual test value, you can reassign them  */
    if (false == ble_hr_have_existed_in_cntx(p_evt->conn_id)) {
        LOG_I(HR_MAIN, "[BLE_HR] ble_hr_dis_evt_handler, invalid conn id \n");
        return;
    } else {
        uint16_t conn_id = p_evt->conn_id;
        uint16_t index = ble_hr_get_cntx_index_by_conn_id(conn_id);
        LOG_I(HR_MAIN, "[BLE_HR] ble_hr_dis_evt_handler: conn_id[%d]\n", conn_id);

        switch (p_evt->evt_type) {
            case BLE_DIS_MANUFACTURER_NAME_STRING_EVT_READ:
            {
                ble_dis_manufacturer_name_string_t dis_name;
                uint8_t name[10] = {0};
                memset(name, 0, sizeof(name));
                memcpy(name, "Mediatek", sizeof("Mediatek"));
                g_hr_cntx_t[index].dis_cntx.manufacturer_name_string_read_offset = p_evt->params.manufacturer_name_string_read_offset;
                dis_name.manufacturer_name.length = sizeof(name);
                dis_name.manufacturer_name.p_str = name;
                ble_dis_manufacturer_name_string_read_rsp(p_dis, &dis_name, conn_id, g_hr_cntx_t[index].dis_cntx.manufacturer_name_string_read_offset);
            }
                break;
            case BLE_DIS_MODEL_NUMBER_STRING_EVT_READ:
            {
                ble_dis_model_number_string_t dis_model;
                uint8_t name[8] = {0};

                g_hr_cntx_t[index].dis_cntx.model_number_string_read_offset = p_evt->params.model_number_string_read_offset;
                memcpy(name, "MTK111", sizeof("MTK111"));
                dis_model.model_number.length = sizeof(name);
                dis_model.model_number.p_str = name;
                ble_dis_model_number_string_read_rsp(p_dis, &dis_model, conn_id, g_hr_cntx_t[index].dis_cntx.model_number_string_read_offset);

            }
                break;
            case BLE_DIS_SERIAL_NUMBER_STRING_EVT_READ:
            {
                ble_dis_serial_number_string_t dis_serial;
                uint8_t name[8] = {0};
                g_hr_cntx_t[index].dis_cntx.serial_number_string_read_offset = p_evt->params.serial_number_string_read_offset;
                memcpy(name, "MTK9000", sizeof("MTK9000"));
                dis_serial.serial_number.length = sizeof(name);
                dis_serial.serial_number.p_str = name;
                ble_dis_serial_number_string_read_rsp(p_dis, &dis_serial, conn_id, g_hr_cntx_t[index].dis_cntx.serial_number_string_read_offset);
            }
                break;
            case BLE_DIS_HARDWARE_REVISION_STRING_EVT_READ:
            {
                ble_dis_hardware_revision_string_t dis_hware;
                uint8_t name[8] = {0};
                g_hr_cntx_t[index].dis_cntx.hardware_revision_string_read_offset = p_evt->params.hardware_revision_string_read_offset;
                memcpy(name, "M37255", sizeof("M37255"));
                dis_hware.hardware_revision.length = sizeof(name);
                dis_hware.hardware_revision.p_str = name;
                ble_dis_hardware_revision_string_read_rsp(p_dis, &dis_hware, conn_id, g_hr_cntx_t[index].dis_cntx.hardware_revision_string_read_offset);
            }
                break;
            case BLE_DIS_FIRMWARE_REVISION_STRING_EVT_READ:
            {
                ble_dis_firmware_revision_string_t dis_fware;
                uint8_t name[8] = {0};
                g_hr_cntx_t[index].dis_cntx.firmware_revision_string_read_offset = p_evt->params.firmware_revision_string_read_offset;
                memcpy(name, "M37256", sizeof("M37256"));

                dis_fware.firmware_revision.length = sizeof(name);
                dis_fware.firmware_revision.p_str = name;
                ble_dis_firmware_revision_string_read_rsp(p_dis, &dis_fware, conn_id, g_hr_cntx_t[index].dis_cntx.firmware_revision_string_read_offset);

            }
                break;
            case BLE_DIS_SOFTWARE_REVISION_STRING_EVT_READ:
            {
                ble_dis_software_revision_string_t dis_soft;
                uint8_t name[8] = {0};
                g_hr_cntx_t[index].dis_cntx.software_revision_string_read_offset = p_evt->params.software_revision_string_read_offset;
                memcpy(name, "M37257", sizeof("M37257"));
                dis_soft.software_revision.length = sizeof(name);
                dis_soft.software_revision.p_str = name;
                ble_dis_software_revision_string_read_rsp(p_dis, &dis_soft, conn_id, g_hr_cntx_t[index].dis_cntx.software_revision_string_read_offset);

            }
                break;
            case BLE_DIS_SYSTEM_ID_EVT_READ:
            {
                ble_dis_system_id_t dis_sys;
                g_hr_cntx_t[index].dis_cntx.system_id_read_offset = p_evt->params.system_id_read_offset;
                dis_sys.manufacturer_identifier = 0x1324;
                dis_sys.organizationally_unique_identifier = 0x5678;
                ble_dis_system_id_read_rsp(p_dis, &dis_sys, conn_id, g_hr_cntx_t[index].dis_cntx.system_id_read_offset);

            }
                break;
            case BLE_DIS_IEEE_11073_20601_REGULATORY_CERTIFICATION_DATA_LIST_EVT_READ:
            {
                ble_dis_ieee_11073_20601_regulatory_certification_data_list_t dis_ieee;
                uint8_t name[8] = {0};
                g_hr_cntx_t[index].dis_cntx.ieee_11073_20601_regulatory_certification_data_list_read_offset = p_evt->params.ieee_11073_20601_regulatory_certification_data_list_read_offset;
                memcpy(name, "yes", sizeof("yes"));
                dis_ieee.data.list_len = sizeof(name);
                dis_ieee.data.p_list = name;
                ble_dis_ieee_11073_20601_regulatory_certification_data_list_read_rsp(p_dis, &dis_ieee, conn_id, g_hr_cntx_t[index].dis_cntx.ieee_11073_20601_regulatory_certification_data_list_read_offset);
            }
                break;
            case BLE_DIS_PNP_ID_EVT_READ:
            {
                ble_dis_pnp_id_t dis_pnp;
                g_hr_cntx_t[index].dis_cntx.pnp_id_read_offset = p_evt->params.pnp_id_read_offset;
                dis_pnp.product_id = 0x10;
                dis_pnp.product_version = 0x02;
                dis_pnp.vendor_id = 0x03;
                dis_pnp.vendor_id_source.vendor_id_source = (ble_dis_enum_vendor_id_source_t)1;
                ble_dis_pnp_id_read_rsp(p_dis, &dis_pnp, conn_id, g_hr_cntx_t[index].dis_cntx.pnp_id_read_offset);

            }
                break;
            default:
                break;
        }
    }
}

/**
 * @brief Function for handling the connection params update complete event.
 *
 * @param[in]	conn_updated  	connection params update complete event structure.
 * 
 */
void ble_hr_connection_param_updated_callback(ble_gap_connection_param_updated_ind_t *conn_updated)
{
    ble_gap_conn_param_priority_t conn_param;   
    conn_param = ble_gap_get_current_connection_interval(conn_updated);

    if (true == ble_hr_have_existed_in_cntx(conn_updated->conn_id)) {
        uint16_t index = ble_hr_get_cntx_index_by_conn_id(conn_updated->conn_id);
        if (conn_param == g_hr_cntx_t[index].current_priority) {
            LOG_I(HR_MAIN, "[BLE_HR] connection param update successfully \n");
        }
    } else {
        LOG_I(HR_MAIN, "[BLE_HR] connection param update fail, invalid conn id \n");
    }
}

/**
 * @brief Function for setting the advertising interval.
 */
ble_status_t ble_hr_set_advertising_interval(uint16_t conn_id)
{
    ble_status_t status;
    uint8_t name[4] = {0};
    ble_gap_adv_interval_enum_t interval = BLE_GAP_ADV_INTERVAL_FAST_CONNECT;

    memcpy(name, "hr", sizeof("hr"));
    status = ble_gap_set_advertising_interval(conn_id, name, interval);
    LOG_I(HR_MAIN, "[BLE_HR] ble_hr_set_advertising_interval: status[%d]\n", status);
    return status;
}

/**
 * @brief Function for setting the connection interval.
 */
ble_status_t ble_hr_update_connection_interval(uint16_t conn_id)
{
    ble_status_t status;
    uint8_t name[4] = {0};
    ble_gap_conn_param_priority_t interval = BLE_GAP_CONN_PARAM_HIGH_SPEED_IOS;

    memcpy(name, "hr", sizeof("hr"));
    status = ble_gap_update_connection_interval(conn_id, name, interval);
    if (status == BLE_STATUS_GAP_PENDING) {  
        uint16_t index = ble_hr_get_cntx_index_by_conn_id(conn_id);
        g_hr_cntx_t[index].current_priority = interval;
    }

    LOG_I(HR_MAIN, "[BLE_HR] ble_hr_update_connection_interval: status[%d]\n", status);
    return status;
}

/**
 * @brief Function for handling the Application's BLE Stack events.
 *
 * @param[in]    p_ble_evt  Bluetooth stack event.
 */
static void ble_hr_on_ble_evt(ble_evt_t *p_ble_evt)
{
    LOG_I(HR_MAIN, "[BLE_HR] ble_hr_on_ble_evt, evt_id: %d\n", p_ble_evt->header.evt_id);
    switch (p_ble_evt->header.evt_id) {
        case BLE_POWER_ON_CNF:
        {
            if (BLE_STATUS_SUCCESS == p_ble_evt->evt.gap_evt.power_on.result) {
                LOG_I(HR_MAIN, "[BLE_HR] ble_hr_on_ble_evt, power on\r\n");
                ble_hr_start_advertise();
                ble_hr_main();
            }
        }
            break;
        case BLE_POWER_OFF_CNF:
        {
            if (BLE_STATUS_SUCCESS == p_ble_evt->evt.gap_evt.power_off.result) {
               LOG_I(HR_MAIN, "[BLE_HR] ble_hr_on_ble_evt, power off\r\n");
            }
        }
            break;
        case BLE_GAP_CONNECTED_IND:
        {
            if (ble_hr_get_link_number() < MAX_LE_DEV_NUM) {
                uint16_t conn_id = p_ble_evt->evt.gap_evt.connected.conn_id;
                if (false == ble_hr_have_existed_in_cntx(conn_id)) {
                    uint16_t index = ble_hr_get_null_cntx_index();
                    memset(&g_hr_cntx_t[index], 0, sizeof(ble_gatts_hr_cntx_t));
                    g_hr_cntx_t[index].conn_id = conn_id;
                    LOG_I(HR_MAIN, "[BLE_HR] BLE_GAP_CONNECTED_IND: conn_id[%d]\n", conn_id);
                }
            }
        }
            break;
        case BLE_GAP_DISCONNECTED_IND:
        {
            if ((true == ble_hr_have_existed_in_cntx(p_ble_evt->evt.gap_evt.disconnected.conn_id))  
                && (0 != p_ble_evt->evt.gap_evt.disconnected.conn_id)) {
                uint16_t index = ble_hr_get_cntx_index_by_conn_id(p_ble_evt->evt.gap_evt.disconnected.conn_id);
                memset(&g_hr_cntx_t[index], 0, sizeof(ble_gatts_hr_cntx_t));

                LOG_I(HR_MAIN, "[BLE_HR] BLE_GAP_DISCONNECTED_IND: conn_id[%d]\n", p_ble_evt->evt.gap_evt.disconnected.conn_id);
            }
        }
            break;
        case BLE_GAP_ADVERTISING_IND:
            break;
        case BLE_GAP_CONNECTION_PARAM_UPDATED_IND:
            ble_hr_connection_param_updated_callback(&(p_ble_evt_t.evt.gap_evt.conn_params));
            break;
        case BLE_GAP_REPORT_RSSI_IND:
            break;
        default:
            break;
    }
}

/**
 * @brief Function for initializing advertising data.
 */
static ble_status_t ble_gap_advertise_data_init(void)
{
    ble_status_t status;
    bt_gap_set_adv_data_req_struct_t req;
    uint8_t ble_hrs_uuid[2] = {0x18, 0x0D};

    memset(&req, 0, sizeof(bt_gap_set_adv_data_req_struct_t));
    req.max_interval = 0xC0;
    req.min_interval = 0xC0;
    req.service_uuid_len = sizeof(ble_hrs_uuid);
    memcpy(req.service_uuid, ble_hrs_uuid, sizeof(ble_hrs_uuid));
    status = ble_gap_set_advertising_data_reqest(&req);
    LOG_I(HR_MAIN, "[BLE_HR] ble_gap_advertise_data_init: status[%d]\n", status);

    return status;
}

/**
 * @brief Function for initializing services that will be used by the application.
 */
static void ble_hr_services_init(void)
{
    int32_t err_code;
    ble_hrs_init_t hrs_init_t;
    ble_dis_init_t dis_init_t;

    /** Initialize HRS Service.*/
    memset(&hrs_init_t, 0, sizeof(ble_hrs_init_t));
    memset(&p_hrs_t, 0, sizeof(ble_hrs_t));
    hrs_init_t.evt_handler = ble_hr_hrs_evt_handler;
    err_code = ble_hrs_init(&p_hrs_t, &hrs_init_t);
    if (0 != err_code) {
        LOG_I(HR_MAIN, "[BLE_HR] ble_hr_services_init hrs init: err_code[%d] \n", err_code);
    }

    /**< Initialize DIS Service. */
    memset(&dis_init_t, 0, sizeof(ble_dis_init_t));
    memset(&p_dis_t, 0, sizeof(ble_dis_t));
    dis_init_t.evt_handler = ble_hr_dis_evt_handler;
    err_code = ble_dis_init(&p_dis_t, &dis_init_t);	
    if (0 != err_code) {
        LOG_I(HR_MAIN, "[BLE_HR] ble_hr_services_init dis init: err_code[%d] \n", err_code);
    }
}

/**
 * @brief Function for application main entry.
 */
void ble_hr_main(void)
{
    /**< Initialize. */
	ble_hr_services_init();	
	LOG_I(HR_MAIN, "[BLE_HR] ble_hr_main\n");
}

/**
 * @brief Function for dispatching a BLE stack event to all modules with a BLE stack event handler.
 *
 * @details        This function is called from the BLE Stack event interrupt handler after a BLE stack
 *                 event has been received.
 *
 * @param[in]      p_ble_evt     Bluetooth stack event.
 */
static void ble_hr_evt_dispatch(ble_evt_t *p_ble_evt)
{
    LOG_I(HR_MAIN, "[BLE_HR] ble_hr_evt_dispatch\n");

    /**< add your app's gatts event handler as below. */
    ble_hrs_on_ble_evt(&p_hrs_t, p_ble_evt);
    ble_dis_on_ble_evt(&p_dis_t, p_ble_evt);
    ble_hr_on_ble_evt(p_ble_evt);
}

/**
 * @brief Function for distinguishing a BLE stack event to all modules with a BLE stack Gatt event handler.
 *
 * @details        This function is called from the BLE Stack Gatt event interrupt handler 
 *
 * @param[in]      message_id  Message from Bluetooth stack.
 
 * @param[in]      *param      Message parameters.
 */
void ble_gatts_message_handler(ble_gatts_message_id_t message_id, void *param)
{
    LOG_I(HR_MAIN, "[BLE_HR] ble_gatts_message_handler: message_id[%d] \n", message_id);
    
    switch (message_id) {   
        case BLE_MSG_GATT_EXCHANGE_MTU_REQ_IND:
        {
            p_ble_evt_t.header.evt_id = BLE_GATT_EXCHANGE_MTU_REQ;
            memcpy(&(p_ble_evt_t.evt.gatts_evt.mtu_change), param, sizeof(ble_gatt_exchange_mtu_req_t));
            LOG_I(HR_MAIN, "[BLE_HR] mtu_change: conn_id[%d], expected_mtu[%d] \n", p_ble_evt_t.evt.gatts_evt.mtu_change.conn_id, p_ble_evt_t.evt.gatts_evt.mtu_change.expected_mtu);
            ble_gatt_confirm_exchange_mtu(p_ble_evt_t.evt.gatts_evt.mtu_change.conn_id, BLE_GATT_MTU_SIZE_CUSTOM);
        }
            break;

        case BLE_MSG_GATTS_READ_REQ_IND:
        {
            ble_gatts_read_req_t *temp_req = (ble_gatts_read_req_t *)param;
            
            p_ble_evt_t.header.evt_id = BLE_GATTS_READ_REQ;
            memcpy(&(p_ble_evt_t.evt.gatts_evt.read), param, sizeof(ble_gatts_read_req_t));

            memset(read_handle, 0, HRP_MAX_MULTIPLE_HANDLE_NUMBER);
            if (temp_req->count <= HRP_MAX_MULTIPLE_HANDLE_NUMBER) {
                memcpy(read_handle, temp_req->handle, (sizeof(uint16_t) * temp_req->count));
            } else {
                memcpy(read_handle, temp_req->handle, (sizeof(uint16_t) * HRP_MAX_MULTIPLE_HANDLE_NUMBER));
            }
            ble_gatts_free_memory(temp_req->handle);
            p_ble_evt_t.evt.gatts_evt.read.handle = read_handle;
        }
            break;

        case BLE_MSG_GATTS_WRITE_REQ_IND:
        {
            ble_gatts_write_req_t *temp_data = (ble_gatts_write_req_t *)param;
            
            p_ble_evt_t.header.evt_id = BLE_GATTS_WRITE_REQ;
            memcpy(&(p_ble_evt_t.evt.gatts_evt.write), param, sizeof(ble_gatts_write_req_t));
            memset(write_data, 0, HRP_MAX_WRITE_DATA_LENGTH);   
            if (temp_data->length <= HRP_MAX_WRITE_DATA_LENGTH) {
                memcpy(write_data, temp_data->data, (sizeof(uint8_t) * temp_data->length));
            } else {
                memcpy(write_data, temp_data->data, (sizeof(uint8_t) * HRP_MAX_WRITE_DATA_LENGTH));
            }
            ble_gatts_free_memory(temp_data->data);
            p_ble_evt_t.evt.gatts_evt.write.data = write_data;
            
        }
            break;

        case BLE_MSG_GATTS_EXCUTE_WRITE_REQ_IND:
        {
            p_ble_evt_t.header.evt_id = BLE_GATTS_EXECUTE_WRITE_REQ;
            memcpy(&(p_ble_evt_t.evt.gatts_evt.excute_write), param, sizeof(ble_gatts_execute_write_req_t));
        }
            break;

        case BLE_MSG_GATTS_HANDLE_VALUE_IND:
        {
            p_ble_evt_t.header.evt_id = BLE_GATTS_HANDLE_VALUE_CNF;
            memcpy(&(p_ble_evt_t.evt.gatts_evt.confirm), param, sizeof(ble_gatts_handle_value_cnf_t));
        }
            break;
            
        default:
            break;  
    }
    if (param != NULL) {
        ble_gatts_free_memory(param);
    }
    ble_hr_evt_dispatch(&p_ble_evt_t); 
}

/**
 * @brief Function for distinguishing a BLE stack event to all modules with a BLE stack Gap event handler.
 *
 *
 * @param[in]   *param      Message parameters.
 */
void ble_gap_message_handler(ble_gap_message_id_t message_id, void *param)
{
    LOG_I(HR_MAIN, "[BLE_HR] ble_gap_message_handler: message_id[%d] \n", message_id);
    switch (message_id) {
        case BLE_MSG_GAP_POWER_ON_IND:
        {
            p_ble_evt_t.header.evt_id = BLE_POWER_ON_CNF;
	    memcpy(&(p_ble_evt_t.evt.gap_evt.power_on), param, sizeof(ble_power_on_cnf_t));
        }
            break;
        case BLE_MSG_GAP_POWER_OFF_IND:
        {
            p_ble_evt_t.header.evt_id = BLE_POWER_OFF_CNF;
            memcpy(&(p_ble_evt_t.evt.gap_evt.power_off), param, sizeof(ble_power_off_cnf_t));
        }
            break;
        case BLE_MSG_GAP_CONNECTED_IND:
        {
            p_ble_evt_t.header.evt_id = BLE_GAP_CONNECTED_IND;
            memcpy(&(p_ble_evt_t.evt.gap_evt.connected), param, sizeof(ble_gap_connected_ind_t));
        }
            break;
        case BLE_MSG_GAP_DISCONNECTED_IND:
        {           
            p_ble_evt_t.header.evt_id = BLE_GAP_DISCONNECTED_IND;
            memcpy(&(p_ble_evt_t.evt.gap_evt.disconnected), param, sizeof(ble_gap_disconnected_ind_t));
        }
            break;
        case BLE_MSG_GAP_REPORT_RSSI_IND:
        {
            p_ble_evt_t.header.evt_id = BLE_GAP_REPORT_RSSI_IND;
            memcpy(&(p_ble_evt_t.evt.gap_evt.tx_power), param, sizeof(ble_gap_report_rssi_ind_t));
        }
            break;
        case BLE_MSG_GAP_ADVERTISING_IND:
        {
            p_ble_evt_t.header.evt_id = BLE_GAP_ADVERTISING_IND;
            memcpy(&(p_ble_evt_t.evt.gap_evt.advertising), param, sizeof(ble_gap_advertising_ind_t));
        }
            break;  
        case BLE_MSG_GAP_CONNECTION_PARAM_UPDATED_IND:
        {
            p_ble_evt_t.header.evt_id = BLE_GAP_CONNECTION_PARAM_UPDATED_IND;
            memcpy(&(p_ble_evt_t.evt.gap_evt.conn_params), param, sizeof(ble_gap_connection_param_updated_ind_t));
        }
            break;
        default:
            break;
    }

    if (param != NULL) {
        ble_gatts_free_memory(param);
    }
    ble_hr_evt_dispatch(&p_ble_evt_t);
}





