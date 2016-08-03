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

#include "hal_audio_test.h"

#if defined(__GNUC__)
#if defined(HAL_I2S_MODULE_ENABLED) || defined(HAL_AUDIO_MODULE_ENABLED)

#if defined(HAL_AUDIO_SDFATFS_ENABLE)
FATFS fatfs;
FIL fdst;
FRESULT res;
UINT temp_count, length_written, length_read;
bool sd_repeat_play = true;
#endif

#ifdef HAL_ACCDET_MODULE_ENABLED
uint8_t earphone_plug_in;
static hal_accdet_debounce_time_t debounce_time[2] = {
    {HOOK_KEY_DEBOUNCE_TIME, PLUG_IN_DEBOUNCE_TIME, PLUG_OUT_DEBOUNCE_TIME}, //before plug in
    {HOOK_KEY_DEBOUNCE_TIME, PLUG_IN_DEBOUNCE_TIME >> 2, PLUG_OUT_DEBOUNCE_TIME} //after plug in
};
#endif

uint8_t audio_test_result = 1;
bool audio_tone = false; //audio:1 voice:0

static uint16_t audio_tone1k_16ksr[32] = {
    0x0000, 0xe475, 0xcd1a, 0xbd80, 0xb806, 0xbd80, 0xcd1b, 0xe475,
    0x0000, 0x1b8c, 0x32e5, 0x4281, 0x47fa, 0x4280, 0x32e6, 0x1b8b,
    0x0000, 0xe475, 0xcd1b, 0xbd81, 0xb806, 0xbd80, 0xcd1b, 0xe474,
    0x0001, 0x1b8c, 0x32e5, 0x4280, 0x47fb, 0x427f, 0x32e5, 0x1b8c
};

static uint16_t audio_tone1k_8ksr[8] = {
    0xfff4, 0x5a7a, 0x7fff, 0x5a8a, 0x000d, 0xa587, 0x8001, 0xa575
};
static uint16_t audio_tone2k_16ksr[32] = {
    0xffff, 0xcd1a, 0xb805, 0xcd1b, 0x0000, 0x32e6, 0x47fb, 0x32e5,
    0x0000, 0xcd1a, 0xb805, 0xcd1b, 0x0000, 0x32e5, 0x47fb, 0x32e5,
    0x0000, 0xcd1b, 0xb805, 0xcd1b, 0x0000, 0x32e5, 0x47fb, 0x32e5,
    0x0000, 0xcd1b, 0xb806, 0xcd1a, 0xffff, 0x32e5, 0x47f9, 0x32e6
};

hal_audio_device_t out_device = HAL_AUDIO_DEVICE_HEADSET;
hal_audio_device_t in_device  = HAL_AUDIO_DEVICE_MAIN_MIC;

uint16_t stream_out_buffer[160];
uint16_t stream_in_buffer [160];
#define USR_BUF_SIZE  PCM_RING_BUFFER_SIZE
#define INT_BUF_SIZE  PCM_RING_BUFFER_SIZE>>1
uint16_t user_buffer[USR_BUF_SIZE];
uint16_t internal_buffer[INT_BUF_SIZE];
uint32_t user_buf_read_ptr = 0;
uint16_t cal_buffer[256]; // for put data for FFT
uint32_t sd_count           = 0;
uint32_t record_test_isr_count           = 0;
bool ul_enable = false;
int32_t max_value;
int32_t max_index;

#if defined(HAL_AUDIO_SLT_ENABLE)
uint16_t temp_save_buffer1[nb_buffer_size * 2];
uint16_t temp_save_buffer2[nb_buffer_size * 2];
Complex rComData[256]; //put the result
uint32_t u4MagData[256];
uint32_t cal_buffer_w_ptr1  = 0;
uint32_t cal_buffer_w_ptr2  = 0;
uint32_t delay_sample_count = 0;
bool slt_enable = false;

const int16_t real_fix[8] = {
    32758,
    32728,
    32610,
    32138,
    30273,
    23170,
    0,
    -32768
};

const int16_t image_fix[8] = {
    -804,
    -1607,
    -3211,
    -6392,
    -12539,
    -23170,
    -32768,
    0
};

static void dif_fft(Complex *x, unsigned int Nu)
{
    unsigned int i, j, k, ip, I;
    unsigned int N, LE, LE1, Nv2;
    Complex Wn, W, t, temp;
    N = 1;
    N <<= Nu;
    LE = N * 2;
    for (i = 1; i <= Nu; i++) {
        LE /= 2;
        LE1 = LE / 2;
        Wn.real = 0x7FFF; // Q1.15
        Wn.image = 0; // Wn(0)
        W.real = real_fix[i - 1];
        W.image = image_fix[i - 1]; // Step of Wn increment
        for (j = 1; j <= LE1; j++) {
            for (k = j; k <= N; k += LE) {
                I = k - 1; // index of upper part of butterfly
                ip = I + LE1; // index of lower part of butterfly
                t.real = x[I].real + x[ip].real;
                t.image = x[I].image + x[ip].image; // the output of butterfly upper part
                temp.real = x[I].real - x[ip].real;
                temp.image = x[I].image - x[ip].image; // the output of butterfly lower part
                x[ip].real = (temp.real * Wn.real - temp.image * Wn.image) >> 15; // lower part has to multiply with Wn(k)
                x[ip].image = (temp.real * Wn.image + temp.image * Wn.real) >> 15;
                x[I].real = t.real;
                x[I].image = t.image; // copy t to x[i] directly
            }
            temp.real = (W.real * Wn.real - W.image * Wn.image) >> 15; // Increment Wn(j) to Wn(j+LE)
            Wn.image = (W.real * Wn.image + W.image * Wn.real) >> 15;
            Wn.real = temp.real;
        }
    }
    Nv2 = N / 2;
    j = 1;
    for (i = 1; i <= N - 1; i++) {
        if (i < j) {
            t.real = x[j - 1].real;
            x[j - 1].real = x[i - 1].real;
            x[i - 1].real = t.real;
            t.image = x[j - 1].image;
            x[j - 1].image = x[i - 1].image;
            x[i - 1].image = t.image;
        }
        k = Nv2;
        while (k < j ) {
            j -= k;
            k /= 2;
        }
        j += k;
    }
}

void apply_fft_256(int16_t *pData, int32_t *maxIndex, int32_t *maxValue)
{
    uint32_t i, u4TmpData;
    //u2IdxStart: skip first 5 frequencies 0~312.5Hz
    uint16_t u2IdxData, u2IdxStart = 0;
    int32_t i4AvgData = 0;
    //1. calculate average value
    for (i = 0; i < 256; i++) {
        i4AvgData = i4AvgData + pData[i];
    }
    i4AvgData = i4AvgData >> 8;
    //2. apply Hanning window
    for (i = 0; i < 256; i++) {
        rComData[i].real = (int64_t)(pData[i] - i4AvgData);
        rComData[i].image = 0.0;
    }

    //3. do 256-pt FFT
    dif_fft(rComData, 8);
    //4. calculate magnatude
    for (i = 0; i < 128; i++) {
        int32_t dr = (int32_t)(rComData[i].real);
        int32_t di = (int32_t)(rComData[i].image);
        //Avoid overflow calculate the square after right shift 8 bit
        dr = dr >> 8;
        di = di >> 8;
        u4MagData[i] = (uint32_t)((int64_t)(dr * dr) + (int64_t)(di * di));
    }
    //5. calculate the frequency
    u2IdxData = u2IdxStart;
    u4TmpData = u4MagData[u2IdxStart];
    for (i = 1; i < 128; i++) {
        if (u4MagData[i] > u4TmpData) {
            u2IdxData = i;
            u4TmpData = u4MagData[i];
        }
    }
    *maxIndex = u2IdxData;
    *maxValue = u4TmpData;
}

uint8_t audio_test_detect_1k_tone_result(void)
{
    bool result = false;
    audio_test_result = 1;
    memcpy(&cal_buffer, &temp_save_buffer1, 256 * sizeof(uint16_t));
    apply_fft_256(&cal_buffer, &max_index, &max_value);
    if (max_index == 32) {
        result = true;
    }
    log_hal_info("path 1 result =%d, max_index=%d", result, max_index);
    if (in_device == HAL_AUDIO_DEVICE_DUAL_DIGITAL_MIC) {
        memcpy(&cal_buffer, &temp_save_buffer2, 256 * sizeof(uint16_t));
        apply_fft_256(&cal_buffer, &max_index, &max_value);
        if (max_index == 32) {
            if (result == true) {
                audio_test_result = 0;
            } else {
                audio_test_result = 1;
            }
        }
        log_hal_info("path 2 result =%d, max_index=%d", audio_test_result, max_index);
    } else {
        if (result == true) {
            audio_test_result = 0;
        }
    }
    return audio_test_result;
}
#endif

#ifdef HAL_ACCDET_MODULE_ENABLED
void accdet_callback(hal_accdet_callback_event_t event, void *user_data)
{
    switch (event) {
        case HAL_ACCDET_EVENT_PLUG_IN:
            hal_accdet_set_debounce_time(&debounce_time[1]);
            earphone_plug_in = 1;
            break;
        case HAL_ACCDET_EVENT_PLUG_OUT:
            hal_accdet_set_debounce_time(&debounce_time[0]);
            earphone_plug_in = 0;
            break;
        case HAL_ACCDET_EVENT_HOOK_KEY:
            hal_accdet_set_debounce_time(&debounce_time[1]);
            earphone_plug_in = 3;
            break;
        default:
            break;
    }
}
#endif

static void audio_test_fill_stream_out_buffer_1(void)
{
    int i;
    int temp = 0;
    for (i = 0 ; i < nb_buffer_size ; i++) {
        stream_out_buffer[i] = audio_tone1k_8ksr[temp];
        temp++;
        if (temp == 8) {
            temp = 0;
        }
    }
}

static void audio_test_isr_stream_out_handler(void)
{
    audio_pcm2way_put_to_stream_out(stream_out_buffer);
}

static void audio_test_isr_stream_in_handler(void)
{
    audio_pcm2way_get_from_stream_in(stream_in_buffer);
#if defined(HAL_AUDIO_SLT_ENABLE)
    if (ul_enable) {
        int32_t i = 0;
        if ((delay_sample_count >= 800) && (cal_buffer_w_ptr1 < 320)) { //collect data after 5 times interrupt
            for (i = 0; i < nb_buffer_size; i++) {
                temp_save_buffer1[cal_buffer_w_ptr1++] = stream_in_buffer[i];
            }
        }
        if (in_device == HAL_AUDIO_DEVICE_DUAL_DIGITAL_MIC) {

            volatile uint16_t *dsp_addptr = DSP_DM_ADDR(6, *DSP_DM_ADDR(6, DSP_SPH_SE2_PTR));
            if ((delay_sample_count >= 800) && (cal_buffer_w_ptr2 < 320)) {
                for (i = 0; i < nb_buffer_size; i++) {
                    temp_save_buffer2[cal_buffer_w_ptr2++] = *dsp_addptr++;
                }
            }
        }
        delay_sample_count += nb_buffer_size;
    }
#endif
}

#if defined(HAL_AUDIO_SDFATFS_ENABLE)
static void audio_pcm2way_record_test_isr_stream_in_handler(void)
{
    audio_pcm2way_get_from_stream_in(&stream_in_buffer);
    record_test_isr_count++;
    res = f_write(&fdst, stream_in_buffer, 320, &length_written);
}

static void audio_pcm2way_record_test_isr_wb_stream_in__handler(void)
{
    audio_pcm2way_get_from_stream_in(&stream_in_buffer);
    res = f_write(&fdst, stream_in_buffer, 640, &length_written);
}
#endif

void audio_test_set_audio_tone(bool enable)
{
    audio_tone = enable;
}

void audio_test_set_output_device(hal_audio_device_t device)
{
    out_device = device;
    log_hal_info("%s, out_device=%d", __FUNCTION__, out_device);
}

uint8_t audio_test_play_voice_1k_tone(void)
{
    log_hal_info("%s", __FUNCTION__);
    audio_pcm2way_config_t config;
    audio_test_result = 1;
    config.stream_out_m2d_on = true;
    config.stream_out_is_after_enhancement = false;
    config.format = PCM2WAY_FORMAT_NORMAL;
    config.band = PCM2WAY_NARROWBAND;
    ul_enable = false;
    hal_audio_set_stream_out_channel_number(HAL_AUDIO_MONO);
    hal_audio_set_stream_out_sampling_rate(HAL_AUDIO_SAMPLING_RATE_8KHZ);
    hal_audio_set_stream_out_device(out_device);
    audio_test_fill_stream_out_buffer_1();
    audio_pcm2way_start(audio_test_isr_stream_out_handler, audio_test_isr_stream_in_handler, &config);
    return 0;
}

static void audio_test_fill_stream_out_buffer_2(void)
{
    int i;
    int temp = 0;
    for (i = 0 ; i < USR_BUF_SIZE ; i += 2) {
        user_buffer[i] = audio_tone1k_16ksr[temp];
        user_buffer[i + 1] = audio_tone2k_16ksr[temp];
        temp++;
        if (temp == 32) {
            temp = 0;
        }
    }
}

void user_audio_stream_in_callback(hal_audio_event_t event, void *user_data)
{
    uint32_t sample_count;
    switch (event) {
        case HAL_AUDIO_EVENT_DATA_NOTIFICATION:
            record_test_isr_count++;
            hal_audio_get_stream_in_sample_count(&sample_count);
            hal_audio_read_stream_in(user_buffer + user_buf_read_ptr, sample_count);
#if defined(HAL_AUDIO_SDFATFS_ENABLE)
            res = f_write(&fdst, (user_buffer + user_buf_read_ptr), sample_count, &length_written);
#endif
            user_buf_read_ptr += (sample_count>>1);
            if(user_buf_read_ptr>=2048) {
                user_buf_read_ptr -= 2048;
            }
            if (record_test_isr_count == 200) {
#if defined(HAL_AUDIO_SDFATFS_ENABLE)
                res = f_close(&fdst);
#endif
                hal_audio_stop_stream_in();
            }
            break;
        default:
            break;
    }
}

void user_audio_stream_out_callback(hal_audio_event_t event, void *user_data)
{
    uint32_t sample_count;
    switch (event) {
        case HAL_AUDIO_EVENT_UNDERFLOW:
        case HAL_AUDIO_EVENT_DATA_REQUEST:
            hal_audio_get_stream_out_sample_count(&sample_count);
            hal_audio_write_stream_out(user_buffer + user_buf_read_ptr, sample_count);
            user_buf_read_ptr += (sample_count >> 1);
            if (user_buf_read_ptr >= INT_BUF_SIZE) {
                user_buf_read_ptr -= INT_BUF_SIZE;
            }
            break;
        default:
            break;
    }
}

/*HQA*/
uint8_t audio_test_play_audio_1k_tone(void)
{
    log_hal_info("%s", __FUNCTION__);
    hal_audio_set_stream_out_sampling_rate(HAL_AUDIO_SAMPLING_RATE_16KHZ);
    audio_test_result = 1;
    hal_audio_set_stream_out_channel_number(HAL_AUDIO_STEREO);
    hal_audio_set_stream_out_device(out_device);
    audio_test_fill_stream_out_buffer_2();
    hal_audio_register_stream_out_callback(user_audio_stream_out_callback, NULL);
    hal_audio_set_memory(&internal_buffer);
#ifdef HAL_AUDIO_LOW_POWER_ENABLED
    audio_lowpower_set_mode(true);
#endif
    hal_audio_write_stream_out(user_buffer, USR_BUF_SIZE);
    hal_audio_start_stream_out(HAL_AUDIO_PLAYBACK_MUSIC);
    return 0;
}

#if defined(HAL_AUDIO_SDFATFS_ENABLE)
uint8_t audio_test_pcm_record(void)
{
    log_hal_info("%s", __FUNCTION__);
    sd_count = 0;
    res = f_mount(&fatfs, "0:", 1);
    if ( !res ) {
        log_hal_info("fmount ok \n");
        res = f_open(&fdst, "SD:/Test_File_WR.pcm", FA_CREATE_ALWAYS | FA_WRITE | FA_READ);
        if (!res) {
            log_hal_info("fopen ok \n");
        } else {
            log_hal_info("fopen error \n");
            return 0;
        }
    } else {
        log_hal_info("fmount error \n");
        return 0;
    }
    hal_audio_set_stream_in_sampling_rate(HAL_AUDIO_SAMPLING_RATE_8KHZ);
    hal_audio_set_stream_in_channel_number(HAL_AUDIO_MONO);
    hal_audio_set_stream_in_device(HAL_AUDIO_DEVICE_DUAL_DIGITAL_MIC);
    hal_audio_register_stream_in_callback(user_audio_stream_in_callback, NULL);
    hal_audio_set_memory(&internal_buffer);
    //*(volatile uint16_t *)(0x82207DB6) = 0x0200; //ul debug tone
    hal_audio_start_stream_in(HAL_AUDIO_RECORD_VOICE);
    //*(volatile uint16_t *)(0x82207DB6) = 0x0800; //ul debug tone
    return 0;
}
#endif

void audio_test_set_input_device(hal_audio_device_t device)
{
    in_device = device;
    log_hal_info("%s, in_device=%d", __FUNCTION__, in_device);
}

uint8_t audio_test_external_loopback_test(void)
{
    log_hal_info("%s", __FUNCTION__);
    audio_pcm2way_config_t config;
    config.stream_in_d2m_on = true;
    config.stream_out_m2d_on = true;
    config.stream_in_is_after_enhancement = true;
    config.stream_out_is_after_enhancement = false;
    config.format = PCM2WAY_FORMAT_NORMAL;
    config.band = PCM2WAY_NARROWBAND;
    ul_enable = true;
    hal_audio_set_stream_out_channel_number(HAL_AUDIO_MONO);
    hal_audio_set_stream_out_sampling_rate(HAL_AUDIO_SAMPLING_RATE_8KHZ);
    hal_audio_set_stream_in_channel_number(HAL_AUDIO_MONO);
    hal_audio_set_stream_in_sampling_rate(HAL_AUDIO_SAMPLING_RATE_8KHZ);
    hal_audio_set_stream_out_device(out_device);
    hal_audio_set_stream_in_device(in_device);
    *(volatile uint16_t *)(0x82207E9C) = 0x0000; //reset to channel 0
    audio_test_fill_stream_out_buffer_1();
    audio_pcm2way_start(audio_test_isr_stream_out_handler, audio_test_isr_stream_in_handler, &config);
    *DSP_SPH_UL_VOL    = 0x1400;
    *DSP_SPH_DL_VOL    = 0x0E3D;
    *(volatile uint16_t *)(0xA21C00B4) = 0x24FF;
    return 0;
}

uint8_t audio_test_loopback_voice_1k_tone(void)
{
    log_hal_info("%s", __FUNCTION__);
    audio_pcm2way_config_t config;
    config.stream_in_d2m_on = true;
    config.stream_out_m2d_on = true;
    config.stream_in_is_after_enhancement = true;
    config.stream_out_is_after_enhancement = false;
    config.format = PCM2WAY_FORMAT_NORMAL;
    config.band = PCM2WAY_NARROWBAND;
    hal_audio_set_stream_out_channel_number(HAL_AUDIO_MONO);
    hal_audio_set_stream_out_sampling_rate(HAL_AUDIO_SAMPLING_RATE_8KHZ);
    hal_audio_set_stream_out_channel_number(HAL_AUDIO_MONO);
    hal_audio_set_stream_out_sampling_rate(HAL_AUDIO_SAMPLING_RATE_8KHZ);
    hal_audio_set_stream_in_channel_number(HAL_AUDIO_MONO);
    hal_audio_set_stream_in_sampling_rate(HAL_AUDIO_SAMPLING_RATE_8KHZ);
    hal_audio_set_stream_out_device(out_device);
    hal_audio_set_stream_in_device(in_device);
    audio_test_fill_stream_out_buffer_1();
    audio_pcm2way_start(audio_test_isr_stream_out_handler, audio_test_isr_stream_in_handler, &config);
    *DSP_SPH_UL_VOL    = 0x1400;
    *DSP_SPH_DL_VOL    = 0x0E3D;
    *(volatile uint16_t *)(0xA21C00B4) = 0x24FF;
    return 0;
}
void audio_test_switch_mic_type(uint8_t mic_type)
{
    if (mic_type == 0) { //ACC
        *ABBA_VBITX_CON4 &= 0x000A;
    } else { //DCC
        *ABBA_VBITX_CON4 = (*ABBA_VBITX_CON4 & 0xA7FF) | 0x4000;
    }
}

#if defined(HAL_AUDIO_SDFATFS_ENABLE)
uint8_t audio_test_pcm2way_record(void)
{
    log_hal_info("%s", __FUNCTION__);
    sd_count = 0;
    res = f_mount(&fatfs, "0:", 1);
    if ( !res ) {
        log_hal_info("fmount ok \n");
        res = f_open(&fdst, "SD:/Test_File_WR.pcm", FA_CREATE_ALWAYS | FA_WRITE | FA_READ);
        if (!res) {
            log_hal_info("fopen ok \n");
        } else {
            log_hal_info("fopen error \n");
            return 0;
        }
    } else {
        log_hal_info("fmount error \n");
        return 0;
    }
    audio_pcm2way_config_t config;
    config.stream_in_d2m_on = true;
    config.stream_out_m2d_on = true;
    config.stream_in_is_after_enhancement = true;
    config.stream_out_is_after_enhancement = false;
    config.format = PCM2WAY_FORMAT_NORMAL;
    config.band = PCM2WAY_NARROWBAND;
    ul_enable = true;
    hal_audio_set_stream_out_channel_number(HAL_AUDIO_MONO);
    hal_audio_set_stream_out_sampling_rate(HAL_AUDIO_SAMPLING_RATE_8KHZ);
    hal_audio_set_stream_in_channel_number(HAL_AUDIO_MONO);
    hal_audio_set_stream_in_sampling_rate(HAL_AUDIO_SAMPLING_RATE_8KHZ);
    hal_audio_set_stream_out_device(out_device);
    hal_audio_set_stream_in_device(in_device);
    *(volatile uint16_t *)(0x82207E9C) = 0x0000; //reset to channel 0
    audio_test_fill_stream_out_buffer_1();
    audio_pcm2way_start(audio_test_isr_stream_out_handler, audio_pcm2way_record_test_isr_stream_in_handler, &config);
    *ABBA_VBITX_CON0 = ((*ABBA_VBITX_CON0 & (~0x01C0)) | 0x3 << 6);
    //*ABBA_VBITX_CON4   = 0x000A;
    *DSP_SPH_UL_VOL    = 0x1400;
    *DSP_SPH_DL_VOL    = 0x0E3D;
    *(volatile uint16_t *)(0xA21C00B4) = 0x24FF;
    return 0;
}

uint8_t audio_test_pcm2way_wb_record(void)
{
    log_hal_info("%s", __FUNCTION__);
    sd_count = 0;
    res = f_mount(&fatfs, "0:", 1);
    if ( !res ) {
        log_hal_info("fmount ok \n");
        res = f_open(&fdst, "SD:/Test_File_WR.pcm", FA_CREATE_ALWAYS | FA_WRITE | FA_READ);
        if (!res) {
            log_hal_info("fopen ok \n");
        } else {
            log_hal_info("fopen error \n");
            return 0;
        }
    } else {
        log_hal_info("fmount error \n");
        return 0;
    }
    audio_pcm2way_config_t config;
    config.stream_in_d2m_on = true;
    config.stream_out_m2d_on = true;
    config.stream_in_is_after_enhancement = true;
    config.stream_out_is_after_enhancement = false;
    config.format = PCM2WAY_FORMAT_NORMAL;
    config.band = PCM2WAY_WIDEBAND;
    ul_enable = true;
    hal_audio_set_stream_out_channel_number(HAL_AUDIO_MONO);
    hal_audio_set_stream_out_sampling_rate(HAL_AUDIO_SAMPLING_RATE_16KHZ);
    hal_audio_set_stream_in_channel_number(HAL_AUDIO_MONO);
    hal_audio_set_stream_in_sampling_rate(HAL_AUDIO_SAMPLING_RATE_16KHZ);
    hal_audio_set_stream_out_device(out_device);
    hal_audio_set_stream_in_device(in_device);
    *(volatile uint16_t *)(0x82207E9C) = 0x0000; //reset to channel 0
    audio_test_fill_stream_out_buffer_1();
    audio_pcm2way_start(audio_test_isr_stream_out_handler, audio_pcm2way_record_test_isr_wb_stream_in__handler, &config);
    *ABBA_VBITX_CON0 = ((*ABBA_VBITX_CON0 & (~0x01C0)) | 0x3 << 6);
    //*ABBA_VBITX_CON4   = 0x000A;
    *DSP_SPH_UL_VOL    = 0x1400;
    *DSP_SPH_DL_VOL    = 0x0E3D;
    *(volatile uint16_t *)(0xA21C00B4) = 0x24FF;
    return 0;
}
#endif

uint8_t audio_test_stop_1k_tone(void)
{
    if (audio_tone) {
        hal_audio_stop_stream_out();
#ifdef HAL_AUDIO_LOW_POWER_ENABLED
        audio_lowpower_set_mode(false);
#endif
    } else {
        audio_pcm2way_stop();
    }
    audio_tone = false;
#if defined(HAL_AUDIO_SLT_ENABLE)
    delay_sample_count = 0;
    cal_buffer_w_ptr1 = 0;
    cal_buffer_w_ptr2 = 0;
#endif
    return 0;
}

#ifdef HAL_ACCDET_MODULE_ENABLED
uint8_t audio_test_detect_earphone(void)
{
    return earphone_plug_in;
}

void register_accdet_callback(void)
{
    uint32_t user_data;
    earphone_plug_in = 0;

    hal_accdet_init();//configure PWM and debouce time
    hal_accdet_set_debounce_time(&debounce_time[0]);
    hal_accdet_register_callback(accdet_callback, (void *)&user_data);/*register callback function*/
    hal_accdet_enable();//enable ACCDET
}
#endif

#if defined(HAL_AUDIO_SLT_ENABLE)
/*SLT*/
uint8_t audio_test_internal_loopback_test(void)
{
    log_hal_info("%s", __FUNCTION__);
    audio_pcm2way_config_t config;
    config.stream_in_d2m_on = true;
    config.stream_out_m2d_on = true;
    config.stream_in_is_after_enhancement = true;
    config.stream_out_is_after_enhancement = false;
    config.format = PCM2WAY_FORMAT_NORMAL;
    config.band = PCM2WAY_NARROWBAND;
    ul_enable = true;
    slt_enable = true;
    hal_audio_set_stream_out_channel_number(HAL_AUDIO_MONO);
    hal_audio_set_stream_out_sampling_rate(HAL_AUDIO_SAMPLING_RATE_8KHZ);
    hal_audio_set_stream_in_channel_number(HAL_AUDIO_MONO);
    hal_audio_set_stream_in_sampling_rate(HAL_AUDIO_SAMPLING_RATE_8KHZ);
    hal_audio_set_stream_in_device(in_device);
    hal_audio_set_stream_out_device(HAL_AUDIO_DEVICE_NONE);
    audio_test_fill_stream_out_buffer_1();
    *AFE_DBG_SIG |= 0x0080; //digital afe dl (@uDSP)
    audio_pcm2way_start(audio_test_isr_stream_out_handler, audio_test_isr_stream_in_handler, &config);
    *DSP_SPH_UL_VOL    = 0x1000;
    *DSP_SPH_DL_VOL    = 0x071E;
    slt_enable = false;
    return 0;
}

uint8_t audio_slt_test(void)
{
    uint8_t result = 1;
    audio_test_set_input_device(HAL_AUDIO_DEVICE_DUAL_DIGITAL_MIC);
    audio_test_set_audio_tone(false);
    audio_test_internal_loopback_test();
    hal_gpt_delay_ms(500);
    audio_test_stop_1k_tone();
    result = audio_test_detect_1k_tone_result();
    return result;
}

bool audio_is_slt_test(void)
{
    return slt_enable;
}
#endif

#if defined(HAL_AUDIO_SDFATFS_ENABLE)
uint8_t audio_test_pcm2way_stop_1k_tone(void)
{
    audio_pcm2way_stop();
    res = f_close(&fdst);
    if (!res) {
        log_hal_info("fclose success \n");
    } else {
        log_hal_info("fclose error \n");
    }
    return 0;
}

void user_audio_stream_out_sd_callback(hal_audio_event_t event, void *user_data)
{
    uint32_t sample_count;
    switch (event) {
        case HAL_AUDIO_EVENT_UNDERFLOW:
        case HAL_AUDIO_EVENT_DATA_REQUEST:
            if (f_eof(&fdst)) {
                audio_test_stop_audio_sd();
                log_hal_info("[EOF]audio_test_stop_audio_sd \n");
                if (sd_repeat_play == true) {
                    audio_test_play_audio_sd();
                    return;
                }
            }
            hal_audio_get_stream_out_sample_count(&sample_count);
            f_read(&fdst, user_buffer, sample_count, &length_read);
            hal_audio_write_stream_out(user_buffer, sample_count);
            break;
    }
}

void audio_test_fill_stream_out_buffer_from_sd(void)
{
    sd_count = 0;
    res = f_mount(&fatfs, "0:", 1);
    if (!res) {
        log_hal_info("fmount ok \n");
        res = f_open(&fdst, "SD:/Trap.pcm", FA_OPEN_EXISTING | FA_READ);
        if (!res) {
            log_hal_info("fopen ok \n");
        } else {
            log_hal_info("fopen error \n");
            return 0;
        }
    } else {
        log_hal_info("fmount error \n");
        return 0;
    }
    memset(user_buffer, 0, sizeof(user_buffer));
    res = f_read(&fdst, user_buffer, USR_BUF_SIZE * 2, &length_read);
}

uint8_t audio_test_play_audio_sd(void)
{
    log_hal_info("%s", __FUNCTION__);
    hal_audio_set_stream_out_sampling_rate(HAL_AUDIO_SAMPLING_RATE_44_1KHZ);
    audio_test_result = 1;
    hal_audio_set_stream_out_channel_number(HAL_AUDIO_STEREO);
    hal_audio_set_stream_out_device(HAL_AUDIO_DEVICE_HEADSET);
    audio_test_fill_stream_out_buffer_from_sd();
    hal_audio_register_stream_out_callback(user_audio_stream_out_sd_callback, NULL);
    hal_audio_set_memory(&internal_buffer);
#ifdef HAL_AUDIO_LOW_POWER_ENABLED
    audio_lowpower_set_mode(true);
#endif
    hal_audio_write_stream_out(user_buffer, USR_BUF_SIZE);
    hal_audio_start_stream_out(HAL_AUDIO_PLAYBACK_MUSIC);
    return 0;
}

uint8_t audio_test_stop_audio_sd(void)
{
    hal_audio_stop_stream_out();
    res = f_close(&fdst);
    if (!res) {
        log_hal_info("fclose success \n");
    } else {
        log_hal_info("fclose error \n");
    }
    return 0;
}

void audio_set_audio_sd_play_repeat(bool repeat)
{
    sd_repeat_play = repeat;
}

#endif

#endif /* defined(HAL_I2S_MODULE_ENABLED) || defined(HAL_AUDIO_MODULE_ENABLED) */
#endif /*defined(__GNUC__)*/
