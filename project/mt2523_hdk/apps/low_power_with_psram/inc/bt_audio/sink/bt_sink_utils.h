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

#ifndef BT_SINK_UTILS_H
#define BT_SINK_UTILS_H
#include <stdint.h>
#include <stdbool.h>
#ifndef WIN32
#include <syslog.h>
#else
#include "osapi.h"
#endif
#include "bt_sink_resource.h"

#define BT_SINK_DB_TIMER "BT_SINK_TIMER_DB"
#define BT_SINK_MAX_TIMER 5

#ifndef NULL
#define NULL ((void *)0)
#endif

typedef void (*bt_sink_timer_callback_t)(void *parameter);

typedef struct {
    uint32_t timer_id;
    uint32_t user_id;
    bt_sink_timer_callback_t callback;
    void *parmaters;
} bt_sink_timer_record_t;

#define bt_sink_assert configASSERT

#ifndef WIN32
#ifdef __BT_SINK_TRACE__
#define bt_sink_report(_message,...) LOG_I(bt_sink, (_message), ##__VA_ARGS__)
#else
#define bt_sink_report (void)0;
#endif
#else
#define bt_sink_report(_message,...) OS_Report((_message), ##__VA_ARGS__)
#endif /* WIN32 */
void *bt_sink_memory_alloc(uint16_t size);

void bt_sink_memory_free(void *point);

void *bt_sink_memset(void *ptr, int32_t value, uint32_t num);

void *bt_sink_memcpy(void *dest, const void *src, uint32_t size);

int32_t bt_sink_memcmp(const void *dest, const void *src, uint32_t count);

void bt_sink_timer_init(void);

void bt_sink_timer_start(uint32_t user_id, uint32_t delay, bt_sink_timer_callback_t function_p, void *parmaters);

void bt_sink_timer_stop(uint32_t user_id);

bool bt_sink_timer_is_exist(uint32_t user_id);

char *bt_sink_strfind(char *str, const char *sub);

char *bt_sink_strcat(char *dest, const char *src);

char *bt_sink_strcpy(char *dest, const char *src);

#endif /* BT_SINK_UTILS_H */
