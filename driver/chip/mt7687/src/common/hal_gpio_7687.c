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

#include "hal_eint.h"
#include "hal_gpio.h"
#if defined(HAL_EINT_MODULE_ENABLED) || defined(HAL_GPIO_MODULE_ENABLED)

#include <stdio.h>
#include "type_def.h"
#include "hal_gpio_7687.h"
#include "nvic.h"
#include "hal_pinmux.h"
#include "hal_nvic.h"
#include "pinmux.h"

INT32 halGPIO_ConfDirection(UINT32 GPIO_pin, UINT8 outEnable)
{

    UINT16 no;
    UINT16 remainder;
    no = GPIO_pin / 32;
    remainder = GPIO_pin % 32;
    switch (no) {
        case 0:

            //config GPIO direction
            if (outEnable == 1) {
                if (remainder <= 26) {
                    DRV_Reg32(IOT_GPIO_AON_BASE + IOT_GPIO_IES0) &= ~(1 << remainder);
                }

                DRV_WriteReg32(IOT_GPIO_AON_BASE + IOT_GPIO_OE1_SET, (1 << remainder));
            } else {

                if (remainder <= 26) {
                    DRV_Reg32(IOT_GPIO_AON_BASE + IOT_GPIO_IES0) |= (1 << remainder);
                }

                DRV_WriteReg32(IOT_GPIO_AON_BASE + IOT_GPIO_OE1_RESET, (1 << remainder));
            }
            break;
        case 1:

            //config GPIO direction
            if (outEnable == 1) {
                if (remainder > 0) {
                    DRV_Reg32(IOT_GPIO_AON_BASE + IOT_GPIO_IES1) &= ~(1 << remainder);
                }

                DRV_WriteReg32(IOT_GPIO_AON_BASE + IOT_GPIO_OE2_SET, (1 << remainder));
            } else {

                if (remainder > 0) {
                    DRV_Reg32(IOT_GPIO_AON_BASE + IOT_GPIO_IES1) |= (1 << remainder);
                }

                DRV_WriteReg32(IOT_GPIO_AON_BASE + IOT_GPIO_OE2_RESET, (1 << remainder));
            }
            break;
        default:
            return -1;
    }
    return 0;
}


INT32 halGPO_Write (UINT32 GPIO_pin, UINT8 writeValue)
{
    UINT16 no;
    UINT16 remainder;
    no = GPIO_pin / 32;
    remainder = GPIO_pin % 32;
    switch (no) {
        case 0:

            if (writeValue) {
                DRV_WriteReg32(IOT_GPIO_AON_BASE + IOT_GPIO_DOUT1_SET, (1 << remainder));
            } else {
                DRV_WriteReg32(IOT_GPIO_AON_BASE + IOT_GPIO_DOUT1_RESET, (1 << remainder));
            }
            break;
        case 1:

            if (writeValue) {
                DRV_WriteReg32(IOT_GPIO_AON_BASE + IOT_GPIO_DOUT2_SET, (1 << remainder));
            } else {
                DRV_WriteReg32(IOT_GPIO_AON_BASE + IOT_GPIO_DOUT2_RESET, (1 << remainder));
            }
            break;
        default:
            return -1;
    }
    return 0;
}





UINT8 halgetGPIO_DOUT(UINT32 GPIO_pin)
{
    UINT16 no;
    UINT16 remainder;
    no = GPIO_pin / 32;
    remainder = GPIO_pin % 32;
    UINT8 dout = 0;

    switch (no) {
        case 0:
            dout = ((DRV_Reg32(IOT_GPIO_AON_BASE + IOT_GPIO_DOUT1) >> remainder) & 1);
            break;
        case 1:
            dout = ((DRV_Reg32(IOT_GPIO_AON_BASE + IOT_GPIO_DOUT2) >> remainder) & 1);
            break;
        default:
            return 0;
    }
    return dout ;
}
UINT8 halgetGPIO_DIN(UINT32 GPIO_pin)
{
    UINT16 no;
    UINT16 remainder;
    no = GPIO_pin / 32;
    remainder = GPIO_pin % 32;
    UINT8 din = 0;
    switch (no) {

        case 0:
            din = ((DRV_Reg32(IOT_GPIO_AON_BASE + IOT_GPIO_DIN1) >> remainder) & 1);
            break;
        case 1:
            din = ((DRV_Reg32(IOT_GPIO_AON_BASE + IOT_GPIO_DIN2) >> remainder) & 1);
            break;
        default:
            return 0;

    }
    return din;
}
UINT8 halgetGPIO_OutEnable(UINT32 GPIO_pin)
{
    UINT16 no;
    UINT16 remainder;
    no = GPIO_pin / 32;
    remainder = GPIO_pin % 32;
    UINT8 outEnable = 0;

    switch (no) {
        case 0:
            outEnable = ((DRV_Reg32(IOT_GPIO_AON_BASE + IOT_GPIO_OE1) >> remainder) & 1);
            break;
        case 1:
            outEnable = ((DRV_Reg32(IOT_GPIO_AON_BASE + IOT_GPIO_OE2) >> remainder) & 1);
            break;
        default:
            return 0;
    }
    return outEnable;
}


INT32 halGPIO_PullUp_SET(UINT32 GPIO_pin)
{
    UINT16 no;
    UINT16 remainder;
    no = GPIO_pin / 32;
    remainder = GPIO_pin % 32;
    halGPIO_PullDown_RESET(GPIO_pin);
    switch (no) {
        case 0:
            if (remainder <= 26) {
                DRV_WriteReg32(IOT_GPIO_AON_BASE +  IOT_GPIO_PU1_SET, (1 << remainder));
            }
            break;
        case 1:
            DRV_WriteReg32(IOT_GPIO_AON_BASE +  IOT_GPIO_PU2_SET, (1 << remainder));
            break;
        default:
            return -1;
    }
    return 0;
}

INT32 halGPIO_PullUp_RESET(UINT32 GPIO_pin)
{
    UINT16 no;
    UINT16 remainder;
    no = GPIO_pin / 32;
    remainder = GPIO_pin % 32;

    switch (no) {
        case 0:
            if (remainder <= 26) {
                DRV_WriteReg32(IOT_GPIO_AON_BASE +  IOT_GPIO_PU1_RESET, (1 << remainder));
            }
            break;
        case 1:
            DRV_WriteReg32(IOT_GPIO_AON_BASE +  IOT_GPIO_PU2_RESET, (1 << remainder));
            break;
        default:
            return -1;
    }
    return 0;
}

INT32 halGPIO_PullDown_SET(UINT32 GPIO_pin)
{
    UINT16 no;
    UINT16 remainder;
    no = GPIO_pin / 32;
    remainder = GPIO_pin % 32;
    halGPIO_PullUp_RESET(GPIO_pin);
    switch (no) {
        case 0:
            if (remainder <= 26) {
                DRV_WriteReg32(IOT_GPIO_AON_BASE +  IOT_GPIO_PD1_SET, (1 << remainder));
            }
            break;
        case 1:
            DRV_WriteReg32(IOT_GPIO_AON_BASE +  IOT_GPIO_PD2_SET, (1 << remainder));
            break;
        default:
            return -1;

    }
    return 0;
}

INT32 halGPIO_PullDown_RESET(UINT32 GPIO_pin)
{

    UINT16 no;
    UINT16 remainder;
    no = GPIO_pin / 32;
    remainder = GPIO_pin % 32;

    switch (no) {
        case 0:
            if (remainder <= 26) {
                DRV_WriteReg32(IOT_GPIO_AON_BASE +  IOT_GPIO_PD1_RESET, (1 << remainder));
            }
            break;
        case 1:
            DRV_WriteReg32(IOT_GPIO_AON_BASE +  IOT_GPIO_PD2_RESET, (1 << remainder));
            break;
        default:
            return -1;

    }
    return 0;
}


void topPAD_Control_GPIO(UINT32 GPIO_pin, ENUM_TOP_PAD_CONTROL_T ePadControl)
{

    UINT16 no;
    UINT16 remainder;
    no = GPIO_pin / 32;
    remainder = GPIO_pin % 32;

    if (ePAD_CONTROL_BY_CM4 == ePadControl) {

        switch (no) {
            case 0:
                DRV_Reg32(IOT_GPIO_PAD_CTRL0) |= (1 << remainder);
                break;
            case 1:
                DRV_Reg32(IOT_GPIO_PAD_CTRL1) |= (1 << remainder);
                break;
            default:
                break;
        }

    } else if (ePAD_CONTROL_BY_N9 == ePadControl) {
        switch (no) {
            case 0:
                DRV_Reg32(IOT_GPIO_PAD_CTRL0) &= ~(1 << remainder);
                break;
            case 1:
                DRV_Reg32(IOT_GPIO_PAD_CTRL1) &= ~(1 << remainder);
                break;
            default:
                break;
        }

    }
}


#if 1

static GPI_LISR_Struct LISR;

void drvGPIO_IDX_0_LISR(hal_nvic_irq_t irq_number)
{


    mSetHWEntry(IOT_GPIO0_NVIC_CLR_PEND, 1);
    LISR.gpiLisr_func.GPIO_IDX_0_LISR((ENUM_IOT_GPIO_NAME_T)ePAD_ANTSEL0);

    //mSetHWEntry(CM4_WIC_SW_CLR, 1);   //this pin has no NVIC wake up function, so no need to clear WIC pending/enable status
    //mSetHWEntry(CM4_WIC_SW_CLR, 0);
}

void drvGPIO_IDX_1_LISR(hal_nvic_irq_t irq_number)
{


    mSetHWEntry(IOT_GPIO1_NVIC_CLR_PEND, 1);
    LISR.gpiLisr_func.GPIO_IDX_1_LISR((ENUM_IOT_GPIO_NAME_T)ePAD_ANTSEL1);


    //mSetHWEntry(CM4_WIC_SW_CLR, 1);   //this pin has no NVIC wake up function, so no need to clear WIC pending/enable status
    //mSetHWEntry(CM4_WIC_SW_CLR, 0);
}

void drvGPIO_IDX_3_LISR(hal_nvic_irq_t irq_number)
{


    mSetHWEntry(IOT_GPIO3_NVIC_CLR_PEND, 1);

    LISR.gpiLisr_func.GPIO_IDX_3_LISR((ENUM_IOT_GPIO_NAME_T)ePAD_ANTSEL3);


    //mSetHWEntry(CM4_WIC_SW_CLR, 1);   //this pin has no NVIC wake up function, so no need to clear WIC pending/enable status
    //mSetHWEntry(CM4_WIC_SW_CLR, 0);
}

void drvGPIO_IDX_4_LISR(hal_nvic_irq_t irq_number)
{


    mSetHWEntry(IOT_GPIO4_NVIC_CLR_PEND, 1);

    LISR.gpiLisr_func.GPIO_IDX_4_LISR((ENUM_IOT_GPIO_NAME_T)ePAD_ANTSEL4);


    //mSetHWEntry(CM4_WIC_SW_CLR, 1);   //this pin has no NVIC wake up function, so no need to clear WIC pending/enable status
    //mSetHWEntry(CM4_WIC_SW_CLR, 0);
}


void drvGPIO_IDX_5_LISR(hal_nvic_irq_t irq_number)
{


    mSetHWEntry(IOT_GPIO5_NVIC_CLR_PEND, 1);

    LISR.gpiLisr_func.GPIO_IDX_5_LISR((ENUM_IOT_GPIO_NAME_T)ePAD_ANTSEL5);


    //mSetHWEntry(CM4_WIC_SW_CLR, 1);   //this pin has no NVIC wake up function, so no need to clear WIC pending/enable status
    //mSetHWEntry(CM4_WIC_SW_CLR, 0);
}

void drvGPIO_IDX_6_LISR(hal_nvic_irq_t irq_number)
{


    mSetHWEntry(IOT_GPIO6_NVIC_CLR_PEND, 1);

    LISR.gpiLisr_func.GPIO_IDX_6_LISR((ENUM_IOT_GPIO_NAME_T)ePAD_ANTSEL6);


    //mSetHWEntry(CM4_WIC_SW_CLR, 1);   //this pin has no NVIC wake up function, so no need to clear WIC pending/enable status
    //mSetHWEntry(CM4_WIC_SW_CLR, 0);
}

void drvGPIO_IDX_7_LISR(hal_nvic_irq_t irq_number)
{


    mSetHWEntry(IOT_GPIO7_NVIC_CLR_PEND, 1);

    LISR.gpiLisr_func.GPIO_IDX_7_LISR((ENUM_IOT_GPIO_NAME_T)ePAD_ANTSEL7);


    //mSetHWEntry(CM4_WIC_SW_CLR, 1);   //this pin has no NVIC wake up function, so no need to clear WIC pending/enable status
    //mSetHWEntry(CM4_WIC_SW_CLR, 0);
}

void drvGPIO_IDX_35_LISR(hal_nvic_irq_t irq_number)
{


    mSetHWEntry(IOT_GPIO35_NVIC_CLR_PEND, 1);

    LISR.gpiLisr_func.GPIO_IDX_35_LISR((ENUM_IOT_GPIO_NAME_T)ePAD_UART_DBG);


    //mSetHWEntry(CM4_WIC_SW_CLR, 1);   //this pin has no NVIC wake up function, so no need to clear WIC pending/enable status
    //mSetHWEntry(CM4_WIC_SW_CLR, 0);
}

void drvGPIO_IDX_37_LISR(hal_nvic_irq_t irq_number)
{


    mSetHWEntry(IOT_GPIO37_NVIC_CLR_PEND, 1);

    LISR.gpiLisr_func.GPIO_IDX_37_LISR((ENUM_IOT_GPIO_NAME_T)ePAD_UART_TX);


    //mSetHWEntry(CM4_WIC_SW_CLR, 1);   //this pin has no NVIC wake up function, so no need to clear WIC pending/enable status
    //mSetHWEntry(CM4_WIC_SW_CLR, 0);
}


void drvGPIO_IDX_38_LISR(hal_nvic_irq_t irq_number)
{


    mSetHWEntry(IOT_GPIO38_NVIC_CLR_PEND, 1);

    LISR.gpiLisr_func.GPIO_IDX_38_LISR((ENUM_IOT_GPIO_NAME_T)ePAD_UART_RTS);


    //mSetHWEntry(CM4_WIC_SW_CLR, 1);   //this pin has no NVIC wake up function, so no need to clear WIC pending/enable status
    //mSetHWEntry(CM4_WIC_SW_CLR, 0);
}

void drvGPIO_IDX_39_LISR(hal_nvic_irq_t irq_number)
{


    mSetHWEntry(IOT_GPIO39_NVIC_CLR_PEND, 1);

    LISR.gpiLisr_func.GPIO_IDX_39_LISR((ENUM_IOT_GPIO_NAME_T)ePAD_UART_CTS);


    //mSetHWEntry(CM4_WIC_SW_CLR, 1);   //this pin has no NVIC wake up function, so no need to clear WIC pending/enable status
    //mSetHWEntry(CM4_WIC_SW_CLR, 0);
}

INT32 halRegGPInterrupt( UINT32 pad_name,
                         UINT8 edgeLevelTrig,
                         UINT8 debounceEnable,
                         UINT8 polarity,
                         UINT8 dual,
                         UINT8 prescaler,
                         UINT8 u1PrescalerCount,
                         void (*USER_GPI_Handler)(ENUM_IOT_GPIO_NAME_T pin))
{

    switch (pad_name) {
        case ePAD_ANTSEL0:

            NVIC_DisableIRQ((IRQn_Type)CM4_EINT_0_IRQ);
            halPinmuxTopOnPadFunc(ePAD_ANTSEL0, 3);  //set value 3 is for EINT
            mSetHWEntry(IOT_GPIO0_DEBOUNCE_CNT, u1PrescalerCount);
            mSetHWEntry(IOT_GPIO0_DEBOUNCE_PRESCALER, prescaler);
            mSetHWEntry(IOT_GPIO0_DEBOUNCE_DUAL, dual);
            mSetHWEntry(IOT_GPIO0_DEBOUNCE_POLARITY, polarity);
            mSetHWEntry(IOT_GPIO0_DEBOUNCE_EN, debounceEnable);
            mSetHWEntry(IOT_GPIO0_NVIC_SENSE, edgeLevelTrig);

            //clear pending status?
            mSetHWEntry(IOT_GPIO0_NVIC_CLR_PEND, 1);   //DE Ethan suggest sequence

            //mSetHWEntry(IOT_GPIO0_NVIC_EN, 1);   //DE Ethan suggest sequence
            LISR.gpiLisr_func.GPIO_IDX_0_LISR = USER_GPI_Handler;
            hal_nvic_register_isr_handler((IRQn_Type)CM4_EINT_0_IRQ, drvGPIO_IDX_0_LISR);
            NVIC_SetPriority((IRQn_Type)CM4_EINT_0_IRQ, CM4_EINT_0_PRI);
            NVIC_EnableIRQ((IRQn_Type)CM4_EINT_0_IRQ);
            break;

        case ePAD_ANTSEL1:

            NVIC_DisableIRQ((IRQn_Type)CM4_EINT_1_IRQ);
            halPinmuxTopOnPadFunc(ePAD_ANTSEL1, 3);  //set value 3 is for EINT
            mSetHWEntry(IOT_GPIO1_DEBOUNCE_CNT, u1PrescalerCount);
            mSetHWEntry(IOT_GPIO1_DEBOUNCE_PRESCALER, prescaler);
            mSetHWEntry(IOT_GPIO1_DEBOUNCE_DUAL, dual);
            mSetHWEntry(IOT_GPIO1_DEBOUNCE_POLARITY, polarity);
            mSetHWEntry(IOT_GPIO1_DEBOUNCE_EN, debounceEnable);
            mSetHWEntry(IOT_GPIO1_NVIC_SENSE, edgeLevelTrig);

            //clear pending status?
            mSetHWEntry(IOT_GPIO1_NVIC_CLR_PEND, 1);   //DE Ethan suggest sequence

            //mSetHWEntry(IOT_GPIO1_NVIC_EN, 1);   //DE Ethan suggest sequence
            LISR.gpiLisr_func.GPIO_IDX_1_LISR = USER_GPI_Handler;
            hal_nvic_register_isr_handler((IRQn_Type)CM4_EINT_1_IRQ, drvGPIO_IDX_1_LISR);
            NVIC_SetPriority((IRQn_Type)CM4_EINT_1_IRQ, CM4_EINT_1_PRI);
            NVIC_EnableIRQ((IRQn_Type)CM4_EINT_1_IRQ);
            break;

        case ePAD_ANTSEL3:

            NVIC_DisableIRQ((IRQn_Type)CM4_EINT_2_IRQ);
            halPinmuxTopOnPadFunc(ePAD_ANTSEL3, 3);  //set value 3 is for EINT
            mSetHWEntry(IOT_GPIO3_DEBOUNCE_CNT, u1PrescalerCount);
            mSetHWEntry(IOT_GPIO3_DEBOUNCE_PRESCALER, prescaler);
            mSetHWEntry(IOT_GPIO3_DEBOUNCE_DUAL, dual);
            mSetHWEntry(IOT_GPIO3_DEBOUNCE_POLARITY, polarity);
            mSetHWEntry(IOT_GPIO3_DEBOUNCE_EN, debounceEnable);
            mSetHWEntry(IOT_GPIO3_NVIC_SENSE, edgeLevelTrig);

            //clear pending status?
            mSetHWEntry(IOT_GPIO3_NVIC_CLR_PEND, 1);   //DE Ethan suggest sequence

            //mSetHWEntry(IOT_GPIO3_NVIC_EN, 1);   //DE Ethan suggest sequence
            LISR.gpiLisr_func.GPIO_IDX_3_LISR = USER_GPI_Handler;
            hal_nvic_register_isr_handler((IRQn_Type)CM4_EINT_2_IRQ, drvGPIO_IDX_3_LISR);
            NVIC_SetPriority((IRQn_Type)CM4_EINT_2_IRQ, CM4_EINT_2_PRI);
            NVIC_EnableIRQ((IRQn_Type)CM4_EINT_2_IRQ);
            break;

        case ePAD_ANTSEL4:

            NVIC_DisableIRQ((IRQn_Type)CM4_EINT_3_IRQ);
            halPinmuxTopOnPadFunc(ePAD_ANTSEL4, 3);  //set value 3 is for EINT
            mSetHWEntry(IOT_GPIO4_DEBOUNCE_CNT, u1PrescalerCount);
            mSetHWEntry(IOT_GPIO4_DEBOUNCE_PRESCALER, prescaler);
            mSetHWEntry(IOT_GPIO4_DEBOUNCE_DUAL, dual);
            mSetHWEntry(IOT_GPIO4_DEBOUNCE_POLARITY, polarity);
            mSetHWEntry(IOT_GPIO4_DEBOUNCE_EN, debounceEnable);
            mSetHWEntry(IOT_GPIO4_NVIC_SENSE, edgeLevelTrig);

            //clear pending status?
            mSetHWEntry(IOT_GPIO4_NVIC_CLR_PEND, 1);   //DE Ethan suggest sequence

            //mSetHWEntry(IOT_GPIO4_NVIC_EN, 1);   //DE Ethan suggest sequence
            LISR.gpiLisr_func.GPIO_IDX_4_LISR = USER_GPI_Handler;
            hal_nvic_register_isr_handler((IRQn_Type)CM4_EINT_3_IRQ, drvGPIO_IDX_4_LISR);
            NVIC_SetPriority((IRQn_Type)CM4_EINT_3_IRQ, CM4_EINT_3_PRI);
            NVIC_EnableIRQ((IRQn_Type)CM4_EINT_3_IRQ);
            break;
        case ePAD_ANTSEL5:

            NVIC_DisableIRQ((IRQn_Type)CM4_EINT_4_IRQ);
            halPinmuxTopOnPadFunc(ePAD_ANTSEL5, 3);  //set value 3 is for EINT
            mSetHWEntry(IOT_GPIO5_DEBOUNCE_CNT, u1PrescalerCount);
            mSetHWEntry(IOT_GPIO5_DEBOUNCE_PRESCALER, prescaler);
            mSetHWEntry(IOT_GPIO5_DEBOUNCE_DUAL, dual);
            mSetHWEntry(IOT_GPIO5_DEBOUNCE_POLARITY, polarity);
            mSetHWEntry(IOT_GPIO5_DEBOUNCE_EN, debounceEnable);
            mSetHWEntry(IOT_GPIO5_NVIC_SENSE, edgeLevelTrig);

            //clear pending status?
            mSetHWEntry(IOT_GPIO5_NVIC_CLR_PEND, 1);   //DE Ethan suggest sequence

            //mSetHWEntry(IOT_GPIO5_NVIC_EN, 1);   //DE Ethan suggest sequence
            LISR.gpiLisr_func.GPIO_IDX_5_LISR = USER_GPI_Handler;
            hal_nvic_register_isr_handler((IRQn_Type)CM4_EINT_4_IRQ, drvGPIO_IDX_5_LISR);
            NVIC_SetPriority((IRQn_Type)CM4_EINT_4_IRQ, CM4_EINT_4_PRI);
            NVIC_EnableIRQ((IRQn_Type)CM4_EINT_4_IRQ);
            break;

        case ePAD_ANTSEL6:

            NVIC_DisableIRQ((IRQn_Type)CM4_EINT_5_IRQ);
            halPinmuxTopOnPadFunc(ePAD_ANTSEL6, 3);  //set value 3 is for EINT
            mSetHWEntry(IOT_GPIO6_DEBOUNCE_CNT, u1PrescalerCount);
            mSetHWEntry(IOT_GPIO6_DEBOUNCE_PRESCALER, prescaler);
            mSetHWEntry(IOT_GPIO6_DEBOUNCE_DUAL, dual);
            mSetHWEntry(IOT_GPIO6_DEBOUNCE_POLARITY, polarity);
            mSetHWEntry(IOT_GPIO6_DEBOUNCE_EN, debounceEnable);
            mSetHWEntry(IOT_GPIO6_NVIC_SENSE, edgeLevelTrig);

            //clear pending status?
            mSetHWEntry(IOT_GPIO6_NVIC_CLR_PEND, 1);   //DE Ethan suggest sequence

            //mSetHWEntry(IOT_GPIO6_NVIC_EN, 1);   //DE Ethan suggest sequence
            LISR.gpiLisr_func.GPIO_IDX_6_LISR = USER_GPI_Handler;
            hal_nvic_register_isr_handler((IRQn_Type)CM4_EINT_5_IRQ, drvGPIO_IDX_6_LISR);
            NVIC_SetPriority((IRQn_Type)CM4_EINT_5_IRQ, CM4_EINT_5_PRI);
            NVIC_EnableIRQ((IRQn_Type)CM4_EINT_5_IRQ);
            break;

        case ePAD_ANTSEL7:

            NVIC_DisableIRQ((IRQn_Type)CM4_EINT_6_IRQ);
            halPinmuxTopOnPadFunc(ePAD_ANTSEL7, 3);  //set value 3 is for EINT
            mSetHWEntry(IOT_GPIO7_DEBOUNCE_CNT, u1PrescalerCount);
            mSetHWEntry(IOT_GPIO7_DEBOUNCE_PRESCALER, prescaler);
            mSetHWEntry(IOT_GPIO7_DEBOUNCE_DUAL, dual);
            mSetHWEntry(IOT_GPIO7_DEBOUNCE_POLARITY, polarity);
            mSetHWEntry(IOT_GPIO7_DEBOUNCE_EN, debounceEnable);
            mSetHWEntry(IOT_GPIO7_NVIC_SENSE, edgeLevelTrig);

            //clear pending status?
            mSetHWEntry(IOT_GPIO7_NVIC_CLR_PEND, 1);   //DE Ethan suggest sequence

            //mSetHWEntry(IOT_GPIO7_NVIC_EN, 1);   //DE Ethan suggest sequence
            LISR.gpiLisr_func.GPIO_IDX_7_LISR = USER_GPI_Handler;
            hal_nvic_register_isr_handler((IRQn_Type)CM4_EINT_6_IRQ, drvGPIO_IDX_7_LISR);
            NVIC_SetPriority((IRQn_Type)CM4_EINT_6_IRQ, CM4_EINT_6_PRI);
            NVIC_EnableIRQ((IRQn_Type)CM4_EINT_6_IRQ);
            break;

        case ePAD_UART_DBG:

            NVIC_DisableIRQ((IRQn_Type)CM4_EINT_19_IRQ);
            halPinmuxTopOnPadFunc(ePAD_UART_DBG, 3);  //set value 3 is for EINT
            mSetHWEntry(IOT_GPIO35_DEBOUNCE_CNT, u1PrescalerCount);
            mSetHWEntry(IOT_GPIO35_DEBOUNCE_PRESCALER, prescaler);
            mSetHWEntry(IOT_GPIO35_DEBOUNCE_DUAL, dual);
            mSetHWEntry(IOT_GPIO35_DEBOUNCE_POLARITY, polarity);
            mSetHWEntry(IOT_GPIO35_DEBOUNCE_EN, debounceEnable);
            mSetHWEntry(IOT_GPIO35_NVIC_SENSE, edgeLevelTrig);

            //clear pending status?
            mSetHWEntry(IOT_GPIO35_NVIC_CLR_PEND, 1);   //DE Ethan suggest sequence

            //mSetHWEntry(IOT_GPIO35_NVIC_EN, 1);   //DE Ethan suggest sequence
            LISR.gpiLisr_func.GPIO_IDX_35_LISR = USER_GPI_Handler;
            hal_nvic_register_isr_handler((IRQn_Type)CM4_EINT_19_IRQ, drvGPIO_IDX_35_LISR);
            NVIC_SetPriority((IRQn_Type)CM4_EINT_19_IRQ, CM4_EINT_19_PRI);
            NVIC_EnableIRQ((IRQn_Type)CM4_EINT_19_IRQ);
            break;

        case ePAD_UART_TX:

            NVIC_DisableIRQ((IRQn_Type)CM4_EINT_20_IRQ);
            halPinmuxTopOnPadFunc(ePAD_UART_TX, 3);  //set value 3 is for EINT
            mSetHWEntry(IOT_GPIO37_DEBOUNCE_CNT, u1PrescalerCount);
            mSetHWEntry(IOT_GPIO37_DEBOUNCE_PRESCALER, prescaler);
            mSetHWEntry(IOT_GPIO37_DEBOUNCE_DUAL, dual);
            mSetHWEntry(IOT_GPIO37_DEBOUNCE_POLARITY, polarity);
            mSetHWEntry(IOT_GPIO37_DEBOUNCE_EN, debounceEnable);
            mSetHWEntry(IOT_GPIO37_NVIC_SENSE, edgeLevelTrig);

            //clear pending status?
            mSetHWEntry(IOT_GPIO37_NVIC_CLR_PEND, 1);   //DE Ethan suggest sequence

            //mSetHWEntry(IOT_GPIO37_NVIC_EN, 1);   //DE Ethan suggest sequence
            LISR.gpiLisr_func.GPIO_IDX_37_LISR = USER_GPI_Handler;
            hal_nvic_register_isr_handler((IRQn_Type)CM4_EINT_20_IRQ, drvGPIO_IDX_37_LISR);
            NVIC_SetPriority((IRQn_Type)CM4_EINT_20_IRQ, CM4_EINT_20_PRI);
            NVIC_EnableIRQ((IRQn_Type)CM4_EINT_20_IRQ);
            break;

        case ePAD_UART_RTS:

            NVIC_DisableIRQ((IRQn_Type)CM4_EINT_21_IRQ);
            halPinmuxTopOnPadFunc(ePAD_UART_RTS, 3);  //set value 3 is for EINT
            mSetHWEntry(IOT_GPIO38_DEBOUNCE_CNT, u1PrescalerCount);
            mSetHWEntry(IOT_GPIO38_DEBOUNCE_PRESCALER, prescaler);
            mSetHWEntry(IOT_GPIO38_DEBOUNCE_DUAL, dual);
            mSetHWEntry(IOT_GPIO38_DEBOUNCE_POLARITY, polarity);
            mSetHWEntry(IOT_GPIO38_DEBOUNCE_EN, debounceEnable);
            mSetHWEntry(IOT_GPIO38_NVIC_SENSE, edgeLevelTrig);

            //clear pending status?
            mSetHWEntry(IOT_GPIO38_NVIC_CLR_PEND, 1);   //DE Ethan suggest sequence

            //mSetHWEntry(IOT_GPIO38_NVIC_EN, 1);   //DE Ethan suggest sequence
            LISR.gpiLisr_func.GPIO_IDX_38_LISR = USER_GPI_Handler;
            hal_nvic_register_isr_handler((IRQn_Type)CM4_EINT_21_IRQ, drvGPIO_IDX_38_LISR);
            NVIC_SetPriority((IRQn_Type)CM4_EINT_21_IRQ, CM4_EINT_21_PRI);
            NVIC_EnableIRQ((IRQn_Type)CM4_EINT_21_IRQ);
            break;

        case ePAD_UART_CTS:

            NVIC_DisableIRQ((IRQn_Type)CM4_EINT_22_IRQ);
            halPinmuxTopOnPadFunc(ePAD_UART_CTS, 3);  //set value 3 is for EINT
            mSetHWEntry(IOT_GPIO39_DEBOUNCE_CNT, u1PrescalerCount);
            mSetHWEntry(IOT_GPIO39_DEBOUNCE_PRESCALER, prescaler);
            mSetHWEntry(IOT_GPIO39_DEBOUNCE_DUAL, dual);
            mSetHWEntry(IOT_GPIO39_DEBOUNCE_POLARITY, polarity);
            mSetHWEntry(IOT_GPIO39_DEBOUNCE_EN, debounceEnable);
            mSetHWEntry(IOT_GPIO39_NVIC_SENSE, edgeLevelTrig);

            //clear pending status?
            mSetHWEntry(IOT_GPIO39_NVIC_CLR_PEND, 1);   //DE Ethan suggest sequence

            //mSetHWEntry(IOT_GPIO39_NVIC_EN, 1);   //DE Ethan suggest sequence
            LISR.gpiLisr_func.GPIO_IDX_39_LISR = USER_GPI_Handler;
            hal_nvic_register_isr_handler((IRQn_Type)CM4_EINT_22_IRQ, drvGPIO_IDX_39_LISR);
            NVIC_SetPriority((IRQn_Type)CM4_EINT_22_IRQ, CM4_EINT_22_PRI);
            NVIC_EnableIRQ((IRQn_Type)CM4_EINT_22_IRQ);
            break;
        default:
            return -1;
    }
    return 0;
}

#endif
#endif

