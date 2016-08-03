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

#include "hal_clock.h"
#include "hal_clock_internal.h"

#ifdef HAL_CLOCK_MODULE_ENABLED

#include <stdio.h>
#include <assert.h>

#include "hal_nvic.h"
#include "hal_nvic_internal.h"
#include "memory_attribute.h"
#include "hal_log.h"
#include "hal_gpt.h"
#include "hal_emi_internal.h"
#include "hal_flash_sf.h"

ATTR_RWDATA_IN_TCM bool clock_initialized = false;
ATTR_RWDATA_IN_TCM bool clock_dfs_initialized = false;
ATTR_RWDATA_IN_TCM uint32_t lfosc_freq = 26000; /* KHz */

/*************************************************************************
 * PLL Structure definition
 *************************************************************************/
struct pll_clock;
struct pll_clock_ops {
    hal_clock_status_t (*enable)(struct pll_clock *target);
    hal_clock_status_t (*disable)(struct pll_clock *target);
    void (*save)(struct pll_clock *target);
    void (*restore)(struct pll_clock *target);
};

struct pll_clock {
    struct pll_clock_ops *ops;
    const char *name;
    const uint32_t sel_mask;
    volatile uint32_t *base_addr;
    int cnt;
    bool saved_pll_req_state;
};

/*************************************************************************
 * Internal used function definition
 *************************************************************************/
/*
ATTR_TEXT_IN_TCM static struct pll_clock *id_to_pll(hal_clock_pll_id pll_id)
{
    return ((uint32_t) pll_id < (uint32_t) NR_PLL_CLOCKS) ? (pll_clock + (uint32_t) pll_id) : NULL;
}
*/

ATTR_TEXT_IN_TCM static hal_clock_status_t general_pll_enable_op(struct pll_clock *target)
{
    uint32_t irq_mask = save_and_set_interrupt_mask();  /* disable interrupt for mux sel */
    if (target->cnt == 0) {
        *(target->base_addr) |= target->sel_mask;
        hal_gpt_delay_us(100);
    }
    if (target->cnt < 32767)
        target->cnt++;
    restore_interrupt_mask(irq_mask);                   /* restore interrupt */
#ifdef CLK_DEBUG
    log_hal_info("%s: %s is enabled, cnt = %d\n", __FUNCTION__, target->name, target->cnt);
#endif
    return HAL_CLOCK_STATUS_OK;
}

ATTR_TEXT_IN_TCM static hal_clock_status_t general_pll_disable_op(struct pll_clock *target)
{
    uint32_t irq_mask = save_and_set_interrupt_mask();  /* disable interrupt for mux sel */
    if (target->cnt <= 1) {
        *(target->base_addr) &= (~target->sel_mask);
    }
    if (target->cnt > 0)
        target->cnt--;
    restore_interrupt_mask(irq_mask);     /* restore interrupt */
#ifdef CLK_DEBUG
    log_hal_info("%s: %s is disabled, cnt = %d\n", __FUNCTION__, target->name, target->cnt);
#endif
    return HAL_CLOCK_STATUS_OK;
}

ATTR_TEXT_IN_TCM static void general_pll_req_save_release_op(struct pll_clock *target)
{
    uint32_t irq_mask = save_and_set_interrupt_mask();  /* disable interrupt for mux sel */
    if (*(target->base_addr) & target->sel_mask) {
        target->saved_pll_req_state = true;
        *(target->base_addr) &= ~(target->sel_mask);
    }
    restore_interrupt_mask(irq_mask);                   /* restore interrupt */
}

ATTR_TEXT_IN_TCM static void general_pll_req_restore_op(struct pll_clock *target)
{
    uint32_t irq_mask = save_and_set_interrupt_mask();  /* disable interrupt for mux sel */
    if (target->saved_pll_req_state) {
        *(target->base_addr) |= target->sel_mask;
        target->saved_pll_req_state = false;            /* clear saved_pll_req_state */
    }
    restore_interrupt_mask(irq_mask);                   /* restore interrupt */
}

ATTR_RWDATA_IN_TCM static struct pll_clock_ops general_pll_clock_ops = {
    /*.enable = */general_pll_enable_op,
    /*.disable = */general_pll_disable_op,
    /*.save = */general_pll_req_save_release_op,
    /*.restore = */general_pll_req_restore_op,
};

/*************************************************************************
 * PLL Clock definition
 * same as bit 0,1,5 DPM_CON0
 *************************************************************************/
ATTR_RWDATA_IN_TCM static struct pll_clock pll_clock[NR_PLL_CLOCKS] = {
    {
        /*.ops = */&(general_pll_clock_ops),
        /*.name = */__stringify(SYSCLK),
        /*.sel_mask = */CLK_CONDC_SYSCLK_REQ_MASK,
    }, {
        /*.ops = */&(general_pll_clock_ops),
        /*.name = */__stringify(UPLLCLK),
        /*.sel_mask = */CLK_CONDC_UPLLCLK_REQ_MASK,
    }, {
        /*.ops = */&(general_pll_clock_ops),
        /*.name = */__stringify(MPLLCLK),
        /*.sel_mask = */CLK_CONDC_MPLLCLK_REQ_MASK,
    },
};

/*************************************************************************
 * PLL register and structure initialize
 *************************************************************************/
ATTR_TEXT_IN_TCM void pll_init(void)
{
    pll_clock[SYSCLK].base_addr = &(CONFIGSYS->CLK_CONDC);
    pll_clock[UPLLCLK].base_addr = &(CONFIGSYS->CLK_CONDC);
    pll_clock[MPLLCLK].base_addr = &(CONFIGSYS->CLK_CONDC);
}

/*************************************************************************
 * PLL Clock API definition
 *************************************************************************/
ATTR_TEXT_IN_TCM hal_clock_status_t hal_clock_enable_upll(void)
{
#ifdef CLK_BRING_UP
    return HAL_CLOCK_STATUS_OK;
#else
    if (!clock_initialized) {
        /* TODO cannot use log_hal_info print log before syslog init */
        log_hal_error("%s: clock is uninitialized\n", __FUNCTION__);
        return HAL_CLOCK_STATUS_UNINITIALIZED;
    }
    return pll_clock[UPLLCLK].ops->enable(&pll_clock[UPLLCLK]);
#endif
}

ATTR_TEXT_IN_TCM hal_clock_status_t hal_clock_disable_upll(void)
{
#ifdef CLK_BRING_UP
    return HAL_CLOCK_STATUS_OK;
#else
    if (!clock_initialized) {
        /* TODO cannot use log_hal_info print log before syslog init */
        log_hal_error("%s: clock is uninitialized\n", __FUNCTION__);
        return HAL_CLOCK_STATUS_UNINITIALIZED;
    }
    return pll_clock[UPLLCLK].ops->disable(&pll_clock[UPLLCLK]);
#endif
}

ATTR_TEXT_IN_TCM hal_clock_status_t hal_clock_enable_mpll(void)
{
#ifdef CLK_BRING_UP
    return HAL_CLOCK_STATUS_OK;
#else
    if (!clock_initialized) {
        /* TODO cannot use log_hal_info print log before syslog init */
        log_hal_error("%s: clock is uninitialized\n", __FUNCTION__);
        return HAL_CLOCK_STATUS_UNINITIALIZED;
    }
    return pll_clock[MPLLCLK].ops->enable(&pll_clock[MPLLCLK]);
#endif
}

ATTR_TEXT_IN_TCM hal_clock_status_t hal_clock_disable_mpll(void)
{
#ifdef CLK_BRING_UP
    return HAL_CLOCK_STATUS_OK;
#else
    if (!clock_initialized) {
        /* TODO cannot use log_hal_info print log before syslog init */
        log_hal_error("%s: clock is uninitialized\n", __FUNCTION__);
        return HAL_CLOCK_STATUS_UNINITIALIZED;
    }
    return pll_clock[MPLLCLK].ops->disable(&pll_clock[MPLLCLK]);
#endif
}

/*************************************************************************
 * Clock source select structure definition part
 *************************************************************************/
struct clock_src_sel;
struct clock_src_sel_ops {
    hal_clock_status_t (*set)(struct clock_src_sel *target, clock_src_id clock_src);
    hal_clock_status_t (*clear)(struct clock_src_sel *target, clock_src_id clock_src);
    int8_t (*get_state)(struct clock_src_sel *target);
};

struct clock_src_sel {
    struct clock_src_sel_ops *ops;
    const char *name;
    const uint32_t sel_mask;
    const uint32_t offset;
    volatile uint32_t *base_addr;
};

/*************************************************************************
 * Clock source select function definition part
 *************************************************************************/
ATTR_TEXT_IN_TCM static hal_clock_status_t general_clock_src_set(struct clock_src_sel *target, clock_src_id clock_src)
{
#ifdef CLK_BRING_UP
    log_hal_info("%s\n", __FUNCTION__);
    return HAL_CLOCK_STATUS_OK;
#else
    if (target != NULL) {
        uint32_t irq_mask = save_and_set_interrupt_mask();  /* disable interrupt for mux sel */
        *(target->base_addr) |= ((clock_src << target->offset) & (target->sel_mask));
        restore_interrupt_mask(irq_mask);                   /* restore interrupt for mux sel */
#ifdef CLK_DEBUG
        log_hal_info("0x%x = %x\n", target->base_addr, *(target->base_addr));
        log_hal_info("%s: %s: clock_src = %d\n", __FUNCTION__, target->name, clock_src);
#endif
        return HAL_CLOCK_STATUS_OK;
    }
    log_hal_error("%s: clock_src = %d, error\n", __FUNCTION__, clock_src);
    return HAL_CLOCK_STATUS_ERROR;
#endif
}

ATTR_TEXT_IN_TCM static hal_clock_status_t general_clock_src_clear(struct clock_src_sel *target, clock_src_id clock_src)
{
#ifdef CLK_BRING_UP
    log_hal_info("%s\n", __FUNCTION__);
    return HAL_CLOCK_STATUS_OK;
#else
    if (target != NULL) {
        uint32_t irq_mask = save_and_set_interrupt_mask();  /* disable interrupt for mux sel */
        *(target->base_addr) &= (~(clock_src << target->offset));
        restore_interrupt_mask(irq_mask);                   /* restore interrupt for mux sel */
#ifdef CLK_DEBUG
        log_hal_info("%s @0x%x = %x\n", target->name, target->base_addr, *(target->base_addr));
#endif
        return HAL_CLOCK_STATUS_OK;
    }
    log_hal_error("%s: clock_src = %d, error\n", __FUNCTION__, clock_src);
    return HAL_CLOCK_STATUS_ERROR;
#endif
}

ATTR_TEXT_IN_TCM static int8_t general_clock_src_get_state(struct clock_src_sel *target)
{
#ifdef CLK_BRING_UP
    log_hal_info("%s\n", __FUNCTION__);
    return -1;
#else
    if (target != NULL) {
        uint32_t state = ((*(target->base_addr) & target->sel_mask) >> target->offset);
#ifdef CLK_DEBUG
        log_hal_info("%s: %s's state = %x\n", __FUNCTION__, target->name, state);
#endif
        return state;
    }
    return 0;
#endif
}

ATTR_RWDATA_IN_TCM static struct clock_src_sel_ops clock_src_sel_ops = {
    /*.set = */general_clock_src_set,
    /*.clear = */general_clock_src_clear,
    /*.get_state = */general_clock_src_get_state,
};

ATTR_RWDATA_IN_TCM static struct clock_src_sel clock_src_sels[NR_CLKSRC_SEL] = {
    {
        /*.ops = */&clock_src_sel_ops,
        /*.name = */__stringify(CM4CLK_SOURCE_SEL),
        /*.sel_mask = */CLK_SOURCE_SEL_CM4CLK_SOURCE_SEL_MASK,
        /*.offset = */CLK_SOURCE_SEL_CM4CLK_SOURCE_SEL_OFFSET,
    }, {
        /*.ops = */&clock_src_sel_ops,
        /*.name = */__stringify(AO_BUSCLK_SOURCE_SEL),
        /*.sel_mask = */CLK_SOURCE_SEL_AO_BUSCLK_SOURCE_SEL_MASK,
        /*.offset = */CLK_SOURCE_SEL_AO_BUSCLK_SOURCE_SEL_OFFSET,
    }, {
        /*.ops = */&clock_src_sel_ops,
        /*.name = */ __stringify(PDN_BUSCLK_SOURCE_SEL),
        /*.sel_mask = */CLK_SOURCE_SEL_PDN_BUSCLK_SOURCE_SEL_MASK,
        /*.offset = */CLK_SOURCE_SEL_PDN_BUSCLK_SOURCE_SEL_OFFSET,
    }, {
        /*.ops = */&clock_src_sel_ops,
        /*.name = */__stringify(SFCCLK_SOURCE_SEL),
        /*.sel_mask = */CLK_SOURCE_SEL_SFCCLK_SOURCE_SEL_MASK,
        /*.offset = */CLK_SOURCE_SEL_SFCCLK_SOURCE_SEL_OFFSET,
    }, {
        /*.ops = */&clock_src_sel_ops,
        /*.name = */__stringify(BSICLK_SOURCE_SEL),
        /*.sel_mask = */CLK_SOURCE_SEL_BSICLK_SOURCE_SEL_MASK,
        /*.offset = */CLK_SOURCE_SEL_BSICLK_SOURCE_SEL_OFFSET,
    }, {
        /*.ops = */&clock_src_sel_ops,
        /*.name = */__stringify(DSPCLK_SOURCE_SEL),
        /*.sel_mask = */CLK_SOURCE_SEL_DSPCLK_SOURCE_SEL_MASK,
        /*.offset = */CLK_SOURCE_SEL_DSPCLK_SOURCE_SEL_OFFSET,
    },
};

ATTR_TEXT_IN_TCM void clock_src_sel_init(void)
{
    int i;
    for (i = 0; i < NR_CLKSRC_SEL; i++) {
        clock_src_sels[i].base_addr = &(CONFIGSYS->CLK_SOURCE_SEL);
    }
}

/*************************************************************************
 * Clock sourse select API definition part
 *************************************************************************/
ATTR_TEXT_IN_TCM hal_clock_status_t clock_source_set(clock_src_sel_id clock_source_sel_id, clock_src_id source_id)
{
#ifdef CLK_DEBUG
    log_hal_info("%s: clock_source_sel_id = %d\n", __FUNCTION__, clock_source_sel_id);
#endif
    struct clock_src_sel *target = &clock_src_sels[clock_source_sel_id];
    if (target != NULL)
        return target->ops->set(target, source_id);
    else
        return HAL_CLOCK_STATUS_INVALID_PARAMETER;
}

ATTR_TEXT_IN_TCM hal_clock_status_t clock_source_clear(clock_src_sel_id clock_source_sel_id, clock_src_id source_id)
{
#ifdef CLK_DEBUG
    log_hal_info("%s: clock_source_sel_id = %d\n", __FUNCTION__, clock_source_sel_id);
#endif
    struct clock_src_sel *target = &clock_src_sels[clock_source_sel_id];
    if (target != NULL)
        return target->ops->clear(target, source_id);
    else
        return HAL_CLOCK_STATUS_INVALID_PARAMETER;
}

/*************************************************************************
 * Clock divider structure definition part
 *************************************************************************/
struct clock_div_grp {
    struct pll_clock *pll;
    const char *name;
    const clock_src_id src_id;
    volatile uint32_t *base_addr;
    /* const int nr_div; */
};

ATTR_RWDATA_IN_TCM static struct clock_div_grp clock_div_grp[NR_DIV_GRP];

struct clock_div;
struct clock_div_ops {
    hal_clock_status_t (*enable)(struct clock_div *target);
    hal_clock_status_t (*disable)(struct clock_div *target);
};

struct clock_div {
    struct clock_div *parent;
    struct clock_div *sibling;
    struct clock_div_grp *grp;
    struct clock_div_ops *ops;
    const char *name;
    const uint32_t sel_mask;
    int cnt;
};

/*************************************************************************
 * Clock divider enable function definition part
 *************************************************************************/
ATTR_RWDATA_IN_TCM static struct clock_div_grp clock_div_grp[NR_DIV_GRP] = {
    {
        /*.pll = */NULL,
        /*.name =  */__stringify(HFOSC_DIV_GRP),
        /*.src_id = */CLK_SRC_HFOSC,
        /* .nr_div = NR_HFOSC_DIV, */
    }, {
        /*.pll = */&pll_clock[UPLLCLK],
        /*.name =  */__stringify(UPLL_DIV_GRP),
        /*.src_id = */CLK_SRC_PLL,
        /* .nr_div = NR_UPLL_DIV, */
    }, {
        /*.pll = */&pll_clock[MPLLCLK],
        /*.name =  */__stringify(MPLL_DIV_GRP),
        /*.src_id = */CLK_SRC_PLL,
        /* .nr_div = NR_MPLL_DIV, */
    },
};

/*
ATTR_TEXT_IN_TCM static struct clock_div *id_to_div(clock_div_id div_id)
{
    return ((uint8_t) div_id < (uint8_t) NR_DIV) ? (clock_div + (uint8_t) div_id) : NULL;
}
*/

ATTR_TEXT_IN_TCM static hal_clock_status_t general_clock_div_enable(struct clock_div *target)
{
#ifdef CLK_BRING_UP
    log_hal_info("%s\n", __FUNCTION__);
    return HAL_CLOCK_STATUS_OK;
#else
    uint32_t irq_mask = save_and_set_interrupt_mask();  /* disable interrupt for mux sel */
    if (target->parent != NULL)                         /* enable parent divider */
        target->parent->ops->enable(target->parent);
    if (target->cnt == 0)
        *(target->grp->base_addr) |= target->sel_mask;
    if (target->cnt < 32767)
        target->cnt++;
    restore_interrupt_mask(irq_mask);                   /* restore interrupt for mux sel */

#ifdef CLK_DEBUG
    log_hal_info("%s @0x%x = %x\n", target->name, target->grp->base_addr, *(target->grp->base_addr));
#endif
    return HAL_CLOCK_STATUS_OK;
#endif
}

ATTR_TEXT_IN_TCM static hal_clock_status_t general_clock_div_disable(struct clock_div *target)
{
#ifdef CLK_BRING_UP
    log_hal_info("%s\n", __FUNCTION__);
    return HAL_CLOCK_STATUS_OK;
#else
    uint32_t irq_mask = save_and_set_interrupt_mask();  /* disable interrupt for mux sel */
    if (target->cnt <= 1) {
        if (target->sibling != NULL) {                  /* check sibling exist */
            if (target->sibling->cnt == 0)              /* sibling is not using, do disable divider */
                *(target->grp->base_addr) &= (~target->sel_mask);
        } else {                                        /* sibling not exist, disable divider directly */
            *(target->grp->base_addr) &= (~target->sel_mask);
        }
    }
    if (target->cnt > 0)
        target->cnt--;
    if (target->parent != NULL)                         /* disable parent divider */
        target->parent->ops->disable(target->parent);
    restore_interrupt_mask(irq_mask);                   /* restore interrupt for mux sel */

#ifdef CLK_DEBUG
    log_hal_info("%s @0x%x = %x\n", target->name, target->grp->base_addr, *(target->grp->base_addr));
#endif
    return HAL_CLOCK_STATUS_OK;
#endif
}

ATTR_RWDATA_IN_TCM static struct clock_div_ops clock_div_ops = {
    /*.enable = */general_clock_div_enable,
    /*.disable = */general_clock_div_disable,
};

ATTR_RWDATA_IN_TCM static struct clock_div clock_div[NR_DIV] = {
    {
        /*.parent = */NULL,
        /*.sibling = */NULL,
        /*.grp = */&clock_div_grp[HFOSC_DIV_GRP],
        /*.ops = */&(clock_div_ops),
        /*.name = */__stringify(HFOSC_DIV1P5_CK),
        /*.sel_mask = */CLK_CONDA_HFOSC_DIV1P5_CK_MASK,
    }, {
        /*.parent = */NULL,
        /*.sibling = */&clock_div[HFOSC_DIV4_CK],   /* sibling with HFOSC_DIV4_CK */
        /*.grp = */&clock_div_grp[HFOSC_DIV_GRP],
        /*.ops = */&(clock_div_ops),
        /*.name = */__stringify(HFOSC_DIV2_CK),
        /*.sel_mask = */CLK_CONDA_HFOSC_DIV2_CK_MASK,
    }, {
        /*.parent = */NULL,
        /*.sibling = */&clock_div[HFOSC_DIV2_CK],   /* sibling with HFOSC_DIV2_CK */
        /*.grp = */&clock_div_grp[HFOSC_DIV_GRP],
        /*.ops = */&(clock_div_ops),
        /*.name = */__stringify(HFOSC_DIV4_CK),
        /*.sel_mask = */CLK_CONDA_HFOSC_DIV4_CK_MASK,
    }, {
        /*.parent = */NULL,
        /*.sibling = */NULL,
        /*.grp = */&clock_div_grp[HFOSC_DIV_GRP],
        /*.ops = */&(clock_div_ops),
        /*.name = */__stringify(HFOSC_DIV2P5_CK),
        /*.sel_mask = */CLK_CONDA_HFOSC_DIV2P5_CK_MASK,
    }, {
        /*.parent = */&clock_div[HFOSC_DIV1P5_CK],  /* parent divider */
        /*.sibling = */&clock_div[HFOSC_DIV6_CK],   /* sibling with HFOSC_DIV6_CK */
        /*.grp = */&clock_div_grp[HFOSC_DIV_GRP],
        /*.ops = */&(clock_div_ops),
        /*.name = */__stringify(HFOSC_DIV3_CK),
        /*.sel_mask = */CLK_CONDA_HFOSC_DIV3_CK_MASK,
    }, {
        /*.parent = */&clock_div[HFOSC_DIV1P5_CK],  /* parent divider */
        /*.sibling = */&clock_div[HFOSC_DIV3_CK],   /* sibling with HFOSC_DIV3_CK */
        /*.grp = */&clock_div_grp[HFOSC_DIV_GRP],
        /*.ops = */&(clock_div_ops),
        /*.name = */__stringify(HFOSC_DIV6_CK),
        /*.sel_mask = */CLK_CONDA_HFOSC_DIV6_CK_MASK,
    }, {
        /*.parent = */NULL,
        /*.sibling = */NULL,
        /*.grp = */&clock_div_grp[HFOSC_DIV_GRP],
        /*.ops = */&(clock_div_ops),
        /*.name = */__stringify(HFOSC_DIV3P5_CK),
        /*.sel_mask = */CLK_CONDA_HFOSC_DIV3P5_CK_MASK,
    }, {
        /*.parent = */NULL,
        /*.sibling = */NULL,
        /*.grp = */&clock_div_grp[HFOSC_DIV_GRP],
        /*.ops = */&(clock_div_ops),
        /*.name = */__stringify(HFOSC_DIV5_CK),
        /*.sel_mask = */CLK_CONDA_HFOSC_DIV5_CK_MASK,
    }, {
        /*.parent = */NULL,
        /*.sibling = */NULL,
        /*.grp = */&clock_div_grp[UPLL_DIV_GRP],
        /*.ops = */&(clock_div_ops),
        /*.name = */__stringify(UPLL_F312M_CK),
        /*.sel_mask = */CLK_CONDA_UPLL_F312M_CK_MASK,
    }, {
        /*.parent = */NULL,
        /*.sibling = */NULL,
        /*.grp = */&clock_div_grp[UPLL_DIV_GRP],
        /*.ops = */&(clock_div_ops),
        /*.name = */__stringify(UPLL_F104M_CK),
        /*.sel_mask = */CLK_CONDA_UPLL_F104M_CK_MASK,
    }, {
        /*.parent = */NULL,
        /*.sibling = */NULL,
        /*.grp = */&clock_div_grp[UPLL_DIV_GRP],
        /*.ops = */&(clock_div_ops),
        /*.name = */__stringify(UPLL_F124M_CK),
        /*.sel_mask = */CLK_CONDA_UPLL_F124M_CK_MASK,
    }, {
        /*.parent = */NULL,
        /*.sibling = */NULL,
        /*.grp = */&clock_div_grp[UPLL_DIV_GRP],
        /*.ops = */&(clock_div_ops),
        /*.name = */__stringify(UPLL_F62M_CK),
        /*.sel_mask = */CLK_CONDA_UPLL_F62M_CK_MASK,
    }, {
        /*.parent = */NULL,
        /*.sibling = */NULL,
        /*.grp = */&clock_div_grp[UPLL_DIV_GRP],
        /*.ops = */&(clock_div_ops),
        /*.name = */__stringify(UPLL_48M_CK),
        /*.sel_mask = */CLK_CONDA_UPLL_48M_CK_MASK,
    }, {
        /*.parent = */NULL,
        /*.sibling = */NULL,
        /*.grp = */&clock_div_grp[MPLL_DIV_GRP],
        /*.ops = */&(clock_div_ops),
        /*.name = */__stringify(MPLL_F312M_CK),
        /*.sel_mask = */CLK_CONDA_MPLL_F312M_CK_MASK,
    }, {
        /*.parent = */NULL,
        /*.sibling = */NULL,
        /*.grp = */&clock_div_grp[MPLL_DIV_GRP],
        /*.ops = */&(clock_div_ops),
        /*.name = */__stringify(MPLL_F250M_CK),
        /*.sel_mask = */CLK_CONDA_MPLL_F250M_CK_MASK,
    }, {
        /*.parent = */NULL,
        /*.sibling = */NULL,
        /*.grp = */&clock_div_grp[MPLL_DIV_GRP],
        /*.ops = */&(clock_div_ops),
        /*.name = */__stringify(MPLL_F208M_CK),
        /*.sel_mask = */CLK_CONDA_MPLL_F208M_CK_MASK,
    }, {
        /*.parent = */&clock_div[MPLL_F250M_CK],    /* parent divider */
        /*.sibling = */NULL,
        /*.grp = */&clock_div_grp[MPLL_DIV_GRP],
        /*.ops = */&(clock_div_ops),
        /*.name = */__stringify(MPLL_F125M_CK),
        /*.sel_mask = */CLK_CONDA_MPLL_F125M_CK_MASK,
    }, {
        /*.parent = */&clock_div[MPLL_F312M_CK],    /* parent divider */
        /*.sibling = */NULL,
        /*.grp = */&clock_div_grp[MPLL_DIV_GRP],
        /*.ops = */&(clock_div_ops),
        /*.name = */__stringify(MPLL_DIV1P5_CK),
        /*.sel_mask = */CLK_CONDA_MPLL_DIV1P5_CK_MASK,
    }, {
        /*.parent = */&clock_div[MPLL_F312M_CK],    /* parent divider */
        /*.sibling = */&clock_div[MPLL_DIV4_CK],    /* sibling with MPLL_DIV4_CK */
        /*.grp = */&clock_div_grp[MPLL_DIV_GRP],
        /*.ops = */&(clock_div_ops),
        /*.name = */__stringify(MPLL_DIV2_CK),
        /*.sel_mask = */CLK_CONDA_MPLL_DIV2_CK_MASK,
    }, {
        /*.parent = */&clock_div[MPLL_F312M_CK],    /* parent divider */
        /*.sibling = */&clock_div[MPLL_DIV2_CK],    /* sibling with MPLL_DIV2_CK */
        /*.grp = */&clock_div_grp[MPLL_DIV_GRP],
        /*.ops = */&(clock_div_ops),
        /*.name = */__stringify(MPLL_DIV4_CK),
        /*.sel_mask = */CLK_CONDA_MPLL_DIV4_CK_MASK,
    }, {
        /*.parent = */&clock_div[MPLL_F312M_CK],    /* parent divider */
        /*.sibling = */NULL,
        /*.grp = */&clock_div_grp[MPLL_DIV_GRP],
        /*.ops = */&(clock_div_ops),
        /*.name = */__stringify(MPLL_DIV2P5_CK),
        /*.sel_mask = */CLK_CONDA_MPLL_DIV2P5_CK_MASK,
    }, {
        /*.parent = */&clock_div[MPLL_DIV1P5_CK],   /* parent divider */
        /*.sibling = */&clock_div[MPLL_DIV6_CK],    /* sibling with MPLL_DIV6_CK */
        /*.grp = */&clock_div_grp[MPLL_DIV_GRP],
        /*.ops = */&(clock_div_ops),
        /*.name = */__stringify(MPLL_DIV3_CK),
        /*.sel_mask = */CLK_CONDA_MPLL_DIV3_CK_MASK,
    }, {
        /*.parent = */&clock_div[MPLL_DIV1P5_CK],   /* parent divider */
        /*.sibling = */&clock_div[MPLL_DIV3_CK],    /* sibling with MPLL_DIV3_CK */
        /*.grp = */&clock_div_grp[MPLL_DIV_GRP],
        /*.ops = */&(clock_div_ops),
        /*.name = */__stringify(MPLL_DIV6_CK),
        /*.sel_mask = */CLK_CONDA_MPLL_DIV6_CK_MASK,
    }, {
        /*.parent = */&clock_div[MPLL_F312M_CK],    /* parent divider */
        /*.sibling = */NULL,
        /*.grp = */&clock_div_grp[MPLL_DIV_GRP],
        /*.ops = */&(clock_div_ops),
        /*.name = */__stringify(MPLL_DIV3P5_CK),
        /*.sel_mask = */CLK_CONDA_MPLL_DIV3P5_CK_MASK,
    }, {
        /*.parent = */&clock_div[MPLL_F312M_CK],    /* parent divider */
        /*.sibling = */NULL,
        /*.grp = */&clock_div_grp[MPLL_DIV_GRP],
        /*.ops = */&(clock_div_ops),
        /*.name = */__stringify(MPLL_DIV5_CK),
        /*.sel_mask = */CLK_CONDA_MPLL_DIV5_CK_MASK,
    }, {
        /*.parent = */&clock_div[MPLL_F312M_CK],    /* parent divider */
        /*.sibling = */NULL,
        /*.grp = */&clock_div_grp[MPLL_DIV_GRP],
        /*.ops = */&(clock_div_ops),
        /*.name = */__stringify(MPLL_F138M_CK),
        /*.sel_mask = */CLK_CONDA_MPLL_F138M_CK_MASK,
    }, {
        /*.parent = */NULL,
        /*.sibling = */NULL,
        /*.grp = */&clock_div_grp[MPLL_DIV_GRP],
        /*.ops = */&(clock_div_ops),
        /*.name = */__stringify(MPLL_F178M_CK),
        /*.sel_mask = */CLK_CONDA_MPLL_F178M_CK_MASK,
    },
};

/*************************************************************************
 * Clock divier enable API definition part
 *************************************************************************/
ATTR_TEXT_IN_TCM hal_clock_status_t clock_div_enable(clock_div_id div_id)
{
#ifdef CLK_DEBUG
    log_hal_info("%s: div_id = %d\n", __FUNCTION__, div_id);
#endif
    struct clock_div *target;

    if (div_id < NR_DIV) {
        target = &clock_div[div_id];
        return target->ops->enable(target);
    } else
        return HAL_CLOCK_STATUS_INVALID_PARAMETER;
}

ATTR_TEXT_IN_TCM hal_clock_status_t clock_div_disable(clock_div_id div_id)
{
#ifdef CLK_DEBUG
    log_hal_info("%s: div_id = %d\n", __FUNCTION__, div_id);
#endif
    struct clock_div *target;

    if (div_id < NR_DIV) {
        target  = &clock_div[div_id];
        return target->ops->disable(target);
    } else
        return HAL_CLOCK_STATUS_INVALID_PARAMETER;
}

/*************************************************************************
 * Clock force on structure definition part
 *************************************************************************/
struct clock_force_on;
struct clock_force_on_ops {
    hal_clock_status_t (*set)(struct clock_force_on *target);
    hal_clock_status_t (*clear)(struct clock_force_on *target);
};

struct clock_force_on {
    struct clock_force_on_ops *ops;
    const char *name;
    const uint32_t sel_mask;
    const uint32_t offset;
    volatile uint32_t *base_addr;
};

/*************************************************************************
 * Clock force on function definition part
 *************************************************************************/
ATTR_TEXT_IN_TCM static hal_clock_status_t general_clock_force_on_set(struct clock_force_on *target)
{
#ifdef CLK_BRING_UP
    log_hal_info("%s\n", __FUNCTION__);
    return HAL_CLOCK_STATUS_OK;
#else
    if (target != NULL) {
        uint32_t irq_mask = save_and_set_interrupt_mask();  /* disable interrupt for mux sel */
        *(target->base_addr) |= target->sel_mask;
        restore_interrupt_mask(irq_mask);     /* restore interrupt for mux sel */
#ifdef CLK_DEBUG
        log_hal_info("%s @0x%x = %x\n", target->name, target->base_addr, *(target->base_addr));
#endif
        return HAL_CLOCK_STATUS_OK;
    }
    log_hal_error("%s: error\n", __FUNCTION__);
    return HAL_CLOCK_STATUS_ERROR;
#endif
}

ATTR_TEXT_IN_TCM static hal_clock_status_t general_clock_force_on_clear(struct clock_force_on *target)
{
#ifdef CLK_BRING_UP
    log_hal_info("%s\n", __FUNCTION__);
    return HAL_CLOCK_STATUS_OK;
#else
    if (target != NULL) {
        uint32_t irq_mask = save_and_set_interrupt_mask();  /* disable interrupt for mux sel */
        *(target->base_addr) &= (~target->sel_mask);
        restore_interrupt_mask(irq_mask);     /* restore interrupt for mux sel */
#ifdef CLK_DEBUG
        log_hal_info("%s @0x%x = %x\n", target->name, target->base_addr, *(target->base_addr));
#endif
        return HAL_CLOCK_STATUS_OK;
    }
    log_hal_error("%s: error\n", __FUNCTION__);
    return HAL_CLOCK_STATUS_ERROR;
#endif
}

ATTR_RWDATA_IN_TCM static struct clock_force_on_ops clock_force_on_ops = {
    /*.set = */general_clock_force_on_set,
    /*.clear = */general_clock_force_on_clear,
};

ATTR_RWDATA_IN_TCM static struct clock_force_on clock_force_on[NR_CLKFORCE_ON] = {
    {
        /*.ops = */&clock_force_on_ops,
        /*.name = */__stringify(BUSCSW_FORCE_ON),
        /*.sel_mask = */CLK_CONDD_BUSCSW_FORCE_ON_MASK,
        /*.offset = */CLK_CONDD_BUSCSW_FORCE_ON_OFFSET,
    }, {
        /*.ops = */&clock_force_on_ops,
        /*.name = */__stringify(CMCSW_FORCE_ON),
        /*.sel_mask = */CLK_CONDD_CMCSW_FORCE_ON_MASK,
        /*.offset = */CLK_CONDD_CMCSW_FORCE_ON_OFFSET,
    }, {
        /*.ops = */&clock_force_on_ops,
        /*.name = */__stringify(SFCCSW_FORCE_ON),
        /*.sel_mask = */CLK_CONDD_SFCCSW_FORCE_ON_MASK,
        /*.offset = */CLK_CONDD_SFCCSW_FORCE_ON_OFFSET,
    }, {
        /*.ops = */&clock_force_on_ops,
        /*.name = */__stringify(SLCDCSW_FORCE_ON),
        /*.sel_mask = */CLK_CONDD_SLCDCSW_FORCE_ON_MASK,
        /*.offset = */CLK_CONDD_SLCDCSW_FORCE_ON_OFFSET,
    }, {
        /*.ops = */&clock_force_on_ops,
        /*.name = */__stringify(BSICSW_FORCE_ON),
        /*.sel_mask = */CLK_CONDD_BSICSW_FORCE_ON_MASK,
        /*.offset = */CLK_CONDD_BSICSW_FORCE_ON_OFFSET,
    }, {
        /*.ops = */&clock_force_on_ops,
        /*.name = */__stringify(DSPCSW_FORCE_ON),
        /*.sel_mask = */CLK_CONDD_DSPCSW_FORCE_ON_MASK,
        /*.offset = */CLK_CONDD_DSPCSW_FORCE_ON_OFFSET,
    }, {
        /*.ops = */&clock_force_on_ops,
        /*.name = */__stringify(CAMCSW_FORCE_ON),
        /*.sel_mask = */CLK_CONDD_CAMCSW_FORCE_ON_MASK,
        /*.offset = */CLK_CONDD_CAMCSW_FORCE_ON_OFFSET,
    }, {
        /*.ops = */&clock_force_on_ops,
        /*.name = */__stringify(USBCSW_FORCE_ON),
        /*.sel_mask = */CLK_CONDD_USBCSW_FORCE_ON_MASK,
        /*.offset = */CLK_CONDD_USBCSW_FORCE_ON_OFFSET,
    }, {
        /*.ops = */&clock_force_on_ops,
        /*.name = */__stringify(DISPPWMCSW_FORCE_ON),
        /*.sel_mask = */CLK_CONDD_DISPPWMCSW_FORCE_ON_MASK,
        /*.offset = */CLK_CONDD_DISPPWMCSW_FORCE_ON_OFFSET,
    }, {
        /*.ops = */&clock_force_on_ops,
        /*.name = */__stringify(MSDC0CSW_FORCE_ON),
        /*.sel_mask = */CLK_CONDD_MSDC0CSW_FORCE_ON_MASK,
        /*.offset = */CLK_CONDD_MSDC0CSW_FORCE_ON_OFFSET,
    }, {
        /*.ops = */&clock_force_on_ops,
        /*.name = */__stringify(MSDC1CSW_FORCE_ON),
        /*.sel_mask = */CLK_CONDD_MSDC1CSW_FORCE_ON_MASK,
        /*.offset = */CLK_CONDD_MSDC1CSW_FORCE_ON_OFFSET,
    },
};

/*************************************************************************
* Clock change bit structure definition part
*************************************************************************/
struct clock_change_bit;
struct clock_change_bit_ops {
    hal_clock_status_t (*set)(struct clock_change_bit *target);
    hal_clock_status_t (*verified)(struct clock_change_bit *target);
};

struct clock_change_bit {
    struct clock_change_bit_ops *ops;
    const char *name;
    const uint32_t sel_mask;
    const uint32_t offset;
    volatile uint32_t *base_addr;
};

/*************************************************************************
* Clock change bit function definition part
*************************************************************************/
ATTR_TEXT_IN_TCM static hal_clock_status_t general_clock_change_bit_set(struct clock_change_bit *target)
{
#ifdef CLK_BRING_UP
    log_hal_info("%s\n", __FUNCTION__);
    return HAL_CLOCK_STATUS_OK;
#else
    uint32_t irq_mask = save_and_set_interrupt_mask();  /* disable interrupt */
    *(target->base_addr) |= target->sel_mask;
    restore_interrupt_mask(irq_mask);     /* restore interrupt */
#ifdef CLK_DEBUG
    log_hal_info("%s: Change bit %s, @0x%x = %x\n", __FUNCTION__, target->name, target->base_addr, *(target->base_addr));
#endif
    return HAL_CLOCK_STATUS_OK;
#endif
}

ATTR_TEXT_IN_TCM static hal_clock_status_t general_clock_change_bit_verified(struct clock_change_bit *target)
{
#ifdef CLK_BRING_UP
    log_hal_info("%s\n", __FUNCTION__);
    return HAL_CLOCK_STATUS_OK;
#else
    uint8_t i = 0;

    while (*(target->base_addr) & target->sel_mask) {
        hal_gpt_delay_us(1);
        if (++i > 5) {
            log_hal_error("%s: Change bit %s, @0x%x = %x cannot be cleared in 5 us\n", __FUNCTION__, target->name, target->base_addr, *(target->base_addr));
            assert(0);
            return HAL_CLOCK_STATUS_ERROR;
        }
    }

#ifdef CLK_DEBUG
    log_hal_info("%s: Change bit %s, @0x%x = %x cleared\n", __FUNCTION__, target->name, target->base_addr, *(target->base_addr));
#endif
    return HAL_CLOCK_STATUS_OK;
#endif
}

ATTR_RWDATA_IN_TCM static struct clock_change_bit_ops clock_change_bit_ops = {
    /*.set = */general_clock_change_bit_set,
    /*.verified = */general_clock_change_bit_verified,
};

ATTR_RWDATA_IN_TCM static struct clock_change_bit clock_change_bit[NR_CLKCHANGE_BIT] = {
    {
        /*.ops = */&clock_change_bit_ops,
        /*.name = */__stringify(CHG_BUS),
        /*.sel_mask = */ACFG_CLK_UPDATE_CHG_BUS_MASK,
        /*.offset = */ACFG_CLK_UPDATE_CHG_BUS_OFFSET,
    }, {
        /*.ops = */&clock_change_bit_ops,
        /*.name = */__stringify(CHG_CM),
        /*.sel_mask = */ACFG_CLK_UPDATE_CHG_CM_MASK,
        /*.offset = */ACFG_CLK_UPDATE_CHG_CM_OFFSET,
    }, {
        /*.ops = */&clock_change_bit_ops,
        /*.name = */__stringify(CHG_SFC),
        /*.sel_mask = */ACFG_CLK_UPDATE_CHG_SFC_MASK,
        /*.offset = */ACFG_CLK_UPDATE_CHG_SFC_OFFSET,
    }, {
        /*.ops = */&clock_change_bit_ops,
        /*.name = */__stringify(CHG_SLCD),
        /*.sel_mask = */ACFG_CLK_UPDATE_CHG_SLCD_MASK,
        /*.offset = */ACFG_CLK_UPDATE_CHG_SLCD_OFFSET,
    }, {
        /*.ops = */&clock_change_bit_ops,
        /*.name = */__stringify(CHG_BSI),
        /*.sel_mask = */ACFG_CLK_UPDATE_CHG_BSI_MASK,
        /*.offset = */ACFG_CLK_UPDATE_CHG_BSI_OFFSET,
    }, {
        /*.ops = */&clock_change_bit_ops,
        /*.name = */__stringify(CHG_DSP),
        /*.sel_mask = */ACFG_CLK_UPDATE_CHG_DSP_MASK,
        /*.offset = */ACFG_CLK_UPDATE_CHG_DSP_OFFSET,
    }, {
        /*.ops = */&clock_change_bit_ops,
        /*.name = */__stringify(CHG_CAM),
        /*.sel_mask = */ACFG_CLK_UPDATE_CHG_CAM_MASK,
        /*.offset = */ACFG_CLK_UPDATE_CHG_CAM_OFFSET,
    }, {
        /*.ops = */&clock_change_bit_ops,
        /*.name = */__stringify(CHG_USB),
        /*.sel_mask = */ACFG_CLK_UPDATE_CHG_USB_MASK,
        /*.offset = */ACFG_CLK_UPDATE_CHG_USB_OFFSET,
    }, {
        /*.ops = */&clock_change_bit_ops,
        /*.name = */__stringify(CHG_DISP_PWM),
        /*.sel_mask = */ACFG_CLK_UPDATE_CHG_DISP_PWM_MASK,
        /*.offset = */ACFG_CLK_UPDATE_CHG_DISP_PWM_OFFSET,
    }, {
        /*.ops = */&clock_change_bit_ops,
        /*.name = */__stringify(CHG_MSDC0),
        /*.sel_mask = */ACFG_CLK_UPDATE_CHG_MSDC0_MASK,
        /*.offset = */ACFG_CLK_UPDATE_CHG_MSDC0_OFFSET,
    }, {
        /*.ops = */&clock_change_bit_ops,
        /*.name = */__stringify(CHG_MSDC1),
        /*.sel_mask = */ACFG_CLK_UPDATE_CHG_MSDC1_MASK,
        /*.offset = */ACFG_CLK_UPDATE_CHG_MSDC1_OFFSET,
    }, {
        /*.ops = */&clock_change_bit_ops,
        /*.name = */__stringify(CHG_LP_CLKSQ),
        /*.sel_mask = */ACFG_CLK_UPDATE_CHG_LP_CLKSQ_MASK,
        /*.offset = */ACFG_CLK_UPDATE_CHG_LP_CLKSQ_OFFSET,
    }, {
        /*.ops = */&clock_change_bit_ops,
        /*.name = */__stringify(CHG_LP_LFOSC),
        /*.sel_mask = */ACFG_CLK_UPDATE_CHG_LP_LFOSC_MASK,
        /*.offset = */ACFG_CLK_UPDATE_CHG_LP_LFOSC_OFFSET,
    },
};

ATTR_TEXT_IN_TCM hal_clock_status_t clock_change_bit_set(clock_change_bit_id change_bit_id)
{
    if (!clock_initialized) {
        /* TODO cannot use log_hal_info print log before syslog init */
        log_hal_error("%s: clock is uninitialized\n", __FUNCTION__);
        return HAL_CLOCK_STATUS_UNINITIALIZED;
    }

    if (change_bit_id < NR_CLKCHANGE_BIT) {
#ifdef CLK_DEBUG
        log_hal_info("%s\n", __FUNCTION__);
#endif
        clock_change_bit[change_bit_id].ops->set(&clock_change_bit[change_bit_id]);
        return clock_change_bit[change_bit_id].ops->verified(&clock_change_bit[change_bit_id]);
    } else {
        /* TODO return fail id information */
        log_hal_error("%s, error\n", __FUNCTION__);
        return HAL_CLOCK_STATUS_INVALID_PARAMETER;
    }
}

ATTR_TEXT_IN_TCM hal_clock_status_t clock_change_bit_verified_all(void)
{
    uint8_t i = 0;

    if (!clock_initialized) {
        /* TODO cannot use log_hal_info print log before syslog init */
        log_hal_error("%s: clock is uninitialized\n", __FUNCTION__);
        return HAL_CLOCK_STATUS_UNINITIALIZED;
    }
#ifdef CLK_DEBUG
    log_hal_info("%s\n", __FUNCTION__);
#endif

    while (ANA_CFGSYS->ACFG_CLK_UPDATE) {
        hal_gpt_delay_us(1);
        if (++i > 5) {
            log_hal_error("%s: Change bit register @0x%x = %x cannot be cleared in 5 us\n", __FUNCTION__, &ANA_CFGSYS->ACFG_CLK_UPDATE, ANA_CFGSYS->ACFG_CLK_UPDATE);
            assert(0);
            return HAL_CLOCK_STATUS_ERROR;
        }
    }

#ifdef CLK_DEBUG
    log_hal_info("%s: Change bit register, @0x%x = %x cleared\n", __FUNCTION__, &ANA_CFGSYS->ACFG_CLK_UPDATE, ANA_CFGSYS->ACFG_CLK_UPDATE);
#endif
    return HAL_CLOCK_STATUS_OK;
}

/*************************************************************************
 * Clock mux select structure definition part
*************************************************************************/
struct clock_mux;
struct clock_mux_ops {
    hal_clock_status_t (*sel)(struct clock_mux *target, uint32_t mux_sel, bool init_used);
    int8_t (*get_state)(struct clock_mux *target);
    hal_clock_status_t (*cg_request_pll)(struct clock_mux *target, bool request);
};

#define NR_DIV_MAPPING 9                /* Maximum mapping for divider */
#define NR_CLKSRC_SEL_MAPPING 2         /* Maximum mapping for clksrc_sel */

struct clock_mux {
    struct clock_mux_ops *ops;
    struct clock_force_on *force_on;
    struct clock_change_bit *change_bit;
    const char *name;
    const uint32_t sel_mask;
    const uint32_t offset;
    const uint8_t nr_inputs;
    const uint8_t div_bitmap[NR_DIV_MAPPING];               /* maximum nr_inputs is 9, for all mux to use */
    const uint8_t clksrc_sel_map[NR_CLKSRC_SEL_MAPPING];    /* maximux number of clock_src_sel for one mux is 2 */
    volatile uint32_t *base_addr;
};

/*************************************************************************
 * Clock mux select function definition part
 *************************************************************************/
ATTR_TEXT_IN_TCM static hal_clock_status_t general_clock_mux_sel(struct clock_mux *target, uint32_t mux_sel, bool init_used)
{
    struct clock_div *divider = NULL;
    struct clock_src_sel *clksrc_sel = NULL;
    uint8_t mux_sel_pre, i;
    bool pll_temporarily_enable = false;

#ifdef CLK_BRING_UP
    if (target->div_bitmap[mux_sel] < NR_DIV) {
        divider = &clock_div[target->div_bitmap[mux_sel]];
        divider->ops->enable(divider);
    }

    if (target->force_on != NULL)
        target->force_on->ops->set(target->force_on);

    if (target->change_bit != NULL) {
        target->change_bit->ops->set(target->change_bit);

        /* verified and polling change bit */
        target->change_bit->ops->verified(target->change_bit);
    }
    if (target->force_on != NULL)
        target->force_on->ops->clear(target->force_on);

    log_hal_info("%s\n", __FUNCTION__);
    return HAL_CLOCK_STATUS_OK;

#else

    if (mux_sel < (target->nr_inputs)) {

        uint32_t irq_mask = save_and_set_interrupt_mask();  /* disable interrupt for mux sel */

        /* Enable corresponding divider */
        if (target->div_bitmap[mux_sel] < NR_DIV) {
            divider = &clock_div[target->div_bitmap[mux_sel]];

            /* Clock source sel setting */
            for (i = 0; i < NR_CLKSRC_SEL_MAPPING; i++) {       /* NR_CLKSRC_SEL_MAPPING = 2 */
                if (target->clksrc_sel_map[i] != NONE_CLK_SOURCE_SEL) {
                    clksrc_sel = &clock_src_sels[target->clksrc_sel_map[i]];
                    clksrc_sel->ops->set(clksrc_sel, divider->grp->src_id);
                } else {
                    /* Do U/MPLL request */
                    if (divider->grp->pll != NULL && init_used == false) {
                        divider->grp->pll->ops->enable(divider->grp->pll);
                        pll_temporarily_enable = true;
                    }
                    break;
                }
            }

            divider->ops->enable(divider);
        }

        /* Change clock mux */
        if (init_used == false) {

            /* Save previous mux setting for disable divider, U/MPLL used */
            mux_sel_pre = target->ops->get_state(target);

            /* change mux setting to new value */
            *(target->base_addr) = (*(target->base_addr) & (~(target->sel_mask)));

            if (target->sel_mask == MSDC_CFG_MSDC_MUX_SEL_MASK)
                *(target->base_addr) |= (((mux_sel & MSDC_CFG_MSDC_MUX_SEL_LSB_MASK) << target->offset) |
                                         ((mux_sel & MSDC_CFG_MSDC_MUX_SEL_MSB_MASK) << MSDC_CFG_MSDC_MUX_SEL_MSB_OFFSET));
            else
                *(target->base_addr) |= (mux_sel << target->offset);

#ifdef CLK_DEBUG
            log_hal_info("%s @0x%x = %x\n", target->name, target->base_addr, *(target->base_addr));
#endif
            /* set force on*/
            if (target->force_on != NULL)
                target->force_on->ops->set(target->force_on);

            if (target->change_bit != NULL) {
                target->change_bit->ops->set(target->change_bit);

                /* verified and polling change bit */
                target->change_bit->ops->verified(target->change_bit);
            }

            /* clear force on*/
            if (target->force_on != NULL)
                target->force_on->ops->clear(target->force_on);

            /* Disable the temporarily enabled PLL */
            if (pll_temporarily_enable)
                divider->grp->pll->ops->disable(divider->grp->pll);

#ifdef CLK_DEBUG
            log_hal_info("%s: %s\n", __FUNCTION__, target->name);
#endif

            /* Disable previous divider and U/MPLL setting */
            if (target->div_bitmap[mux_sel_pre] < NR_DIV) {
                divider = &clock_div[target->div_bitmap[mux_sel_pre]];
                divider->ops->disable(divider);

                /* Don't disable clock source here and will only disable them during Vcore is 0.9V. */
                for (i = 0; i < NR_CLKSRC_SEL_MAPPING; i++) {               /* NR_CLKSRC_SEL_MAPPING = 2 */
                    if (target->clksrc_sel_map[i] == NONE_CLK_SOURCE_SEL && divider->grp->pll != NULL) {
                        divider->grp->pll->ops->disable(divider->grp->pll); /* Release U/MPLL request */
                        break;
                    }
                }
            }
        }

        restore_interrupt_mask(irq_mask);     /* restore interrupt for mux sel */
        return HAL_CLOCK_STATUS_OK;
    }
    /* TODO return fail mux_sel input parameter */
    log_hal_error("%s: %s, error\n", __FUNCTION__, target->name);
    return HAL_CLOCK_STATUS_ERROR;
#endif
}

ATTR_TEXT_IN_TCM static int8_t general_clock_mux_get_state(struct clock_mux *target)
{
#ifdef CLK_BRING_UP
    log_hal_info("%s\n", __FUNCTION__);
    return -1;
#endif
    int8_t mux;
    uint32_t tmp = *(target->base_addr) & target->sel_mask;

    if (target->sel_mask == MSDC_CFG_MSDC_MUX_SEL_MASK)
        mux = (((tmp >> target->offset) & MSDC_CFG_MSDC_MUX_SEL_LSB_MASK) |
               ((tmp >> MSDC_CFG_MSDC_MUX_SEL_MSB_OFFSET) & MSDC_CFG_MSDC_MUX_SEL_MSB_MASK));
    else
        mux = (tmp >> target->offset);

#ifdef CLK_DEBUG
    log_hal_info("%s, %s's mux state = %d\n", __FUNCTION__, target->name, mux);
#endif
    return mux;
}

ATTR_TEXT_IN_TCM static hal_clock_status_t general_clock_mux_request_pll(struct clock_mux *target, bool request)
{
    struct clock_div *divider = NULL;
    uint8_t mux;

#ifdef CLK_BRING_UP
    log_hal_info("%s\n", __FUNCTION__);
    return HAL_CLOCK_STATUS_OK;
#endif

    uint32_t irq_mask = save_and_set_interrupt_mask();  /* disable interrupt for mux sel */
    mux = target->ops->get_state(target); /*get mux option */
#ifdef CLK_DEBUG
    log_hal_info("%s, %s's mux = %d\n", __FUNCTION__, target->name, mux);
#endif
    if (mux < (target->nr_inputs)) {
        if (target->clksrc_sel_map[0] == NONE_CLK_SOURCE_SEL && target->div_bitmap[mux] < NR_DIV) {
            divider = &clock_div[target->div_bitmap[mux]];
            if (divider->grp->pll != NULL) {
                if (request)
                    divider->grp->pll->ops->enable(divider->grp->pll);
                else
                    divider->grp->pll->ops->disable(divider->grp->pll);
            }
        }
    }
    restore_interrupt_mask(irq_mask);                   /* restore interrupt for mux sel */

    return HAL_CLOCK_STATUS_OK;
}

ATTR_RWDATA_IN_TCM static struct clock_mux_ops clock_mux_ops = {
    /* .sel = */general_clock_mux_sel,
    /*.get_state = */general_clock_mux_get_state,
    /*.cg_request_pll = */general_clock_mux_request_pll,
};

ATTR_RWDATA_IN_TCM static struct clock_mux muxs[NR_MUXS] = {
    {
        /*.ops = */&clock_mux_ops,
        /*.force_on = */&clock_force_on[BUSCSW_FORCE_ON],
        /*.change_bit = */&clock_change_bit[CHG_BUS],
        /*.name = */__stringify(BUS_MUX_SEL),
        /*.sel_mask = */CLK_CONDB_BUS_MUX_SEL_MASK,
        /*.offset = */CLK_CONDB_BUS_MUX_SEL_OFFSET,
        /*.nr_inputs = */CLK_CONDB_BUS_MUX_SEL_NR_INPUTS,
        /*.div_bitmap = */{
            CLKSQ_F26M_CK, LFOSC_F26M_CK, UPLL_F62M_CK, MPLL_DIV5_CK,
            MPLL_DIV6_CK, HFOSC_DIV5_CK, HFOSC_DIV6_CK, CLK_DIV_NONE, CLK_DIV_NONE
        },
        /*.clksrc_sel_map = */{PDN_BUSCLK_SOURCE_SEL, NONE_CLK_SOURCE_SEL},
    }, {
        /*.ops = */&clock_mux_ops,
        /*.force_on = */&clock_force_on[CMCSW_FORCE_ON],
        /*.change_bit = */&clock_change_bit[CHG_CM],
        /*.name = */__stringify(CM_MUX_SEL),
        /*.sel_mask = */CLK_CONDB_CM_MUX_SEL_MASK,
        /*.offset = */CLK_CONDB_CM_MUX_SEL_OFFSET,
        /*.nr_inputs = */CLK_CONDB_CM_MUX_SEL_NR_INPUTS,
        /*.div_bitmap = */{
            CLKSQ_F26M_CK, LFOSC_F26M_CK, UPLL_F104M_CK, MPLL_DIV3_CK,
            HFOSC_DIV3_CK, MPLL_F208M_CK, MPLL_DIV2_CK, HFOSC_DIV1P5_CK, HFOSC_DIV2_CK
        },
        /*.clksrc_sel_map = */{CM4CLK_SOURCE_SEL, AO_BUSCLK_SOURCE_SEL},
    }, {
        /*.ops = */&clock_mux_ops,
        /*.force_on = */&clock_force_on[SFCCSW_FORCE_ON],
        /*.change_bit = */&clock_change_bit[CHG_SFC],
        /*.name = */__stringify(SFC_MUX_SEL),
        /*.sel_mask = */CLK_CONDB_SFC_MUX_SEL_MASK,
        /*.offset = */CLK_CONDB_SFC_MUX_SEL_OFFSET,
        /*.nr_inputs = */CLK_CONDB_SFC_MUX_SEL_NR_INPUTS,
        /*.div_bitmap = */{
            CLKSQ_F26M_CK, LFOSC_F26M_CK, MPLL_DIV3_CK, MPLL_F125M_CK,
            MPLL_DIV4_CK, MPLL_DIV5_CK, HFOSC_DIV3_CK, HFOSC_DIV4_CK, HFOSC_DIV5_CK
        },
        /*.clksrc_sel_map = */{SFCCLK_SOURCE_SEL, NONE_CLK_SOURCE_SEL},
    }, {
        /*.ops = */&clock_mux_ops,
        /*.force_on = */&clock_force_on[SLCDCSW_FORCE_ON],
        /*.change_bit = */&clock_change_bit[CHG_SLCD],
        /*.name = */__stringify(SLCD_MUX_SEL),
        /*.sel_mask = */CLK_CONDB_SLCD_MUX_SEL_MASK,
        /*.offset = */CLK_CONDB_SLCD_MUX_SEL_OFFSET,
        /*.nr_inputs = */CLK_CONDB_SLCD_MUX_SEL_NR_INPUTS,
        /*.div_bitmap = */{
            CLKSQ_F26M_CK, LFOSC_F26M_CK, MPLL_DIV2P5_CK, MPLL_DIV3_CK,
            MPLL_DIV4_CK, HFOSC_DIV2P5_CK, HFOSC_DIV3_CK, HFOSC_DIV4_CK, CLK_DIV_NONE
        },
        /*.clksrc_sel_map = */{NONE_CLK_SOURCE_SEL, NONE_CLK_SOURCE_SEL},
    }, {
        /*.ops = */&clock_mux_ops,
        /*.force_on = */&clock_force_on[BSICSW_FORCE_ON],
        /*.change_bit = */&clock_change_bit[CHG_BSI],
        /*.name = */__stringify(BSI_MUX_SEL),
        /*.sel_mask = */CLK_CONDB_BSI_MUX_SEL_MASK,
        /*.offset = */CLK_CONDB_BSI_MUX_SEL_OFFSET,
        /*.nr_inputs = */CLK_CONDB_BSI_MUX_SEL_NR_INPUTS,
        /*.div_bitmap = */{
            CLKSQ_F26M_CK, LFOSC_F26M_CK, MPLL_DIV2P5_CK, MPLL_DIV3_CK,
            HFOSC_DIV2P5_CK, HFOSC_DIV3_CK, CLK_DIV_NONE, CLK_DIV_NONE, CLK_DIV_NONE
        },
        /*.clksrc_sel_map = */{BSICLK_SOURCE_SEL, NONE_CLK_SOURCE_SEL},
    }, {
        /*.ops = */&clock_mux_ops,
        /*.force_on = */&clock_force_on[DSPCSW_FORCE_ON],
        /*.change_bit = */&clock_change_bit[CHG_DSP],
        /*.name = */__stringify(DSP_MUX_SEL),
        /*.sel_mask = */CLK_CONDB_DSP_MUX_SEL_MASK,
        /*.offset = */CLK_CONDB_DSP_MUX_SEL_OFFSET,
        /*.nr_inputs = */CLK_CONDB_DSP_MUX_SEL_NR_INPUTS,
        /*.div_bitmap = */{
            CLKSQ_F26M_CK, LFOSC_F26M_CK, MPLL_DIV2_CK, MPLL_F138M_CK,
            MPLL_DIV2P5_CK, UPLL_F124M_CK, HFOSC_DIV2_CK, HFOSC_DIV2P5_CK, CLK_DIV_NONE
        },
        /*.clksrc_sel_map = */{DSPCLK_SOURCE_SEL, NONE_CLK_SOURCE_SEL},
    }, {
        /*.ops = */&clock_mux_ops,
        /*.force_on = */&clock_force_on[CAMCSW_FORCE_ON],
        /*.change_bit = */&clock_change_bit[CHG_CAM],
        /*.name = */__stringify(CAM_MUX_SEL),
        /*.sel_mask = */CLK_CONDB_CAM_MUX_SEL_MASK,
        /*.offset = */CLK_CONDB_CAM_MUX_SEL_OFFSET,
        /*.nr_inputs = */CLK_CONDB_CAM_MUX_SEL_NR_INPUTS,
        /*.div_bitmap = */{
            CLKSQ_F26M_CK, LFOSC_F26M_CK, UPLL_48M_CK, UPLL_F312M_CK,
            CLK_DIV_NONE, CLK_DIV_NONE, CLK_DIV_NONE, CLK_DIV_NONE, CLK_DIV_NONE
        },
        /*.clksrc_sel_map = */{NONE_CLK_SOURCE_SEL, NONE_CLK_SOURCE_SEL},
    }, {
        /*.ops = */&clock_mux_ops,
        /*.force_on = */&clock_force_on[USBCSW_FORCE_ON],
        /*.change_bit = */&clock_change_bit[CHG_USB],
        /*.name = */__stringify(USB_MUX_SEL),
        /*.sel_mask = */CLK_CONDB_USB_MUX_SEL_MASK,
        /*.offset = */CLK_CONDB_USB_MUX_SEL_OFFSET,
        /*.nr_inputs = */CLK_CONDB_USB_MUX_SEL_NR_INPUTS,
        /*.div_bitmap = */{
            CLKSQ_F26M_CK, LFOSC_F26M_CK, UPLL_48M_CK, UPLL_F62M_CK,
            CLK_DIV_NONE, CLK_DIV_NONE, CLK_DIV_NONE, CLK_DIV_NONE, CLK_DIV_NONE
        },
        /*.clksrc_sel_map = */{NONE_CLK_SOURCE_SEL, NONE_CLK_SOURCE_SEL},
    }, {
        /*.ops = */&clock_mux_ops,
        /*.force_on = */&clock_force_on[DISPPWMCSW_FORCE_ON],
        /*.change_bit = */&clock_change_bit[CHG_DISP_PWM],
        /*.name = */__stringify(DISP_PWM_MUX_SEL),
        /*.sel_mask = */CLK_CONDB_DISP_PWM_MUX_SEL_MASK,
        /*.offset = */CLK_CONDB_DISP_PWM_MUX_SEL_OFFSET,
        /*.nr_inputs = */CLK_CONDB_DISP_PWM_MUX_SEL_NR_INPUTS,
        /*.div_bitmap = */{
            CLKSQ_F26M_CK, LFOSC_F26M_CK, UPLL_F104M_CK, HFOSC_DIV3_CK,
            CLK_DIV_NONE, CLK_DIV_NONE, CLK_DIV_NONE, CLK_DIV_NONE, CLK_DIV_NONE
        },
        /*.clksrc_sel_map = */{NONE_CLK_SOURCE_SEL, NONE_CLK_SOURCE_SEL},
    }, {
        /*.ops = */&clock_mux_ops,
        /*.force_on = */&clock_force_on[MSDC0CSW_FORCE_ON],
        /*.change_bit = */&clock_change_bit[CHG_MSDC0],
        /*.name = */__stringify(MSDC0_MUX_SEL),
        /*.sel_mask = */MSDC_CFG_MSDC_MUX_SEL_MASK,
        /*.offset = */MSDC_CFG_MSDC_MUX_SEL_OFFSET,
        /*.nr_inputs = */MSDC_CFG_MSDC_MUX_SEL_NR_INPUTS,
        /*.div_bitmap = */{
            CLKSQ_F26M_CK, LFOSC_F26M_CK, MPLL_DIV3P5_CK, MPLL_DIV4_CK,
            MPLL_DIV5_CK, HFOSC_DIV3P5_CK, HFOSC_DIV4_CK, HFOSC_DIV5_CK
        },
        /*.clksrc_sel_map = */{NONE_CLK_SOURCE_SEL, NONE_CLK_SOURCE_SEL},
    }, {
        /*.ops = */&clock_mux_ops,
        /*.force_on = */&clock_force_on[MSDC1CSW_FORCE_ON],
        /*.change_bit = */&clock_change_bit[CHG_MSDC1],
        /*.name = */__stringify(MSDC1_MUX_SEL),
        /*.sel_mask = */MSDC_CFG_MSDC_MUX_SEL_MASK,
        /*.offset = */MSDC_CFG_MSDC_MUX_SEL_OFFSET,
        /*.nr_inputs = */MSDC_CFG_MSDC_MUX_SEL_NR_INPUTS,
        /*.div_bitmap = */{
            CLKSQ_F26M_CK, LFOSC_F26M_CK, MPLL_DIV3P5_CK, MPLL_DIV4_CK,
            MPLL_DIV5_CK, HFOSC_DIV3P5_CK, HFOSC_DIV4_CK, HFOSC_DIV5_CK
        },
        /*.clksrc_sel_map = */{NONE_CLK_SOURCE_SEL, NONE_CLK_SOURCE_SEL},
    }, {
        /*.ops = */&clock_mux_ops,
        /*.force_on = */NULL,
        /*.change_bit = */&clock_change_bit[CHG_LP_CLKSQ],
        /*.name = */__stringify(LP_CLKSQ_MUX_SEL),
        /*.sel_mask = */CLK_CONDD_LP_CLKSQ_MUX_SEL_MASK,
        /*.offset = */CLK_CONDD_LP_CLKSQ_MUX_SEL_OFFSET,
        /*.nr_inputs = */CLK_CONDD_LP_CLKSQ_MUX_SEL_NR_INPUTS,
        /*.div_bitmap = */{
            CLK_DIV_NONE, CLK_DIV_NONE, CLK_DIV_NONE, CLK_DIV_NONE,
            CLK_DIV_NONE, CLK_DIV_NONE, CLK_DIV_NONE, CLK_DIV_NONE, CLK_DIV_NONE
        },
        /*.clksrc_sel_map = */{AO_BUSCLK_SOURCE_SEL, PDN_BUSCLK_SOURCE_SEL},
    }, {
        /*.ops = */&clock_mux_ops,
        /*.force_on = */NULL,
        /*.change_bit = */&clock_change_bit[CHG_LP_LFOSC],
        /*.name = */__stringify(LP_LFOSC_MUX_SEL),
        /*.sel_mask = */CLK_CONDD_LP_LFOSC_MUX_SEL_MASK,
        /*.offset = */CLK_CONDD_LP_LFOSC_MUX_SEL_OFFSET,
        /*.nr_inputs = */CLK_CONDD_LP_LFOSC_MUX_SEL_NR_INPUTS,
        /*.div_bitmap = */{
            CLK_DIV_NONE, CLK_DIV_NONE, CLK_DIV_NONE, CLK_DIV_NONE,
            CLK_DIV_NONE, CLK_DIV_NONE, CLK_DIV_NONE, CLK_DIV_NONE, CLK_DIV_NONE
        },
        /*.clksrc_sel_map = */{AO_BUSCLK_SOURCE_SEL, PDN_BUSCLK_SOURCE_SEL},
    }, {
        /*.ops = */&clock_mux_ops,
        /*.force_on = */NULL,
        /*.change_bit = */&clock_change_bit[CHG_SLCD],
        /*.name = */__stringify(RG_SLCD_CK_SEL),
        /*.sel_mask = */CLK_CONDD_RG_SLCD_CK_SEL_MASK,
        /*.offset = */CLK_CONDD_RG_SLCD_CK_SEL_OFFSET,
        /*.nr_inputs = */CLK_CONDD_RG_SLCD_CK_SEL_NR_INPUTS,
        /*.div_bitmap = */{
            CLK_DIV_NONE, MPLL_F125M_CK, CLK_DIV_NONE, CLK_DIV_NONE,
            CLK_DIV_NONE, CLK_DIV_NONE, CLK_DIV_NONE, CLK_DIV_NONE, CLK_DIV_NONE
        },
        /*.clksrc_sel_map = */{NONE_CLK_SOURCE_SEL, NONE_CLK_SOURCE_SEL},
    }, {
        /*.ops = */&clock_mux_ops,
        /*.force_on = */NULL,
        /*.change_bit = */NULL,
        /*.name = */__stringify(GP_F26M_GFMUX_SEL),
        /*.sel_mask = */CLK_CONDB_GP_F26M_GFMUX_SEL_MASK,
        /*.offset = */CLK_CONDB_GP_F26M_GFMUX_SEL_OFFSET,
        /*.nr_inputs = */CLK_CONDB_GP_F26M_GFMUX_SEL_NR_INPUTS,
        /*.div_bitmap = */{
            CLKSQ_F26M_CK, LFOSC_F26M_CK, CLK_DIV_NONE, CLK_DIV_NONE,
            CLK_DIV_NONE, CLK_DIV_NONE, CLK_DIV_NONE, CLK_DIV_NONE, CLK_DIV_NONE
        },
        /*.clksrc_sel_map = */{NONE_CLK_SOURCE_SEL, NONE_CLK_SOURCE_SEL},
    }, {
        /*.ops = */&clock_mux_ops,
        /*.force_on = */NULL,
        /*.change_bit = */NULL,
        /*.name = */__stringify(LP_F26M_GFMUX_SEL),
        /*.sel_mask = */CLK_CONDB_LP_F26M_GFMUX_SEL_MASK,
        /*.offset = */CLK_CONDB_LP_F26M_GFMUX_SEL_OFFSET,
        /*.nr_inputs = */CLK_CONDB_LP_F26M_GFMUX_SEL_NR_INPUTS,
        /*.div_bitmap = */{
            CLKSQ_F26M_CK, LFOSC_F26M_CK, CLK_DIV_NONE, CLK_DIV_NONE,
            CLK_DIV_NONE, CLK_DIV_NONE, CLK_DIV_NONE, CLK_DIV_NONE, CLK_DIV_NONE
        },
        /*.clksrc_sel_map = */{NONE_CLK_SOURCE_SEL, NONE_CLK_SOURCE_SEL},
    },
};

/*************************************************************************
 * Clock mux select API definition part
 *************************************************************************/
ATTR_TEXT_IN_TCM hal_clock_status_t clock_mux_sel(clock_mux_sel_id mux_id, uint32_t mux_sel)
{
    if (!clock_initialized) {
        /* TODO cannot use log_hal_info print log before syslog init */
        log_hal_error("%s: clock is uninitialized\n", __FUNCTION__);
        return HAL_CLOCK_STATUS_UNINITIALIZED;
    }

    if (mux_id < NR_MUXS) {
        /* TODO do something relations setting for mux_sel */
#ifdef CLK_DEBUG
        log_hal_info("%s\n", __FUNCTION__);
#endif
        struct clock_mux *target = &muxs[mux_id];
        return target->ops->sel(target, mux_sel, false);
    } else {
        /* TODO return fail id information */
        log_hal_error("%s, error\n", __FUNCTION__);
        return HAL_CLOCK_STATUS_INVALID_PARAMETER;
    }
}

ATTR_TEXT_IN_TCM int8_t clock_mux_get_state(clock_mux_sel_id mux_id)
{
    if (!clock_initialized) {
        /* TODO cannot use log_hal_info print log before syslog init */
        log_hal_error("%s: clock is uninitialized\n", __FUNCTION__);
        return HAL_CLOCK_STATUS_UNINITIALIZED;
    }

    if (mux_id < NR_MUXS) {
        /* TODO do something relations setting for mux_sel */
#ifdef CLK_DEBUG
        log_hal_info("%s\n", __FUNCTION__);
#endif
        struct clock_mux *target = &muxs[mux_id];
        return target->ops->get_state(target);
    } else {
        /* TODO return fail id information */
        log_hal_error("%s, error\n", __FUNCTION__);
        return HAL_CLOCK_STATUS_INVALID_PARAMETER;
    }
}

ATTR_TEXT_IN_TCM static void clock_api_internal_reset(void)
{
    int i;
    uint32_t irq_mask = save_and_set_interrupt_mask();  /* disable interrupt for mux sel */

    for (i = 0; i < NR_PLL_CLOCKS; i++)
        (&pll_clock[i])->cnt = 0;

    for (i = 0; i < NR_DIV; i++)
        (&clock_div[i])->cnt = 0;

    restore_interrupt_mask(irq_mask);                   /* restore interrupt for mux sel */
}

ATTR_TEXT_IN_TCM static void clock_api_internal_init(void)
{
    int i;
    struct clock_mux *target;
    uint32_t irq_mask = save_and_set_interrupt_mask();  /* disable interrupt for mux sel */

    for (i = 0; i < NR_MUXS; i++) {
        target = &muxs[i];
        target->ops->sel(target, target->ops->get_state(target), true);
    }

    restore_interrupt_mask(irq_mask);                   /* restore interrupt for mux sel */
}

ATTR_TEXT_IN_TCM void clock_mux_init(void)
{
    int i;

    /* MUX init */
    /* For LP_CLKSQ_MUX_SEL, LP_LFOSC_MUX_SEL, RG_SLCD_CK_SEL, */
    for (i = LP_CLKSQ_MUX_SEL; i < GP_F26M_GFMUX_SEL; i++) {
        muxs[i].base_addr = &(CONFIGSYS->CLK_CONDD);
    }
    /* For GP_F26M_GFMUX_SEL, LP_F26M_GFMUX_SEL, */
    for (i = GP_F26M_GFMUX_SEL; i < NR_MUXS; i++) {
        muxs[i].base_addr = &(CONFIGSYS->CLK_CONDB);
    }
    /* For BUS_MUX_SEL, CM_MUX_SEL, SFC_MUX_SEL, SLCD_MUX_SEL, BSI_MUX_SEL, DSP_MUX_SEL, CAM_MUX_SEL, USB_MUX_SEL, DISP_PWM_MUX_SEL, */
    for (i = BUS_MUX_SEL; i < MSDC0_MUX_SEL; i++) {
        muxs[i].base_addr = &(CONFIGSYS->CLK_CONDB);
    }
    /* For MSDC0_MUX_SEL, MSDC1_MUX_SEL, */
    muxs[MSDC0_MUX_SEL].base_addr = &(CLK_USED_MSDC0->MSDC_CFG);
    muxs[MSDC1_MUX_SEL].base_addr = &(CLK_USED_MSDC1->MSDC_CFG);

    /* FORCE ON init */
    for (i = BUSCSW_FORCE_ON; i < NR_CLKFORCE_ON; i++) {
        clock_force_on[i].base_addr = &(CONFIGSYS->CLK_CONDD);
    }

    /* CHANGE BIT init */
    for (i = CHG_BUS; i < NR_CLKCHANGE_BIT; i++) {
        clock_change_bit[i].base_addr = &(ANA_CFGSYS->ACFG_CLK_UPDATE);
    }

    /* DIVIDER init */
    for (i = HFOSC_DIV_GRP; i < NR_DIV_GRP; i++) {
        clock_div_grp[i].base_addr = &(CONFIGSYS->CLK_CONDA);
    }
}

#define NR_MUX_MAPPING 32           /* Maximun mapping for mux to cg grp bit */
/*************************************************************************
 * CG Structure definition
 *************************************************************************/
struct cg_grp;
struct cg_grp_ops {
    /* int (*prepare)(struct cg_grp *grp); */
    /* int (*finished)(struct cg_grp *grp); */
    unsigned int (*get_state)(struct cg_grp *target);
    /* int (*dump_regs)(struct cg_grp *grp, unsigned int *ptr); */
};

struct cg_grp {
    struct cg_grp_ops *ops;
    const char *name;
    unsigned int state;
    const uint8_t mux_map[NR_MUX_MAPPING];
    volatile const uint32_t *sta_addr;
    volatile uint32_t *set_addr;
    volatile uint32_t *clr_addr;
};

struct cg_clock;
struct cg_clock_ops {
    /* int (*check_validity)(struct cg_clock *target); //1: valid, 0: invalid */
    bool (*get_state)(struct cg_clock *target, uint32_t bit_idx);
    hal_clock_status_t (*enable)(struct cg_clock *target, uint32_t bit_idx);
    hal_clock_status_t (*disable)(struct cg_clock *target, uint32_t bit_idx);
    hal_clock_status_t (*disable_force)(struct cg_clock *target, uint32_t bit_idx);
};

struct cg_clock {
    struct cg_clock_ops *ops;
    struct cg_grp *grp;
    unsigned int state;
    unsigned int force_on;
    int cnt;
};

ATTR_ZIDATA_IN_TCM static struct cg_clock clocks[NR_CLOCKS];

/*************************************************************************
 * CG Group function definition
 *************************************************************************/
ATTR_TEXT_IN_TCM static unsigned int general_grp_get_state_op(struct cg_grp *target)
{
    return 0;
}

ATTR_RWDATA_IN_TCM static struct cg_grp_ops general_cg_grp_ops = {
    /* .get_state = */general_grp_get_state_op
};

/*************************************************************************
 * CG Group definition
 *************************************************************************/
ATTR_RWDATA_IN_TCM static struct cg_grp grps[NR_GRPS] = {
    {
        /*.ops = */&general_cg_grp_ops,
        /*.name = */__stringify(GRP_PDN_COND0),
        /*.state = */0,
        /*.mux_map = */{
            CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   /*  0 -  7 bit */
            CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CAM_MUX_SEL,        CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   BUS_MUX_SEL,        CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   /*  8 - 15 bit */
            CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   BUS_MUX_SEL,        CM_MUX_SEL,         CLK_MUX_SEL_NONE,   BSI_MUX_SEL,        /* 16 - 23 bit */
            CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   /* 24 - 31 bit */
        },
    }, {
        /*.ops = */&general_cg_grp_ops,
        /*.name = */__stringify(GRP_PDN_COND1),
        /*.state = */0,
        /*.mux_map = */{
            BUS_MUX_SEL,        BUS_MUX_SEL,        USB_MUX_SEL,        MSDC0_MUX_SEL,      CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   BUS_MUX_SEL,        BUS_MUX_SEL,        /*  0 -  7 bit */
            CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   BUS_MUX_SEL,        CLK_MUX_SEL_NONE,   BUS_MUX_SEL,        CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   /*  8 - 15 bit */
            CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   BUS_MUX_SEL,        CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   /* 16 - 23 bit */
            CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   /* 24 - 31 bit */
        },
    }, {
        /*.ops = */&general_cg_grp_ops,
        /*.name = */__stringify(GRP_PDN_COND2),
        /*.state = */0,
        /*.mux_map = */{
            CLK_MUX_SEL_NONE,   BUS_MUX_SEL,        CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   /*  0 -  7 bit */
            CM_MUX_SEL,         SFC_MUX_SEL,        MSDC1_MUX_SEL,      CLK_MUX_SEL_NONE,   BUS_MUX_SEL,        BUS_MUX_SEL,        DISP_PWM_MUX_SEL,   CLK_MUX_SEL_NONE,   /*  8 - 15 bit */
            BUS_MUX_SEL,        CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   /* 16 - 23 bit */
            CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   /* 24 - 31 bit */
        },
    }, {
        /*.ops = */&general_cg_grp_ops,
        /*.name = */__stringify(GRP_CM_PDN_COND0),
        /*.state = */0,
        /*.mux_map = */{
            CM_MUX_SEL,         CM_MUX_SEL,         CM_MUX_SEL,         CM_MUX_SEL,         CM_MUX_SEL,         CM_MUX_SEL,         CM_MUX_SEL,         CM_MUX_SEL,         /*  0 -  7 bit */
            CM_MUX_SEL,         CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   /*  8 - 15 bit */
            CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   /* 16 - 23 bit */
            CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   /* 24 - 31 bit */
        },
    } , {
        /*.ops = */&general_cg_grp_ops,
        /*.name = */__stringify(GRP_ACFG_PDN_RG),
        /*.state = */0,
        /*.mux_map = */{
            CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   /*  0 -  7 bit */
            CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   /*  8 - 15 bit */
            CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   /* 16 - 23 bit */
            CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   CLK_MUX_SEL_NONE,   /* 24 - 31 bit */
        },
    }
};

ATTR_RODATA_IN_TCM static const uint32_t clk_cg_mask[NR_GRPS] = {
    /*  GRP_PDN_COND0 bits 23, 21, 20, 15, 13, 10, 9 */
    0x00B0A600,

    /*  GRP_PDN_COND1 bits 23, 22, 21, 20, 19, 16, 15, */
    /*  14, 13, 12, 10, 7, 6, 3, 2, 1, 0 */
    0x00F9F4CF,

    /*  GRP_PDN_COND2 bits 22, 21, 20, 19, 16, 14, 13, */
    /*  12, 10, 9, 8, 7, 6, 4, 3, 2, 1, 0 */
    0x007977DF,

    /*  GRP_CM_PDN_COND0 bits 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 */
    0x000003FF,

    /*  GRP_ACFG_PDN_RG bits 10, 9, 8, 6, 2 */
    0x00000744,
};

/*************************************************************************
 * CG Clock function definition
 *************************************************************************/
ATTR_TEXT_IN_TCM bool general_clock_get_state_op(struct cg_clock *target, uint32_t bit_idx)
{
    if (((*(target->grp->sta_addr)) & (0x1 << bit_idx)) != 0x0) {
        /* TODO cannot use log_hal_info print log before syslog init */
#ifdef CLK_DEBUG
        log_hal_info("%s: %s: bit = %d: clock is disabled\n", __FUNCTION__, target->grp->name, bit_idx);
#endif
        return false;
    } else {
        /* TODO cannot use log_hal_info print log before syslog init */
#ifdef CLK_DEBUG
        log_hal_info("%s: %s: bit = %d: clock is enabled\n", __FUNCTION__, target->grp->name, bit_idx);
#endif
        return true;
    }
}

/*
ATTR_TEXT_IN_TCM static int general_clock_check_validity_op(struct cg_clock *target)
{
    int valid = 0;
    return valid;
}
*/

ATTR_TEXT_IN_TCM static hal_clock_status_t general_clock_enable_op(struct cg_clock *target, uint32_t bit_idx)
{
    struct clock_mux *mux;
    uint32_t irq_mask = save_and_set_interrupt_mask();  /* disable interrupt */

#ifdef CLK_DEBUG
    log_hal_info("%s enable %d, %d < %d\n\n", target->grp->name, bit_idx, target->grp->mux_map[bit_idx], NR_MUXS);
#endif

    if (target->grp->mux_map[bit_idx] < NR_MUXS) {      /* integrate to mux, check upll/mpll request set */
        mux = &muxs[target->grp->mux_map[bit_idx]];
        mux->ops->cg_request_pll(mux, true);            /* cg request pll */
    }

    if (target->cnt == 0) {
        *(target->grp->clr_addr) = (0x1 << bit_idx);    /* HW Register is write 1 clear */
        target->state = 1;
    }
    if (target->cnt < 32767) {
        target->cnt++;
    }

    restore_interrupt_mask(irq_mask);                   /* restore interrupt */
    /* TODO cannot use log_hal_info print log before syslog init */
#ifdef CLK_DEBUG
    log_hal_info("%s, ref_cnt = %d, @0x%x = %x\n", target->grp->name, target->cnt, target->grp->sta_addr, *(target->grp->sta_addr));
#endif
    return HAL_CLOCK_STATUS_OK;
}

ATTR_TEXT_IN_TCM static hal_clock_status_t general_clock_disable_op(struct cg_clock *target, uint32_t bit_idx)
{
    struct clock_mux *mux;
    uint32_t irq_mask = save_and_set_interrupt_mask();  /* disable interrupt */

    if (target->cnt <= 1) {
        *(target->grp->set_addr) = (0x1 << bit_idx);    /* HW Register is write 1 clear */
        target->state = 0;
    }
    if (target->cnt > 0) {
        target->cnt--;
    }

#ifdef CLK_DEBUG
    log_hal_info("%s disable %d, %d < %d\n\n", target->grp->name, bit_idx, target->grp->mux_map[bit_idx], NR_MUXS);
#endif

    if (target->grp->mux_map[bit_idx] < NR_MUXS) {      /* integrate to mux, check upll/mpll request clear */
        mux = &muxs[target->grp->mux_map[bit_idx]];
        mux->ops->cg_request_pll(mux, false);           /* cg release pll */
    }

    restore_interrupt_mask(irq_mask);                   /* restore interrupt */
    /* TODO cannot use log_hal_info print log before syslog init */
#ifdef CLK_DEBUG
    log_hal_info("%s, ref_cnt = %d, @0x%x = %x\n", target->grp->name, target->cnt, target->grp->sta_addr, *(target->grp->sta_addr));
#endif
    return HAL_CLOCK_STATUS_OK;
}

ATTR_TEXT_IN_TCM static hal_clock_status_t general_clock_force_disable_op(struct cg_clock *target, uint32_t bit_idx)
{
    uint32_t irq_mask = save_and_set_interrupt_mask();  /* disable interrupt */

    *(target->grp->set_addr) = (0x1 << bit_idx);        /* HW Register is write 1 clear */

    target->state = 0;
    target->cnt = 0;

    restore_interrupt_mask(irq_mask);                   /* restore interrupt */

    return HAL_CLOCK_STATUS_OK;
}

ATTR_RWDATA_IN_TCM static struct cg_clock_ops general_cg_clock_ops = {
    /*.get_state = */general_clock_get_state_op,
    /*.enable = */general_clock_enable_op,
    /*.disable = */general_clock_disable_op,
    /*.disable_force = */general_clock_force_disable_op,
    /* .check_validity = general_clock_check_validity_op, */
};

/* TODO
static struct cg_grp *id_to_grp(uint32_t grp_id)
{
    return grp_id < NR_GRPS ? grps + grp_id : NULL;
}
*/

/*************************************************************************
 * Clock gating register and structure initialize
 *************************************************************************/
ATTR_TEXT_IN_TCM void cg_init(void)
{
    int i = 0;

    grps[GRP_PDN_COND0].sta_addr = &(CONFIGSYS->PDN_COND0);
    grps[GRP_PDN_COND0].set_addr = &(CONFIGSYS->PDN_SETD0);
    grps[GRP_PDN_COND0].clr_addr = &(CONFIGSYS->PDN_CLRD0);
    grps[GRP_PDN_COND1].sta_addr = &(CONFIGSYS->PDN_COND1);
    grps[GRP_PDN_COND1].set_addr = &(CONFIGSYS->PDN_SETD1);
    grps[GRP_PDN_COND1].clr_addr = &(CONFIGSYS->PDN_CLRD1);
    grps[GRP_PDN_COND2].sta_addr = &(CONFIGSYS->PDN_COND2);
    grps[GRP_PDN_COND2].set_addr = &(CONFIGSYS->PDN_SETD2);
    grps[GRP_PDN_COND2].clr_addr = &(CONFIGSYS->PDN_CLRD2);
    grps[GRP_CM_PDN_COND0].sta_addr = &(CM_MEMS_DCM->CM_PDN_COND0);
    grps[GRP_CM_PDN_COND0].set_addr = &(CM_MEMS_DCM->CM_PDN_SETD0);
    grps[GRP_CM_PDN_COND0].clr_addr = &(CM_MEMS_DCM->CM_PDN_CLRD0);
    grps[GRP_ACFG_PDN_RG].sta_addr = &(ANA_CFGSYS->ACFG_CLK_CG);
    grps[GRP_ACFG_PDN_RG].set_addr = &(ANA_CFGSYS->ACFG_CLK_CG_SET);
    grps[GRP_ACFG_PDN_RG].clr_addr = &(ANA_CFGSYS->ACFG_CLK_CG_CLR);

    for (i = 0; i < NR_CLOCKS; i++) {
        if (i >= PDN_COND0_FROM && i <= PDN_COND0_TO) {
            clocks[i].ops = &general_cg_clock_ops;
            clocks[i].grp = &grps[GRP_PDN_COND0];
        } else if (i >= PDN_COND1_FROM && i <= PDN_COND1_TO) {
            clocks[i].ops = &general_cg_clock_ops;
            clocks[i].grp = &grps[GRP_PDN_COND1];
        } else if (i >= PDN_COND2_FROM && i <= PDN_COND2_TO) {
            clocks[i].ops = &general_cg_clock_ops;
            clocks[i].grp = &grps[GRP_PDN_COND2];
        } else if (i >= CM_PDN_COND0_FROM && i <= CM_PDN_COND0_TO) {
            clocks[i].ops = &general_cg_clock_ops;
            clocks[i].grp = &grps[GRP_CM_PDN_COND0];
        } else if (i >= ACFG_PDN_RG_FROM && i <= ACFG_PDN_RG_TO) {
            clocks[i].ops = &general_cg_clock_ops;
            clocks[i].grp = &grps[GRP_ACFG_PDN_RG];
        }
    }
}   /*  void cg_init(void) */

/*************************************************************************
 * CG Clock definition
 *************************************************************************/
/*
#if 0
ATTR_RWDATA_IN_TCM static struct cg_clock clocks[NR_CLOCKS] = {
    [PDN_COND0_FROM ... PDN_COND0_TO] = {
        .cnt = 0,
        .state = 0,
        .ops = &general_cg_clock_ops,
        .grp = &grps[GRP_PDN_COND0],
    },
    [PDN_COND1_FROM ... PDN_COND1_TO] = {
        .cnt = 0,
        .state = 0,
        .ops = &general_cg_clock_ops,
        .grp = &grps[GRP_PDN_COND1],
    },
    [PDN_COND2_FROM ... PDN_COND2_TO] = {
        .cnt = 0,
        .state = 0,
        .ops = &general_cg_clock_ops,
        .grp = &grps[GRP_PDN_COND2],
    },
    [CM_PDN_COND0_FROM ... CM_PDN_COND0_TO] = {
        .cnt = 0,
        .state = 0,
        .ops = &general_cg_clock_ops,
        .grp = &grps[GRP_CM_PDN_COND0],
    },
    [ACFG_PDN_RG_FROM ... ACFG_PDN_RG_TO] = {
        .cnt = 0,
        .state = 0,
        .ops = &general_cg_clock_ops,
        .grp = &grps[GRP_ACFG_PDN_RG],
    },
};
#endif
*/

/*************************************************************************
 * Internal used function definition
 *************************************************************************/
ATTR_TEXT_IN_TCM static struct cg_clock *id_to_clock(hal_clock_cg_id clock_id)
{
    return ((uint32_t) clock_id < (uint32_t) NR_CLOCKS) ? (clocks + (uint32_t) clock_id) : NULL;
}

ATTR_TEXT_IN_TCM static hal_clock_status_t _hal_clock_enable_(hal_clock_cg_id clock_id, char const *file_name)
{
    struct cg_clock *target = id_to_clock(clock_id);

    if (!clock_initialized) {
        /* TODO cannot use log_hal_info print log before syslog init */
        log_hal_error("%s: clock is uninitialized\n", __FUNCTION__);
        return HAL_CLOCK_STATUS_UNINITIALIZED;
    }

    /* TODO file name tracking for debugging */
    /* if (file_name != NULL) ...            */

    if (target != NULL)
        return target->ops->enable(target, clock_id % 32);
    else
        return HAL_CLOCK_STATUS_INVALID_PARAMETER;
}

ATTR_TEXT_IN_TCM static hal_clock_status_t _hal_clock_disable_(hal_clock_cg_id clock_id, char const *file_name)
{
    struct cg_clock *target = id_to_clock(clock_id);

    if (!clock_initialized) {
        /* TODO cannot use log_hal_info print log before syslog init */
        log_hal_error("%s: clock is uninitialized\n", __FUNCTION__);
        return HAL_CLOCK_STATUS_UNINITIALIZED;
    }

    /* TODO file name tracking for debugging */
    /* if (file_name != NULL) ...            */

    if (target != NULL)
        return target->ops->disable(target, clock_id % 32);
    else
        return HAL_CLOCK_STATUS_INVALID_PARAMETER;
}

/* Debug only */
ATTR_TEXT_IN_TCM hal_clock_status_t hal_clock_force_disable(hal_clock_cg_id clock_id)
{
    struct cg_clock *target = id_to_clock(clock_id);

    if (!clock_initialized) {
        log_hal_error("%s: clock is uninitialized\n", __FUNCTION__);
        return HAL_CLOCK_STATUS_UNINITIALIZED;
    }

    if (target != NULL)
        return target->ops->disable_force(target, clock_id % 32);
    else
        return HAL_CLOCK_STATUS_INVALID_PARAMETER;
}

/*************************************************************************
 * CG Clock API definition
 *************************************************************************/
ATTR_TEXT_IN_TCM hal_clock_status_t hal_clock_enable(hal_clock_cg_id clock_id)
{
#ifdef CLK_DEBUG
    /* TODO cannot print log before log_hal_info init done */
    log_hal_info("%s: clock_id=%d\n", __FUNCTION__, clock_id);
#endif /* ifdef CLK_DEBUG */
#ifdef CLK_BRING_UP
    return HAL_CLOCK_STATUS_OK;
#else
#ifdef CLK_DEBUG
    /* TODO cannot print log before log_hal_info init done */
    return _hal_clock_enable_(clock_id, __FILE__);
#else
    /* TODO cannot print log before log_hal_info init done */
    return _hal_clock_enable_(clock_id, NULL);
#endif /* ifdef CLK_DEBUG */
#endif /* ifdef CLK_BRING_UP */
}

ATTR_TEXT_IN_TCM hal_clock_status_t hal_clock_disable(hal_clock_cg_id clock_id)
{
#ifdef CLK_DEBUG
    /* TODO cannot print log before log_hal_info init done */
    log_hal_info("%s: clock_id=%d\n", __FUNCTION__, clock_id);
#endif
#ifdef CLK_BRING_UP
    return HAL_CLOCK_STATUS_OK;
#else
#ifdef CLK_DEBUG
    /* TODO cannot print log before log_hal_info init done */
    return _hal_clock_disable_(clock_id, __FILE__);
#else
    /* TODO cannot print log before log_hal_info init done */
    return _hal_clock_disable_(clock_id, NULL);
#endif /* ifdef CLK_DEBUG */
#endif /* ifdef CLK_BRING_UP */
}

ATTR_TEXT_IN_TCM bool hal_clock_is_enabled(hal_clock_cg_id clock_id)
{
    struct cg_clock *target = id_to_clock(clock_id);

#ifdef CLK_BRING_UP
    return true;
#endif

    if (!clock_initialized) {
        /* TODO cannot use log_hal_info print log before syslog init */
        log_hal_error("%s: clock is uninitialized\n", __FUNCTION__);
        return false;
    }

    if (target != NULL) {
        return target->ops->get_state(target, clock_id % 32);
    }
    return false;
}

ATTR_TEXT_IN_TCM void clock_pre_on(void)
{
    hal_clock_enable(HAL_CLOCK_CG_CM4_OSTIMER);
}

ATTR_TEXT_IN_TCM void clock_all_on(void)
{
#if 1
    int grp_idx, bit_idx;
    for (grp_idx = 0; grp_idx < NR_GRPS; grp_idx++) {
        for (bit_idx = 0; bit_idx < 32; bit_idx++) {
            if ((clk_cg_mask[grp_idx] >> bit_idx) & 0x1) {
                _hal_clock_enable_((hal_clock_cg_id)(bit_idx + (32 * grp_idx)), NULL);
            }
        }
    }
#else
    *(volatile uint32_t *)(CONFIG_BASE + 0x320) = 0x00B0A600;       /* PDN_CLRD0 @A201_0320 */
    *(volatile uint32_t *)(CONFIG_BASE + 0x324) = 0x00F9F4CF;       /* PDN_CLRD1 @A201_0324 */
    *(volatile uint32_t *)(CONFIG_BASE + 0x328) = 0x007977DF;       /* PDN_CLRD2 @A201_0328 */
    *(volatile uint32_t *)(CM_MEMS_DCM_BASE + 0x20) = 0x000003FF;   /* CM_PDN_CLRD0 @0xA20A_0020 */
    *(volatile uint32_t *)(ANA_CFGSYS_BASE + 0x10) = 0x00000744;    /* ACFG_CLK_CG_CLR @0xA21D_0010 */
#endif
}

ATTR_TEXT_IN_TCM void clock_all_off(void)
{
#if 1
    int grp_idx, bit_idx;
    for (grp_idx = 0; grp_idx < NR_GRPS; grp_idx++) {
        for (bit_idx = 0; bit_idx < 32; bit_idx++) {
            if ((clk_cg_mask[grp_idx] >> bit_idx) & 0x1) {
                _hal_clock_disable_((hal_clock_cg_id)(bit_idx + (32 * grp_idx)), NULL);
            }
        }
    }
#else
    *(volatile uint32_t *)(CONFIG_BASE + 0x310) = 0x00B0A600;       /* PDN_SETD0 @A201_0310 */
    *(volatile uint32_t *)(CONFIG_BASE + 0x314) = 0x00F9F4CF;       /* PDN_SETD1 @A201_0314 */
    *(volatile uint32_t *)(CONFIG_BASE + 0x318) = 0x007977DF;       /* PDN_SETD2 @A201_0318 */
    *(volatile uint32_t *)(CM_MEMS_DCM_BASE + 0x10) = 0x000003FF;   /* CM_PDN_SETD0 @0xA20A_0010 */
    *(volatile uint32_t *)(ANA_CFGSYS_BASE + 0x0C) = 0x00000744;    /* ACFG_CLK_CG_SET @0xA21D_000C */
#endif
}

/*
 * Funtion: Query frequency meter
 * tcksel: TESTED clock selection
 * fcksel: FIXED clock selection
 * over78m: input "true" if frequency is over 78MHz
 * return frequency unit: KHz
 */
ATTR_TEXT_IN_TCM uint32_t clock_get_freq_meter(uint16_t tcksel, uint16_t fcksel, bool over78m)
{
    uint32_t target_freq = 0, tmp = 0;

    /* select fqmtr_ck by FQMTR_TCKSEL  bit[4:0], select fixed_ck by FQMTR_FCKSEL[2:0] */
    MIXEDSYSD->ABIST_FQMTR_CON1 = (tcksel << 0) | (fcksel << 8);
#ifdef CLK_DEBUG
    log_hal_info("%s: ABIST_FQMTR_CON1=0x%x \n\r", __func__, MIXEDSYSD->ABIST_FQMTR_CON1);
#endif
    /* fqmtr_rst = 0, for divider, set winset=100 */
    MIXEDSYSD->ABIST_FQMTR_CON0 = 0x0000CFFF; /* rest meter */
    hal_gpt_delay_us(2);
    MIXEDSYSD->ABIST_FQMTR_CON0 = 0x00008FFF;

    /* FQMTR_TCKSEL = 0x4, 0x6, 0xC etc. >78MHz clock need this define */
    if (over78m) {
        MIXEDSYSD->ABIST_FQMTR_CON2 = 0x00000001;     /* fqmtr_ck div 4 */
        MIXEDSYSD->ABIST_FQMTR_CON2 = 0x00000011;     /* enable div clock */
    } else
        MIXEDSYSD->ABIST_FQMTR_CON2 = 0x00000000;     /* disable div clock */

    tmp = MIXEDSYSD->ABIST_FQMTR_CON1;
    tmp = MIXEDSYSD->ABIST_FQMTR_CON1;
    while (tmp & (0x1 << 15)) {
        tmp = MIXEDSYSD->ABIST_FQMTR_CON1;
    }

    hal_gpt_delay_us(6);

    /* fqmtr_ck = fixed_ck*fqmtr_data/winset, */
    /* ex. 312*0.96=26*fqmtr_data/100, expect fqmtr_data=0x480 */
    target_freq = (26 * 1000 * MIXEDSYSD->ABIST_FQMTR_DATA) / 4096;
    if (over78m)
	target_freq = (26 * 4 * 1000 * MIXEDSYSD->ABIST_FQMTR_DATA) / 4096;
    else {
	target_freq = (26 * 10000 * MIXEDSYSD->ABIST_FQMTR_DATA) / 4096;
	target_freq /= 10;
    }

#ifdef CLK_DEBUG
    log_hal_info("[PLL] %s: ABIST_FQMTR_CON2=0x%x,CON1=0x%x,DATA=0x%x \n\r", __func__,
                 MIXEDSYSD->ABIST_FQMTR_CON2, MIXEDSYSD->ABIST_FQMTR_CON1, MIXEDSYSD->ABIST_FQMTR_DATA);
#endif

    return target_freq;
}

#if 0
/*
 * Funtion: Query frequency meter & set GPIO30 as debug monitor to measure clock (DEBUG ONLY)
 * tcksel: TESTED clock selection
 * fcksel: FIXED clock selection
 * over78m: input "true" if frequency is over 78MHz
 * return frequency unit: MHz
 */
ATTR_TEXT_IN_TCM static uint32_t clock_get_freq_monitor(uint16_t tcksel, uint16_t fcksel, bool over78m)
{
    uint32_t target_freq = 0, tmp = 0;
    uint32_t bak_gpio30_mode3, bak_gpio30_dir0;

    /* select fqmtr_ck by FQMTR_TCKSEL  bit[4:0], select fixed_ck by FQMTR_FCKSEL[2:0] */
    MIXEDSYSD->ABIST_FQMTR_CON1 = (tcksel << 0) | (fcksel << 8);
#ifdef CLK_DEBUG
    log_hal_info("%s: ABIST_FQMTR_CON1=0x%x \n\r", __func__, MIXEDSYSD->ABIST_FQMTR_CON1);
#endif
    /* fqmtr_rst = 0, for divider, set winset=100 */
    MIXEDSYSD->ABIST_FQMTR_CON0 = 0x0000CFFF; /* rest meter */
    hal_gpt_delay_us(2);
    MIXEDSYSD->ABIST_FQMTR_CON0 = 0x00008FFF;

    /* FQMTR_TCKSEL = 0x4, 0x6, 0xC etc. >78MHz clock need this define */
    if (over78m) {
        MIXEDSYSD->ABIST_FQMTR_CON2 = 0x00000001;     /* fqmtr_ck div 4 */
        MIXEDSYSD->ABIST_FQMTR_CON2 = 0x00000011;     /* enable div clock */
    } else
        MIXEDSYSD->ABIST_FQMTR_CON2 = 0x00000000;     /* disable div clock */

    MIXEDSYSD->SYS_ABIST_MON_CON1 = 0x00000201;   /* mon sel to mon_fq_mtr_ck */

    bak_gpio30_mode3 = (FREQ_METER_GPIO_USED->GPIO_MODE3 & 0x0F000000); /* backup setting for gpio30 in mode3 */
    bak_gpio30_dir0 = (FREQ_METER_GPIO_USED->GPIO_DIR0 & 0x40000000);   /* backup setting for gpio30 in dir0 */

    CONFIGSYS->TOP_DEBUG = 0x0000000E;                      /* debug_sel to mixedsys_d output */
    FREQ_METER_GPIO_USED->GPIO_MODE3_CLR = 0x0F000000;      /* clear setting for gpio30 in mode3 */
    FREQ_METER_GPIO_USED->GPIO_MODE3_SET = 0x07000000;      /* GPIO30 select to debug mode */
    FREQ_METER_GPIO_USED->GPIO_DIR0_SET = 0x40000000;       /* DIR = output mode */

    tmp = MIXEDSYSD->ABIST_FQMTR_CON1;
    tmp = MIXEDSYSD->ABIST_FQMTR_CON1;
    while (tmp & (0x1 << 15)) {
        tmp = MIXEDSYSD->ABIST_FQMTR_CON1;
    }

    hal_gpt_delay_us(240);

    /* fqmtr_ck = fixed_ck*fqmtr_data/winset, */
    /* ex. 312*0.96=26*fqmtr_data/100, expect fqmtr_data=0x480 */
    target_freq = (26 * MIXEDSYSD->ABIST_FQMTR_DATA) / 4096;
    if (over78m)
        target_freq *= 4;

#ifdef CLK_DEBUG
    log_hal_info("[PLL] %s: ABIST_FQMTR_CON2=0x%x,CON1=0x%x,DATA=0x%x \n\r", __func__,
                 MIXEDSYSD->ABIST_FQMTR_CON2, MIXEDSYSD->ABIST_FQMTR_CON1, MIXEDSYSD->ABIST_FQMTR_DATA);
#endif

    FREQ_METER_GPIO_USED->GPIO_MODE3_CLR = 0x0F000000;          /* clear setting for gpio30 in mode3 */
    FREQ_METER_GPIO_USED->GPIO_DIR0_CLR = 0x40000000;           /* clear setting for gpio30 in dir0 */
    FREQ_METER_GPIO_USED->GPIO_MODE3_SET = bak_gpio30_mode3;    /* restore setting for gpio30 in mode3 */
    FREQ_METER_GPIO_USED->GPIO_DIR0_SET = bak_gpio30_dir0;      /* restore setting for gpio30 in dir0 */

    return target_freq;
}
#endif

ATTR_TEXT_IN_TCM uint32_t freq_monitor(uint16_t target, bool over78m)
{
    return clock_get_freq_meter(target, 0, over78m);
}


/* For DVT DVFS used */
ATTR_TEXT_IN_TCM void cm_bus_clk_208m_mpll_mpll(void)
{
    uint32_t irq_mask = save_and_set_interrupt_mask();  /* disable interrupt */

    if (CLK_USED_EFUSE->EFUSE_CPU_104M & 0x400) {
        restore_interrupt_mask(irq_mask);               /* restore interrupt */
        return;
    }

    if (clock_dfs_initialized == false) {
        CONFIGSYS->CLK_SOURCE_SEL |= 0x00333333;
        hal_gpt_delay_us(100);
        CONFIGSYS->CLK_CONDA |= ((0x1 << 16) | (0x1 << 18) | (0x1 << 21) | (0x1 << 22) | (0x1 << 24) | (0x1 << 25));
        hal_gpt_delay_us(2);
        CONFIGSYS->CLK_CONDB |= ((uint32_t) 0x1 << 31);
        CONFIGSYS->CLK_CONDB &= (~((0x7 << 28) | (0x7 << 17) | (0x7 << 14) | (0xF << 3) | (0x7 << 0)));
        CONFIGSYS->CLK_CONDB |= (((0x2 << 28) | (0x2 << 17) | (0x2 << 14) | (0x5 << 3) | (0x3 << 0)));
        CONFIGSYS->CLK_CONDD = 0x10000FFF;
        ANA_CFGSYS->ACFG_CLK_UPDATE = 0x073;
        clock_change_bit_verified_all();
        hal_gpt_delay_us(2);
        CONFIGSYS->CLK_CONDD = 0x10000000;
        CONFIGSYS->CLK_CONDA = (0xB00 | (0x1 << 16) | (0x1 << 18) | (0x1 << 21) | (0x1 << 22) | (0x1 << 25));
        CONFIGSYS->CLK_SOURCE_SEL = 0x00333333;
    } else {
        clock_mux_sel(DSP_MUX_SEL, 2);
        clock_mux_sel(BSI_MUX_SEL, 2);
        clock_mux_sel(SLCD_MUX_SEL, 2);
        clock_mux_sel(CM_MUX_SEL, 5);
        clock_mux_sel(BUS_MUX_SEL, 3);
        CONFIGSYS->CLK_SOURCE_SEL = 0x00333333;
    }

    /* BUS clock from 31.2M to 62.4M with DCM */
    /* disable clock slow down when idle */
    /* ANA CFGSYS_BASE (0XA21D0000) + 0x0154, set rg_pllck_sel = 1'b0 */
    ANA_CFGSYS->ACFG_FREQ_SWCH = 0x0000;

    /* CONFG_BASE (0XA2010000)+ 0x011C, bit[4:0]=5'd10, Bus free dcm active clock /1 */
    CONFIGSYS->CLK_CONDH = 0x10;

    /* CONFG_BASE (0XA2010000)+ 0x0114, PD_BUS DCM, idle clock 62.4M/32 */
    CONFIGSYS->CLK_CONDF = (CONFIGSYS->CLK_CONDF & ~0xFC00) | 0x00000603;

    /* CONFG_BASE (0XA2010000)+ 0x0118, AO_BUS DCM, idle clock 62.4M/32 */
    CONFIGSYS->CLK_CONDG = (CONFIGSYS->CLK_CONDG & ~0xFC00) | 0x00000603;

    /* CM_MEMS_DCM_BASE (0XA20A0000) + 0x0150, CM4 DCM, idle clock 104M/64 */
    CM_MEMS_DCM->CM4_DCM_CON = (CM_MEMS_DCM->CM4_DCM_CON & ~0xFC00) | 0x0000;

    /* CM_MEMS_DCM_BASE (0XA20A0000) + 0x0154, MEMS DCM, idle clock 52M/32 */
    CM_MEMS_DCM->MEMS_DCM_CON = (CM_MEMS_DCM->MEMS_DCM_CON & ~0xFC00) | 0x400;

    /* enable clock slow down when idle */
    /* ANA CFGSYS_BASE (0XA21D0000) + 0x0154, set rg_pllck_sel = 1'b1, bus active clock change to 62.4M */
    ANA_CFGSYS->ACFG_FREQ_SWCH = 0x0001;

    /* change clock freq state to 208m */
    cm_freq_state = CM_208M_STA;

    if (clock_dfs_initialized == false) {
        /* reset clock api related counting value */
        clock_api_internal_reset();

        /* re-init clock api related value */
        clock_api_internal_init();

        clock_dfs_initialized = true;
    }

    restore_interrupt_mask(irq_mask);                   /* restore interrupt */
}

ATTR_TEXT_IN_TCM void cm_bus_clk_104m_mpll_mpll(void)
{
    uint32_t irq_mask = save_and_set_interrupt_mask();  /* disable interrupt */

    if (clock_dfs_initialized == false) {
        CONFIGSYS->CLK_SOURCE_SEL |= 0x00333333;
        hal_gpt_delay_us(100);
        CONFIGSYS->CLK_CONDA |= ((0x1 << 16) | (0x1 << 20) | (0x1 << 22) | (0x1 << 23) | (0x1 << 24) | (0x1 << 25));
        hal_gpt_delay_us(2);
        CONFIGSYS->CLK_CONDB |= ((uint32_t) 0x1 << 31);
        CONFIGSYS->CLK_CONDB &= (~((0x7 << 28) | (0x7 << 17) | (0x7 << 14) | (0xF << 3) | (0x7 << 0)));
        CONFIGSYS->CLK_CONDB |= (((0x4 << 28) | (0x2 << 17) | (0x2 << 14) | (0x3 << 3) | (0x3 << 0)));
        CONFIGSYS->CLK_CONDD = 0x10000FFF;
        ANA_CFGSYS->ACFG_CLK_UPDATE = 0x073;
        clock_change_bit_verified_all();
        hal_gpt_delay_us(2);
        CONFIGSYS->CLK_CONDD = 0x10000000;
        CONFIGSYS->CLK_CONDA = (0xB00 | (0x1 << 16) | (0x1 << 20) | (0x1 << 22) | (0x1 << 23) | (0x1 << 24) | (0x1 << 25));
        CONFIGSYS->CLK_SOURCE_SEL = 0x00333333;
    } else {
        clock_mux_sel(DSP_MUX_SEL, 4);
        clock_mux_sel(BSI_MUX_SEL, 2);
        clock_mux_sel(SLCD_MUX_SEL, 2);
        clock_mux_sel(CM_MUX_SEL, 3);
        clock_mux_sel(BUS_MUX_SEL, 3);
        CONFIGSYS->CLK_SOURCE_SEL = 0x00333333;
    }

    /* BUS clock from 31.2M to 62.4M with DCM */
    /* disable clock slow down when idle */
    /* ANA CFGSYS_BASE (0XA21D0000) + 0x0154, set rg_pllck_sel = 1'b0 */
    ANA_CFGSYS->ACFG_FREQ_SWCH = 0x0000;

    /* CONFG_BASE (0XA2010000)+ 0x011C, bit[4:0]=5'd10, Bus free dcm active clock /1 */
    CONFIGSYS->CLK_CONDH = 0x10;

    /* CONFG_BASE (0XA2010000)+ 0x0114, PD_BUS DCM, idle clock 62.4M/32 */
    CONFIGSYS->CLK_CONDF = (CONFIGSYS->CLK_CONDF & ~0xFC00) | 0x00000603;

    /* CONFG_BASE (0XA2010000)+ 0x0118, AO_BUS DCM, idle clock 62.4M/32 */
    CONFIGSYS->CLK_CONDG = (CONFIGSYS->CLK_CONDG & ~0xFC00) | 0x00000603;

    /* CM_MEMS_DCM_BASE (0XA20A0000) + 0x0150, CM4 DCM, idle clock 104M/64 */
    CM_MEMS_DCM->CM4_DCM_CON = (CM_MEMS_DCM->CM4_DCM_CON & ~0xFC00) | 0x0000;

    /* CM_MEMS_DCM_BASE (0XA20A0000) + 0x0154, MEMS DCM, idle clock 52M/32 */
    CM_MEMS_DCM->MEMS_DCM_CON = (CM_MEMS_DCM->MEMS_DCM_CON & ~0xFC00) | 0x400;

    /* enable clock slow down when idle */
    /* ANA CFGSYS_BASE (0XA21D0000) + 0x0154, set rg_pllck_sel = 1'b1, bus active clock change to 62.4M */
    ANA_CFGSYS->ACFG_FREQ_SWCH = 0x0001;

    /* change clock freq state to 104m */
    cm_freq_state = CM_104M_STA;

    if (clock_dfs_initialized == false) {
        /* reset clock api related counting value */
        clock_api_internal_reset();

        /* re-init clock api related value */
        clock_api_internal_init();

        clock_dfs_initialized = true;
    }

    restore_interrupt_mask(irq_mask);                   /* restore interrupt */
}

ATTR_TEXT_IN_TCM void cm_bus_clk_208m_mpll_hfosc(void)
{
    uint32_t irq_mask = save_and_set_interrupt_mask();  /* disable interrupt */

    if (CLK_USED_EFUSE->EFUSE_CPU_104M & 0x400) {
        restore_interrupt_mask(irq_mask);               /* restore interrupt */
        return;
    }

    if (clock_dfs_initialized == false) {
        CONFIGSYS->CLK_SOURCE_SEL |= 0x00399988;
        hal_gpt_delay_us(100);
        CONFIGSYS->CLK_CONDA |= ((0x1 << 18) | (0x1 << 5) | (0x1 << 4) | (0x1 << 2) | (0x1 << 1));
        hal_gpt_delay_us(2);
        CONFIGSYS->CLK_CONDB |= ((uint32_t) 0x1 << 31);
        CONFIGSYS->CLK_CONDB &= (~((0x7 << 28) | (0x7 << 17) | (0x7 << 14) | (0xF << 3) | (0x7 << 0)));
        CONFIGSYS->CLK_CONDB |= ((0x6 << 28) | (0x4 << 17) | (0x5 << 14) | (0x5 << 3) | (0x5 << 0));
        CONFIGSYS->CLK_CONDD = 0x10000FFF;
        ANA_CFGSYS->ACFG_CLK_UPDATE = 0x073;
        clock_change_bit_verified_all();
        hal_gpt_delay_us(2);
        CONFIGSYS->CLK_CONDD = 0x10000000;
        CONFIGSYS->CLK_CONDA = (0xB00 | (0x1 << 18) | (0x1 << 5) | (0x1 << 4) | (0x1 << 2) | (0x1 << 1));
        CONFIGSYS->CLK_SOURCE_SEL = 0x00399988;
    } else {
        clock_mux_sel(DSP_MUX_SEL, 6);
        clock_mux_sel(BSI_MUX_SEL, 4);
        clock_mux_sel(SLCD_MUX_SEL, 5);
        clock_mux_sel(CM_MUX_SEL, 5);
        clock_mux_sel(BUS_MUX_SEL, 5);
        CONFIGSYS->CLK_SOURCE_SEL = 0x00399988;
    }

    /* BUS clock from 31.2M to 62.4M with DCM */
    /* disable clock slow down when idle */
    /* ANA CFGSYS_BASE (0XA21D0000) + 0x0154, set rg_pllck_sel = 1'b0 */
    ANA_CFGSYS->ACFG_FREQ_SWCH = 0x0000;

    /* CONFG_BASE (0XA2010000)+ 0x011C, bit[4:0]=5'd10, Bus free dcm active clock /1 */
    CONFIGSYS->CLK_CONDH = 0x10;

    /* CONFG_BASE (0XA2010000)+ 0x0114, PD_BUS DCM, idle clock 62.4M/32 */
    CONFIGSYS->CLK_CONDF = (CONFIGSYS->CLK_CONDF & ~0xFC00) | 0x00000603;

    /* CONFG_BASE (0XA2010000)+ 0x0118, AO_BUS DCM, idle clock 62.4M/32 */
    CONFIGSYS->CLK_CONDG = (CONFIGSYS->CLK_CONDG & ~0xFC00) | 0x00000603;

    /* CM_MEMS_DCM_BASE (0XA20A0000) + 0x0150, CM4 DCM, idle clock 104M/64 */
    CM_MEMS_DCM->CM4_DCM_CON = (CM_MEMS_DCM->CM4_DCM_CON & ~0xFC00) | 0x0000;

    /* CM_MEMS_DCM_BASE (0XA20A0000) + 0x0154, MEMS DCM, idle clock 52M/32 */
    CM_MEMS_DCM->MEMS_DCM_CON = (CM_MEMS_DCM->MEMS_DCM_CON & ~0xFC00) | 0x400;

    /* enable clock slow down when idle */
    /* ANA CFGSYS_BASE (0XA21D0000) + 0x0154, set rg_pllck_sel = 1'b1, bus active clock change to 62.4M */
    ANA_CFGSYS->ACFG_FREQ_SWCH = 0x0001;

    /* change clock freq state to 208m */
    cm_freq_state = CM_208M_STA;

    if (clock_dfs_initialized == false) {
        /* reset clock api related counting value */
        clock_api_internal_reset();

        /* re-init clock api related value */
        clock_api_internal_init();

        clock_dfs_initialized = true;
    }

    restore_interrupt_mask(irq_mask);                   /* restore interrupt */
}

ATTR_TEXT_IN_TCM void cm_bus_clk_104m_hfosc_hfosc(void)
{
    uint32_t irq_mask = save_and_set_interrupt_mask();  /* disable interrupt */

    if (clock_dfs_initialized == false) {
        if (is_clk_use_lfosc())
            CONFIGSYS->CLK_SOURCE_SEL |= 0x00888888;
        else
            CONFIGSYS->CLK_SOURCE_SEL |= 0x00899988;
        hal_gpt_delay_us(100);
        CONFIGSYS->CLK_CONDA |= ((0x1 << 5) | (0x1 << 4) | (0x1 << 3) | (0x1 << 2) | (0x1 << 1) | (0x1 << 0));
        hal_gpt_delay_us(2);
        CONFIGSYS->CLK_CONDB |= ((uint32_t) 0x1 << 31);
        CONFIGSYS->CLK_CONDB &= (~((0x7 << 28) | (0x7 << 17) | (0x7 << 14) | (0xF << 3) | (0x7 << 0)));
        CONFIGSYS->CLK_CONDB |= ((0x7 << 28) | (0x4 << 17) | (0x5 << 14) | (0x4 << 3) | (0x5 << 0));
        CONFIGSYS->CLK_CONDD = 0x10000FFF;
        ANA_CFGSYS->ACFG_CLK_UPDATE = 0x073;
        clock_change_bit_verified_all();
        hal_gpt_delay_us(2);
        CONFIGSYS->CLK_CONDD = 0x10000000;
        CONFIGSYS->CLK_CONDA = (0xB00 | (0x1 << 5) | (0x1 << 4) | (0x1 << 3) | (0x1 << 2) | (0x1 << 1) | (0x1 << 0));
        if (is_clk_use_lfosc())
            CONFIGSYS->CLK_SOURCE_SEL = 0x00888888;
        else
            CONFIGSYS->CLK_SOURCE_SEL = 0x00899988;
    } else {
        clock_mux_sel(DSP_MUX_SEL, 7);
        clock_mux_sel(BSI_MUX_SEL, 4);
        clock_mux_sel(SLCD_MUX_SEL, 5);
        clock_mux_sel(CM_MUX_SEL, 4);
        clock_mux_sel(BUS_MUX_SEL, 5);
        if (is_clk_use_lfosc())
            CONFIGSYS->CLK_SOURCE_SEL = 0x00888888;
        else
            CONFIGSYS->CLK_SOURCE_SEL = 0x00899988;
    }

    /* BUS clock from 31.2M to 62.4M with DCM */
    /* disable clock slow down when idle */
    /* ANA CFGSYS_BASE (0XA21D0000) + 0x0154, set rg_pllck_sel = 1'b0 */
    ANA_CFGSYS->ACFG_FREQ_SWCH = 0x0000;

    /* CONFG_BASE (0XA2010000)+ 0x011C, bit[4:0]=5'd10, Bus free dcm active clock /1 */
    CONFIGSYS->CLK_CONDH = 0x10;

    /* CONFG_BASE (0XA2010000)+ 0x0114, PD_BUS DCM, idle clock 62.4M/32 */
    CONFIGSYS->CLK_CONDF = (CONFIGSYS->CLK_CONDF & ~0xFC00) | 0x00000603;

    /* CONFG_BASE (0XA2010000)+ 0x0118, AO_BUS DCM, idle clock 62.4M/32 */
    CONFIGSYS->CLK_CONDG = (CONFIGSYS->CLK_CONDG & ~0xFC00) | 0x00000603;

    /* CM_MEMS_DCM_BASE (0XA20A0000) + 0x0150, CM4 DCM, idle clock 104M/64 */
    CM_MEMS_DCM->CM4_DCM_CON = (CM_MEMS_DCM->CM4_DCM_CON & ~0xFC00) | 0x0000;

    /* CM_MEMS_DCM_BASE (0XA20A0000) + 0x0154, MEMS DCM, idle clock 52M/32 */
    CM_MEMS_DCM->MEMS_DCM_CON = (CM_MEMS_DCM->MEMS_DCM_CON & ~0xFC00) | 0x400;

    /* enable clock slow down when idle */
    /* ANA CFGSYS_BASE (0XA21D0000) + 0x0154, set rg_pllck_sel = 1'b1, bus active clock change to 62.4M */
    ANA_CFGSYS->ACFG_FREQ_SWCH = 0x0001;

    /* change clock freq state to 104m */
    cm_freq_state = CM_104M_STA;

    if (clock_dfs_initialized == false) {
        /* reset clock api related counting value */
        clock_api_internal_reset();

        /* re-init clock api related value */
        clock_api_internal_init();

        clock_dfs_initialized = true;
    }

    restore_interrupt_mask(irq_mask);                   /* restore interrupt */
}

ATTR_TEXT_IN_TCM void cm_bus_clk_26m_lfosc_lfosc(void)
{
    uint32_t irq_mask = save_and_set_interrupt_mask();  /* disable interrupt */

    if (clock_dfs_initialized == false) {
        CONFIGSYS->CLK_SOURCE_SEL |= 0x00444444;
        hal_gpt_delay_us(5);
        CONFIGSYS->CLK_CONDA |= 0x0;
        hal_gpt_delay_us(2);
        CONFIGSYS->CLK_CONDB = 0x97B24489;
        CLK_USED_MSDC0->MSDC_CFG &= (~((0x1 << 23) | (0x3 << 3)));
        CLK_USED_MSDC0->MSDC_CFG |= ((0x1 << 3));
        CLK_USED_MSDC1->MSDC_CFG &= (~((0x1 << 23) | (0x3 << 3)));
        CLK_USED_MSDC1->MSDC_CFG |= ((0x1 << 3));
        CONFIGSYS->CLK_CONDD = 0x10000FFF;
        ANA_CFGSYS->ACFG_CLK_UPDATE = 0x0E7F;
        clock_change_bit_verified_all();
        hal_gpt_delay_us(2);
        CONFIGSYS->CLK_CONDD = 0x10000000;
        CONFIGSYS->CLK_CONDA = (0xB00 | 0x0);
        CONFIGSYS->CLK_CONDB &= (~(0x1 << 31));
        CONFIGSYS->CLK_SOURCE_SEL = 0x00444444;
    } else {
        clock_mux_sel(DSP_MUX_SEL, 1);
        clock_mux_sel(DISP_PWM_MUX_SEL, 1);
        clock_mux_sel(USB_MUX_SEL, 3);
        clock_mux_sel(CAM_MUX_SEL, 2);
        clock_mux_sel(GP_F26M_GFMUX_SEL, 1);
        clock_mux_sel(LP_F26M_GFMUX_SEL, 1);
        clock_mux_sel(BSI_MUX_SEL, 1);
        clock_mux_sel(SLCD_MUX_SEL, 1);
        clock_mux_sel(SFC_MUX_SEL, 1);
        clock_mux_sel(CM_MUX_SEL, 1);
        clock_mux_sel(BUS_MUX_SEL, 1);
        clock_mux_sel(MSDC0_MUX_SEL, 1);
        clock_mux_sel(MSDC1_MUX_SEL, 1);
        CONFIGSYS->CLK_SOURCE_SEL = 0x00444444;
    }
    /* disable clock slow down when idle */
    /* ANA CFGSYS_BASE (0XA21D0000) + 0x0154, set rg_pllck_sel = 1'b0 */
    ANA_CFGSYS->ACFG_FREQ_SWCH = 0x0000;

    /* CONFG_BASE (0XA2010000)+ 0x011C, bit[4:0]=5'd8, Bus free dcm active clock /2 */
    CONFIGSYS->CLK_CONDH = 0x8;

    /* CONFG_BASE (0XA2010000)+ 0x0114, PD_BUS DCM, idle clock 26M/16 */
    CONFIGSYS->CLK_CONDF = (CONFIGSYS->CLK_CONDF & ~0xFC00) | 0x00000A03;

    /* CONFG_BASE (0XA2010000)+ 0x0118, AO_BUS DCM, idle clock 26M/16 */
    CONFIGSYS->CLK_CONDG = (CONFIGSYS->CLK_CONDG & ~0xFC00) | 0x00000A03;

    /* CM_MEMS_DCM_BASE (0XA20A0000) + 0x0150, CM4 DCM, idle clock 26M/16 */
    CM_MEMS_DCM->CM4_DCM_CON = (CM_MEMS_DCM->CM4_DCM_CON & ~0xFC00) | 0x800;

    /* CM_MEMS_DCM_BASE (0XA20A0000) + 0x0154, MEMS DCM, idle clock 13M/8 */
    CM_MEMS_DCM->MEMS_DCM_CON = (CM_MEMS_DCM->MEMS_DCM_CON & ~0xFC00) | 0x1000;

    /* enable clock slow down when idle */
    /* ANA CFGSYS_BASE (0XA21D0000) + 0x0154, set rg_pllck_sel = 1'b1, bus active clock change to 13M */
    ANA_CFGSYS->ACFG_FREQ_SWCH = 0x0001;

    /* change clock freq state to 26m */
    cm_freq_state = CM_26M_STA;

    if (clock_dfs_initialized == false) {
        /* reset clock api related counting value */
        clock_api_internal_reset();

        /* re-init clock api related value */
        clock_api_internal_init();

        clock_dfs_initialized = true;
    }

    restore_interrupt_mask(irq_mask);                   /* restore interrupt */
}

ATTR_TEXT_IN_TCM void peri_26m_lfosc(void)
{
    uint32_t irq_mask = save_and_set_interrupt_mask();  /* disable interrupt */
#if CLK_DFS_USE_HARD_CODE
    CONFIGSYS->CLK_SOURCE_SEL |= 0x00044400;
    hal_gpt_delay_us(5);
    CONFIGSYS->CLK_CONDB &= (~(0x3 << 26));
    CONFIGSYS->CLK_CONDB |= ((0x1 << 26) | (0x1 << 21) | (0x1 << 20));
    CLK_USED_MSDC0->MSDC_CFG &= (~((0x1 << 23) | (0x3 << 3)));
    CLK_USED_MSDC0->MSDC_CFG |= ((0x1 << 3));
    CLK_USED_MSDC1->MSDC_CFG &= (~((0x1 << 23) | (0x3 << 3)));
    CLK_USED_MSDC1->MSDC_CFG |= ((0x1 << 3));
    CONFIGSYS->CLK_CONDD = 0x10000FFF;
    ANA_CFGSYS->ACFG_CLK_UPDATE = 0x0E00;
    clock_change_bit_verified_all();
    hal_gpt_delay_us(2);
    CONFIGSYS->CLK_CONDD = 0x10000000;
    CONFIGSYS->CLK_SOURCE_SEL &= (~((0x1 << 16) | (0x1 << 12) | (0x1 << 8)));
#else
    clock_mux_sel(DISP_PWM_MUX_SEL, 1);
    clock_mux_sel(GP_F26M_GFMUX_SEL, 1);
    clock_mux_sel(LP_F26M_GFMUX_SEL, 1);
    clock_mux_sel(MSDC0_MUX_SEL, 1);
    clock_mux_sel(MSDC1_MUX_SEL, 1);
    clock_source_clear(AO_BUSCLK_SOURCE_SEL, CLK_SRC_DCXO);
    clock_source_clear(PDN_BUSCLK_SOURCE_SEL, CLK_SRC_DCXO);
    clock_source_clear(SFCCLK_SOURCE_SEL, CLK_SRC_DCXO);
#endif
    restore_interrupt_mask(irq_mask);                   /* restore interrupt */
}

ATTR_TEXT_IN_TCM void cm_bus_clk_26m_dcxo_dcxo(void)
{
    uint32_t irq_mask = save_and_set_interrupt_mask();  /* disable interrupt */

    if (clock_dfs_initialized == false) {
        (CONFIGSYS)->CLK_SOURCE_SEL |= 0x00111111;
        hal_gpt_delay_ms(4);
        CONFIGSYS->CLK_CONDA |= 0x0;
        hal_gpt_delay_us(2);
        CONFIGSYS->CLK_CONDB = 0x83800000;
        CLK_USED_MSDC0->MSDC_CFG &= (~((0x1 << 23) | (0x3 << 3)));
        CLK_USED_MSDC1->MSDC_CFG &= (~((0x1 << 23) | (0x3 << 3)));
        CONFIGSYS->CLK_CONDD = 0x10000FFF;
        ANA_CFGSYS->ACFG_CLK_UPDATE = 0x0E7F;
        clock_change_bit_verified_all();
        hal_gpt_delay_us(2);
        CONFIGSYS->CLK_CONDD = 0x10000000;
        CONFIGSYS->CLK_CONDA = (0xB00 | 0x0);
        CONFIGSYS->CLK_CONDB &= (~((uint32_t) 0x1 << 31));
        CONFIGSYS->CLK_SOURCE_SEL = 0x00111111;
    } else {
        clock_mux_sel(DSP_MUX_SEL, 0);
        clock_mux_sel(DISP_PWM_MUX_SEL, 0);
        clock_mux_sel(USB_MUX_SEL, 3);
        clock_mux_sel(CAM_MUX_SEL, 2);
        clock_mux_sel(GP_F26M_GFMUX_SEL, 0);
        clock_mux_sel(LP_F26M_GFMUX_SEL, 0);
        clock_mux_sel(BSI_MUX_SEL, 0);
        clock_mux_sel(SLCD_MUX_SEL, 0);
        clock_mux_sel(SFC_MUX_SEL, 0);
        clock_mux_sel(CM_MUX_SEL, 0);
        clock_mux_sel(BUS_MUX_SEL, 0);
        clock_mux_sel(MSDC0_MUX_SEL, 0);
        clock_mux_sel(MSDC1_MUX_SEL, 0);
        CONFIGSYS->CLK_SOURCE_SEL = 0x00111111;
    }

    /* disable clock slow down when idle */
    /* ANA CFGSYS_BASE (0XA21D0000) + 0x0154, set rg_pllck_sel = 1'b0 */
    ANA_CFGSYS->ACFG_FREQ_SWCH = 0x0000;

    /* CONFG_BASE (0XA2010000)+ 0x011C, bit[4:0]=5'd8, Bus free dcm active clock /2 */
    CONFIGSYS->CLK_CONDH = 0x8;

    /* CONFG_BASE (0XA2010000)+ 0x0114, PD_BUS DCM, idle clock 26M/16 */
    CONFIGSYS->CLK_CONDF = (CONFIGSYS->CLK_CONDF & ~0xFC00) | 0x00000A03;

    /* CONFG_BASE (0XA2010000)+ 0x0118, AO_BUS DCM, idle clock 26M/16 */
    CONFIGSYS->CLK_CONDG = (CONFIGSYS->CLK_CONDG & ~0xFC00) | 0x00000A03;

    /* CM_MEMS_DCM_BASE (0XA20A0000) + 0x0150, CM4 DCM, idle clock 26M/16 */
    CM_MEMS_DCM->CM4_DCM_CON = (CM_MEMS_DCM->CM4_DCM_CON & ~0xFC00) | 0x800;

    /* CM_MEMS_DCM_BASE (0XA20A0000) + 0x0154, MEMS DCM, idle clock 13M/8 */
    CM_MEMS_DCM->MEMS_DCM_CON = (CM_MEMS_DCM->MEMS_DCM_CON & ~0xFC00) | 0x1000;

    /* enable clock slow down when idle */
    /* ANA CFGSYS_BASE (0XA21D0000) + 0x0154, set rg_pllck_sel = 1'b1, bus active clock change to 13M */
    ANA_CFGSYS->ACFG_FREQ_SWCH = 0x0001;

    /* change clock freq state to 26m */
    cm_freq_state = CM_26M_STA;

    if (clock_dfs_initialized == false) {
        /* reset clock api related counting value */
        clock_api_internal_reset();

        /* re-init clock api related value */
        clock_api_internal_init();

        clock_dfs_initialized = true;
    }

    restore_interrupt_mask(irq_mask);                   /* restore interrupt */
}

ATTR_TEXT_IN_TCM void peri_26m_dcxo(void)
{
    uint32_t irq_mask = save_and_set_interrupt_mask();  /* disable interrupt */
#if CLK_DFS_USE_HARD_CODE
    CONFIGSYS->CLK_SOURCE_SEL |= 0x00011100;
    hal_gpt_delay_ms(4);
    CONFIGSYS->CLK_CONDB &= (~((0x3 << 26) | (0x1 << 21) | (0x1 << 20)));
    CLK_USED_MSDC0->MSDC_CFG &= (~((0x1 << 23) | (0x3 << 3)));
    CLK_USED_MSDC1->MSDC_CFG &= (~((0x1 << 23) | (0x3 << 3)));
    CONFIGSYS->CLK_CONDD = 0x10000FFF;
    ANA_CFGSYS->ACFG_CLK_UPDATE = 0x0E00;
    clock_change_bit_verified_all();
    hal_gpt_delay_us(2);
    CONFIGSYS->CLK_CONDD = 0x10000000;
    CONFIGSYS->CLK_SOURCE_SEL &= (~((0x1 << 18) | (0x1 << 14) | (0x1 << 10)));
#else
    clock_mux_sel(DISP_PWM_MUX_SEL, 0);
    clock_mux_sel(GP_F26M_GFMUX_SEL, 0);
    clock_mux_sel(LP_F26M_GFMUX_SEL, 0);
    clock_mux_sel(MSDC0_MUX_SEL, 0);
    clock_mux_sel(MSDC1_MUX_SEL, 0);
    clock_source_clear(AO_BUSCLK_SOURCE_SEL, CLK_SRC_LFOSC);
    clock_source_clear(PDN_BUSCLK_SOURCE_SEL, CLK_SRC_LFOSC);
    clock_source_clear(SFCCLK_SOURCE_SEL, CLK_SRC_LFOSC);
#endif
    restore_interrupt_mask(irq_mask);                   /* restore interrupt */
}

/*************************************************************************
 * Suspend/Resume struct definition
 *************************************************************************/
struct clock_suspend_resume_ops {
    hal_clock_status_t (*suspend)(bool is_sleep_with_26m);
    hal_clock_status_t (*resume)(void);
};

struct clock_suspend_resume {
    struct clock_suspend_resume_ops *ops;
    clock_cm_freq_state_id cm_freq_saved;
    bool sleep_with_26m;
};

ATTR_RWDATA_IN_TCM static struct clock_suspend_resume_ops clk_suspend_resume_ops;
ATTR_RWDATA_IN_TCM static struct clock_suspend_resume clk_suspend_resume;

/*************************************************************************
 * Suspend/Resume internal function definition
 *************************************************************************/
ATTR_TEXT_IN_TCM static hal_clock_status_t general_clock_suspend_op(bool is_sleep_with_26m)
{
    uint32_t irq_mask = save_and_set_interrupt_mask();          /* disable interrupt */

    clk_suspend_resume.cm_freq_saved = cm_freq_state;           /* save cm_freq_saved before suspend */
    pll_clock[SYSCLK].ops->save(&pll_clock[SYSCLK]);            /* save and force release pll request */
    pll_clock[UPLLCLK].ops->save(&pll_clock[UPLLCLK]);
    pll_clock[MPLLCLK].ops->save(&pll_clock[MPLLCLK]);
    if (is_sleep_with_26m) {
        clk_suspend_resume.sleep_with_26m = true;               /* set sleep_with_26m flag */
        cm_bus_clk_26m();                                       /* switch to 26m */
        if (cm_freq_state != CM_26M_STA) {
            EMI_DynamicClockSwitch(EMI_CLK_HIGH_TO_LOW);
            /* Set flash timing delay to 26MHz, switch SFC PLL to 26MHz DCXO or LFOSC */
            SF_DAL_DEV_SWITCH_TO_LOW_FQ();
        }
    }

    restore_interrupt_mask(irq_mask);                           /* restore interrupt */
    return HAL_CLOCK_STATUS_OK;
}

ATTR_TEXT_IN_TCM static hal_clock_status_t general_clock_resume_op(void)
{
    uint32_t irq_mask = save_and_set_interrupt_mask();          /* disable interrupt */

    if (clk_suspend_resume.sleep_with_26m) {
        if (clk_suspend_resume.cm_freq_saved == CM_208M_STA) {  /* check cm_freq_saved when resume */
            cm_bus_clk_208m();
            EMI_DynamicClockSwitch(EMI_CLK_LOW_TO_HIGH);
            SF_DAL_DEV_SWITCH_TO_HIGH_FQ();
        } else if (clk_suspend_resume.cm_freq_saved == CM_104M_STA) {
            cm_bus_clk_104m();
            EMI_DynamicClockSwitch(EMI_CLK_LOW_TO_HIGH);
            SF_DAL_DEV_SWITCH_TO_HIGH_FQ();
        } else {
            cm_bus_clk_26m();   /* CM_26M_STA */
        }
        clk_suspend_resume.sleep_with_26m = false;              /* clear sleep_with_26m flag */
    }

    pll_clock[SYSCLK].ops->restore(&pll_clock[SYSCLK]);         /* restore the pll request */
    pll_clock[UPLLCLK].ops->restore(&pll_clock[UPLLCLK]);
    pll_clock[MPLLCLK].ops->restore(&pll_clock[MPLLCLK]);
    hal_gpt_delay_us(100);

    restore_interrupt_mask(irq_mask);                           /* restore interrupt */
    return HAL_CLOCK_STATUS_OK;
}

ATTR_RWDATA_IN_TCM static struct clock_suspend_resume_ops clk_suspend_resume_ops = {
    /*.suspend = */general_clock_suspend_op,
    /*.resume = */general_clock_resume_op,
};

ATTR_RWDATA_IN_TCM static struct clock_suspend_resume clk_suspend_resume = {
    /*.ops = */&clk_suspend_resume_ops,
    /*.cm_freq_saved = */CM_208M_STA,
    /*.sleep_with_26m = */false,
};

/*************************************************************************
 * Suspend/Resume API definition
 *************************************************************************/
ATTR_TEXT_IN_TCM hal_clock_status_t clock_suspend(bool is_sleep_with_26m)
{
    if (!clock_initialized) {
        /* TODO cannot use log_hal_info print log before syslog init */
        log_hal_error("%s: clock is uninitialized\n", __FUNCTION__);
        return HAL_CLOCK_STATUS_UNINITIALIZED;
    }

    return clk_suspend_resume.ops->suspend(is_sleep_with_26m);
}

ATTR_TEXT_IN_TCM hal_clock_status_t clock_resume(void)
{
    if (!clock_initialized) {
        /* TODO cannot use log_hal_info print log before syslog init */
        log_hal_error("%s: clock is uninitialized\n", __FUNCTION__);
        return HAL_CLOCK_STATUS_UNINITIALIZED;
    }

    return clk_suspend_resume.ops->resume();
}

#if 0
ATTR_TEXT_IN_TCM void lpm_testing(uint16_t RG_SRC_SEL, uint16_t RG_32K_CK_SEL, uint16_t RG_GOOD_DURATION_CRITERIA)
{
    CONFIGSYS->PDN_CLRD2 = 0x80;
    CONFIGSYS->LPM_CON &= (0xffffffff ^ 0xffff01f6);
    CONFIGSYS->LPM_CON |= (RG_SRC_SEL << 4);
    hal_gpt_delay_us(1);
    CONFIGSYS->LPM_CON &= ~(0x1 << 0);
    CONFIGSYS->LPM_CON |= ((0x1 << 0) | (RG_32K_CK_SEL << 1) | RG_GOOD_DURATION_CRITERIA << 16);
    CONFIGSYS->LPM_CON |= (0x1 << 2);
}
#endif

#ifdef CLK_TOP_CLK_DVT
#define CM4_TOPSM_SYSTEM_CONTROL                ((volatile uint32_t*)0xE000ED10)
ATTR_TEXT_IN_TCM void clock_test_idle(void)
{
    *(volatile uint32_t *)(GPIO_BASE + 0xC40) =
        (*(volatile uint32_t *)(GPIO_BASE + 0xC40) & ~(0xF000)) | 0x6000;

    *(volatile uint32_t *)(GPIO_BASE + 0xE20) = 0xB;

    uint32_t irq_mask = save_and_set_interrupt_mask();  /* disable interrupt */

    *CM4_TOPSM_SYSTEM_CONTROL = *CM4_TOPSM_SYSTEM_CONTROL | 0x4;
    __asm volatile("dsb");
    __asm volatile("wfi");
    __asm volatile("isb");

    restore_interrupt_mask(irq_mask);     /* restore interrupt */
}

ATTR_TEXT_IN_TCM void clock_test_efuse_104m(void)
{
    log_hal_info("%s: S\n", __FUNCTION__);

    MIXEDSYSD->MPLL_CON0 = 0x3041;
    MIXEDSYSD->MPLL_CON1 = 0x40;
    MIXEDSYSD->MPLL_CON2 = 0x104;
    MIXEDSYSD->UPLL_CON0 = 0x3041;
    MIXEDSYSD->UPLL_CON1 = 0x0;
    MIXEDSYSD->UPLL_CON2 = 0x105;

    hal_gpt_delay_us(20);

    log_hal_info("%s: cm4 freq=%d\n", __FUNCTION__, freq_monitor(19, true));

    clock_mux_sel(CM_MUX_SEL, 0); // CM4 26M
    log_hal_info("%s: cm4 freq=%d, cm_mux_sel=0\n", __FUNCTION__, freq_monitor(19, true));

    clock_mux_sel(CM_MUX_SEL, 1);
    log_hal_info("%s: cm4 freq=%d, cm_mux_sel=1\n", __FUNCTION__, freq_monitor(19, true));

    clock_mux_sel(CM_MUX_SEL, 2);
    log_hal_info("%s: cm4 freq=%d, cm_mux_sel=2\n", __FUNCTION__, freq_monitor(19, true));

    clock_mux_sel(CM_MUX_SEL, 3);
    log_hal_info("%s: cm4 freq=%d, cm_mux_sel=3\n", __FUNCTION__, freq_monitor(19, true));

    clock_mux_sel(CM_MUX_SEL, 4);
    log_hal_info("%s: cm4 freq=%d, cm_mux_sel=4\n", __FUNCTION__, freq_monitor(19, true));

    clock_mux_sel(CM_MUX_SEL, 5);
    log_hal_info("%s: cm4 freq=%d, cm_mux_sel=5\n", __FUNCTION__, freq_monitor(19, true));

    clock_mux_sel(CM_MUX_SEL, 6);
    log_hal_info("%s: cm4 freq=%d, cm_mux_sel=6\n", __FUNCTION__, freq_monitor(19, true));

    clock_mux_sel(CM_MUX_SEL, 7);
    log_hal_info("%s: cm4 freq=%d, cm_mux_sel=7\n", __FUNCTION__, freq_monitor(19, true));

    clock_mux_sel(CM_MUX_SEL, 8);
    log_hal_info("%s: cm4 freq=%d, cm_mux_sel=8\n", __FUNCTION__, freq_monitor(19, true));
}
#endif

/* return unit: KHz */
uint32_t clock_get_freq_lfosc(void)
{
    return lfosc_freq;
}

/* return unit: Hz */
uint32_t clock_get_freq_peri_bus(void)
{
    uint8_t bus_mux_sel = (CONFIGSYS->CLK_CONDB & 0x7);
    uint8_t bus_free_fsel = (CONFIGSYS->CLK_CONDH & 0x1F);
    uint32_t bus_freq; /* Hz */

    switch (bus_mux_sel) {
        case 0:
        case 1:
        case 7:
            bus_freq = CLK_BUS_FREQ_26M;
            break;
        case 2:
        case 3:
        case 5:
            bus_freq = CLK_BUS_FREQ_62M;
            break;
        case 4:
        case 6:
            bus_freq = CLK_BUS_FREQ_52M;
            break;
        default:
            bus_freq = CLK_BUS_FREQ_26M;
            break;
    }

    switch (bus_free_fsel) {
        case 0:
            bus_freq /= 32;
            break;
        case 1:
            bus_freq /= 16;
            break;
        case 2:
            bus_freq /= 8;
            break;
        case 4:
            bus_freq /= 4;
            break;
        case 8:
            bus_freq /= 2;
            break;
        case 16:
        default:
            bus_freq /= 1;
            break;
    }

    return bus_freq;
}

/* LFOSC cali and ft setting offset for UART testing */
ATTR_TEXT_IN_TCM void lfosc_cali_ft_set_offset(int offset)
{
    uint32_t lfosc_cali, lfosc_ft;

    lfosc_cali = ((MIXEDSYSD->LFOSC_CON0 & LFOSC_CON0_RG_LFOSC_CALI_MASK) >> LFOSC_CON0_RG_LFOSC_CALI_OFFSET) + (offset / 16);
    lfosc_ft = ((MIXEDSYSD->LFOSC_CON0 & LFOSC_CON0_RG_LFOSC_FT_MASK) >> LFOSC_CON0_RG_LFOSC_FT_OFFSET) + (offset % 16);
    MIXEDSYSD->LFOSC_CON0 = (MIXEDSYSD->LFOSC_CON0 & LFOSC_CON0_RG_LFOSC_CALI_MASK) | (lfosc_cali << LFOSC_CON0_RG_LFOSC_CALI_OFFSET);
    MIXEDSYSD->LFOSC_CON0 = (MIXEDSYSD->LFOSC_CON0 & LFOSC_CON0_RG_LFOSC_FT_MASK) | (lfosc_ft << LFOSC_CON0_RG_LFOSC_FT_OFFSET);
}

ATTR_TEXT_IN_TCM void clock_dump_log(void)
{
    uint32_t mon_data0 = 0;

    /* fix IAR build warning: undefined behavior, the order of volatile accesses is undefined in this statement. */
    uint32_t reg_tmp0, reg_tmp1, reg_tmp2, reg_tmp3, reg_tmp4, reg_tmp5, reg_tmp6, reg_tmp7, reg_tmp8;

    reg_tmp0 = CONFIGSYS->PDN_COND0;
    reg_tmp1 = CONFIGSYS->PDN_COND1;
    reg_tmp2 = CONFIGSYS->PDN_COND2;
    reg_tmp3 = CM_MEMS_DCM->CM_PDN_COND0;
    reg_tmp4 = ANA_CFGSYS->ACFG_CLK_CG;

    /* clock_dump_cg */
    log_hal_info("%s: PDN_COND0=0x%x, PDN_COND1=0x%x, PDN_COND2=0x%x, CM_PDN_COND0=0x%x, ACFG_CLK_CG=0x%x\r\n",
                 __FUNCTION__, reg_tmp0, reg_tmp1, reg_tmp2, reg_tmp3, reg_tmp4);

    reg_tmp0 = CONFIGSYS->CLK_CONDA;
    reg_tmp1 = CONFIGSYS->CLK_CONDB;
    reg_tmp2 = CLK_USED_MSDC0->MSDC_CFG;
    reg_tmp3 = CLK_USED_MSDC1->MSDC_CFG;
    reg_tmp4 = CONFIGSYS->CLK_CONDC;
    reg_tmp5 = CONFIGSYS->CLK_CONDD;
    reg_tmp6 = CONFIGSYS->CLK_SOURCE_SEL;
    reg_tmp7 = ANA_CFGSYS->ACFG_CLK_UPDATE;

    /* clock_dump_mux */
    log_hal_info("%s: (divider)CLK_CONDA=0x%x, (mux)CLK_CONDB=0x%x, (mux)MSDC0_CFG=0x%x, (mux)MSDC1_CFG=0x%x, (pll_req)CLK_CONDC=0x%x, (force_on)CLK_CONDD=0x%x, (source)CLK_SOURCE_SEL=0x%x, (change_bit)ACFG_CLK_UPDATE=0x%x\r\n",
                 __FUNCTION__, reg_tmp0, reg_tmp1, reg_tmp2, reg_tmp3, reg_tmp4,
                 reg_tmp5, reg_tmp6, reg_tmp7);

    MIXEDSYSD->SYS_ABIST_MON_CON1 = 0x00001112;   /* HMON_SEL=DA_SRCLKENA, LMON_SEL=DA_UPLL_EN */
    mon_data0 = MIXEDSYSD->ABIST_MON_DATA0;
    log_hal_info("%s: DA_SRCLKENA=0x%x, DA_UPLL_EN=0x%x\r\n",
                 __FUNCTION__, (mon_data0 & 0x100) >> 8, (mon_data0 & 0x1));

    MIXEDSYSD->SYS_ABIST_MON_CON1 = 0x00001415;   /* HMON_SEL=RG_MDDS_EN, LMON_SEL=DA_MPLL_EN */
    mon_data0 = MIXEDSYSD->ABIST_MON_DATA0;
    log_hal_info("%s: RG_MDDS_EN=0x%x, DA_MPLL_EN=0x%x\r\n",
                 __FUNCTION__, (mon_data0 & 0x100) >> 8, (mon_data0 & 0x1));

    MIXEDSYSD->SYS_ABIST_MON_CON1 = 0x00001316;   /* HMON_SEL=DA_LFOSC_EN, LMON_SEL=DA_HFOSC_EN */
    mon_data0 = MIXEDSYSD->ABIST_MON_DATA0;
    log_hal_info("%s: DA_LFOSC_EN=0x%x, DA_HFOSC_EN=0x%x\r\n",
                 __FUNCTION__, (mon_data0 & 0x100) >> 8, (mon_data0 & 0x1));

    reg_tmp0 = ANA_CFGSYS->ACFG_TEST_CON1;

    ANA_CFGSYS->ACFG_TEST_CON0 = 0x12; /* acfg debug sel to test_in2, bit [3]=dcxo_f26m_ready */
    log_hal_info("%s: ACFG_TEST_CON1=0x%x, dcxo_f26m_ready=%d\r\n",
                 __FUNCTION__, reg_tmp0, (reg_tmp0 & 0x8) >> 3);

    reg_tmp0 = MIXEDSYSD->FH_CON0;
    reg_tmp1 = MIXEDSYSD->FH_CON1;
    reg_tmp2 = MIXEDSYSD->FH_CON4;
    reg_tmp3 = MIXEDSYSD->FH_CON5;
    reg_tmp4 = MIXEDSYSD->SSC_CON0;
    reg_tmp5 = MIXEDSYSD->SSC_CON1;
    reg_tmp6 = MIXEDSYSD->SSC_CON2;
    reg_tmp7 = MIXEDSYSD->SSC_CON3;
    reg_tmp8 = MIXEDSYSD->SSC_CON4;

    /* clock_dump_freq_hopping */
    log_hal_info("%s: FH_CON0=0x%x, FH_CON1=0x%x, FH_CON4=0x%x, FH_CON5=0x%x, SSC_CON0=0x%x, SSC_CON1=0x%x, SSC_CON2=0x%x, SSC_CON3=0x%x, SSC_CON4=0x%x\r\n",
                 __FUNCTION__, reg_tmp0, reg_tmp1, reg_tmp2, reg_tmp3, reg_tmp4,
                 reg_tmp5, reg_tmp6, reg_tmp7, reg_tmp8);
}

ATTR_TEXT_IN_TCM void clock_dump_info(void)
{
    uint32_t efuse_data = CLK_USED_EFUSE->EFUSE_CPU_104M;

    if (is_clk_use_lfosc())
        log_hal_info("%s: CLK_USE_LFOSC\n", __FUNCTION__);
    else
        log_hal_info("%s: CLK_USE_DCXO\n", __FUNCTION__);

    log_hal_info("%s: cm4 freq=%d, bus freq=%d, lfosc=%d, efuse_data=0x%x, peri_bus=%d\n",
		 __FUNCTION__, freq_monitor(19, true), freq_monitor(8, false),
		 lfosc_freq, efuse_data, clock_get_freq_peri_bus());

    clock_dump_log();

#ifdef CLK_DEBUG
    int i = 0;

    for (i = 0; i < NR_CLOCKS; i++) {
        if (i >= PDN_COND0_FROM && i <= PDN_COND0_TO) {
            log_hal_info("%s: clocks[%d] name=%s\n", __FUNCTION__, i, clocks[i].grp->name);
        } else if (i >= PDN_COND1_FROM && i <= PDN_COND1_TO) {
            log_hal_info("%s: clocks[%d] name=%s\n", __FUNCTION__, i, clocks[i].grp->name);
        } else if (i >= PDN_COND2_FROM && i <= PDN_COND2_TO) {
            log_hal_info("%s: clocks[%d] name=%s\n", __FUNCTION__, i, clocks[i].grp->name);
        } else if (i >= CM_PDN_COND0_FROM && i <= CM_PDN_COND0_TO) {
            log_hal_info("%s: clocks[%d] name=%s\n", __FUNCTION__, i, clocks[i].grp->name);
        } else if (i >= ACFG_PDN_RG_FROM && i <= ACFG_PDN_RG_TO) {
            log_hal_info("%s: clocks[%d] name=%s\n", __FUNCTION__, i, clocks[i].grp->name);
        }
    }
#endif

#if DVT_CG_TEST
    bool cg_stat;
    int clk_cg_id_test = HAL_CLOCK_CG_AUXADC;

    cg_stat = hal_clock_is_enabled(clk_cg_id_test);
    log_hal_info("%s: before testing CG %d cg_stat=%d\n",
                 __FUNCTION__, clk_cg_id_test, cg_stat);

    hal_clock_disable(clk_cg_id_test);
    cg_stat = hal_clock_is_enabled(clk_cg_id_test);
    log_hal_info("%s: after disable CG %d cg_stat=%d\n",
                 __FUNCTION__, clk_cg_id_test, cg_stat);

    hal_clock_enable(clk_cg_id_test);
    cg_stat = hal_clock_is_enabled(clk_cg_id_test);
    log_hal_info("%s: after enable CG %d cg_stat=%d\n",
                 __FUNCTION__, clk_cg_id_test, cg_stat);

    hal_clock_disable(clk_cg_id_test);
    cg_stat = hal_clock_is_enabled(clk_cg_id_test);
    log_hal_info("%s: after disable CG %d cg_stat=%d\n",
                 __FUNCTION__, clk_cg_id_test, cg_stat);

    hal_clock_enable(clk_cg_id_test);
    cg_stat = hal_clock_is_enabled(clk_cg_id_test);
    log_hal_info("%s: after enable CG %d cg_stat=%d\n",
                 __FUNCTION__, clk_cg_id_test, cg_stat);

    hal_clock_enable(clk_cg_id_test);
    cg_stat = hal_clock_is_enabled(clk_cg_id_test);
    log_hal_info("%s: after enable CG %d cg_stat=%d\n",
                 __FUNCTION__, clk_cg_id_test, cg_stat);

    hal_clock_force_disable(clk_cg_id_test);
    cg_stat = hal_clock_is_enabled(clk_cg_id_test);
    log_hal_info("%s: after force_disable CG %d cg_stat=%d\n",
                 __FUNCTION__, clk_cg_id_test, cg_stat);

    hal_clock_enable(clk_cg_id_test);
    cg_stat = hal_clock_is_enabled(clk_cg_id_test);
    log_hal_info("%s: after enable CG %d cg_stat=%d\n",
                 __FUNCTION__, clk_cg_id_test, cg_stat);
#endif

#ifdef CLK_TOP_CLK_DVT
    hal_clock_disable_upll();
    hal_clock_enable_upll();
    hal_clock_disable_mpll();
    hal_clock_enable_mpll();
#if DVT_GP_26M_CK_ENABLE
    DVT_GP_26M_CK(1, LFOSC_26M, CLOCK_LOWER_78M);
    DVT_GP_26M_CK(0, DCXO_26M, CLOCK_LOWER_78M);    /* original setting */
#endif
#if DVT_BUS_ENABLE
    DVT_BUS_CK(0, DCXO_26M, CLOCK_LOWER_78M);
    DVT_BUS_CK(1, LFOSC_26M, CLOCK_LOWER_78M);
    DVT_BUS_CK(3, MPLL_62P4M, CLOCK_LOWER_78M);
    DVT_BUS_CK(4, MPLL_52M, CLOCK_LOWER_78M);
    DVT_BUS_CK(5, HFOSC_62P4M, CLOCK_LOWER_78M);
    DVT_BUS_CK(6, HFOSC_52M, CLOCK_LOWER_78M);
    DVT_BUS_CK(2, UPLL_62P4M, CLOCK_LOWER_78M);     /* original setting */
#endif
#if DVT_LP_26M_CK_ENABLE
    DVT_LP_26M_CK(1, LFOSC_26M, CLOCK_LOWER_78M);
    DVT_LP_26M_CK(0, DCXO_26M, CLOCK_LOWER_78M);    /* original setting */
#endif
#if DVT_DISP_PWM_CK_ENABLE
    DVT_DISP_PWM_CK(0, DCXO_26M, CLOCK_LOWER_78M);
    DVT_DISP_PWM_CK(1, LFOSC_26M, CLOCK_LOWER_78M);
    DVT_DISP_PWM_CK(3, HFOSC_104M, CLOCK_OVER_78M);
    DVT_DISP_PWM_CK(2, UPLL_104M, CLOCK_OVER_78M);  /* original setting */
#endif
#if DVT_CAM_CK_ENABLE
    DVT_CAM_CK(0, DCXO_26M, CLOCK_LOWER_78M);
    DVT_CAM_CK(1, LFOSC_26M, CLOCK_LOWER_78M);
    DVT_CAM_CK(3, UPLL_312M, CLOCK_OVER_78M);
    DVT_CAM_CK(2, UPLL_48M, CLOCK_LOWER_78M);       /* original setting */
#endif
#if DVT_SFC_CK_ENABLE
    DVT_SFC_CK(0, DCXO_26M, CLOCK_LOWER_78M);
    DVT_SFC_CK(1, LFOSC_26M, CLOCK_LOWER_78M);
    DVT_SFC_CK(2, MPLL_104M, CLOCK_OVER_78M);
    DVT_SFC_CK(4, MPLL_78M, CLOCK_LOWER_78M);
    DVT_SFC_CK(5, MPLL_62P4M, CLOCK_LOWER_78M);
    DVT_SFC_CK(7, HFOSC_78M, CLOCK_LOWER_78M);
    DVT_SFC_CK(8, HFOSC_62P4M, CLOCK_LOWER_78M);
    DVT_SFC_CK(6, HFOSC_104M, CLOCK_OVER_78M);      /* original setting */
    /* DVT_SFC_CK(3, MPLL_124P8M, CLOCK_OVER_78M); */
#endif
#if DVT_CM_CK_ENABLE
    DVT_CM_CK(0, DCXO_26M, CLOCK_LOWER_78M);
    DVT_CM_CK(1, LFOSC_26M, CLOCK_LOWER_78M);
    DVT_CM_CK(2, UPLL_104M, CLOCK_OVER_78M);
    DVT_CM_CK(3, MPLL_104M, CLOCK_OVER_78M);
    DVT_CM_CK(5, MPLL_208M, CLOCK_OVER_78M);
    DVT_CM_CK(6, MPLL_156M, CLOCK_OVER_78M);
    DVT_CM_CK(7, HFOSC_208M, CLOCK_OVER_78M);
    DVT_CM_CK(8, HFOSC_156M, CLOCK_OVER_78M);
    DVT_CM_CK(4, HFOSC_104M, CLOCK_OVER_78M);       /* original setting in 1.1V */
#endif
#if DVT_DSP_CK_ENABLE
    DVT_DSP_CK(0, DCXO_26M, CLOCK_LOWER_78M);
    DVT_DSP_CK(1, LFOSC_26M, CLOCK_LOWER_78M);
    DVT_DSP_CK(2, MPLL_156M, CLOCK_OVER_78M);
    DVT_DSP_CK(3, MPLL_138P6M, CLOCK_OVER_78M);
    DVT_DSP_CK(4, MPLL_124P8M, CLOCK_OVER_78M);
    DVT_DSP_CK(5, UPLL_124P8M, CLOCK_OVER_78M);
    DVT_DSP_CK(6, HFOSC_156M, CLOCK_OVER_78M);
    DVT_DSP_CK(7, HFOSC_124P8M, CLOCK_OVER_78M);    /* original setting */
#endif
#if DVT_BSI_CK_ENABLE
    DVT_BSI_CK(0, DCXO_26M, CLOCK_LOWER_78M);
    DVT_BSI_CK(1, LFOSC_26M, CLOCK_LOWER_78M);
    DVT_BSI_CK(2, MPLL_124P8M, CLOCK_OVER_78M);
    DVT_BSI_CK(3, MPLL_104M, CLOCK_OVER_78M);
    DVT_BSI_CK(5, HFOSC_104M, CLOCK_OVER_78M);
    DVT_BSI_CK(4, HFOSC_124P8M, CLOCK_OVER_78M);    /* original setting */
#endif
#if DVT_SLCD_CK_ENABLE
    DVT_SLCD_CK(2, MPLL_124P8M, CLOCK_OVER_78M);
    DVT_SLCD_CK(1, LFOSC_26M, CLOCK_LOWER_78M);
    DVT_SLCD_CK(3, MPLL_104M, CLOCK_OVER_78M);
    DVT_SLCD_CK(4, MPLL_78M, CLOCK_LOWER_78M);
    DVT_SLCD_CK(5, HFOSC_124P8M, CLOCK_OVER_78M);
    DVT_SLCD_CK(6, HFOSC_104M, CLOCK_OVER_78M);
    DVT_SLCD_CK(7, HFOSC_78M, CLOCK_LOWER_78M);
    DVT_SLCD_CK(0, DCXO_26M, CLOCK_LOWER_78M);
    DVT_RG_SLCD_CK(1, MPLL_124P8M, CLOCK_OVER_78M);
    DVT_RG_SLCD_CK(0, CLK_DIV_NONE, CLOCK_OVER_78M);/* original setting */
    DVT_SLCD_CK(5, HFOSC_124P8M, CLOCK_OVER_78M);   /* original setting */
#endif
#if DVT_USB_CK_ENABLE
    DVT_USB_CK(0, DCXO_26M, CLOCK_LOWER_78M);
    DVT_USB_CK(1, LFOSC_26M, CLOCK_LOWER_78M);
    DVT_USB_CK(2, UPLL_48M, CLOCK_LOWER_78M);
    DVT_USB_CK(3, UPLL_62P4M, CLOCK_LOWER_78M);     /* original setting */
#endif
#if DVT_MSDC0_CK_ENABLE
    DVT_MSDC0_CK(0, DCXO_26M, CLOCK_LOWER_78M);
    DVT_MSDC0_CK(1, LFOSC_26M, CLOCK_LOWER_78M);
    DVT_MSDC0_CK(2, MPLL_89M, CLOCK_OVER_78M);
    DVT_MSDC0_CK(3, MPLL_78M, CLOCK_LOWER_78M);
    DVT_MSDC0_CK(4, MPLL_62P4M, CLOCK_LOWER_78M);
    DVT_MSDC0_CK(6, HFOSC_78M, CLOCK_LOWER_78M);
    DVT_MSDC0_CK(7, HFOSC_62P4M, CLOCK_LOWER_78M);
    DVT_MSDC0_CK(5, HFOSC_89M, CLOCK_OVER_78M);  	/* original setting */
#endif
#if DVT_MSDC1_CK_ENABLE
    DVT_MSDC1_CK(0, DCXO_26M, CLOCK_LOWER_78M);
    DVT_MSDC1_CK(1, LFOSC_26M, CLOCK_LOWER_78M);
    DVT_MSDC1_CK(2, MPLL_89M, CLOCK_OVER_78M);
    DVT_MSDC1_CK(3, MPLL_78M, CLOCK_LOWER_78M);
    DVT_MSDC1_CK(4, MPLL_62P4M, CLOCK_LOWER_78M);
    DVT_MSDC1_CK(6, HFOSC_78M, CLOCK_LOWER_78M);
    DVT_MSDC1_CK(7, HFOSC_62P4M, CLOCK_LOWER_78M);
    DVT_MSDC1_CK(5, HFOSC_89M, CLOCK_OVER_78M);  	/* original setting */
#endif
#endif
}

#ifdef CLK_DFS_TEST
ATTR_TEXT_IN_TCM void clock_dfs_testing(void)
{
    /* 208m to 104m */
    cm_bus_clk_104m();
    EMI_DynamicClockSwitch(EMI_CLK_HIGH_TO_LOW);

    /* 104m to 208m */
    cm_bus_clk_208m();
    EMI_DynamicClockSwitch(EMI_CLK_LOW_TO_HIGH);

    /* 208m to 26m */
    cm_bus_clk_26m();
    EMI_DynamicClockSwitch(EMI_CLK_HIGH_TO_LOW);
    /* Set flash timing delay to 26MHz, switch SFC PLL to 26MHz DCXO or LFOSC */
    SF_DAL_DEV_SWITCH_TO_LOW_FQ();

    /* 26m to 208m */
    cm_bus_clk_208m();
    EMI_DynamicClockSwitch(EMI_CLK_LOW_TO_HIGH);
    /* Restore flash timing delay to 78MHz, call clock_mux_sel(SFC_MUX_SEL, 7) and switch SFC PLL to 78MHz HFOSC */
    SF_DAL_DEV_SWITCH_TO_HIGH_FQ();

    /* 208m to 104m */
    cm_bus_clk_104m();
    EMI_DynamicClockSwitch(EMI_CLK_HIGH_TO_LOW);

    /* 104m to 26m */
    cm_bus_clk_26m();
    EMI_DynamicClockSwitch(EMI_CLK_HIGH_TO_LOW);
    /* Set flash timing delay to 26MHz, switch SFC PLL to 26MHz DCXO or LFOSC */
    SF_DAL_DEV_SWITCH_TO_LOW_FQ();

    /* 26m to 104m */
    cm_bus_clk_104m();
    EMI_DynamicClockSwitch(EMI_CLK_LOW_TO_HIGH);
    /* Restore flash timing delay to 78MHz, call clock_mux_sel(SFC_MUX_SEL, 7) and switch SFC PLL to 78MHz HFOSC */
    SF_DAL_DEV_SWITCH_TO_HIGH_FQ();
}
#endif

ATTR_TEXT_IN_TCM int clk_init(void)
{
    if (clock_initialized)
        return 0;

    cg_init();
    clock_src_sel_init();
    clock_mux_init();
    pll_init();

    lfosc_freq = clock_get_freq_meter(11, 0, false);

    clock_initialized = true;
#if 0
    lpm_testing(0xF, 0x0, 0x68);
#endif

    /* Set peripheral 26M clk match compile option to clock source DCXO or LFOSC */
    peri_clk_26m();

    /*
        cm_bus_clk_208m();
        cm_bus_clk_104m();
        cm_bus_clk_26m();
        peri_clk_26m();
    */

#if defined CLK_CG_DEFAULT_ON
    clock_all_on();
#else
    clock_pre_on();
#endif

#ifdef CLK_DFS_TEST
    clock_dfs_testing();
#endif

#ifdef CLK_SUSPEND_RESUME_TEST
    clock_suspend(true);
    clock_resume();
#endif

    clock_api_internal_init();

    return 0;
}

ATTR_TEXT_IN_TCM hal_clock_status_t hal_clock_init(void)
{
    return (hal_clock_status_t)(clk_init());
}

#endif /* HAL_CLOCK_MODULE_ENABLED */
