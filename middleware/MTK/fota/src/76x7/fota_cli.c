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

#ifndef MOD_CFG_FOTA_DISABLE_CLI


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <string.h>

/* FreeRTOS */
#include "FreeRTOS.h"
#include "task.h"

/* Wrapper */
//#include "os_util.h"
//#include "os.h"

/* Other modules */
#include "cli.h"
#include "toi.h"
#include "hal_gpio.h"

#ifndef __PARAM_VERIFY__
#include "hal_flash.h"
#endif

#include "tftpc.h"

#include "fota.h"
#include "fota_config.h"
#include "fota_76x7.h"
#include "fota_download_interface.h"

/****************************************************************************
 * Macros
 ****************************************************************************/


#define HIGH    (true)
#define LOW     (false)

#define I_O_DEBUG

#ifdef I_O_DEBUG
#define I   printf("enter: %s %d\n", __FUNCTION__, __LINE__)
#define O   printf("exit: %s %d\n", __FUNCTION__, __LINE__)
#else
#define I
#define O
#endif

/****************************************************************************
 * Static variables
 ****************************************************************************/


static int8_t s_fota_cli_gpio_pin = -1;


/****************************************************************************
 * Utility
 ****************************************************************************/


static void _fota_gpio_pull(bool high)
{
    hal_gpio_data_t bit;
    hal_gpio_status_t s;

    if (s_fota_cli_gpio_pin == -1) {
        return;
    }

    bit = high ? HAL_GPIO_DATA_HIGH : HAL_GPIO_DATA_LOW;

    s = hal_gpio_set_output(s_fota_cli_gpio_pin, bit);
    if (s != HAL_GPIO_STATUS_OK) {
        printf("err %d\n", s);
    }
}


static ssize_t _fota_download(char *address, char *filename, bool writing,
                              uint32_t partition)
{
    tftpc_t         *tftpc;
    ssize_t         total_len = 0;
    tftpc_status_t  status;
    uint16_t        pkt_len;

    tftpc = tftpc_read_init(address, TFTP_PORT, 21000, filename);

    if (tftpc == NULL) {
        printf("tftpc init failed\n");
        return -1;
    }

    if (writing) {
        if (fota_init(&fota_flash_default_config) != FOTA_STATUS_OK) {
            return -2;
        }

        if (fota_seek(partition, 0) != FOTA_STATUS_OK) {
            return -3;
        }
    }

    do {
        status = tftpc_read_data(tftpc, &pkt_len);

        if (status == TFTPC_STATUS_MORE || status == TFTPC_STATUS_DONE) {
            if (pkt_len != 0) {
                pkt_len     -= 4;
                total_len   += pkt_len;

                if (writing) {
                    if (fota_write(partition, &tftpc->buf[0] + 4, pkt_len) != FOTA_STATUS_OK) {
                        total_len = -4;
                        break;
                    }
                }
            }
        }
        else
        {
            printf("update bin download fail!\n");
        }
    } while (status == TFTPC_STATUS_MORE);

    tftpc_read_done(tftpc);

    if (total_len < 0) {
        printf("download error\n");
    } else if (total_len > 0) {
        printf("total_len %d\n", total_len); // todo: remove me.
    }

    return total_len;
}


/****************************************************************************
 * CLI
 ****************************************************************************/


static uint8_t _fota_cli_erase (uint8_t len, char *param[])
{
    uint32_t    address;
    uint8_t     err;
    hal_flash_status_t f;

    if (len != 1) {
        printf("need params: <addr>\n");
        return -1;
    }

    address = toi(param[0], &err);
    if (err == TOI_ERR) {
        printf("invalid address!\n");
        return -1;
    }

#ifdef __PARAM_VERIFY__
    if ((address & 0xFFF) != 0) {
        printf("erase address must be at 4K boundary\n");
    } else {
        printf("address = 0x%x\n", (unsigned int)address);
    }
#else
    taskDISABLE_INTERRUPTS();
    _fota_gpio_pull(LOW);

    f = hal_flash_erase(address, HAL_FLASH_BLOCK_4K);

    if (f != HAL_FLASH_STATUS_OK) {
        _fota_gpio_pull(HIGH);

        taskENABLE_INTERRUPTS();

        printf("erase failed %d!\n", f);

        return -1;
    }

    _fota_gpio_pull(HIGH);

    taskENABLE_INTERRUPTS();
#endif

    return 0;
}


static uint8_t _fota_cli_write (uint8_t len, char *param[])
{
    uint32_t    address;
    uint32_t    length;
    int         padding;
    uint8_t     *buffer;
    uint8_t     err;
    hal_flash_status_t f;

    if (len != 3) {
        printf("incomplete cmd\n");
        return -1;
    }

    address = toi(param[0], &err);
    if (err == TOI_ERR) {
        printf("invalid address!\n");
        return -1;
    }

    length = toi(param[1], &err);
    if (err == TOI_ERR) {
        printf("invalid length!\n");
        return -1;
    }

    padding = toi(param[2], &err);
    if (err == TOI_ERR) {
        printf("invalid padding!\n");
        return -1;
    }

    if (length < 1) {
        printf("invalid length\n");
        return -1;
    }

    if ((buffer = (uint8_t *)pvPortMalloc(length)) == 0) {
        printf("malloc failed\n");
        return -1;
    }

    memset(buffer, padding, (size_t)length);

#ifdef __PARAM_VERIFY__
    printf("address 0x%0x length %d\n", (unsigned int)address,
                                        (unsigned int)length);
#else
    taskDISABLE_INTERRUPTS();
    _fota_gpio_pull(LOW);
    f = hal_flash_write(address, buffer, length);

    if (f != HAL_FLASH_STATUS_OK) {
        _fota_gpio_pull(HIGH);
        taskENABLE_INTERRUPTS();
        printf("write failed! %d\n", f);
        return -1;
    }
    _fota_gpio_pull(HIGH);
    taskENABLE_INTERRUPTS();
#endif

    vPortFree(buffer);

    return 0;
}


static uint8_t _fota_cli_read  (uint8_t len, char *param[])
{
    uint32_t    address;
    uint32_t    length;
    uint8_t     err;
    uint8_t     *buffer;
    uint32_t    i;
    static      char *newline = "\n";
    static      char *space   = " ";
    static      char *dash    = " - ";

    if (len != 2) {
        printf("incomplete cmd\n");
        return -1;
    }

    address = toi(param[0], &err);
    if (err == TOI_ERR) {
        printf("invalid input!\n");
        return -1;
    }

    length = toi(param[1], &err);
    if (err == TOI_ERR || length > (64 * 1024) || length < 1) {
        printf("invalid input!\n");
        return -1;
    }

    if ((buffer = (uint8_t *)pvPortMalloc(length)) == 0) {
        printf("malloc failed!\n");
        return -1;
    }

#ifdef __PARAM_VERIFY__
    (void)address;
    memset(buffer, 0xcc, length);
#else
    taskDISABLE_INTERRUPTS();
    _fota_gpio_pull(LOW);
    if (hal_flash_read(address, buffer, length) < 0) {
        _fota_gpio_pull(HIGH);
        taskENABLE_INTERRUPTS();
        printf("read failed!\n");
        return -1;
    }
    _fota_gpio_pull(HIGH);
    taskENABLE_INTERRUPTS();
#endif

    for (i = 0; i < length; ) {
        char *separator;

        printf("%02x", buffer[i++] & 0xFF);

        switch (i & 0xF) {
            case 8:  separator = dash;    break;
            case 0:  separator = newline; break;
            default: separator = space;   break;
        }

        if (i == length) {
            separator = newline;
        }

        printf(separator);
    }

    vPortFree(buffer);

    return 0;
}


static uint8_t _fota_cli_gpio  (uint8_t len, char *param[])
{
    uint32_t    pin;
    uint8_t     err;
    hal_gpio_status_t s;

    if (len != 1) {
        printf("need pin\n");
        return -1;
    }

    pin = toi(param[0], &err);

    if ((err == TOI_ERR) ||
        (pin >= HAL_GPIO_MAX)) {
        printf("inv pin\n");
        return -1;
    }

    s = hal_gpio_set_direction(pin, HAL_GPIO_DIRECTION_OUTPUT);

    if (s != HAL_GPIO_STATUS_OK) {
        printf("dir err %d\n", s);
        return -1;
    }

    s_fota_cli_gpio_pin = (int8_t)pin;

    _fota_gpio_pull(LOW);
    _fota_gpio_pull(HIGH);
    _fota_gpio_pull(LOW);
    _fota_gpio_pull(HIGH);

    return 0;
}


static uint8_t _fota_cli_dl    (uint8_t len, char *param[])
{
    ssize_t         size;
    bool            writing = false;

    if (len < 2 || len > 3) {
        printf("need <ip> <filename> [w]\n");
        return -1;
    }

    if (len == 3) {
        if (param[2][0] == 'w' && param[2][1] == '\0' ) {
            writing = true;
        } else {
            printf("err\n");
            return -2;
        }
    }

    size = _fota_download(param[0], param[1], writing, FOTA_PARITION_TMP);

    if (size > 0)
    {
        printf("received %ld bytes in total\n", (int32_t)size);
    }

    return 0;
}


static uint8_t _fota_cli_trigger(uint8_t len, char *param[])
{
    fota_trigger_update();
    printf("\n[FOTA_SDK]Trigger done.\n");
    return 0;
}


static uint8_t _fota_cli_dl_by_http(uint8_t len, char *param[])
{
    uint8_t ret;

    if (len != 1) {
        printf("need <url>\n");
        return -1;
    }
    else {
        ret = fota_download_by_http(param[0]);
        printf ("fota_download_by_http, ret = %d \r\n", ret);
        return ret;
    }

}

/****************************************************************************
 * CLI API Variable
 ****************************************************************************/


cmd_t fota_cli[] =
{
    { "e",      "erases 4KB, param <addr>",         _fota_cli_erase    },
    { "w",      "write <addr> <length> <char>",     _fota_cli_write    },
    { "r",      "read <addr> length",               _fota_cli_read     },
    { "gpio",   "gpio pin to pull",                 _fota_cli_gpio     },
    { "dl",     "<ip> <file> [w]",                  _fota_cli_dl       },
    { "go",     "trigger update",                   _fota_cli_trigger  },
    { "httpdl", "download by http, <url>",          _fota_cli_dl_by_http  },
    { NULL,     NULL,                               NULL               }
};

#endif /* MOD_CFG_FOTA_DISABLE_CLI */

