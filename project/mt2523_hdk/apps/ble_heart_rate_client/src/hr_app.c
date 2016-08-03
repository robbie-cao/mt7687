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

#include "hr_app.h"
#include <stdlib.h>
#include "ble_gattc_srv.h"
//#include "att_i.h"
#include "bt_status.h"
#include "syslog.h"
#include <string.h>
#include "ble_gattc_connect.h"

log_create_module(app_hrc, PRINT_LEVEL_INFO);

static void heart_rate_connect_cb(gattc_user_connect_struct *conn, uint16_t connected, ble_address_t *bd_addr, uint16_t is_ind);

static void heart_rate_write_descriptor_cb(gattc_user_connect_struct *conn, int32_t result, gattc_descr_info_t *descr_info);


static void heart_rate_update_notify_cb(gattc_user_connect_struct *conn,
                                        gattc_char_info_struct *char_info, gattc_value_t *value, uint16_t is_notify);

static app_hrp_info_dev_struct *heart_rate_query_dev_info(app_query_info_type_enum type, void *para);

static app_hrp_info_dev_struct *heart_rate_create_new_dev_info(uint16_t conn_id, ble_address_t *addr);

static void heart_rate_clear_dev_info(app_hrp_info_dev_struct *dev_info);

static void heart_rate_handle_req(app_hrp_info_dev_struct *dev_info, uint16_t id, gatt_uuid_t *start_uuid);

static void heart_rate_read_descriptor_cb(gattc_user_connect_struct *conn, int32_t result,
        gattc_descr_info_t *descr_info, gattc_value_t *value);

static void heart_rate_search_service_from_db(gattc_user_connect_struct *conn, app_hrp_info_dev_struct *dev_info);

static uint16_t heart_rate_get_srv_by_char_uuid(uint16_t char_uuid);

static app_hrp_char_info_struct *heart_rate_query_char_info(app_hrp_info_dev_struct *dev_info, uint16_t id);

static void heart_rate_callback_init(app_callback_struct *app_gattc_cb);

static uint8_t heart_rate_get_srv_index_by_uuid(uint16_t srv_uuid);

static app_hrp_context_struct *heart_rate_get_cntx(void);

static void ble_hrc_app_show_data(ble_event_t event_id, const void *param);

#define APP_QUEUE_SIZE      10

QueueHandle_t app_queue = NULL;
hear_rate_message_struct app_queue_data;
TaskHandle_t xCreatedTask;
extern TaskHandle_t cmd_xCreatedTask;

app_callback_struct g_hrcp_gatt_cb;
app_hrp_context_struct g_hrcp_cntx;
#ifdef __GATTC_SRV_TEST__
uint8_t  g_hr_srv_uuid[] =  {
    0xFB, 0x34, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80,
    0x00, 0x10, 0x00, 0x00, 0x09, 0x18, 0x00, 0x00,
};

#else

uint8_t  g_hr_srv_uuid[] =  {
    0xFB, 0x34, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80,
    0x00, 0x10, 0x00, 0x00, 0x0D, 0x18, 0x00, 0x00,
};
#endif

uint16_t g_srv_uuid[] = {
    SRV_HRM_SER_UUID,
    SRV_DIS_UUID
};

uint16_t  g_hr_char_uuid[HRART_RATE_TYPE_TOTAL] = {
    HRM_CHAR_UUID, HBL_CHAR_UUID, HCP_CHAR_UUID
};

uint16_t g_desc_uuid[] = {
    CLIENT_CONFI_DESCRI
};


void heart_rate_init(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    app_hrp_context_struct *hrc_cntx;
    gattc_register_req_struct req;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    hrc_cntx = heart_rate_get_cntx();
    heart_rate_callback_init(&g_hrcp_gatt_cb);
    memset(&req, 0, sizeof(gattc_register_req_struct));
    req.uuid_count = 1;
    req.uuid = g_hr_srv_uuid;
    hrc_cntx->reg_ctx = gattc_register(&req, &g_hrcp_gatt_cb);
    if (!hrc_cntx->reg_ctx) {
        /*reg is  null*/
        LOG_I(app_hrc, "[GATTC]heart_rate_init: -- register failed");
    }
}


void heart_rate_deinit()
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    app_hrp_context_struct *hrc_cntx;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    hrc_cntx = heart_rate_get_cntx();
    gattc_deregister((gattc_user_context_t *)hrc_cntx->reg_ctx);
}


static uint8_t heart_rate_get_srv_index_by_uuid(uint16_t srv_uuid)
{
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    uint8_t i;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    for (i = 0; i < SRV_HR_SERVICE_MAX_NUM; i++) {
        if (srv_uuid == g_srv_uuid[i]) {/*only need modify max num and srv_uuid's conetent*/
            break;
        }
    }
    return i;
}


uint8_t heart_rate_get_char_index(app_hrp_info_dev_struct *dev_info, gatt_uuid_t *uuid, app_decl_type_enum type)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    uint8_t i = 0;
    uint16_t char_uuid, srv_uuid, exist_uuid;
    app_hrp_char_info_struct *char_info = NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    char_uuid = gattc_convert_srv_uuid_to_u16(uuid);

    srv_uuid = heart_rate_get_srv_by_char_uuid(char_uuid);

    char_info = heart_rate_query_char_info(dev_info, srv_uuid);

    switch (type) {
        case TYPE_CHAR: {
            for (i = 0; i < HRART_RATE_TYPE_TOTAL; i++) {
                if (!char_info[i].is_cvalid) {//char_uuid == g_hr_char_uuid[i])
                    break;
                } else {
                    exist_uuid = gattc_convert_srv_uuid_to_u16(&(char_info[i].char_uuid));
                    if (exist_uuid == char_uuid) {
                        break;
                    }
                }
            }
            break;
        }
        case TYPE_DESC: {
            for (i = 0; i < SRV_HR_MAX_DES_NUM; i++) {
                if (char_uuid == g_desc_uuid[i]) {
                    break;
                }
            }
        }
        break;
    }
    return i;
}


static uint16_t heart_rate_get_srv_by_char_uuid(uint16_t char_uuid)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    uint8_t i;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    for (i = 0; i < HRART_RATE_TYPE_TOTAL; i++) {
        if (char_uuid == g_hr_char_uuid[i]) {
            return SRV_HRM_SER_UUID;
        }
    }
    return 0;
}


uint16_t heart_rate_get_max_char_by_uuid(uint16_t uuid)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    uint16_t max_char = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    if (uuid == SRV_HRM_SER_UUID) {

        max_char = HRART_RATE_TYPE_TOTAL;

    } else if (uuid == SRV_DIS_UUID) {

        max_char = SRV_HR_MAX_DES_NUM;
    }
    return max_char;
}


static app_hrp_char_info_struct *heart_rate_query_char_info(app_hrp_info_dev_struct *dev_info, uint16_t id)
{
    if (dev_info) {

        if (id == SRV_HRM_SER_UUID) {

            return  dev_info->char_info;

        } else if (id == SRV_DIS_UUID) {

            return dev_info->dis_char_info;
        }
        return NULL;
    }
    return NULL;
}


static void heart_rate_scan_cb(void *reg_cntx, ble_gap_advertising_ind_t *param)
{
    LOG_I(app_hrc, "[GATTC]gattc_scan_cb: -- start: type = %d", param->advertising_type);

    LOG_I(app_hrc, "[GATTC]gattc_scan_cb: -- addr: addr[0] = %x, addr[1] = %x, addr[2] = %x, addr: addr[3] = %x, addr[4] = %x, addr[5] = %x",
          param->peer_addr.address[0], param->peer_addr.address[1],
          param->peer_addr.address[2], param->peer_addr.address[3], param->peer_addr.address[4], param->peer_addr.address[5]);
}


static void heart_rate_connect_cb(gattc_user_connect_struct *conn, uint16_t connected, ble_address_t *bd_addr, uint16_t is_ind)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    app_hrp_context_struct *hrc_cntx;
    app_hrp_info_dev_struct *dev_info;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    hrc_cntx = heart_rate_get_cntx();
    LOG_I(app_hrc, "[HRAPP]heart_rate_connect_cb:connected =%d, is_ind = %d", connected , is_ind);
    dev_info = heart_rate_query_dev_info(BY_CONN_ID, &conn->conn_id);
    if (hrc_cntx->reg_ctx == conn->reg_cntx) {
        LOG_I(app_hrc, "[HRAPP]heart_rate_connect_cb: dev_info = %x", dev_info);
        if (connected && !is_ind) {
            if (dev_info == NULL) {
                dev_info = heart_rate_create_new_dev_info(conn->conn_id, bd_addr);
            }
            if (dev_info) {
                ble_gattc_start_discover_service(conn->conn_id);/*start search service remote*/
            }

            if (hrc_cntx->ctotal_count >= SRV_MAX_DEV) {/*when link num reach the max*/
                ble_gap_stop_advertiser();
            }
        } else { /*disconnect  clear addreess*/

            if (connected && is_ind) {
                return;
            }
            if (dev_info) {
                heart_rate_clear_dev_info(dev_info);
            }
        }
    }
}


static void heart_rate_search_service_from_db(gattc_user_connect_struct *conn, app_hrp_info_dev_struct *dev_info)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    int8_t i = -1;
    uint16_t ser_uuid;
    int32_t  status = GATTC_STATUS_INVALID;
    gattc_search_service_req_t req;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    if (dev_info) {
        if (dev_info->status != GETTING_SERVICE) {
            dev_info->status = GETTING_SERVICE;
            req.conn = conn;
            memcpy(&req.uuid.uuid, g_hr_srv_uuid, 16);
            req.uuid.is_primary = 1;
            status = gattc_search_service(&req);
        }
    }

    LOG_I(app_hrc, "[HRAPP]heart_rate_search_service_from_db:status = %d", status);

    if (status == BT_STATUS_SUCCESS) {//judge search result is ok

        ser_uuid = ((uint16_t)g_hr_srv_uuid[13]) << 8 | g_hr_srv_uuid[12];
        if (dev_info != NULL ) {

            i =  heart_rate_get_srv_index_by_uuid(ser_uuid);

            LOG_I(app_hrc, "[HRAPP]heart_rate_search_service_from_db:srv_uuid = %x, i = %d", ser_uuid, i);

            if (i < SRV_HR_SERVICE_MAX_NUM) { /*if i is max number means service not found*/
                dev_info->srv_info[i].is_smatch = 1;
                memcpy(&dev_info->srv_info[i].srv_id, &req.uuid, sizeof(gatt_srv_uuid_t));
                dev_info->status = GETTING_CHAR;
                heart_rate_handle_req(dev_info, ser_uuid, NULL);
            }
        }
    } else {

        if (dev_info) {
            dev_info->status = GETTING_CONNECT;
        }
    }
    LOG_I(app_hrc, "[HRAPP]heart_rate_search_service_from_db:end--dev_status = %d", dev_info->status);
}


static void heart_rate_search_complete_cb(gattc_user_connect_struct *conn, int32_t result)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    app_hrp_info_dev_struct *dev_info;
//    gattc_cfg_mtu_t req;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    dev_info = heart_rate_query_dev_info(BY_CONN_ID, (void *)&conn->conn_id);

    LOG_I(app_hrc, "[HRAPP]heart_rate_search_complete_cb:start--dev = %x, result = %d", dev_info, result);

    if (result == GATTC_STATUS_SUCCESS) {
        if (dev_info) {
            //req.conn = conn;
            heart_rate_search_service_from_db(conn, dev_info);
        }
    }
}


static void heart_rate_read_descriptor_cb(gattc_user_connect_struct *conn, int32_t result,
        gattc_descr_info_t *descr_info, gattc_value_t *value)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    app_hrp_char_info_struct *ch_info = NULL;
    app_hrp_info_dev_struct *dev_info = NULL;
    app_hrp_context_struct *hrc_cntx;
    uint16_t srv_uuid;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    dev_info = heart_rate_query_dev_info(BY_CONN_ID, &conn->conn_id);
    hrc_cntx = heart_rate_get_cntx();
    srv_uuid = gattc_convert_srv_uuid_to_u16(&descr_info->svc_uuid.uuid);

    LOG_I(app_hrc, "[HRAPP]heart_rate_read_descriptor_cb:start--srv_uuid = %d", srv_uuid);

    LOG_I(app_hrc, "[HRAPP]heart_rate_read_descriptor_cb:start--dev = %x, result = %d", dev_info, result);

    if (dev_info && hrc_cntx->reg_ctx == conn->reg_cntx) {

        if (result == BT_STATUS_SUCCESS) {
            int8_t i;

            i = heart_rate_get_char_index(dev_info, &descr_info->char_uuid, TYPE_CHAR);
            LOG_I(app_hrc, "[HRAPP]heart_rate_read_descriptor_cb:start--i = %d", i);

            if (i < HRART_RATE_TYPE_TOTAL) {
                ch_info = &dev_info->char_info[i];
                if (ch_info) {
                    ch_info->des_read = true;
                    memcpy(ch_info->descr_value, value->value, sizeof (value->len));
                    heart_rate_handle_req(dev_info, srv_uuid, NULL);
                }
            }
        }
    }
}


static void heart_rate_write_descriptor_cb(gattc_user_connect_struct *conn, int32_t result,
        gattc_descr_info_t *descr_info)

{

    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    uint16_t srv_uuid;
    uint8_t i;
    app_hrp_context_struct *hrc_cntx;
    app_hrp_info_dev_struct *dev_info = NULL;
    app_hrp_char_info_struct *ch_info = NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    hrc_cntx = heart_rate_get_cntx();

    dev_info = heart_rate_query_dev_info(BY_CONN_ID, &conn->conn_id);
    srv_uuid = gattc_convert_srv_uuid_to_u16(&descr_info->svc_uuid.uuid);


    if (dev_info && hrc_cntx->reg_ctx == conn->reg_cntx) {

        i = heart_rate_get_char_index(dev_info, &descr_info->char_uuid, TYPE_CHAR);
        LOG_I(app_hrc, "[HRAPP]heart_rate_write_descriptor_cb:start--i = %d", i);
        ch_info = &dev_info->char_info[i];/*modify*/

        if (ch_info) {
            ch_info->des_writed = true;
            heart_rate_handle_req(dev_info, srv_uuid, NULL);
        }
    }
}


static void heart_rate_update_notify_cb(gattc_user_connect_struct *conn,
                                        gattc_char_info_struct *char_info, gattc_value_t *value, uint16_t is_notify)
{

    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    uint16_t srv_uuid;
    uint16_t char_uuid;
    app_hrp_info_dev_struct *dev_info;
    app_hrp_context_struct *hrc_cntx;
    //hr_data_t data;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    hrc_cntx = heart_rate_get_cntx();

    srv_uuid = gattc_convert_srv_uuid_to_u16(&(char_info->svc_uuid.uuid));
    char_uuid = gattc_convert_srv_uuid_to_u16(&char_info->char_uuid);


    LOG_I(app_hrc, "[HRAPP]heart_rate_update_notify_cb:start--srv_uuid = %x, char_uuid = %x", srv_uuid, char_uuid);

    dev_info = heart_rate_query_dev_info(BY_CONN_ID, &conn->conn_id);

    if (dev_info != NULL && conn->reg_cntx == hrc_cntx->reg_ctx) {

        if (srv_uuid == SRV_HRM_SER_UUID && char_uuid == HRM_CHAR_UUID) {
            hear_rate_message_struct msg;
            msg.event_id = BLE_GATTC_HANDLE_VALUE_NOTIFICATION;
            memcpy(msg.param, value, sizeof(gattc_value_t));
            ble_hrc_app_callback(&msg);
        }
    }
}


static void ble_hrc_app_show_data(ble_event_t event_id, const void *param)
{

    if (event_id == BLE_GATTC_HANDLE_VALUE_NOTIFICATION) {
        gattc_value_t *value;
        hr_data_t data;
        value = (gattc_value_t *)param;
        gattc_decode_char_data(value, &data);
        if (data.val) {
            LOG_I(app_hrc, "[HRAPP]ble_hrc_app_show_data:start--heart_value = %d", data.val);
        }
    }
}


void ble_hrc_app_callback(hear_rate_message_struct *msg)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    BaseType_t ret = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    ret = xQueueSend(app_queue, (void *)msg, 0);  //if queue is full ,the new notifications will be dropped
    LOG_I(app_hrc, "[HRapp]send ret = %d\r\n", ret);
}

/* this fuction used for all main operation which includes:
    get characteristic, get descriptor, read descriptor, write descriptor,
*/
static void heart_rate_handle_req(app_hrp_info_dev_struct *dev_info, uint16_t id, gatt_uuid_t *start_uuid)
{

    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    uint8_t state = 0;
    int8_t i = 0, j = 0;
    gatt_uuid_t *start_char;
    app_hrp_context_struct *hrc_cntx;
    app_hrp_char_info_struct *ch_info;
    int32_t status = 0;
    gattc_user_connect_struct conn;
    uint32_t max_char = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    state = dev_info->status;
    ch_info = heart_rate_query_char_info(dev_info, id);
    max_char = heart_rate_get_max_char_by_uuid(id);

    j = heart_rate_get_srv_index_by_uuid(id);
    hrc_cntx = heart_rate_get_cntx();
    conn.reg_cntx = hrc_cntx->reg_ctx;
    conn.conn_id = dev_info->conn_id;
    LOG_I(app_hrc, "[HRAPP]heart_rate_handle_req: --start: dev_info = %x, max_char = %d",  ch_info, max_char);

    switch (state) {
        case GETTING_CHAR: {
            gattc_get_char_req_t req;
            gatt_get_char_result_t re_char;
            gatt_srv_uuid_t srv;
            start_char = start_uuid;
            for (i = 0; i < max_char;) {
                LOG_I(app_hrc, "[HRAPP]heart_rate_handle_req: --GETTING_CHAR: is_valid = %d, i = %d",  ch_info[i].is_cvalid, i);
                if (!ch_info[i].is_cvalid) {
                    memset(&req, 0, sizeof (gattc_get_char_req_t));
                    req.conn = &conn;
                    srv.is_primary = 1;
                    memcpy(&srv.uuid, &dev_info->srv_info[j].srv_id.uuid, sizeof(gatt_uuid_t));
                    req.svc_uuid = &srv;
                    req.start_char_uuid = start_char;

                    status = gattc_get_char(&req, &re_char);
                    LOG_I(app_hrc, "[HRAPP]heart_rate_handle_req: --GETTING_CHAR: status = %d, i = %d",  status, i);
                    if (status == BT_STATUS_SUCCESS) {
                        ch_info[i].is_cvalid = true;
                        memcpy(&ch_info[i].char_uuid, &re_char.char_info, sizeof(gatt_uuid_t));
                        start_char = &re_char.char_info;
                    }
                }
                i++;
            }
            if (i == max_char || status == GATTC_STATUS_CHAR_NOT_FOUND) {
                dev_info->status = GETTING_DESCR;
                heart_rate_handle_req(dev_info, id, NULL);
            }

            break;
        }
        case GETTING_DESCR: {
            gattc_get_descr_req_t req;
            gatt_uuid_t descr;
            req.conn = &conn;
            for (i = 0; i < max_char;) {
                LOG_I(app_hrc, "[HRAPP]heart_rate_handle_req: --GETTING_DESCR: is_valid = %d, i = %d, has_desc = %d",  ch_info[i].is_cvalid, i, ch_info[i].has_descr);
                if (ch_info[i].is_cvalid && !ch_info[i].has_descr) {
                    req.svc_uuid = &dev_info->srv_info[j].srv_id;
                    req.char_uuid = &ch_info[i].char_uuid;
                    req.start_descr_uuid = NULL;
                    status = gattc_get_descr(&req, &descr);
                    LOG_I(app_hrc, "[HRAPP]heart_rate_handle_req: --GETTING_DESCR: status = %d, i = %d",  status, i);
                    if (status == BT_STATUS_SUCCESS) {
                        ch_info[i].has_descr = true;
                        memcpy(&ch_info[i].descr_uuid, &descr, sizeof (gatt_uuid_t));
                        req.start_descr_uuid = &descr;
                    }
                }
                i++;
            }
            if (i == max_char || status != BT_STATUS_SUCCESS) {
                dev_info->status = WRITING_DESCR ;
                heart_rate_handle_req(dev_info, id, NULL);
            }
            break;
        }
        case WRITING_DESCR: {
            gattc_write_descr_req_t req;

            for (i = 0; i < max_char; ++i) {
                LOG_I(app_hrc, "[HRAPP]heart_rate_handle_req: --WRITING_DESCR: is_valid = %d, i = %d, ",  ch_info[i].is_cvalid, i);
                LOG_I(app_hrc, "[HRAPP]heart_rate_handle_req: --WRITING_DESCR:has_descr = %d, desc_write = %d, ",  ch_info[i].has_descr, ch_info[i].des_writed);
                if (ch_info[i].is_cvalid &&
                        ch_info[i].has_descr && !ch_info[i].des_writed) {
                    gattc_value_t value;
                    descr_cfg_t desc_value;
                    memset(&desc_value, 0 , sizeof (descr_cfg_t));
                    desc_value.desc_value = 1;

                    gattc_config_encode_data(&desc_value, &value);
                    req.svc_uuid = &dev_info->srv_info[j].srv_id;
                    req.char_uuid = &ch_info[i].char_uuid;

                    req.desc_uuid = &ch_info[i].descr_uuid;
                    req.conn = &conn;
                    req.len = value.len;
                    memcpy(req.value, value.value, value.len);
                    gattc_write_descr(&req);

                    break;
                }
            }

            if (i == max_char) {
                dev_info->status = WRITING_DONE;
            }
            break;
        }
        default:
            break;
    }
}


static void heart_rate_callback_init(app_callback_struct *app_gattc_cb)
{
    app_gattc_cb->scan_cb = heart_rate_scan_cb;
    app_gattc_cb->connect_cb = heart_rate_connect_cb;
    app_gattc_cb->search_complete_cb = heart_rate_search_complete_cb;
    app_gattc_cb->write_descriptor_cb = heart_rate_write_descriptor_cb;
    app_gattc_cb->read_descriptor_cb = heart_rate_read_descriptor_cb;
    app_gattc_cb->research_service_cb = NULL;
    app_gattc_cb->notify_cb = heart_rate_update_notify_cb;
}


static app_hrp_info_dev_struct *heart_rate_query_dev_info(app_query_info_type_enum type, void *para)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    int8_t i;
    app_hrp_info_dev_struct *dev_info = NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    for (i = 0; i < SRV_MAX_DEV; i++) {
        dev_info = &g_hrcp_cntx.dev_info[i];

        switch (type) {
            case BY_ADDRESS: {
                ble_address_t *addr = (ble_address_t *)para;
                if (addr) {
                    if (dev_info->dev_flag && !memcmp(addr, &dev_info->adr, sizeof(ble_address_t))) {
                        return dev_info;
                    }
                }

                break;
            }
            case BY_CONN_ID: {
                uint16_t *conn_id = (uint16_t *)para;
                if (dev_info->dev_flag && *conn_id == dev_info->conn_id) {
                    return dev_info;
                }
                break;
            }
            case BY_INDEX: {
                uint8_t index = 0;
                uint32_t tmp = (uint32_t) para;
                index = (uint8_t)tmp;
                if (index >= SRV_MAX_DEV) {
                    return  NULL;
                }
                dev_info = &g_hrcp_cntx.dev_info[index];
                if (dev_info) {
                    return dev_info;
                }
                break;
            }
        }
    }

    return NULL;
}


static app_hrp_info_dev_struct *heart_rate_create_new_dev_info(uint16_t conn_id, ble_address_t *addr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    uint8_t i;
    app_hrp_info_dev_struct *dev_info = NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    for (i = 0; i < SRV_MAX_DEV; i++) {
        dev_info = &g_hrcp_cntx.dev_info[i];
        if (dev_info->dev_flag == 0) {
            memset(dev_info, 0 , sizeof (app_hrp_info_dev_struct));
            dev_info->dev_flag |= 0x01;
            dev_info->conn_id = conn_id;
            dev_info->index = i;
            if (addr) {
                memcpy(dev_info->adr.address, addr->address, 6);
            }
            break;
        }
    }
    g_hrcp_cntx.ctotal_count++;
    return dev_info;
}


static void heart_rate_clear_dev_info(app_hrp_info_dev_struct *dev_info)
{
    if (dev_info) {
        memset(dev_info, 0x00, sizeof(app_hrp_info_dev_struct));
        g_hrcp_cntx.ctotal_count--;
    }
}


static app_hrp_context_struct *heart_rate_get_cntx(void)
{
    return &g_hrcp_cntx;
}

void heart_rate_task(void *arg)
{
    ble_event_t event_id;
    //void *param = NULL;
    LOG_I(app_hrc, "[HRAPP]App test task begin\r\n");
    /*queue ring buffer*/


    gattc_service_init();
    heart_rate_init();

    app_queue = xQueueCreate(APP_QUEUE_SIZE, sizeof(hear_rate_message_struct));
    if ( app_queue == NULL ) {
        LOG_I(app_hrc, "[HRAPP]create queue failed!\r\n");
        return;
    }
    memset((void *)&app_queue_data, 0, sizeof(hear_rate_message_struct));

    while (1) {
        if (xQueueReceive(app_queue, (void *)&app_queue_data, portMAX_DELAY)) {

            event_id = app_queue_data.event_id;
            //param =  app_queue_data.param;
            printf("[HRAPP]handle one message id = %d\r\n", app_queue_data.event_id);
            if (event_id == BLE_GAP_ADVERTISING_IND || event_id == BLE_GAP_CONNECTED_IND
                    || event_id == BLE_GAP_DISCONNECTED_IND) {
                ble_gap_event_handler(app_queue_data.event_id, app_queue_data.param);

            } else {
                ble_hrc_app_show_data(app_queue_data.event_id, app_queue_data.param);
            }
        }
    }

}


void heart_rate_task_init(void)
{
    //UBaseType_t size;
    xTaskCreate(heart_rate_task, "heart_rate_test_task", 240, NULL, 1, &xCreatedTask);

}

