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

 /* Includes ------------------------------------------------------------------*/
#include "stdio.h"
#include "stdarg.h"
#include "exception_handler.h"

#define MAX_EXCEPTION_CONFIGURATIONS 6

typedef struct
{
  int items;
  exception_config_type configs[MAX_EXCEPTION_CONFIGURATIONS];
} exception_config_t;

typedef struct
{
  bool is_valid;
  const char *expr;
  const char *file;
  int line;
} assert_expr_t;

static exception_config_t exception_config = {0};

static assert_expr_t assert_expr = {0};

extern memory_region_type memory_regions[];

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/
void abort(void)
{
    __asm("cpsid i");
    SCB->CCR |=  SCB_CCR_UNALIGN_TRP_Msk;
    *((volatile unsigned int *) 0xFFFFFFF1) = 1;
    for (;;);
}

void platform_assert(const char *expr, const char *file, int line)
{
    __asm("cpsid i");
    SCB->CCR |=  SCB_CCR_UNALIGN_TRP_Msk;
    assert_expr.is_valid = true;
    assert_expr.expr = expr;
    assert_expr.file = file;
    assert_expr.line = line;
    *((volatile unsigned int *) 0xFFFFFFF1) = 1;
    for (;;);
}

#if defined (__CC_ARM) || defined (__ICCARM__)

void __aeabi_assert(const char *expr, const char *file, int line)
{
    platform_assert(expr, file, line);
}

#endif /* __CC_ARM */

bool exception_register_callbacks(exception_config_type *cb)
{
    if (exception_config.items >= MAX_EXCEPTION_CONFIGURATIONS) {
       return false;
    } else {
       exception_config.configs[exception_config.items].init_cb = cb->init_cb;
       exception_config.configs[exception_config.items].dump_cb = cb->dump_cb;
       exception_config.items++;
       return true;
    }
}

#if defined (__ICCARM__)

#define __EXHDLR_INIT__
#pragma location=".ram_code"
void exception_init(void);

#else

#if (PRODUCT_VERSION == 2523)
#define __EXHDLR_INIT__   __attribute__((section(".ram_code")))
#endif

#if (PRODUCT_VERSION == 7687) || (PRODUCT_VERSION == 7697)
#define __EXHDLR_INIT__
#endif

#endif

__EXHDLR_INIT__ void exception_init(void)
{
    int i;

    SCB->CCR &= ~SCB_CCR_UNALIGN_TRP_Msk;

#if (configUSE_FLASH_SUSPEND == 1)
    Flash_ReturnReady();
#endif

    for (i = 0; i < exception_config.items; i++) {
        if (exception_config.configs[i].init_cb) {
            exception_config.configs[i].init_cb();
        }
    }

    if (assert_expr.is_valid) {
        printf("assert failed: %s, file: %s, line: %d\n\r",
               assert_expr.expr,
               assert_expr.file,
               assert_expr.line);
    }
}

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

void printUsageErrorMsg(uint32_t CFSRValue)
{
    printf("Usage fault: ");
    CFSRValue >>= 16; /* right shift to lsb */
    if ((CFSRValue & (1 << 9)) != 0) {
        printf("Divide by zero\n\r");
    }
    if ((CFSRValue & (1 << 8)) != 0) {
        printf("Unaligned access\n\r");
    }
    if ((CFSRValue & (1 << 3)) != 0) {
        printf("Coprocessor error\n\r");
    }
    if ((CFSRValue & (1 << 2)) != 0) {
        printf("Invalid EXC_RETURN\n\r");
    }
    if ((CFSRValue & (1 << 1)) != 0) {
        printf("Invalid state\n\r");
    }
    if ((CFSRValue & (1 << 0)) != 0) {
        printf("Undefined instruction\n\r");
    }
}

void printMemoryManagementErrorMsg(uint32_t CFSRValue)
{
    printf("Memory Management fault: ");
    CFSRValue &= 0x000000FF; /* mask mem faults */
    if ((CFSRValue & (1 << 5)) != 0) {
        printf("A MemManage fault occurred during FP lazy state preservation\n\r");
    }
    if ((CFSRValue & (1 << 4)) != 0) {
        printf("A derived MemManage fault occurred on exception entry\n\r");
    }
    if ((CFSRValue & (1 << 3)) != 0) {
        printf("A derived MemManage fault occurred on exception return\n\r");
    }
    if ((CFSRValue & (1 << 1)) != 0) { /* Need to check valid bit (bit 7 of CFSR)? */
        printf("Data access violation @0x%08x\n\r", (unsigned int)SCB->MMFAR);
    }
    if ((CFSRValue & (1 << 0)) != 0) {
        printf("MPU or Execute Never (XN) default memory map access violation\n\r");
    }
    if ((CFSRValue & (1 << 7)) != 0) { /* To review: remove this if redundant */
        printf("SCB->MMFAR = 0x%08x\n\r", (unsigned int)SCB->MMFAR );
    }
}

void printBusFaultErrorMsg(uint32_t CFSRValue)
{
    printf("Bus fault: ");
    CFSRValue &= 0x0000FF00; /* mask bus faults */
    CFSRValue >>= 8;
    if ((CFSRValue & (1 << 5)) != 0) {
        printf("A bus fault occurred during FP lazy state preservation\n\r");
    }
    if ((CFSRValue & (1 << 4)) != 0) {
        printf("A derived bus fault has occurred on exception entry\n\r");
    }
    if ((CFSRValue & (1 << 3)) != 0) {
        printf("A derived bus fault has occurred on exception return\n\r");
    }
    if ((CFSRValue & (1 << 2)) != 0) {
        printf("Imprecise data access error has occurred\n\r");
    }
    if ((CFSRValue & (1 << 1)) != 0) { /* Need to check valid bit (bit 7 of CFSR)? */
        printf("A precise data access error has occurred @x%08x\n\r", (unsigned int)SCB->BFAR);
    }
    if ((CFSRValue & (1 << 0)) != 0) {
        printf("A bus fault on an instruction prefetch has occurred\n\r");
    }
    if ((CFSRValue & (1 << 7)) != 0) { /* To review: remove this if redundant */
        printf("SCB->BFAR = 0x%08x\n\r", (unsigned int)SCB->BFAR );
    }
}

enum { r0, r1, r2, r3, r12, lr, pc, psr,
       s0, s1, s2, s3, s4, s5, s6, s7,
       s8, s9, s10, s11, s12, s13, s14, s15,
       fpscr
     };

typedef struct TaskContextType {
    unsigned int r0;
    unsigned int r1;
    unsigned int r2;
    unsigned int r3;
    unsigned int r4;
    unsigned int r5;
    unsigned int r6;
    unsigned int r7;
    unsigned int r8;
    unsigned int r9;
    unsigned int r10;
    unsigned int r11;
    unsigned int r12;
    unsigned int sp;              /* after pop r0-r3, lr, pc, xpsr                   */
    unsigned int lr;              /* lr before exception                             */
    unsigned int pc;              /* pc before exception                             */
    unsigned int psr;             /* xpsr before exeption                            */
    unsigned int control;         /* nPRIV bit & FPCA bit meaningful, SPSEL bit = 0  */
    unsigned int exc_return;      /* current lr                                      */
    unsigned int msp;             /* msp                                             */
    unsigned int psp;             /* psp                                             */
    unsigned int fpscr;
    unsigned int s0;
    unsigned int s1;
    unsigned int s2;
    unsigned int s3;
    unsigned int s4;
    unsigned int s5;
    unsigned int s6;
    unsigned int s7;
    unsigned int s8;
    unsigned int s9;
    unsigned int s10;
    unsigned int s11;
    unsigned int s12;
    unsigned int s13;
    unsigned int s14;
    unsigned int s15;
    unsigned int s16;
    unsigned int s17;
    unsigned int s18;
    unsigned int s19;
    unsigned int s20;
    unsigned int s21;
    unsigned int s22;
    unsigned int s23;
    unsigned int s24;
    unsigned int s25;
    unsigned int s26;
    unsigned int s27;
    unsigned int s28;
    unsigned int s29;
    unsigned int s30;
    unsigned int s31;
} TaskContext;

static TaskContext taskContext = {0};
TaskContext *pTaskContext = &taskContext;

void stackDump(uint32_t stack[])
{
    taskContext.r0   = stack[r0];
    taskContext.r1   = stack[r1];
    taskContext.r2   = stack[r2];
    taskContext.r3   = stack[r3];
    taskContext.r12  = stack[r12];
    taskContext.sp   = ((uint32_t)stack) + 0x20;
    taskContext.lr   = stack[lr];
    taskContext.pc   = stack[pc];
    taskContext.psr  = stack[psr];

    /* FPU context? */
    if ( (taskContext.exc_return & 0x10) == 0 ) {
        taskContext.s0 = stack[s0];
        taskContext.s1 = stack[s1];
        taskContext.s2 = stack[s2];
        taskContext.s3 = stack[s3];
        taskContext.s4 = stack[s4];
        taskContext.s5 = stack[s5];
        taskContext.s6 = stack[s6];
        taskContext.s7 = stack[s7];
        taskContext.s8 = stack[s8];
        taskContext.s9 = stack[s9];
        taskContext.s10 = stack[s10];
        taskContext.s11 = stack[s11];
        taskContext.s12 = stack[s12];
        taskContext.s13 = stack[s13];
        taskContext.s14 = stack[s14];
        taskContext.s15 = stack[s15];
        taskContext.fpscr = stack[fpscr];
        taskContext.sp += 72; /* s0-s15, fpsr, reserved */
    }

    /* if CCR.STKALIGN=1, check PSR[9] to know if there is forced stack alignment */
    if ( (SCB->CCR & SCB_CCR_STKALIGN_Msk) && (taskContext.psr & 0x200)) {
        taskContext.sp += 4;
    }

    printf("r0  = 0x%08x\n\r", taskContext.r0);
    printf("r1  = 0x%08x\n\r", taskContext.r1);
    printf("r2  = 0x%08x\n\r", taskContext.r2);
    printf("r3  = 0x%08x\n\r", taskContext.r3);
    printf("r4  = 0x%08x\n\r", taskContext.r4);
    printf("r5  = 0x%08x\n\r", taskContext.r5);
    printf("r6  = 0x%08x\n\r", taskContext.r6);
    printf("r7  = 0x%08x\n\r", taskContext.r7);
    printf("r8  = 0x%08x\n\r", taskContext.r8);
    printf("r9  = 0x%08x\n\r", taskContext.r9);
    printf("r10 = 0x%08x\n\r", taskContext.r10);
    printf("r11 = 0x%08x\n\r", taskContext.r11);
    printf("r12 = 0x%08x\n\r", taskContext.r12);
    printf("lr  = 0x%08x\n\r", taskContext.lr);
    printf("pc  = 0x%08x\n\r", taskContext.pc);
    printf("psr = 0x%08x\n\r", taskContext.psr);
    printf("EXC_RET = 0x%08x\n\r", taskContext.exc_return);

    /* update CONTROL.SPSEL and psp if returning to thread mode */
    if (taskContext.exc_return & 0x4) {
        taskContext.control |= 0x2; /* CONTROL.SPSel */
        taskContext.psp = taskContext.sp;
    } else { /* update msp if returning to handler mode */
        taskContext.msp = taskContext.sp;
    }

    /* FPU context? */
    if ( (taskContext.exc_return & 0x10) == 0 ) {
        taskContext.control |= 0x4; /* CONTROL.FPCA */
        printf("s0  = 0x%08x\n\r", taskContext.s0);
        printf("s1  = 0x%08x\n\r", taskContext.s1);
        printf("s2  = 0x%08x\n\r", taskContext.s2);
        printf("s3  = 0x%08x\n\r", taskContext.s3);
        printf("s4  = 0x%08x\n\r", taskContext.s4);
        printf("s5  = 0x%08x\n\r", taskContext.s5);
        printf("s6  = 0x%08x\n\r", taskContext.s6);
        printf("s7  = 0x%08x\n\r", taskContext.s7);
        printf("s8  = 0x%08x\n\r", taskContext.s8);
        printf("s9  = 0x%08x\n\r", taskContext.s9);
        printf("s10 = 0x%08x\n\r", taskContext.s10);
        printf("s11 = 0x%08x\n\r", taskContext.s11);
        printf("s12 = 0x%08x\n\r", taskContext.s12);
        printf("s13 = 0x%08x\n\r", taskContext.s13);
        printf("s14 = 0x%08x\n\r", taskContext.s14);
        printf("s15 = 0x%08x\n\r", taskContext.s15);
        printf("s16 = 0x%08x\n\r", taskContext.s16);
        printf("s17 = 0x%08x\n\r", taskContext.s17);
        printf("s18 = 0x%08x\n\r", taskContext.s18);
        printf("s19 = 0x%08x\n\r", taskContext.s19);
        printf("s20 = 0x%08x\n\r", taskContext.s20);
        printf("s21 = 0x%08x\n\r", taskContext.s21);
        printf("s22 = 0x%08x\n\r", taskContext.s22);
        printf("s23 = 0x%08x\n\r", taskContext.s23);
        printf("s24 = 0x%08x\n\r", taskContext.s24);
        printf("s25 = 0x%08x\n\r", taskContext.s25);
        printf("s26 = 0x%08x\n\r", taskContext.s26);
        printf("s27 = 0x%08x\n\r", taskContext.s27);
        printf("s28 = 0x%08x\n\r", taskContext.s28);
        printf("s29 = 0x%08x\n\r", taskContext.s29);
        printf("s30 = 0x%08x\n\r", taskContext.s30);
        printf("s31 = 0x%08x\n\r", taskContext.s31);
        printf("fpscr = 0x%08x\n\r", taskContext.fpscr);
    }

    printf("CONTROL = 0x%08x\n\r", taskContext.control);
    printf("MSP     = 0x%08x\n\r", taskContext.msp);
    printf("PSP     = 0x%08x\n\r", taskContext.psp);
    printf("sp      = 0x%08x\n\r", taskContext.sp);
}

void memoryDumpAll(void)
{
    unsigned int *current, *end;
    unsigned int i;

    for (i = 0; ; i++) {

        if ( !memory_regions[i].region_name ) {
           break;
        }

        if ( !memory_regions[i].is_dumped ) {
           continue;
        }

        current = memory_regions[i].start_address;
        end     = memory_regions[i].end_address;

        for (; current < end; current += 4) {

            if (*(current + 0) == 0 &&
                *(current + 1) == 0 &&
                *(current + 2) == 0 &&
                *(current + 3) == 0 ) {
                    continue;
            }

            printf("0x%08x: %08x %08x %08x %08x\n\r",
                   (unsigned int)current,
                   *(current + 0),
                   *(current + 1),
                   *(current + 2),
                   *(current + 3));
         }
    }

    for (i = 0; i < exception_config.items; i++) {
        if (exception_config.configs[i].dump_cb) {
            exception_config.configs[i].dump_cb();
        }
    }

    printf("\n\rmemory dump completed.\n\r");
}

/*
 * Debug scenarios:
 *
 * (1) debug with debugger, stop in first exception.
 *     Print the exception context, and halt cpu.
 *
 *     DEBUGGER_ON: 1
 *
 * (2) debug with uart, stop in first exception.
 *     Print the exception context, and enter an infinite loop.
 *
 *     DEBUGGER_ON: 0
 */

#define DEBUGGER_ON    0

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void Hard_Fault_Handler(uint32_t stack[])
{
    printf("\n\rIn Hard Fault Handler\n\r");
    printf("SCB->HFSR = 0x%08x\n\r", (unsigned int)SCB->HFSR);
    if ((SCB->HFSR & (1 << 30)) != 0) {
        printf("Forced Hard Fault\n\r");
        printf("SCB->CFSR = 0x%08x\n\r", (unsigned int)SCB->CFSR );
        if ((SCB->CFSR & 0xFFFF0000) != 0) {
            printUsageErrorMsg(SCB->CFSR);
        }
        if ((SCB->CFSR & 0x0000FF00) != 0 ) {
            printBusFaultErrorMsg(SCB->CFSR);
        }
        if ((SCB->CFSR & 0x000000FF) != 0 ) {
            printMemoryManagementErrorMsg(SCB->CFSR);
        }
    }

    stackDump(stack);

    memoryDumpAll();

#if DEBUGGER_ON
    __ASM volatile("BKPT #01");
#else
    while (1);
#endif
}

void MemManage_Fault_Handler(uint32_t stack[])
{
    printf("\n\rIn MemManage Fault Handler\n\r");
    printf("SCB->CFSR = 0x%08x\n\r", (unsigned int)SCB->CFSR );
    if ((SCB->CFSR & 0xFF) != 0) {
        printMemoryManagementErrorMsg(SCB->CFSR);
    }

    stackDump(stack);

    memoryDumpAll();

#if DEBUGGER_ON
    __ASM volatile("BKPT #01");
#else
    while (1);
#endif
}

void Bus_Fault_Handler(uint32_t stack[])
{
    printf("\n\rIn Bus Fault Handler\n\r");
    printf("SCB->CFSR = 0x%08x\n\r", (unsigned int)SCB->CFSR );
    if ((SCB->CFSR & 0xFF00) != 0) {
        printBusFaultErrorMsg(SCB->CFSR);
    }

    stackDump(stack);

    memoryDumpAll();

#if DEBUGGER_ON
    __ASM volatile("BKPT #01");
#else
    while (1);
#endif
}

void Usage_Fault_Handler(uint32_t stack[])
{
    printf("\n\rIn Usage Fault Handler\n\r");
    printf("SCB->CFSR = 0x%08x\n\r", (unsigned int)SCB->CFSR);
    if ((SCB->CFSR & 0xFFFF0000) != 0) {
        printUsageErrorMsg(SCB->CFSR);
    }

    stackDump(stack);

    memoryDumpAll();

#if DEBUGGER_ON
    __ASM volatile("BKPT #01");
#else
    while (1);
#endif
}

/******************************************************************************/
/*                   Toolchain Dependent Part                                 */
/******************************************************************************/
#if defined(__GNUC__)

#if (PRODUCT_VERSION == 2523)
#define __EXHDLR_ATTR__   __attribute__((naked, section(".ram_code")))
#endif

#if (PRODUCT_VERSION == 7687) || (PRODUCT_VERSION == 7697)
#define __EXHDLR_ATTR__   __attribute__((naked))
#endif

/**
  * @brief  This function is the common part of exception handlers.
  * @param  r3 holds EXC_RETURN value
  * @retval None
  */
__EXHDLR_ATTR__ void CommonFault_Handler(void)
{
    __asm volatile
    (
        "cpsid i                       \n"     /* disable irq                 */
        "push {r3, lr}                 \n"
        "bl exception_init             \n"
        "pop  {r3, lr}                 \n"
        "ldr r0, =pTaskContext         \n"
        "ldr r0, [r0]                  \n"
        "add r0, r0, #16               \n"     /* point to context.r4         */
        "stmia r0!, {r4-r11}           \n"     /* store r4-r11                */
        "add r0, r0, #20               \n"     /* point to context.control    */
        "mrs r1, control               \n"     /* move CONTROL to r1          */
        "str r1, [r0], #4              \n"     /* store CONTROL               */
        "str r3, [r0], #4              \n"     /* store EXC_RETURN            */
        "mrs r1, msp                   \n"     /* move MSP to r1              */
        "str r1, [r0], #4              \n"     /* store MSP                   */
        "mrs r1, psp                   \n"     /* move PSP to r1              */
        "str r1, [r0]                  \n"     /* store PSP                   */
        "tst r3, #0x10                 \n"     /* FPU context?                */
        "itt eq                        \n"
        "addeq r0, r0, #68             \n"     /* point to contex.s16         */
        "vstmeq r0, {s16-s31}          \n"     /* store r16-r31               */
        "tst r3, #4                    \n"     /* thread or handler mode?     */
        "ite eq                        \n"
        "mrseq r0, msp                 \n"
        "mrsne r0, psp                 \n"
        "bx lr                         \n"
    );
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
__EXHDLR_ATTR__ void HardFault_Handler(void)
{
    __asm volatile
    (
        "mov r3, lr                    \n"
        "bl CommonFault_Handler        \n"
        "bl Hard_Fault_Handler         \n"
    );
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
__EXHDLR_ATTR__ void MemManage_Handler(void)
{
    __asm volatile
    (
        "mov r3, lr                    \n"
        "bl CommonFault_Handler        \n"
        "bl MemManage_Fault_Handler    \n"
    );
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
__EXHDLR_ATTR__ void BusFault_Handler(void)
{
    __asm volatile
    (
        "mov r3, lr                    \n"
        "bl CommonFault_Handler        \n"
        "bl Bus_Fault_Handler          \n"
    );
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
__EXHDLR_ATTR__ void UsageFault_Handler(void)
{
    __asm volatile
    (
        "mov r3, lr                    \n"
        "bl CommonFault_Handler        \n"
        "bl Usage_Fault_Handler        \n"
    );
}

#endif /* __GNUC__ */

#if defined (__CC_ARM)

#if (PRODUCT_VERSION == 2523)
#define __EXHDLR_ATTR__   __asm __attribute__((section(".ram_code")))
#endif

#if (PRODUCT_VERSION == 7687) || (PRODUCT_VERSION == 7697)
#define __EXHDLR_ATTR__ __asm
#endif

/**
  * @brief  This function is the common part of exception handlers.
  * @param  r3 holds EXC_RETURN value
  * @retval None
  */
__EXHDLR_ATTR__ void CommonFault_Handler(void)
{
    extern pTaskContext

    PRESERVE8

    cpsid i                       /* disable irq                 */
    push {r3, lr}
    bl __cpp(exception_init)
    pop  {r3, lr}
    ldr r0, =pTaskContext
    ldr r0, [r0]
    add r0, r0, #16               /* point to context.r4          */
    stmia r0!, {r4-r11}           /* store r4-r11                 */
    add r0, r0, #20               /* point to context.control     */
    mrs r1, control               /* move CONTROL to r1           */
    str r1, [r0], #4              /* store CONTROL                */
    str r3, [r0], #4              /* store EXC_RETURN             */
    mrs r1, msp                   /* move MSP to r1               */
    str r1, [r0], #4              /* store MSP                    */
    mrs r1, psp                   /* move PSP to r1               */
    str r1, [r0]                  /* store PSP                    */
    tst r3, #0x10                 /* FPU context?                 */
    itt eq
    addeq r0, r0, #68             /* point to contex.s16          */
    vstmeq r0, {s16-s31}          /* store r16-r31                */
    tst r3, #4                    /* thread or handler mode?      */
    ite eq
    mrseq r0, msp
    mrsne r0, psp
    bx lr
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
__EXHDLR_ATTR__ void HardFault_Handler(void)
{
    PRESERVE8

    mov r3, lr
    bl __cpp(CommonFault_Handler)
    bl __cpp(Hard_Fault_Handler)
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
__EXHDLR_ATTR__ void MemManage_Handler(void)
{
    PRESERVE8

    mov r3, lr
    bl __cpp(CommonFault_Handler)
    bl __cpp(MemManage_Fault_Handler)
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
__EXHDLR_ATTR__ void BusFault_Handler(void)
{
    PRESERVE8

    mov r3, lr
    bl __cpp(CommonFault_Handler)
    bl __cpp(Bus_Fault_Handler)
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
__EXHDLR_ATTR__ void UsageFault_Handler(void)
{
    PRESERVE8

    mov r3, lr
    bl __cpp(CommonFault_Handler)
    bl __cpp(Usage_Fault_Handler)
}

#endif /* __CC_ARM */

#if defined (__ICCARM__)

#define __EXHDLR_ATTR__  __stackless

#pragma location=".ram_code"
TaskContext* get_pTaskContext(void);

#pragma location=".ram_code"
void CommonFault_Handler(void);

#pragma location=".ram_code"
void HardFault_Handler(void);

#pragma location=".ram_code"
void MemManage_Handler(void);

#pragma location=".ram_code"
void BusFault_Handler(void);

#pragma location=".ram_code"
void UsageFault_Handler(void);

/**
  * @brief  Get pTaskContext
  * @param  None
  * @retval R0 = pTaskContext
  */
TaskContext* get_pTaskContext(void)
{
    /* [IAR porting issue]
     * 1. asm("ldr r0, =pTaskContext")
     *    Not compiled ok
     * 2. asm("ldr r0, [%0]" :: "r"(pTaskContext))
     *    The generated assembly for the function is not all correct.
     *
     * [Patch]
     * Add a new function to return the pTaskContext in R0 (AAPCS)
     */
    return pTaskContext;
}

/**
  * @brief  This function is the common part of exception handlers.
  * @param  r3 holds EXC_RETURN value
  * @retval None
  */
__EXHDLR_ATTR__ void CommonFault_Handler(void)
{
    __asm volatile
    (
        "cpsid i                       \n"     /* disable irq                 */
        "push {r3, lr}                 \n"
        "bl exception_init             \n"
        "bl get_pTaskContext           \n"
        "pop  {r3, lr}                 \n"     /* R0 = pTaskContext           */
        "add r0, r0, #16               \n"     /* point to context.r4         */
        "stmia r0!, {r4-r11}           \n"     /* store r4-r11                */
        "add r0, r0, #20               \n"     /* point to context.control    */
        "mrs r1, control               \n"     /* move CONTROL to r1          */
        "str r1, [r0], #4              \n"     /* store CONTROL               */
        "str r3, [r0], #4              \n"     /* store EXC_RETURN            */
        "mrs r1, msp                   \n"     /* move MSP to r1              */
        "str r1, [r0], #4              \n"     /* store MSP                   */
        "mrs r1, psp                   \n"     /* move PSP to r1              */
        "str r1, [r0]                  \n"     /* store PSP                   */
        "tst r3, #0x10                 \n"     /* FPU context?                */
        "itt eq                        \n"
        "addeq r0, r0, #68             \n"     /* point to contex.s16         */
        "vstmeq r0, {s16-s31}          \n"     /* store r16-r31               */
        "tst r3, #4                    \n"     /* thread or handler mode?     */
        "ite eq                        \n"
        "mrseq r0, msp                 \n"
        "mrsne r0, psp                 \n"
        "bx lr                         \n"
    );
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
__EXHDLR_ATTR__ void HardFault_Handler(void)
{
    __asm volatile
    (
        "mov r3, lr                    \n"
        "bl CommonFault_Handler        \n"
        "bl Hard_Fault_Handler         \n"
    );
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
__EXHDLR_ATTR__ void MemManage_Handler(void)
{
    __asm volatile
    (
        "mov r3, lr                    \n"
        "bl CommonFault_Handler        \n"
        "bl MemManage_Fault_Handler    \n"
    );
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
__EXHDLR_ATTR__ void BusFault_Handler(void)
{
    __asm volatile
    (
        "mov r3, lr                    \n"
        "bl CommonFault_Handler        \n"
        "bl Bus_Fault_Handler          \n"
    );
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
__EXHDLR_ATTR__ void UsageFault_Handler(void)
{
    __asm volatile
    (
        "mov r3, lr                    \n"
        "bl CommonFault_Handler        \n"
        "bl Usage_Fault_Handler        \n"
    );
}

#endif /* __ICCARM__ */
