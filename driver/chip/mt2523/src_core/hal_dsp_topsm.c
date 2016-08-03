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

#include "hal_dsp_topsm.h"

#ifdef HAL_SLEEP_MANAGER_ENABLED

#include "hal_cm4_topsm.h"
#include "hal_log.h"
#include "hal_ostd.h"
#include "assert.h"

extern cm4_topsm_mtcmos_control mtcmos_control;
uint32_t rm_resource_request_module = 0;

void dsp_topsm_init(void)
{
    *DSP_TOPSM_SM_SLV_REQ_IRQ = 0xF;
    *DSP_TOPSM_SM_MAS_RDY_IRQ = 0xF;
    *DSP_TOPSM_RM_TMR_PWR0 = 0x00000001;
    *DSP_TOPSM_RM_PLL_MASK0 = 0xFEFEFEFE; /*Setting TOPSM PLL mask*/ // {bis,x4g,dsp,bus}
    *DSP_TOPSM_RM_PWR_PER0 = 0x020F771F;
    *DSP_TOPSM_PROTECT_ACK_MASK = 0x00000000;
    *DSP_TOPSM_RM_PWR_CON0 &= 0xFFFFFF7F;
#ifdef HAL_DSP_TOPSMTOPSM_LOW_POWER_AUDIO
    *DSP_TOPSM_RM_PWR_CON0 = (1 << 0) // sw_pwr_rst_b
                             | (0 << 1) //sw_pwr_iso
                             | (1 << 2) //sw_pwr_on_0
                             | (0 << 3) //sw_pwr_mem_off
                             | (0 << 4) //sw_pwr_clk_dis
                             | (0 << 5) //sw_pwr_mem_prot
                             | (1 << 7) //sw_pwr_ctrl_en
                             | (0 << 16) //sw_pwr_protect_req
                             | (1 << 17) //sw_pwr_on_1
                             | (1 << 19) //sw_pwr_mem_isointb
                             | (0 << 20) //sno_pwr_off
                             ;
#else
    *DSP_TOPSM_RM_PWR_CON0 = 0x00000200;
#endif
    *DSP_TOPSM_CCF_CLK_CON |= (0x01 << 12);
    *DSP_TOPSM_SM_TRIG_SETTLE0 = 0x00000008; //SM_TRIG_SETTLE0 > 2+2+2 = RM_PLL_SETTLE+RM_PLL2_SETTLE+(MAX_PWR_SETTLE in RM_PWR_PER)
    *DSP_TOPSM_SM_REQ_MASK = 0xFFFFFFFE; // Mask for software wakeup and timer base wakeup requess to other sleep manager
    *DSP_TOPSM_RM_SM_MASK = 0xFFFFFFFE; // Specofy topsm assert reuest when timer is normal mode
    *DSP_TOPSM_RM_SM_TRG = 0xFFFFFFFF;
    *DSP_TOPSM_RM_SM_PWR = 0x00000003;
    *DSP_TOPSM_RM_SM_PLL_MASK0 = 0xFFFF0000; //Mask all request
    *DSP_TOPSM_TOPSM_DBG |= 0x1; //disable TDMA timer request
}

void hal_dsp_topsm_get_resource(hal_dsp_topsm_Resource_Module_t module, bool config)
{
    switch (module) {
        case RM_MODEM_DSP_1:
            if (config) {
                *DSP_TOPSM_SW_CLK_FORCE_ON_SET |= 0xA;
            } else {
                *DSP_TOPSM_RM_SM_PLL_MASK0 = 0xFFFFFFFF;
                *DSP_TOPSM_SW_CLK_FORCE_ON_CLR |= 0xA;
            }

            break;
        case RM_MODEM_DSP_2:
            PMIC_VCORE_VOSEL_CONFIG(PMIC_PSI_LP, PMIC_VCORE_1P1V);
            PMIC_VCORE_VOSEL_CONFIG(PMIC_PSI_S1, PMIC_VCORE_0P9V);
            if (config) {
                cm4_topsm_lock_MD2G_MTCMOS();
                *DSP_TOPSM_RM_PWR_CON0 = 0x42244;
                *MODEM_CFG_SLV_SM_WAKUP_REQ |= 0x1;
            } else {
                cm4_topsm_unlock_MD2G_MTCMOS();
                *DSP_TOPSM_RM_PWR_CON0 = 0x42200;
            }
            break;
        case RM_MODEM_DSP_3:
            if (config) {
                *DSP_TOPSM_RM_TMR_PWR0 = 0x00000001;
                *DSP_TOPSM_RM_PLL_MASK0 = 0xFEFEFEFE; /*Setting TOPSM PLL mask*/ // {bis,x4g,dsp,bus}
                *DSP_TOPSM_RM_PWR_PER0 = 0x020F771F;
                *DSP_TOPSM_RM_PWR_CON0 = 0x00000200;
                *DSP_TOPSM_CCF_CLK_CON |= (0x01 << 12);
                *DSP_TOPSM_SM_TRIG_SETTLE0 = 0x00000008; //SM_TRIG_SETTLE0 > 2+2+2 = RM_PLL_SETTLE+RM_PLL2_SETTLE+(MAX_PWR_SETTLE in RM_PWR_PER)
                *DSP_TOPSM_SM_REQ_MASK = 0xFFFFFFFE; // Mask for software wakeup and timer base wakeup requess to other sleep manager
                *DSP_TOPSM_RM_SM_MASK = 0xFFFFFFFE; // Specofy topsm assert reuest when timer is normal mode
                *DSP_TOPSM_RM_SM_PWR = 0x00000003;
                *DSP_TOPSM_RM_SM_PLL_MASK0 = 0xFFFF0000; //Mask all request
                *DSP_TOPSM_INDIV_CLK_PROTECT_ACK_MASK = 0;
                *DSP_TOPSM_TOPSM_DBG |= 0x1; //disable TDMA timer request
                *DSP_TOPSM_DBG_RM_SM_MASK = 0;
            } else {
                *DSP_TOPSM_RM_PWR_CON0 = 0x42200;
            }
            break;
        default:
            assert(0);
    }

}
#endif /* HAL_SLEEP_MANAGER_ENABLED */
