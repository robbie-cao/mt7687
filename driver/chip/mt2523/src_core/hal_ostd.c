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

#include "hal_ostd.h"

#ifdef HAL_SLEEP_MANAGER_ENABLED

#include "hal_cm4_topsm.h"
#include "hal_sleep_manager_platform.h"
#include "hal_sleep_manager.h"
#include "hal_log.h"
#include "memory_attribute.h"
#include "hal_nvic.h"
hal_ostd_attribute_t ostd_info;
bool cm4_ostd_check_sleep(void)
{
    volatile uint16_t reg_val2 = 0, reg_val3 = 0;
    volatile uint16_t wait_time = 0;
    uint32_t sleepDisable = 0, sleepDisable_ext = 0;
    uint32_t OST_FRM_NUM = 0;
    if ((sleepDisable != 0) || (sleepDisable_ext != 0)) {
        return false;
    }
    HW_WRITE(CM4_OST_CMD, (CMD_MAGIC_VALUE + 0x00000002));
    do {
        reg_val3 = *CM4_OST_STA;
        wait_time++;
    } while (!(reg_val3 & 0x0002));
    ostd_info.frame_info.curr_afn = *CM4_OST_AFN_R;
    ostd_info.frame_info.curr_ufn = *CM4_OST_UFN_R;
    ostd_info.frame_info.curr_afn_dly = *CM4_OST_AFN_DLY;
    OST_FRM_NUM = (*CM4_OST_FRM_F32K >> 12) & 0xF;
    (void)OST_FRM_NUM;
//	    /* Trigger Pause Request */
    wait_time = 0;
    /* Then, check if OST_STA.PAUSE_REQ == "11" or "10". */
    reg_val2 = HW_READ(CM4_OST_STA);

    /* Failed to trigger OST PAUSE mode due to UFN < 2 */
    if ((reg_val2 & 0x0018) == 0x0018) {
        return false;
    }
    /* Failed to trigger OST PAUSE mode due to wake-up event happened before activate CP15. (i.e. Sleep Abort) */
    else if ((reg_val2 & 0x0010) == 0x0010) {
        return false;
    }
    /* Successfully trigger OST HW ready to sleep */
    else { /* OST_STA.PAUSE_REQ = "00" */
    }
    ostd_info.sleeped = true;
    ostd_info.slept_cnt++;

    return true;
}

void cm4_ostd_set_frame_duration(uint32_t frame_dur)
{
    uint32_t tmp_frm_f32k = (frame_dur * 8) / (245);/*30.5176*8*/
    uint32_t ost_frm_num = 0;
    uint32_t target_ost_frm_f32k = 0; /*bit8~0 in OST_FRM_F32K*/
    uint32_t register_ost_frm_f32k = 0;
    uint32_t settle_time;
    settle_time = CM4_OSTD_SYSCLK_SETTLE + CM4_OSTD_PLL_SETTLE;
    ost_frm_num = ((((settle_time + (tmp_frm_f32k - 1)) / tmp_frm_f32k) + 1) > 2 ? (((settle_time + (tmp_frm_f32k - 1)) / tmp_frm_f32k) + 1) : 2);
    target_ost_frm_f32k = (ost_frm_num * tmp_frm_f32k) - 5;
#ifndef duration_1ms
    register_ost_frm_f32k = (ost_frm_num << 12) + (target_ost_frm_f32k);
    *CM4_TOPSM_RM_CLK_SETTLE = 0x050f0013;
#else
    register_ost_frm_f32k = (0x6 << 12) + 0x24 * CM4_TOPSM_RM_CLK_SETTLE = 0x050f0010;
#endif
    *CM4_OST_FRM_F32K = register_ost_frm_f32k;
    *CM4_OST_FRM = frame_dur;
}

bool cm4_ostd_check_in_settle_state(void)
{
    uint32_t OST_FRM_NUM = 0;
    volatile uint32_t start_us;
    ostd_info.pre_wfi_frc = *CM4_TOPSM_FRC_VAL_R;
    OST_FRM_NUM = (*CM4_OST_FRM_F32K >> 12) & 0xF;
    if (ostd_info.frame_info.curr_ufn <= OST_FRM_NUM) {
        cm4_ostd_set_ufn(OST_FRM_NUM - 1);
        start_us = *CM4_TOPSM_FRC_VAL_R;
        (void)start_us;
        ostd_info.EnterSettleState = true;
        return true;
    }
    return false;
}

OSTD_RESULT_Enum cm4_ostd_set_afn(uint32_t afn)
{
    uint16_t reg_val = 0;
    uint16_t wait_time = 0;
    uint32_t curr_afn = 0;
    /*Trigger HW Read command*/
    *CM4_OST_CMD = (0x11530000 + 0x00000002);
    do {
        reg_val = *CM4_OST_STA;
        wait_time++;
    } while (!(reg_val & 0x0002));
    curr_afn = *CM4_OST_AFN_R;
    (void)curr_afn;
    {
        /*Set AFN value to HW*/
        *CM4_OST_AFN = afn;
        do {
            reg_val = *CM4_OST_STA;
            wait_time++;
        } while (!(reg_val & 0x0002));
        return OSTD_SUCCESS_TRUE;
    }

}

OSTD_RESULT_Enum cm4_ostd_set_ufn(uint32_t ufn)
{
    uint16_t reg_val = 0;
    uint16_t wait_time = 0;
    uint32_t curr_ufn = 0;

    *CM4_OST_CMD = (0x11530000 + 0x00000002);
    do {
        reg_val = *CM4_OST_STA;
        wait_time++;
    } while (!(reg_val & 0x0002));
    curr_ufn = *CM4_OST_UFN_R;
    (void)curr_ufn;
    if ((*CM4_OST_CON & 0x2) == 0) {
        return false;
    } else {
        /*ost should be in Normal State Set UFN value to HW*/
        *CM4_OST_UFN = ufn;
        wait_time = 0;
        *CM4_OST_CMD = (0x11530000 + 0x00002004);
        do {
            reg_val = *CM4_OST_STA;
            wait_time++;
        } while (!(reg_val & 0x0002));
        return OSTD_SUCCESS_TRUE;
    }
}

ATTR_TEXT_IN_TCM void cm4_ostd_enter_sleep_cmsys_memory_sleep(uint32_t Target_Sleep_Frames)
{
    uint16_t reg_val1 = 0, reg_val2 = 0, reg_val3 = 0;
    uint16_t wait_time = 0;
    uint32_t Set_AFN = 0, Set_UFN = 0;

//	/*Set AFN UFN before sleep*/
    Set_AFN = Set_UFN = Target_Sleep_Frames;
    cm4_ostd_set_afn(Set_AFN);
    cm4_ostd_set_ufn(Set_UFN);

    *CM4_OST_INT_MASK = (~(0x001F)) & 0x1F; //turn on AFN ,UFN,PauseAbort and PauseINT ,and also Frames Tick
    /*Trigger HW Read commnad*/
    *CM4_OST_CMD = (0x11530000 + 0x00000002);
    do {
        reg_val3 = *CM4_OST_STA;
        wait_time++;
    } while (!(reg_val3 & 0x0002));
    /*Trigger Pause Request*/
    wait_time = 0;
    *CM4_OST_CMD = (0x11530000 + 0x00000001);
    /*First , Polling until OST_STA.CMD_CPL = 1 (i.e the ACK of this Pause Request)*/
    do {
        reg_val1 = *CM4_OST_STA;
        wait_time++;
    } while (!(reg_val1 & 0x0002));
    /*Then , check if OST_STA.PAUSE_REQ "11" or "10" */
    reg_val2 = *CM4_OST_STA;
    //ostd.FRC_Pre =HW_READ(FRC_VAL_R);
    if ((reg_val2 & 0x0018) == 0x0018) {
        //printf("Failed to trigger ost pause mode due to FUN <2");
    } else if ((reg_val2 & 0x0010) == 0x0010) {
        //printf("Failed to trigger ost pause mode due to wake up event happend before activate cp15 ex: sleep abort");
    } else { /*OST_STA.PAUSE_REQ = "00" */
        //TINF0 = "Successfully trigger OST HW ready to sleep"
    }
}

/*Enable OS timer and check if busy*/
void cm4_ostd_enable_ost(uint32_t enable)
{
    ////printf("cm4_ostd_enable_ost enable:%d \n",enable);
    uint16_t reg_val = 0;
    uint16_t wait_time = 0;
    uint16_t tmp = 0;
    tmp = *CM4_OST_CON;
    /*set 1:Always enable UFN down-count feature in ARM os TImer Driver*/
    tmp |= 0x2;
    /*set 0:Disable OST DBG mode*/
    tmp &= ~(0x4);
    /*Enable /Disable OS Timer */
    if (enable == 0x1) {
        tmp |= 0x1;    /*The FRC should be enabled before Enable OST*/
    } else {
        tmp &= ~(0x1);
    }

    *CM4_OST_CON = tmp;
    *CM4_OST_CMD = (0x11530000 + 0x0000E004);
    do {
        reg_val = *CM4_OST_STA;
        wait_time++;
    } while (!(reg_val & 0x0002));
}

void cm4_ostd_get_currfrm(hal_ostd_info_t *pfrm_Info)
{
    *pfrm_Info = ostd_info.frame_info;
}

void cm4_ostd_mask_all_irq(void)
{
    *CM4_OST_EVENT_MASK = 0xFFFFFFFF;

}

void cm4_ostd_unmask_all_irq(void)
{
    *CM4_OST_EVENT_MASK = 0x40;
}

bool cm4_ostd_set_one_irq_enable(hal_sleep_manager_wakeup_source_t index)
{
    *CM4_OST_EVENT_MASK = *CM4_OST_EVENT_MASK & ~(1 << index);
    return true;
}

bool cm4_ostd_set_one_irq_disable(hal_sleep_manager_wakeup_source_t index)
{
    *CM4_OST_EVENT_MASK = *CM4_OST_EVENT_MASK | (1 << index);
    return true;
}
void cm4_ostd_init()
{
    *CM4_OST_INT_MASK = (~(0x001E)) & 0x1F; /*unmask AFE timeout ,UFN timeout , pasuse abort, and pause interrupt*/
    *CM4_OST_DBG_WAKEUP = 0x80000000; /*Cirq mask enable*/
    //cm4_ostd_mask_all_irq();
    cm4_ostd_unmask_all_irq();

    hal_nvic_register_isr_handler(OSTimer_IRQn, (hal_nvic_isr_t)ostimer_irq_callback);
    cm4_topsm_set_sleep_timeframe(SET_F32K_4096_MAX_DELAY_MS,255, 4095);
}

uint32_t cm4_ostd_get_scale(void)
{
    uint32_t scale = 1;
    switch (*CM4_TOPSM_FRC_F32K_FM) {
        case SET_F32K_1_MS:
            scale = 1;
            break;
        case SET_F32K_8_MS:
            scale = 8;
            break;
        case SET_F32K_16_MS:
            scale = 16;
            break;
        case SET_F32K_64_MS:
            scale = 64;
            break;
        case SET_F32K_128_MS:
            scale = 128;
            break;
        case SET_F32K_256_MS:
            scale = 256;
            break;
        case SET_F32K_4096_MS:
            scale = 4096;
            break;
    }
    return scale;
}

void ostimer_irq_callback(void *user_data)
{
    if(*CM4_OST_ISR != 0)
    {
        *CM4_OST_ISR = 0x1F;
        NVIC_DisableIRQ(OSTimer_IRQn);
    }
}
#endif /* HAL_SLEEP_MANAGER_ENABLED */
