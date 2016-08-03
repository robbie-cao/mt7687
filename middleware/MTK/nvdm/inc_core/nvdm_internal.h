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

#ifndef __NVDM_INTERNAL_H__
#define __NVDM_INTERNAL_H__

#include "string.h"
#include "stdint.h"
#include "stdbool.h"
#include <stdio.h>

#ifndef NULL
#define NULL    (void *)(0)
#endif

typedef enum {
    PEB_STATUS_VIRGIN = 0xFF,   /* the block status is undefined, it maybe has erased or not erased completely */
    PEB_STATUS_EMPTY = 0xFE,    /* the block has valid PEB header, no valid data */
    PEB_STATUS_ACTIVING = 0xFC, /* the block is frist being filled with data item */
    PEB_STATUS_TRANSFERING = 0xF8,  /* the block is being transfered data from reclaimed block */
    PEB_STATUS_TRANSFERED = 0xF0,   /* the block has being transfered data completely */
    PEB_STATUS_ACTIVED = 0xE0,  /* the block has valid PEB header and data */
    PEB_STATUS_RECLAIMING = 0xC0,   /* the block is very dirty, and is being reclaimed */
    PEB_STATUS_ERASING = 0x80,  /* the block is being erased */
} peb_status_t;

#define PEB_HEADER_MAGIC    (0x4d44564e)  /* "NVDM" */
#define PEB_UNRESERVED  (0xF0)
#define NVDM_VERSION    (0x01)

typedef struct {
    uint32_t magic;         /* erase header magic number */
    uint32_t erase_count;   /* erase count of this block */
    peb_status_t status;    /* status of PEB */
    uint8_t peb_reserved;   /* block is reserved and not used */
    uint8_t version;        /* version of NVDM */
    uint8_t reserved;       /* reserved byte */
} peb_header_t;
#define PEB_MAGIC_OFFSET        (0)   /* magic field offset to PEB begin */
#define PEB_ERASE_COUNT_OFFSET  (4)   /* erase_count field offset to PEB begin */
#define PEB_STATUS_OFFSET       (8)   /* status field offset to PEB begin */
#define PEB_RESERVED_OFFSET     (9)   /* peb_reserved field offset to PEB begin */
#define PEB_VERSION_OFFSET      (10)  /* peb_version field offset to PEB begin */
#define PEB_HEADER_SIZE sizeof(peb_header_t)
#define ERASE_COUNT_MAX (0xffffffff)

#define MAX_WRITE_SEQUENCE_NUMBER   (0xffffffff)

typedef enum {
    DATA_ITEM_STATUS_EMPTY = 0xFF,  /* no data item exist after it */
    DATA_ITEM_STATUS_WRITING = 0xFE, /* data item is being written to new place */
    DATA_ITEM_STATUS_VALID = 0xFC, /* data item has been written to this place successfully */
    DATA_ITEM_STATUS_DELETE = 0xF8, /* data item has been discarded because of new copy is ready */
} data_item_status_t;

typedef struct {
    data_item_status_t status;  /* status of data item */
    uint8_t pnum; /* which PEB this data item record store at */
    uint16_t reserved; /* reserved bytes */
    uint16_t offset; /* offset in PEB where data item record begin */
    uint8_t group_name_size;  /* length of group name */
    uint8_t data_item_name_size;  /* length of data item name */
    uint16_t value_size; /* size of data item's content */
    uint8_t index; /* index for data item */
    nvdm_data_item_type_t type; /* display type of data item, it can be type of binary/hex/decimal/string/structure */
    uint32_t sequence_number; /* write sequence number for this data item record */
    uint32_t hash_name; /* hash name of this data item */
} data_item_header_t;
#define DATA_ITEM_HEADER_SIZE   (sizeof(data_item_header_t))
#define DATA_ITEM_CHECKSUM_SIZE (2)
#define MAX_DATA_ITEM_SIZE  (2048)

/* define buffer size of internal used buffer */
#define NVDM_BUFFER_SIZE    (128)

/* Number of blocks used for reclaiming blocks */
#define NVDM_RESERVED_PEB_COUNT (1)

void data_item_scan(int32_t pnum);
void data_item_init(void);
int32_t date_item_migration(int32_t src_pnum, int32_t dst_pnum, int32_t offset);
void peb_io_read(int32_t pnum, int32_t offset, uint8_t *buf, int32_t len);
void peb_io_write(int32_t pnum, int32_t offset, const uint8_t *buf, int32_t len);
void peb_erase(int32_t pnum);
void peb_read_header(int32_t pnum, peb_header_t *peb_header);
void peb_write_data(int32_t pnum, int32_t offset, const uint8_t *buf, int32_t len);
void peb_read_data(int32_t pnum, int32_t offset, uint8_t *buf, int32_t len);
void peb_update_status(int32_t pnum, peb_status_t status);
int32_t peb_activing(int32_t pnum);
int32_t space_allocation(int32_t alloc_size, int32_t added_size, int32_t *poffset);
bool space_is_enough(int32_t size);
void peb_add_drity(int32_t pnum, int32_t drity);
void peb_add_free(int32_t pnum, int32_t free);
void peb_sub_free(int32_t pnum, int32_t free);

#endif

