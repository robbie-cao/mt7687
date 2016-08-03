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

#include <stdint.h>
#include <stdio.h>
#include <string.h>


#include "FreeRTOS.h"
#include "task.h"

#include "task_def.h"
#include "syslog.h"
#include "nvdm.h"
#include "sensor_demo.h" /* Turn ON/OFF SENSOR_DEMO inside */
#ifdef SENSOR_DEMO
#include "sensor_manager.h" /* sensor manager task */
#endif

#define ATCI_DEMO
#define DISPLAY_DEMO
#define WATCH_APP_DEMO
#define BT_SINK_DEMO
#define BLE_PXP_DEMO

#ifdef WATCH_APP_DEMO
#include "wf_image.h"
#endif

#ifdef DISPLAY_DEMO
#include "display_demo.h"
#endif

#ifdef BT_SINK_DEMO
#include "bt_sink_task.h"
#include "bt_sink_am_task.h"
#include "bt_hfp_codec_internal.h"
#endif /* BT_SINK_DEMO */

#ifdef ATCI_DEMO
#include "atci.h"

void atci_def_task(void *param)
{
    while (1) {
        atci_processing();
    }
}
#endif

/****************************************************************************
 * Types.
 ****************************************************************************/

typedef struct tasks_list_ {
    TaskFunction_t      pvTaskCode;
    char                *pcName;
    uint16_t            usStackDepth;
    void                *pvParameters;
    UBaseType_t         uxPriority;
} tasks_list_t;

typedef enum module_list_{
    MODULE_NONE,
    MODULE_SENSOR,
    MODULE_BLE_PXP,
    MODULE_BT_SINK,
    MODULE_WF_APP
} module_list;

/****************************************************************************
 * Forward Declarations.
 ****************************************************************************/
extern void ui_task_main();
extern void wf_app_task(void *arg);
extern void bt_codec_task_main(void *arg);
extern void am_task_main(void *arg);
extern void bt_sink_task_main(void *arg);
extern void ble_app_task(void *arg);

static const tasks_list_t tasks_list[] = {
#ifdef SENSOR_DEMO
    { sensor_mgr_task,      "SensorMgr",    6144/(( uint32_t )sizeof( StackType_t )),     NULL,   3 },
#endif

#ifdef ATCI_DEMO
    { atci_def_task,        "ATCI",         2048,     NULL,   3 },
#endif

    { ui_task_main,         "UI_DEMO",      4800,     NULL,   3 },
#ifdef BLE_PXP_DEMO
    { ble_app_task,         "ble_app_task", 512,     NULL,   1 },
#endif

#ifdef WATCH_APP_DEMO
    { wf_app_task,          "wf_task",      1024,     NULL,   1 },
#endif

#ifdef DISPLAY_DEMO
    //{ display_demo_task,    "display_task", 2048,     NULL,   1 },
#endif

#ifdef BT_SINK_DEMO
    { bt_codec_task_main,   "BT_codec_task", 1024,    NULL,   3 },
    { am_task_main,         "AudioManager",  512,     NULL,   3 },
    { bt_sink_task_main,    "BT_sink_task",  1024,    NULL,   1 },
#endif

};

#define tasks_list_count  (sizeof(tasks_list) / sizeof(tasks_list_t))

static TaskHandle_t     task_handles[tasks_list_count];

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static module_list task_get_current_module()
{
    module_list mod;
    nvdm_status_t status;
    uint32_t size = sizeof(mod);
    status = nvdm_read_data_item("common",
                                 "task_mod",
                                 &mod,
                                 &size);
    if (status != NVDM_STATUS_OK) {
        LOG_I(common, "read nvdm failed %d", status);
        mod = MODULE_NONE;
    }
    return mod;
}

static void task_single_create(char *pTaskName)
{
    uint16_t            i;
    BaseType_t          x;
    const tasks_list_t  *t;

    for (i = 0; i < tasks_list_count; i++) {
        t = &tasks_list[i];
        if (strncmp(t->pcName, pTaskName, strlen(t->pcName)) != 0){
            continue;
        }

        LOG_I(common, "xCreate task %s, pri %d", t->pcName, (int)t->uxPriority);

        x = xTaskCreate(t->pvTaskCode,
                        t->pcName,
                        t->usStackDepth,
                        t->pvParameters,
                        t->uxPriority,
                        &task_handles[i]);

        if (x != pdPASS) {
            LOG_E(common, ": failed");
        } else {
            LOG_I(common, ": succeeded");
        }
        break;
    }
    LOG_I(common, "Free Heap size:%d bytes", xPortGetFreeHeapSize());
}

static atci_status_t task_atci_reg_callback(atci_parse_cmd_param_t *cmd)
{
    atci_response_t output = {{0}};

    LOG_I(common,"[TASK_ATCI] mode:%d, CMD:%s", cmd->mode, cmd->string_ptr);
     
    if (cmd->mode == ATCI_CMD_MODE_EXECUTION) {
        bool result;
        if (strstr(cmd->string_ptr, "AT+ETSMO=") != NULL) {
            /* 9byte */
            uint32_t _mode;
            module_list mod;
            if (sscanf(cmd->string_ptr+9, "%d", (int *)&_mode) > 0) {
                mod = _mode;
                LOG_I(common, "mod %d", mod);
                nvdm_status_t ret = nvdm_write_data_item("common",
                                                         "task_mod",
                                                         NVDM_DATA_ITEM_TYPE_RAW_DATA,
                                                         &mod,
                                                         sizeof(mod));
                if (ret != NVDM_STATUS_OK) {
                    result = false;
                }
            } else {
                result =false;
            }
        }
        if (result) {
            strcpy((char *)output.response_buf, "Parse OK\n");
        } else {
            strcpy((char *)output.response_buf, "Parse failed\n");
        }
        atci_send_response(&output); 
         
    } else {

        strcpy((char *)output.response_buf, "Not Support\n");
        output.response_len = strlen((char *)output.response_buf);
        output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_ERROR; 
        atci_send_response(&output); 
    }
    return ATCI_STATUS_OK;
 
}

static atci_cmd_hdlr_item_t task_at_cmd = {
    .command_head ="AT+ETSMO",
    .command_hdlr = task_atci_reg_callback,
    .hash_value1 = 0,
    .hash_value2 = 0,
};

static void task_atci_init()
{
    atci_status_t ret = atci_register_handler(&task_at_cmd, 1);   
    if (ret == ATCI_STATUS_OK) {
        LOG_I(common, "task_atci register success");
    } else {
        LOG_W(common, "task_atci register fail");
    }

}
/****************************************************************************
 * Public API
 ****************************************************************************/
extern size_t xPortGetFreeHeapSize( void );


void task_def_init(void)
{
    module_list mod = task_get_current_module();
#ifdef ATCI_DEMO
    atci_init(HAL_UART_1);
    task_single_create("ATCI");
    task_atci_init();
#endif
    LOG_I(common, "current_mod %d", mod);
    switch (mod) {
        case MODULE_SENSOR:
            break;
        case MODULE_BLE_PXP:
            task_single_create("ble_app_task");
            break;
        case MODULE_BT_SINK:
             hal_audio_init();
             task_single_create("BT_codec_task");
             task_single_create("AudioManager");
             task_single_create("BT_sink_task");
             log_config_print_switch(a2dp, DEBUG_LOG_OFF);
             log_config_print_switch(avrcp, DEBUG_LOG_OFF);
            break;
        default:
            break;
    }
/*
#ifdef SENSOR_DEMO
    //sensor_peripheral_init();
    sensor_manager_init();

    //enable_all_sensors();
    log_config_print_switch(bt_spp, DEBUG_LOG_OFF);
#endif
    
#ifdef ATCI_DEMO
    atci_init(HAL_UART_1);
#endif
    
#ifdef WATCH_APP_DEMO
    wf_app_init();
#endif
        
#ifdef BT_SINK_DEMO
    *(volatile uint32_t *)(0xA201010C) = 0x10000FFF;
    hal_audio_init();
    log_config_print_switch(a2dp, DEBUG_LOG_OFF);
    log_config_print_switch(avrcp, DEBUG_LOG_OFF);    
#endif */
    
}

void task_def_create(void)
{
    uint16_t            i;
    BaseType_t          x;
    const tasks_list_t  *t;

    for (i = 0; i < tasks_list_count; i++) {
        t = &tasks_list[i];

        LOG_I(common, "xCreate task %s, pri %d", t->pcName, (int)t->uxPriority);

        x = xTaskCreate(t->pvTaskCode,
                        t->pcName,
                        t->usStackDepth,
                        t->pvParameters,
                        t->uxPriority,
                        &task_handles[i]);

        if (x != pdPASS) {
            LOG_E(common, ": failed");
        } else {
            LOG_I(common, ": succeeded");
        }
    }
    LOG_I(common, "Free Heap size:%d bytes", xPortGetFreeHeapSize());
}

