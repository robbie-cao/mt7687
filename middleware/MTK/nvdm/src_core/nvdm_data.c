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

static int32_t g_sum_data_item_headers = 0;
static uint32_t g_write_sequence_number = 0;
static data_item_header_t *g_data_item_headers;
uint32_t g_max_data_item_size;
uint32_t g_max_group_name_size;
uint32_t g_max_data_item_name_size;
uint32_t g_total_data_item_count;
uint8_t g_working_buffer[NVDM_BUFFER_SIZE];

extern bool nvdm_init_status;
extern uint32_t g_nvdm_peb_size;

static void data_item_header_print_info(data_item_header_t *header)
{
    nvdm_port_log_info("data item header info show below:");
    nvdm_port_log_info("status: 0x%02x", header->status);
    nvdm_port_log_info("pnum: %d", header->pnum);
    nvdm_port_log_info("offset: 0x%04x", header->offset);
    nvdm_port_log_info("sequence_number: %d", header->sequence_number);
    nvdm_port_log_info("group_name_size: %d", header->group_name_size);
    nvdm_port_log_info("data_item_name_size: %d", header->data_item_name_size);
    nvdm_port_log_info("value_size: %d", header->value_size);
    nvdm_port_log_info("index: %d", header->index);
    nvdm_port_log_info("type: %d", header->type);
    nvdm_port_log_info("hash_name: 0x%08x", header->hash_name);
}

static int32_t find_data_item_by_hashname(uint32_t hashname)
{
    int32_t i;

    for (i = 0; i < g_total_data_item_count; i++) {
        if (g_data_item_headers[i].value_size == 0) {
            continue;
        }
        if (hashname == g_data_item_headers[i].hash_name) {
            return i;
        }
    }

    return -1;
}

static int32_t find_empty_data_item(void)
{
    int32_t i;

    for (i = 0; i < g_total_data_item_count; i++) {
        if (g_data_item_headers[i].value_size == 0) {
            return i;
        }
    }

    return -1;
}

static uint16_t calculate_checksum(uint16_t checksum, const uint8_t *buffer, int32_t size)
{
    uint8_t *byte_checksum;
    int32_t i;

    byte_checksum = (uint8_t *)&checksum;

    for (i = 0; i < size; i++) {
        if (i & 0x01) {
            *(byte_checksum + 1) += *(buffer + i);
        } else {
            *byte_checksum += *(buffer + i);
        }
    }

    return checksum;
}

static uint16_t calculate_data_item_checksum(data_item_header_t *header, int32_t pnum, int32_t position)
{
    int32_t i, offset, fragment;
    uint16_t checksum;
    uint8_t *working_buffer = g_working_buffer;

    checksum = 0;

    /* checksum for data item's header
        * skip frist byte because it's not calculated by checksum.
        */
    checksum = calculate_checksum(checksum, &header->pnum, DATA_ITEM_HEADER_SIZE - 1);

    /* add checksum for group name and data item name */
    offset = position;
    peb_read_data(pnum, offset, working_buffer, header->group_name_size + header->data_item_name_size);
    checksum = calculate_checksum(checksum, working_buffer, header->group_name_size + header->data_item_name_size);

    /* add checksum for data item's value */
    offset += header->group_name_size + header->data_item_name_size;
    fragment = header->value_size / NVDM_BUFFER_SIZE;
    for (i = 0; i < fragment; i++) {
        memset(working_buffer, 0, NVDM_BUFFER_SIZE);
        peb_read_data(pnum, offset, working_buffer, NVDM_BUFFER_SIZE);
        checksum = calculate_checksum(checksum, working_buffer, NVDM_BUFFER_SIZE);
        offset += NVDM_BUFFER_SIZE;
    }
    if (header->value_size % NVDM_BUFFER_SIZE) {
        memset(working_buffer, 0, NVDM_BUFFER_SIZE);
        peb_read_data(pnum, offset, working_buffer, header->value_size % NVDM_BUFFER_SIZE);
        checksum = calculate_checksum(checksum, working_buffer, header->value_size % NVDM_BUFFER_SIZE);
    }

    return checksum;
}

static uint32_t calculate_data_item_hashname(const char *group_name, const char *data_item_name)
{
    int32_t i, len;
    uint32_t hash, a = 63689, b = 378551;
    char str[64];

    len = snprintf(str, sizeof(str), "%s%s", group_name, data_item_name);
    if (len < 0) {
        nvdm_port_log_error("group_name=%s, data_item_name=%s", group_name, data_item_name);
        return 0;
    }

    hash = 0;
    for (i = 0; i < len; i++) {
        hash = hash * a + str[i];
        a = a * b;
    }

    nvdm_port_log_info("hashname = 0x%08x", hash);

    return hash;
}

nvdm_status_t nvdm_read_data_item(const char *group_name,
                                  const char *data_item_name,
                                  uint8_t *buffer,
                                  uint32_t *size)
{
    int32_t index;
    uint32_t offset, hashname;
    uint16_t checksum1, checksum2;

    nvdm_port_log_info("nvdm_read_data_item: begin to read");

    if ((group_name == NULL) ||
            (data_item_name == NULL) ||
            (buffer == NULL) ||
            (size == NULL) ||
            (*size == 0)) {
        if (size != NULL) {
            *size = 0;
        }
        return NVDM_STATUS_INVALID_PARAMETER;
    }

    if ((strlen(group_name) > g_max_group_name_size) ||
            (strlen(data_item_name) > g_max_data_item_name_size)) {
        return NVDM_STATUS_INVALID_PARAMETER;
    }

    if (nvdm_init_status == false) {
        return NVDM_STATUS_ERROR;
    }

    nvdm_port_mutex_take();

    hashname = calculate_data_item_hashname(group_name, data_item_name);
    index = find_data_item_by_hashname(hashname);
    if (index < 0) {
        nvdm_port_mutex_give();
        *size = 0;
        return NVDM_STATUS_ITEM_NOT_FOUND;
    }

    /* check whether buffer size is enough */
    if (*size < g_data_item_headers[index].value_size) {
        *size = 0;
        nvdm_port_mutex_give();
        return NVDM_STATUS_INVALID_PARAMETER;
    }

    /* verify checksum of date item */
    checksum1 = calculate_data_item_checksum(&g_data_item_headers[index],
                g_data_item_headers[index].pnum,
                g_data_item_headers[index].offset + DATA_ITEM_HEADER_SIZE);
    offset = g_data_item_headers[index].offset + DATA_ITEM_HEADER_SIZE + g_data_item_headers[index].group_name_size + g_data_item_headers[index].data_item_name_size + g_data_item_headers[index].value_size;
    peb_read_data(g_data_item_headers[index].pnum, offset, (uint8_t *)&checksum2, DATA_ITEM_CHECKSUM_SIZE);
    if (checksum1 != checksum2) {
        *size = 0;
        nvdm_port_mutex_give();
        return NVDM_STATUS_INCORRECT_CHECKSUM;
    }

    /* checksum is ok, so read it to user buffer */
    offset = g_data_item_headers[index].offset + DATA_ITEM_HEADER_SIZE + g_data_item_headers[index].group_name_size + g_data_item_headers[index].data_item_name_size;
    peb_read_data(g_data_item_headers[index].pnum, offset, buffer, g_data_item_headers[index].value_size);
    buffer += g_data_item_headers[index].value_size;
    memset(buffer, 0, *size - g_data_item_headers[index].value_size);

    *size = g_data_item_headers[index].value_size;

    nvdm_port_log_info("group_name = %s, data_item_name = %s, size = %d", group_name, data_item_name, g_data_item_headers[index].value_size);

    nvdm_port_mutex_give();

    return NVDM_STATUS_OK;
}

nvdm_status_t nvdm_write_data_item(const char *group_name,
                                   const char *data_item_name,
                                   nvdm_data_item_type_t type,
                                   const uint8_t *buffer,
                                   uint32_t size)
{
    int32_t append, peb_status_update;
    int32_t added_size, alloc_size, group_name_size, data_item_name_size;
    int32_t index, pnum, old_pnum, offset, old_offset;
    uint32_t hashname;
    uint16_t checksum;
    uint8_t *working_buffer;
    data_item_header_t *p_data_item_header;
    data_item_header_t data_item_header;
    data_item_status_t status;
    int32_t ret;

    nvdm_port_log_info("nvdm_write_data_item: begin to write");

    if ((group_name == NULL) ||
            (data_item_name == NULL) ||
            (buffer == NULL) ||
            (size > g_max_data_item_size) ||
            (size == 0)) {
        return NVDM_STATUS_INVALID_PARAMETER;
    }

    if ((type != NVDM_DATA_ITEM_TYPE_RAW_DATA) &&
            (type != NVDM_DATA_ITEM_TYPE_STRING)) {
        return NVDM_STATUS_INVALID_PARAMETER;
    }

    if ((strlen(group_name) > g_max_group_name_size) ||
            (strlen(data_item_name) > g_max_data_item_name_size)) {
        return NVDM_STATUS_INVALID_PARAMETER;
    }

    if (nvdm_init_status == false) {
        return NVDM_STATUS_ERROR;
    }

    nvdm_port_mutex_take();

    group_name_size = strlen(group_name) + 1;
    data_item_name_size = strlen(data_item_name) + 1;

    hashname = calculate_data_item_hashname(group_name, data_item_name);
    index = find_data_item_by_hashname(hashname);
    nvdm_port_log_info("find_data_item_by_hashname return %d", index);
    if (index < 0) {
        append = 1;
        /* find a empty position to fill in */
        index = find_empty_data_item();
    } else {
        append = 0;
    }

    /* check whether we have enough free space for append */
    if (append) {
        added_size = size + group_name_size + data_item_name_size + DATA_ITEM_HEADER_SIZE + DATA_ITEM_CHECKSUM_SIZE;
    } else {
        added_size = (int32_t)size - (int32_t)g_data_item_headers[index].value_size;
    }
    ret = space_is_enough(added_size);
    if (ret == false) {
        nvdm_port_log_info("peb free space is not enough\n");
        nvdm_port_mutex_give();
        return NVDM_STATUS_INSUFFICIENT_SPACE;
    }

    /* find a peb with require free space to place data item */
    alloc_size = size + group_name_size + data_item_name_size + DATA_ITEM_HEADER_SIZE + DATA_ITEM_CHECKSUM_SIZE;
    pnum = space_allocation(alloc_size, added_size, &offset);
    if (pnum < 0) {
        nvdm_port_log_info("space_allocation fail");
        nvdm_port_mutex_give();
        return NVDM_STATUS_INSUFFICIENT_SPACE;
    }

    if (append) {
        nvdm_port_log_info("new data item append");
        p_data_item_header = &g_data_item_headers[index];
        p_data_item_header->reserved = 0xFF;
        p_data_item_header->type = type;
        p_data_item_header->hash_name = hashname;
        p_data_item_header->value_size = size;
        p_data_item_header->index = g_sum_data_item_headers++;
        p_data_item_header->sequence_number = ++g_write_sequence_number;
        p_data_item_header->pnum = pnum;
        p_data_item_header->group_name_size = group_name_size;
        p_data_item_header->data_item_name_size = data_item_name_size;
        p_data_item_header->offset = offset;
        data_item_header_print_info(p_data_item_header);
    } else {
        nvdm_port_log_info("old data item overwrite");
        p_data_item_header = &g_data_item_headers[index];
        p_data_item_header->sequence_number = ++g_write_sequence_number;
        old_pnum = p_data_item_header->pnum;
        p_data_item_header->pnum = pnum;
        old_offset = p_data_item_header->offset;
        p_data_item_header->offset = offset;
        p_data_item_header->value_size = size;
        data_item_header_print_info(&g_data_item_headers[index]);
    }

    if (g_sum_data_item_headers > g_total_data_item_count) {
        nvdm_port_log_error("too many data items in nvdm region\n");
        return NVDM_STATUS_ERROR;
    }

    nvdm_port_log_info("group_name = %s, data_item_name = %s, size = %d", group_name, data_item_name, size);

    /* calculate checksum for new data item copy */
    checksum = 0;
    /* DATA_ITEM_HEADER_SIZE-1 must be power of 2 */
    checksum = calculate_checksum(checksum, &p_data_item_header->pnum, DATA_ITEM_HEADER_SIZE - 1);
    working_buffer = g_working_buffer;
    memcpy(working_buffer, group_name, group_name_size);
    working_buffer += group_name_size;
    memcpy(working_buffer, data_item_name, data_item_name_size);
    working_buffer -= group_name_size;
    checksum = calculate_checksum(checksum, working_buffer, (group_name_size + data_item_name_size));
    checksum = calculate_checksum(checksum, buffer, size);

    /* this peb is frist written, so status of PEB need to be modified */
    if (peb_activing(pnum)) {
        peb_status_update = 1;
    } else {
        peb_status_update = 0;
    }

    peb_sub_free(pnum, alloc_size);

    /* set status of data item to writing */
    status = DATA_ITEM_STATUS_WRITING;
    peb_write_data(pnum, offset, (uint8_t *)&status, 1);
    nvdm_port_poweroff(1);
    /* write header of data item (not including status) */
    offset += 1;
    peb_write_data(pnum, offset, &p_data_item_header->pnum, DATA_ITEM_HEADER_SIZE - 1);
    /* write group name and data item name */
    offset += DATA_ITEM_HEADER_SIZE - 1;
    peb_write_data(pnum, offset, working_buffer, group_name_size + data_item_name_size);
    /* write value of data item */
    offset += group_name_size + data_item_name_size;
    peb_write_data(pnum, offset, (uint8_t *)buffer, size);
    /* write checksum of data item */
    offset += size;
    peb_write_data(pnum, offset, (uint8_t *)&checksum, DATA_ITEM_CHECKSUM_SIZE);
    /* set status of data item to valid */
    offset -= p_data_item_header->value_size + DATA_ITEM_HEADER_SIZE + group_name_size + data_item_name_size;
    status = DATA_ITEM_STATUS_VALID;
    peb_write_data(pnum, offset, (uint8_t *)&status, 1);
    nvdm_port_poweroff(2);

    if (peb_status_update) {
        /* now we have at least one data item in PEB,
              * so update it's status to PEB_STATUS_ACTIVED
              */
        peb_update_status(pnum, PEB_STATUS_ACTIVED);
        nvdm_port_poweroff(3);
    }

    if (!append) {
        /* because we have write new copy successfully,
              * so we can invalidate old copy now!
              */
        status = DATA_ITEM_STATUS_DELETE;
        peb_write_data(old_pnum, old_offset, (uint8_t *)&status, 1);
        nvdm_port_poweroff(4);
        /* mark drity for old copy */
        peb_read_data(old_pnum, old_offset, (uint8_t *)&data_item_header, DATA_ITEM_HEADER_SIZE);
        size = DATA_ITEM_HEADER_SIZE +
               data_item_header.group_name_size +
               data_item_header.data_item_name_size +
               data_item_header.value_size +
               DATA_ITEM_CHECKSUM_SIZE;
        peb_add_drity(old_pnum, size);
    }

    nvdm_port_mutex_give();

    return NVDM_STATUS_OK;
}

void data_item_scan(int32_t pnum)
{
    int32_t offset, oldpnum;
    int32_t peb_drity, peb_valid;
    data_item_header_t data_item_header;
    uint16_t checksum1, checksum2;
    uint16_t size;
    static int32_t abnormal_data_item = -1;
    data_item_status_t status;

    nvdm_port_log_info("scanning pnum(%d) to analysis data item info", pnum);
    offset = 0;
    peb_drity = 0;
    peb_valid = 0;
    /* scan entire peb content */
    while (offset < (g_nvdm_peb_size - PEB_HEADER_SIZE)) {
        peb_read_data(pnum, offset, (uint8_t *)&data_item_header, DATA_ITEM_HEADER_SIZE);
        data_item_header_print_info(&data_item_header);
        size = DATA_ITEM_HEADER_SIZE + data_item_header.group_name_size + data_item_header.data_item_name_size + data_item_header.value_size + DATA_ITEM_CHECKSUM_SIZE;
        switch (data_item_header.status) {
            case DATA_ITEM_STATUS_EMPTY:
                peb_add_free(pnum, (g_nvdm_peb_size - PEB_HEADER_SIZE) - peb_drity - peb_valid);
                return;
            case DATA_ITEM_STATUS_WRITING:
                /* we can't belive data item header if we found it's writting,
                          * so just mark rest of space is dirty.
                          */
                peb_add_drity(pnum, (g_nvdm_peb_size - PEB_HEADER_SIZE) - peb_drity - peb_valid);
                return;
            case DATA_ITEM_STATUS_VALID:
                break;
            case DATA_ITEM_STATUS_DELETE:
                peb_drity += size;
                offset += size;
                peb_add_drity(pnum, size);
                continue;
            default:
                nvdm_port_log_error("pnum=%d, offset=0x%x", pnum, offset);
                return;
        }

        /* verify checksum of data item */
        checksum1 = calculate_data_item_checksum(&data_item_header, pnum, data_item_header.offset + DATA_ITEM_HEADER_SIZE);
        offset += DATA_ITEM_HEADER_SIZE + data_item_header.group_name_size + data_item_header.data_item_name_size + data_item_header.value_size;
        peb_read_data(pnum, offset, (uint8_t *)&checksum2, DATA_ITEM_CHECKSUM_SIZE);
        offset += DATA_ITEM_CHECKSUM_SIZE;
        if (checksum1 != checksum2) {
            nvdm_port_log_info("detect checksum error\n");
            peb_drity += size;
            peb_add_drity(pnum, size);
            continue;
        }

        /* update max write sequence number */
        if (g_write_sequence_number < data_item_header.sequence_number) {
            g_write_sequence_number = data_item_header.sequence_number;
        }
        /* update count of data items */
        if (g_data_item_headers[data_item_header.index].sequence_number == MAX_WRITE_SEQUENCE_NUMBER) {
            /* we find this frist time */
            memcpy(&g_data_item_headers[data_item_header.index], &data_item_header, sizeof(data_item_header_t));
            g_sum_data_item_headers++;
            if (g_sum_data_item_headers > g_total_data_item_count) {
                nvdm_port_log_error("too many data items in nvdm region\n");
                return;
            }
            peb_valid += size;
        } else {
            /* we found it before, so compare sequence number of them
                    * this is possible that new copy is total update
                    * but old copy has not been invalidated when power-off happen.
                    */
            nvdm_port_log_info("detect two valid copy of data item");
            nvdm_port_log_info("copy1(pnum=%d, offset=0x%04x), copy2(pnum=%d, offset=0x%04x)\n",
                               g_data_item_headers[data_item_header.index].pnum,
                               g_data_item_headers[data_item_header.index].offset,
                               data_item_header.pnum,
                               data_item_header.offset);
            if ((abnormal_data_item > 0) ||
                    (g_data_item_headers[data_item_header.index].sequence_number == data_item_header.sequence_number)) {
                /* this should only happen once at most */
                nvdm_port_log_error("abnormal_data_item = %d", abnormal_data_item);
                return;
            }
            abnormal_data_item = 1;
            if (g_data_item_headers[data_item_header.index].sequence_number < data_item_header.sequence_number) {
                /* we find new copy, so mark old peb as delete */
                status = DATA_ITEM_STATUS_DELETE;
                peb_write_data(g_data_item_headers[data_item_header.index].pnum,
                               g_data_item_headers[data_item_header.index].offset,
                               (uint8_t *)&status,
                               1);
                nvdm_port_poweroff(5);
                /* add valid info */
                peb_valid += size;
                /* add dirty info */
                oldpnum = g_data_item_headers[data_item_header.index].pnum;
                size = g_data_item_headers[data_item_header.index].value_size +
                       g_data_item_headers[data_item_header.index].group_name_size +
                       g_data_item_headers[data_item_header.index].data_item_name_size +
                       DATA_ITEM_CHECKSUM_SIZE + DATA_ITEM_HEADER_SIZE;
                peb_add_drity(oldpnum, size);
                /* if we found old copy in same peb, we must substract it's size from peb_valid */
                if (oldpnum == pnum) {
                    peb_valid -= size;
                }
                memcpy(&g_data_item_headers[data_item_header.index], &data_item_header, sizeof(data_item_header_t));

                /* if we found it in the same peb last time */
                if (oldpnum == pnum) {
                    peb_drity += size;
                }
            } else {
                /* we find old copy, so mark it as delete directly */
                status = DATA_ITEM_STATUS_DELETE;
                peb_write_data(data_item_header.pnum,
                               data_item_header.offset,
                               (uint8_t *)&status,
                               1);
                nvdm_port_poweroff(6);
                peb_drity += size;
                peb_add_drity(pnum, size);
            }
        }
    }
}

void data_item_init(void)
{
    uint32_t i;

    g_total_data_item_count = nvdm_port_get_data_item_config(&g_max_data_item_size, &g_max_group_name_size, &g_max_data_item_name_size);
    if (g_max_data_item_size > MAX_DATA_ITEM_SIZE) {
        nvdm_port_log_error("Max size of data item must less than or equal to 2048 bytes");
        return;
    }

    g_data_item_headers = (data_item_header_t *)nvdm_port_malloc(g_total_data_item_count * sizeof(data_item_header_t));
    if (g_data_item_headers == NULL) {
        nvdm_port_log_error("alloc data_item_headers fail");
        return;
    }

    memset(g_data_item_headers, 0, g_total_data_item_count * sizeof(data_item_header_t));
    for (i = 0; i < g_total_data_item_count; i++) {
        g_data_item_headers[i].sequence_number = MAX_WRITE_SEQUENCE_NUMBER;
    }
    g_write_sequence_number = 0;
    g_sum_data_item_headers = 0;
}

static void data_migration(int32_t src_pnum, int32_t src_offset,
                           int32_t dst_pnum, int32_t dst_offset, int32_t size)
{
    int32_t i, delta, fragment;
    uint8_t *working_buffer = g_working_buffer;

    fragment = size / NVDM_BUFFER_SIZE;
    delta = 0;
    for (i = 0; i < fragment; i++) {
        memset(working_buffer, 0, NVDM_BUFFER_SIZE);
        peb_read_data(src_pnum, src_offset + delta, working_buffer, NVDM_BUFFER_SIZE);
        peb_write_data(dst_pnum, dst_offset + delta, working_buffer, NVDM_BUFFER_SIZE);
        delta += NVDM_BUFFER_SIZE;
    }
    if (size % NVDM_BUFFER_SIZE) {
        memset(working_buffer, 0, NVDM_BUFFER_SIZE);
        peb_read_data(src_pnum, src_offset + delta, working_buffer, size % NVDM_BUFFER_SIZE);
        peb_write_data(dst_pnum, dst_offset + delta, working_buffer, size % NVDM_BUFFER_SIZE);
    }
}

int32_t date_item_migration(int32_t src_pnum, int32_t dst_pnum, int32_t offset)
{
    data_item_header_t data_item_header;
    data_item_status_t status;
    int32_t pos, size;
    uint16_t checksum;

    /* search valid data item */
    pos = 0;
    while (pos < (g_nvdm_peb_size - PEB_HEADER_SIZE)) {
        peb_read_data(src_pnum, pos, (uint8_t *)&data_item_header, DATA_ITEM_HEADER_SIZE);
        switch (data_item_header.status) {
            case DATA_ITEM_STATUS_WRITING:
            case DATA_ITEM_STATUS_EMPTY:
                /* no more data item after it, just return */
                return offset;
            case DATA_ITEM_STATUS_DELETE:
                /* do nothing, just skip it to find next data item.
                          * data item is marked as delete status, it must be an old copy.
                          */
                pos += DATA_ITEM_HEADER_SIZE +
                       data_item_header.group_name_size + data_item_header.data_item_name_size +
                       data_item_header.value_size +
                       DATA_ITEM_CHECKSUM_SIZE;
                break;
            case DATA_ITEM_STATUS_VALID:
                if (g_data_item_headers[data_item_header.index].sequence_number != data_item_header.sequence_number) {
                    /* find old copy, this should not happen,
                                 * because it's fixed in init phase.
                                 */
                    nvdm_port_log_error("old_src_pnum=%d, old_pos=0x%x, new_src_pnum=%d, new_pos=0x%x",
                                        src_pnum, pos,
                                        g_data_item_headers[data_item_header.index].pnum,
                                        g_data_item_headers[data_item_header.index].offset);
                    return 0;
                    /* update offset for next write */
                    //pos += DATA_ITEM_HEADER_SIZE + data_item_header.value_size + DATA_ITEM_CHECKSUM_SIZE;
                } else {
                    /* find up-to-date copy, so migrate it to target peb update header */
                    g_data_item_headers[data_item_header.index].pnum = dst_pnum;
                    g_data_item_headers[data_item_header.index].offset = offset;

                    /* calculate new checksum */
                    checksum = calculate_data_item_checksum(&g_data_item_headers[data_item_header.index], src_pnum, data_item_header.offset + DATA_ITEM_HEADER_SIZE);

                    /* mark writ of beginning */
                    status = DATA_ITEM_STATUS_WRITING;
                    peb_write_data(dst_pnum, offset, (uint8_t *)&status, 1);
                    nvdm_port_poweroff(7);

                    /* write header of data item */
                    peb_write_data(dst_pnum, offset + 1, &g_data_item_headers[data_item_header.index].pnum, DATA_ITEM_HEADER_SIZE - 1);

                    /* write group name, data item name and value of data item */
                    data_migration(src_pnum, pos + DATA_ITEM_HEADER_SIZE, dst_pnum, offset + DATA_ITEM_HEADER_SIZE, data_item_header.group_name_size + data_item_header.data_item_name_size + data_item_header.value_size);

                    /* write checksum of data item */
                    peb_write_data(dst_pnum, offset + DATA_ITEM_HEADER_SIZE + data_item_header.group_name_size + data_item_header.data_item_name_size + data_item_header.value_size, (uint8_t *)&checksum, DATA_ITEM_CHECKSUM_SIZE);

                    /* mark write of end */
                    status = DATA_ITEM_STATUS_VALID;
                    peb_write_data(dst_pnum, offset, (uint8_t *)&status, 1);
                    nvdm_port_poweroff(8);

                    size = DATA_ITEM_HEADER_SIZE + data_item_header.group_name_size + data_item_header.data_item_name_size + data_item_header.value_size + DATA_ITEM_CHECKSUM_SIZE;

                    /* substract free size of target peb */
                    peb_sub_free(dst_pnum, size);

                    /* update offset for next write */
                    offset += size;
                    pos += size;
                }
                break;
            default:
                nvdm_port_log_error("src_pnum=%d, pos=0x%x", src_pnum, pos);
                return 0;
        }
    }

    return offset;
}

