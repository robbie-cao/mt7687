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

#include "bt_a2dp_codec_internal.h"

const bt_a2dp_sbc_codec_t sink_capability_sbc[1] = {
    {
        18,  /* min_bit_pool       */
        75,  /* max_bit_pool       */
        0xf, /* block_len: all     */
        0xf, /* subband_num: all   */
        0x3, /* both snr/loudness  */
        0xf, /* sample_rate: all   */
        0xf  /* channel_mode: all  */
    }
};

const bt_a2dp_aac_codec_t sink_capability_aac[1] = {
    {
        true,    /*VBR         */
        0xc0,    /*Object type */
        0x03,    /*Channels    */
        0x0ff8,  /*Sample_rate */
        0x60000  /*bit_rate, 384 Kbps */
    }
};

static void bt_set_buffer(bt_media_handle_t *handle, uint8_t *buffer, uint32_t length)
{
    handle->buffer_info.buffer_base = buffer;
    length &= ~0x1; // make buffer size even
    handle->buffer_info.buffer_size = length;
    handle->buffer_info.write = 0;
    handle->buffer_info.read = 0;
    handle->waiting = false;
    handle->underflow = false;
}

static void bt_get_write_buffer(bt_media_handle_t *handle, uint8_t **buffer, uint32_t *length)
{
    int32_t count = 0;

    if (handle->buffer_info.read > handle->buffer_info.write) {
        count = handle->buffer_info.read - handle->buffer_info.write - 1;
    } else if (handle->buffer_info.read == 0) {
        count = handle->buffer_info.buffer_size - handle->buffer_info.write - 1;
    } else {
        count = handle->buffer_info.buffer_size - handle->buffer_info.write;
    }
    *buffer = handle->buffer_info.buffer_base + handle->buffer_info.write;
    *length = count;
}

static void bt_get_read_buffer(bt_media_handle_t *handle, uint8_t **buffer, uint32_t *length)
{
    int32_t count = 0;

    if (handle->buffer_info.write >= handle->buffer_info.read) {
        count = handle->buffer_info.write - handle->buffer_info.read;
    } else {
        count = handle->buffer_info.buffer_size - handle->buffer_info.read;
    }
    *buffer = handle->buffer_info.buffer_base + handle->buffer_info.read;
    *length = count;
}

static void bt_write_data_done(bt_media_handle_t *handle, uint32_t length)
{
    handle->buffer_info.write += length;
    if (handle->buffer_info.write == handle->buffer_info.buffer_size) {
        handle->buffer_info.write = 0;
    }
}

static void bt_finish_write_data(bt_media_handle_t *handle)
{
    handle->waiting = false;
    handle->underflow = false;
}

static void bt_reset_share_buffer(bt_media_handle_t *handle)
{
    handle->buffer_info.write = 0;
    handle->buffer_info.read = 0;
    handle->waiting = false;
    handle->underflow = false;
}

static void bt_read_data_done(bt_media_handle_t *handle, uint32_t length)
{
    handle->buffer_info.read += length;
    if (handle->buffer_info.read == handle->buffer_info.buffer_size) {
        handle->buffer_info.read = 0;
    }
}

static int32_t bt_get_free_space(bt_media_handle_t *handle)
{
    int32_t count = 0;

    count = handle->buffer_info.read - handle->buffer_info.write - 1;
    if (count < 0) {
        count += handle->buffer_info.buffer_size;
    }
    return count;
}

static int32_t bt_get_data_count(bt_media_handle_t *handle)
{
    int32_t count = 0;

    count = handle->buffer_info.write - handle->buffer_info.read;
    if (count < 0) {
        count += handle->buffer_info.buffer_size;
    }
    return count;
}

static void bt_codec_buffer_function_init(bt_media_handle_t *handle)
{
    handle->set_buffer         = bt_set_buffer;
    handle->get_write_buffer   = bt_get_write_buffer;
    handle->get_read_buffer    = bt_get_read_buffer;
    handle->write_data_done    = bt_write_data_done;
    handle->finish_write_data  = bt_finish_write_data;
    handle->reset_share_buffer = bt_reset_share_buffer;
    handle->read_data_done     = bt_read_data_done;
    handle->get_free_space     = bt_get_free_space;
    handle->get_data_count     = bt_get_data_count;
}

#if defined(BT_A2DP_BITSTREAM_DUMP_DEBUG)
#define BT_A2DP_BS_LEN 160000
uint32_t bt_a2dp_ptr = 0;
uint8_t bt_a2dp_bitstream[BT_A2DP_BS_LEN];
#endif /* BT_A2DP_BITSTREAM_DUMP_DEBUG */

static void bt_write_bs_to_dsp(bt_a2dp_audio_internal_handle_t *internal_handle)
{
    bt_media_handle_t *handle = (bt_media_handle_t *)internal_handle;
    bt_a2dp_codec_type_t codec_type = internal_handle->codec_info.codec_cap.type;
    ring_buffer_information_t *p_ring = &internal_handle->ring_info;
    uint16_t bs_page;
    uint16_t bs_addr;
    uint16_t bs_size;
    uint16_t bs_wptr;
    uint16_t bs_rptr;
    if (codec_type == BT_A2DP_CODEC_SBC) {
        bs_page = *DSP_SBC_DEC_DM_BS_PAGE;
        bs_addr = *DSP_SBC_DEC_DM_BS_ADDR;
        bs_size = *DSP_SBC_DEC_DM_BS_LEN;
        bs_wptr = *DSP_SBC_DEC_DM_BS_MCU_W_PTR;
        bs_rptr = *DSP_SBC_DEC_DM_BS_DSP_R_PTR;
        p_ring->write_pointer       = (uint32_t)bs_wptr;
        p_ring->read_pointer        = (uint32_t)bs_rptr;
        p_ring->buffer_byte_count   = (uint32_t)bs_size;
        p_ring->buffer_base_pointer = (uint8_t *)DSP_DM_ADDR(bs_page, bs_addr);
    }
#ifdef MTK_BT_A2DP_AAC_ENABLE
    else if (codec_type == BT_A2DP_CODEC_AAC) {
        bs_addr = *DSP_AAC_DEC_DM_BS_ADDR;
        bs_wptr = *DSP_AAC_DEC_DM_BS_MCU_W_PTR;    // in word
        bs_rptr = *DSP_AAC_DEC_DM_BS_DSP_R_PTR;    // in word
        p_ring->write_pointer       = (uint32_t)((bs_wptr - bs_addr) << 1); // in byte
        p_ring->read_pointer        = (uint32_t)((bs_rptr - bs_addr) << 1);
    }
#endif /*MTK_BT_A2DP_AAC_ENABLE*/
    {
        uint32_t loop_idx;
        uint32_t loop_cnt = 4;
        for (loop_idx = 0; loop_idx < loop_cnt; loop_idx++) {
            uint32_t           read_byte_cnt  = 0;
            uint32_t           write_byte_cnt = 0;
            uint32_t           move_byte_cnt;
            uint8_t           *p_mcu_buf      = NULL;
            volatile uint16_t *p_dsp_buf      = NULL;
            handle->get_read_buffer(handle, &p_mcu_buf, &read_byte_cnt);
            read_byte_cnt  &= ~0x1;     // Make it even
            if (codec_type == BT_A2DP_CODEC_SBC) {
                ring_buffer_get_write_information(p_ring, (uint8_t **)&p_dsp_buf, &write_byte_cnt);
            }
#ifdef MTK_BT_A2DP_AAC_ENABLE
            else if (codec_type == BT_A2DP_CODEC_AAC) {
                ring_buffer_get_write_information_non_mirroring(p_ring, (uint8_t **)&p_dsp_buf, &write_byte_cnt);
            }
#endif /*MTK_BT_A2DP_AAC_ENABLE*/
            write_byte_cnt &= ~0x1;     // Make it even
            move_byte_cnt = MINIMUM(write_byte_cnt, read_byte_cnt);
            {
                // Move data
                uint32_t move_word_cnt = move_byte_cnt >> 1;
                if (move_word_cnt > 0) {
#if defined(BT_A2DP_BITSTREAM_DUMP_DEBUG)
                    if (bt_a2dp_ptr + move_byte_cnt <= BT_A2DP_BS_LEN) {
                        memcpy(bt_a2dp_bitstream + bt_a2dp_ptr, p_mcu_buf, move_byte_cnt);
                        bt_a2dp_ptr += move_byte_cnt;
                    }
#endif /* BT_A2DP_BITSTREAM_DUMP_DEBUG */
                    audio_idma_write_to_dsp(p_dsp_buf, (uint16_t *)p_mcu_buf, move_word_cnt);
                } else {    // Read buffer empty or write buffer full
                    break;
                }
            }
            handle->read_data_done (handle, move_byte_cnt);
            if (codec_type == BT_A2DP_CODEC_SBC) {
                ring_buffer_write_done (p_ring, move_byte_cnt);
                *DSP_SBC_DEC_DM_BS_MCU_W_PTR = (uint16_t)p_ring->write_pointer;
            }
#ifdef MTK_BT_A2DP_AAC_ENABLE
            else if (codec_type == BT_A2DP_CODEC_AAC) {
                ring_buffer_write_done_non_mirroring(p_ring, move_byte_cnt);
                *DSP_AAC_DEC_DM_BS_MCU_W_PTR = (uint16_t)(p_ring->write_pointer >> 1) + bs_addr;
            }
#endif /*MTK_BT_A2DP_AAC_ENABLE*/
        }
    }
    return;
}

static void sbc_decoder_isr_handler(void *data)
{
    bt_a2dp_audio_internal_handle_t *internal_handle = (bt_a2dp_audio_internal_handle_t *)data;
    bt_media_handle_t *handle = (bt_media_handle_t *)internal_handle;

    if ((GET_DSP_VALUE(RG_SBC_DEC_FSM) == DSP_SBC_STATE_IDLE) || (handle->state == BT_CODEC_STATE_ERROR)) {
        return;
    }

    /* error handling */
    if (GET_DSP_VALUE(RG_SBC_DEC_FSM) == DSP_SBC_STATE_RUNNING) {
        if (   (GET_DSP_VALUE(RG_SBC_DEC_STATUS) == DSP_SBC_DEC_SYNC_ERR)
                || (GET_DSP_VALUE(RG_SBC_DEC_STATUS) == DSP_SBC_DEC_CRC_ERR)
                || (GET_DSP_VALUE(RG_SBC_DEC_STATUS) == DSP_SBC_DEC_BITPOOL_ERR)
                || (GET_DSP_VALUE(RG_SBC_PAR_STATUS) == DSP_SBC_PAR_MAGIC_WORD_ERR)
                || (GET_DSP_VALUE(RG_SBC_PAR_STATUS) == DSP_SBC_PAR_INVALID_NON_FRAGMENTED_PAYLOAD)
                || (GET_DSP_VALUE(RG_SBC_PAR_STATUS) == DSP_SBC_PAR_START_PAYLOAD_ERR)
                || (GET_DSP_VALUE(RG_SBC_PAR_STATUS) == DSP_SBC_PAR_INVALID_FRAGMENT_INDEX_1)
                || (GET_DSP_VALUE(RG_SBC_PAR_STATUS) == DSP_SBC_PAR_INVALID_FRAGMENT_INDEX_2)
                || (GET_DSP_VALUE(RG_SBC_PAR_STATUS) == DSP_SBC_PAR_INVALID_FRAME_NUMBER)
           ) {
            LISR_LOG_E("[A2DP]DECODER ERR, PAR:%d  DEC=%d\n", GET_DSP_VALUE(RG_SBC_PAR_STATUS), GET_DSP_VALUE(RG_SBC_DEC_STATUS));
            handle->handler(handle, BT_MEDIA_ERROR);
            handle->state = BT_CODEC_STATE_ERROR;
            return;
        }
    }

    /* fill bitstream */
    bt_write_bs_to_dsp(internal_handle);
    if (!handle->waiting) {
        handle->waiting = true;
        if ((!handle->underflow) && (GET_DSP_VALUE(RG_SBC_DEC_STATUS) == DSP_SBC_DEC_BS_UNDERFLOW)) {
            handle->underflow = true;
            LISR_LOG_I("[A2DP] DSP underflow \n");
            handle->handler(handle, BT_MEDIA_UNDERFLOW);
        } else {
            handle->handler(handle, BT_MEDIA_REQUEST);
        }
    }
}

static bt_status_t bt_open_sink_sbc_codec(bt_media_handle_t *handle)
{
    uint16_t I = 0;
    bt_a2dp_audio_internal_handle_t *internal_handle = (bt_a2dp_audio_internal_handle_t *) handle;

    TASK_LOG_CTRL("[SBC]open_codec\r\n");
    audio_service_hook_isr(DSP_D2C_SBC_DEC_INT, sbc_decoder_isr_handler, internal_handle);
    audio_service_setflag(handle->audio_id);
    if (GET_DSP_VALUE(RG_SBC_DEC_FSM) != DSP_SBC_STATE_IDLE) {
        return BT_STATUS_MEDIA_ERROR;
    }

    *DSP_AUDIO_CTRL2 |= DSP_PCM_R_DIS;
    *DSP_AUDIO_FLEXI_CTRL |= (FLEXI_VBI_ENABLE | FLEXI_SD_ENABLE);

    *DSP_SBC_PAR_MAGIC_WORD = 0x3453;
    SET_DSP_VALUE(RG_SBC_PARSER_EN, 1);
    SET_DSP_VALUE(RG_SBC_DEC_FSM, DSP_SBC_STATE_START);
    *DSP_SBC_DEC_DM_BS_MCU_W_PTR = 0;
    dsp_audio_fw_dynamic_download(DDID_SBC_DECODE);
    afe_set_path_type(HAL_AUDIO_PLAYBACK_MUSIC);
    audio_playback_on(ASP_TYPE_SBC_DEC, internal_handle->sample_rate);

    for (I = 0; ; I++) {
        if (GET_DSP_VALUE(RG_SBC_DEC_FSM) == DSP_SBC_STATE_RUNNING) {
            break;
        }
        if (I > 80) {
            return BT_STATUS_MEDIA_ERROR;
        }
        hal_gpt_delay_ms(9);
    }
    handle->state = BT_CODEC_STATE_PLAY;
    return BT_STATUS_MEDIA_OK;
}

static bt_status_t bt_close_sink_sbc_codec(bt_media_handle_t *handle)
{
    uint16_t I = 0;
    TASK_LOG_CTRL("[SBC]close_codec\r\n");
    for (I = 0; ; I++) {
        if (GET_DSP_VALUE(RG_SBC_DEC_FSM) == DSP_SBC_STATE_IDLE) {
            break;
        }
        if (GET_DSP_VALUE(RG_SBC_DEC_FSM) == DSP_SBC_STATE_RUNNING) {
            SET_DSP_VALUE(RG_SBC_DEC_FSM, DSP_SBC_STATE_FLUSH);
        }
        if (I > 80) {
            return BT_STATUS_MEDIA_ERROR;
        }
        hal_gpt_delay_ms(9);
    }
    SET_DSP_VALUE(RG_SBC_PARSER_EN, 0);

    *DSP_AUDIO_CTRL2 &= ~DSP_PCM_R_DIS;
    *DSP_AUDIO_FLEXI_CTRL &= ~(FLEXI_VBI_ENABLE | FLEXI_SD_ENABLE);

    audio_playback_off();
    audio_service_unhook_isr(DSP_D2C_SBC_DEC_INT);
    audio_service_clearflag(handle->audio_id);
    handle->state = BT_CODEC_STATE_STOP;
    return BT_STATUS_MEDIA_OK;
}

static bt_status_t bt_a2dp_sink_sbc_play(bt_media_handle_t *handle)
{
    TASK_LOG_CTRL("[SBC]play\r\n");
    if (handle->state != BT_CODEC_STATE_READY) {
        return BT_STATUS_MEDIA_ERROR;
    }
#if defined(BT_A2DP_BITSTREAM_DUMP_DEBUG)
    bt_a2dp_ptr = 0;
    memset(bt_a2dp_bitstream, 0, BT_A2DP_BS_LEN * sizeof(uint8_t));
#endif /* BT_A2DP_BITSTREAM_DUMP_DEBUG */
    return bt_open_sink_sbc_codec(handle);
}

static bt_status_t bt_a2dp_sink_sbc_stop(bt_media_handle_t *handle)
{
    TASK_LOG_CTRL("[SBC]stop\r\n");
    if (handle->state != BT_CODEC_STATE_PLAY) {
        return BT_STATUS_MEDIA_ERROR;
    }
    return bt_close_sink_sbc_codec(handle);
}

static bt_status_t bt_a2dp_sink_sbc_process(bt_media_handle_t *handle, bt_event_t event)
{
    bt_a2dp_audio_internal_handle_t *internal_handle = (bt_a2dp_audio_internal_handle_t *) handle;
    if (internal_handle == NULL) {
        return BT_STATUS_MEDIA_ERROR;
    }
    return BT_STATUS_MEDIA_OK;
}

static bt_status_t bt_a2dp_sink_parse_sbc_info(bt_a2dp_audio_internal_handle_t *internal_handle)
{
    uint8_t channel_mode, sample_rate;
    bt_codec_a2dp_audio_t *pParam = (bt_codec_a2dp_audio_t *)&internal_handle->codec_info;

    channel_mode = pParam->codec_cap.codec.sbc.channel_mode;
    sample_rate  = pParam->codec_cap.codec.sbc.sample_rate;
    TASK_LOG_I("[A2DP]sample rate=%d, channel=%d \n", sample_rate, channel_mode);
    switch (channel_mode) {
        case 8:
            internal_handle->channel_number = 1;
            break;
        case 4:
        case 2:
        case 1:
            internal_handle->channel_number = 2;
            break;
        default:
            return BT_STATUS_MEDIA_INVALID_PARAM;
    }

    switch (sample_rate) {
        case 8:
            internal_handle->sample_rate = HAL_AUDIO_SAMPLING_RATE_16KHZ;
            break;
        case 4:
            internal_handle->sample_rate = HAL_AUDIO_SAMPLING_RATE_32KHZ;
            break;
        case 2:
            internal_handle->sample_rate = HAL_AUDIO_SAMPLING_RATE_44_1KHZ;
            break;
        case 1:
            internal_handle->sample_rate = HAL_AUDIO_SAMPLING_RATE_48KHZ;
            break;
        default:
            return BT_STATUS_MEDIA_INVALID_PARAM;
    }
    return BT_STATUS_MEDIA_OK;
}

#ifdef MTK_BT_A2DP_AAC_ENABLE
/* aac */
static void aac_get_silence_pattern(bt_a2dp_audio_internal_handle_t *internal_handle)
{
    uint8_t channel_number, frequence_index;
    uint16_t sample_rate;
    uint8_t *silence_frame = internal_handle->aac_silence_pattern;

    channel_number = internal_handle->channel_number;
    sample_rate  = internal_handle->sample_rate;

    memcpy(silence_frame, AAC_ADTS_HEADER, ADTS_HEADER_LENGTH);

    switch (sample_rate) {
        case HAL_AUDIO_SAMPLING_RATE_8KHZ:
            frequence_index = 11;
            break;
        case HAL_AUDIO_SAMPLING_RATE_11_025KHZ:
            frequence_index = 10;
            break;
        case HAL_AUDIO_SAMPLING_RATE_12KHZ:
            frequence_index = 9;
            break;
        case HAL_AUDIO_SAMPLING_RATE_16KHZ:
            frequence_index = 8;
            break;
        case HAL_AUDIO_SAMPLING_RATE_22_05KHZ:
            frequence_index = 7;
            break;
        case HAL_AUDIO_SAMPLING_RATE_24KHZ:
            frequence_index = 6;
            break;
        case HAL_AUDIO_SAMPLING_RATE_32KHZ:
            frequence_index = 5;
            break;
        case HAL_AUDIO_SAMPLING_RATE_44_1KHZ:
            frequence_index = 4;
            break;
        case HAL_AUDIO_SAMPLING_RATE_48KHZ:
            frequence_index = 3;
            break;
        default:
            frequence_index = 0;
            break;
    }
    /* fill AAC silence data */
    if(channel_number == 1) {
        memcpy(silence_frame+ADTS_HEADER_LENGTH, aac_silence_data_mono, SILENCE_DATA_LENGTH);
    } else {
        memcpy(silence_frame+ADTS_HEADER_LENGTH, aac_silence_data_stereo, SILENCE_DATA_LENGTH);
    }
    /* update AAC ADTS header */
    silence_frame[2] |= (frequence_index << 2);
    silence_frame[3] |= ((channel_number) << 6) | ((SILENCE_TOTAL_LENGTH) >> 11);
    silence_frame[4] = ((SILENCE_TOTAL_LENGTH) >> 3) & 0xFF;
    silence_frame[5] |= ((SILENCE_TOTAL_LENGTH) & 0x07) << 5;

}

static void aac_write_silence(bt_a2dp_audio_internal_handle_t *internal_handle)
{
    ring_buffer_information_t *p_ring = &internal_handle->ring_info;
    uint16_t bs_addr;
    uint16_t bs_wptr;
    uint16_t bs_rptr;

    bs_addr = *DSP_AAC_DEC_DM_BS_ADDR;
    bs_wptr = *DSP_AAC_DEC_DM_BS_MCU_W_PTR;    // in word
    bs_rptr = *DSP_AAC_DEC_DM_BS_DSP_R_PTR;    // in word
    p_ring->write_pointer       = (uint32_t)((bs_wptr - bs_addr) << 1); // in byte
    p_ring->read_pointer        = (uint32_t)((bs_rptr - bs_addr) << 1);

    {
        uint32_t loop_idx;
        uint32_t loop_cnt = 2;
        int32_t read_byte_cnt = SILENCE_TOTAL_LENGTH;
        for (loop_idx = 0; loop_idx < loop_cnt; loop_idx++) {
            uint32_t           write_byte_cnt = 0;
            uint32_t           move_byte_cnt;
            uint8_t            *p_mcu_buf      = internal_handle->aac_silence_pattern;
            volatile uint16_t  *p_dsp_buf      = NULL;

            p_mcu_buf += (SILENCE_TOTAL_LENGTH - read_byte_cnt);
            ring_buffer_get_write_information_non_mirroring(p_ring, (uint8_t **)&p_dsp_buf, &write_byte_cnt);

            write_byte_cnt &= ~0x1;     // Make it even
            move_byte_cnt = MINIMUM(write_byte_cnt, read_byte_cnt);
            {
                // Move data
                uint32_t move_word_cnt = move_byte_cnt >> 1;
                if (move_word_cnt > 0) {
                    audio_idma_write_to_dsp(p_dsp_buf, (uint16_t *)p_mcu_buf, move_word_cnt);
                    read_byte_cnt -= (move_word_cnt << 1);
                } else {    // Read buffer empty or write buffer full
                    break;
                }
            }
            ring_buffer_write_done_non_mirroring(p_ring, move_byte_cnt);
            *DSP_AAC_DEC_DM_BS_MCU_W_PTR = (uint16_t)(p_ring->write_pointer >> 1) + bs_addr;
        }
    }
    return;
}

static void aac_decoder_isr_handler(void *data)
{
    bt_a2dp_audio_internal_handle_t *internal_handle = (bt_a2dp_audio_internal_handle_t *)data;
    bt_media_handle_t *handle = (bt_media_handle_t *)internal_handle;

    *DSP_TASK4_COSIM_HANDSHAKE = 0;
    if ((*DSP_AAC_DEC_FSM == DSP_AAC_STATE_IDLE) || (handle->state == BT_CODEC_STATE_ERROR)) {
        return;
    }

    /* error handling, but bypass the buffer underflow warning from DSP */
    if ((*DSP_AAC_DEC_ERROR_REPORT != DSP_AAC_REPORT_NONE)
            && (*DSP_AAC_DEC_ERROR_REPORT != DSP_AAC_REPORT_UNDERFLOW)) {
        internal_handle->error_count ++;

        /* fill silence when underflow continuously */
        if (*DSP_AAC_DEC_ERROR_REPORT == DSP_AAC_REPORT_BUFFER_NOT_ENOUGH) {
            int32_t mcu_data_count = handle->get_data_count(handle);
            if(mcu_data_count < AAC_FILL_SILENCE_TRHESHOLD) {
                aac_write_silence(internal_handle);
            }
        } else {
            LISR_LOG_E("[A2DP]DECODER ERR(%x), FSM:%x REPORT=%x\n", (unsigned int)internal_handle->error_count, *DSP_AAC_DEC_FSM, *DSP_AAC_DEC_ERROR_REPORT);
        }
        if (internal_handle->error_count >= AAC_ERROR_FRAME_THRESHOLD) {
            LISR_LOG_E("[A2DP]DECODER ERR OVER THRESHOLD\n");
            *DSP_AAC_DEC_FSM = DSP_AAC_STATE_STOP;
            handle->state = BT_CODEC_STATE_ERROR;
            handle->handler(handle, BT_MEDIA_ERROR);
            return;
        }
    } else { //if error is not consecutive, reset to 0
        internal_handle->error_count = 0;
    }

    /* bitstream buffer initialization */
    if (!internal_handle->ring_info.buffer_base_pointer) {
        uint16_t bs_page = DSP_AAC_PAGE_NUM;
        uint16_t bs_addr = *DSP_AAC_DEC_DM_BS_ADDR;
        uint16_t bs_size = *DSP_AAC_DEC_DM_BS_LEN << 1;
        internal_handle->ring_info.buffer_byte_count = (uint32_t)bs_size;
        internal_handle->ring_info.buffer_base_pointer = (uint8_t *)DSP_DM_ADDR(bs_page, bs_addr);
    }

    /* fill bitstream */
    bt_write_bs_to_dsp(internal_handle);

    if (!handle->waiting) {
        handle->waiting = true;
        if ((!handle->underflow) && (*DSP_AAC_DEC_ERROR_REPORT == DSP_AAC_REPORT_UNDERFLOW)) {
            handle->underflow = true;
            LISR_LOG_I("[A2DP]DSP underflow \n");
            handle->handler(handle, BT_MEDIA_UNDERFLOW);
        } else {
            handle->handler(handle, BT_MEDIA_REQUEST);
        }
    }
}

static bt_status_t bt_open_sink_aac_codec(bt_media_handle_t *handle)
{
    uint16_t I = 0;
    bt_a2dp_audio_internal_handle_t *internal_handle = (bt_a2dp_audio_internal_handle_t *) handle;

    TASK_LOG_CTRL("[AAC]open_codec\r\n");
    internal_handle->error_count = 0;

    audio_service_hook_isr(DSP_D2C_AAC_DEC_INT, aac_decoder_isr_handler, internal_handle);
    audio_service_setflag(handle->audio_id);
    *DSP_AUDIO_ASP_COMMON_FLAG_1 = 0;    //clear dsp audio common flag

    *DSP_AAC_DEC_ALLERROR_REPORT = 0;
    *DSP_AAC_DEC_DUAL_SCE = 0;

    if (*DSP_AAC_DEC_FSM != DSP_AAC_STATE_IDLE) {
        TASK_LOG_E("[A2DP]AAC OPEN STATE ERROR(%x)\n", *DSP_AAC_DEC_FSM);
        return BT_STATUS_MEDIA_ERROR;
    }
    *DSP_AAC_DEC_FSM = DSP_AAC_STATE_START;

    *DSP_AUDIO_CTRL2 |= (DSP_AAC_CTRL_ASP | DSP_PCM_R_DIS);
    *DSP_AUDIO_FLEXI_CTRL |= (FLEXI_VBI_ENABLE | FLEXI_SD_ENABLE);
    afe_set_path_type(HAL_AUDIO_PLAYBACK_MUSIC);
    dsp_audio_fw_dynamic_download(DDID_AAC);

    aac_get_silence_pattern(internal_handle);
    audio_playback_on(ASP_TYPE_AAC_DEC, internal_handle->sample_rate);

    for (I = 0; ; I++) {
        if (*DSP_AAC_DEC_FSM == DSP_AAC_STATE_PLAYING) {
            break;
        }
        /* This is the case when AAC codec has started and encoutered an error,
           aacPlaybackHisr found this and set the state to STOP, then AAC codec
           set the state to IDLE. */
        if (*DSP_AAC_DEC_FSM == DSP_AAC_STATE_IDLE) {
            TASK_LOG_E("[A2DP]ERROR when AAC CODEC STARTS \n");
            break;
        }
        if (I > 80) {
            TASK_LOG_E("[A2DP]AAC CODEC OPEN ERROR\n");
            return BT_STATUS_MEDIA_ERROR;
        }

        hal_gpt_delay_ms(9);
    }

    handle->state = BT_CODEC_STATE_PLAY;
    return BT_STATUS_MEDIA_OK;
}

static bt_status_t bt_close_sink_aac_codec(bt_media_handle_t *handle)
{
    uint16_t I = 0;
    TASK_LOG_CTRL("[AAC]close_codec\r\n");
    for (I = 0; ; I++) {
        if (*DSP_AAC_DEC_FSM == DSP_AAC_STATE_IDLE) {
            break;
        }
        if (*DSP_AAC_DEC_FSM == DSP_AAC_STATE_PLAYING) {
            *DSP_AAC_DEC_FSM = DSP_AAC_STATE_STOP;
        }
        if (I > 80) {
            return BT_STATUS_MEDIA_ERROR;
        }
        hal_gpt_delay_ms(9);
    }

    *DSP_AUDIO_CTRL2 &= ~(DSP_AAC_CTRL_ASP | DSP_PCM_R_DIS);
    *DSP_AUDIO_FLEXI_CTRL &= ~(FLEXI_VBI_ENABLE | FLEXI_SD_ENABLE);

    audio_playback_off();

    audio_service_unhook_isr(DSP_D2C_AAC_DEC_INT);
    audio_service_clearflag(handle->audio_id);
    handle->state = BT_CODEC_STATE_STOP;
    return BT_STATUS_MEDIA_OK;
}

static bt_status_t bt_a2dp_sink_aac_play(bt_media_handle_t *handle)
{
    TASK_LOG_CTRL("[AAC]play\r\n");
    if (handle->state != BT_CODEC_STATE_READY) {
        return BT_STATUS_MEDIA_ERROR;
    }
    return bt_open_sink_aac_codec(handle);
}

static bt_status_t bt_a2dp_sink_aac_stop(bt_media_handle_t *handle)
{
    TASK_LOG_CTRL("[AAC]stop\r\n");
    if (handle->state != BT_CODEC_STATE_PLAY) {
        return BT_STATUS_MEDIA_ERROR;
    }
    return bt_close_sink_aac_codec(handle);
}

static bt_status_t bt_a2dp_sink_aac_process(bt_media_handle_t *handle, bt_event_t event)
{
    bt_a2dp_audio_internal_handle_t *internal_handle = (bt_a2dp_audio_internal_handle_t *) handle;
    if (internal_handle == NULL) {
        return BT_STATUS_MEDIA_ERROR;
    }
    return BT_STATUS_MEDIA_OK;
}

static bt_status_t bt_a2dp_sink_parse_aac_info(bt_a2dp_audio_internal_handle_t *internal_handle)
{
    uint8_t channel_mode;
    uint16_t sample_rate;
    bt_codec_a2dp_audio_t *pParam = (bt_codec_a2dp_audio_t *)&internal_handle->codec_info;

    channel_mode = pParam->codec_cap.codec.aac.channels;
    sample_rate  = pParam->codec_cap.codec.aac.sample_rate;
    TASK_LOG_I("[A2DP]AAC sample rate=%x, channel=%x \n", sample_rate, channel_mode);

    switch (channel_mode) {
        case 0x2:
            internal_handle->channel_number = 1;
            break;
        case 0x1:
            internal_handle->channel_number = 2;
            break;
        default:
            return BT_STATUS_MEDIA_INVALID_PARAM;
            break;
    }
    switch (sample_rate) {
        case 0x800:
            internal_handle->sample_rate = HAL_AUDIO_SAMPLING_RATE_8KHZ;
            break;
        case 0x400:
            internal_handle->sample_rate = HAL_AUDIO_SAMPLING_RATE_11_025KHZ;
            break;
        case 0x200:
            internal_handle->sample_rate = HAL_AUDIO_SAMPLING_RATE_12KHZ;
            break;
        case 0x100:
            internal_handle->sample_rate = HAL_AUDIO_SAMPLING_RATE_16KHZ;
            break;
        case 0x80:
            internal_handle->sample_rate = HAL_AUDIO_SAMPLING_RATE_22_05KHZ;
            break;
        case 0x40:
            internal_handle->sample_rate = HAL_AUDIO_SAMPLING_RATE_24KHZ;
            break;
        case 0x20:
            internal_handle->sample_rate = HAL_AUDIO_SAMPLING_RATE_32KHZ;
            break;
        case 0x10:
            internal_handle->sample_rate = HAL_AUDIO_SAMPLING_RATE_44_1KHZ;
            break;
        case 0x8:
            internal_handle->sample_rate = HAL_AUDIO_SAMPLING_RATE_48KHZ;
            break;
        default:
            return BT_STATUS_MEDIA_INVALID_PARAM;
            break;
    }
    return BT_STATUS_MEDIA_OK;
}
#endif /*MTK_BT_A2DP_AAC_ENABLE*/
bt_media_handle_t *bt_codec_a2dp_open(bt_codec_a2dp_callback_t bt_a2dp_callback , const bt_codec_a2dp_audio_t *param)
{
    bt_media_handle_t *handle;
    bt_a2dp_audio_internal_handle_t *internal_handle; /*internal handler*/
    uint16_t audio_id = audio_get_id();
    TASK_LOG_I("[A2DP]Open codec\n");
    if (audio_id > MAX_AUDIO_FUNCTIONS) {
        return 0;
    }

    /* alloc internal handle space */
    internal_handle = (bt_a2dp_audio_internal_handle_t *)pvPortMalloc(sizeof(bt_a2dp_audio_internal_handle_t));
    memset(internal_handle, 0, sizeof(bt_a2dp_audio_internal_handle_t));
    handle = &internal_handle->handle;

    internal_handle->codec_info = *(bt_codec_a2dp_audio_t *)param;
    handle->audio_id = audio_id;
    handle->handler = bt_a2dp_callback;
    bt_codec_buffer_function_init(handle);
    if (internal_handle->codec_info.role == BT_A2DP_SINK) {
        if (internal_handle->codec_info.codec_cap.type == BT_A2DP_CODEC_SBC) {
            bt_status_t result = BT_STATUS_MEDIA_OK;
            handle->play    = bt_a2dp_sink_sbc_play;
            handle->stop    = bt_a2dp_sink_sbc_stop;
            handle->process = bt_a2dp_sink_sbc_process;
            result = bt_a2dp_sink_parse_sbc_info(internal_handle);
            if (BT_STATUS_MEDIA_OK != result) {
                return 0;
            }
        }
#ifdef MTK_BT_A2DP_AAC_ENABLE
        else if (internal_handle->codec_info.codec_cap.type == BT_A2DP_CODEC_AAC) {
            bt_status_t result = BT_STATUS_MEDIA_OK;
            handle->play    = bt_a2dp_sink_aac_play;
            handle->stop    = bt_a2dp_sink_aac_stop;
            handle->process = bt_a2dp_sink_aac_process;
            result = bt_a2dp_sink_parse_aac_info(internal_handle);
            if (BT_STATUS_MEDIA_OK != result) {
                return 0;
            }
        }
#endif /*MTK_BT_A2DP_AAC_ENABLE*/
        else {
            return 0;
        }
    } else {
        /* A2DP source role */
    }
    handle->state = BT_CODEC_STATE_READY;
    return handle;
}

bt_status_t bt_codec_a2dp_close(bt_media_handle_t *handle)
{
    bt_a2dp_audio_internal_handle_t *internal_handle = (bt_a2dp_audio_internal_handle_t *) handle;
    TASK_LOG_I("[A2DP]Close codec\n");
    if (handle->state != BT_CODEC_STATE_STOP) {
        return BT_STATUS_MEDIA_ERROR;
    }
    handle->state = BT_CODEC_STATE_IDLE;
    audio_free_id(handle->audio_id);
    vPortFree(internal_handle);
    return BT_STATUS_MEDIA_OK;
}
