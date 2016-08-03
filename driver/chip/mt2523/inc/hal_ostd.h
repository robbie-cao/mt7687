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

/**
 * @author  Mediatek
 * @version 1.0
 * @date    2015.10.29
 *
 */

#ifndef __HAL_OSTD_H__
#define __HAL_OSTD_H__

#include "hal_platform.h"
#ifdef HAL_SLEEP_MANAGER_ENABLED
#include "hal_sleep_manager_platform.h"

#define HW_WRITE(ptr,data) (*(ptr) = (data))
#define HW_READ(ptr)           (*(ptr))
#define CMD_MAGIC_VALUE        0x11530000
//Settle
#define CM4_OSTD_SYSCLK_SETTLE 0X2
#define CM4_OSTD_PLL_SETTLE    0X1E
//#define CM4_OSTD_CURR_SETTLE   0X0
#define CM4_OSTD_CURR_SETTLE 0x20 //need check
typedef enum {
    OSTD_SUCCESS_FALSE = 0, /*OSTD read back AFN/UFN and found that current value is <=2*/
    OSTD_SUCCESS_TRUE, /*OSTD has successfully set the AFN value into ARM OS Timer HW*/
    OSTD_TIME_OUT, /*OSTD failed to poll ready bit*/
    OSTD_FAIL, /* Other unknown fail reason */
    OSTD_RESULT_NUM
} OSTD_RESULT_Enum;

typedef struct {
    uint32_t          curr_afn;            /* for OSTD to fill back current AFN value */
    uint32_t          curr_ufn;            /* for OSTD to fill back current UFN value */
    uint32_t          curr_afn_dly;            /* for OSTD to fill back current AFN_DLY value */
} hal_ostd_info_t;

typedef struct {
    bool            sleeped;
    hal_ostd_info_t   frame_info;           /*log the AFN/UFN before WFI*/
    uint32_t          pre_wfi_frc;             /*log the FRC before WFI*/
    bool            EnterSettleState;     /*config about in settle time or not */
    uint32_t          FRC_Pre;   /*log the FRC before sleeping*/
    uint32_t          slept_cnt;
} hal_ostd_attribute_t;

/******************** ostd.c internal usage ********************/
#define OST_MAX_WAIT_TIME       600 /* Based on designer's guarantee, the wait time for CMD ACK should be less than 1us. (Assume ARM clock is 600MHz) */
#define OST_MAX_SLEEP_UFN       3460 /* 16 sec => 16/4.615 = 3466.67  (for UDVT not care about WatchDog)*/
//#define  CM4_OSTIMER_BASE                 (0xA2120000)     /* CM4_OSTIMER_BASE: 0x83010000 is used for Larkspur */
#define CM4_OST_INT_MASK           ((volatile uint16_t*)(CM4_OSTIMER_BASE+0x30))  /* OS Timer Interrupt Mask (All default masked: disabled)       */
#define CM4_OST_DBG_WAKEUP         ((volatile uint32_t*)(CM4_OSTIMER_BASE+0x60))  /* OS Timer Debug Wakeup        */
#define CM4_OST_FRM_F32K           ((volatile uint32_t*)(CM4_OSTIMER_BASE+0x10))  /* OS Timer Frame Duration by 32K clock [8:0]  */
#define CM4_OST_FRM                ((volatile uint32_t*)(CM4_OSTIMER_BASE+0x0C))  /* OS Timer Frame Duration [12:0]        */
#define CM4_OST_UFN_R              ((volatile uint32_t*)(CM4_OSTIMER_BASE+0x20))  /* Current OS Timer Un-alignement Frame Number [31:0] */
#define CM4_OST_AFN_R              ((volatile uint32_t*)(CM4_OSTIMER_BASE+0x24))  /* Current OS Timer Alignement Frame Number [31:0] */
#define CM4_OST_AFN_DLY            ((volatile uint32_t*)(CM4_OSTIMER_BASE+0x1C))  /* OS Timer Alignment Frame Delay Number [31:0]*/
#define CM4_OST_CMD                ((volatile uint32_t*)(CM4_OSTIMER_BASE+0x04))  /* OS Timer Command Register*/
#define CM4_OST_UFN                ((volatile uint32_t*)(CM4_OSTIMER_BASE+0x14))  /* OS Timer Un-alignment Frame Number [31:0]  */
#define CM4_OST_AFN                ((volatile uint32_t*)(CM4_OSTIMER_BASE+0x18))  /* OS Timer Alignment Frame Number [31:0] */
#define CM4_OST_CON                ((volatile uint16_t*)(CM4_OSTIMER_BASE+0x00))  /* OS Timer Control Register  0: EN*/
#define CM4_OST_STA                ((volatile uint16_t*)(CM4_OSTIMER_BASE+0x08))  /* OS Timer Command Status Register*/
#define CM4_OST_EVENT_MASK         ((volatile uint32_t*)(CM4_OSTIMER_BASE+0x50))  /* OS Timer Event Mask (All default enabled)*/
#define CM4_OST_WAKEUP_STA         ((volatile uint32_t*)(CM4_OSTIMER_BASE+0x54))  /* OS Timer Event Wakeup Status [auto-clear with source]: record the first wakeup sources       */
#define CM4_OST_ISR                ((volatile uint32_t*)(CM4_OSTIMER_BASE+0x40))  /* OS Timer Interrupt Status        */

void cm4_ostd_init(void);
bool cm4_ostd_check_sleep(void);
void cm4_ostd_set_frame_duration (uint32_t frame_dur);
void cm4_ostd_get_currfrm( hal_ostd_info_t *pfrm_Info );
bool cm4_ostd_enforce_enter_sleep(uint8_t target_sleep_seconds);
void cm4_ostd_set_wakeup_event(uint32_t wakeup_event );
void cm4_ostd_mask_all_irq(void);
void cm4_ostd_unmask_all_irq(void);
bool cm4_ostd_set_one_irq_enable(hal_sleep_manager_wakeup_source_t index);
bool cm4_ostd_set_one_irq_disable(hal_sleep_manager_wakeup_source_t index);
bool cm4_ostd_check_in_settle_state (void);
void cm4_ostd_stress_test(bool enable);
OSTD_RESULT_Enum cm4_ostd_set_afn(uint32_t afn);
OSTD_RESULT_Enum cm4_ostd_set_ufn(uint32_t ufn);
void cm4_ostd_enable_ost(uint32_t enable);
uint32_t cm4_ostd_get_scale(void);
void ostimer_irq_callback(void *user_data);
#endif /* HAL_SLEEP_MANAGER_ENABLED */
#endif /* __HAL_OSTD_H__ */
