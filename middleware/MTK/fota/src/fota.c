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

#include "fota.h"
#ifdef MTK_FOTA_ON_7687
#include "flash_map.h"
#include "hal_flash.h"
#else
#include "hal_wdt_retention.h"
#endif


#ifdef MTK_FOTA_ON_7687
/* trigger flag address. Don't modify it please */
#define FOTA_RESERVED_PARTITION_END_ADDRESS    (CM4_FLASH_TMP_ADDR + FLASH_TMP_SIZE)
#define FOTA_TRIGGER_FLAG_ADDRESS    (FOTA_RESERVED_PARTITION_END_ADDRESS - 512)

/* fota magic pattern */
#define FOTA_HEADER_MAGIC_PATTERN      0x004D4D4D  // "MMM"

//static const uint32_t pattern = FOTA_HEADER_MAGIC_PATTERN;
#else

/* #define FOTA_RETENTION_FULL_DL_EN    (0x00000008) */
#define FOTA_RETENTION_FULL_DL_BIT_OFFSET      (3)
#define FOTA_RETENTION_FULL_DL_FLAG_NUMBER     (0)
#endif

#ifdef BL_FOTA_DEBUG
#include "hw_uart.h"
#define FOTA_API_PRINT_I hw_uart_printf
#define FOTA_API_PRINT_W hw_uart_printf
#define FOTA_API_PRINT_E hw_uart_printf
#else
#include "syslog.h"
log_create_module(fota_module_api, PRINT_LEVEL_INFO);
#define FOTA_API_PRINT_I(_message,...)  LOG_I(fota_module_api, (_message), ##__VA_ARGS__)
#define FOTA_API_PRINT_W(_message,...)  LOG_W(fota_module_api, (_message), ##__VA_ARGS__)
#define FOTA_API_PRINT_E(_message,...)  LOG_E(fota_module_api, (_message), ##__VA_ARGS__)
#endif


#ifndef MTK_FOTA_ON_7687
static void fota_set_retention_flag(void)
{
    wdt_set_retention_flag(WDT_RETENTION_FLAG_USER_BL, FOTA_RETENTION_FULL_DL_FLAG_NUMBER, FOTA_RETENTION_FULL_DL_BIT_OFFSET, 1); 
}

static uint8_t fota_read_retention_flag(void)
{
    return wdt_read_retention_flag(FOTA_RETENTION_FULL_DL_FLAG_NUMBER, FOTA_RETENTION_FULL_DL_BIT_OFFSET);
}
#endif

fota_ret_t fota_trigger_update(void)
{
   
#ifdef MTK_FOTA_ON_7687
    hal_flash_status_t ret;
    uint32_t pattern = FOTA_HEADER_MAGIC_PATTERN;
    ret = hal_flash_init();
    if (ret < HAL_FLASH_STATUS_OK)
    {
        FOTA_API_PRINT_E("\n[FOTA_SDK]Hal flash initialize fail, ret = %d\n", ret);
        return FOTA_TRIGGER_FAIL;
    }

    /* erase the last 4k block in fota reserved partition */
    ret = hal_flash_erase(FOTA_RESERVED_PARTITION_END_ADDRESS - 4096, HAL_FLASH_BLOCK_4K);
    if (ret < HAL_FLASH_STATUS_OK)
    {
        FOTA_API_PRINT_E("\n[FOTA_SDK]erase 4k block fail, ret = %d\n", ret);
        return FOTA_TRIGGER_FAIL;
    }

    /* write pattern into 512 bytes ahead of the reserved partition end address */
    FOTA_API_PRINT_I("\n[FOTA_SDK]flag address is 0x%x\n", FOTA_TRIGGER_FLAG_ADDRESS);
    ret = hal_flash_write(FOTA_TRIGGER_FLAG_ADDRESS, (const uint8_t*)&pattern, sizeof(uint32_t));
    if (ret < HAL_FLASH_STATUS_OK)
    {
        FOTA_API_PRINT_I("\n[FOTA_SDK]Trigger update result is %d\n", ret);
        return FOTA_TRIGGER_FAIL;
    }
    else
    {
        FOTA_API_PRINT_I("\n[FOTA_SDK]Trigger update result is %d\n", ret);
        return FOTA_TRIGGER_SUCCESS;
    }
#else
    uint8_t ret;
    fota_set_retention_flag();
    ret = fota_read_retention_flag();
    if (ret)
    {
        return FOTA_TRIGGER_SUCCESS;
    }
    else
    {
        return FOTA_TRIGGER_FAIL;
    }
#endif
}

