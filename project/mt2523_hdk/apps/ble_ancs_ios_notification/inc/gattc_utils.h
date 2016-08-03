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

#ifndef __GATT_UTILS_H__
#define __GATT_UTILS_H__
#include <stdbool.h>
#include <stdint.h>
#include "gattc_utils.h"
#include <assert.h>
#include <stdio.h>

typedef struct _ListNode {
    struct _ListNode *Flink;
    struct _ListNode *Blink;

} ListNode;

#define Get_Head_List(ListHead) (ListHead)->Flink
#define Get_Next_Node(Node)     (Node)->Flink

#define Is_Node_Connected(n) (((n)->Blink->Flink == (n)) && ((n)->Flink->Blink == (n)))
#define Insert_Head_List(a, b) (assert(Is_List_Circular(a)), \
                              _Insert_Head_List(a, b),       \
                              assert(Is_List_Circular(a)))
#define Is_Valid_User(u) ((u) != NULL && Is_Node_OnList(&GATTC(userlist), &((u)->node)))

uint8_t Is_List_Empty(ListNode *list);

void Remove_Entry_List(ListNode *entry);

void Init_List_Entry(ListNode *list);

void Init_List_Head(ListNode *list);

void Insert_Tail_List(ListNode *head, ListNode *entry);

bool Is_List_Circular(ListNode *list);

ListNode *Remove_Head_List(ListNode *head);

void *gattc_malloc(uint32_t size);

void gattc_free(void *ptr);

void _Insert_Head_List(ListNode *head, ListNode *entry);

bool Is_Node_OnList(ListNode *head, ListNode *node);

#define GATTC_UTILS_MALLOC(size)      gattc_malloc(size)
#define GATTC_UTILS_FREE(p)           gattc_free(p)

#endif
