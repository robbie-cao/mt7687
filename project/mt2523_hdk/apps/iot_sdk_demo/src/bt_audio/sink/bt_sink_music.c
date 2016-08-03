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

#include "bt_sink_music.h"

bt_sink_context_t g_bt_sink_cntx;

extern SinkProfileA2dp sink_a2dp[BT_SINK_CM_MAX_DEVICE_NUMBER];

extern SinkProfileAvrcp sink_avrcp[BT_SINK_CM_MAX_DEVICE_NUMBER];



void bt_sink_common_init(void)
{
    int32_t i = 0;

    memset(&g_bt_sink_cntx, 0x00, sizeof(bt_sink_context_t));

    for (i = 0; i < BT_SINK_CM_MAX_DEVICE_NUMBER; ++i) {
        g_bt_sink_cntx.sink_dev[i].a2dp = &sink_a2dp[i];
        g_bt_sink_cntx.sink_dev[i].avrcp = &sink_avrcp[i];
        /* init invalid cid */
        g_bt_sink_cntx.sink_dev[i].conn_id = BT_SINK_INVALID_CID;
    }
    g_bt_sink_cntx.partymode = BT_SINK_PARTYMODE_INTERRUPT;
    g_bt_sink_cntx.conn_id = BT_SINK_INVALID_CID;
    g_bt_sink_cntx.aud_id = BT_SINK_INVALID_AID;
    bt_sink_report("[sink][music]init-\r\n");
}


bt_sink_context_t *bt_sink_get_context(void)
{
    return &g_bt_sink_cntx;
}


bt_sink_device_t *bt_sink_get_device(bt_sink_device_type_t type, const void *param)
{
    bt_sink_device_t *dev = NULL;
    bt_sink_context_t *cntx = NULL;
    uint16_t cid = 0;
    bt_address_t *dev_addr = NULL;
    int32_t i = 0;
    uint16_t *p_cid = NULL;

    cntx = bt_sink_get_context();

    switch (type) {
        case BT_SINK_DEVICE_CID: {
            p_cid = (uint16_t *)param;
            cid = *p_cid;

            for (i = 0; i < BT_SINK_CM_MAX_DEVICE_NUMBER; ++i) {
                if (cntx->sink_dev[i].conn_id != BT_SINK_INVALID_CID
                        && cntx->sink_dev[i].conn_id == cid) {
                    dev = &(cntx->sink_dev[i]);
                    break;
                }
            }

            break;
        }

        case BT_SINK_DEVICE_ADDR: {
            dev_addr = (bt_address_t *)param;

            for (i = 0; i < BT_SINK_CM_MAX_DEVICE_NUMBER; ++i) {
                if (cntx->sink_dev[i].conn_id != BT_SINK_INVALID_CID
                        && (memcmp(dev_addr, &(cntx->sink_dev[i].dev_addr),
                                   sizeof(bt_address_t)) == 0)) {
                    dev = &(cntx->sink_dev[i]);
                    break;
                }
            }

            break;
        }

        case BT_SINK_DEVICE_UNUSED: {
            for (i = 0; i < BT_SINK_CM_MAX_DEVICE_NUMBER; ++i) {
                if (cntx->sink_dev[i].conn_id == BT_SINK_INVALID_CID) {
                    dev = &(cntx->sink_dev[i]);
                    break;
                }
            }

            break;
        }

        case BT_SINK_DEVICE_USED: {
            for (i = 0; i < BT_SINK_CM_MAX_DEVICE_NUMBER; ++i) {
                if (cntx->sink_dev[i].conn_id != BT_SINK_INVALID_CID) {
                    dev = &(cntx->sink_dev[i]);
                    break;
                }
            }

            break;
        }

        default:
            break;
    }

    bt_sink_report("[sink][music]get_dev-dev: 0x%x, type: %d, param: 0x%x\n",
                   dev, type, param);

    return dev;
}


void bt_sink_reset_device(bt_sink_device_t *dev)
{
    dev->conn_id = BT_SINK_INVALID_CID;
    dev->conn_bit = 0x00;
    dev->flag = 0x00;
}

