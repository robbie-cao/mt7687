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

#ifndef __HAL_AUDIO_INTERNAL_SERVICE_H__
#define __HAL_AUDIO_INTERNAL_SERVICE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "hal_dvfs_internal.h"
#include "hal_audio_post_processing_internal.h"


#ifndef NULL
#define NULL 0
#endif

#define UPPER_BOUND(in,up)      ((in) > (up) ? (up) : (in))
#define LOWER_BOUND(in,lo)      ((in) < (lo) ? (lo) : (in))
#define BOUNDED(in,up,lo)       ((in) > (up) ? (up) : (in) <= (lo) ? (lo) : (in))
#define MAXIMUM(a,b)            ((a) > (b) ? (a) : (b))
#define MINIMUM(a,b)            ((a) < (b) ? (a) : (b))
#define FOUR_BYTE_ALIGNED(size) (((size) + 3) & ~0x3)

typedef void (*isr)(void *user_data);
#define  MAXISR 6
#define  MAX_AUDIO_FUNCTIONS 16

#define  VBI_RESET() SHERIF_WRITE(SHARE_M2DI1,0x0838)
#define  VBI_END()   SHERIF_WRITE(SHARE_M2DI1,0x0839)
#define  AUDIO_GET_CHIP_ID() ((volatile uint16_t *)0xA2000000)
typedef enum {
    AUDIO_COMMON_SAMPLING_RATE_8KHZ      = 0, /**< 8000Hz  */
    AUDIO_COMMON_SAMPLING_RATE_11_025KHZ = 1, /**< 11025Hz */
    AUDIO_COMMON_SAMPLING_RATE_12KHZ     = 2, /**< 12000Hz */
    AUDIO_COMMON_SAMPLING_RATE_16KHZ     = 3, /**< 16000Hz */
    AUDIO_COMMON_SAMPLING_RATE_22_05KHZ  = 4, /**< 22050Hz */
    AUDIO_COMMON_SAMPLING_RATE_24KHZ     = 5, /**< 24000Hz */
    AUDIO_COMMON_SAMPLING_RATE_32KHZ     = 6, /**< 32000Hz */
    AUDIO_COMMON_SAMPLING_RATE_44_1KHZ   = 7, /**< 44100Hz */
    AUDIO_COMMON_SAMPLING_RATE_48KHZ     = 8, /**< 48000Hz */
    AUDIO_COMMON_SAMPLING_RATE_96KHZ     = 9  /**< 96000Hz */
} common_sampling_rate_t;

typedef enum {
    SPH_VOICE_NONE   = 0,
    SPH_VOICE_RECORD ,
    SPH_VOICE_PCM2WAY_UL,
    SPH_VOICE_PCM2WAY_DL,
    SPH_BOTH_PATH = 0xFFFF
} speech_path_type_t;

typedef enum {
    AUDIO_DSP_ASSERT_NONE = 0,
    AUDIO_DSP_ASSERT_FROM_MCU,
    AUDIO_DSP_ASSERT_FROM_DSP
} audio_dsp_assert_source_t;

typedef struct {
    uint16_t audio_id;
    uint16_t num[MAXISR];
    isr function[MAXISR];
    void *userdata[MAXISR];
} audio_isr_t;

typedef struct {
    uint32_t                    running_state;
    uint8_t                     sleep_handle;
    audio_dsp_assert_source_t   assert_source;
    uint16_t                    active_type;
#if defined(MTK_AUDIO_IIR_FILTER_ENABLE)
    int32_t                     apply_iir;
#endif
} audio_common_t;

typedef struct {
    uint32_t write_pointer;
    uint32_t read_pointer;
    uint32_t buffer_byte_count;
    uint8_t *buffer_base_pointer;
} ring_buffer_information_t;

/*DSP to MCU interrupt ID*/
#define IO4567_D2C_ID_SPEECH_BASE        1          //1~30
#define IO4567_D2C_ID_AUDIO_BASE        31          //31~60
#define IO4567_D2C_ID_OTHER_BASE        61          //61~63
#define PSEUDO_SAL_DSPINT_PRIO_REC_VMEPL 1
#define PSEUDO_SAL_DSPINT_PRIO_REC_PCM  2
#define PSEUDO_SAL_DSPINT_PRIO_REC_VM   3
#define PSEUDO_SAL_DSPINT_PRIO_PNW_DL   6
#define PSEUDO_SAL_DSPINT_PRIO_PNW_UL   7
#define DSP_IID_SPEECH_UL_ID            1
#define DSP_IID_SPEECH_DL_ID            2
#define PSEUDO_SAL_DSPINT_PRIO_MAX      13

#define PSEUDO_SAL_DSPINT_ID_REC_EPL    (1 << 1)
#define PSEUDO_SAL_DSPINT_ID_REC_PCM    (1 << 2)
#define PSEUDO_SAL_DSPINT_ID_REC_VM     (1 << 3)
#define PSEUDO_SAL_DSPINT_ID_PNW_DL     (1 << 6)
#define PSEUDO_SAL_DSPINT_ID_PNW_UL     (1 << 7)

#define DSP_SD_DONE                   (IO4567_D2C_ID_SPEECH_BASE+3) /*IDLE mode playback  */ /* SD1+SD2 is done, MCU can WRITE data TO SHERIF */
#define DSP_D2M_PCM_INT               (IO4567_D2C_ID_SPEECH_BASE+4) /*PCM to MCU interrup*/
#define DSP_INT_MAPPING_BASIC         (IO4567_D2C_ID_SPEECH_BASE+5) /*Speech Driver Mapping which can be changed. It is independent of SpeechDSP's setting(D2C_Status_80.h)*/
#define DSP_D2M_PCM_ROUTE_INT         (IO4567_D2C_ID_AUDIO_BASE+1)  /*PCM Route D2M interrupt*/
#define DSP_D2C_AAC_DEC_INT           (IO4567_D2C_ID_AUDIO_BASE+3)  /*AAC Decoder D2M interrupt*/
#define DSP_D2C_SBC_DEC_INT           (IO4567_D2C_ID_AUDIO_BASE+11) /*SBC Decoder D2M Interrupt*/
#define DSP_D2C_PCM_SD_PLAYBACK_INT   (IO4567_D2C_ID_AUDIO_BASE+12) /* PCM SD Playback D2M Interrupt */
#define DSP_DSP_EXCEPTION_ID          (IO4567_D2C_ID_OTHER_BASE+1)  /*DSP dead interrupt. 0x003E(DD44)*/
#define DSP_VMEPL_REC_INT               (DSP_INT_MAPPING_BASIC+PSEUDO_SAL_DSPINT_PRIO_REC_VMEPL)  /*8 PCMRECORF 8k*/
#define DSP_PCM_REC_INT               (DSP_INT_MAPPING_BASIC+PSEUDO_SAL_DSPINT_PRIO_REC_PCM)  /*8 PCMRECORF 8k*/
#define DSP_VM_REC_INT                (DSP_INT_MAPPING_BASIC+PSEUDO_SAL_DSPINT_PRIO_REC_VM)   /*9 PCMRECORF 16k*/
#define DSP_PCM_EX_DL                 (DSP_INT_MAPPING_BASIC+PSEUDO_SAL_DSPINT_PRIO_PNW_DL)   /*12 PCMNWAY for DL*/
#define DSP_PCM_EX_UL                 (DSP_INT_MAPPING_BASIC+PSEUDO_SAL_DSPINT_PRIO_PNW_UL)   /*13 PCMNWAY for UL*/

/*Delay margin definition */
#define  ASP_DELM_PCM_HIGFS   576
#define  ASP_DELM_PCM_MEDFS   288
#define  ASP_DELM_PCM_LOWFS   144
#define  ASP_DELM_I2SBypass   1020
#define  ASP_DELM_SBC_DEC     2048
#define  ASP_DELM_AAC_DEC     2048

/*DSP frame length*/
#define  ASP_FRAMELEN_PCM     0x2000
#define  ASP_FRAMELEN_I2S     ASP_DELM_I2SBypass /*1020*/
#define  ASP_FRAMELEN_SBC_DEC ASP_DELM_SBC_DEC
#define  ASP_FRAMELEN_AAC_DEC ASP_DELM_AAC_DEC

/* Audio type */
#define  ASP_TYPE_VOICE             0
#define  ASP_TYPE_PCM_HI            1
#define  ASP_TYPE_I2SBypass         5
#define  ASP_TYPE_SBC_DEC           10
#define  ASP_TYPE_PCM_SD_PLAYBACK   11
#define  ASP_TYPE_AAC_DEC           32

/* Speech WB/NB buffer size */
#define  NB_BUFFER_SIZE       160 /*narrow band 20ms * 8k*/
#define  WB_BUFFER_SIZE       320 /*width band 20ms * 16k*/
/*Frequency mapping for DSP */
#define  ASP_FS_8K            0x00
#define  ASP_FS_11K           0x11
#define  ASP_FS_12K           0x12
#define  ASP_FS_16K           0x20
#define  ASP_FS_22K           0x21
#define  ASP_FS_24K           0x22
#define  ASP_FS_32K           0x40
#define  ASP_FS_44K           0x41
#define  ASP_FS_48K           0x42
#define  ASP_FS_96K           0x43

/* Flexible control bit map, DSP_AUDIO_FLEXI_CTRL*/
#define  FLEXI_VBI_ENABLE     0x0001
#define  FLEXI_SE_ENABLE      0x0002
#define  FLEXI_SD_ENABLE      0x0004

/* DSP_AUDIO_CTRL2 bit map */
#define  DSP_AAC_CTRL_ASP     0x0001
#define  DSP_PCM_R_DIS        0x0002

#define AUD_BIT_COMFLG1_DSPMEM_NEED_CACHE_INVALID               0
#define AUD_BIT_COMFLG1_AUDLP_ENABLE                            1

#define AUDIO_DSP_PM_PAGE_COUNT 2
#define AUDIO_DSP_DM_PAGE_COUNT 3

/* DVFS CPU frequency */
#define AUDIO_CPU_FREQ_L_BOUND 104000

/*Functions prototype*/
void audio_playback_on(uint16_t active_type, uint16_t sample_rate);
void audio_playback_off(void);
void audio_service_hook_isr(uint16_t isrtype, isr function, void *user_data);
void audio_service_unhook_isr(uint16_t isr_type);
void audio_service_dsp_to_mcu_interrupt(void);
uint16_t audio_get_id(void);
void audio_free_id(uint16_t audio_id);
void audio_service_setflag(uint16_t audio_id);
void audio_service_clearflag(uint16_t audio_id);
void audio_dsp_write_anti_alias_filter(void);
void audio_dsp_reload_coeficient(void);
void audio_dsp_speech_set_delay(uint8_t type);
void audio_clear_dsp_common_flag(void);
uint16_t audio_transfer_sample_rate(uint16_t sample_rate);
uint8_t audio_transfer_channel_number(uint16_t channel);

extern uint32_t save_and_set_interrupt_mask(void);
extern void restore_interrupt_mask(uint32_t mask);

extern void platform_assert(const char *, const char *, int);
#define configASSERT( x ) if( (x) == 0 ) { platform_assert(#x, __FILE__, __LINE__); }
void audio_service_dsp_memory_dump_init (void);

void audio_turn_on_pcm_interface(bool is_wideband);
void audio_turn_off_pcm_interface(void);

/*IDMA part*/
void audio_idma_read_from_dsp(uint16_t *dst, volatile uint16_t *src, uint32_t length);
void audio_idma_read_from_dsp_dropR(uint16_t *dst, volatile uint16_t *src, uint32_t length);
void audio_idma_write_to_dsp(volatile uint16_t *dst, uint16_t *src, uint32_t length);
void audio_idma_write_to_dsp_duplicate(volatile uint16_t *dst, uint16_t *src, uint32_t length);
void audio_idma_fill_to_dsp(volatile uint16_t *dst, uint16_t value, uint32_t length);

/*
Function Name:
    ring_buffer_get_data_byte_count

Return Type:
    uint32_t

Parameters:
    ring_buffer_information_t *p_info

Function Content with Full Assertion Protection:
    uint32_t buffer_byte_count;
    uint32_t write_pointer;
    uint32_t read_pointer;
    uint32_t data_byte_count;
    // ASSERT_IF_FALSE(p_info != NULL);
    buffer_byte_count = p_info->buffer_byte_count;  // ASSERT_IF_FALSE(buffer_byte_count < 0x80000000);
    write_pointer     = p_info->write_pointer;      // ASSERT_IF_FALSE(write_pointer < (buffer_byte_count << 1));
    read_pointer      = p_info->read_pointer;       // ASSERT_IF_FALSE(read_pointer < (buffer_byte_count << 1));
    if (write_pointer >= read_pointer) {
        data_byte_count = write_pointer - read_pointer;
    } else { // write_pointer < read_pointer
        data_byte_count = (buffer_byte_count << 1) - read_pointer + write_pointer;
    }
    // ASSERT_IF_FALSE(data_byte_count <= buffer_byte_count);
    return data_byte_count;

Function Name:
    ring_buffer_get_space_byte_count

Return Type:
    uint32_t

Parameters:
    ring_buffer_information_t *p_info

Function Content with Full Assertion Protection:
    uint32_t buffer_byte_count;
    uint32_t data_byte_count;
    uint32_t space_byte_count;
    // ASSERT_IF_FALSE(p_info != NULL);
    buffer_byte_count = p_info->buffer_byte_count;                  // ASSERT_IF_FALSE(buffer_byte_count < 0x80000000);
    data_byte_count   = ring_buffer_get_data_byte_count(p_info);    // ASSERT_IF_FALSE(data_byte_count <= buffer_byte_count);
    space_byte_count  = buffer_byte_count - data_byte_count;
    return space_byte_count;

Function Name:
    ring_buffer_get_write_information

Return Type:
    void

Parameters:
    ring_buffer_information_t *p_info
    uint8_t **pp_buffer
    uint32_t *p_byte_count

Function Content with Full Assertion Protection:
    uint32_t buffer_byte_count;
    uint32_t space_byte_count;
    uint8_t *buffer_pointer;
    uint32_t write_pointer;
    uint32_t read_pointer;
    uint32_t tail_byte_count;
    uint32_t write_byte_cnt;
    // ASSERT_IF_FALSE(p_info != NULL);
    // ASSERT_IF_FALSE(pp_buffer != NULL);
    // ASSERT_IF_FALSE(p_byte_count != NULL);
    buffer_byte_count = p_info->buffer_byte_count;                  // ASSERT_IF_FALSE(buffer_byte_count < 0x80000000);
    space_byte_count  = ring_buffer_get_space_byte_count(p_info);   // ASSERT_IF_FALSE(space_byte_count <= buffer_byte_count);
    buffer_pointer    = p_info->buffer_base_pointer;                // ASSERT_IF_FALSE(buffer_pointer != NULL);
    write_pointer     = p_info->write_pointer;                      // ASSERT_IF_FALSE(write_pointer < (buffer_byte_count << 1));
    read_pointer      = p_info->read_pointer;                       // ASSERT_IF_FALSE(read_pointer < (buffer_byte_count << 1));
    if (write_pointer < buffer_byte_count) {    // 1st mirror part
        buffer_pointer += write_pointer;
        tail_byte_count = buffer_byte_count - write_pointer;
    } else {                                    // 2nd mirror part
        buffer_pointer += write_pointer - buffer_byte_count;
        tail_byte_count = (buffer_byte_count << 1) - write_pointer;
    }
    write_byte_cnt = MINIMUM(data_byte_count, tail_byte_count);
    *pp_buffer = buffer_pointer;
    *p_byte_count = write_byte_cnt;
    return;

Function Name:
    ring_buffer_get_read_information

Return Type:
    void

Parameters:
    ring_buffer_information_t *p_info
    uint8_t **pp_buffer
    uint32_t *p_byte_count

Function Content with Full Assertion Protection:
    uint32_t buffer_byte_count;
    uint32_t data_byte_count;
    uint8_t *buffer_pointer;
    uint32_t write_pointer;
    uint32_t read_pointer;
    uint32_t tail_byte_count;
    uint32_t read_byte_cnt;
    // ASSERT_IF_FALSE(p_info != NULL);
    // ASSERT_IF_FALSE(pp_buffer != NULL);
    // ASSERT_IF_FALSE(p_byte_count != NULL);
    buffer_byte_count = p_info->buffer_byte_count;                  // ASSERT_IF_FALSE(buffer_byte_count < 0x80000000);
    data_byte_count   = ring_buffer_get_data_byte_count(p_info);    // ASSERT_IF_FALSE(data_byte_count <= buffer_byte_count);
    buffer_pointer    = p_info->buffer_base_pointer;                // ASSERT_IF_FALSE(buffer_pointer != NULL);
    write_pointer     = p_info->write_pointer;                      // ASSERT_IF_FALSE(write_pointer < (buffer_byte_count << 1));
    read_pointer      = p_info->read_pointer;                       // ASSERT_IF_FALSE(read_pointer < (buffer_byte_count << 1));
    if (read_pointer < buffer_byte_count) { // 1st mirror part
        buffer_pointer += read_pointer;
        tail_byte_count = buffer_byte_count - read_pointer;
    } else {                                // 2nd mirror part
        buffer_pointer += read_pointer - buffer_byte_count;
        tail_byte_count = (buffer_byte_count << 1) - read_pointer;
    }
    read_byte_cnt = MINIMUM(data_byte_count, tail_byte_count);
    *pp_buffer = buffer_pointer;
    *p_byte_count = read_byte_cnt;
    return;

Function Name:
    ring_buffer_write_done

Return Type:
    void

Parameters:
    ring_buffer_information_t *p_info
    uint32_t write_byte_count

Function Content with Full Assertion Protection:
    uint32_t buffer_byte_count;
    uint32_t buffer_end;
    // uint32_t space_byte_count;
    uint32_t write_pointer;
    // uint32_t tail_byte_count
    // ASSERT_IF_FALSE(p_info != NULL);
    buffer_byte_count = p_info->buffer_byte_count;                  // ASSERT_IF_FALSE(buffer_byte_count < 0x80000000);
    buffer_end        = buffer_byte_count << 1;
    // space_byte_count  = ring_buffer_get_space_byte_count(p_info);   // ASSERT_IF_FALSE(space_byte_count <= buffer_byte_count);
    //                                                                 // ASSERT_IF_FALSE(write_byte_count <= space_byte_count);
    write_pointer = p_info->write_pointer; // ASSERT_IF_FALSE(write_pointer < buffer_end);
    // if (write_pointer < buffer_byte_count) { // 1st mirror part
    //     tail_byte_count = buffer_byte_count - write_pointer;
    // } else {                                // 2nd mirror part
    //     tail_byte_count = buffer_end - write_pointer;
    // }
    // ASSERT_IF_FALSE(write_byte_count <= tail_byte_count);
    write_pointer += write_byte_count;
    if (write_pointer >= buffer_end) {
        write_pointer -= buffer_end;
    }
    p_info->write_pointer = write_pointer;
    return;

Function Name:
    ring_buffer_read_done

Return Type:
    void

Parameters:
    ring_buffer_information_t *p_info
    uint32_t read_byte_count

Function Content with Full Assertion Protection:
    uint32_t buffer_byte_count;
    uint32_t buffer_end;
    // uint32_t data_byte_count;
    uint32_t read_pointer;
    // uint32_t tail_byte_count
    // ASSERT_IF_FALSE(p_info != NULL);
    buffer_byte_count = p_info->buffer_byte_count;                  // ASSERT_IF_FALSE(buffer_byte_count < 0x80000000);
    buffer_end        = buffer_byte_count << 1;
    // data_byte_count   = ring_buffer_get_data_byte_count(p_info);    // ASSERT_IF_FALSE(data_byte_count <= buffer_byte_count);
    //                                                                 // ASSERT_IF_FALSE(read_byte_count <= data_byte_count);
    read_pointer = p_info->read_pointer; // ASSERT_IF_FALSE(read_pointer < buffer_end);
    // if (read_pointer < buffer_byte_count) { // 1st mirror part
    //     tail_byte_count = buffer_byte_count - read_pointer;
    // } else {                                // 2nd mirror part
    //     tail_byte_count = buffer_end - read_pointer;
    // }
    // ASSERT_IF_FALSE(read_byte_count <= tail_byte_count);
    read_pointer += read_byte_count;
    if (read_pointer >= buffer_end) {
        read_pointer -= buffer_end;
    }
    p_info->read_pointer = read_pointer;
    return;
*/

uint32_t ring_buffer_get_data_byte_count (ring_buffer_information_t *p_info);
uint32_t ring_buffer_get_space_byte_count (ring_buffer_information_t *p_info);
void ring_buffer_get_write_information (ring_buffer_information_t *p_info, uint8_t **pp_buffer, uint32_t *p_byte_count);
void ring_buffer_get_read_information (ring_buffer_information_t *p_info, uint8_t **pp_buffer, uint32_t *p_byte_count);
void ring_buffer_write_done (ring_buffer_information_t *p_info, uint32_t write_byte_count);
void ring_buffer_read_done (ring_buffer_information_t *p_info, uint32_t read_byte_count);

uint32_t ring_buffer_get_data_byte_count_non_mirroring (ring_buffer_information_t *p_info);
uint32_t ring_buffer_get_space_byte_count_non_mirroring (ring_buffer_information_t *p_info);
void ring_buffer_get_write_information_non_mirroring (ring_buffer_information_t *p_info, uint8_t **pp_buffer, uint32_t *p_byte_count);
void ring_buffer_get_read_information_non_mirroring (ring_buffer_information_t *p_info, uint8_t **pp_buffer, uint32_t *p_byte_count);
void ring_buffer_write_done_non_mirroring (ring_buffer_information_t *p_info, uint32_t write_byte_count);
void ring_buffer_read_done_non_mirroring (ring_buffer_information_t *p_info, uint32_t read_byte_count);

#ifdef __cplusplus
}
#endif

#endif /*__HAL_AUDIO_INTERNAL_SERVICE_H__*/
