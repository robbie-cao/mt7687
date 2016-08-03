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

#include "bt_sink_a2dp.h"
#include "bt_sink_music.h"
#include "bt_sink_event.h"
#include "bt_sink_ami.h"
#include "bt_gap.h"
#ifdef WIN32
#include "wavelib.h"
#include "sbc.h"
#endif
#include "bt_sink_utils.h"
#include "hal_audio.h"
#include <string.h>

#define BUF_SIZE 8192
#define BT_SINK_A2DP_DATA_RECE_CNT          (10)

#ifdef MTK_BT_A2DP_AAC_ENABLE
#define BT_SINK_A2DP_AAC_SUPPORT
#endif

#define AAC_ADTS_LENGTH                     (7)

#ifdef BT_SINK_A2DP_AAC_SUPPORT
#define BT_SINK_A2DP_SNK_SEP_NUM    (BT_SINK_CM_MAX_DEVICE_NUMBER * 2)
#else
#define BT_SINK_A2DP_SNK_SEP_NUM    (BT_SINK_CM_MAX_DEVICE_NUMBER * 1)
#endif

SinkProfileA2dp sink_a2dp[BT_SINK_CM_MAX_DEVICE_NUMBER];

static bt_a2dp_codec_capability_t cap[BT_SINK_A2DP_SNK_SEP_NUM];

static uint32_t bt_sink_a2dp_data_rece_cnt = 0;

static const bt_a2dp_sbc_codec_t sbc_cap_snk[1] = {
    {
        25,  // min_bit_pool
        75,  // max_bit_pool
        0xf, // block_len: all
        0xf, // subband_num: all
        0x3, // both snr/loudness
        0xf, // sample_rate: all
        0xf  // channel_mode: all
    }
};

static uint16_t g_a2dp_bit_pool = 0;
#ifdef BT_SINK_A2DP_AAC_SUPPORT
static bt_a2dp_aac_codec_t aac_cap_snk[1] = {
    {
    true,    /*VBR         */
    0xc0,    /*Object type */
    0x03,    /*Channels    */
    0x0ff8,  /*Sample_rate */
    0x60000  /*bit_rate, 384 Kbps */
    }
};
#endif

static const uint8_t aac_header[AAC_ADTS_LENGTH] = {0xFF, 0xF9, 0x40, 0x20,
                                                    0x00, 0x1F, 0xFC
                                                   };


#ifdef WIN32

HWAVELIB hd_wave = NULL;

uint8_t med_buf[BUF_SIZE];

#endif





bt_status_t bt_sink_a2dp_get_init_params(bt_a2dp_init_params_t *params)
{
    bt_status_t ret = BT_STATUS_A2DP_FATAL_ERROR;
    int32_t idx = 0;

    if (params) {
        params->role = BT_A2DP_SINK;
        /* init sink sep */
        params->sink_sep_num = BT_SINK_A2DP_SNK_SEP_NUM;
        idx = 0;
        while (idx < BT_SINK_A2DP_SNK_SEP_NUM) {
            cap[idx].type = BT_A2DP_CODEC_SBC;
            memcpy(&(cap[idx].codec.sbc), &sbc_cap_snk[0], sizeof(bt_a2dp_sbc_codec_t));
            ++idx;

#ifdef BT_SINK_A2DP_AAC_SUPPORT
            cap[idx].type = BT_A2DP_CODEC_AAC;
            memcpy(&(cap[idx].codec.sbc), &aac_cap_snk[0], sizeof(bt_a2dp_aac_codec_t));
            ++idx;
#endif
        }
        params->sink_sep = cap;

        /* init src sep */
        params->src_sep_num = 0;
        params->src_sep = NULL;

        ret = BT_STATUS_A2DP_OK;
    }

    bt_sink_report("[sink][a2dp]init-ret: %d\n", ret);

    return ret;
}

static void bt_sink_a2dp_init()
{
    uint32_t i;
    int8_t ori_aid = 0;
    bt_sink_context_t *cntx = NULL;

    for (i = 0; i < BT_SINK_CM_MAX_DEVICE_NUMBER; i++) {
        sink_a2dp[i].profile.uuid = BT_SINK_UUID_A2DP;
        sink_a2dp[i].profile.state = BT_SINK_PROFILE_STATE_DISCONNECTED;
    }

    cntx = bt_sink_get_context();
    ori_aid = cntx->aud_id;
    if (cntx->aud_id != BT_SINK_INVALID_AID) {
        bt_sink_ami_audio_close(cntx->aud_id);
        cntx->aud_id = BT_SINK_INVALID_AID;
    }
#ifdef __BT_SINK_AM_SUPPORT__
    cntx->aud_id = bt_sink_ami_audio_open(AUD_MIDDLE, bt_sink_a2dp_ami_hdr);
#endif /* __BT_SINK_AM_SUPPORT__ */
    bt_sink_report("[sink][a2dp]init-aud_id: %d, ori: %d\r\n",
                   cntx->aud_id, ori_aid);
}


static void bt_sink_a2dp_fill_aac_header(uint8_t *data, uint32_t len, bt_a2dp_aac_codec_t *aac)
{
    uint8_t freq_index = 0;
    uint8_t channel_num = 2; // Assume mono
    uint8_t temp;

    temp = aac->sample_rate;

    while (temp ^ 1) {
        freq_index++;
        temp >>= 1;
    }

    temp = aac->channels;
    if (temp == 1) {
        channel_num = 2;
    } else if (temp == 2) {
        channel_num = 1;
    } else {
        //configASSERT(0);  // Wrong channel
    }

    memcpy(data, aac_header, AAC_ADTS_LENGTH * sizeof(uint8_t));

    data[2] |= (freq_index << 2);
    data[3] |= (channel_num << 6) | (len >> 11);
    data[4] = (len >> 3) & 0xFF;
    data[5] |= (len & 0x07) << 5;
}


static void bt_sink_a2dp_fetch_media_header(uint8_t *data, bt_sink_a2dp_media_header_t *header)
{
    uint8_t temp = 0;

    temp = data[0];
    header->version = temp >> 6;
    header->padding = (temp & 0x20) >> 5;
    header->extension = (temp & 0x10) >> 4;
    header->csrc_count = temp & 0x0F;

    temp = data[1];
    header->marker = temp >> 7;
    header->payload_type = temp & 0x7F;
    header->sequence_number = (data[0] << 8) | data[0];
}


static void *bt_sink_a2dp_uppack_media_data(uint16_t conn_id, void *data, uint32_t data_len, uint32_t *payload_len, uint32_t *bit_pool)
{
    SinkProfileA2dp *a2dp = NULL;
    void *p_in = NULL;
    uint8_t *med = NULL;

    p_in = (uint8_t *)data + BT_SINK_MEDIA_PKT_HEADER_LEN;
    med = p_in;
    *payload_len = data_len - BT_SINK_MEDIA_PKT_HEADER_LEN;
    a2dp = (SinkProfileA2dp *)bt_sink_cm_find_profile_by_id(conn_id, BT_SINK_UUID_A2DP);
    // search the payload start pointer for AAC.
    if (a2dp->codec.type == BT_A2DP_CODEC_AAC) {
        uint8_t payload_offset = 9;
        uint8_t tempByte = 0;
        do {
            tempByte = ((uint8_t *)data)[BT_SINK_MEDIA_PKT_HEADER_LEN + payload_offset];
            payload_offset++;
        } while (tempByte == 0xFF);

        *payload_len -= payload_offset;
        med = (uint8_t *)data + BT_SINK_MEDIA_PKT_HEADER_LEN + payload_offset;

        med -= 7;
        *payload_len += 7;
        bt_sink_a2dp_fill_aac_header(med, *payload_len, &(a2dp->codec.codec.aac));
    } else if (a2dp->codec.type == BT_A2DP_CODEC_SBC) {
        //add sync word
        med = med - BT_SINK_MEDIA_SBC_SYNC_WORD_LEN;
        med[0] = (BT_SINK_MEDIA_SBC_SYNC_WORD & 0x00ff);
        med[1] = (BT_SINK_MEDIA_SBC_SYNC_WORD & 0xff00) >> 8;
        med[2] = (*payload_len & 0x00ff);
        med[3] = (*payload_len & 0xff00) >> 8;
        *payload_len = *payload_len + BT_SINK_MEDIA_SBC_SYNC_WORD_LEN;
        *bit_pool = ((uint8_t *)p_in)[3];
    }

    return med;
}


void bt_sink_a2dp_ami_hdr(bt_sink_am_id_t aud_id, bt_sink_am_cb_msg_class_t msg_id, bt_sink_am_cb_sub_msg_t sub_msg, void *parm)
{
    SinkProfileA2dp *a2dp = NULL;
    SinkProfileAvrcp *avrcp = NULL;
    bt_sink_context_t *cntx = NULL;
    int32_t ret = 0;
    int32_t err_code = 0;
    bt_sink_am_media_handle_t *media = NULL;
    bt_sink_a2dp_event_t a2dp_event;
    bt_sink_a2dp_play_t a2dp_play;

    cntx = bt_sink_get_context();

    if (cntx->aud_id == aud_id && msg_id == AUD_A2DP_PROC_IND && sub_msg == AUD_STREAM_EVENT_DATA_REQ) {
        // drop
        ;
    } else {
        bt_sink_report("[sink][a2dp]ami_hdr[s]-aid: %d, aud_id: %d, msg_id: %d, sub_msg: %d\n",
                       cntx->aud_id, aud_id, msg_id, sub_msg);
    }
    if (cntx->aud_id == aud_id) {
        switch (msg_id) {
            case AUD_SINK_OPEN_CODEC: {
                if (cntx->state == BT_SINK_AMI_PLAY) {
                    media = (bt_sink_am_media_handle_t *)parm;
                    avrcp = (SinkProfileAvrcp *)bt_sink_cm_find_profile_by_id(cntx->conn_id, BT_SINK_UUID_AVRCP);
                    a2dp = (SinkProfileA2dp *)bt_sink_cm_find_profile_by_id(cntx->conn_id, BT_SINK_UUID_A2DP);
                    if (avrcp && a2dp) {
                        memcpy(&(a2dp->med_hd), media, sizeof(bt_sink_am_media_handle_t));
                        a2dp->profile.state = BT_SINK_PROFILE_STATE_AUDIO_ON;
                        avrcp->profile.state = BT_SINK_AVRCP_PLAY;
                        cntx->state = BT_SINK_PLAY;
                        a2dp->header.sequence_number = 0;
                        ret = bt_a2dp_start_streaming_response(cntx->conn_id, true);

                        a2dp_play.conn_id = cntx->conn_id;
                        /* notify a2dp event */
                        a2dp_event.event = BT_SINK_A2DP_EVENT_PLAY;
                        a2dp_event.param = &a2dp_play;
                        bt_sink_event_send(BT_SINK_EVENT_A2DP_NOTIFY, &a2dp_event);

                        cntx->flag &= ~(BT_SINK_FLAG_PLAYING);
                        cntx->write_len = 0;
                    } else {
                        // trace error
                        err_code = -1;
                    }
                }
                break;
            }

            /* interrupt */
            case AUD_SUSPEND_BY_IND: {
                if (cntx->state == BT_SINK_PLAY) {
                    if (AUD_SUSPEND_BY_HFP == sub_msg) {
                        /* whether the same device */
                        if (cntx->conn_id == 12) {
                            /* ignore */
                        } else {
                            avrcp = (SinkProfileAvrcp *)bt_sink_cm_find_profile_by_id(cntx->conn_id, BT_SINK_UUID_AVRCP);
                            a2dp = (SinkProfileA2dp *)bt_sink_cm_find_profile_by_id(cntx->conn_id, BT_SINK_UUID_A2DP);
                            if (avrcp && a2dp) {
                                a2dp->profile.state = BT_SINK_PROFILE_STATE_CONNECTED;
                                avrcp->profile.state = BT_SINK_AVRCP_STOP;
                                cntx->state = BT_SINK_READY;
                                ret = bt_sink_avrcp_stop_music(cntx->conn_id);
                            } else {
                                // trace error
                                err_code = -2;
                            }
                        }
                    } else if (AUD_SUSPEND_BY_NONE == sub_msg) {
                        /* whether the same device */
                        if (cntx->conn_id == 12) {
                            /* ignore */
                        } else {
                            avrcp = (SinkProfileAvrcp *)bt_sink_cm_find_profile_by_id(cntx->conn_id, BT_SINK_UUID_AVRCP);
                            a2dp = (SinkProfileA2dp *)bt_sink_cm_find_profile_by_id(cntx->conn_id, BT_SINK_UUID_A2DP);
                            if (avrcp && a2dp) {
                                a2dp->profile.state = BT_SINK_PROFILE_STATE_CONNECTED;
                                avrcp->profile.state = BT_SINK_AVRCP_STOP;
                                cntx->state = BT_SINK_READY;
                                ret = bt_sink_avrcp_stop_music(cntx->conn_id);
                            } else {
                                // trace error
                                err_code = -2;
                            }
                        }
                    } else {
                        avrcp = (SinkProfileAvrcp *)bt_sink_cm_find_profile_by_id(cntx->conn_id, BT_SINK_UUID_AVRCP);
                        a2dp = (SinkProfileA2dp *)bt_sink_cm_find_profile_by_id(cntx->conn_id, BT_SINK_UUID_A2DP);
                        if (avrcp && a2dp) {
                            a2dp->profile.state = BT_SINK_PROFILE_STATE_CONNECTED;
                            avrcp->profile.state = BT_SINK_AVRCP_STOP;
                            cntx->state = BT_SINK_READY;
                            ret = bt_sink_avrcp_stop_music(cntx->conn_id);
                        } else {
                            // trace error
                            err_code = -3;
                        }
                    }
                }
                break;
            }

            case AUD_RESUME_IND: {
                break;
            }

            case AUD_A2DP_PROC_IND: {
                avrcp = (SinkProfileAvrcp *)bt_sink_cm_find_profile_by_id(cntx->conn_id, BT_SINK_UUID_AVRCP);
                a2dp = (SinkProfileA2dp *)bt_sink_cm_find_profile_by_id(cntx->conn_id, BT_SINK_UUID_A2DP);
                if (avrcp && a2dp) {
                    media = &(a2dp->med_hd);
                    //media->stop(cntx->aud_id);
                    //media->play(cntx->aud_id);
                    //media->process(cntx->aud_id, sub_msg);
                } else {
                    // trace error
                    err_code = -4;
                }
                break;
            }
            default:
                break;
        }
    }

    if (cntx->aud_id == aud_id && msg_id == AUD_A2DP_PROC_IND && sub_msg == AUD_STREAM_EVENT_DATA_REQ) {
        // drop
        ;
    } else {
        bt_sink_report("[sink][a2dp]ami_hdr[e]-err_code: %d, ret: %d\r\n", err_code, ret);
    }
}


static int32_t bt_sink_a2dp_handle_connect_cnf(bt_a2dp_connect_cnf_t *conn_cnf)
{
    SinkProfileA2dp *a2dp = NULL;
    bt_sink_device_t *dev = NULL;
    bt_sink_context_t *cntx = NULL;
    bt_sink_a2dp_event_t a2dp_event;
    bt_sink_a2dp_connect_t a2dp_connect;
    int32_t ret = -110;

    cntx = bt_sink_get_context();
    a2dp = (SinkProfileA2dp *)bt_sink_cm_find_profile_by_id(conn_cnf->conn_id, BT_SINK_UUID_A2DP);
    if (a2dp) {
        a2dp->profile.state = BT_SINK_PROFILE_STATE_CONNECTED;
    }

    if (conn_cnf->status == BT_STATUS_A2DP_OK) {
        /* a2dp connected */
        dev = bt_sink_get_device(BT_SINK_DEVICE_CID, (void *)(&(conn_cnf->conn_id)));
        if (dev) {
            ;
        } else {
            dev = bt_sink_get_device(BT_SINK_DEVICE_UNUSED, NULL);
        }
        /* as slave */
        bt_gap_set_role(&(conn_cnf->dev_addr), BT_GAP_ROLE_SLAVE);
        dev->conn_id = conn_cnf->conn_id;
        memcpy(&(dev->dev_addr), &(conn_cnf->dev_addr), sizeof(bt_address_t));
        dev->conn_bit |= BT_SINK_A2DP_CONN_BIT;

        /* 1st dev a2dp connect*/
        if (cntx->state == BT_SINK_IDLE) {
            cntx->state = BT_SINK_READY;
        }

        /* fill notify event struct */
        a2dp_connect.ret = 0;
        a2dp_connect.conn_id = conn_cnf->conn_id;
        memcpy(&(a2dp_connect.dev_addr), &(conn_cnf->dev_addr), sizeof(bt_address_t));

        if ((dev->flag & BT_SINK_FLAG_A2DP_CON_IND)) {
            a2dp_connect.ind = true;
        } else {
            a2dp_connect.ind = false;
        }
        #if 0
        if ((dev->flag & BT_SINK_FLAG_A2DP_CON_IND) && (!(dev->conn_bit & BT_SINK_AVRCP_CONN_BIT))) {
            /* connect avrcp */
            bt_avrcp_connect(&(dev->dev_addr), BT_AVRCP_ROLE_CT);
        }
        /* active connect avrcp */
        if ((!(dev->flag & BT_SINK_FLAG_A2DP_CON_IND)) && (!(dev->conn_bit & BT_SINK_AVRCP_CONN_BIT))) {
            /* connect avrcp */
            bt_avrcp_connect(&(dev->dev_addr), BT_AVRCP_ROLE_CT);
        }
        #endif
        if (!(dev->conn_bit & BT_SINK_AVRCP_CONN_BIT)) {
            ret = bt_avrcp_connect(&(dev->dev_addr), BT_AVRCP_ROLE_CT);
            bt_sink_report("[sink][a2dp]connect_cnf-ret: %d, flag: 0x%x\n", ret, dev->flag);
        }
    } else {
        /* fill notify event struct */
        a2dp_connect.ret = -1;
        dev = bt_sink_get_device(BT_SINK_DEVICE_CID, (void *)(&(conn_cnf->conn_id)));
        if (dev) {
            if ((dev->flag & BT_SINK_FLAG_A2DP_CON_IND)) {
                a2dp_connect.ind = true;
            } else {
                a2dp_connect.ind = false;
            }
            bt_sink_reset_device(dev);
        }
    }

    /* notify a2dp event */
    a2dp_event.event = BT_SINK_A2DP_EVENT_CONNECT;
    a2dp_event.param = &a2dp_connect;
    bt_sink_event_send(BT_SINK_EVENT_A2DP_NOTIFY, &a2dp_event);

    return ret;
}


static int32_t bt_sink_a2dp_handle_connect_ind(bt_a2dp_connect_ind_t *conn_ind)
{
    bt_sink_device_t *dev = NULL;
    int32_t ret = 0;

    /* a2dp connected */
    dev = bt_sink_get_device(BT_SINK_DEVICE_CID, (void *)(&(conn_ind->conn_id)));
    if (dev) {
        ;
    } else {
        dev = bt_sink_get_device(BT_SINK_DEVICE_UNUSED, NULL);
    }
    if (dev) {
        dev->conn_id = conn_ind->conn_id;
        memcpy(&(dev->dev_addr), &(conn_ind->dev_addr), sizeof(bt_address_t));
        dev->flag |= BT_SINK_FLAG_A2DP_CON_IND;
    }
    ret = bt_a2dp_connect_response(conn_ind->conn_id, true);

    return ret;
}


static int32_t bt_sink_a2dp_handle_disconnect_cnf(bt_a2dp_disconnect_cnf_t *disconn_cnf)
{
    SinkProfileA2dp *a2dp = NULL;
    bt_sink_device_t *dev = NULL;
    bt_sink_context_t *cntx = NULL;
    int32_t ret = 0;
    bt_sink_a2dp_event_t a2dp_event;
    bt_sink_a2dp_disconnect_t a2dp_disconnect;

    cntx = bt_sink_get_context();
    if (disconn_cnf->status == BT_STATUS_A2DP_OK) {
        a2dp = (SinkProfileA2dp *)bt_sink_cm_find_profile_by_id(disconn_cnf->conn_id, BT_SINK_UUID_A2DP);
        if (a2dp) {
            a2dp->profile.state = BT_SINK_PROFILE_STATE_DISCONNECTED;
            memset(&(a2dp->header), 0x00, sizeof(bt_sink_a2dp_media_header_t));
        }

        dev = bt_sink_get_device(BT_SINK_DEVICE_CID, (void *)(&(disconn_cnf->conn_id)));
        if (dev) {
            dev->conn_bit &= ~(BT_SINK_A2DP_CONN_BIT);
            a2dp_disconnect.conn_id = dev->conn_id;
            memcpy(&(a2dp_disconnect.dev_addr), &(dev->dev_addr), sizeof(bt_address_t));
            if (!(dev->conn_bit)) {
                bt_sink_reset_device(dev);
            }
        }

        if (cntx->conn_id == disconn_cnf->conn_id) {
            cntx->conn_id = BT_SINK_INVALID_CID;
            cntx->state = BT_SINK_IDLE;
        }
        a2dp_disconnect.ret = 0;
    } else {
        a2dp_disconnect.ret = -1;
    }

    a2dp_disconnect.ind = false;
    /* notify a2dp event */
    a2dp_event.event = BT_SINK_A2DP_EVENT_DISCONNECT;
    a2dp_event.param = &a2dp_disconnect;
    bt_sink_event_send(BT_SINK_EVENT_A2DP_NOTIFY, &a2dp_event);

    return ret;
}


static int32_t bt_sink_a2dp_handle_disconnect_ind(bt_a2dp_disconnect_ind_t *disconn_ind)
{
    SinkProfileA2dp *a2dp = NULL;
    bt_sink_device_t *dev = NULL;
    bt_sink_context_t *cntx = NULL;
    int32_t ret = -101, err = -102;
    bt_sink_a2dp_event_t a2dp_event;
    bt_sink_a2dp_disconnect_t a2dp_disconnect;

    cntx = bt_sink_get_context();

    bt_sink_report("[sink][a2dp]disconnect_ind-state: %d\n", cntx->state);

    a2dp = (SinkProfileA2dp *)bt_sink_cm_find_profile_by_id(disconn_ind->conn_id, BT_SINK_UUID_A2DP);
    if (a2dp) {
        a2dp->profile.state = BT_SINK_PROFILE_STATE_DISCONNECTED;
        memset(&(a2dp->header), 0x00, sizeof(bt_sink_a2dp_media_header_t));
    }

    dev = bt_sink_get_device(BT_SINK_DEVICE_CID, (void *)(&(disconn_ind->conn_id)));
    if (dev) {
        dev->conn_bit &= ~(BT_SINK_A2DP_CONN_BIT);
        a2dp_disconnect.conn_id = dev->conn_id;
        memcpy(&(a2dp_disconnect.dev_addr), &(dev->dev_addr), sizeof(bt_address_t));
        if (!(dev->conn_bit)) {
            bt_sink_reset_device(dev);
        }
    }
    if (cntx->conn_id == disconn_ind->conn_id) {
        if (cntx->state == BT_SINK_PLAY) {
            err = a2dp->med_hd.stop(cntx->aud_id);
            ret = bt_sink_ami_audio_stop(cntx->aud_id);
            bt_sink_report("[sink][a2dp]disconnect_ind-ret: %d, err: %d\n", ret, err);
        }
        cntx->conn_id = BT_SINK_INVALID_CID;
        cntx->state = BT_SINK_IDLE;
    }

    a2dp_disconnect.ret = 0;
    a2dp_disconnect.ind = true;
    /* notify a2dp event */
    a2dp_event.event = BT_SINK_A2DP_EVENT_DISCONNECT;
    a2dp_event.param = &a2dp_disconnect;
    bt_sink_event_send(BT_SINK_EVENT_A2DP_NOTIFY, &a2dp_event);

    return ret;
}


static int32_t bt_sink_a2dp_handle_strat_streaming_cnf(bt_a2dp_start_streaming_cnf_t *start_cnf)
{
    return 0;
}


static int32_t bt_sink_a2dp_handle_strat_streaming_ind(bt_a2dp_start_streaming_ind_t *start_ind)
{
    SinkProfileA2dp *a2dp = NULL;
    SinkProfileAvrcp *avrcp = NULL;
    bt_sink_context_t *cntx = NULL;
    int32_t ret = -100;
    bt_sink_am_id_t ami_ret = 0;
    bt_sink_am_audio_capability_t  aud_cap;
    bt_a2dp_sbc_codec_t *sbc_arg = &(start_ind->codec_cap.codec.sbc);

    cntx = bt_sink_get_context();

    bt_sink_report("[sink][a2dp]handle_strat_streaming_ind[s]-state: %d, pd: %d\r\n",
                   cntx->state, cntx->partymode);

    bt_sink_report("[HQA][A2DP] STREAMING, SR:%d, CH_M:%d", sbc_arg->sample_rate, sbc_arg->channel_mode);

#ifdef WIN32
    avrcp = (SinkProfileAvrcp *)bt_sink_cm_find_profile_by_id(start_ind->conn_id, BT_SINK_UUID_AVRCP);
    a2dp = (SinkProfileA2dp *)bt_sink_cm_find_profile_by_id(start_ind->conn_id, BT_SINK_UUID_A2DP);
    a2dp->profile.state = BT_SINK_PROFILE_STATE_AUDIO_ON;
    avrcp->profile.state = BT_SINK_AVRCP_PLAY;
    cntx->state = BT_SINK_PLAY;
    cntx->conn_id = start_ind->conn_id;
    memcpy(&a2dp->codec, &(start_ind->codec_cap), sizeof(bt_a2dp_codec_capability_t));

    bt_a2dp_start_streaming_response(start_ind->conn_id, true);
#else
    /* always accept to avoid IOT issue */
    //bt_a2dp_start_streaming_response(start_ind->conn_id, true);
    if (cntx->state == BT_SINK_PLAY) {
        /* party mode */
        switch (cntx->partymode) {
            case BT_SINK_PARTYMODE_INTERRUPT: {
                /* change state and stop pre music */
                avrcp = (SinkProfileAvrcp *)bt_sink_cm_find_profile_by_id(cntx->conn_id, BT_SINK_UUID_AVRCP);
                a2dp = (SinkProfileA2dp *)bt_sink_cm_find_profile_by_id(cntx->conn_id, BT_SINK_UUID_A2DP);
                if (avrcp && a2dp) {
                    int32_t ret1 = -1, ret2 = -1, ret3 = -1;
                    a2dp->profile.state = BT_SINK_PROFILE_STATE_CONNECTED;
                    ret1 = bt_sink_avrcp_stop_music(cntx->conn_id);
                    cntx->conn_id = start_ind->conn_id;
                    ret2 = a2dp->med_hd.stop(cntx->aud_id);
                    cntx->state = BT_SINK_AMI_STOP;
                    ret3 = bt_sink_ami_audio_stop(cntx->aud_id);
                    bt_sink_report("[sink][a2dp]start_ind(int)-ret1: %d, ret2: %d, ret3: %d\n", ret1, ret2, ret3);
                } else {
                    // trace error
                    ret = -(1000 + 0);
                }

                avrcp = (SinkProfileAvrcp *)bt_sink_cm_find_profile_by_id(start_ind->conn_id, BT_SINK_UUID_AVRCP);
                a2dp = (SinkProfileA2dp *)bt_sink_cm_find_profile_by_id(start_ind->conn_id, BT_SINK_UUID_A2DP);
                if (avrcp && a2dp) {
                    cntx->conn_id = start_ind->conn_id;
                    cntx->state = BT_SINK_AMI_PLAY;
                    /* fill aud param */
                    memset(&aud_cap, 0x00, sizeof(bt_sink_am_audio_capability_t));
                    aud_cap.type = A2DP;
                    aud_cap.codec.a2dp_format.a2dp_codec.role = BT_A2DP_SINK;
                    memcpy(&(aud_cap.codec.a2dp_format.a2dp_codec.codec_cap), &(start_ind->codec_cap), sizeof(bt_a2dp_codec_capability_t));
                    aud_cap.audio_stream_out.audio_device = HAL_AUDIO_DEVICE_HEADSET;
                    aud_cap.audio_stream_out.audio_volume = AUD_VOL_OUT_LEVEL6;
                    aud_cap.audio_stream_out.audio_mute = false;
                    ami_ret = bt_sink_ami_audio_play(cntx->aud_id, &aud_cap);
                    memcpy(&a2dp->codec, &(start_ind->codec_cap), sizeof(bt_a2dp_codec_capability_t));
                } else {
                    // trace error
                    ret = -(1000 + 1);
                }
                break;
            }

            case BT_SINK_PARTYMODE_DROP: {
                if (start_ind->conn_id != cntx->conn_id) {
                    bt_a2dp_start_streaming_response(start_ind->conn_id, true);
                    avrcp = (SinkProfileAvrcp *)bt_sink_cm_find_profile_by_id(start_ind->conn_id, BT_SINK_UUID_AVRCP);
                    bt_sink_avrcp_stop_music(start_ind->conn_id);
                    if (avrcp) {
                        avrcp->profile.state = BT_SINK_AVRCP_STOP;
                    } else {
                        // trace error
                        ret = -(1000 + 2);
                    }
                }
                break;
            }

            default:
                break;
        }
    } else if (cntx->state == BT_SINK_READY) {
        avrcp = (SinkProfileAvrcp *)bt_sink_cm_find_profile_by_id(start_ind->conn_id, BT_SINK_UUID_AVRCP);
        a2dp = (SinkProfileA2dp *)bt_sink_cm_find_profile_by_id(start_ind->conn_id, BT_SINK_UUID_A2DP);
        if (avrcp && a2dp) {
            //a2dp->profile.state = BT_SINK_A2DP_STREAMING;
            //avrcp->profile.state = BT_SINK_AVRCP_PLAY;
            cntx->conn_id = start_ind->conn_id;
            //cntx->state = BT_SINK_PLAY;
            cntx->state = BT_SINK_AMI_PLAY;
            /* fill aud param */
            memset(&aud_cap, 0x00, sizeof(bt_sink_am_audio_capability_t));
            aud_cap.type = A2DP;
            aud_cap.codec.a2dp_format.a2dp_codec.role = BT_A2DP_SINK;
            memcpy(&(aud_cap.codec.a2dp_format.a2dp_codec.codec_cap), &(start_ind->codec_cap), sizeof(bt_a2dp_codec_capability_t));
#if 1
            aud_cap.audio_stream_out.audio_device = HAL_AUDIO_DEVICE_HEADSET;
            aud_cap.audio_stream_out.audio_volume = AUD_VOL_OUT_LEVEL6;
            aud_cap.audio_stream_out.audio_mute = false;
            ami_ret = bt_sink_ami_audio_play(cntx->aud_id, &aud_cap);
            bt_sink_report("[sink][a2dp]start_ind(am)--ret: %d\r\n", ami_ret);
#else
            bt_sink_report("[sink][a2dp]common_hdr without\r\n");
            bt_a2dp_start_streaming_response(start_ind->conn_id, true);
            cntx->state = BT_SINK_PLAY;
#endif
            memcpy(&a2dp->codec, &(start_ind->codec_cap), sizeof(bt_a2dp_codec_capability_t));
        } else {
            // trace error
            ret = -(1000 + 3);
        }
    } else if ((cntx->state == BT_SINK_AMI_PLAY)) {
        /* party mode */
        switch (cntx->partymode) {
            case BT_SINK_PARTYMODE_INTERRUPT: {
                /* change state and stop pre music */
                avrcp = (SinkProfileAvrcp *)bt_sink_cm_find_profile_by_id(cntx->conn_id, BT_SINK_UUID_AVRCP);
                a2dp = (SinkProfileA2dp *)bt_sink_cm_find_profile_by_id(cntx->conn_id, BT_SINK_UUID_A2DP);
                if (avrcp && a2dp) {
                    a2dp->profile.state = BT_SINK_PROFILE_STATE_CONNECTED;
                    avrcp->profile.state = BT_SINK_AVRCP_STOP;
                    bt_a2dp_start_streaming_response(cntx->conn_id, true);
                    bt_sink_avrcp_stop_music(cntx->conn_id);
                } else {
                    // trace error
                    ret = -(1000 + 4);
                }
                avrcp = (SinkProfileAvrcp *)bt_sink_cm_find_profile_by_id(start_ind->conn_id, BT_SINK_UUID_AVRCP);
                a2dp = (SinkProfileA2dp *)bt_sink_cm_find_profile_by_id(start_ind->conn_id, BT_SINK_UUID_A2DP);
                if (avrcp && a2dp) {
                    cntx->conn_id = start_ind->conn_id;
                    cntx->state = BT_SINK_AMI_PLAY;
                    /* fill aud param */
                    memset(&aud_cap, 0x00, sizeof(bt_sink_am_audio_capability_t));
                    aud_cap.type = A2DP;
                    aud_cap.codec.a2dp_format.a2dp_codec.role = BT_A2DP_SINK;
                    memcpy(&(aud_cap.codec.a2dp_format.a2dp_codec.codec_cap), &(start_ind->codec_cap), sizeof(bt_a2dp_codec_capability_t));
                    aud_cap.audio_stream_out.audio_device = HAL_AUDIO_DEVICE_HEADSET;
                    aud_cap.audio_stream_out.audio_volume = AUD_VOL_OUT_LEVEL6;
                    aud_cap.audio_stream_out.audio_mute = false;
                    ami_ret = bt_sink_ami_audio_play(cntx->aud_id, &aud_cap);
                    memcpy(&a2dp->codec, &(start_ind->codec_cap), sizeof(bt_a2dp_codec_capability_t));
                } else {
                    // trace error
                    ret = -(1000 + 5);
                }
                break;
            }

            case BT_SINK_PARTYMODE_DROP: {
                if (start_ind->conn_id != cntx->conn_id) {
                    bt_a2dp_start_streaming_response(start_ind->conn_id, true);
                    avrcp = (SinkProfileAvrcp *)bt_sink_cm_find_profile_by_id(start_ind->conn_id, BT_SINK_UUID_AVRCP);
                    bt_sink_avrcp_stop_music(start_ind->conn_id);
                    if (avrcp) {
                        avrcp->profile.state = BT_SINK_AVRCP_STOP;
                    } else {
                        // trace error
                        ret = -(1000 + 6);
                    }
                }
                break;
            }

            default:
                break;
        }
    } else {
        /* upexpected state */
        ret = -(1000 + 7);
    }
#endif

    return ret;
}


static int32_t bt_sink_a2dp_handle_suspend_streaming_cnf(bt_a2dp_suspend_streaming_cnf_t *suspend_cnf)
{
    bt_sink_context_t *cntx = NULL;
    int32_t ret = 0;

    cntx = bt_sink_get_context();
#ifdef WIN32
    if (suspend_cnf->status == BT_STATUS_A2DP_OK) {
        bt_sink_report("remote device agree with supending streaming");
        WaveLib_Pause(hd_wave, true);
    } else {
        bt_sink_report("remote device disagree with suspending streaming");
    }
#endif
    cntx->state = BT_SINK_READY;

    return ret;
}


static int32_t bt_sink_a2dp_handle_suspend_streaming_ind(bt_a2dp_suspend_streaming_ind_t *suspend_ind)
{
    SinkProfileA2dp *a2dp = NULL;
    SinkProfileAvrcp *avrcp = NULL;
    bt_sink_context_t *cntx = NULL;
    int32_t ret = 0, err = 0;
    bt_sink_a2dp_event_t a2dp_event;
    bt_sink_a2dp_suspend_t a2dp_suspend;

    cntx = bt_sink_get_context();
    a2dp = (SinkProfileA2dp *)bt_sink_cm_find_profile_by_id(suspend_ind->conn_id, BT_SINK_UUID_A2DP);
    ret = bt_a2dp_suspend_streaming_response(suspend_ind->conn_id, true);
    if ((cntx->state == BT_SINK_PLAY) && (cntx->conn_id == suspend_ind->conn_id)) {
        cntx->state = BT_SINK_READY;
        err = a2dp->med_hd.stop(cntx->aud_id);
        ret = bt_sink_ami_audio_stop(cntx->aud_id);
        bt_sink_report("[sink][a2dp]suspend_streaming_ind(am)-ret: %d, err: %d\n", ret, err);
    } else {
        ;
    }
    a2dp = (SinkProfileA2dp *)bt_sink_cm_find_profile_by_id(suspend_ind->conn_id, BT_SINK_UUID_A2DP);
    avrcp = (SinkProfileAvrcp *)bt_sink_cm_find_profile_by_id(suspend_ind->conn_id, BT_SINK_UUID_AVRCP);
    if (avrcp && a2dp) {
        a2dp->profile.state = BT_SINK_PROFILE_STATE_CONNECTED;
        memset(&(a2dp->header), 0x00, sizeof(bt_sink_a2dp_media_header_t));
        avrcp->profile.state = BT_SINK_AVRCP_STOP;
    } else {
        // trace error
    }

    a2dp_suspend.conn_id = suspend_ind->conn_id;
    /* notify a2dp event */
    a2dp_event.event = BT_SINK_A2DP_EVENT_SUSPEND;
    a2dp_event.param = &a2dp_suspend;
    bt_sink_event_send(BT_SINK_EVENT_A2DP_NOTIFY, &a2dp_event);

    return ret;
}


static int32_t bt_sink_a2dp_handle_reconfigure_cnf(bt_a2dp_reconfigure_cnf_t *reconfigure_cnf)
{
    return 0;
}


static int32_t bt_sink_a2dp_handle_reconfigure_ind(bt_a2dp_reconfigure_ind_t *reconfigure_ind)
{
    SinkProfileA2dp *a2dp = NULL;
    int32_t ret = 0;

    a2dp = (SinkProfileA2dp *)bt_sink_cm_find_profile_by_id(reconfigure_ind->conn_id, BT_SINK_UUID_A2DP);
    if (a2dp) {
        memcpy(&a2dp->codec, &(reconfigure_ind->codec_cap), sizeof(bt_a2dp_codec_capability_t));
    }
    ret = bt_a2dp_reconfigure_response(reconfigure_ind->conn_id, true);

    return ret;
}


static int32_t bt_sink_a2dp_handle_data_received_ind(bt_a2dp_stream_data_received_ind_t *data_ind)
{
    SinkProfileA2dp *a2dp = NULL;
    bt_sink_device_t *dev = NULL;
    bt_sink_context_t *cntx = NULL;
    int32_t ret = 0;
    bt_sink_am_media_handle_t *med_hd = NULL;
    uint32_t free_len = 0, cur_len = 0, write_len = 0;
    uint8_t *ring_buf = NULL;
    uint8_t dev_db = 0;
    // bt_sink_am_volume_index_t am_volume;
    uint8_t *med_data = NULL;
    uint32_t payload_size = 0;
    bt_sink_a2dp_event_t a2dp_event;
    bt_sink_a2dp_suspend_t a2dp_streaming;
    uint32_t bit_pool = 0;
    cntx = bt_sink_get_context();
    if ((cntx->state == BT_SINK_PLAY) &&
            (cntx->conn_id == data_ind->conn_id)) {
        a2dp = (SinkProfileA2dp *)bt_sink_cm_find_profile_by_id(data_ind->conn_id, BT_SINK_UUID_A2DP);
        med_hd = &(a2dp->med_hd);

        if (a2dp->header.sequence_number == 0) {
            bt_sink_a2dp_fetch_media_header(data_ind->data, &(a2dp->header));
        }

        med_data = bt_sink_a2dp_uppack_media_data(data_ind->conn_id,
                   data_ind->data, data_ind->payload_size, &payload_size, &bit_pool);
        if (g_a2dp_bit_pool != bit_pool) {
            g_a2dp_bit_pool = bit_pool;
            bt_sink_report("[HQA][A2DP] New bit_pool: %d, FRM_HDR:%02x%02x%02x", bit_pool, med_data[5], med_data[6], med_data[7]);
        }

#ifdef WIN32
        if (a2dp) {
            len = A2DP_decodec_sbc(a2dp->codec, payload_size, med_data, med_buf, BUF_SIZE);
            A2DP_play_raw_data(a2dp->codec, med_buf, len);
        }
#else
        free_len = med_hd->get_free_space(cntx->aud_id);
        if (free_len <= payload_size) {
            /* error */
            bt_sink_report("[sink][a2dp]handle_data(error)-free_len: %d, pay_size: %d\n", free_len, payload_size);
        } else {
            med_hd->get_write_buffer(cntx->aud_id, &ring_buf, &write_len);
            if (write_len < payload_size) {
                /* first part */
                memcpy(ring_buf, med_data, write_len);
                med_hd->write_data_done(cntx->aud_id, write_len);
                cur_len = write_len;

                /* second part */
                med_hd->get_write_buffer(cntx->aud_id, &ring_buf, &write_len);
                memcpy(ring_buf, med_data + cur_len, payload_size - cur_len);
                //med_hd->write_data_done(cntx->aud_id, write_len);
                med_hd->write_data_done(cntx->aud_id, payload_size - cur_len);
            } else {
                memcpy(ring_buf, med_data, payload_size);
                med_hd->write_data_done(cntx->aud_id, payload_size);
            }

            med_hd->finish_write_data(cntx->aud_id);
            cntx->write_len += payload_size;
            if (!(cntx->flag & BT_SINK_FLAG_PLAYING)) {
                if (cntx->write_len > BT_SINK_A2DP_DATA_THRESHHOLD) {
                    cntx->flag |= (BT_SINK_FLAG_PLAYING);
                    dev = bt_sink_get_device(BT_SINK_DEVICE_CID, (void *)(&(cntx->conn_id)));
                    if (bt_sink_cm_get_profile_data(dev->conn_id, BT_SINK_UUID_A2DP, &dev_db)) {
                        if (dev_db == BT_SINK_A2DP_MAGIC_CODE) {
                            /* use storge volume value */
                        } else {
                            /* use dedefault volume value and update it */
                            dev_db = BT_SINK_A2DP_MAGIC_CODE;
                            bt_sink_cm_set_profile_data(dev->conn_id, BT_SINK_UUID_A2DP, &dev_db);
                        }
                    }

                    //ami_ret = bt_sink_ami_audio_set_volume(cntx->aud_id, am_volume);
                    ret = med_hd->play(cntx->aud_id);
                    //hal_audio_set_stream_out_device(HAL_AUDIO_DEVICE_HEADSET);
                    //*(volatile uint16_t *)(0xA21C00B4) = 0x7E31;
                    bt_sink_report("[sink][a2dp]handle_data-start play--ret: 0x%x\n", ret);
                }
            }
        }
#endif
    } else {
        /* drop media data */
        bt_sink_report("[sink][a2dp]drop_data-state: %d, c_id: %d, d_id: %d\n", cntx->state, cntx->conn_id, data_ind->conn_id);
    }

    if ((bt_sink_a2dp_data_rece_cnt < BT_SINK_A2DP_DATA_RECE_CNT)) {
        a2dp_streaming.conn_id = data_ind->conn_id;
        /* notify a2dp event */
        a2dp_event.event = BT_SINK_A2DP_EVENT_STREAMING;
        a2dp_event.param = &a2dp_streaming;
        bt_sink_event_send(BT_SINK_EVENT_A2DP_NOTIFY, &a2dp_event);
    }

    return ret;
}


bt_sink_status_t bt_sink_a2dp_event_handler(bt_sink_event_id_t event, void *param)
{
    bt_status_t ret = BT_STATUS_SUCCESS;
    bt_address_t *dev_addr = NULL;
    bt_sink_context_t *cntx = NULL;

    bt_sink_report("[sink][a2dp]process_a2dp_event[s]-event: 0x%x, base: 0x%x\n", event, BT_SINK_EVENT_A2DP_CONNECT);
    cntx = bt_sink_get_context();
    switch (event) {
        case BT_SINK_EVENT_CM_PROFILE_INIT: {
            bt_sink_a2dp_init();
            break;
        }

        case BT_SINK_EVENT_CM_PROFILE_DEINIT: {
            if (cntx->aud_id != BT_SINK_INVALID_AID) {
                bt_sink_ami_audio_close(cntx->aud_id);
                cntx->aud_id = BT_SINK_INVALID_AID;
            }
            break;
        }

        case BT_SINK_EVENT_A2DP_CONNECT:
        case BT_SINK_EVENT_CM_PROFILE_CONNECT_REQ: {
            dev_addr = (bt_address_t *)param;
            ret = bt_a2dp_connect(dev_addr, BT_A2DP_SINK);
            break;
        }

        case BT_SINK_EVENT_A2DP_DISCONNECT:
        case BT_SINK_EVENT_CM_PROFILE_DISCONNECT_REQ: {
            ret = bt_a2dp_disconnect(*((uint16_t *)param));
            break;
        }

        default:
            break;
    }

    bt_sink_report("[sink][a2dp]process_a2dp_event[e]-ret: %d\n", ret);
    return BT_SINK_STATUS_SUCCESS;
}


void bt_sink_a2dp_common_callback(bt_event_t event_id, const void *param)
{
    int32_t ret = 0;

    if ((bt_sink_a2dp_data_rece_cnt < BT_SINK_A2DP_DATA_RECE_CNT) || (event_id != BT_A2DP_STREAM_RECEIVED_IND)) {
        bt_sink_report("[sink][a2dp]common_hdr[s]-eid: 0x%x\n", event_id);
    }
    switch (event_id) {
        case BT_A2DP_CONNECT_CNF: {
            bt_a2dp_connect_cnf_t *conn_cnf = (bt_a2dp_connect_cnf_t *)param;
            if (conn_cnf->status == BT_STATUS_A2DP_OK) {
                bt_sink_report("[HQA][A2DP] CONNECTED, id: %d", conn_cnf->conn_id);
            }

            ret = bt_sink_a2dp_handle_connect_cnf(conn_cnf);
            break;
        }

        case BT_A2DP_CONNECT_IND: {
            bt_a2dp_connect_ind_t *conn_ind = (bt_a2dp_connect_ind_t *)param;

            ret = bt_sink_a2dp_handle_connect_ind(conn_ind);
            break;
        }

        case BT_A2DP_DISCONNECT_CNF: {
            bt_a2dp_disconnect_cnf_t *disconn_cnf = (bt_a2dp_disconnect_cnf_t *)param;
            bt_sink_a2dp_data_rece_cnt = 0;
            if (disconn_cnf->status == BT_STATUS_A2DP_OK) {
                bt_sink_report("[HQA][A2DP] DISCONNECTED, id: %d", disconn_cnf->conn_id);
            }
            g_a2dp_bit_pool = 0;
            ret = bt_sink_a2dp_handle_disconnect_cnf(disconn_cnf);
            break;
        }

        case BT_A2DP_DISCONNECT_IND: {
            bt_a2dp_disconnect_ind_t *disconn_ind = (bt_a2dp_disconnect_ind_t *)param;
            bt_sink_a2dp_data_rece_cnt = 0;
            g_a2dp_bit_pool = 0;

            ret = bt_sink_a2dp_handle_disconnect_ind(disconn_ind);
            bt_sink_report("[HQA][A2DP] DISCONNECTED, id: %d", disconn_ind->conn_id);

            break;
        }

        case BT_A2DP_START_STREAMING_CNF: {
            bt_a2dp_start_streaming_cnf_t *start_cnf = (bt_a2dp_start_streaming_cnf_t *)param;

            ret = bt_sink_a2dp_handle_strat_streaming_cnf(start_cnf);
            break;
        }

        case BT_A2DP_START_STREAMING_IND: {
            bt_a2dp_start_streaming_ind_t *start_ind = (bt_a2dp_start_streaming_ind_t *)param;
            bt_sink_a2dp_data_rece_cnt = 0;
            g_a2dp_bit_pool = 0;

            ret = bt_sink_a2dp_handle_strat_streaming_ind(start_ind);
            break;
        }

        case BT_A2DP_SUSPEND_STREAMING_CNF: {
            bt_a2dp_suspend_streaming_cnf_t *suspend_cnf = (bt_a2dp_suspend_streaming_cnf_t *)param;

            ret = bt_sink_a2dp_handle_suspend_streaming_cnf(suspend_cnf);
            break;
        }

        case BT_A2DP_SUSPEND_STREAMING_IND: {
            bt_a2dp_suspend_streaming_ind_t *suspend_ind = (bt_a2dp_suspend_streaming_ind_t *)param;

            bt_sink_a2dp_data_rece_cnt = 0;
            bt_sink_report("[HQA][A2DP] STOPPED");
            g_a2dp_bit_pool = 0;

            ret = bt_sink_a2dp_handle_suspend_streaming_ind(suspend_ind);
            break;
        }

        case BT_A2DP_RECONFIGURE_CNF: {
            bt_a2dp_reconfigure_cnf_t *reconfigure_cnf = (bt_a2dp_reconfigure_cnf_t *)param;

            ret = bt_sink_a2dp_handle_reconfigure_cnf(reconfigure_cnf);
            break;
        }

        case BT_A2DP_RECONFIGURE_IND: {
            bt_a2dp_reconfigure_ind_t *reconfigure_ind = (bt_a2dp_reconfigure_ind_t *)param;

            ret = bt_sink_a2dp_handle_reconfigure_ind(reconfigure_ind);
            break;
        }

        case BT_A2DP_STREAM_RECEIVED_IND: {
            bt_a2dp_stream_data_received_ind_t *data_ind = (bt_a2dp_stream_data_received_ind_t *)param;
            ++bt_sink_a2dp_data_rece_cnt;
#if 1
            ret = bt_sink_a2dp_handle_data_received_ind(data_ind);
#else
            if ((bt_sink_a2dp_data_rece_cnt < BT_SINK_A2DP_DATA_RECE_CNT)) {
                a2dp_streaming.conn_id = data_ind->conn_id;
                /* notify a2dp event */
                a2dp_event.event = BT_SINK_A2DP_EVENT_STREAMING;
                a2dp_event.param = &a2dp_streaming;
                bt_sink_event_send(BT_SINK_EVENT_A2DP_NOTIFY, &a2dp_event);
            }
#endif
            break;
        }

        default:
            break;
    }

    if ((bt_sink_a2dp_data_rece_cnt < BT_SINK_A2DP_DATA_RECE_CNT) || (event_id != BT_A2DP_STREAM_RECEIVED_IND)) {
        bt_sink_report("[sink][a2dp]common_hdr[e]-ret: %d\n", ret);
    }
}

