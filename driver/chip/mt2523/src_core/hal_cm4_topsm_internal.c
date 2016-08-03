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
#include "mt2523.h"
#include "hal_cm4_topsm_internal.h"
#include "hal_cache.h"
#include "hal_cache_internal.h"
#include "core_cm4.h"
#include "memory_attribute.h"
#include "hal_mpu.h"
#include "hal_mpu_internal.h"



#define SAVE_PRIORITY_GROUP (IRQ_NUMBER_MAX)
typedef struct {
    uint32_t nvic_iser;                    /**< eint hardware debounce time */
    uint32_t nvic_iser1;
    uint32_t nvic_ip[SAVE_PRIORITY_GROUP];
} nvic_sleep_backup_register_t;


ATTR_RWDATA_IN_TCM volatile nvic_sleep_backup_register_t nvic_backup_register;

//typedef struct {
//    volatile uint32_t PSR;      /* ASOR,IRSR,EPSR */
//    volatile uint32_t PRIMASK;
//    volatile uint32_t FAULTMASK;
//    volatile uint32_t BASEPRI;
//    volatile uint32_t CONTROL;
//} CPU_CORE_BAKEUP_REG_T;

typedef struct {
    volatile uint32_t ACTLR;    /* Auxiliary Control Register */
    volatile uint32_t VTOR;     /* Vector Table Offset Register */
    volatile uint32_t SCR;      /* System Control Register */
    volatile uint32_t CCR;      /* Configuration Control Register */
    volatile uint8_t SHP[12];   /* System Handlers Priority Registers (4-7, 8-11, 12-15) */
    volatile uint32_t SHCSR;    /* System Handler Control and State Register */
    volatile uint32_t CPACR;    /* Coprocessor Access Control Register */
} CM4_SYS_CTRL_BAKEUP_REG_T;

/* save on TCM or PSRAM, please Charlie comment !*/
//volatile ATTR_RWDATA_IN_TCM CPU_CORE_BAKEUP_REG_T  cpu_core_reg ;
//volatile ATTR_RWDATA_IN_TCM CPU_CORE_BAKEUP_REG_T  *p_cpu_core_reg = &cpu_core_reg ;
ATTR_RWDATA_IN_TCM volatile CM4_SYS_CTRL_BAKEUP_REG_T  cm4_sys_ctrl_reg ;

/* FPU backup register struct
*/
typedef struct {
    volatile uint32_t FPCCR;
    volatile uint32_t FPCAR;
} FPU_BAKEUP_REG_T;

/* CMSYS_CFG backup register struct
*/
typedef struct {
    volatile uint32_t STCALIB;
    volatile uint32_t AHB_BUFFERALBE;
    volatile uint32_t AHB_FIFO_TH;
    volatile uint32_t INT_ACTIVE_HL0;
    volatile uint32_t INT_ACTIVE_HL1;
    volatile uint32_t DCM_CTRL_REG;
} CMSYS_CFG_BAKEUP_REG_T;

/* CMSYS_CFG_EXT backup register struct
*/
typedef struct {
    __IO uint32_t CG_EN;
    __IO uint32_t DCM_EN;
} CMSYS_CFG_EXT_BAKEUP_REG_T;


/* save on TCM or PSRAM, please Charlie comment !*/
ATTR_RWDATA_IN_TCM volatile FPU_BAKEUP_REG_T  fpu_reg;
ATTR_RWDATA_IN_TCM volatile CMSYS_CFG_BAKEUP_REG_T  cmsys_cfg_reg;
ATTR_RWDATA_IN_TCM volatile CMSYS_CFG_EXT_BAKEUP_REG_T cmsys_cfg_ext_reg;



ATTR_TEXT_IN_TCM inline uint32_t nvic_sleep_backup_register(void)
{
    uint32_t i;
    nvic_backup_register.nvic_iser = NVIC->ISER[0];
    nvic_backup_register.nvic_iser1 = NVIC->ISER[1];
    for (i = 0; i < SAVE_PRIORITY_GROUP; i++) {
        nvic_backup_register.nvic_ip[i] = NVIC->IP[i];
    }
    return 0;
}

ATTR_TEXT_IN_TCM inline uint32_t nvic_sleep_restore_register(void)
{
    uint32_t i;
    NVIC->ISER[0] = nvic_backup_register.nvic_iser;
    NVIC->ISER[1] = nvic_backup_register.nvic_iser1;
    for (i = 0; i < SAVE_PRIORITY_GROUP; i++) {
        NVIC->IP[i] = nvic_backup_register.nvic_ip[i];
    }
    return 0;
}

/*
* backup CPU core registers
*/
ATTR_TEXT_IN_TCM inline void deepsleep_backup_CPU_core_reg(void)
{
//#ifdef __GNUC__
//    __asm volatile
//    (
//        "push {lr}                     \n"
//        "ldr r0, =p_cpu_core_reg       \n"
//        "ldr r0, [r0]                  \n"
//        "mrs r1, psr                   \n"
//        "str r1, [r0],4                \n"      /* backup PSR  takecare: maybe no need to backup!! */
//        "mrs r1, primask               \n"
//        "str r1, [r0],4                \n"      /* backup PRIMASK              */
//        "mrs r1, faultmask             \n"
//        "str r1, [r0],4                \n"      /* backup FAULTMASK          */
//        "mrs r1, basepri               \n"
//        "str r1, [r0],4                \n"      /* backup BASEPRI               */
//        "mrs r1, control               \n"
//        "str r1, [r0]                  \n"      /* backup CONTROL              */
//        "pop {lr}                      \n"
//        "bx  lr                        \n"
//    );
//#else //KEIL
//    //[ToDo] Add ARM's in-line assembly code
//#endif
}

/*
* restore CPU core registers
*/
ATTR_TEXT_IN_TCM inline void deepsleep_restore_CPU_core_reg(void)
{
//#ifdef __GNUC__
//    __asm volatile
//    (
//        "push {lr}                     \n"
//        "ldr r0, =p_cpu_core_reg       \n"
//        "ldr r0, [r0]                  \n"     /* get backup value            */
//        "ldr r1, [r0],4                \n"
//        "msr psr, r1                   \n"     /* restore PSR   takecare: maybe no need to restore!! */
//        "ldr r1, [r0],4                \n"
//        "msr primask, r1               \n"     /* restore PRIMASK             */
//        "ldr r1, [r0],4                \n"
//        "msr faultmask, r1             \n"     /* restore FAULTMASK         */
//        "ldr r1, [r0],4                \n"
//        "msr basepri, r1               \n"     /* restore BASEPRI              */
//        "ldr r1, [r0],4                \n"
//        "msr control, r1               \n"     /* restore CONTROL              */
//        "pop {lr}                      \n"
//        "bx  lr                        \n"
//    );
//#else //KEIL
//    //[ToDo] Add ARM's in-line assembly code
//#endif

}

/*
* backup CM4 system control registers
*/
ATTR_TEXT_IN_TCM inline void deepsleep_backup_CM4_sys_ctrl_reg(void)
{
    cm4_sys_ctrl_reg.ACTLR = SCnSCB->ACTLR;
    cm4_sys_ctrl_reg.VTOR = SCB->VTOR;
    cm4_sys_ctrl_reg.SCR = SCB->SCR;
    cm4_sys_ctrl_reg.CCR = SCB->CCR;

    cm4_sys_ctrl_reg.SHP[0] = SCB->SHP[0]; /* MemMange */
    cm4_sys_ctrl_reg.SHP[1] = SCB->SHP[1]; /* BusFault */
    cm4_sys_ctrl_reg.SHP[2] = SCB->SHP[2]; /* UsageFault */
    cm4_sys_ctrl_reg.SHP[7] = SCB->SHP[7]; /* SVCall */
    cm4_sys_ctrl_reg.SHP[8] = SCB->SHP[8]; /* DebugMonitor */
    cm4_sys_ctrl_reg.SHP[10] = SCB->SHP[10]; /* PendSV */
    cm4_sys_ctrl_reg.SHP[11] = SCB->SHP[11]; /* SysTick */

    cm4_sys_ctrl_reg.SHCSR = SCB->SHCSR;
    cm4_sys_ctrl_reg.CPACR = SCB->CPACR;
}
/*
* restore CM4 system control registers
*/
ATTR_TEXT_IN_TCM inline void deepsleep_restore_CM4_sys_ctrl_reg(void)
{
    SCnSCB->ACTLR = cm4_sys_ctrl_reg.ACTLR;
    SCB->VTOR = cm4_sys_ctrl_reg.VTOR;
    SCB->SCR = cm4_sys_ctrl_reg.SCR;
    SCB->CCR = cm4_sys_ctrl_reg.CCR;

    SCB->SHP[0] = cm4_sys_ctrl_reg.SHP[0]; /* MemMange */
    SCB->SHP[1] = cm4_sys_ctrl_reg.SHP[1]; /* BusFault */
    SCB->SHP[2] = cm4_sys_ctrl_reg.SHP[2]; /* UsageFault */
    SCB->SHP[7] = cm4_sys_ctrl_reg.SHP[7]; /* SVCall */
    SCB->SHP[8] = cm4_sys_ctrl_reg.SHP[8]; /* DebugMonitor */
    SCB->SHP[10] = cm4_sys_ctrl_reg.SHP[10]; /* PendSV */
    SCB->SHP[11] = cm4_sys_ctrl_reg.SHP[11]; /* SysTick */

    SCB->SHCSR = cm4_sys_ctrl_reg.SHCSR;
    SCB->CPACR = cm4_sys_ctrl_reg.CPACR;
}

ATTR_TEXT_IN_TCM inline void cmsys_cfg_status_save(void)
{
    cmsys_cfg_reg.STCALIB = CMSYS_CFG->STCALIB;
    cmsys_cfg_reg.AHB_BUFFERALBE = CMSYS_CFG->AHB_BUFFERALBE;
    cmsys_cfg_reg.AHB_FIFO_TH = CMSYS_CFG->AHB_FIFO_TH;
    cmsys_cfg_reg.INT_ACTIVE_HL0 = CMSYS_CFG->INT_ACTIVE_HL0;
    cmsys_cfg_reg.INT_ACTIVE_HL1 = CMSYS_CFG->INT_ACTIVE_HL1;
    cmsys_cfg_reg.DCM_CTRL_REG = CMSYS_CFG->DCM_CTRL_REG;

    cmsys_cfg_ext_reg.CG_EN = CMSYS_CFG_EXT->CG_EN;
    cmsys_cfg_ext_reg.DCM_EN = CMSYS_CFG_EXT->DCM_EN;
}

ATTR_TEXT_IN_TCM inline void cmsys_cfg_status_restore(void)
{
    CMSYS_CFG->STCALIB = cmsys_cfg_reg.STCALIB;
    CMSYS_CFG->AHB_BUFFERALBE = cmsys_cfg_reg.AHB_BUFFERALBE;
    CMSYS_CFG->AHB_FIFO_TH = cmsys_cfg_reg.AHB_FIFO_TH;
    CMSYS_CFG->INT_ACTIVE_HL0 = cmsys_cfg_reg.INT_ACTIVE_HL0;
    CMSYS_CFG->INT_ACTIVE_HL1 = cmsys_cfg_reg.INT_ACTIVE_HL1;
    CMSYS_CFG->DCM_CTRL_REG = cmsys_cfg_reg.DCM_CTRL_REG;

    CMSYS_CFG_EXT->CG_EN = cmsys_cfg_ext_reg.CG_EN;
    CMSYS_CFG_EXT->DCM_EN = cmsys_cfg_ext_reg.DCM_EN;
}

ATTR_TEXT_IN_TCM inline void fpu_status_save(void)
{
    fpu_reg.FPCCR = FPU->FPCCR;
    fpu_reg.FPCAR = FPU->FPCAR;
}

ATTR_TEXT_IN_TCM inline void fpu_status_restore(void)
{
    FPU->FPCCR = fpu_reg.FPCCR;
    FPU->FPCAR = fpu_reg.FPCAR;
}


inline int32_t cm4_cmsys_register_backup(void)
{
    /* NVIC backup*/
    nvic_sleep_backup_register();   //look ok!!

    /* cache backcp */
#ifdef HAL_CACHE_MODULE_ENABLED
    cache_status_save();            //modify finish
#endif

#ifdef HAL_MPU_MODULE_ENABLED
    mpu_status_save();              //modify finish
#endif
    /* cmsys config backup */
    cmsys_cfg_status_save();        //be modify

    /* fpu backup */
    fpu_status_save();              //be modify

    /* CM4 system control registers backup*/
    deepsleep_backup_CM4_sys_ctrl_reg();

    /* CM4 CPU core registers backup*/
    deepsleep_backup_CPU_core_reg();
    return 0;
}

int32_t cm4_cmsys_register_restore(void)
{
    /* CM4 CPU core registers restore*/
    //deepsleep_restore_CPU_core_reg();
    /* CM4 system control registers restore*/
    deepsleep_restore_CM4_sys_ctrl_reg();

    /* NVIC restore */
    nvic_sleep_restore_register();

    /* fpu restore */
    fpu_status_restore();

    /* cmsys config restore */
    cmsys_cfg_status_restore();

    /* cache restore */
#ifdef HAL_CACHE_MODULE_ENABLED
    cache_status_restore();
#endif

#ifdef HAL_MPU_MODULE_ENABLED
    mpu_status_restore();
#endif

    return 0;
}

/*
* example code only for backup/restore CPU general registers
*/
ATTR_RWDATA_IN_NONCACHED_RAM uint32_t psp_bak_reg, *p_psp_bak_reg = &psp_bak_reg;
ATTR_RWDATA_IN_NONCACHED_RAM uint32_t msp_bak_reg, *p_msp_bak_reg = &msp_bak_reg;
#ifdef __GNUC__
void idletask_example(void)
{
    /* ...... */

    __asm volatile( "push {r0-r12,lr}" );       /* save general register to PSP */
    /* bakeup psp */
    __asm volatile( "ldr r0, =p_psp_bak_reg" );
    __asm volatile( "ldr r0,[r0]" );
    __asm volatile( "mrs r1,psp" );
    __asm volatile( "str r1,[r0]");

    /* bakeup MSP */
    __asm volatile( "ldr r0, =p_msp_bak_reg" );
    __asm volatile( "ldr r0,[r0]" );
    __asm volatile( "mrs r1,msp" );
    __asm volatile( "str r1,[r0]");

    /* set bootslave register for wakeup */
    //*(bootslave_1) = msp_bak_reg;   //need fix!!
    // *(bootslave_2) = PC_wakeup_addr; //need fix !!

    /* lowpower others operation */

    /* enter WFI */
    __asm volatile( "dsb" );
    __asm volatile( "wfi" );
    __asm volatile( "isb" );


    /* after wakeup: thread mode with MSP*/
    __asm volatile( "mrs r0,control " );
    __asm volatile( "orr r0,#2 " );
    __asm volatile( "msr control,r0 " );         /* switch to PSP in thread mode */
    __asm volatile( "isb" );

    __asm volatile( "ldr r0, =p_psp_bak_reg" );  /* restore PSP */
    __asm volatile( "ldr r0,[r0]" );
    __asm volatile( "ldr r0,[r0]" );
    __asm volatile( "msr psp,r0" );

    __asm volatile( "pop {r0-r12,lr}" );         /* restore general registers from PSP */

    /* ...... */
}
#endif //#ifdef __GNUC__
#endif /* HAL_SLEEP_MANAGER_ENABLED */
