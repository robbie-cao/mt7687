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

#if defined(HAL_I2S_MODULE_ENABLED) || defined(HAL_AUDIO_MODULE_ENABLED)

#include "hal_audio_internal_service.h"
#include "hal_audio_internal_afe.h"
#include "hal_audio_internal_pcm.h"
#include "hal_audio_fw_sherif.h"
#include "hal_audio_fw_interface.h"
#include "hal_pmu_internal.h"
#include "hal_nvic_internal.h"
#include "hal_sleep_manager.h"
#include "hal_dsp_topsm.h"
#include "hal_log.h"
#ifdef HAL_AUDIO_LOW_POWER_ENABLED
#include "hal_dsp_topsm.h"
#endif
#include "exception_handler.h"
#ifdef HAL_DVFS_MODULE_ENABLED
#include "hal_dvfs.h"
#endif
#include "hal_audio_post_processing_internal.h"

audio_isr_t audio_isr;
audio_common_t audio_common;
extern afe_t afe;

#ifdef HAL_DVFS_MODULE_ENABLED
static bool dsp_dvfs_valid(uint32_t voltage, uint32_t frequency)
{
    if (frequency < AUDIO_CPU_FREQ_L_BOUND) {
        return false;
    }
    else {
        return true;
    }
}

static dvfs_notification_t dsp_dvfs_desc = {
    .domain = "VCORE",
    .module = "CM_CK0",
    .addressee = "dsp_dvfs",
    .ops = {
        .valid = dsp_dvfs_valid,
    }
};

static void audio_register_dsp_dvfs (bool flag)
{
    if (flag) {
        dvfs_register_notification(&dsp_dvfs_desc);
        hal_dvfs_target_cpu_frequency(AUDIO_CPU_FREQ_L_BOUND, HAL_DVFS_FREQ_RELATION_L);
    }
    else{
        dvfs_deregister_notification(&dsp_dvfs_desc);
    }

}
#endif /*HAL_DVFS_MODULE_ENABLED*/

const int16_t speech_src_filter_coefficient[82] = {
    96,  162,   -92,   -488,  -302,   408,   372,  -660,  -780,    782,
    1272, -958, -2094,   1088,  3458, -1200, -6426,  1266, 20318,  30834,
    20318, 1266, -6426,  -1200,  3458,  1088, -2094,  -958,  1272,    782,
    -780, -660,   372,    408,  -302,  -488,   -92,   162,    96,      0,
    0,
    -36, -175,  -262,    -51,   277,   146,  -376,  -317,   489,    595,
    -597, -1034,   689,   1777,  -744, -3351,   727, 10500, 15852,  10500,
    727, -3351,  -744,   1777,   689, -1034,  -597,  -595,   489,   -317,
    -376,  146,   277,    -51,  -262,  -175,   -36,     0,     0,      0,
    0
};

void audio_dsp_reload_coeficient(void)
{
    audio_dsp_write_anti_alias_filter();
}

void audio_dsp_write_anti_alias_filter(void)
{
    int i;
    const int16_t *ptr         = speech_src_filter_coefficient;
    volatile uint16_t *dsp_ptr = DSP_PM_ADDR(3, *DSP_SPH_FLT_COEF_ADDR_SRC);
    for (i = 82; i > 0; i--) {
        *dsp_ptr++ = *ptr++;
    }
}

const uint16_t audio_dsp_pcm_idle_delay_table[3][3] = {
    { 0x0A, 0x0A , 0x4F + (16 << 10) + (2 << 8)}, /*for pcm 8k record*/
    { 0x0A, 0x0A , 0x28 + (16 << 10) + (2 << 8)}, /*for pcm 16k record*/
    { 0x0A, 0x0A , 0x96 + (16 << 10) + (2 << 8)}, /*for pcm2way*/
};

void audio_dsp_speech_set_delay(uint8_t type)
{
    *DSP_SPH_DEL_R = audio_dsp_pcm_idle_delay_table[type][0];
    *DSP_SPH_DEL_W = audio_dsp_pcm_idle_delay_table[type][1];
    *DSP_SPH_DEL_M = audio_dsp_pcm_idle_delay_table[type][2];
}

static void audio_service_dsp_wakeup_setting(bool flag)
{
    uint32_t savedmask = save_and_set_interrupt_mask();
    uint16_t reg_value = *SHARE_D2M_WAKEUP_CTL;
    if (flag) {
        reg_value |=  (0x1 << 0);
    } else {
        reg_value &= ~(0x1 << 0);
    }
    *SHARE_D2M_WAKEUP_CTL = reg_value;
    restore_interrupt_mask(savedmask);
    return;
}

static void audio_service_clean_dsp_wakeup_event(void)
{
    *SHARE_D2M_WAKEUP_STA = 0x1;
    return;
}

void audio_service_hook_isr(uint16_t isrtype, isr function, void *userdata)
{
    uint32_t func_id;
    for (func_id = 0; func_id < MAXISR; func_id++) {
        if (audio_isr.num[func_id] == 0) {
            audio_isr.num[func_id] = isrtype;
            audio_isr.function[func_id] = function;
            audio_isr.userdata[func_id] = userdata;
            break;
        }
    }
    return;
}

void audio_service_unhook_isr(uint16_t isrtype)
{
    uint32_t func_id;
    for (func_id = 0; func_id < MAXISR; func_id++) {
        if (audio_isr.num[func_id] == isrtype) {
            audio_isr.num[func_id] = 0;
            audio_isr.function[func_id] = NULL;
            audio_isr.userdata[func_id] = NULL;
            break;
        }
    }
    return;
}

static bool audio_is_from_speech(uint16_t iid, uint32_t *sph_int)
{
    uint32_t val = 0;
    uint16_t mask = 0;
    bool from_sph = true;
    switch (iid) {
        case DSP_IID_SPEECH_UL_ID:
            mask = *DSP_D2C_SPEECH_UL_INT;
            if (mask & 0x0001) {
                val += PSEUDO_SAL_DSPINT_ID_REC_PCM;
            }
            if (mask & 0x0002) {
                val += PSEUDO_SAL_DSPINT_ID_REC_EPL;
            }
            if (mask & 0x0004) {
                val += PSEUDO_SAL_DSPINT_ID_PNW_UL;
            }
            *DSP_D2C_SPEECH_UL_INT = 0;
            break;
        case DSP_IID_SPEECH_DL_ID:
            mask = *DSP_D2C_SPEECH_DL_INT;
            if (mask & 0x0001) {
                val += PSEUDO_SAL_DSPINT_ID_REC_VM;
            }
            if (mask & 0x0004) {
                val += PSEUDO_SAL_DSPINT_ID_PNW_DL;
            }
            *DSP_D2C_SPEECH_DL_INT = 0;
            break;
        default:
            from_sph = false;
            break;
    }
    *sph_int = val;
    return from_sph;
}

static void audio_service_trigger_callback (uint16_t magic_number)
{
    uint32_t func_id;
    for (func_id = 0; func_id < MAXISR; func_id++) {
        if (magic_number == audio_isr.num[func_id]) {
            void *userdata = audio_isr.userdata[func_id];
            audio_isr.function[func_id](userdata);
            break;
        }
    }
    return;
}

static void audio_service_dsp_to_mcu_lisr (uint16_t itype)
{
    if (itype != 0x0000) {
        uint32_t sph_int = 0;
        bool from_sph = audio_is_from_speech(itype, &sph_int);
        if (from_sph) {
            uint16_t bit_idx;
            for (bit_idx = 1; bit_idx < PSEUDO_SAL_DSPINT_PRIO_MAX ; bit_idx++) {
                if (sph_int & (1 << bit_idx)) {
                    audio_service_trigger_callback(DSP_INT_MAPPING_BASIC + bit_idx);
                }
            }
        } else {
            audio_service_trigger_callback(itype);
        }
    }
    return;
}

static void audio_service_dsp_memory_callback_init (void)
{
    if (audio_common.assert_source == AUDIO_DSP_ASSERT_NONE) {
        DSP_WRITE(DSP_MCU_STATUS, 0);   /* Freeze DSP */
        __disable_irq();
        audio_common.assert_source = AUDIO_DSP_ASSERT_FROM_MCU;
    }
    return;
}

const uint32_t audio_dsp_pm_page_table[AUDIO_DSP_PM_PAGE_COUNT] = {0, 3};
const uint32_t audio_dsp_dm_page_table[AUDIO_DSP_DM_PAGE_COUNT] = {5, 6, 7};

static void audio_service_dsp_memory_callback_dump (void)
{
    {   /* Dump PM */
        uint32_t table_idx;
        for (table_idx = 0; table_idx < AUDIO_DSP_PM_PAGE_COUNT; table_idx++) {
            uint32_t page_idx = audio_dsp_pm_page_table[table_idx];
            volatile uint16_t *p_src = DSP_PM_ADDR(page_idx, 0x0000);
            uint32_t loop_idx;
            for (loop_idx = 0; loop_idx < 0x4000; loop_idx += 8) {
                uint16_t dsp_val[8];
                uint32_t val_idx;
                uint32_t base_ptr = (uint32_t)p_src;
                for (val_idx = 0; val_idx < 8; val_idx++) {
                    dsp_val[val_idx] = *p_src++;
                }
                printf("0x%08x: %04x %04x %04x %04x %04x %04x %04x %04x\n\r", (unsigned int)base_ptr, dsp_val[0], dsp_val[1], dsp_val[2], dsp_val[3], dsp_val[4], dsp_val[5], dsp_val[6], dsp_val[7]);
            }
        }
    }
    {   /* Dump DM */
        uint32_t table_idx;
        for (table_idx = 0; table_idx < AUDIO_DSP_DM_PAGE_COUNT; table_idx++) {
            uint32_t page_idx = audio_dsp_dm_page_table[table_idx];
            volatile uint16_t *p_src = DSP_DM_ADDR(page_idx, 0x0000);
            uint32_t loop_idx;
            for (loop_idx = 0; loop_idx < 0x4000; loop_idx += 8) {
                uint16_t dsp_val[8];
                uint32_t val_idx;
                uint32_t base_ptr = (uint32_t)p_src;
                for (val_idx = 0; val_idx < 8; val_idx++) {
                    dsp_val[val_idx] = *p_src++;
                }
                printf("0x%08x: %04x %04x %04x %04x %04x %04x %04x %04x\n\r", (unsigned int)base_ptr, dsp_val[0], dsp_val[1], dsp_val[2], dsp_val[3], dsp_val[4], dsp_val[5], dsp_val[6], dsp_val[7]);
           }
        }
    }
    return;
}

void audio_service_dsp_memory_dump_init (void)
{
    exception_config_type callback_config;
    callback_config.init_cb = audio_service_dsp_memory_callback_init;
    callback_config.dump_cb = audio_service_dsp_memory_callback_dump;
    exception_register_callbacks(&callback_config);
    audio_common.assert_source = AUDIO_DSP_ASSERT_NONE;
    return;
}

void audio_service_dsp_to_mcu_interrupt(void)
{
    /* mask D2C interrupt and Ack D2M interrupt */
    NVIC_DisableIRQ(DSP22CPU_IRQn);
    audio_service_clean_dsp_wakeup_event();
    {
        uint16_t d2m_st = DSP_READ(SHARE_D2MSTA);
        if (d2m_st & (1 << 4)) {    /* Task 4 */
            uint16_t d2m_id = DSP_READ(DSP_D2M_TASK4);
            audio_service_dsp_to_mcu_lisr(d2m_id);
        }
        if (d2m_st & (1 << 5)) {    /* Task 5 */
            uint16_t d2m_id = DSP_READ(DSP_D2M_TASK5);
            audio_service_dsp_to_mcu_lisr(d2m_id);
        }
        if (d2m_st & (1 << 6)) {    /* Task 6 */
            uint16_t d2m_id = DSP_READ(DSP_D2M_TASK6);
            audio_service_dsp_to_mcu_lisr(d2m_id);
        }
        if (d2m_st & (1 << 7)) {    /* Task 7 : DSP 0xDD44 crash */
            uint16_t d2m_id = DSP_READ(DSP_D2M_TASK7);
            if (d2m_id == DSP_DSP_EXCEPTION_ID) {
                DSP_WRITE(DSP_MCU_STATUS, 0);   /* Freeze DSP */
                __disable_irq();
                audio_common.assert_source = AUDIO_DSP_ASSERT_FROM_DSP;
                configASSERT(0);
            }
        }
    }
    /* unmask D2C interrupt */
    NVIC_EnableIRQ(DSP22CPU_IRQn);
    /* At this point, the next D2M interrupt is able to trigger MCU. */
    return;
}

static uint16_t audio_convert_dsp_sample(uint16_t frequency)
{
    uint16_t dsp_frequency;
    switch (frequency) {
        case AUDIO_COMMON_SAMPLING_RATE_8KHZ:
            dsp_frequency = ASP_FS_8K;
            break;
        case AUDIO_COMMON_SAMPLING_RATE_11_025KHZ:
            dsp_frequency = ASP_FS_11K;
            break;
        case AUDIO_COMMON_SAMPLING_RATE_12KHZ:
            dsp_frequency = ASP_FS_12K;
            break;
        case AUDIO_COMMON_SAMPLING_RATE_16KHZ:
            dsp_frequency = ASP_FS_16K;
            break;
        case AUDIO_COMMON_SAMPLING_RATE_22_05KHZ:
            dsp_frequency = ASP_FS_22K;
            break;
        case AUDIO_COMMON_SAMPLING_RATE_24KHZ:
            dsp_frequency = ASP_FS_24K;
            break;
        case AUDIO_COMMON_SAMPLING_RATE_32KHZ:
            dsp_frequency = ASP_FS_32K;
            break;
        case AUDIO_COMMON_SAMPLING_RATE_44_1KHZ:
            dsp_frequency = ASP_FS_44K;
            break;
        case AUDIO_COMMON_SAMPLING_RATE_48KHZ:
            dsp_frequency = ASP_FS_48K;
            break;
        case AUDIO_COMMON_SAMPLING_RATE_96KHZ:
            dsp_frequency = ASP_FS_96K;
            break;
        default:
            dsp_frequency = 0;
    }
    return dsp_frequency;
}

uint16_t audio_transfer_sample_rate(uint16_t sample_rate)
{
    uint16_t sample_rate_temp = 8000;
    switch (sample_rate) {
        case HAL_AUDIO_SAMPLING_RATE_8KHZ:
            sample_rate_temp = 8000;
            break;
        case HAL_AUDIO_SAMPLING_RATE_11_025KHZ:
            sample_rate_temp = 11025;
            break;
        case HAL_AUDIO_SAMPLING_RATE_12KHZ:
            sample_rate_temp = 12000;
            break;
        case HAL_AUDIO_SAMPLING_RATE_16KHZ:
            sample_rate_temp = 16000;
            break;
        case HAL_AUDIO_SAMPLING_RATE_22_05KHZ:
            sample_rate_temp = 22050;
            break;
        case HAL_AUDIO_SAMPLING_RATE_24KHZ:
            sample_rate_temp = 24000;
            break;
        case HAL_AUDIO_SAMPLING_RATE_32KHZ:
            sample_rate_temp = 32000;
            break;
        case HAL_AUDIO_SAMPLING_RATE_44_1KHZ:
            sample_rate_temp = 44100;
            break;
        case HAL_AUDIO_SAMPLING_RATE_48KHZ:
            sample_rate_temp = 48000;
            break;
        default:
            return sample_rate_temp;
    }
    return  sample_rate_temp;
}

uint8_t audio_transfer_channel_number(uint16_t channel)
{
    uint8_t channel_temp = 2;
    switch (channel) {
        case HAL_AUDIO_MONO:
            channel_temp = 1;
            break;
        case HAL_AUDIO_STEREO:
            channel_temp = 2;
            break;
        default:
            return  channel_temp;
    }
    return  channel_temp;
}

#if defined(MTK_AUDIO_IIR_FILTER_ENABLE)
const uint16_t pcm_sd_playback_frame_length_table[9] = {
    256, 256, 256, 512, 512, 512, 1024, 1024, 1024
};
#endif

static void audio_dsp_playback_on(uint16_t active_type, uint16_t sample_rate)
{
    uint16_t isr_cycle = 0;
    uint16_t del_m = 0;
    uint16_t del_w = 0x20;
    uint16_t uFrameLen = ASP_FRAMELEN_PCM;
#if defined(MTK_AUDIO_IIR_FILTER_ENABLE)
    {   /* Check & Apply IIR If Needed */
        int32_t apply_iir;
        uint32_t sr_val = (uint32_t)audio_transfer_sample_rate (sample_rate);
        audio_iir_generate_coefficient_table(sr_val);
        apply_iir = is_applying_audio_iir(sr_val);
        audio_common.apply_iir = apply_iir;
        if (apply_iir) {
            audio_write_iir_coefficients_to_dsp();
            audio_iir_start();
            if (active_type == ASP_TYPE_PCM_HI) {
                sample_rate = BOUNDED(sample_rate, AUDIO_COMMON_SAMPLING_RATE_48KHZ, AUDIO_COMMON_SAMPLING_RATE_8KHZ);
                uFrameLen = pcm_sd_playback_frame_length_table[sample_rate];
                audio_pcm_set_dsp_sd_task_playback(uFrameLen);
                active_type = ASP_TYPE_PCM_SD_PLAYBACK;
            }
        }
    }
#endif
    audio_common.active_type = active_type;
    switch (active_type) {
#if defined(MTK_AUDIO_IIR_FILTER_ENABLE)
        case ASP_TYPE_PCM_SD_PLAYBACK:
            del_m = uFrameLen;
            isr_cycle = del_m;
            break;
#endif
        case ASP_TYPE_PCM_HI:
            del_m = ASP_DELM_PCM_HIGFS;
            if (((*DSP_AUDIO_PP_CTRL_BASE) & 0x0020) == 0) {
                if (sample_rate <= AUDIO_COMMON_SAMPLING_RATE_12KHZ) {
                    del_m = ASP_DELM_PCM_LOWFS;
                } else if (sample_rate <= AUDIO_COMMON_SAMPLING_RATE_24KHZ) {
                    del_m = ASP_DELM_PCM_MEDFS;
                }
            }
            isr_cycle = uFrameLen;
            break;
        case ASP_TYPE_I2SBypass:
            uFrameLen = ASP_FRAMELEN_I2S;
            del_m = ASP_DELM_I2SBypass;
            isr_cycle = del_m;
            break;
        case ASP_TYPE_SBC_DEC:
            uFrameLen = ASP_FRAMELEN_SBC_DEC;
            del_m = ASP_DELM_SBC_DEC;
            isr_cycle = del_m;
            break;
        case ASP_TYPE_AAC_DEC:
            uFrameLen = ASP_FRAMELEN_AAC_DEC;
            del_m = ASP_DELM_AAC_DEC;
            isr_cycle = del_m;
            break;
        default:
            break;
    }
    *DSP_AUDIO_ASP_TYPE_ADDR      = active_type;
    *DSP_AUDIO_ASP_FS_ADDR        = audio_convert_dsp_sample(sample_rate);
    *DSP_AUDIO_ASP_DEL_W_ADDR     = del_w;
    *DSP_AUDIO_ASP_DEL_MARGIN_W_ADDR     = del_m;
    *DSP_AUDIO_DEC_FRAME_LENGTH = uFrameLen;
    *DSP_AUDIO_SD_ISR_CYCLE  = isr_cycle;
    *DSP_ASP_FORCE_KT_8K = 0x8000;
    VBI_RESET();
    return;
}

static void audio_dsp_playback_off()
{
#if defined(MTK_AUDIO_IIR_FILTER_ENABLE)
    if (audio_common.apply_iir) {
        audio_iir_stop();
        audio_common.apply_iir = 0;
    }
#endif
    *DSP_AUDIO_ASP_TYPE_ADDR      = ASP_TYPE_VOICE;
    *DSP_AUDIO_ASP_FS_ADDR        = ASP_FS_8K;
    *DSP_AUDIO_ASP_DEL_W_ADDR     = 95;
    *DSP_AUDIO_ASP_DEL_MARGIN_W_ADDR = 95;
    *DSP_AUDIO_DEC_FRAME_LENGTH = 0;
    *DSP_AUDIO_SD_ISR_CYCLE    = *DSP_AUDIO_SD_ISR_CYCLE;
    *DSP_ASP_FORCE_KT_8K = 0x8000;
    VBI_END();
    return;
}

void audio_playback_on(uint16_t active_type, uint16_t sample_rate)
{
    afe_audio_digital_on(sample_rate);
    audio_dsp_playback_on(active_type, sample_rate);
    afe_switch_tx_signal_path();
    if (active_type != ASP_TYPE_I2SBypass) {
        afe_audio_device_volume_update();
    }
}

void audio_playback_off(void)
{
    afe_audio_analog_device_off();
    audio_dsp_playback_off();
    afe_audio_digital_off();
}

uint16_t audio_get_id(void)
{
    uint16_t i = 0;
    for (i = 0; i < MAX_AUDIO_FUNCTIONS; i++) {
        if ((audio_isr.audio_id & (1 << i)) == 0) {
            audio_isr.audio_id |= (1 << i);
            break;
        }
    }
    return i;
}

void audio_free_id(uint16_t audio_id)
{
    audio_isr.audio_id &= ~(1 << audio_id);
}

void audio_service_wake_dsp(bool flag)
{
    if (flag) {
        afe_restore_register(); /*restore AFE register*/
        hal_dsp_topsm_get_resource(RM_MODEM_DSP_3, true);
        dsp_audio_fw_init();
        audio_service_dsp_wakeup_setting(flag);
    } else {
        audio_service_dsp_wakeup_setting(flag);
        hal_dsp_topsm_get_resource(RM_MODEM_DSP_3, false);
        afe_backup_register(); /*Backup AFE register*/
        audio_service_clean_dsp_wakeup_event();
    }
}

void audio_service_setflag(uint16_t audio_id)
{
    uint32_t savedmask;
    if (AUDIO_GET_CHIP_ID() == 0) {     /*E1 chip*/
        pmu_ctrl_va28_ldo(1);
    }
    if (audio_common.running_state == 0) {
#ifdef HAL_DVFS_MODULE_ENABLED
        audio_register_dsp_dvfs(true);
#endif
#ifdef HAL_AUDIO_LOW_POWER_ENABLED
        if (!audio_lowpower_check_status())
#endif
        {
            audio_common.sleep_handle = hal_sleep_manager_set_sleep_handle("audio");
            hal_sleep_manager_lock_sleep(audio_common.sleep_handle);
        }
        audio_service_wake_dsp(true);
    }
    *(volatile uint16_t *)(0x82255D5E) = 0x0000;  //temp solution to clear speech debug sheriff
    savedmask = save_and_set_interrupt_mask();
    audio_common.running_state |= (1 << audio_id);
    restore_interrupt_mask(savedmask);
}

void audio_service_clearflag(uint16_t audio_id)
{
    uint32_t savedmask;
    savedmask = save_and_set_interrupt_mask();
    audio_common.running_state &= ~(1 << audio_id);
    restore_interrupt_mask(savedmask);
    if (audio_common.running_state == 0 ) {
#ifdef HAL_AUDIO_LOW_POWER_ENABLED
        if (!audio_lowpower_check_status())
#endif
        {
            hal_sleep_manager_unlock_sleep(audio_common.sleep_handle);
            hal_sleep_manager_release_sleep_handle(audio_common.sleep_handle);
        }
        audio_service_wake_dsp(false);
#ifdef HAL_DVFS_MODULE_ENABLED
        audio_register_dsp_dvfs(false);
#endif
        if (AUDIO_GET_CHIP_ID() == 0) {     /*E1 chip*/
            pmu_ctrl_va28_ldo(0);
        }
    }
}

void audio_clear_dsp_common_flag(void)
{
    *DSP_AUDIO_ASP_COMMON_FLAG_1 = 0;
}


/*pcm interface and slave i2s */
void audio_turn_on_pcm_interface(bool is_wideband)
{
    // Zero-pending
    *DSP_SPH_BT_CTRL = 0x0002;
    *DSP_SPH_BT_MODE = 0;

    *DSP_SPH_BT_CTRL &= ~(0x0010);   // MSB first
    *DSP_SPH_BT_CTRL |= (0x100 & 0xff00); //0x100 is UL default setting 0dB

    *DSP_SPH_8K_CTRL = (*DSP_SPH_8K_CTRL & 0x00FF) | 0x2000;
    if (is_wideband) {
        *DSP_SPH_8K_CTRL |= (1 << 10);
    } else {
        *DSP_SPH_8K_CTRL &= ~(1 << 10);
    }
}

void audio_turn_off_pcm_interface()
{
    *DSP_SPH_8K_CTRL = (*DSP_SPH_8K_CTRL & ~0x2000) | 0x8000;
}

/*idma read/write common functions */
void audio_idma_read_from_dsp(uint16_t *dst, volatile uint16_t *src, uint32_t length)
{
    uint32_t i;
    for (i = length; i > 0; i--) {
        *dst++ = *src++;
    }
}

void audio_idma_read_from_dsp_dropR(uint16_t *dst, volatile uint16_t *src, uint32_t length)
{
    uint32_t i;
    for (i = length; i > 0; i--) {
        *dst++ = *src;
        src += 2;
    }
}

void audio_idma_write_to_dsp(volatile uint16_t *dst, uint16_t *src, uint32_t length)
{
    uint32_t i;
    for (i = length; i > 0; i--) {
        *dst++ = *src++;
    }
}

void audio_idma_write_to_dsp_duplicate(volatile uint16_t *dst, uint16_t *src, uint32_t length)
{
    uint32_t i;

    for (i = length; i > 0; i -= 2) {
        *dst++ = *src;
        *dst++ = *src++;
    }
}

void audio_idma_fill_to_dsp(volatile uint16_t *dst, uint16_t value, uint32_t length)
{
    uint32_t i;

    for (i = length; i > 0; i--) {
        *dst++ = value;
    }
}
/*@brief     circular buffer(ring buffer) implemented by mirroring, which keep an extra bit to distinguish empty and full situation. */
uint32_t ring_buffer_get_data_byte_count (ring_buffer_information_t *p_info)
{
    uint32_t buffer_byte_count = p_info->buffer_byte_count;
    uint32_t write_pointer     = p_info->write_pointer;
    uint32_t read_pointer      = p_info->read_pointer;
    uint32_t data_byte_count;
    if (write_pointer >= read_pointer) {
        data_byte_count = write_pointer - read_pointer;
    } else {
        data_byte_count = (buffer_byte_count << 1) - read_pointer + write_pointer;
    }
    return data_byte_count;
}

uint32_t ring_buffer_get_space_byte_count (ring_buffer_information_t *p_info)
{
    return p_info->buffer_byte_count - ring_buffer_get_data_byte_count(p_info);
}

void ring_buffer_get_write_information (ring_buffer_information_t *p_info, uint8_t **pp_buffer, uint32_t *p_byte_count)
{
    uint32_t buffer_byte_count = p_info->buffer_byte_count;
    uint32_t space_byte_count  = ring_buffer_get_space_byte_count(p_info);
    uint8_t *buffer_pointer    = p_info->buffer_base_pointer;
    uint32_t write_pointer     = p_info->write_pointer;
    uint32_t tail_byte_count;
    if (write_pointer < buffer_byte_count) {
        *pp_buffer = buffer_pointer + write_pointer;
        tail_byte_count = buffer_byte_count - write_pointer;
    } else {
        *pp_buffer = buffer_pointer + write_pointer - buffer_byte_count;
        tail_byte_count = (buffer_byte_count << 1) - write_pointer;
    }
    *p_byte_count = MINIMUM(space_byte_count, tail_byte_count);
    return;
}

void ring_buffer_get_read_information (ring_buffer_information_t *p_info, uint8_t **pp_buffer, uint32_t *p_byte_count)
{
    uint32_t buffer_byte_count = p_info->buffer_byte_count;
    uint32_t data_byte_count   = ring_buffer_get_data_byte_count(p_info);
    uint8_t *buffer_pointer    = p_info->buffer_base_pointer;
    uint32_t read_pointer      = p_info->read_pointer;
    uint32_t tail_byte_count;
    if (read_pointer < buffer_byte_count) {
        *pp_buffer = buffer_pointer + read_pointer;
        tail_byte_count = buffer_byte_count - read_pointer;
    } else {
        *pp_buffer = buffer_pointer + read_pointer - buffer_byte_count;
        tail_byte_count = (buffer_byte_count << 1) - read_pointer;
    }
    *p_byte_count = MINIMUM(data_byte_count, tail_byte_count);
    return;
}

void ring_buffer_write_done (ring_buffer_information_t *p_info, uint32_t write_byte_count)
{
    uint32_t buffer_byte_count = p_info->buffer_byte_count;
    uint32_t buffer_end        = buffer_byte_count << 1;
    uint32_t write_pointer     = p_info->write_pointer + write_byte_count;
    p_info->write_pointer = write_pointer >= buffer_end ? write_pointer - buffer_end : write_pointer;
    return;
}

void ring_buffer_read_done (ring_buffer_information_t *p_info, uint32_t read_byte_count)
{
    uint32_t buffer_byte_count = p_info->buffer_byte_count;
    uint32_t buffer_end        = buffer_byte_count << 1;
    uint32_t read_pointer      = p_info->read_pointer + read_byte_count;
    p_info->read_pointer = read_pointer >= buffer_end ? read_pointer - buffer_end : read_pointer;
    return;
}

#ifdef MTK_BT_A2DP_AAC_ENABLE
/*@brief     circular buffer(ring buffer) implemented by keeping one slot open. Full buffer has at most (size - 1) slots. */
uint32_t ring_buffer_get_data_byte_count_non_mirroring (ring_buffer_information_t *p_info)
{
    uint32_t buffer_byte_count = p_info->buffer_byte_count;
    uint32_t write_pointer     = p_info->write_pointer;
    uint32_t read_pointer      = p_info->read_pointer;
    uint32_t data_byte_count;

    if (write_pointer >= read_pointer) {
        data_byte_count = write_pointer - read_pointer;
    } else {
        data_byte_count = buffer_byte_count - read_pointer + write_pointer;
    }
    return data_byte_count;
}

uint32_t ring_buffer_get_space_byte_count_non_mirroring (ring_buffer_information_t *p_info)
{
    return p_info->buffer_byte_count - ring_buffer_get_data_byte_count_non_mirroring(p_info);
}

void ring_buffer_get_write_information_non_mirroring (ring_buffer_information_t *p_info, uint8_t **pp_buffer, uint32_t *p_byte_count)
{
    uint32_t buffer_byte_count = p_info->buffer_byte_count;     //buffer size
    uint32_t space_byte_count  = ring_buffer_get_space_byte_count_non_mirroring(p_info) - 2;  //space two bytes(one word) empty for DSP operation
    uint8_t *buffer_pointer    = p_info->buffer_base_pointer;
    uint32_t write_pointer     = p_info->write_pointer;
    uint32_t tail_byte_count;

    tail_byte_count = buffer_byte_count - write_pointer;
    *pp_buffer = buffer_pointer + write_pointer;
    *p_byte_count = MINIMUM(space_byte_count, tail_byte_count);
    return;
}

void ring_buffer_get_read_information_non_mirroring(ring_buffer_information_t *p_info, uint8_t **pp_buffer, uint32_t *p_byte_count)
{
    uint32_t buffer_byte_count = p_info->buffer_byte_count;
    uint32_t data_byte_count   = ring_buffer_get_data_byte_count_non_mirroring(p_info);
    uint8_t *buffer_pointer    = p_info->buffer_base_pointer;
    uint32_t read_pointer      = p_info->read_pointer;
    uint32_t tail_byte_count;

    *pp_buffer = buffer_pointer + read_pointer;
    tail_byte_count = buffer_byte_count - read_pointer;
    *p_byte_count = MINIMUM(data_byte_count, tail_byte_count);
    return;
}


void ring_buffer_write_done_non_mirroring(ring_buffer_information_t *p_info, uint32_t write_byte_count)
{
    uint32_t buffer_byte_count = p_info->buffer_byte_count;
    uint32_t write_pointer     = p_info->write_pointer + write_byte_count;
    p_info->write_pointer = write_pointer == buffer_byte_count ? write_pointer - buffer_byte_count : write_pointer;
    return;
}

void ring_buffer_read_done_non_mirroring(ring_buffer_information_t *p_info, uint32_t read_byte_count)
{
    uint32_t buffer_byte_count = p_info->buffer_byte_count;
    uint32_t read_pointer      = p_info->read_pointer + read_byte_count;
    p_info->read_pointer = read_pointer == buffer_byte_count ? read_pointer - buffer_byte_count : read_pointer;
    return;
}
#endif /*MTK_BT_A2DP_AAC_ENABLE*/

#endif /* defined(HAL_I2S_MODULE_ENABLED) || defined(HAL_AUDIO_MODULE_ENABLED) */
