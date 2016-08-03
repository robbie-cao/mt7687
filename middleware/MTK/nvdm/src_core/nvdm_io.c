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

#include "nvdm.h"
#include "nvdm_port.h"
#include "nvdm_internal.h"

static uint8_t g_io_buffer[NVDM_BUFFER_SIZE];

extern uint32_t g_nvdm_peb_size;

static int32_t check_pattern(const void *buf, uint8_t patt, int32_t len)
{
    int32_t i;

    for (i = 0; i < len; i++) {
        if (((const uint8_t *)buf)[i] != patt) {
            return -1;
        }
    }

    return 0;
}

static int32_t compare_buffer(const void *buf1, const void *buf2, int32_t len)
{
    int32_t i;

    for (i = 0; i < len; i++) {
        if (((const uint8_t *)buf1)[i] != ((const uint8_t *)buf2)[i]) {
            return -1;
        }
    }

    return 0;
}

void peb_io_read(int32_t pnum, int32_t offset, uint8_t *buf, int32_t len)
{
    uint32_t addr;

    addr = nvdm_port_get_peb_address(pnum, offset);

    nvdm_port_flash_read(addr, buf, len);
}

void peb_io_write(int32_t pnum, int32_t offset, const uint8_t *buf, int32_t len)
{
    int32_t ret, i, fragment;
    uint32_t addr;
    void *io_buffer;

    addr = nvdm_port_get_peb_address(pnum, offset);

    io_buffer = g_io_buffer;

    nvdm_port_flash_write(addr, buf, len);

    /* check whether data has been written correctlly */
    fragment = len / NVDM_BUFFER_SIZE;
    for (i = 0; i < fragment; i++) {
        memset(io_buffer, 0, NVDM_BUFFER_SIZE);
        nvdm_port_flash_read(addr, io_buffer, NVDM_BUFFER_SIZE);
        ret = compare_buffer(io_buffer, &buf[NVDM_BUFFER_SIZE * i], NVDM_BUFFER_SIZE);
        if (ret) {
            nvdm_port_log_error("pnum=%d, offset=0x%x, len=%d", pnum, offset, len);
            return;
        }
        addr += NVDM_BUFFER_SIZE;
    }
    if (len % NVDM_BUFFER_SIZE) {
        memset(io_buffer, 0, NVDM_BUFFER_SIZE);
        nvdm_port_flash_read(addr, io_buffer, len % NVDM_BUFFER_SIZE);
        ret = compare_buffer(io_buffer, &buf[NVDM_BUFFER_SIZE * fragment], len % NVDM_BUFFER_SIZE);
        if (ret) {
            nvdm_port_log_error("addr=0x%x, pnum=%d, offset=0x%x, len=%d", addr, pnum, offset, len);
        }
    }
}

void peb_erase(int32_t pnum)
{
    int32_t ret, i;
    uint32_t addr;
    void *io_buffer;

    addr = nvdm_port_get_peb_address(pnum, 0);

    nvdm_port_flash_erase(addr);

    io_buffer = g_io_buffer;

    /* peb size must be aligned to NVDM_BUFFER_SIZE boundary */
    for (i = 0; i < g_nvdm_peb_size / NVDM_BUFFER_SIZE; i++) {
        nvdm_port_flash_read(addr, io_buffer, NVDM_BUFFER_SIZE);
        ret = check_pattern(io_buffer, 0xFF, NVDM_BUFFER_SIZE);
        if (ret) {
            nvdm_port_log_error("pnum=%d", pnum);
            return;
        }
        addr += NVDM_BUFFER_SIZE;
    }
}

