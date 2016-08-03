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

#include "bt_sink_avrcp.h"
#include "bt_sink_music.h"
#include "bt_sink_event.h"
#include "bt_sink_utils.h"

SinkProfileAvrcp sink_avrcp[BT_SINK_CM_MAX_DEVICE_NUMBER];


static void bt_sink_avrcp_init()
{
    uint32_t i;

    for (i = 0; i < BT_SINK_CM_MAX_DEVICE_NUMBER; i++) {
        sink_avrcp[i].profile.uuid = BT_SINK_UUID_AVRCP;
        sink_avrcp[i].profile.state = BT_SINK_AVRCP_DISCONNECTED;
    }
}


bt_status_t bt_sink_avrcp_get_init_params(bt_avrcp_init_params_t *param)
{
    bt_status_t ret = BT_STATUS_AVRCP_OK;

    if (param) {
        param->device_role = BT_AVRCP_ROLE_CT;
        param->ct_supported_feature = (bt_avrcp_support_feature_t)0x10;
        param->tg_supported_feature = (bt_avrcp_support_feature_t)0x00;
        //ret = 1;
    }
    bt_sink_report("[sink][avrcp]init-ret: %d\n", ret);
    return ret;
}


bt_sink_status_t bt_sink_avrcp_event_handler(bt_sink_event_id_t event, void *param)
{
    bt_status_t ret = BT_STATUS_SUCCESS;
    int32_t ret1 = 0;
    bt_address_t *dev_addr = NULL;
    bt_sink_device_t *dev = NULL;

    bt_sink_report("[sink][avrcp]process_avrcp_event[s]-event: 0x%x, base: 0x%x\n", event, BT_SINK_EVENT_AVRCP_CONNECT);

    switch (event) {
        case BT_SINK_EVENT_CM_PROFILE_INIT: {
            bt_sink_avrcp_init();
            break;
        }

        case BT_SINK_EVENT_AVRCP_CONNECT: {
            dev_addr = (bt_address_t *)param;
            ret = bt_avrcp_connect(dev_addr, BT_AVRCP_ROLE_CT);
            break;
        }

        case BT_SINK_EVENT_CM_PROFILE_CONNECT_REQ: {
            dev_addr = (bt_address_t *)param;
            /* ignore active connect */
            //ret = bt_avrcp_connect(dev_addr, BT_AVRCP_ROLE_CT);
            break;
        }

        case BT_SINK_EVENT_AVRCP_DISCONNECT:
        case BT_SINK_EVENT_CM_PROFILE_DISCONNECT_REQ: {
            ret = bt_avrcp_disconnect(*((uint16_t *)param));
            break;
        }

        case BT_SINK_EVENT_AVRCP_PLAY: {
            dev = bt_sink_get_device(BT_SINK_DEVICE_USED, NULL);
            ret1 = bt_sink_avrcp_play_music(dev->conn_id);
            break;
        }

        case BT_SINK_EVENT_AVRCP_PAUSE: {
            dev = bt_sink_get_device(BT_SINK_DEVICE_USED, NULL);
            ret1 = bt_sink_avrcp_stop_music(dev->conn_id);
            break;
        }

        case BT_SINK_EVENT_AVRCP_NEXT_TRACK: {
            dev = bt_sink_get_device(BT_SINK_DEVICE_USED, NULL);
            ret1 = bt_sink_avrcp_change_ntrack(dev->conn_id);
            break;
        }

        case BT_SINK_EVENT_AVRCP_PRE_TRACK: {
            dev = bt_sink_get_device(BT_SINK_DEVICE_USED, NULL);
            ret1 = bt_sink_avrcp_change_ptrack(dev->conn_id);
            break;
        }

        default:
            break;
    }

    bt_sink_report("[sink][avrcp]process_avrcp_event[e]-ret: %d, ret1: %d\n", ret, ret1);
    return BT_SINK_STATUS_SUCCESS;
}


void bt_sink_avrcp_common_callback(bt_event_t event_id, const void *param)
{
    int32_t ret = 0;
    SinkProfileAvrcp *avrcp = NULL;
    bt_sink_device_t *dev = NULL;
    bt_sink_avrcp_event_t avrcp_event;
    bt_sink_avrcp_disconnect_t avrcp_disconnect;
    bt_sink_avrcp_connect_t avrcp_connect;

    bt_sink_report("[sink][avrcp]common_hdr[s]-id: 0x%x\n", event_id);
    switch (event_id) {
        case BT_AVRCP_CONNECT_CNF: {
            bt_avrcp_connect_cnf_t *conn_cnf = (bt_avrcp_connect_cnf_t *)param;

            if (conn_cnf->result == BT_STATUS_AVRCP_OK) {
                /* avrcp connected */
                dev = bt_sink_get_device(BT_SINK_DEVICE_CID, (void *)(&(conn_cnf->conn_id)));
                if (dev) {
                    ;
                } else {
                    dev = bt_sink_get_device(BT_SINK_DEVICE_UNUSED, NULL);
                    dev->conn_id = conn_cnf->conn_id;
                    memcpy(&(dev->dev_addr), &(conn_cnf->device_addr), sizeof(bt_address_t));
                }
                dev->conn_bit |= BT_SINK_AVRCP_CONN_BIT;
                /* fill avrcp notify event */
                avrcp_connect.ret = 0;
                avrcp_connect.conn_id = dev->conn_id;
                memcpy(&(avrcp_connect.dev_addr), &(dev->dev_addr), sizeof(bt_address_t));
                avrcp = (SinkProfileAvrcp *)bt_sink_cm_find_profile_by_id(conn_cnf->conn_id, BT_SINK_UUID_AVRCP);
                if (avrcp) {
                    avrcp->profile.state = BT_SINK_AVRCP_CONNECTED;
                } else {
                    // trace error
                }
            } else {
                avrcp_connect.ret = -1;
            }

            avrcp_connect.ind = true;
            /* notify a2dp event */
            avrcp_event.event = BT_SINK_AVRCP_EVENT_CONNECT;
            avrcp_event.param = &avrcp_connect;
            bt_sink_event_send(BT_SINK_EVENT_AVRCP_NOTIFY, &avrcp_event);
            break;
        }

        case BT_AVRCP_CONNECT_IND: {
            bt_avrcp_connect_ind_t *conn_ind = (bt_avrcp_connect_ind_t *)param;

            ret = bt_avrcp_connect_response(&(conn_ind->device_addr), true);

            break;
        }

        case BT_AVRCP_DISCONNECT_IND: {
            bt_avrcp_disconnect_ind_t *disconn_ind = (bt_avrcp_disconnect_ind_t *)param;

            dev = bt_sink_get_device(BT_SINK_DEVICE_CID, (void *)(&(disconn_ind->conn_id)));
            if (dev) {
                dev->conn_bit &= ~(BT_SINK_AVRCP_CONN_BIT);
                /* fill avrcp notify event */
                avrcp_disconnect.ret = 0;
                avrcp_disconnect.conn_id = dev->conn_id;
                memcpy(&(avrcp_disconnect.dev_addr), &(dev->dev_addr), sizeof(bt_address_t));
                if (!(dev->conn_bit)) {
                    bt_sink_reset_device(dev);
                }
            }
            avrcp = (SinkProfileAvrcp *)bt_sink_cm_find_profile_by_id(disconn_ind->conn_id, BT_SINK_UUID_AVRCP);
            if (avrcp) {
                avrcp->profile.state = BT_SINK_AVRCP_DISCONNECTED;
            } else {
                // trace error
            }

            avrcp_disconnect.ind = true;
            /* notify a2dp event */
            avrcp_event.event = BT_SINK_AVRCP_EVENT_DISCONNECT;
            avrcp_event.param = &avrcp_disconnect;
            bt_sink_event_send(BT_SINK_EVENT_AVRCP_NOTIFY, &avrcp_event);

            break;
        }

        case BT_AVRCP_PASS_THROUGH_CNF: {
            bt_avrcp_pass_through_cnf_t *through_cnf = (bt_avrcp_pass_through_cnf_t *)param;

            if (through_cnf->result == BT_STATUS_AVRCP_OK) {
                if (through_cnf->key_state == BT_AVRCP_KEY_PRESS) {
                    ret = bt_avrcp_send_pass_through_command(through_cnf->conn_id, through_cnf->key_code, BT_AVRCP_KEY_RELEASE);
                } else if (through_cnf->key_state == BT_AVRCP_KEY_RELEASE) {
                    /* ignore */
                }
            }
            break;
        }

        default:
            break;
    }

    bt_sink_report("[sink][avrcp]common_hdr[e]-ret: %d\n", ret);
}


int32_t bt_sink_avrcp_play_music(uint16_t conn_id)
{
    int32_t ret = -3;
    bt_sink_device_t *dev = NULL;

    dev = bt_sink_get_device(BT_SINK_DEVICE_CID, (void *)(&conn_id));
    if (dev) {
        ret = bt_avrcp_send_pass_through_command(conn_id, BT_AVRCP_COMMAND_OPCODE_PLAY, BT_AVRCP_KEY_PRESS);
    }
    bt_sink_report("[sink][avrcp]play_music-cid: %d, ret: %d\n", conn_id, ret);
    return ret;
}


int32_t bt_sink_avrcp_stop_music(uint16_t conn_id)
{
    int32_t ret = -3;
    bt_sink_device_t *dev = NULL;

    dev = bt_sink_get_device(BT_SINK_DEVICE_CID, (void *)(&conn_id));
    if (dev) {
        ret = bt_avrcp_send_pass_through_command(conn_id, BT_AVRCP_COMMAND_OPCODE_PAUSE, BT_AVRCP_KEY_PRESS);
    }
    bt_sink_report("[sink][avrcp]stop_music-cid: %d, ret: %d\n", conn_id, ret);
    return ret;
}


int32_t bt_sink_avrcp_change_ntrack(uint16_t conn_id)
{
    int32_t ret = -3;
    bt_sink_device_t *dev = NULL;

    dev = bt_sink_get_device(BT_SINK_DEVICE_CID, (void *)(&conn_id));
    if (dev) {
        ret = bt_avrcp_send_pass_through_command(conn_id, BT_AVRCP_COMMAND_OPCODE_FORWARD, BT_AVRCP_KEY_PRESS);
    }
    bt_sink_report("[sink][avrcp]change_ntrack-cid: %d, ret: %d\n", conn_id, ret);
    return ret;
}


int32_t bt_sink_avrcp_change_ptrack(uint16_t conn_id)
{
    int32_t ret = -3;
    bt_sink_device_t *dev = NULL;

    dev = bt_sink_get_device(BT_SINK_DEVICE_CID, (void *)(&conn_id));
    if (dev) {
        ret = bt_avrcp_send_pass_through_command(conn_id, BT_AVRCP_COMMAND_OPCODE_BACKWARD, BT_AVRCP_KEY_PRESS);
    }
    bt_sink_report("[sink][avrcp]change_ptrack-cid: %d, ret: %d\n", conn_id, ret);
    return ret;
}

