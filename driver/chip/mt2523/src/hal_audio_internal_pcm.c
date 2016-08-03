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

#include "hal_audio.h"

#ifdef HAL_AUDIO_MODULE_ENABLED

#include "hal_audio_internal_pcm.h"
#include "hal_audio_enhancement.h"
#include "hal_audio_internal_afe.h"
#include "hal_audio_fw_sherif.h"
#include "hal_audio_fw_interface.h"
#include "hal_audio_internal_pcm2way.h"
#ifdef HAL_AUDIO_LOW_POWER_ENABLED
#include "hal_audio_low_power.h"
#endif

pcm_playback_internal_t pcm_internal_handle;
extern hal_audio_stream_t audio_config;
uint16_t pcm_nb_stream_in_buffer [NB_BUFFER_SIZE];
uint16_t pcm_wb_stream_in_buffer [WB_BUFFER_SIZE];
uint16_t pcm_nb_stream_in_buffer_sec_mic [NB_BUFFER_SIZE];
uint16_t pcm_wb_stream_in_buffer_sec_mic [WB_BUFFER_SIZE];


hal_audio_status_t audio_pcm_set_memory(uint16_t *memory)
{
    uint32_t offset = 0;
    uint16_t *p_buffer;
    p_buffer = memory;

    if (p_buffer == NULL) {
        return HAL_AUDIO_STATUS_INVALID_PARAMETER;
    }
    /*address should be 4 bytes aligned*/
    if ((((uint32_t)p_buffer) & 0x3) != 0 ) {
        return HAL_AUDIO_STATUS_INVALID_PARAMETER;
    }

    /*assign memory pool memory*/
    /*internal buffer*/
    {
        ring_buffer_information_t *p_mcu_info = &pcm_internal_handle.mcu_buffer_info;
        p_mcu_info->read_pointer        = 0;
        p_mcu_info->write_pointer       = 0;
        p_mcu_info->buffer_byte_count   = PCM_RING_BUFFER_SIZE;
        p_mcu_info->buffer_base_pointer = (uint8_t *)p_buffer;
    }
    offset += PCM_RING_BUFFER_SIZE;
    return HAL_AUDIO_STATUS_OK;
}

uint32_t audio_pcm_get_memory_size(void)
{
    return (PCM_RING_BUFFER_SIZE);
}

void audio_pcm_register_stream_out_callback(hal_audio_stream_out_callback_t callback, void *user_data)
{
    pcm_internal_handle.out_callback = callback;
    pcm_internal_handle.user_data = user_data;
}

void audio_pcm_register_stream_in_callback(hal_audio_stream_in_callback_t callback, void *user_data)
{
    pcm_internal_handle.in_callback = callback;
    pcm_internal_handle.user_data = user_data;
}
static void audio_pcm_set_state(pcm_state_t state)
{
    pcm_internal_handle.state = state;
}

/*MCU buffer operations*/
void audio_pcm_get_stream_out_free_space(uint32_t *sample_count) //[in bytes]
{
    *sample_count = ring_buffer_get_space_byte_count(&pcm_internal_handle.mcu_buffer_info);
}

hal_audio_status_t audio_pcm_stream_out_write(const void *buffer, uint32_t size) //[size: bytes]
{
    ring_buffer_information_t *p_mcu_info = &pcm_internal_handle.mcu_buffer_info;
    hal_audio_status_t result;
    uint32_t space_byte_count = ring_buffer_get_space_byte_count(p_mcu_info);
    if (space_byte_count >= size) {
        uint8_t *p_dst = NULL;
        uint8_t *p_src = (uint8_t *)buffer;
        uint32_t data_byte_count = size;
        uint32_t write_byte_count = 0;
        uint32_t loop_idx;
        for (loop_idx = 0; loop_idx < 2; loop_idx++) {
            ring_buffer_get_write_information (p_mcu_info, &p_dst, &write_byte_count);
            write_byte_count = MINIMUM(write_byte_count, data_byte_count);
            memcpy(p_dst, p_src, write_byte_count);
            ring_buffer_write_done (p_mcu_info, write_byte_count);
            data_byte_count -= write_byte_count;
            p_src += write_byte_count;
        }
        pcm_internal_handle.out_waiting = false;
        pcm_internal_handle.underflow = false;
        result = HAL_AUDIO_STATUS_OK;
    } else {
        result = HAL_AUDIO_STATUS_ERROR;
    }
    return result;
}

hal_audio_status_t audio_pcm_stream_in_read(const void *buffer, uint32_t size) //[size: bytes]
{
    hal_audio_status_t result;
    ring_buffer_information_t *p_mcu_info = &pcm_internal_handle.mcu_buffer_info;
    uint32_t read_byte_count = 0;
    uint8_t *p_src = NULL;
    uint8_t *p_dst = (uint8_t *)buffer;
    uint32_t data_byte_count = size;
    for (uint32_t loop_idx = 0; loop_idx < 2; loop_idx++) {
        ring_buffer_get_read_information(p_mcu_info, (uint8_t **)&p_src, &read_byte_count);
        read_byte_count = MINIMUM(read_byte_count, data_byte_count);
        memcpy(p_dst, p_src, read_byte_count);
        ring_buffer_read_done (p_mcu_info, read_byte_count);
        data_byte_count -= read_byte_count;
        p_dst += read_byte_count;
    }
    pcm_internal_handle.in_waiting = false;
    result = HAL_AUDIO_STATUS_OK;
    return result;
}

static void audio_pcm_set_interrupt_sample(uint16_t sample)
{
    *DSP_AUDIO_DEL_CNTR_THRESHOLD = sample;
}

static void audio_pcm_playback_init_dsp(void)
{
    if (pcm_internal_handle.state == PCM_STATE_INIT) {
        ring_buffer_information_t *p_dsp_info = &pcm_internal_handle.dsp_buffer_info;
        p_dsp_info->read_pointer        = 0;
        p_dsp_info->write_pointer       = 0;
        p_dsp_info->buffer_byte_count   = ASP_FRAMELEN_PCM << 1;
        p_dsp_info->buffer_base_pointer = (uint8_t *)DSP_DM_ADDR(PCM_PLAYBACK_DSP_PAGE_NUM, ASP_DSP_PCM_BUFFER_BASE_PCM);
        *DSP_ASP_WAV_W_IO_ADDR = ASP_DSP_PCM_BUFFER_BASE_PCM;
        *DSP_ASP_WAV_R_IO_ADDR = ASP_DSP_PCM_BUFFER_BASE_PCM;
        pcm_internal_handle.state = PCM_STATE_RUNNING;
    }
}

static void audio_pcm_set_interrupt_time(uint32_t msec)
{
    uint32_t sample = (uint32_t)pcm_internal_handle.sample_rate * pcm_internal_handle.channel * msec / 1000;
    audio_pcm_set_interrupt_sample((uint16_t)sample);
}

static uint32_t audio_pcm_dsp_get_data_count(void)
{
    uint32_t current_read, current_write, data_count;
    current_read  = (uint32_t) * DSP_ASP_WAV_R_IO_ADDR;
    current_write = (uint32_t) * DSP_ASP_WAV_W_IO_ADDR;
    if (current_write >= current_read) {
        data_count = current_write - current_read;
    } else {
        data_count = ASP_FRAMELEN_PCM + current_write - current_read;
    }
    return data_count;
}

static void audio_pcm_dsp_get_write_buffer(uint16_t *write, uint32_t *length)
{
    uint32_t current_read, current_write, dsp_write_length;
    current_read  = (uint32_t) * DSP_ASP_WAV_R_IO_ADDR;
    current_write = (uint32_t) * DSP_ASP_WAV_W_IO_ADDR;
    if (current_read > current_write) {
        dsp_write_length = current_read - current_write - 1;
    } else {
        dsp_write_length = ASP_DSP_PCM_BUFFER_BASE_PCM + ASP_FRAMELEN_PCM - current_write;
        if (current_read == ASP_DSP_PCM_BUFFER_BASE_PCM) {
            dsp_write_length--;
        }
    }
    *length = dsp_write_length & ~0x1; /* For L / R pair */
    *write = (uint16_t)current_write;
}

static void audio_pcm_dsp_write_data_done(uint32_t length)
{
    uint16_t current_write = *DSP_ASP_WAV_W_IO_ADDR;
    current_write += length;
    if (current_write >= ASP_DSP_PCM_BUFFER_BASE_PCM + ASP_FRAMELEN_PCM) {
        current_write = ASP_DSP_PCM_BUFFER_BASE_PCM;
    }
    *DSP_ASP_WAV_W_IO_ADDR = current_write;
}

static uint32_t audio_pcm_write_data_to_dsp(uint16_t *source, uint32_t source_word, bool mono_to_stereo)
{
    volatile uint16_t *dsp_addptr;
    uint32_t dsp_word = 0, use_word = 0;
    uint16_t dsp_write_position = 0;
    if (source_word > 0) {
        audio_pcm_dsp_get_write_buffer(&dsp_write_position, &dsp_word);
        dsp_addptr = DSP_DM_ADDR(PCM_PLAYBACK_DSP_PAGE_NUM, dsp_write_position);
        if (mono_to_stereo) {
            dsp_word = MINIMUM(dsp_word, source_word << 1);
        } else {
            dsp_word = MINIMUM(dsp_word, source_word);
        }
        if (dsp_word > 0) {
            if (mono_to_stereo) {
                audio_idma_write_to_dsp_duplicate(dsp_addptr, source, dsp_word);
                use_word = dsp_word >> 1;
            } else {
                audio_idma_write_to_dsp(dsp_addptr, source, dsp_word);
                use_word = dsp_word;
            }
            audio_pcm_dsp_write_data_done(dsp_word);
        }
    }
    return use_word;
}

#if defined(MTK_AUDIO_IIR_FILTER_ENABLE)
static uint32_t audio_pcm_sd_write_data_to_dsp(uint16_t *source, uint32_t source_word, bool mono_to_stereo)
{
    volatile uint16_t *p_dsp_buf = NULL;
    ring_buffer_information_t *p_dsp_info = &pcm_internal_handle.dsp_buffer_info;
    uint32_t read_byte_cnt  = source_word << 1;
    uint32_t write_byte_cnt = 0;
    uint32_t move_byte_cnt;
    uint32_t move_word_cnt;
    p_dsp_info->read_pointer = *DSP_PCM_SD_PLAYBACK_DM_DSP_R_PTR;
    ring_buffer_get_write_information(p_dsp_info, (uint8_t **)&p_dsp_buf, &write_byte_cnt);
    write_byte_cnt &= ~0x1;     /* Make it even */
    if (mono_to_stereo) {
        read_byte_cnt <<= 1;
    }
    move_byte_cnt = MINIMUM(write_byte_cnt, read_byte_cnt);
    {   /* Move data */
        move_word_cnt = move_byte_cnt >> 1;
        if (move_word_cnt > 0) {
            if (mono_to_stereo) {
                move_word_cnt >>= 1;
                audio_idma_write_to_dsp_duplicate(p_dsp_buf, source, move_word_cnt);
            } else {
                audio_idma_write_to_dsp(p_dsp_buf, source, move_word_cnt);
            }
        }
    }
    ring_buffer_write_done (p_dsp_info, move_byte_cnt);
    *DSP_PCM_SD_PLAYBACK_DM_MCU_W_PTR = p_dsp_info->write_pointer;
    return move_word_cnt;
}
#endif

void audio_pcm_put_audio_data(bool mono_to_stereo)
{
    ring_buffer_information_t *p_mcu_info = &pcm_internal_handle.mcu_buffer_info;
    uint16_t *p_src;
    uint32_t move_word_cnt = 0, read_byte_count = 0, dsp_use_word = 0;
    uint32_t consumed_byte_count;
    uint32_t loop_idx;
    uint32_t loop_cnt = 4;
    for (loop_idx = 0; loop_idx < loop_cnt; loop_idx++) {
        ring_buffer_get_read_information(p_mcu_info, (uint8_t **)&p_src, &read_byte_count);
        read_byte_count &= ~0x1;     //make it even
        move_word_cnt = read_byte_count >> 1;
        if (move_word_cnt > 0) {
#if defined(MTK_AUDIO_IIR_FILTER_ENABLE)
            if (pcm_internal_handle.is_sd_task) {
                dsp_use_word = audio_pcm_sd_write_data_to_dsp(p_src, move_word_cnt, mono_to_stereo);
            } else {
                dsp_use_word = audio_pcm_write_data_to_dsp(p_src, move_word_cnt, mono_to_stereo);
            }
#else
            dsp_use_word = audio_pcm_write_data_to_dsp(p_src, move_word_cnt, mono_to_stereo);
#endif
            consumed_byte_count = dsp_use_word << 1;
            ring_buffer_read_done(p_mcu_info, consumed_byte_count);
        } else {    // Read buffer empty
            break;
        }
    }
}

#if defined(MTK_AUDIO_IIR_FILTER_ENABLE)
void audio_pcm_sd_playback_hisr (void)
{
    *DSP_TASK4_COSIM_HANDSHAKE = 0;
    if (pcm_internal_handle.state == PCM_STATE_RUNNING) {
        audio_pcm_put_audio_data(pcm_internal_handle.channel == 1);
        if (!pcm_internal_handle.out_waiting) {
            pcm_internal_handle.out_waiting = true;
            pcm_internal_handle.out_callback(HAL_AUDIO_EVENT_DATA_REQUEST, pcm_internal_handle.user_data);
        }
    }
    return;
}
#endif

void audio_pcm_playback_hisr(void)
{
    uint32_t buffer_time;
    bool is_dsp_underflow = false;
#ifdef HAL_AUDIO_LOW_POWER_ENABLED
    audio_lowpower_set_26mclock(false);
#endif
    *DSP_TASK4_COSIM_HANDSHAKE = 0;
    if (pcm_internal_handle.state != PCM_STATE_RUNNING && pcm_internal_handle.state != PCM_STATE_FLUSHED) {
#ifdef HAL_AUDIO_LOW_POWER_ENABLED
        audio_lowpower_set_26mclock(true);
#endif
        return;
    }
    if (pcm_internal_handle.state == PCM_STATE_RUNNING) {
        audio_pcm_put_audio_data(pcm_internal_handle.channel == 1);
        if (!pcm_internal_handle.out_waiting) {
            pcm_internal_handle.out_waiting = true;
            if (is_dsp_underflow) {
                pcm_internal_handle.underflow = true;
                pcm_internal_handle.out_callback(HAL_AUDIO_EVENT_UNDERFLOW, pcm_internal_handle.user_data);
            } else {
                pcm_internal_handle.out_callback(HAL_AUDIO_EVENT_DATA_REQUEST, pcm_internal_handle.user_data);
            }
        }
    }
    buffer_time = audio_pcm_dsp_get_data_count() * 1000 / (pcm_internal_handle.sample_rate << 1);
    if (buffer_time < (PCM_SOUND_INTERRUPT_PERIOD + 20)) {
        audio_pcm_set_interrupt_time(PCM_SOUND_INTERRUPT_PERIOD);
    } else {
        audio_pcm_set_interrupt_time(buffer_time - PCM_SOUND_INTERRUPT_PERIOD);
    }
#ifdef HAL_AUDIO_LOW_POWER_ENABLED
    audio_lowpower_set_26mclock(true);
#endif
}

#if defined(MTK_AUDIO_IIR_FILTER_ENABLE)
void audio_pcm_set_dsp_sd_task_playback(uint16_t frame_length)
{
    ring_buffer_information_t *p_dsp_info = &pcm_internal_handle.dsp_buffer_info;
    p_dsp_info->read_pointer        = 0;
    p_dsp_info->write_pointer       = 0;
    p_dsp_info->buffer_byte_count   = frame_length << 2;
    p_dsp_info->buffer_base_pointer = (uint8_t *)DSP_DM_ADDR(PCM_SD_PLAYBACK_DSP_PAGE, PCM_SD_PLAYBACK_DSP_ADDR);
    {   /* Clear buffer */
        volatile uint16_t *p_buf = (volatile uint16_t *)p_dsp_info->buffer_base_pointer;
        int32_t frm_len;
        for (frm_len = 0; frm_len < (frame_length << 1); frm_len++) {
            *p_buf++ = 0;
        }
    }
    ring_buffer_write_done (p_dsp_info, frame_length << 2);
    *DSP_PCM_SD_PLAYBACK_DM_MCU_W_PTR = p_dsp_info->write_pointer;
    *DSP_PCM_SD_PLAYBACK_CTRL = PCM_SD_PLAYBACK_STATE_START;
    *DSP_AUDIO_CTRL2 |= DSP_PCM_R_DIS;
    *DSP_AUDIO_FLEXI_CTRL |= (FLEXI_VBI_ENABLE | FLEXI_SD_ENABLE);
    pcm_internal_handle.is_sd_task = 1;
    return;
}
#endif

hal_audio_status_t audio_pcm_play_open_device(void)
{
    if (pcm_internal_handle.state != PCM_STATE_RUNNING) {
#if defined(MTK_AUDIO_IIR_FILTER_ENABLE)
        pcm_internal_handle.is_sd_task = 0;
#endif
        pcm_internal_handle.state = PCM_STATE_INIT;
        pcm_internal_handle.audio_id = audio_get_id();
        pcm_internal_handle.channel = audio_transfer_channel_number(audio_config.stream_out.stream_channel);
        pcm_internal_handle.sample_rate = audio_transfer_sample_rate(audio_config.stream_out.stream_sampling_rate);
        if (pcm_internal_handle.audio_id > MAX_AUDIO_FUNCTIONS) {
            return HAL_AUDIO_STATUS_ERROR;
        }
        audio_service_hook_isr(DSP_D2M_PCM_INT, (isr)audio_pcm_playback_hisr, NULL);
#if defined(MTK_AUDIO_IIR_FILTER_ENABLE)
        audio_service_hook_isr(DSP_D2C_PCM_SD_PLAYBACK_INT, (isr)audio_pcm_sd_playback_hisr, NULL);
#endif
        audio_service_setflag(pcm_internal_handle.audio_id);
#ifdef HAL_AUDIO_LOW_POWER_ENABLED
        if (audio_lowpower_check_status()) {
            audio_lowpower_set_clock_mode(true);
            audio_lowpower_sw_trigger_event_set(true);
            audio_lowpower_set_26mclock(true);
        }
#endif
        audio_clear_dsp_common_flag();
        dsp_audio_fw_dynamic_download(DDID_AUDIO_HEADER);
        audio_pcm_playback_init_dsp();
        audio_pcm_set_interrupt_sample(0);
        audio_playback_on(ASP_TYPE_PCM_HI, audio_config.stream_out.stream_sampling_rate);
    } else {
        return HAL_AUDIO_STATUS_ERROR;
    }
    return HAL_AUDIO_STATUS_OK;
}

void audio_pcm_play_close_device(void)
{
    audio_pcm_set_state(PCM_STATE_STOP);
    audio_playback_off();
    audio_service_unhook_isr(DSP_D2M_PCM_INT);
#if defined(MTK_AUDIO_IIR_FILTER_ENABLE)
    audio_service_unhook_isr(DSP_D2C_PCM_SD_PLAYBACK_INT);
#endif
    audio_service_clearflag(pcm_internal_handle.audio_id);
#ifdef HAL_AUDIO_LOW_POWER_ENABLED
    if (audio_lowpower_check_status()) {
        audio_lowpower_set_26mclock(false);
        audio_lowpower_sw_trigger_event_set(false);
        audio_lowpower_set_clock_mode(false);
    }
#endif
    pcm_internal_handle.state = PCM_STATE_IDLE;
    audio_free_id(pcm_internal_handle.audio_id);
}

bool pcm_record_is_record_silence( void )
{
    if (pcm_internal_handle.presilence > RECORD_PRESILENCE_NUM) {
        return false;
    }
    pcm_internal_handle.presilence++;
    return true;
}

void audio_pcm_get_stream_in_record_size(uint32_t *sample_count) //[in bytes]
{
    *sample_count = ring_buffer_get_data_byte_count(&pcm_internal_handle.mcu_buffer_info);
}

void audio_pcm_record_mediadata_notification()
{

    if (!pcm_internal_handle.in_waiting) {
        pcm_internal_handle.in_waiting = true;
        pcm_internal_handle.in_callback(HAL_AUDIO_EVENT_DATA_NOTIFICATION, pcm_internal_handle.user_data);
    }
}

void audio_pcm_record_hisr()
{
    ring_buffer_information_t *p_mcu_info = &pcm_internal_handle.mcu_buffer_info;
    int32_t data_byte_count;
    uint8_t *p_dst = NULL;
    uint32_t write_byte_count = 0;
    if (audio_config.stream_in.stream_sampling_rate == HAL_AUDIO_SAMPLING_RATE_16KHZ) {
        data_byte_count = WB_BUFFER_SIZE * 2;//WB_BUFFER_SIZE * sizeof(int16_t)
    } else {
        data_byte_count = NB_BUFFER_SIZE * 2;//NB_BUFFER_SIZE * sizeof(int16_t)
    }

    if (!pcm_record_is_record_silence()) {

        {
            uint16_t *ptr_in_buffer_ch1;
            uint16_t *ptr_in_buffer_ch2;
            if (audio_config.stream_in.stream_sampling_rate == HAL_AUDIO_SAMPLING_RATE_16KHZ) {
                ptr_in_buffer_ch1 = pcm_wb_stream_in_buffer;
            } else {
                ptr_in_buffer_ch1 = pcm_nb_stream_in_buffer;
            }
            volatile uint16_t *dspAddr_ch1 = DSP_DM_ADDR(5, *(DSP_DM_ADDR(5, *DSP_SPH_DM_ADDR_SE_PTR)));
            uint32_t I = data_byte_count >> 1;
            while (I-- > 0) {
                *ptr_in_buffer_ch1++ = *dspAddr_ch1++;
            }
            if (audio_config.stream_in.audio_device == HAL_AUDIO_DEVICE_DUAL_DIGITAL_MIC) {
                if (audio_config.stream_in.stream_sampling_rate == HAL_AUDIO_SAMPLING_RATE_16KHZ) {
                    ptr_in_buffer_ch2 = pcm_wb_stream_in_buffer_sec_mic;
                } else {
                    ptr_in_buffer_ch2 = pcm_nb_stream_in_buffer_sec_mic;
                }
                volatile uint16_t *dspAddr_ch2 = DSP_DM_ADDR(5, *DSP_DM_ADDR(5, DSP_SPH_SE2_PTR));
                //volatile uint16_t *dspAddr_ch2 = DSP_DM_ADDR(5, *(volatile uint16_t *)(0x82256642));
                uint32_t I = data_byte_count >> 1;
                while (I-- > 0) {
                    *ptr_in_buffer_ch2++ = *dspAddr_ch2++;
                }
            }
        }
        {
            uint8_t *ptr_in_buffer_ch1;
            uint8_t *ptr_in_buffer_ch2;
            if (audio_config.stream_in.stream_sampling_rate == HAL_AUDIO_SAMPLING_RATE_16KHZ) {
                ptr_in_buffer_ch1 = (uint8_t *)pcm_wb_stream_in_buffer;
            } else {
                ptr_in_buffer_ch1 = (uint8_t *)pcm_nb_stream_in_buffer;
            }
            if (audio_config.stream_in.stream_sampling_rate == HAL_AUDIO_SAMPLING_RATE_16KHZ) {
                ptr_in_buffer_ch2 = (uint8_t *)pcm_wb_stream_in_buffer_sec_mic;
            } else {
                ptr_in_buffer_ch2 = (uint8_t *)pcm_nb_stream_in_buffer_sec_mic;
            }
            if (audio_config.stream_in.audio_device == HAL_AUDIO_DEVICE_SINGLE_DIGITAL_MIC || audio_config.stream_in.audio_device == HAL_AUDIO_DEVICE_MAIN_MIC) {
                for (uint32_t loop_idx = 0; loop_idx < 2; loop_idx++) {
                    ring_buffer_get_write_information (p_mcu_info, &p_dst, &write_byte_count);
                    write_byte_count = MINIMUM(write_byte_count, data_byte_count);
                    memcpy(p_dst, ptr_in_buffer_ch1, write_byte_count);
                    ring_buffer_write_done (p_mcu_info, write_byte_count);
                    data_byte_count -= write_byte_count;
                    ptr_in_buffer_ch1 += write_byte_count;
                }
            } else {
                data_byte_count = data_byte_count * 2;
                for (uint32_t loop_idx = 0; loop_idx < 2; loop_idx++) {
                    ring_buffer_get_write_information (p_mcu_info, &p_dst, &write_byte_count);
                    write_byte_count = MINIMUM(write_byte_count, data_byte_count);
                    for (int i = 0; i < write_byte_count / 2 ; i++) {
                        if (i % 2 == 0) {
                            p_dst[i * 2] = ptr_in_buffer_ch1[i];
                            p_dst[i * 2 + 1] = ptr_in_buffer_ch1[i + 1];
                        } else {
                            p_dst[i * 2] = ptr_in_buffer_ch2[i - 1];
                            p_dst[i * 2 + 1] = ptr_in_buffer_ch2[i];
                        }
                    }
                    ring_buffer_write_done (p_mcu_info, write_byte_count);
                    data_byte_count -= write_byte_count;
                    ptr_in_buffer_ch1 += write_byte_count / 2;
                    ptr_in_buffer_ch2 += write_byte_count / 2;
                }
            }
        }
    }
    uint32_t sample_count = ring_buffer_get_data_byte_count(p_mcu_info);
    if (sample_count > PCM_RECORD_RINGBUFFER_DEFAULT_THRESHOLD) {
        audio_pcm_record_mediadata_notification();
    }
}

void audio_pcm_record_on()
{
    if (audio_config.stream_in.stream_sampling_rate == HAL_AUDIO_SAMPLING_RATE_16KHZ) {
        *DSP_SPH_SCH_IMPROVE_CTRL |= 0x0030; //fix width band SCH-BAND
        *DSP_SPH_SFE_CTRL = 0x0;
        *DSP_SPH_8K_CTRL = 0x8008; //COD_BAND
        *AFE_VMCU_CON1 &= ~0x0200;
        *DSP_SPH_FLT_COEF_ADDR_SRC = 1;
        *DSP_SPH_MUTE_CTRL |= 0x0002;
        *DSP_SPH_COD_CTRL = 0x0D00;
        audio_dsp_speech_set_delay(1);
    } else {
        if (audio_config.stream_in.audio_device == HAL_AUDIO_DEVICE_SINGLE_DIGITAL_MIC || audio_config.stream_in.audio_device == HAL_AUDIO_DEVICE_MAIN_MIC) {
            *DSP_SPH_PCM_REC_CTRL |= 0x0001;
            *DSP_SPH_SCH_IMPROVE_CTRL |= 0x0030; //fix width band SCH-BAND
            *DSP_SPH_SFE_CTRL = 0x0;
            *DSP_SPH_8K_CTRL |= 0x8000; //COD_BAND
            *AFE_VMCU_CON1 &= ~0x0200;
            *DSP_SPH_MUTE_CTRL |= 0x0002;
            *DSP_SC_MODE =  0x0000;
            *DSP_SPH_COD_CTRL |= 0x0022;
            audio_dsp_speech_set_delay(0);
        } else {
            *DSP_SPH_PCM_REC_CTRL |= 0x0001;
            *DSP_SPH_SFE_CTRL = 0x2;
            *AFE_VMCU_CON1 &= ~0x0200;
            *DSP_SPH_MUTE_CTRL |= 0x0002;
            *DSP_SC_MODE =  0x0000;
            *DSP_SPH_COD_CTRL |= 0x0022;
            audio_dsp_speech_set_delay(0);
        }
    }
    afe_voice_digital_on();
    VBI_RESET();
}

void audio_pcm_record_off()
{
    if (audio_config.stream_in.stream_sampling_rate == HAL_AUDIO_SAMPLING_RATE_16KHZ) {
        *DSP_SPH_MUTE_CTRL &= ~0x0002;
        *DSP_SPH_COD_CTRL = 0x0000;
    } else {
        *DSP_SPH_PCM_REC_CTRL &= ~0x0001;
        *DSP_SPH_COD_CTRL &= ~0x0022;
        *DSP_SPH_MUTE_CTRL &= ~0x0002;
        *DSP_SPH_FLT_COEF_ADDR_SRC = 0;
    }
    VBI_END();
    afe_audio_analog_device_off();
    afe_voice_digital_off();
}

void audio_pcm_record_handle(bool enable)
{
    if (enable) {
        pcm_internal_handle.audio_id = audio_get_id();
        audio_service_setflag(pcm_internal_handle.audio_id);
        pcm_internal_handle.presilence = 0;
        if (audio_config.stream_in.stream_sampling_rate == HAL_AUDIO_SAMPLING_RATE_16KHZ) {
            audio_service_hook_isr(DSP_VM_REC_INT, (isr)audio_pcm_record_hisr, NULL);
        } else {
            audio_service_hook_isr(DSP_PCM_REC_INT, (isr)audio_pcm_record_hisr, NULL);
        }
        audio_pcm_record_on();
        if (audio_config.stream_in.audio_device == HAL_AUDIO_DEVICE_SINGLE_DIGITAL_MIC || audio_config.stream_in.audio_device == HAL_AUDIO_DEVICE_MAIN_MIC || audio_config.stream_in.audio_device == HAL_AUDIO_DEVICE_HEADSET_MIC) {
            speech_set_mode(SPH_MODE_RECORD);
            speech_set_enhancement(true);
        }
    } else {
        audio_free_id(pcm_internal_handle.audio_id);
        if (audio_config.stream_in.stream_sampling_rate == HAL_AUDIO_SAMPLING_RATE_16KHZ) {
            audio_service_unhook_isr(DSP_VM_REC_INT);
        } else {
            audio_service_unhook_isr(DSP_PCM_REC_INT);
        }
        if (audio_config.stream_in.audio_device == HAL_AUDIO_DEVICE_SINGLE_DIGITAL_MIC || audio_config.stream_in.audio_device == HAL_AUDIO_DEVICE_MAIN_MIC || audio_config.stream_in.audio_device == HAL_AUDIO_DEVICE_HEADSET_MIC) {
            speech_set_enhancement(false);
        }
        audio_pcm_record_off();
        audio_service_clearflag(pcm_internal_handle.audio_id);
    }
}

#endif /*HAL_AUDIO_MODULE_ENABLED*/
