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

#include "hal_platform.h"

#ifdef HAL_SLEEP_MANAGER_ENABLED

#include "hal_cm4_topsm.h"
#include "hal_dsp_topsm.h"
#include "hal_sys_topsm.h"
#include "hal_ostd.h"
#include "hal_flash_sf.h"
#include "hal_log.h"
#include "memory_attribute.h"
#include "hal_gpt.h"
#include "hal_clock.h"
#include "hal_clock_internal.h"
#include "hal_sleep_driver.h"
#include "hal_cm4_topsm_internal.h"
#include "hal_cache.h"
#include "hal_cache_internal.h"
#include "hal_mpu.h"
#include "hal_mpu_internal.h"
#include "hal_emi_internal.h"
#include "hal_pmu.h"


bool CM4_TOPSM_DEBUG_OPTION = false;
bool CM4_TOPSM_PRINTF_OPTION = true;
ATTR_RWDATA_IN_TCM bool FAST_WAKEUP_VERIFICATION = false;
bool first_init = true;
bool BT_workaround = false;
void cm4_sleep_mode_no_init(unsigned short type);
void clock_gated_observe(void);
void clock_gated(void);
void cm4_sleep_bootup_check(const uint32_t wakeup_source_index);
void hal_cm4_topsm_run_suspend_cbs(void);
void hal_cm4_topsm_run_resume_cbs(void);

void hal_cm4_topsm_run_suspend_cbs(void);
void hal_cm4_topsm_run_resume_cbs(void);

ATTR_RWDATA_IN_TCM CPU_CORE_BAKEUP_REG_T  cpu_core_reg ;
ATTR_RWDATA_IN_TCM volatile uint32_t Vector0_backup, temp_reg;
ATTR_RWDATA_IN_TCM volatile uint32_t origin_msp_bak_reg, origin_psp_bak_reg, backup_return_address;

ATTR_RWDATA_IN_TCM uint32_t sleep_time_ms = 0;
ATTR_RWDATA_IN_TCM uint32_t Target_Sleep_Frames = 0;
ATTR_RWDATA_IN_TCM uint32_t frm_dur = 0;
extern void hal_module_sleep_register_callback(void);
cm4_topsm_mtcmos_control mtcmos_control;

#define MAX_CB_NUM 32
int32_t suspend_cb_idx = 0;
int32_t resume_cb_idx = 0;
hal_cm4_topsm_cb_t suspend_cbs[MAX_CB_NUM];
hal_cm4_topsm_cb_t resume_cbs[MAX_CB_NUM];

ATTR_TEXT_IN_TCM extern void cm4_ostd_enter_sleep_cmsys_memory_sleep(uint32_t Target_Sleep_Frames);
ATTR_TEXT_IN_TCM extern void uart_backup_all_registers(void);
ATTR_TEXT_IN_TCM extern void uart_restore_all_registers(void);

void cm4_topsm_debug_option(bool sta)
{
    CM4_TOPSM_DEBUG_OPTION = sta;
}
void cm4_tdma_enter_sleep()
{
    *CM4_TOPSM_MODEM_CG_CLR2 = 0x1;
}

void cm4_topsm_set_f32k_value(int unit)
{
    switch (unit) {
        case F32K_MS_1:
            *CM4_TOPSM_FRC_F32K_FM = SET_F32K_1_MS;
            break;
        case F32K_MS_8:
            *CM4_TOPSM_FRC_F32K_FM = SET_F32K_8_MS;
            break;
        case F32K_MS_16:
            *CM4_TOPSM_FRC_F32K_FM = SET_F32K_16_MS;
            break;
        case F32K_MS_64:
            *CM4_TOPSM_FRC_F32K_FM = SET_F32K_64_MS;
            break;
        case F32K_MS_128:
            *CM4_TOPSM_FRC_F32K_FM = SET_F32K_128_MS;
            break;
        case F32K_MS_256:
            *CM4_TOPSM_FRC_F32K_FM = SET_F32K_256_MS;
            break;
        case F32K_MS_4096:
            *CM4_TOPSM_FRC_F32K_FM = SET_F32K_4096_MS;
            break;
    }
}

ATTR_TEXT_IN_TCM void cm4_topsm_set_sleep_timeframe(uint32_t sleep_time, uint32_t frame, uint32_t duration)
{
    Target_Sleep_Frames = frame;
    frm_dur = duration;
    sleep_time_ms = sleep_time;
}

int cm4_topsm_lock_BT_MTCMOS()
{
    mtcmos_control.BT_Count++;
    if (mtcmos_control.BT_Count != 0) {
        sys_topsm_mtcmos_control(HAL_SYS_TOPSM_MTCMOS_BTSYS, 1);
    }
    return mtcmos_control.BT_Count;
}
int cm4_topsm_lock_MM_MTCMOS()
{
    mtcmos_control.MM_Count++;
    if (mtcmos_control.MM_Count != 0) {
        sys_topsm_mtcmos_control(HAL_SYS_TOPSM_MTCMOS_MMSYS, 1);
    }
    return mtcmos_control.MM_Count;
}
int cm4_topsm_lock_MD2G_MTCMOS()
{
    mtcmos_control.MD2G_Count++;
    if (mtcmos_control.MD2G_Count != 0) {
        sys_topsm_mtcmos_control(HAL_SYS_TOPSM_MTCMOS_MD2G, 1);
    }
    return mtcmos_control.MD2G_Count;
}
int cm4_topsm_unlock_BT_MTCMOS()
{
    if (mtcmos_control.BT_Count == 0) {
        return 0;
    }
    mtcmos_control.BT_Count--;
    if (mtcmos_control.BT_Count == 0) {
        sys_topsm_mtcmos_control(HAL_SYS_TOPSM_MTCMOS_BTSYS, 0);
    }
    return mtcmos_control.BT_Count;
}
int cm4_topsm_unlock_MM_MTCMOS()
{
    if (mtcmos_control.MM_Count == 0) {
        return 0;
    }
    mtcmos_control.MM_Count--;
    if (mtcmos_control.MM_Count == 0) {
        sys_topsm_mtcmos_control(HAL_SYS_TOPSM_MTCMOS_MMSYS, 0);
    }
    return mtcmos_control.MM_Count;
}
int cm4_topsm_unlock_MD2G_MTCMOS()
{
    if (mtcmos_control.MD2G_Count == 0) {
        return 0;
    }
    mtcmos_control.MD2G_Count--;
    if (mtcmos_control.MD2G_Count == 0) {
        sys_topsm_mtcmos_control(HAL_SYS_TOPSM_MTCMOS_MD2G, 0);
    }
    return mtcmos_control.MD2G_Count;
}
void mtcmos_bt()
{
    mtcmos_control.BT_Count = 0;
}
void mtcmos_mm()
{
    mtcmos_control.MM_Count = 0;
}
void mtcmos_md2g()
{
    mtcmos_control.MD2G_Count = 0;
}
void bt_mtcmos_workaround(bool b)
{
    BT_workaround = b;
}
void cm4_topsm_printf_option(bool b)
{
    CM4_TOPSM_PRINTF_OPTION = b ;
}

void cm4_topsm_rm_init_interrupt(void)
{
    uint32_t timer_status = 0;
    do {
        timer_status = *CM4_TOPSM_RM_TMR_SSTA;
    } while (!(timer_status == TIMER_NORMAL_STATE));
    *CM4_OST_ISR = 0x001F;
}

void cm4_topsm_rm_init_misc(void)
{
    *CM4_TOPSM_SM_SLV_REQ_IRQ = 0xF;
    *CM4_TOPSM_SM_MAS_RDY_IRQ = 0xF;
    //*CM4_TOPSM_FRC_CON = 0x11530000 + 0x00000001; //Enable FRC for OST and FM
}

void Vcore_switch(PMIC_PSI psi_mode, PMIC_VCORE_VOSEL vcore_vosel)
{
    PMIC_VCORE_VOSEL_CONFIG(psi_mode, vcore_vosel);
}

uint32_t cm4_topsm_get_free_run_counter(void)
{
    return *CM4_TOPSM_FRC_VAL_R;
}
uint32_t cm4_topsm_get_32k_free_run_counter(void)
{
    return *CM4_TOPSM_F32K_CNT;
}

#if     defined (__GNUC__)      //GCC disable compiler optimize
#pragma GCC push_options
#pragma GCC optimize ("O0")
#elif   defined (__ICCARM__)    //IAR disable compiler optimize
#pragma optimize=none
#elif   defined (__CC_ARM)      //MDK disable compiler optimize
#pragma push
#pragma O0
#endif
ATTR_TEXT_IN_TCM void cm4_topsm_enter_suspend(hal_sleep_mode_t mode)
{
    if ((mode != HAL_SLEEP_MODE_SLEEP) && (mode != HAL_SLEEP_MODE_DEEP_SLEEP)) {
        return;
    }
    if (CM4_TOPSM_DEBUG_OPTION) {
        printf("[Start Enter Sleep]\n");
    }

    pmu_set_vcore_s1_buck();    //Set Vcore S1 Buck Voltage

    hal_cm4_topsm_run_suspend_cbs();
    cm4_topsm_init();
    dsp_topsm_init();
    cm4_ostd_init();
    cm4_ostd_set_frame_duration(frm_dur); // Max fram time : 4096*1/32k = 128ms
#ifndef CM4_TCM_pd
    *CMCFG_SYSRAM_CFG = 0xFFFFFFFF;
#endif

    clock_suspend(false);
    *((volatile uint32_t *)0xA21C0028) &= ~(1 << 15); //ABBA_TOP_CON0[15] RG_ABB_LVSH_EN

    if (mtcmos_control.BT_Count == 0) {
        sys_topsm_mtcmos_control(HAL_SYS_TOPSM_MTCMOS_BTSYS, 0);
    }
    if (mtcmos_control.MM_Count == 0) {
        sys_topsm_mtcmos_control(HAL_SYS_TOPSM_MTCMOS_MMSYS, 0);
    }
    if (mtcmos_control.MD2G_Count == 0) {
        sys_topsm_mtcmos_control(HAL_SYS_TOPSM_MTCMOS_MD2G, 0);
    }

    cm4_ostd_enable_ost(1);
    cm4_tdma_enter_sleep();
    while ((*CM4_TOPSM_SM_SLV_REQ_STA & 0x1) == 0x1); /*turn off dsp_topsm clk req*/
    *DSP_TOPSM_DBG_RM_SM_MASK = 0x4;
    *SYS_TOPSM_RM_PWR_CON0 = *SYS_TOPSM_RM_PWR_CON0 & 0xFFFFFFBD; // [6]:PWR_REQ_EN=0 [2]:PWR_ON_0 = 0

    *CM4_TOPSM_DBG_RM_SM_MASK = 0x7;
    *CM4_TOPSM_RM_CLK_SETTLE = 0x050F0068; //cm4 overflow
    *CM4_TOPSM_SM_TRIG_SETTLE0 = 0x6;

    if (mode == HAL_SLEEP_MODE_SLEEP) {
        printf("In Fast Wakeup Setting\n");
        clock_suspend(true);
        PSI_SLEEP_AT_S0(SLEEP_PSI_MASTER_CM4);
        *CM4_TOPSM_MCF_CNT_BASE = 0x2;
        *CM4_TOPSM_RM_PWR_PER1 = *CM4_TOPSM_RM_PWR_PER1 | 0xB00;
        *CM4_TOPSM_RM_PWR_PER0 = *CM4_TOPSM_RM_PWR_PER0 | 0x6;
        *CM4_TOPSM_RM_CLK_SETTLE = 0x050f0013;
        *CM4_TOPSM_RM_CLK_SETTLE = *CM4_TOPSM_RM_CLK_SETTLE & 0xFF00FFFF;
        *CM4_TOPSM_RM_PWR_CON0 = *CM4_TOPSM_RM_PWR_CON0 & 0xFFFFE0FF;
        *CM4_OST_EVENT_MASK = *CM4_OST_EVENT_MASK | 0x3FFD;
    } else {
        PSI_SLEEP_AT_S1(SLEEP_PSI_MASTER_CM4);  //Normal
        *CM4_TOPSM_RM_PWR_PER1 = *CM4_TOPSM_RM_PWR_PER1 | 0x300; // [11:8] reg_memup_settle
        if (mtcmos_control.BT_Count) {
            PSI_SLEEP_AT_S1(SLEEP_PSI_MASTER_BT); //Normal
        }
        if (FAST_WAKEUP_VERIFICATION == 1) {
            *PSI_MST_CON2 = 0x01;
        }
        *PSI_MST_CON2 = 0x70007;    //PSI settle time
    }

    cm4_ostd_enter_sleep_cmsys_memory_sleep(Target_Sleep_Frames);
    uint16_t reg_val = 0;
    uint16_t wait_time = 0;
    /*Trigger HW Read command*/
    *CM4_OST_CMD = (0x11530000 + 0x00000002);
    do {
        reg_val = *CM4_OST_STA;
        wait_time++;
    } while (!(reg_val & 0x0002));

    if (CM4_TOPSM_DEBUG_OPTION) {
        printf("[BT_C:%d][MM_C:%d][MD2G_C:%d]\n", mtcmos_control.BT_Count, mtcmos_control.MM_Count, mtcmos_control.MD2G_Count);
        printf("[EINT IRQ(EINT_STA):%lx]\n", *((volatile uint32_t *)(0xA2030300)));
        printf("[EINT Wakeup Event(EINT_EEVT):%lx]\n", *((volatile uint32_t *)(0xA2030310)));
        printf("[sleep_time_ms:%d;frame:%d;duration:%lx;CM4_TOPSM_FRC_F32K_FM:%lx]\n", (int)sleep_time_ms, (int)Target_Sleep_Frames, frm_dur, *CM4_TOPSM_FRC_F32K_FM);
        printf("[SystemCoreClock  :%ld]\n", SystemCoreClock);
        sys_topsm_debug_log(HAL_SYS_TOPSM_MTCMOS_DEBUG);
    }
    cm4_topsm_enter_deep_sleep(mode);
}
#if     defined (__GNUC__)
#pragma GCC push_options
#elif   defined (__CC_ARM)
#pragma pop
#endif

#if     defined (__GNUC__)      //GCC disable compiler optimize
#pragma GCC push_options
#pragma GCC optimize ("O0")
#elif   defined (__ICCARM__)    //IAR disable compiler optimize
#pragma optimize=none
#elif   defined (__CC_ARM)      //MDK disable compiler optimize
#pragma push
#pragma diag_suppress 1267
#pragma O0
#endif
ATTR_TEXT_IN_TCM void cm4_topsm_enter_deep_sleep(hal_sleep_mode_t mode)
{
#if defined (__CC_ARM)
    /* Backup function return address(R14)*/
    __asm volatile("mov backup_return_address,__return_address() \n");
#endif

    /* For LP Setting : AUXADC */
    /* Turn off AuxADC */
    *PMIC_WR_PATH_CON0 = 0x0001;//(default value:0x0004) // (WR_PATH_CON0) AUXADC_PWDB=0
    /* DMIC off */
    *ABBA_AUDIODL_CON4 = 0x0000;//(it's default value) DMIC global bias on

    /* Because GPIO design,must be set, otherwise there will be an exception IO */
    GPIO_TDSEL_SET;

    if (FAST_WAKEUP_VERIFICATION) {
        *((volatile uint32_t *)(0xA2020304)) = 0x00008000;
    }

    *CM4_TOPSM_RM_TMR_PWR0 = 1;
    __asm volatile("dsb");
    *CM4_TOPSM_RM_PWR_CON0 = 0x6901; // CM4 MTCMOS OFF while sleep

    /* UART backup */
    uart_backup_all_registers();

    /*backup CPU core registers*/
    temp_reg = (unsigned int)&cpu_core_reg;
    __CPU_CORE_REG_BACKUP(temp_reg);

    /* Set Boot Slave */
    *CMCFG_BOOT_FROM_SLV = 0x1;

    /* CM4_TOPSM_SYSTEM_CONTROL ? */
    *CM4_TOPSM_SYSTEM_CONTROL = *CM4_TOPSM_SYSTEM_CONTROL | 0x4;

    /* Start back up Cmsys */
    /* NVIC backup */
    nvic_sleep_backup_register();

    /* cache backcp */
#ifdef HAL_CACHE_MODULE_ENABLED
    cache_status_save();
#endif

    /* mpu backcp */
#ifdef HAL_MPU_MODULE_ENABLED
    mpu_status_save();
#endif

    /* cmsys config backup */
    cmsys_cfg_status_save();

    /* fpu backup */
    fpu_status_save();

    /* CM4 system control registers backup*/
    deepsleep_backup_CM4_sys_ctrl_reg();

    /* general register backup */
    __CUP_STACK_POINT_BACKUP(origin_psp_bak_reg, origin_msp_bak_reg);

    /* Check LDO Codition,switch PMU MTCMOS Power ON/OFF in S0,S1 Mode */
    pmu_control_mtcmos();

    /* Flash Enter Powerdown Mode */
    SF_DAL_DEV_Enter_DPD();

    /* PSRAM Enter Sleep Mode */
    mtk_psram_half_sleep_enter();

    /* infra power down */
    INFRA_POWER_OFF;

    /* clean SYS_TOPSM_RM_PERI_CON bits*/
    *((volatile uint32_t *)0xA20F0030) = *((volatile uint32_t *)0xA20F0030) & 0xFFFEFFFF;

    /* backup BootVector0 Stack Address */
    Vector0_backup = *((volatile uint32_t *)0xA2110000); //0xA2110000 : boot vector 0(boot slave stack point)

    /* Disable I2C wakeup source*/
    *CM4_OST_EVENT_MASK = 0x40;

    /* backup MSP Address */
#if (defined (__GNUC__) || defined (__ICCARM__))
    __asm volatile("push {r0-r12, lr}");
    __asm volatile("mov %0, sp" : "=r"(temp_reg));
#elif defined (__CC_ARM)
    __PUSH_CUP_REG();
    __BACKUP_SP(temp_reg);
#endif
    *((volatile uint32_t *)0xA2110000) = temp_reg;  //VECTOR0 write MSP Address

    /* Enable OST ISR */
    *CM4_OST_INT_MASK = 0x17;
    *((volatile uint32_t *)0xe000E100) |= 1;        //NVIC_EnableIRQ(OSTimer_IRQn);
    *((volatile uint32_t *)0xe000e280) = 0x1;       //Clear CM4 OST IRQ Pending bits
    *CM4_OST_ISR = 0x1F;

    /* Enter Deep Sleep */
    temp_reg = 0xA2110004;                          //CMCFG_BOOT_VECTOR1 Address
    __ENTER_DEEP_SLEEP(temp_reg);

    /* pop CPU Reg R0-R12 */
#if defined (__CC_ARM)
    __POP_CUP_REG();
#endif

    /* Clear OST Interrupt Flag */
    *CM4_OST_ISR = 0x1F;

    /* Set SYS_TOPSM_RM_PERI_CON bits */
    *((volatile uint32_t *)0xA20F0030) |= 0x10000;

    if (FAST_WAKEUP_VERIFICATION == 1) {
        *((volatile uint32_t *)(0xA2020308)) = 0x00008000;  //low
    }
    if (mode == HAL_SLEEP_MODE_DEEP_SLEEP) {
        hal_gpt_delay_us(400);
    }else{
        hal_gpt_delay_us(600);
    }
    /* infra power on */
    INFRA_POWER_ON;

    /* PSRAM Leave Sleep Mode */
    mtk_psram_half_sleep_exit();

    /* Flash Leave Powerdown Mode */
    SF_DAL_DEV_Leave_DPD();

    /* CMSYS Peripheral : make virtual space available */
    *((volatile uint32_t *)(0xA0881000)) = 0x10000023;
    *((volatile uint32_t *)(0xA0881004)) = 0x0;

    /* restore MSP */
    temp_reg = (unsigned int)&origin_msp_bak_reg;
    __MSP_RESTORE(temp_reg);

    /* swtich stack point to psp */
    __SWITCH_TO_PSP_STACK_POINT();

    /* restore PSP */
    temp_reg = (unsigned int)&origin_psp_bak_reg;
    __PSP_RESTORE(temp_reg);

    /* restore boot Vector */
    *CMCFG_BOOT_FROM_SLV = 0x0;
    *((volatile uint32_t *)0xA2110000) = Vector0_backup ;

    /* Start restore Cmsys */
    /* CM4 system control registers restore*/
    deepsleep_restore_CM4_sys_ctrl_reg();

    /* fpu restore */
    fpu_status_restore();

    /* cmsys config restore */
    cmsys_cfg_status_restore();

    /* mpu restore */
#ifdef HAL_MPU_MODULE_ENABLED
    mpu_status_restore();
#endif

    /* cache restore */
#ifdef HAL_CACHE_MODULE_ENABLED
    cache_status_restore();
#endif

    /* restore CPU core registers */
    temp_reg = (unsigned int)&cpu_core_reg;
    __CPU_CORE_REG_RESTORE(temp_reg);

    /* Clear CM4 Deep Sleep bits */
    *CM4_TOPSM_SYSTEM_CONTROL = *CM4_TOPSM_SYSTEM_CONTROL & (~0x4);

    /* NVIC restore */
    nvic_sleep_restore_register();
    /* End restore Cmsys */

    /* UART restore */
    uart_restore_all_registers();

    if (mtcmos_control.BT_Count == 0) {    //BT default on
        sys_topsm_mtcmos_control(HAL_SYS_TOPSM_MTCMOS_BTSYS, 1);
    }

    if (mtcmos_control.MM_Count == 0) {    /* MM power on */
        sys_topsm_mtcmos_control(HAL_SYS_TOPSM_MTCMOS_MMSYS, 1);
    }

    if (mtcmos_control.MD2G_Count == 0) {    /* MD2G power on */
        sys_topsm_mtcmos_control(HAL_SYS_TOPSM_MTCMOS_MD2G, 1);
    }

    /* ABBA_TOP_CON0[15] RG_ABB_LVSH_EN */
    *((volatile uint32_t *)0xA21C0028) |= (1 << 15);

    clock_resume();

    hal_cm4_topsm_run_resume_cbs();

    /* Turn off AuxADC */
    *PMIC_WR_PATH_CON0 = 0x0001;//(default value:0x0004) // (WR_PATH_CON0) AUXADC_PWDB=0, Turn off AuxADC

    /* Because GPIO design,must be set, otherwise there will be an exception IO */
    GPIO_TDSEL_CLR;

#if defined (__CC_ARM)
    __RESTORE_LR(backup_return_address);
#endif
}
#if     defined (__GNUC__)
#pragma GCC push_options
#elif   defined (__CC_ARM)
#pragma pop
#endif

void cm4_topsm_init(void)
{
    *CM4_TOPSM_RM_CLK_SETTLE = 0x50F01FF; /*8:0]=rm_sysclk_settle max value*/
    *CM4_TOPSM_RM_TMR_TRG0 = 0x1;
    *CM4_TOPSM_RM_TMR_PWR0 = 0x00000001; // setting ostimer -> MCU power region
    *CM4_TOPSM_RM_PWR_PER0 = 0x0900121F; // For Power state setting time
    *CM4_TOPSM_RM_PWR_PER1 = 0x11111111;
    *CM4_TOPSM_PROTECT_ACK_MASK = 0x00000000; // Mask when power domain always on
    *CM4_TOPSM_RM_PWR_CON0 = 0x00042244; // CM4 MTCMOS ON while sleep
    *CM4_TOPSM_CCF_CLK_CON |= (0x01 << 12); /*Setting for TOPSM state debug output*/
    *CM4_TOPSM_SM_TRIG_SETTLE0 = 0x00000008; //SM_TRIG_SETTLE0 > 2+2+2 = RM_PLL1_SETTLE+RM_PLL2_SETTLE+(MAX_PWR_SETTLE in RM_PWR_PER)
    *CM4_TOPSM_SM_REQ_MASK = 0x00000000; // {2: to MODEM , 0:to ARM7 }
    *CM4_TOPSM_RM_SM_TRG = 0xFFFFFFFF;
    *CM4_TOPSM_RM_SM_PWR = 0x00000000;
    *CM4_TOPSM_RM_SM_PLL_MASK0 = 0xFFFFFFFF;
    *CM4_TOPSM_INDIV_CLK_PROTECT_ACK_MASK = 0x00;
    cm4_topsm_rm_init_interrupt();
    cm4_topsm_rm_init_misc();

    *CM4_TOPSM_CCF_CLK_CON = *CM4_TOPSM_CCF_CLK_CON | 0x4000;
    *SYS_TOPSM_CCF_CLK_CON = *SYS_TOPSM_CCF_CLK_CON | 0x4000;
    *DSP_TOPSM_CCF_CLK_CON = *DSP_TOPSM_CCF_CLK_CON | 0x4000;

    *CM4_TOPSM_TOPSM_DBG = *CM4_TOPSM_TOPSM_DBG & 0xFFFFFFFE;
    *SYS_TOPSM_TOPSM_DBG = *SYS_TOPSM_TOPSM_DBG & 0xFFFFF0FF;
    *DSP_TOPSM_TOPSM_DBG = *DSP_TOPSM_TOPSM_DBG & 0xFFFFFFFE;

    *((volatile uint32_t *)0xA20F0030) |= 0x10000;  //Set SYS_TOPSM_RM_PERI_CON bits
}

bool hal_cm4_topsm_register_suspend_cb(cm4_topsm_cb func, void *data)
{
    if (suspend_cb_idx >= MAX_CB_NUM) {
        printf("over max suspend callback function can be registered\n");
        return false;
    }

    suspend_cbs[suspend_cb_idx].func = func;
    suspend_cbs[suspend_cb_idx].para = data;
    suspend_cb_idx++;
    return true;
}

void hal_cm4_topsm_run_suspend_cbs(void)
{
    for (int32_t i = 0; i < suspend_cb_idx; i++) {
        suspend_cbs[i].func(suspend_cbs[i].para);
    }
}

bool hal_cm4_topsm_register_resume_cb(cm4_topsm_cb func, void *data)
{
    if (resume_cb_idx >= MAX_CB_NUM) {
        printf("over max resume callback function can be registered\n");
        return false;
    }

    resume_cbs[resume_cb_idx].func = func;
    resume_cbs[resume_cb_idx].para = data;

    resume_cb_idx++;

    return true;
}

void hal_cm4_topsm_run_resume_cbs(void)
{
    for (int32_t i = 0; i < resume_cb_idx; i++) {
        resume_cbs[i].func(resume_cbs[i].para);
    }
}
void cm4_topsm_sleep_default_callback_function()
{
    hal_module_sleep_register_callback();
}

#endif /* HAL_SLEEP_MANAGER_ENABLED */
