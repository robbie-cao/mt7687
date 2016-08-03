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

#include <stdlib.h>

#include "hal_efuse.h"

#include "type_def.h"
#include "mt7637_cm4_hw_memmap.h"

/* register address */

#define EFUSE_BIT(n)                ((kal_uint32) 1 << (n))

#define SYS_EFUSE_BASE_ADDR         (EFUSE_base)

#define SYS_EE_CTRL					(0x00 + SYS_EFUSE_BASE_ADDR)
#define SYS_EE_CTRL_EEDONE_MASK		EFUSE_BIT(31)
#define SYS_EE_CTRL_EXT_EE_SEL      EFUSE_BIT(30)
#define SYS_EE_CTRL_CR_CFGWR_START_SW   BIT(8)
#define SYS_EE_CTRL_MCU_ROM_SDOUT_VLD   BIT(4)
#define SYS_EFUSE_CFG				(0x04 + SYS_EFUSE_BASE_ADDR)
#define SYS_EFUSE_CTRL              (0x08 + SYS_EFUSE_BASE_ADDR)
#define SYS_EFUSE_CTRL_EFSROM_DOUT_VLD      EFUSE_BIT(29)
#define SYS_EFUSE_WDATA0            (0x10 + SYS_EFUSE_BASE_ADDR)
#define SYS_EFUSE_WDATA1            (0x14 + SYS_EFUSE_BASE_ADDR)
#define SYS_EFUSE_WDATA2            (0x18 + SYS_EFUSE_BASE_ADDR)
#define SYS_EFUSE_WDATA3            (0x1C + SYS_EFUSE_BASE_ADDR)
#define SYS_EFUSE_CRYSTAL		    (0x3A + SYS_EFUSE_BASE_ADDR)
#define SYS_EFUSE_CRYSTAL_CAPID_MASK		EFUSE_BIT(7)
#define SYS_EFUSE_CRYSTAL_BIT0_2_BIT6_MASK	0x7f
#define SYS_EFUSE_RDATA0            (0x30 + SYS_EFUSE_BASE_ADDR)
#define SYS_EFUSE_RDATA1            (0x34 + SYS_EFUSE_BASE_ADDR)
#define SYS_EFUSE_RDATA2            (0x38 + SYS_EFUSE_BASE_ADDR)
#define SYS_EFUSE_RDATA3            (0x3C + SYS_EFUSE_BASE_ADDR)


hal_efuse_status_t hal_efuse_write(uint32_t magic, uint16_t addr, const uint8_t *buf, uint8_t len)
{
    if (buf == NULL || len != 16) {
        return HAL_EFUSE_INVALID_PARAMETER;
    }

    if (magic != HAL_EFUSE_WRITE_MAGIC) {
        return HAL_EFUSE_INVALID_MAGIC;
    }

    // call real efuse api to do the job.

    return HAL_EFUSE_OK;
}


//mode: 0: physical read, 1: logical read
void SYSefuse_Read16Bytes(u_int32 position, u_int32 *data, u_int32 *log_data_vld,  u_int8 mode)
{
    u_int32 rdata;

    if (position & 0x0F) {
        //postion must be 16-aligned
        return;
    }

    if (0 == mode) {
        // physical read
        rdata = DRV_Reg32(SYS_EFUSE_CTRL);
        rdata &= ~(0x43FF00C0); //clear bit 30, 25-16, 7-6
        DRV_WriteReg32(SYS_EFUSE_CTRL, rdata | (1 << 6) | (position << 16) | (1 << 30));
    } else {
        // logical read
        rdata = DRV_Reg32(SYS_EFUSE_CTRL);
        rdata &= ~(0x43FF00C0); //clear bit 30, 25-16, 7-6
        DRV_WriteReg32(SYS_EFUSE_CTRL, rdata | (0 << 6) | (position << 16) | (1 << 30));
    }

    while ( (DRV_Reg32(SYS_EFUSE_CTRL) >> 30) & 0x1);
    *log_data_vld = (DRV_Reg32(SYS_EFUSE_CTRL) >> 29) & 0x1;


    *data = DRV_Reg32(SYS_EFUSE_RDATA0);
    *(data + 1) = DRV_Reg32(SYS_EFUSE_RDATA1);
    *(data + 2) = DRV_Reg32(SYS_EFUSE_RDATA2);
    *(data + 3) = DRV_Reg32(SYS_EFUSE_RDATA3);

    return;
}

hal_efuse_status_t hal_efuse_read(uint16_t addr, uint8_t *buf, uint8_t len)
{
    u_int32 i;
    u_int8 data16[16];
    u_int8 residual_bytes = len;
    u_int8 offset = 0;
    u_int8 length;
    u_int8 shift;
    u_int8 valid_len;
    u_int32 valid_bit;

    if (buf == NULL) {
        return HAL_EFUSE_INVALID_PARAMETER;
    }

    // call real efuse api to do the job.

    if ( (DRV_Reg32(SYS_EE_CTRL) &  SYS_EE_CTRL_EXT_EE_SEL) == SYS_EE_CTRL_EXT_EE_SEL) {    // 1: select external eeprom
        return HAL_EFUSE_INVALID_ACCESS;
    }

    while (residual_bytes) {
        // offset must be 16-align
        shift = (addr + offset) & 0x0F;
        valid_len = 16 - shift;

        SYSefuse_Read16Bytes(addr + offset - shift, (u_int32 *)data16, &valid_bit, 1);

        if (valid_bit == 0) {
            return HAL_EFUSE_INVALID_ACCESS;
        }

        if (valid_len >= residual_bytes) {
            length = residual_bytes;
        } else {
            length = valid_len;
        }

        for (i = 0; i < length; i++) {
            *buf = data16[i + shift];
            buf++;
        }

        residual_bytes = residual_bytes - length;
        offset += length;
    }

    return HAL_EFUSE_OK;
}


