#include "hal_audio_fw_slt.h"
#include "hal_audio_fw_slt_internal.h"

uint32_t dsp_slt_step     = 0;
uint16_t dsp_slt_checksum = 0;

int32_t dsp_audio_fw_slt_entry(void)
{
    int32_t result = DSP_AUDIO_FW_SLT_PASS;
    {   /* Check DSP HW patch */
        dsp_slt_step = 1;
        dsp_audio_fw_slt_process();                            /* Test DSP HW patch entries */
        dsp_slt_step = 2;
        while (*DSP_SLT_READY != SLT_READY_MAGIC_WORD) ;    /* Busy wait loop            */
        dsp_slt_step = 3;
        dsp_slt_checksum = *DSP_SLT_CHECKSUM;
        if (dsp_slt_checksum != SLT_CHECKSUM_ANSWER) {
            result = DSP_AUDIO_FW_SLT_FAIL;
        }
        dsp_slt_step = 4;
        dsp_audio_fw_lib_init();                               /* Recover DSP HW patch      */
        dsp_slt_step = 5;
    }
    {   /* PM RAM Write */
        uint16_t test_val = 0;
        volatile uint16_t *p_dst;
        uint32_t loop_idx;
        p_dst = DSP_PM_3_RAM_ADDRESS;
        dsp_slt_step = 6;
        for (loop_idx = 0; loop_idx < DSP_PM_3_RAM_LENGTH; loop_idx++) {
            *p_dst++ = test_val++;
        }
        dsp_slt_step = 7;
        p_dst = DSP_PM_F_RAM_ADDRESS;
        for (loop_idx = 0; loop_idx < DSP_PM_F_RAM_LENGTH; loop_idx++) {
            *p_dst++ = test_val++;
        }
        dsp_slt_step = 8;
    }
    {   /* PM RAM Read & Check */
        uint32_t error_count = 0;
        uint16_t test_val = 0;
        volatile uint16_t *p_src;
        uint32_t loop_idx;
        p_src = DSP_PM_3_RAM_ADDRESS;
        dsp_slt_step = 9;
        for (loop_idx = 0; loop_idx < DSP_PM_3_RAM_LENGTH; loop_idx++) {
            if (*p_src++ != test_val++) {
                error_count++;
            }
        }
        dsp_slt_step = 10;
        p_src = DSP_PM_F_RAM_ADDRESS;
        for (loop_idx = 0; loop_idx < DSP_PM_F_RAM_LENGTH; loop_idx++) {
            if (*p_src++ != test_val++) {
                error_count++;
            }
        }
        dsp_slt_step = 11;
        if (error_count != 0) {
            result = DSP_AUDIO_FW_SLT_FAIL;
        }
    }
    {   /* PM RAM Clear */
        volatile uint16_t *p_dst;
        uint32_t loop_idx;
        p_dst = DSP_PM_3_RAM_ADDRESS;
        dsp_slt_step = 12;
        for (loop_idx = 0; loop_idx < DSP_PM_3_RAM_LENGTH; loop_idx++) {
            *p_dst++ = 0;
        }
        dsp_slt_step = 13;
        p_dst = DSP_PM_F_RAM_ADDRESS;
        for (loop_idx = 0; loop_idx < DSP_PM_F_RAM_LENGTH; loop_idx++) {
            *p_dst++ = 0;
        }
        dsp_slt_step = 14;
    }
    return result;
}
