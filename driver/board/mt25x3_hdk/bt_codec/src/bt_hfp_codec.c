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

#include "bt_hfp_codec_internal.h"
#include "cvsd_codec.h"
#include "msbc_codec.h"
#include "plc.h"
#include "hal_gpt.h"
#include "hal_audio_enhancement.h"
#include "hal_audio_internal_afe.h"
#include "hal_audio.h"

/*******************************************************************************\
| ROM Tables                                                                    |
\*******************************************************************************/

static const uint32_t bt_hfp_rx_packet_valid_mask_table[BT_HFP_PKT_TYPE_IDX_MAX][BT_HFP_HW_SRAM_RX_PKT_CNT] = {
    {0x1 << 0, 0x1 << 0, 0x1 << 1, 0x1 << 1, 0x1 <<  2, 0x1 <<  2}, /* 60 Bytes */
    {0x1 << 0, 0x1 << 1, 0x1 << 2, 0x1 << 3, 0x1 <<  4, 0x1 <<  5}, /* 30 Bytes */
    {0x3 << 0, 0x3 << 1, 0x3 << 3, 0x3 << 4, 0x3 <<  6, 0x3 <<  7}, /* 20 Bytes */
    {0x7 << 0, 0x7 << 3, 0x7 << 6, 0x7 << 9, 0x7 << 12, 0x7 << 15}  /* 10 Bytes */
};

static const uint8_t bt_hfp_msbc_header[4] = {0x08, 0x38, 0xc8, 0xf8};

/*******************************************************************************\
| Global Variables                                                              |
\*******************************************************************************/

static bt_hfp_codec_internal_handle_t *bt_hfp_codec_internal_handle = NULL;
PRIVILEGED_DATA static QueueHandle_t bt_codec_queue_handle = NULL;
static uint32_t bt_codec_queue_reg_num = 0;
static bt_codec_queue_event_id_t bt_codec_queue_id_array[MAX_BT_CODEC_FUNCTIONS];
static bt_codec_callback_t bt_codec_queue_handler[MAX_BT_CODEC_FUNCTIONS];

/* #define BT_HFP_CODEC_CHECK_INTERRUPT_INTERVAL */

#if defined(BT_HFP_CODEC_CHECK_INTERRUPT_INTERVAL)
uint32_t bt_hfp_rx_cnt = 0;
uint32_t p2w_tx_cnt = 0;
uint32_t p2w_rx_cnt = 0;
#endif  /* BT_HFP_CODEC_CHECK_INTERRUPT_INTERVAL */

static uint8_t g_codec_tx_buffer[7168];
static uint8_t g_codec_rx_buffer[14336];

/*******************************************************************************\
| Private Functions                                                             |
\*******************************************************************************/
static uint32_t bt_codec_get_interrupt_time (void)
{
    /* Check interrupt interval */
    uint32_t curr_cnt = 0;
    hal_gpt_status_t result = hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &curr_cnt);
    if (HAL_GPT_STATUS_OK != result) {
        printf("gpt result %d\r\n", result);
    }
    return curr_cnt;
}

static void bt_hfp_cvsd_open (bt_hfp_speech_tx_information_t *p_tx_info, bt_hfp_speech_rx_information_t *p_rx_info)
{
    uint32_t tx_buffer_size = 0;
    uint32_t rx_buffer_size = 0;
    uint8_t *tx_buffer_pointer;
    uint8_t *rx_buffer_pointer;
    uint32_t encoder_buffer_size           = cvsd_encoder_get_buffer_size();
    uint32_t src_up_sampling_buffer_size   = cvsd_src_up_sampling_get_buffer_size();
    uint32_t decoder_buffer_size           = cvsd_decoder_get_buffer_size();
    uint32_t src_down_sampling_buffer_size = cvsd_src_down_sampling_get_buffer_size();
    uint32_t plc_buffer_size               = plc_get_buffer_size();
    {   /* Force buffer size to be 4-byte-aligned */
        encoder_buffer_size           = FOUR_BYTE_ALIGNED(encoder_buffer_size);
        src_up_sampling_buffer_size   = FOUR_BYTE_ALIGNED(src_up_sampling_buffer_size);
        decoder_buffer_size           = FOUR_BYTE_ALIGNED(decoder_buffer_size);
        src_down_sampling_buffer_size = FOUR_BYTE_ALIGNED(src_down_sampling_buffer_size);
        plc_buffer_size               = FOUR_BYTE_ALIGNED(plc_buffer_size);
    }
    {   /* Record CVSD related SWIP internal buffer sizes */
        p_tx_info->encoder.internal_buffer_size           = encoder_buffer_size;
        p_tx_info->src_up_sampling.internal_buffer_size   = src_up_sampling_buffer_size;
        p_rx_info->decoder.internal_buffer_size           = decoder_buffer_size;
        p_rx_info->src_down_sampling.internal_buffer_size = src_down_sampling_buffer_size;
        p_rx_info->plc.internal_buffer_size               = plc_buffer_size;
    }
    {   /* Sum up to total internal buffer size */
        tx_buffer_size += encoder_buffer_size;
        tx_buffer_size += src_up_sampling_buffer_size;
        tx_buffer_size += BT_HFP_TX_BITSTREAM_BUFFER_SIZE;
        tx_buffer_size += BT_HFP_TX_CVSD_PCM_8KHZ_BUFFER_SIZE_PER_PACKET;
        tx_buffer_size += BT_HFP_TX_CVSD_PCM_64KHZ_BUFFER_SIZE_PER_PACKET;
        tx_buffer_size += BT_HFP_TX_CVSD_PCM_TEMP_BUFFER_SIZE_PER_PACKET;
        tx_buffer_size += BT_HFP_TX_PCM_RING_BUFFER_SIZE;
        tx_buffer_size += BT_HFP_DSP_FRAME_SIZE;
        rx_buffer_size += decoder_buffer_size;
        rx_buffer_size += src_down_sampling_buffer_size;
        rx_buffer_size += plc_buffer_size;
        rx_buffer_size += BT_HFP_RX_BITSTREAM_BUFFER_SIZE;
        rx_buffer_size += BT_HFP_RX_CVSD_PCM_64KHZ_BUFFER_SIZE_PER_PACKET;
        rx_buffer_size += BT_HFP_RX_CVSD_PCM_8KHZ_BUFFER_SIZE_PER_PACKET;
        rx_buffer_size += BT_HFP_RX_CVSD_PCM_TEMP_BUFFER_SIZE_PER_PACKET;
        rx_buffer_size += BT_HFP_RX_PCM_RING_BUFFER_SIZE;
        rx_buffer_size += BT_HFP_DSP_FRAME_SIZE;
    }
    {   /* Allocate total internal buffer */
        tx_buffer_pointer = &g_codec_tx_buffer[0];
        p_tx_info->buffer_size    = tx_buffer_size;
        p_tx_info->buffer_pointer = tx_buffer_pointer;
        rx_buffer_pointer = &g_codec_rx_buffer[0];
        p_rx_info->buffer_size    = rx_buffer_size;
        p_rx_info->buffer_pointer = rx_buffer_pointer;
    }
    {   /* Assign internal buffer to each SWIP */
        ASSIGN_HANDLE_UPDATE_POINTER          (tx_buffer_pointer, p_tx_info->encoder.handle          , encoder_buffer_size);
        ASSIGN_HANDLE_UPDATE_POINTER          (tx_buffer_pointer, p_tx_info->src_up_sampling.handle  , src_up_sampling_buffer_size);
        ASSIGN_U8_TX_2D_BUFFER_UPDATE_POINTER (tx_buffer_pointer, p_tx_info->pkt_buf                 , BT_HFP_TX_BITSTREAM_BUFFER_SIZE);
        ASSIGN_S16_BUFFER_UPDATE_POINTER      (tx_buffer_pointer, p_tx_info->pcm_8khz_buf            , BT_HFP_TX_CVSD_PCM_8KHZ_BUFFER_SIZE_PER_PACKET);
        ASSIGN_S16_BUFFER_UPDATE_POINTER      (tx_buffer_pointer, p_tx_info->pcm_64khz_buf           , BT_HFP_TX_CVSD_PCM_64KHZ_BUFFER_SIZE_PER_PACKET);
        ASSIGN_S16_BUFFER_UPDATE_POINTER      (tx_buffer_pointer, p_tx_info->pcm_tmp_buf             , BT_HFP_TX_CVSD_PCM_TEMP_BUFFER_SIZE_PER_PACKET);
        ASSIGN_U8_BUFFER_UPDATE_POINTER       (tx_buffer_pointer, p_tx_info->pcm_ring_buf            , BT_HFP_TX_PCM_RING_BUFFER_SIZE);
        ASSIGN_U16_BUFFER_UPDATE_POINTER      (tx_buffer_pointer, p_tx_info->pcm_tx_tmp_buf          , BT_HFP_DSP_FRAME_SIZE);
        ASSIGN_HANDLE_UPDATE_POINTER          (rx_buffer_pointer, p_rx_info->decoder.handle          , decoder_buffer_size);
        ASSIGN_HANDLE_UPDATE_POINTER          (rx_buffer_pointer, p_rx_info->src_down_sampling.handle, src_down_sampling_buffer_size);
        ASSIGN_HANDLE_UPDATE_POINTER          (rx_buffer_pointer, p_rx_info->plc.handle              , plc_buffer_size);
        ASSIGN_U8_RX_2D_BUFFER_UPDATE_POINTER (rx_buffer_pointer, p_rx_info->pkt_buf                 , BT_HFP_RX_BITSTREAM_BUFFER_SIZE);
        ASSIGN_S16_BUFFER_UPDATE_POINTER      (rx_buffer_pointer, p_rx_info->pcm_64khz_buf           , BT_HFP_RX_CVSD_PCM_64KHZ_BUFFER_SIZE_PER_PACKET);
        ASSIGN_S16_BUFFER_UPDATE_POINTER      (rx_buffer_pointer, p_rx_info->pcm_8khz_buf            , BT_HFP_RX_CVSD_PCM_8KHZ_BUFFER_SIZE_PER_PACKET);
        ASSIGN_S16_BUFFER_UPDATE_POINTER      (rx_buffer_pointer, p_rx_info->pcm_tmp_buf             , BT_HFP_RX_CVSD_PCM_TEMP_BUFFER_SIZE_PER_PACKET);
        ASSIGN_U8_BUFFER_UPDATE_POINTER       (rx_buffer_pointer, p_rx_info->pcm_ring_buf            , BT_HFP_RX_PCM_RING_BUFFER_SIZE);
        ASSIGN_U16_BUFFER_UPDATE_POINTER      (rx_buffer_pointer, p_rx_info->pcm_rx_tmp_buf          , BT_HFP_DSP_FRAME_SIZE);
    }
    return;
}

static void bt_hfp_cvsd_close(bt_hfp_speech_tx_information_t *p_tx_info, bt_hfp_speech_rx_information_t *p_rx_info)
{
    memset(g_codec_tx_buffer, 0, sizeof(g_codec_tx_buffer));
    memset(g_codec_rx_buffer, 0, sizeof(g_codec_rx_buffer));
    return;
}

static void bt_hfp_msbc_open(bt_hfp_speech_tx_information_t *p_tx_info, bt_hfp_speech_rx_information_t *p_rx_info)
{
    uint32_t tx_buffer_size = 0;
    uint32_t rx_buffer_size = 0;
    uint8_t *tx_buffer_pointer;
    uint8_t *rx_buffer_pointer;
    uint32_t encoder_buffer_size = msbc_encoder_get_buffer_size();
    uint32_t decoder_buffer_size = msbc_decoder_get_buffer_size();
    uint32_t plc_buffer_size     = plc_get_buffer_size();
    {   /* Force buffer size to be 4-byte-aligned */
        encoder_buffer_size = FOUR_BYTE_ALIGNED(encoder_buffer_size);
        decoder_buffer_size = FOUR_BYTE_ALIGNED(decoder_buffer_size);
        plc_buffer_size     = FOUR_BYTE_ALIGNED(plc_buffer_size);
    }
    {   /* Record CVSD related SWIP internal buffer sizes */
        p_tx_info->encoder.internal_buffer_size = encoder_buffer_size;
        p_rx_info->decoder.internal_buffer_size = decoder_buffer_size;
        p_rx_info->plc.internal_buffer_size     = plc_buffer_size;
    }
    {   /* Sum up to total internal buffer size */
        tx_buffer_size += encoder_buffer_size;
        tx_buffer_size += BT_HFP_TX_BITSTREAM_BUFFER_SIZE;
        tx_buffer_size += BT_HFP_TX_MSBC_PCM_16KHZ_BUFFER_SIZE_PER_FRAME;
        tx_buffer_size += BT_HFP_TX_PCM_RING_BUFFER_SIZE;
        tx_buffer_size += BT_HFP_DSP_FRAME_SIZE;
        rx_buffer_size += decoder_buffer_size;
        rx_buffer_size += plc_buffer_size;
        rx_buffer_size += BT_HFP_RX_BITSTREAM_BUFFER_SIZE;
        rx_buffer_size += BT_HFP_RX_MSBC_BITSTREAM_BUFFER_SIZE_PER_FRAME;
        rx_buffer_size += BT_HFP_RX_MSBC_PCM_16KHZ_BUFFER_SIZE_PER_FRAME;
        rx_buffer_size += BT_HFP_RX_PCM_RING_BUFFER_SIZE;
        rx_buffer_size += BT_HFP_DSP_FRAME_SIZE;
    }
    {   /* Allocate total internal buffer */
        tx_buffer_pointer = &g_codec_tx_buffer[0];
        p_tx_info->buffer_size    = tx_buffer_size;
        p_tx_info->buffer_pointer = tx_buffer_pointer;
        rx_buffer_pointer = &g_codec_rx_buffer[0];
        p_rx_info->buffer_size    = rx_buffer_size;
        p_rx_info->buffer_pointer = rx_buffer_pointer;
    }
    {   /* Assign internal buffer to each SWIP */
        ASSIGN_HANDLE_UPDATE_POINTER          (tx_buffer_pointer, p_tx_info->encoder.handle, encoder_buffer_size);
        ASSIGN_U8_TX_2D_BUFFER_UPDATE_POINTER (tx_buffer_pointer, p_tx_info->pkt_buf       , BT_HFP_TX_BITSTREAM_BUFFER_SIZE);
        ASSIGN_S16_BUFFER_UPDATE_POINTER      (tx_buffer_pointer, p_tx_info->pcm_16khz_buf , BT_HFP_TX_MSBC_PCM_16KHZ_BUFFER_SIZE_PER_FRAME);
        ASSIGN_U8_BUFFER_UPDATE_POINTER       (tx_buffer_pointer, p_tx_info->pcm_ring_buf  , BT_HFP_TX_PCM_RING_BUFFER_SIZE);
        ASSIGN_U16_BUFFER_UPDATE_POINTER      (tx_buffer_pointer, p_tx_info->pcm_tx_tmp_buf, BT_HFP_DSP_FRAME_SIZE);
        ASSIGN_HANDLE_UPDATE_POINTER          (rx_buffer_pointer, p_rx_info->decoder.handle, decoder_buffer_size);
        ASSIGN_HANDLE_UPDATE_POINTER          (rx_buffer_pointer, p_rx_info->plc.handle    , plc_buffer_size);
        ASSIGN_U8_RX_2D_BUFFER_UPDATE_POINTER (rx_buffer_pointer, p_rx_info->pkt_buf       , BT_HFP_RX_BITSTREAM_BUFFER_SIZE);
        ASSIGN_U8_BUFFER_UPDATE_POINTER       (rx_buffer_pointer, p_rx_info->pkt_tmp_buf   , BT_HFP_RX_MSBC_BITSTREAM_BUFFER_SIZE_PER_FRAME);
        ASSIGN_S16_BUFFER_UPDATE_POINTER      (rx_buffer_pointer, p_rx_info->pcm_16khz_buf , BT_HFP_RX_MSBC_PCM_16KHZ_BUFFER_SIZE_PER_FRAME);
        ASSIGN_U8_BUFFER_UPDATE_POINTER       (rx_buffer_pointer, p_rx_info->pcm_ring_buf  , BT_HFP_RX_PCM_RING_BUFFER_SIZE);
        ASSIGN_U16_BUFFER_UPDATE_POINTER      (rx_buffer_pointer, p_rx_info->pcm_rx_tmp_buf, BT_HFP_DSP_FRAME_SIZE);
    }
    return;
}

static void bt_hfp_msbc_close(bt_hfp_speech_tx_information_t *p_tx_info, bt_hfp_speech_rx_information_t *p_rx_info)
{
    memset(g_codec_tx_buffer, 0, sizeof(g_codec_tx_buffer));
    memset(g_codec_rx_buffer, 0, sizeof(g_codec_rx_buffer));
    return;
}

static uint32_t bt_hfp_get_packet_type_index(uint32_t pkt_type)
{
    uint32_t type_idx;
    if (pkt_type == BT_HFP_PKT_LEN_60) {
        type_idx = 0;
    } else if (pkt_type == BT_HFP_PKT_LEN_30) {
        type_idx = 1;
    } else if (pkt_type == BT_HFP_PKT_LEN_20) {
        type_idx = 2;
    } else if (pkt_type == BT_HFP_PKT_LEN_10) {
        type_idx = 3;
    } else {
        type_idx = BT_HFP_PKT_TYPE_IDX_MAX;
    }
    return type_idx;
}

static void bt_hfp_read_from_shared_memory (bt_hfp_shared_memory_information_t *p_mem_info, uint8_t *p_dst)
{
    /* Copy from shared memory to temp buffer */
    uint32_t offset = *(p_mem_info->p_offset_r);
    volatile uint8_t *p_src = (volatile uint8_t *)(p_mem_info->base_address + offset);
    uint32_t bs_idx;
    for (bs_idx = 0; bs_idx < BT_HFP_HW_SRAM_SIZE; bs_idx++) {
        *p_dst++ = *p_src++;
    }
    return;
}

static void bt_hfp_write_to_shared_memory (bt_hfp_shared_memory_information_t *p_mem_info, uint8_t *p_src)
{
    /* Copy from temp buffer to shared memory  */
    uint32_t offset = *(p_mem_info->p_offset_w);
    volatile uint8_t *p_dst = (volatile uint8_t *)(p_mem_info->base_address + offset);
    uint32_t bs_idx;
    for (bs_idx = 0; bs_idx < BT_HFP_HW_SRAM_SIZE; bs_idx++) {
        *p_dst++ = *p_src++;
    }
    return;
}

static void bt_hfp_fill_pattern_to_shared_memory (bt_hfp_shared_memory_information_t *p_mem_info, uint8_t pattern)
{
    /* Fill shared memory with pattern */
    uint32_t offset = *(p_mem_info->p_offset_w);
    volatile uint8_t *p_dst = (volatile uint8_t *)(p_mem_info->base_address + offset);
    uint32_t bs_idx;
    for (bs_idx = 0; bs_idx < BT_HFP_HW_SRAM_SIZE; bs_idx++) {
        *p_dst++ = pattern;
    }
    return;
}

static void bt_hfp_read_from_bt(bt_hfp_shared_memory_information_t *p_mem_info, bt_hfp_speech_rx_information_t *p_rx_info, uint32_t *p_tmp_buf, uint32_t mask_array[BT_HFP_HW_SRAM_RX_PKT_CNT], uint32_t bt_hw_ctrl)
{
    bt_hfp_read_from_shared_memory(p_mem_info, (uint8_t *)p_tmp_buf);
    {   /* Copy from temp buffer to decoder input buffer & Check if packet is valid */
        uint32_t blk_idx;
        uint8_t *p_src                                              = (uint8_t *)p_tmp_buf;
        uint32_t rx_pkt_w                                           = p_rx_info->pkt_w;
        uint8_t  (*rx_pkt_buf)[BT_HFP_RX_BITSTREAM_SIZE_PER_PACKET] = p_rx_info->pkt_buf;
        uint32_t *rx_pkt_valid                                      = p_rx_info->pkt_valid;
        for (blk_idx = 0; blk_idx < BT_HFP_HW_SRAM_RX_PKT_CNT; blk_idx++) {
            uint32_t pkt_idx = rx_pkt_w & BT_HFP_RX_PACKET_MASK;
            {   /* Copy from temp buffer to decoder input buffer */
                uint8_t *p_dst = rx_pkt_buf[pkt_idx];
                memcpy(p_dst, p_src, BT_HFP_RX_BITSTREAM_SIZE_PER_PACKET);
                p_src += BT_HFP_RX_BITSTREAM_SIZE_PER_PACKET;
            }
            {   /* Check if packet is valid */
                uint32_t tab_val = mask_array[blk_idx];
                uint32_t reg_val = bt_hw_ctrl & tab_val;
                rx_pkt_valid[pkt_idx] = reg_val == tab_val ? BT_HFP_TRUE : BT_HFP_FALSE;
            }
            rx_pkt_w++;
        }
        p_rx_info->pkt_w = rx_pkt_w;
    }
    return;
}

static void bt_hfp_write_to_bt(bt_hfp_shared_memory_information_t *p_mem_info, bt_hfp_speech_tx_information_t *p_tx_info, uint32_t *p_tmp_buf)
{
    {   /* Copy from encoder output buffer to temp buffer */
        uint32_t blk_idx;
        uint8_t *p_dst                                              = (uint8_t *)p_tmp_buf;
        uint32_t tx_pkt_r                                           = p_tx_info->pkt_r;
        uint8_t  (*tx_pkt_buf)[BT_HFP_TX_BITSTREAM_SIZE_PER_PACKET] = p_tx_info->pkt_buf;
        for (blk_idx = 0; blk_idx < BT_HFP_HW_SRAM_TX_PKT_CNT; blk_idx++) {
            uint32_t pkt_idx = tx_pkt_r & BT_HFP_TX_PACKET_MASK;
            uint8_t *p_src   = tx_pkt_buf[pkt_idx];
            memcpy(p_dst, p_src, BT_HFP_TX_BITSTREAM_SIZE_PER_PACKET);
            p_dst += BT_HFP_TX_BITSTREAM_SIZE_PER_PACKET;
            tx_pkt_r++;
        }
        p_tx_info->pkt_r = tx_pkt_r;
    }
    bt_hfp_write_to_shared_memory(p_mem_info, (uint8_t *)p_tmp_buf);
    return;
}

static void bt_codec_read_from_rx_buffer (bt_hfp_shared_memory_information_t *p_mem_info, bt_hfp_speech_rx_information_t *p_rx_info, uint32_t *p_tmp_buf, uint32_t *mask_array, uint32_t bt_hw_ctrl)
{
    uint32_t w_pkt_cnt        = p_rx_info->pkt_w;
    uint32_t r_pkt_cnt        = p_rx_info->pkt_r;
    uint32_t remained_pkt_cnt = REMAINED_PKT_CNT(w_pkt_cnt, r_pkt_cnt);
    uint32_t rx_overflow;
    if (remained_pkt_cnt > 6) {
        rx_overflow = BT_HFP_TRUE;
    } else if (remained_pkt_cnt == 0) {
        rx_overflow = BT_HFP_FALSE;
    } else {
        rx_overflow = p_rx_info->pkt_overflow;
    }
    if (rx_overflow) {
        LISR_LOG_W("BT Rx OF\r\n");
    } else {
        bt_hfp_read_from_bt(p_mem_info, p_rx_info, p_tmp_buf, mask_array, bt_hw_ctrl);
    }
    p_rx_info->pkt_overflow = rx_overflow;
    return;
}

static void bt_codec_write_to_tx_buffer (bt_hfp_shared_memory_information_t *p_mem_info, bt_hfp_speech_tx_information_t *p_tx_info, uint32_t *p_tmp_buf, bt_hf_codec_type_t codec_type)
{
    uint32_t w_pkt_cnt          = p_tx_info->pkt_w;
    uint32_t r_pkt_cnt          = p_tx_info->pkt_r;
    uint32_t remained_pkt_cnt   = REMAINED_PKT_CNT(w_pkt_cnt, r_pkt_cnt);
    uint32_t data_byte_cnt      = p_tx_info->pcm_w; /* PCM buffer, unit: byte */
    uint32_t threshold_byte_cnt = codec_type == BT_HF_CODEC_CVSD ? 40 : 80; /* 2.5 ms for CVSD 8kHz PCM buffer & mSBC 16kHz PCM buffer */
    uint32_t tx_underflow;
    if (remained_pkt_cnt < 3) {
        tx_underflow = BT_HFP_TRUE;
    } else if (remained_pkt_cnt >= 6 || (remained_pkt_cnt == 5 && data_byte_cnt >= threshold_byte_cnt)) {
        tx_underflow = BT_HFP_FALSE;    /* [6 x 7.5 ms] or [5 x 7.5 ms + 2.5 ms] */
    } else {
        tx_underflow = p_tx_info->pkt_underflow;
    }
    if (tx_underflow) {
        uint8_t pattern = codec_type == BT_HF_CODEC_CVSD ? 0x55 : 0x00;
        bt_hfp_fill_pattern_to_shared_memory (p_mem_info, pattern);
        LISR_LOG_W("BT Tx UF\r\n");
    } else {
        bt_hfp_write_to_bt(p_mem_info, p_tx_info, p_tmp_buf);
    }
    p_tx_info->pkt_underflow = tx_underflow;
    return;
}

static void bt_codec_event_send_from_isr (bt_codec_queue_event_id_t id, void *parameter)
{
    bt_codec_queue_event_t event;
    event.id        = id;
    event.parameter = parameter;
    if (xQueueSendFromISR(bt_codec_queue_handle, &event, 0) != pdPASS) {
        LISR_LOG_W("queue not pass %d\r\n", id);
        return;
    }
    return;
}

static void bt_codec_event_register_callback (bt_codec_queue_event_id_t reg_id, bt_codec_callback_t callback)
{
    uint32_t id_idx;
    for (id_idx = 0; id_idx < MAX_BT_CODEC_FUNCTIONS; id_idx++) {
        if (bt_codec_queue_id_array[id_idx] == BT_CODEC_QUEUE_EVENT_NONE) {
            bt_codec_queue_id_array[id_idx] = reg_id;
            bt_codec_queue_handler[id_idx]  = callback;
            bt_codec_queue_reg_num++;
            break;
        }
    }
    return;
}

static void bt_codec_event_deregister_callback (bt_codec_queue_event_id_t dereg_id)
{
    uint32_t id_idx;
    for (id_idx = 0; id_idx < MAX_BT_CODEC_FUNCTIONS; id_idx++) {
        if (bt_codec_queue_id_array[id_idx] == dereg_id) {
            bt_codec_queue_id_array[id_idx] = BT_CODEC_QUEUE_EVENT_NONE;
            bt_codec_queue_reg_num--;
            break;
        }
    }
    return;
}

static void bt_hfp_tx_cvsd_encode(bt_hfp_speech_tx_information_t *p_tx_info)
{
    /* 8kHz speech Tx buffer --> 64kHz temp PCM buffer --> CVSD bitstream buffer */
    ring_buffer_information_t *p_tx_ring = &p_tx_info->pcm_ring_info;
    uint32_t data_8k_byte_cnt   = p_tx_info->pcm_w;
    uint32_t space_8k_byte_cnt  = BT_HFP_TX_CVSD_PCM_8KHZ_BUFFER_SIZE_PER_PACKET - data_8k_byte_cnt;
    uint32_t b_pcm_empty        = BT_HFP_FALSE; /* 8kHz speech Tx buffer flag */
    uint32_t b_cvsd_full        = BT_HFP_FALSE; /* CVSD Tx bitstream buffer flag */
    TASK_LOG_I("+cvsd_encode\r\n");
    while (!b_cvsd_full && !b_pcm_empty) {
        uint32_t data_64k_byte_cnt  = 0;
        uint32_t space_64k_byte_cnt = BT_HFP_TX_CVSD_PCM_64KHZ_BUFFER_SIZE_PER_PACKET;
        {   /* Move 8kHz speech Tx ring buffer data to 8kHz temp PCM buffer */
            uint32_t loop_idx;
            for (loop_idx = 0; loop_idx < 2; loop_idx++) {
                if (data_8k_byte_cnt < BT_HFP_TX_CVSD_PCM_8KHZ_BUFFER_SIZE_PER_PACKET) {
                    uint8_t *p_read_buf    = NULL;
                    uint32_t read_byte_cnt = 0;
                    ring_buffer_get_read_information(p_tx_ring, &p_read_buf, &read_byte_cnt);
                    if (read_byte_cnt > 0) {
                        uint8_t *p_ou_base         = (uint8_t *)p_tx_info->pcm_8khz_buf;
                        int16_t *p_in_buf          = (int16_t *)p_read_buf;
                        int16_t *p_ou_buf          = (int16_t *)(p_ou_base + data_8k_byte_cnt);
                        uint32_t consumed_byte_cnt = MINIMUM(read_byte_cnt >> 1, space_8k_byte_cnt >> 1) << 1;
                        memcpy(p_ou_buf, p_in_buf, consumed_byte_cnt);
                        data_8k_byte_cnt  += consumed_byte_cnt;
                        space_8k_byte_cnt -= consumed_byte_cnt;
                        ring_buffer_read_done(p_tx_ring, consumed_byte_cnt);
                    } else {    /* 8kHz speech Tx buffer is empty */
                        b_pcm_empty = BT_HFP_TRUE;
                        break;
                    }
                }
            }
        }
        if (data_8k_byte_cnt >= BT_HFP_TX_CVSD_PCM_8KHZ_BUFFER_SIZE_PER_PACKET) { /* 8kHz temp PCM buffer --> 64kHz temp PCM buffer */
            void *handle               = p_tx_info->src_up_sampling.handle;
            int16_t *p_in_buf          = p_tx_info->pcm_8khz_buf;
            int16_t *p_ou_buf          = p_tx_info->pcm_64khz_buf;
            int16_t *p_tmp_buf         = p_tx_info->pcm_tmp_buf;
            uint32_t in_byte_cnt       = BT_HFP_TX_CVSD_PCM_8KHZ_BUFFER_SIZE_PER_PACKET;
            uint32_t ou_byte_cnt       = in_byte_cnt << 3;
            int32_t result;
            result = cvsd_src_up_sampling_process (handle, p_tmp_buf, p_in_buf, p_ou_buf, in_byte_cnt);
            if (result < 0) {   /* CVSD SRC up sampling error */
                TASK_LOG_E("CVSD up %ld\r\n", result);
                return;
            }
            data_64k_byte_cnt  += ou_byte_cnt;
            space_64k_byte_cnt -= ou_byte_cnt;
            data_8k_byte_cnt    = 0;
            space_8k_byte_cnt   = BT_HFP_TX_CVSD_PCM_8KHZ_BUFFER_SIZE_PER_PACKET;
        }
        if (!b_pcm_empty && data_64k_byte_cnt >= BT_HFP_TX_CVSD_PCM_64KHZ_BUFFER_SIZE_PER_PACKET) { /* 64kHz temp PCM buffer --> CVSD bitstream buffer */
            uint32_t w_pkt_cnt        = p_tx_info->pkt_w;
            uint32_t r_pkt_cnt        = p_tx_info->pkt_r;
            uint32_t remained_pkt_cnt = REMAINED_PKT_CNT(w_pkt_cnt, r_pkt_cnt);
            TASK_LOG_I(" cvsd_encode, w_pkt_cnt %d, r_pkt_cnt %d\r\n", w_pkt_cnt, r_pkt_cnt);
            if (remained_pkt_cnt < BT_HFP_TX_PACKET_NUM) {  /* Encode */
                void    *handle      = p_tx_info->encoder.handle;
                int16_t *p_in_buf    = (int16_t *)p_tx_info->pcm_64khz_buf;
                uint8_t *p_ou_buf    = (uint8_t *)p_tx_info->pkt_buf[w_pkt_cnt & BT_HFP_TX_PACKET_MASK];
                uint32_t in_byte_cnt = BT_HFP_TX_CVSD_PCM_64KHZ_BUFFER_SIZE_PER_PACKET;
                uint32_t ou_byte_cnt = BT_HFP_TX_BITSTREAM_SIZE_PER_PACKET;
                int32_t result;
                result = cvsd_encode_process (handle, p_in_buf, &in_byte_cnt, p_ou_buf, &ou_byte_cnt);
                if (result < 0) {   /* CVSD encode error */
                    TASK_LOG_E("CVSD en %ld\r\n", result);
                    return;
                }
                w_pkt_cnt++;
            } else {    /* CVSD Tx bitstream buffer is full */
                b_cvsd_full = BT_HFP_TRUE;
            }
            p_tx_info->pkt_w = w_pkt_cnt;
        }
    }
    p_tx_info->pcm_w = data_8k_byte_cnt;
    TASK_LOG_I("-cvsd_encode\r\n");
    return;
}

static void bt_hfp_rx_cvsd_decode(bt_hfp_speech_rx_information_t *p_rx_info, uint32_t control_reg, uint32_t isr_time)
{
    /* CVSD bitstream buffer --> 64kHz temp PCM buffer --> 8kHz temp PCM buffer --> 8kHz speech Rx buffer */
    ring_buffer_information_t *p_rx_ring = &p_rx_info->pcm_ring_info;
    uint32_t space_byte_cnt = p_rx_info->pcm_r;                                                 /* 8kHz temp PCM buffer space byte count */
    uint32_t data_byte_cnt  = BT_HFP_RX_CVSD_PCM_8KHZ_BUFFER_SIZE_PER_PACKET - space_byte_cnt;  /* 8kHz temp PCM buffer data byte count */
    uint32_t b_cvsd_empty   = BT_HFP_FALSE;
    uint32_t b_pcm_full     = BT_HFP_FALSE;
    uint32_t num_bad        = 0;
    uint32_t num_good       = 0;
    uint32_t num_headfail   = 0;
    TASK_LOG_I("+cvsd_decode\r\n");
    while (!b_cvsd_empty && !b_pcm_full) {
        if (space_byte_cnt >= BT_HFP_RX_CVSD_PCM_8KHZ_BUFFER_SIZE_PER_PACKET) { /* 8kHz temp PCM buffer is empty, decode new CVSD bitstream */
            uint32_t w_pkt_cnt = p_rx_info->pkt_w;
            uint32_t r_pkt_cnt = p_rx_info->pkt_r;
            TASK_LOG_I(" cvsd_decode, w_pkt_cnt %d, r_pkt_cnt %d\r\n", w_pkt_cnt, r_pkt_cnt);
            if (w_pkt_cnt != r_pkt_cnt) {   /* Decode 30-byte CVSD bitstream --> 480-byte 64kHz PCM --> 60-byte 8kHz PCM */
                uint32_t valid_flag = p_rx_info->pkt_valid[r_pkt_cnt & BT_HFP_RX_PACKET_MASK];
                plc_runtime_parameter_t param;
                if (valid_flag) {   /* Packet Valid */
                    {   /* Decode 3.75 ms (CVSD bitstream buffer --> 64kHz temp PCM buffer) */
                        void    *handle      = p_rx_info->decoder.handle;
                        uint8_t *p_in_buf    = p_rx_info->pkt_buf[r_pkt_cnt & BT_HFP_RX_PACKET_MASK];
                        int16_t *p_ou_buf    = p_rx_info->pcm_64khz_buf;
                        uint32_t in_byte_cnt = BT_HFP_RX_BITSTREAM_SIZE_PER_PACKET; /* 30 bytes */
                        uint32_t ou_byte_cnt = BT_HFP_RX_CVSD_PCM_64KHZ_BUFFER_SIZE_PER_PACKET; /* 240 samples @ 64kHz (= 3.75 ms) */
                        int32_t  result;
                        result = cvsd_decode_process (handle, p_in_buf, &in_byte_cnt, p_ou_buf, &ou_byte_cnt);
                        if (result < 0) {   /* CVSD decode error */
                            TASK_LOG_E("CVSD de %ld\r\n", result);
                            return;
                        }
                    }
                    {   /* Downsample 3.75 ms (64kHz temp PCM buffer --> 8kHz temp PCM buffer) */
                        int16_t *p_in_buf    = p_rx_info->pcm_64khz_buf;
                        int16_t *p_ou_buf    = p_rx_info->pcm_8khz_buf;
                        int16_t *p_tmp_buf   = p_rx_info->pcm_tmp_buf;
                        void    *handle      = p_rx_info->src_down_sampling.handle;
                        uint32_t in_byte_cnt = BT_HFP_RX_CVSD_PCM_64KHZ_BUFFER_SIZE_PER_PACKET; /* 240 samples @ 64kHz (= 3.75 ms) */
                        int32_t result;
                        result = cvsd_src_down_sampling_process (handle, p_tmp_buf, p_in_buf, p_ou_buf, in_byte_cnt);
                        if (result < 0) {   /* CVSD SRC down sampling error */
                            TASK_LOG_E("CVSD dw %ld\r\n", result);
                            return;
                        }
                    }
                    param.bad_frame_indicator  = 0;
                    param.bt_ev3_half_bad_flag = 0;
                    num_good++;
                } else {            /* Packet Invalid */
                    int16_t *p_ou_buf = p_rx_info->pcm_8khz_buf;
                    memset(p_ou_buf, 0, BT_HFP_RX_CVSD_PCM_8KHZ_BUFFER_SIZE_PER_PACKET);
                    param.bad_frame_indicator  = 1;
                    param.bt_ev3_half_bad_flag = 0;
                    p_rx_info->pkt_lost_cnt++;
                    num_bad++;
                    TASK_LOG_I("cvsd lost %d\r\n", p_rx_info->pkt_lost_cnt);
                }
                {   /* Apply PLC */
                    int16_t *p_ou_buf    = p_rx_info->pcm_8khz_buf;
                    void    *handle      = p_rx_info->plc.handle;
                    int32_t result;
                    result = plc_process (handle, p_ou_buf, &param);
                    if (result < 0) {   /* CVSD PLC error */
                        TASK_LOG_E("CVSD plc %ld\r\n", result);
                    }
                }
                r_pkt_cnt++;
                space_byte_cnt = 0;
                data_byte_cnt = BT_HFP_RX_CVSD_PCM_8KHZ_BUFFER_SIZE_PER_PACKET;
            } else {    /* CVSD Rx bitsream buffer is empty */
                b_cvsd_empty = BT_HFP_TRUE;
            }
            p_rx_info->pkt_r = r_pkt_cnt;
        }
        if (!b_cvsd_empty) {    /* Move at most twice for ring buffer usage (8kHz temp PCM buffer --> 8kHz speech Rx buffer) */
            uint32_t loop_idx;
            for (loop_idx = 0; loop_idx < 2; loop_idx++) {
                if (space_byte_cnt < BT_HFP_RX_CVSD_PCM_8KHZ_BUFFER_SIZE_PER_PACKET) {
                    uint8_t *p_ou_buf    = NULL;
                    uint32_t ou_byte_cnt = 0;
                    ring_buffer_get_write_information(p_rx_ring, &p_ou_buf, &ou_byte_cnt);
                    if (ou_byte_cnt > 0) {
                        uint32_t consumed_byte_cnt = MINIMUM(data_byte_cnt, ou_byte_cnt);
                        uint8_t *p_in_base         = (uint8_t *)p_rx_info->pcm_8khz_buf;
                        uint8_t *p_in_buf          = p_in_base + space_byte_cnt;
                        memcpy(p_ou_buf, p_in_buf, consumed_byte_cnt);
                        space_byte_cnt += consumed_byte_cnt;
                        data_byte_cnt  -= consumed_byte_cnt;
                        ring_buffer_write_done (p_rx_ring, consumed_byte_cnt);
                    } else {    /* 8kHz speech Rx buffer is full */
                        b_pcm_full = BT_HFP_TRUE;
                        break;
                    }
                }
            }
        }
    }
    {
        uint32_t acc_bad      = p_rx_info->acc_bad;
        uint32_t acc_good     = p_rx_info->acc_good;
        uint32_t acc_headfail = p_rx_info->acc_headfail;
        acc_bad      += num_bad;
        acc_good     += num_good;
        acc_headfail += num_headfail;
        p_rx_info->acc_bad      = acc_bad;
        p_rx_info->acc_good     = acc_good;
        p_rx_info->acc_headfail = acc_headfail;
        if (num_bad > 0) {
            TASK_LOG_W("CVSD %d %d %d %d %d %x %d %d\r\n", isr_time, acc_bad, acc_good, num_bad, num_good, control_reg, acc_headfail, num_headfail);
        }
    }
    p_rx_info->pcm_r = space_byte_cnt;
    TASK_LOG_I("-cvsd_decode\r\n");
    return;
}

static void bt_hfp_tx_msbc_encode(bt_hfp_speech_tx_information_t *p_tx_info)
{
    /* 16kHz speech Tx buffer --> mSBC bitstream buffer */
    ring_buffer_information_t *p_tx_ring = &p_tx_info->pcm_ring_info;
    uint32_t data_byte_cnt  = p_tx_info->pcm_w;
    uint32_t space_byte_cnt = BT_HFP_TX_MSBC_PCM_16KHZ_BUFFER_SIZE_PER_FRAME - data_byte_cnt;
    uint32_t b_pcm_empty    = BT_HFP_FALSE; /* 16kHz speech Tx buffer flag */
    uint32_t b_msbc_full    = BT_HFP_FALSE; /* mSBC Tx bitstream buffer flag */
    TASK_LOG_I("+msbc_encode\r\n");
    while (!b_msbc_full && !b_pcm_empty) {
        {   /* Move at most twice for ring buffer usage (16kHz speech Tx buffer --> 16kHz temp PCM buffer) */
            uint32_t loop_idx;
            for (loop_idx = 0; loop_idx < 2; loop_idx++) {
                if (data_byte_cnt < BT_HFP_TX_MSBC_PCM_16KHZ_BUFFER_SIZE_PER_FRAME) {
                    uint8_t *p_in_buf    = NULL;
                    uint32_t in_byte_cnt = 0;
                    ring_buffer_get_read_information(p_tx_ring, &p_in_buf, &in_byte_cnt);
                    if (in_byte_cnt > 0) {
                        uint32_t consumed_byte_cnt = MINIMUM(space_byte_cnt, in_byte_cnt);
                        uint8_t *p_ou_base         = (uint8_t *)p_tx_info->pcm_16khz_buf;
                        uint8_t *p_ou_buf          = p_ou_base + data_byte_cnt;
                        memcpy(p_ou_buf, p_in_buf, consumed_byte_cnt);
                        data_byte_cnt  += consumed_byte_cnt;
                        space_byte_cnt -= consumed_byte_cnt;
                        ring_buffer_read_done (p_tx_ring, consumed_byte_cnt);
                    } else {    /* 16kHz speech Tx buffer is empty */
                        b_pcm_empty = BT_HFP_TRUE;
                        break;
                    }
                }
            }
        }
        if (!b_pcm_empty && data_byte_cnt >= BT_HFP_TX_MSBC_PCM_16KHZ_BUFFER_SIZE_PER_FRAME) {  /* 16kHz temp PCM buffer --> mSBC bitstream buffer */
            uint32_t w_pkt_cnt        = p_tx_info->pkt_w;
            uint32_t r_pkt_cnt        = p_tx_info->pkt_r;
            uint32_t remained_pkt_cnt = REMAINED_PKT_CNT(w_pkt_cnt, r_pkt_cnt);
            if (remained_pkt_cnt < BT_HFP_TX_PACKET_NUM) {  /* Encode */
                void    *handle      = p_tx_info->encoder.handle;
                uint32_t w_pkt_idx   = w_pkt_cnt & BT_HFP_TX_PACKET_MASK;
                int16_t *p_in_buf    = (int16_t *)p_tx_info->pcm_16khz_buf;
                uint8_t *p_ou_buf    = (uint8_t *)&p_tx_info->pkt_buf[w_pkt_idx][2];
                uint32_t in_byte_cnt = BT_HFP_TX_MSBC_PCM_16KHZ_BUFFER_SIZE_PER_FRAME;
                uint32_t ou_byte_cnt = BT_HFP_TX_MSBC_BITSTREAM_SIZE_PER_FRAME;
                int32_t result;
                result = msbc_encode_process (handle, p_in_buf, &in_byte_cnt, p_ou_buf, &ou_byte_cnt);
                if (result < 0) {   /* mSBC encode error */
                    TASK_LOG_E("mSBC en %ld\r\n", result);
                    return;
                }
                p_tx_info->pkt_buf[w_pkt_idx][ 0] = 0x01;
                p_tx_info->pkt_buf[w_pkt_idx][ 1] = bt_hfp_msbc_header[w_pkt_idx & 0x3];
                p_tx_info->pkt_buf[w_pkt_idx][59] = 0x00;
                data_byte_cnt  = 0;
                space_byte_cnt = BT_HFP_TX_MSBC_PCM_16KHZ_BUFFER_SIZE_PER_FRAME;
                w_pkt_cnt++;
            } else {    /* mSBC Tx bitstream buffer is full */
                b_msbc_full = BT_HFP_TRUE;
            }
            p_tx_info->pkt_w = w_pkt_cnt;
        }
    }
    p_tx_info->pcm_w = data_byte_cnt;
    TASK_LOG_I("-msbc_encode\r\n");
    return;
}

static void bt_hfp_rx_msbc_decode(bt_hfp_speech_rx_information_t *p_rx_info, uint32_t control_reg, uint32_t isr_time)
{
    /* mSBC bitstream buffer --> 16kHz temp PCM buffer --> 16kHz speech Rx buffer */
    ring_buffer_information_t *p_rx_ring = &p_rx_info->pcm_ring_info;
    uint32_t space_byte_cnt = p_rx_info->pcm_r;                                                 /* 16kHz temp PCM buffer space byte count */
    uint32_t data_byte_cnt  = BT_HFP_RX_MSBC_PCM_16KHZ_BUFFER_SIZE_PER_FRAME - space_byte_cnt;  /* 16kHz temp PCM buffer data byte count */
    uint32_t b_msbc_empty   = BT_HFP_FALSE;
    uint32_t b_pcm_full     = BT_HFP_FALSE;
    uint32_t num_bad        = 0;
    uint32_t num_good       = 0;
    uint32_t num_headfail   = 0;
    TASK_LOG_I("+msbc_decode\r\n");
    while (!b_msbc_empty && !b_pcm_full) {
        if (space_byte_cnt >= BT_HFP_RX_MSBC_PCM_16KHZ_BUFFER_SIZE_PER_FRAME) { /* 16kHz temp PCM buffer is empty, decode new CVSD bitstream */
            uint32_t w_pkt_cnt = p_rx_info->pkt_w;
            uint32_t r_pkt_cnt = p_rx_info->pkt_r;
            if (w_pkt_cnt >= r_pkt_cnt + 2) {   /* Decode 57-byte mSBC bitstream --> 240-byte 16kHz PCM */
                uint32_t pkt_1_idx    = (r_pkt_cnt + 0) & BT_HFP_RX_PACKET_MASK;
                uint32_t pkt_2_idx    = (r_pkt_cnt + 1) & BT_HFP_RX_PACKET_MASK;
                uint8_t *p_in_buf_1   = p_rx_info->pkt_buf[pkt_1_idx];
                uint8_t *p_in_buf_2   = p_rx_info->pkt_buf[pkt_2_idx];
                uint32_t valid_flag_1 = p_rx_info->pkt_valid[pkt_1_idx];
                uint32_t valid_flag_2 = p_rx_info->pkt_valid[pkt_2_idx];
                uint32_t header_check = p_in_buf_1[0] == 0x01 && (p_in_buf_1[1] & 0x0F) == 0x08 ? BT_HFP_TRUE : BT_HFP_FALSE;
                plc_runtime_parameter_t param;
                if (header_check && valid_flag_1 && valid_flag_2) { /* Good Frame or 2nd Half Bad */
                    {   /* Collect 57-byte mSBC bitstream from two 30-byte Rx packets */
                        uint8_t *p_ou_buf   = p_rx_info->pkt_tmp_buf;
                        memcpy(p_ou_buf +  0, p_in_buf_1 + 2, 28);
                        memcpy(p_ou_buf + 28, p_in_buf_2 + 0, 29); /* total 57 bytes */
                    }
                    {   /* Decode 7.5 ms (mSBC bitstream buffer --> 64kHz temp PCM buffer) */
                        void    *handle      = p_rx_info->decoder.handle;
                        uint8_t *p_in_buf    = p_rx_info->pkt_tmp_buf;
                        int16_t *p_ou_buf    = p_rx_info->pcm_16khz_buf;
                        uint32_t in_byte_cnt = BT_HFP_RX_MSBC_BITSTREAM_SIZE_PER_FRAME;         /* 57 bytes */
                        uint32_t ou_byte_cnt = BT_HFP_RX_MSBC_PCM_16KHZ_BUFFER_SIZE_PER_FRAME;  /* 120 samples @ 16kHz (= 7.5 ms) */
                        int32_t  result      = msbc_decode_process (handle, p_in_buf, &in_byte_cnt, p_ou_buf, &ou_byte_cnt);
                        if (result < 0) {   /* mSBC decode error */
                            memset(p_ou_buf, 0, BT_HFP_RX_MSBC_PCM_16KHZ_BUFFER_SIZE_PER_FRAME);
                            TASK_LOG_E("mSBC de %ld\r\n", result);
                        }
                    }
                    param.bad_frame_indicator  = 0;
                    param.bt_ev3_half_bad_flag = 0;
                    num_good++;
                } else {    /* Bad Frame */
                    int16_t *p_ou_buf = p_rx_info->pcm_16khz_buf;
                    memset(p_ou_buf, 0, BT_HFP_RX_MSBC_PCM_16KHZ_BUFFER_SIZE_PER_FRAME);
                    param.bad_frame_indicator  = 1;
                    param.bt_ev3_half_bad_flag = 0;
                    p_rx_info->pkt_lost_cnt++;
                    if (valid_flag_1 && valid_flag_2) {
                        num_headfail++;
                    }
                    num_bad++;
                    TASK_LOG_I("mSBC lost %d\r\n", p_rx_info->pkt_lost_cnt);
                }
                {   /* Apply PLC */
                    int16_t *p_ou_buf    = p_rx_info->pcm_16khz_buf;
                    void    *handle      = p_rx_info->plc.handle;
                    int32_t result;
                    result = plc_process (handle, p_ou_buf, &param);
                    if (result < 0) {   /* mSBC PLC error */
                        TASK_LOG_E("mSBC plc %ld\r\n", result);
                    }
                }
                r_pkt_cnt += 2;
                space_byte_cnt = 0;
                data_byte_cnt = BT_HFP_RX_MSBC_PCM_16KHZ_BUFFER_SIZE_PER_FRAME;
            } else {    /* mSBC Rx bitsream buffer is empty */
                b_msbc_empty = BT_HFP_TRUE;
            }
            p_rx_info->pkt_r = r_pkt_cnt;
        }
        if (!b_msbc_empty) {    /* Move at most twice for ring buffer usage (16kHz temp PCM buffer --> 16kHz speech Rx buffer) */
            uint32_t loop_idx;
            for (loop_idx = 0; loop_idx < 2; loop_idx++) {
                if (space_byte_cnt < BT_HFP_RX_MSBC_PCM_16KHZ_BUFFER_SIZE_PER_FRAME) {
                    uint8_t *p_ou_buf    = NULL;
                    uint32_t ou_byte_cnt = 0;
                    ring_buffer_get_write_information(p_rx_ring, &p_ou_buf, &ou_byte_cnt);
                    if (ou_byte_cnt > 0) {
                        uint32_t consumed_byte_cnt = MINIMUM(data_byte_cnt, ou_byte_cnt);
                        uint8_t *p_in_base         = (uint8_t *)p_rx_info->pcm_16khz_buf;
                        uint8_t *p_in_buf          = p_in_base + space_byte_cnt;
                        memcpy(p_ou_buf, p_in_buf, consumed_byte_cnt);
                        space_byte_cnt += consumed_byte_cnt;
                        data_byte_cnt  -= consumed_byte_cnt;
                        ring_buffer_write_done (p_rx_ring, consumed_byte_cnt);
                    } else {    /* 16kHz speech Rx buffer is full */
                        b_pcm_full = BT_HFP_TRUE;
                        break;
                    }
                }
            }
        }
    }
    {
        uint32_t acc_bad      = p_rx_info->acc_bad;
        uint32_t acc_good     = p_rx_info->acc_good;
        uint32_t acc_headfail = p_rx_info->acc_headfail;
        acc_bad      += num_bad;
        acc_good     += num_good;
        acc_headfail += num_headfail;
        p_rx_info->acc_bad      = acc_bad;
        p_rx_info->acc_good     = acc_good;
        p_rx_info->acc_headfail = acc_headfail;
        if (num_bad > 0) {
            TASK_LOG_W("mSBC %d %d %d %d %d %x %d %d\r\n", isr_time, acc_bad, acc_good, num_bad, num_good, control_reg, acc_headfail, num_headfail);
        }
    }
    p_rx_info->pcm_r = space_byte_cnt;
    TASK_LOG_I("-msbc_decode\r\n");
    return;
}

static void bt_hfp_codec_loopback_process(ring_buffer_information_t *p_tx_ring, ring_buffer_information_t *p_rx_ring)
{
    uint32_t loop_idx;
    TASK_LOG_I("+codec_loopback\r\n");
    for (loop_idx = 0; loop_idx < 4; loop_idx++) {
        uint8_t *p_in_buf = NULL;
        uint8_t *p_ou_buf = NULL;
        uint32_t byte_cnt_in = 0;
        uint32_t byte_cnt_ou = 0;
        uint32_t byte_cnt_moved;
        ring_buffer_get_read_information  (p_rx_ring, &p_in_buf, &byte_cnt_in);
        ring_buffer_get_write_information (p_tx_ring, &p_ou_buf, &byte_cnt_ou);
        byte_cnt_moved = MINIMUM(byte_cnt_in, byte_cnt_ou);
        memcpy(p_ou_buf, p_in_buf, byte_cnt_moved);
        ring_buffer_write_done (p_tx_ring, byte_cnt_moved);
        ring_buffer_read_done  (p_rx_ring, byte_cnt_moved);
    }
    TASK_LOG_I("-codec_loopback\r\n");
    return;
}

static void bt_hfp_codec_tx_process_handler(void *parameter)
{
    bt_hfp_codec_internal_handle_t *p_info = bt_hfp_codec_internal_handle;
    RETURN_IF_FALSE(p_info != NULL);
    TASK_LOG_I("+codec_tx\r\n");
    {   /* BT Encode */
        bt_hfp_speech_tx_information_t *p_tx_info = &p_info->tx_info;
        bt_hf_codec_type_t codec_type = p_info->codec_type;
        if (codec_type == BT_HF_CODEC_CVSD) {
            bt_hfp_tx_cvsd_encode(p_tx_info);
        } else {
            bt_hfp_tx_msbc_encode(p_tx_info);
        }
    }
    TASK_LOG_I("-codec_tx\r\n");
    return;
}

static void bt_hfp_codec_rx_process_handler(void *parameter)
{
    bt_hfp_codec_internal_handle_t *p_info = bt_hfp_codec_internal_handle;
    RETURN_IF_FALSE(p_info != NULL);
    TASK_LOG_I("+codec_rx\r\n");
    {   /* BT Decode */
        bt_hfp_speech_rx_information_t *p_rx_info = &p_info->rx_info;
        bt_hf_codec_type_t codec_type = p_info->codec_type;
        bt_hfp_mode_t mode = p_info->mode;
        uint32_t control_reg = p_info->control_reg;
        uint32_t isr_time    = p_info->isr_time;
        if (codec_type == BT_HF_CODEC_CVSD) {
            bt_hfp_rx_cvsd_decode(p_rx_info, control_reg, isr_time);
        } else {
            bt_hfp_rx_msbc_decode(p_rx_info, control_reg, isr_time);
        }
        if (mode == BT_HFP_MODE_LOOPBACK_WITH_CODEC) {
            bt_hfp_speech_tx_information_t *p_tx_info = &p_info->tx_info;
            bt_hfp_speech_rx_information_t *p_rx_info = &p_info->rx_info;
            ring_buffer_information_t *p_tx_ring = &p_tx_info->pcm_ring_info;
            ring_buffer_information_t *p_rx_ring = &p_rx_info->pcm_ring_info;
            bt_hfp_codec_loopback_process(p_tx_ring, p_rx_ring);
            bt_hfp_codec_tx_process_handler(NULL);
        }
    }
    TASK_LOG_I("-codec_rx\r\n");
    return;
}

static void bt_hfp_dsp_tx_process_handler(void *parameter)
{
    bt_hfp_codec_tx_process_handler(parameter);
    return;
}

static void bt_hfp_dsp_rx_process_handler(void *parameter)
{
    return;
}

#if defined(BT_HFP_CODEC_CHECK_INTERRUPT_INTERVAL)
static void initialize_interrupt_interval (void)
{
    bt_hfp_rx_cnt = 0;
    p2w_tx_cnt = 0;
    p2w_rx_cnt = 0;
    return;
}

static void print_interrupt_interval (uint32_t *p_counter, const char *p_description)
{
    /* Check interrupt interval */
    uint32_t prev_cnt = *p_counter;
    uint32_t curr_cnt = 0;
    hal_gpt_status_t result = hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &curr_cnt);
    if (HAL_GPT_STATUS_OK != result) {
        LISR_LOG_E("EXCEPTION, [%s] gpt result = %d\r\n", p_description, result);
    } else if (prev_cnt == 0) {
        LISR_LOG_I("[%s]first time\r\n", p_description);
    } else {
        uint32_t interval = curr_cnt >= prev_cnt ? curr_cnt - prev_cnt : 0xffffffff - prev_cnt + curr_cnt;
        LISR_LOG_I("[%s]interval = %d\r\n", p_description, interval);
    }
    *p_counter = curr_cnt;
    return;
}
#endif  /* BT_HFP_CODEC_CHECK_INTERRUPT_INTERVAL */

static uint32_t bt_hfp_check_state (bt_codec_state_t ref_state)
{
    bt_hfp_codec_internal_handle_t *p_info = bt_hfp_codec_internal_handle;
    uint32_t result = BT_HFP_FALSE;
    if (p_info != NULL) {
        bt_media_handle_t *handle = &p_info->handle;
        bt_codec_state_t cur_state = handle->state;
        if (cur_state == ref_state) {
            result = BT_HFP_TRUE;
        }
    }
    return result;
}

static void bt_hfp_bt_interrupt_callback_internal (bt_hfp_codec_internal_handle_t *p_info)
{
    {   /* Check & Exchange */
        bt_hfp_shared_memory_information_t *p_mem_info = &p_info->mem_info;
        volatile uint32_t *p_control = p_mem_info->p_control;
        uint32_t bt_hw_ctrl  = *p_control;
        uint32_t pkt_type    = (bt_hw_ctrl >> 18) & 0x7;
        uint32_t type_idx    = bt_hfp_get_packet_type_index(pkt_type);
        bt_hf_codec_type_t codec_type = p_info->codec_type;
        {   /* Check information */
            if (bt_hfp_check_state(BT_CODEC_STATE_PLAY) == BT_HFP_FALSE) {
                //int32_t isr_time = bt_codec_get_interrupt_time();
                LISR_LOG_W("BT ISR not PLAY, %d\r\n", bt_codec_get_interrupt_time());
                return;
            }
            if ((bt_hw_ctrl & BT_HFP_HW_INTERRUPT) == 0) {
                LISR_LOG_I("bt interrupt re-entry\r\n");
                return;
            }
            if (type_idx >= BT_HFP_PKT_TYPE_IDX_MAX) {
                LISR_LOG_E("EXCEPTION, type_idx %d\r\n", type_idx);
                return;
            }
            if (codec_type != BT_HF_CODEC_CVSD && codec_type != BT_HF_CODEC_MSBC) {
                LISR_LOG_E("EXCEPTION, codec_type %d\r\n", codec_type);
                return;
            }
        }
        LISR_LOG_I("+bt interrupt\r\n");
        p_info->isr_time = bt_codec_get_interrupt_time();
        p_info->control_reg = bt_hw_ctrl;
        {   /* Exchange bitstream with BT shared buffer */
            uint32_t *mask_array = (uint32_t *)&bt_hfp_rx_packet_valid_mask_table[type_idx];
            uint32_t *p_tmp_buf  = p_info->pkt_tmp_buf;
            bt_hfp_mode_t mode = p_info->mode;
            if (mode == BT_HFP_MODE_LOOPBACK_WITHOUT_CODEC) {
                LISR_LOG_I("+loopback_without_codec\r\n");
                bt_hfp_read_from_shared_memory (p_mem_info, (uint8_t *)p_tmp_buf);
                bt_hfp_write_to_shared_memory  (p_mem_info, (uint8_t *)p_tmp_buf);
                LISR_LOG_I("-loopback_without_codec\r\n");
                return;
            }
            bt_codec_read_from_rx_buffer (p_mem_info, &p_info->rx_info, p_tmp_buf, mask_array, bt_hw_ctrl);
            bt_codec_write_to_tx_buffer  (p_mem_info, &p_info->tx_info, p_tmp_buf, codec_type);
        }
        {
            bt_hfp_mode_t mode = p_info->mode;
            if (mode == BT_HFP_MODE_LOOPBACK_WITH_CODEC) {
                LISR_LOG_I("+loopback_with_codec\r\n");
                bt_hfp_codec_rx_process_handler(NULL);
                LISR_LOG_I("-loopback_with_codec\r\n");
                return;
            } else {
                bt_codec_event_send_from_isr(BT_CODEC_QUEUE_EVENT_HFP_RX_PROCESS, NULL);
            }
        }
        LISR_LOG_I("-bt interrupt\r\n");
    }
    return;
}

static void bt_hfp_bt_interrupt_callback (void *user_data)
{
#if defined(BT_HFP_CODEC_CHECK_INTERRUPT_INTERVAL)
    print_interrupt_interval (&bt_hfp_rx_cnt, "BT Interrupt");
#endif  /* BT_HFP_CODEC_CHECK_INTERRUPT_INTERVAL */
    {
        bt_hfp_codec_internal_handle_t *p_info = bt_hfp_codec_internal_handle;
        bt_hfp_bt_interrupt_callback_internal(p_info);
        *BT_HFP_HW_REG_CONTROL &= ~BT_HFP_HW_CTRL_MASK;
        p_info->eint_status = hal_eint_unmask(p_info->eint_number);
        if (p_info->eint_status != HAL_EINT_STATUS_OK) {
            LISR_LOG_E(" play, EXCEPTION, unmask eint status %d\r\n", p_info->eint_status);
        }
    }
    return;
}

static void bt_hfp_codec_pcm2way_stream_in_callback (void)
{
    //uint32_t isr_time = bt_codec_get_interrupt_time();    // temp solution for LISR_LOG_W build warring:  unused variable 'isr_time' 
#if defined(BT_HFP_CODEC_CHECK_INTERRUPT_INTERVAL)
    print_interrupt_interval (&p2w_tx_cnt, "P2W Tx");
#endif  /* BT_HFP_CODEC_CHECK_INTERRUPT_INTERVAL */
    if (bt_hfp_check_state(BT_CODEC_STATE_PLAY) == BT_HFP_FALSE) {
        LISR_LOG_W("P2W Tx not PLAY, %d\r\n", bt_codec_get_interrupt_time());
    } else {
        bt_hfp_codec_internal_handle_t *p_info = bt_hfp_codec_internal_handle;
        bt_hfp_speech_tx_information_t *p_tx_info    = &p_info->tx_info;
        ring_buffer_information_t      *p_tx_ring    = &p_tx_info->pcm_ring_info;
        bt_hf_codec_type_t              codec_type   = p_info->codec_type;
        uint32_t                        frm_byte_cnt = codec_type == BT_HF_CODEC_CVSD ? 320 : 640;
        uint16_t                       *p_tmp_base   = p_tx_info->pcm_tx_tmp_buf;
        LISR_LOG_I("+dsp_tx\r\n");
        {   /* Check Tx PCM ring buffer space byte count */
            uint32_t space_byte_cnt = ring_buffer_get_space_byte_count(p_tx_ring);
            if (space_byte_cnt >= frm_byte_cnt) {
                p_tx_info->pcm_overflow = BT_HFP_FALSE;
            } else {    /* Tx PCM ring buffer overflow */
                LISR_LOG_W("DSP Tx OF, %d\r\n", bt_codec_get_interrupt_time());
                p_tx_info->pcm_overflow = BT_HFP_TRUE;
                bt_codec_event_send_from_isr(BT_CODEC_QUEUE_EVENT_DSP_TX_PROCESS, NULL);
                return;
            }
        }
        {   /* Get PCM data from stream in & write to Tx PCM ring buffer */
            uint8_t *p_tmp_buf = (uint8_t *)p_tmp_base;
            uint8_t *p_pcm_buf = NULL;
            uint32_t write_byte_cnt = 0;
            uint32_t loop_idx;
            audio_pcm2way_get_from_stream_in(p_tmp_base);
            for (loop_idx = 0; loop_idx < 2; loop_idx++) {
                ring_buffer_get_write_information(p_tx_ring, &p_pcm_buf, &write_byte_cnt);
                write_byte_cnt = MINIMUM(write_byte_cnt, frm_byte_cnt);
                memcpy(p_pcm_buf, p_tmp_buf, write_byte_cnt);
                p_tmp_buf += write_byte_cnt;
                frm_byte_cnt -= write_byte_cnt;
                ring_buffer_write_done(p_tx_ring, write_byte_cnt);
            }
        }
        LISR_LOG_I("-dsp_tx\r\n");
        bt_codec_event_send_from_isr(BT_CODEC_QUEUE_EVENT_DSP_TX_PROCESS, NULL);
    }
    return;
}

static void bt_hfp_codec_pcm2way_stream_out_callback (void)
{
    //uint32_t isr_time = bt_codec_get_interrupt_time();    // temp solution for LISR_LOG_W build warring:  unused variable 'isr_time' 
#if defined(BT_HFP_CODEC_CHECK_INTERRUPT_INTERVAL)
    print_interrupt_interval (&p2w_rx_cnt, "P2W Rx");
#endif  /* BT_HFP_CODEC_CHECK_INTERRUPT_INTERVAL */
    if (bt_hfp_check_state(BT_CODEC_STATE_PLAY) == BT_HFP_FALSE) {
        LISR_LOG_W("P2W Rx not PLAY, %d\r\n", bt_codec_get_interrupt_time());
    } else {
        bt_hfp_codec_internal_handle_t *p_info       = bt_hfp_codec_internal_handle;
        bt_hfp_speech_rx_information_t *p_rx_info    = &p_info->rx_info;
        ring_buffer_information_t      *p_rx_ring    = &p_rx_info->pcm_ring_info;
        bt_hf_codec_type_t              codec_type   = p_info->codec_type;
        uint32_t                        frm_byte_cnt = codec_type == BT_HF_CODEC_CVSD ? 320 : 640;
        uint16_t                       *p_tmp_base   = p_rx_info->pcm_rx_tmp_buf;
        LISR_LOG_I("+dsp_rx\r\n");
        {   /* Check Rx PCM ring buffer data byte count */
            uint32_t data_byte_cnt = ring_buffer_get_data_byte_count(p_rx_ring);
            if (data_byte_cnt >= frm_byte_cnt) {
                p_rx_info->pcm_underflow = BT_HFP_FALSE;
            } else {    /* Rx PCM ring buffer underflow */
                LISR_LOG_W("DSP Rx UF, %d\r\n", bt_codec_get_interrupt_time());
                p_rx_info->pcm_underflow = BT_HFP_TRUE;
                memset(p_tmp_base, 0, BT_HFP_DSP_FRAME_SIZE);
                audio_pcm2way_put_to_stream_out(p_tmp_base);
                return;
            }
        }
        {   /* Get PCM data from Rx PCM ring buffer & write to stream out */
            uint8_t *p_tmp_buf = (uint8_t *)p_tmp_base;
            uint8_t *p_pcm_buf = NULL;
            uint32_t read_byte_cnt = 0;
            uint32_t loop_idx;
            for (loop_idx = 0; loop_idx < 2; loop_idx++) {
                ring_buffer_get_read_information(p_rx_ring, &p_pcm_buf, &read_byte_cnt);
                read_byte_cnt = MINIMUM(read_byte_cnt, frm_byte_cnt);
                memcpy(p_tmp_buf, p_pcm_buf, read_byte_cnt);
                p_tmp_buf += read_byte_cnt;
                frm_byte_cnt -= read_byte_cnt;
                ring_buffer_read_done(p_rx_ring, read_byte_cnt);
            }
            audio_pcm2way_put_to_stream_out(p_tmp_base);
        }
        LISR_LOG_I("-dsp_rx\r\n");
        bt_codec_event_send_from_isr(BT_CODEC_QUEUE_EVENT_DSP_RX_PROCESS, NULL);
    }
    return;
}

static bt_status_t bt_hfp_play(bt_media_handle_t *handle)
{
#if defined(BT_HFP_CODEC_CHECK_INTERRUPT_INTERVAL)
    initialize_interrupt_interval();
#endif  /* BT_HFP_CODEC_CHECK_INTERRUPT_INTERVAL */
    {
        uint32_t isr_time = bt_codec_get_interrupt_time();
        TASK_LOG_CTRL("+play, %ld\r\n", isr_time);
    }
    {   /* PLAY */
        bt_hfp_codec_internal_handle_t *p_info = bt_hfp_codec_internal_handle;
        bt_hfp_mode_t mode = p_info->mode;
        bt_hf_codec_type_t codec_type = p_info->codec_type;
        /******************[HEAD] Check State *******************************************************/
        if (bt_hfp_check_state(BT_CODEC_STATE_READY) == BT_HFP_FALSE) {
            TASK_LOG_E("play not ready\r\n");
            return BT_STATUS_MEDIA_ERROR;
        }
        /******************[TAIL] Check State *******************************************************/
        /******************[HEAD] Initialize Codec Library ******************************************/
        {
            bt_hfp_speech_tx_information_t *p_tx_info = &p_info->tx_info;
            bt_hfp_speech_rx_information_t *p_rx_info = &p_info->rx_info;
            void **en_handle = &p_tx_info->encoder.handle;
            void **de_handle = &p_rx_info->decoder.handle;
            void **pl_handle = &p_rx_info->plc.handle;
            uint8_t *en_int_buf = (uint8_t *)(*en_handle);
            uint8_t *de_int_buf = (uint8_t *)(*de_handle);
            uint8_t *pl_int_buf = (uint8_t *)(*pl_handle);
            plc_initial_parameter_t param;
            if (codec_type == BT_HF_CODEC_CVSD) {
                void **up_handle = &p_tx_info->src_up_sampling.handle;
                void **dw_handle = &p_rx_info->src_down_sampling.handle;
                uint8_t *up_int_buf = (uint8_t *)(*up_handle);
                uint8_t *dw_int_buf = (uint8_t *)(*dw_handle);
                param.sampling_rate = 8000;
                cvsd_encode_init            (en_handle, en_int_buf);
                cvsd_src_up_sampling_init   (up_handle, up_int_buf);
                cvsd_decode_init            (de_handle, de_int_buf);
                cvsd_src_down_sampling_init (dw_handle, dw_int_buf);
            } else {
                param.sampling_rate = 16000;
                msbc_encode_init (en_handle, en_int_buf);
                msbc_decode_init (de_handle, de_int_buf);
            }
            {
                uint32_t buf_size = p_rx_info->plc.internal_buffer_size;
                memset(pl_int_buf, 0, buf_size);
                plc_init (pl_handle, pl_int_buf, &param);
            }
        }
        /******************[TAIL] Initialize Codec Library ******************************************/
        /******************[HEAD] Hook BT Interrupt Handler *****************************************/
        bt_codec_event_register_callback(BT_CODEC_QUEUE_EVENT_HFP_RX_PROCESS, bt_hfp_codec_rx_process_handler);
        /******************[TAIL] Hook BT Interrupt Handler *****************************************/
        /******************[HEAD] Set PLAY State ****************************************************/
        handle->state = BT_CODEC_STATE_PLAY;
        /******************[TAIL] Set PLAY State ****************************************************/
        /******************[HEAD] Unmask BT Interrupt ***********************************************/
        p_info->eint_status = hal_eint_unmask(p_info->eint_number);
        if (p_info->eint_status != HAL_EINT_STATUS_OK) {
            TASK_LOG_E("play unmask %d\r\n", p_info->eint_status);
            return BT_STATUS_MEDIA_ERROR;
        }
        /******************[TAIL] Unmask BT Interrupt ***********************************************/
        /******************[HEAD] PCM2WAY Start *****************************************************/
        if (mode == BT_HFP_MODE_SPEECH) {
            audio_pcm2way_band_t band;
            audio_pcm2way_format_t format;
            audio_pcm2way_config_t config;
            if (codec_type == BT_HF_CODEC_CVSD) {
                band   = PCM2WAY_NARROWBAND;
                format = PCM2WAY_FORMAT_NORMAL;
            } else {
                band   = PCM2WAY_WIDEBAND;
                format = PCM2WAY_FORMAT_WB_NORMAL;
            }
            config.stream_in_d2m_on                = BT_HFP_TRUE;
            config.stream_in_is_after_enhancement  = BT_HFP_TRUE;
            config.stream_out_m2d_on               = BT_HFP_TRUE;
            config.stream_out_is_after_enhancement = BT_HFP_FALSE;
            config.band                            = band;
            config.format                          = format;
            bt_codec_event_register_callback(BT_CODEC_QUEUE_EVENT_DSP_TX_PROCESS, bt_hfp_dsp_tx_process_handler);
            bt_codec_event_register_callback(BT_CODEC_QUEUE_EVENT_DSP_RX_PROCESS, bt_hfp_dsp_rx_process_handler);
            TASK_LOG_CTRL(" play, p2w\r\n");
            {
                uint16_t device = afe_audio_get_output_device();
                hal_speech_device_mode_t enh_mode;
                if (device == HAL_AUDIO_DEVICE_HEADSET || device == HAL_AUDIO_DEVICE_HEADSET_MONO) {
                    enh_mode = SPH_MODE_BT_EARPHONE;
                } else {
                    enh_mode = SPH_MODE_BT_LOUDSPEAKER;
                }
                speech_set_mode(enh_mode);
            }
            audio_pcm2way_start(bt_hfp_codec_pcm2way_stream_out_callback, bt_hfp_codec_pcm2way_stream_in_callback, &config);
            speech_set_enhancement(true);
        }
        /******************[TAIL] PCM2WAY Start *****************************************************/
    }
    {
        uint32_t isr_time = bt_codec_get_interrupt_time();
        TASK_LOG_CTRL("-play, %ld\r\n", isr_time);
    }
    return BT_STATUS_MEDIA_OK;
}

static bt_status_t bt_hfp_stop(bt_media_handle_t *handle)
{
    {
        uint32_t isr_time = bt_codec_get_interrupt_time();
        TASK_LOG_CTRL("+stop, %ld\r\n", isr_time);
    }
    {   /* STOP */
        bt_hfp_codec_internal_handle_t *p_info = bt_hfp_codec_internal_handle;
        bt_hfp_mode_t mode = p_info->mode;
        /******************[HEAD] Check State *******************************************************/
        if (bt_hfp_check_state(BT_CODEC_STATE_PLAY) == BT_HFP_FALSE) {
            TASK_LOG_E("stop not play\r\n");
            return BT_STATUS_MEDIA_ERROR;
        }
        /******************[TAIL] Check State *******************************************************/
        /******************[HEAD] Set STOP State ****************************************************/
        handle->state = BT_CODEC_STATE_STOP;
        /******************[TAIL] Set STOP State ****************************************************/
        /******************[HEAD] PCM2WAY Stop ******************************************************/
        if (mode == BT_HFP_MODE_SPEECH) {
            speech_set_enhancement(false);
            audio_pcm2way_stop();
            bt_codec_event_deregister_callback(BT_CODEC_QUEUE_EVENT_DSP_TX_PROCESS);
            bt_codec_event_deregister_callback(BT_CODEC_QUEUE_EVENT_DSP_RX_PROCESS);
            TASK_LOG_CTRL(" stop, p2w\r\n");
        }
        /******************[TAIL] PCM2WAY Stop ******************************************************/
        /******************[HEAD] Mask BT Interrupt *************************************************/
        p_info->eint_status = hal_eint_mask(p_info->eint_number);
        if (p_info->eint_status != HAL_EINT_STATUS_OK) {
            TASK_LOG_E("stop mask %d\r\n", p_info->eint_status);
            return BT_STATUS_MEDIA_ERROR;
        }
        /******************[TAIL] Mask BT Interrupt *************************************************/
        /******************[HEAD] Unhook BT Interrupt Handler ***************************************/
        bt_codec_event_deregister_callback(BT_CODEC_QUEUE_EVENT_HFP_RX_PROCESS);
        /******************[TAIL] Unhook BT Interrupt Handler ***************************************/
    }
    {
        uint32_t isr_time = bt_codec_get_interrupt_time();
        TASK_LOG_CTRL("-stop, %ld\r\n", isr_time);
    }
    return BT_STATUS_MEDIA_OK;
}

/* Should not use this API */
static bt_status_t bt_hfp_process(bt_media_handle_t *handle, bt_event_t event)
{
    TASK_LOG_E("process\r\n");
    return BT_STATUS_MEDIA_ERROR;
}

static bt_media_handle_t *bt_hfp_open_codec_internal(bt_hf_codec_type_t codec_type, bt_hfp_mode_t mode)
{
    bt_hfp_codec_internal_handle_t *p_info;
    bt_media_handle_t *handle;
    TASK_LOG_CTRL("+open\r\n");
    {   /* Allocate Internal Handle */
        bt_hfp_speech_tx_information_t *p_tx_info;
        bt_hfp_speech_rx_information_t *p_rx_info;
        uint32_t *pkt_tmp_buf;
        p_info = (bt_hfp_codec_internal_handle_t *)pvPortMalloc(sizeof(bt_hfp_codec_internal_handle_t));
        pkt_tmp_buf = (uint32_t *)pvPortMalloc(BT_HFP_HW_SRAM_SIZE);
        memset(p_info, 0, sizeof(bt_hfp_codec_internal_handle_t));
        handle = &p_info->handle;
        handle->play    = bt_hfp_play;
        handle->stop    = bt_hfp_stop;
        handle->process = bt_hfp_process;
        bt_hfp_codec_internal_handle = p_info;
        p_info->codec_type  = codec_type;
        p_info->mode        = mode;
        p_info->pkt_tmp_buf = pkt_tmp_buf;
        p_tx_info = &p_info->tx_info;
        p_rx_info = &p_info->rx_info;
        bt_hfp_set_shared_memory_information(BTSRAM_BASE, BT_HFP_HW_REG_PACKET_R, BT_HFP_HW_REG_PACKET_W, BT_HFP_HW_REG_CONTROL);
        if (mode == BT_HFP_MODE_SPEECH || mode == BT_HFP_MODE_LOOPBACK_WITH_CODEC) {
            /* Codec Setting */
            if (codec_type == BT_HF_CODEC_CVSD) {
                bt_hfp_cvsd_open(p_tx_info, p_rx_info);
            } else {
                bt_hfp_msbc_open(p_tx_info, p_rx_info);
            }
        }
        {   /* Set ring buffer */
            ring_buffer_information_t *p_tx_ring = &p_tx_info->pcm_ring_info;
            ring_buffer_information_t *p_rx_ring = &p_rx_info->pcm_ring_info;
            p_tx_ring->buffer_base_pointer = p_tx_info->pcm_ring_buf;
            p_rx_ring->buffer_base_pointer = p_rx_info->pcm_ring_buf;
            p_tx_ring->buffer_byte_count = BT_HFP_TX_PCM_RING_BUFFER_SIZE;
            p_rx_ring->buffer_byte_count = BT_HFP_RX_PCM_RING_BUFFER_SIZE;
        }
    }
    {   /* EINT Settings */
        hal_eint_number_t eint_num = HAL_EINT_BTSYS;
        hal_eint_status_t status;
        p_info->eint_number = eint_num;
        {   /* EINT Mask */
            status = hal_eint_mask(eint_num);
            p_info->eint_status = status;
            RETURN_NULL_IF_FALSE(status == HAL_EINT_STATUS_OK);
        }
        {   /* EINT Init */
            hal_eint_config_t *p_config = &p_info->eint_config;
            p_config->trigger_mode = HAL_EINT_LEVEL_LOW;
            p_config->debounce_time = 3;    /* Unit: ms */
            status = hal_eint_init(eint_num, p_config);
            p_info->eint_status = status;
            RETURN_NULL_IF_FALSE(status == HAL_EINT_STATUS_OK);
        }
        {   /* EINT Callback */
            hal_eint_callback_t callback = bt_hfp_bt_interrupt_callback;
            void *user_data = NULL;
            p_info->eint_callback = callback;
            status = hal_eint_register_callback(eint_num, callback, user_data);
            p_info->eint_status = status;
            RETURN_NULL_IF_FALSE(status == HAL_EINT_STATUS_OK);
        }
    }
    handle->state = BT_CODEC_STATE_READY;
    TASK_LOG_CTRL("-open\r\n");
    return handle;
}

static bt_status_t bt_hfp_close_codec_internal(void)
{
    bt_hfp_codec_internal_handle_t *p_info = bt_hfp_codec_internal_handle;
    bt_media_handle_t *handle;
    bt_codec_state_t state;
    bt_hfp_mode_t mode;
    {   /* Basic Settings & Check */
        RETURN_MEDIA_ERROR_IF_FALSE(p_info != NULL);
        handle = &p_info->handle;
        state = handle->state;
        mode = p_info->mode;
        RETURN_MEDIA_ERROR_IF_FALSE(state == BT_CODEC_STATE_STOP);
    }
    TASK_LOG_CTRL("+close\r\n");
    if (mode == BT_HFP_MODE_SPEECH || mode == BT_HFP_MODE_LOOPBACK_WITH_CODEC) {
        /* Codec Settings */
        bt_hf_codec_type_t codec_type = p_info->codec_type;
        bt_hfp_speech_tx_information_t *p_tx_info = &p_info->tx_info;
        bt_hfp_speech_rx_information_t *p_rx_info = &p_info->rx_info;
        RETURN_MEDIA_ERROR_IF_FALSE(codec_type == BT_HF_CODEC_CVSD || codec_type == BT_HF_CODEC_MSBC);
        if (codec_type == BT_HF_CODEC_CVSD) {
            bt_hfp_cvsd_close(p_tx_info, p_rx_info);
        } else {
            bt_hfp_msbc_close(p_tx_info, p_rx_info);
        }
    }
    {   /* EINT Deinit */
        hal_eint_number_t eint_num = p_info->eint_number;
        hal_eint_status_t status = hal_eint_deinit(eint_num);
        p_info->eint_status = status;
        RETURN_MEDIA_ERROR_IF_FALSE(status == HAL_EINT_STATUS_OK);
        p_info->eint_number = HAL_EINT_NUMBER_0;
    }
    handle->state = BT_CODEC_STATE_IDLE;
    vPortFree(p_info->pkt_tmp_buf);
    vPortFree(p_info);
    TASK_LOG_CTRL("-close\r\n");
    return BT_STATUS_MEDIA_OK;
}

void bt_codec_task_main(void *arg)
{
    bt_codec_queue_event_t event;
    bt_codec_queue_handle = xQueueCreate(BT_CODEC_QUEUE_SIZE, sizeof(bt_codec_queue_event_t));
    {   /* Initialize queue registration */
        uint32_t id_idx;
        for (id_idx = 0; id_idx < MAX_BT_CODEC_FUNCTIONS; id_idx++) {
            bt_codec_queue_id_array[id_idx] = BT_CODEC_QUEUE_EVENT_NONE;
        }
    }
    while (1) {
        if (xQueueReceive(bt_codec_queue_handle, &event, portMAX_DELAY)) {
            bt_codec_queue_event_id_t rece_id = event.id;
            uint32_t id_idx;
            for (id_idx = 0; id_idx < MAX_BT_CODEC_FUNCTIONS; id_idx++) {
                if (bt_codec_queue_id_array[id_idx] == rece_id) {
                    bt_codec_queue_handler[id_idx](event.parameter);
                    break;
                }
            }
        }
    }
}

/*******************************************************************************\
| Public Functions                                                              |
\*******************************************************************************/

void bt_codec_task_create(void)
{
    xTaskCreate(bt_codec_task_main, "BT_codec_task", BT_CODEC_TASK_STACK_SIZE, NULL, BT_CODEC_TASK_PRIORITY, NULL);
    return;
}

bt_media_handle_t *bt_codec_hfp_open(bt_codec_hfp_callback_t callback, const bt_codec_hfp_audio_t *param)
{
    bt_hf_codec_type_t codec_type;
    bt_hfp_mode_t mode;
    RETURN_NULL_IF_FALSE(param != NULL);
    codec_type = param->type;
    RETURN_NULL_IF_FALSE(codec_type == BT_HF_CODEC_CVSD || codec_type == BT_HF_CODEC_MSBC);
    mode = BT_HFP_MODE_SPEECH;
    TASK_LOG_CTRL("open_codec\r\n");
    return bt_hfp_open_codec_internal(codec_type, mode);
}

bt_status_t bt_codec_hfp_close (bt_media_handle_t *handle)
{
    TASK_LOG_CTRL("close_codec\r\n");
    return bt_hfp_close_codec_internal();
}

bt_media_handle_t *bt_hf_open_loopback(const bt_hf_loopback_t *param)
{
    bt_hf_codec_type_t codec_type;
    bt_hf_loopback_mode_t loopback_mode;
    bt_hfp_mode_t mode;
    RETURN_NULL_IF_FALSE(param != NULL);
    codec_type = param->type;
    RETURN_NULL_IF_FALSE(codec_type == BT_HF_CODEC_CVSD || codec_type == BT_HF_CODEC_MSBC);
    loopback_mode = param->mode;
    RETURN_NULL_IF_FALSE(loopback_mode == BT_HF_LOOPBACK_WITHOUT_CODEC || loopback_mode == BT_HF_LOOPBACK_WITH_CODEC);
    if (loopback_mode == BT_HF_LOOPBACK_WITHOUT_CODEC) {
        mode = BT_HFP_MODE_LOOPBACK_WITHOUT_CODEC;
    } else {
        mode = BT_HFP_MODE_LOOPBACK_WITH_CODEC;
    }
    TASK_LOG_CTRL("open_loopback\r\n");
    return bt_hfp_open_codec_internal(codec_type, mode);
}

bt_status_t bt_hf_close_loopback(void)
{
    TASK_LOG_CTRL("close_loopback\r\n");
    return bt_hfp_close_codec_internal();
}

void bt_hfp_set_shared_memory_information (uint32_t base_address, volatile uint32_t *p_offset_r, volatile uint32_t *p_offset_w, volatile uint32_t *p_control)
{
    bt_hfp_codec_internal_handle_t *p_info = bt_hfp_codec_internal_handle;
    bt_hfp_shared_memory_information_t *p_mem_info = &p_info->mem_info;
    p_mem_info->base_address = base_address;
    p_mem_info->p_offset_r   = p_offset_r;
    p_mem_info->p_offset_w   = p_offset_w;
    p_mem_info->p_control    = p_control;
    return;
}

void bt_hfp_ut_process (void)
{
    bt_hfp_codec_internal_handle_t *p_info = bt_hfp_codec_internal_handle;
    RETURN_IF_FALSE(p_info != NULL);
    {   /* Exchange bitstream with BT */
        bt_hfp_shared_memory_information_t *p_mem_info = &p_info->mem_info;
        volatile uint32_t *p_control = p_mem_info->p_control;
        uint32_t bt_hw_ctrl  = *p_control;
        uint32_t pkt_type    = (bt_hw_ctrl >> 18) & 0x7;
        uint32_t type_idx    = bt_hfp_get_packet_type_index(pkt_type);
        uint32_t *mask_array = (uint32_t *)&bt_hfp_rx_packet_valid_mask_table[type_idx];
        uint32_t *p_tmp_buf  = p_info->pkt_tmp_buf;
        bt_hf_codec_type_t codec_type = p_info->codec_type;
        bt_hfp_mode_t mode = p_info->mode;
        if (mode == BT_HFP_MODE_LOOPBACK_WITHOUT_CODEC) {
            bt_hfp_read_from_shared_memory (p_mem_info, (uint8_t *)p_tmp_buf);
            bt_hfp_write_to_shared_memory  (p_mem_info, (uint8_t *)p_tmp_buf);
            *p_control &= ~BT_HFP_HW_CTRL_MASK;
            return;
        }
        bt_codec_read_from_rx_buffer (p_mem_info, &p_info->rx_info, p_tmp_buf, mask_array, bt_hw_ctrl);
        bt_codec_write_to_tx_buffer  (p_mem_info, &p_info->tx_info, p_tmp_buf, codec_type);
        *p_control &= ~BT_HFP_HW_CTRL_MASK;
        bt_hfp_codec_rx_process_handler(NULL);
    }
    return;
}
