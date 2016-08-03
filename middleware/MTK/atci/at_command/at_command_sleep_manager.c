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

#include "FreeRTOS.h"
#include "task.h"
#include <string.h>
#include <stdio.h>
#include "at_command.h"
#include "syslog.h"
#include "hal_cm4_topsm.h"
#include "hal_sys_topsm.h"
#include "hal_bsi.h"
#include "hal_sleep_manager.h"
#include "hal_pmu_internal.h"
#include "hal_dsp_topsm.h"
#include "hal_gpt.h"
#include "hal_clock.h"
#include "hal_clock_internal.h"
#include "hal_sleep_driver.h"
#include "hal_nvic_internal.h"
#include "hal_pmu.h"
#include "memory_attribute.h"
#include "hal_feature_config.h"
#include <ctype.h>
#include "hal_nvic.h"
char *end_pos = NULL;
int input_value = 0;
int input_1 = 0;
int input_2 = 0;
char *mid_pos = NULL;
char str[20] = { 0 };

extern void mtcmos_bt(void);
extern void mtcmos_mm(void);
extern void mtcmos_md2g(void);
extern void bt_mtcmos_workaround(bool b);
extern void cm4_topsm_printf_option(bool b);
extern hal_sleep_manager_status_t hal_sleep_manager_unlock_sleep(uint8_t handle_index);
extern void vPortSetupTimerInterrupt(void);
extern atci_status_t atci_deinit(hal_uart_port_t port);
ATTR_RWDATA_IN_TCM extern bool FAST_WAKEUP_VERIFICATION;
ATTR_RWDATA_IN_TCM volatile unsigned int atsm_mask_pri;
extern uint8_t at_cmd_lock_sleep_handle;
/*
 * sample code
*/
#define LOGE(fmt,arg...)   LOG_E(atcmd, "ATCMD: "fmt,##arg)
#define LOGW(fmt,arg...)   LOG_W(atcmd, "ATCMD: "fmt,##arg)
#define LOGI(fmt,arg...)   LOG_I(atcmd ,"ATCMD: "fmt,##arg)

atci_response_t response = {{0}};


/*--- Function ---*/
atci_status_t atci_cmd_hdlr_sleep_manager(atci_parse_cmd_param_t *parse_cmd);

/*
AT+TEST=<op>	              |   "OK" or "ERROR"

*/

// AT command handler

void suspend_call_back_test(void *para)
{
    LOGW("suspend callback\n");
}

void resume_call_back_test(void *para)
{
    LOGW("resume callback\n");
}

int reload_pmu_power_mode_table(unsigned int control);
// change the data with hex type in string to data with dec type
int sm_htoi(char s[])
{
    int i;
    int n = 0;
    if (s[0] == '0' && (s[1]=='x' || s[1]=='X')){
        i = 2;
    }else{
        i = 0;
    }
    for (; (s[i] >= '0' && s[i] <= '9') || (s[i] >= 'a' && s[i] <= 'z') || (s[i] >='A' && s[i] <= 'Z');++i){
        if (tolower((unsigned char)s[i]) > '9'){
            n = 16 * n + (10 + tolower((unsigned char)s[i]) - 'a');
        }else{
            n = 16 * n + (tolower((unsigned char)s[i]) - '0');
        }
    }
    return n;
}


atci_status_t atci_cmd_hdlr_sleep_manager(atci_parse_cmd_param_t *parse_cmd)
{
    LOGW("=atci_cmd_hdlr_sleep_manager==\n");
    response.response_flag = 0; // Command Execute Finish.

    if (strstr((char *) parse_cmd->string_ptr, "AT+SM=info") != NULL) {
        LOGW("MTCMOS Control:  AT+SM=MTCMOS,a,b \n a:MTCMOS index \n b: 0:off ; 1:on\n EX: AT+SM=MTCMOS,0,0 // BT MTCMOS OFF");
    }
    if (strstr((char *) parse_cmd->string_ptr, "AT+SM=MTCMOS") != NULL) {
        mid_pos = strchr(parse_cmd->string_ptr, ',');
        mid_pos++;
        end_pos = strchr(mid_pos, ',');
        memcpy(str, mid_pos, strlen(mid_pos) - strlen(end_pos));
        input_1 = sm_htoi(mid_pos);
        end_pos++;
        input_2 = sm_htoi(end_pos);
        mid_pos = NULL;
        end_pos = NULL;
        LOGW("[MTCMOS : %d %d ]",input_1,input_2);
        if (input_1 > 2 || input_2 > 1) {
            LOGW("[Error Conmand]\n");
            return ATCI_STATUS_OK;
        }
        sys_topsm_mtcmos_control((hal_sys_topsm_mtcmos_enum_t) input_1,input_2);
    } else if (strstr((char *) parse_cmd->string_ptr, "AT+SM=VCORE") != NULL) {
        mid_pos = strchr(parse_cmd->string_ptr, ',');
        mid_pos++;
        end_pos = strchr(mid_pos, ',');
        memcpy(str, mid_pos, strlen(mid_pos) - strlen(end_pos));
        input_1 = sm_htoi(mid_pos);
        end_pos++;
        input_2 = sm_htoi(end_pos);
        mid_pos = NULL;
        end_pos = NULL;
        LOGW("[VCORE : %d %d ]",input_1,input_2);
        switch (input_1) {
            case 0: //HP
                PMIC_VCORE_VOSEL_CONFIG(PMIC_PSI_HP,(PMIC_VCORE_VOSEL) input_2);
                break;
            case 1: //LP
                PMIC_VCORE_VOSEL_CONFIG(PMIC_PSI_LP,(PMIC_VCORE_VOSEL) input_2);
                break;
            case 2: //S0
                PMIC_VCORE_VOSEL_CONFIG(PMIC_PSI_S0,(PMIC_VCORE_VOSEL) input_2);
                break;
            case 3: //S1
                PMIC_VCORE_VOSEL_CONFIG(PMIC_PSI_S1,(PMIC_VCORE_VOSEL) input_2);
                break;
        }
    } else if (strstr((char *) parse_cmd->string_ptr, "AT+SM=SLEEP") != NULL) {
        mid_pos = strchr(parse_cmd->string_ptr, ',');
        mid_pos++;
        end_pos = strchr(mid_pos, ',');
        memcpy(str, mid_pos, strlen(mid_pos) - strlen(end_pos));
        input_1 = sm_htoi(mid_pos);
        end_pos++;
        input_2 = sm_htoi(end_pos);
        mid_pos = NULL;
        end_pos = NULL;
        LOGW("[SLEEP : %d %d ]",input_1,input_2);
        switch (input_1) {
            case 0:
                LOGW("[SHOW MTCMOS DEBUG OPTION]\n");
                sys_topsm_debug_log(HAL_SYS_TOPSM_MTCMOS_DEBUG);
                break;
            case 1:
                if (input_2 == 1) {
                    LOGW("[ENABLE MTCMOS DEBUG OPTION]\n");
                    sys_topsm_debug_option(true);
                    cm4_topsm_debug_option(true);
                } else if (input_2 == 0) {
                    LOGW("[DISABLE MTCMOS DEBUG OPTION]\n");
                    sys_topsm_debug_option(false);
                    cm4_topsm_debug_option(false);
                } else {
                    LOGW("[Error Command \n");
                }
                break;
            case 2:
                LOGW("[MT2523 HW Debug Mode\n");
                *((volatile uint32_t*) 0xA2010238) = 0x1B;
                *((volatile uint32_t*) 0xa2020C30) = 0x77777777;
                *((volatile uint32_t*) 0xa2020C40) = 0x77771117;
                *((volatile uint32_t*) 0xa2020C50) = 0x7700;
                break;
            case 3:
                LOGW("[Show Lock Sleep Mode\n");
                sleepdrv_get_lock_module();
                break;
            case 4:
                break;
            case 5:

                break;
            case 6:

            case 7:
                if (input_2 == 0) {
                    LOGW("[BT MTCMOS lock in sleep]\n");
                    cm4_topsm_lock_BT_MTCMOS();
                } else if (input_2 == 1) {
                    LOGW("[MM MTCMOS lock in sleep]\n");
                    cm4_topsm_lock_MM_MTCMOS();
                } else if (input_2 == 2) {
                    LOGW("[MD2G MTCMOS lock in sleep]\n");
                    cm4_topsm_lock_MD2G_MTCMOS();
                } else if (input_2 == 3) {
                    LOGW("[BT MTCMOS unlock in sleep]\n");
                    cm4_topsm_unlock_BT_MTCMOS();
                } else if (input_2 == 4) {
                    LOGW("[MM MTCMOS unlock in sleep]\n");
                    cm4_topsm_unlock_MM_MTCMOS();
                } else if (input_2 == 5) {
                    LOGW("[MD2G MTCMOS unlock in sleep]\n");
                    cm4_topsm_unlock_MD2G_MTCMOS();
                }
                break;
            case 8:
                if (input_2 == 0) {
                    LOGW("[BT MTCMOS Close in sleep]\n");
                    mtcmos_bt();
                } else if (input_2 == 1) {
                    LOGW("[MM MTCMOS Close in sleep]\n");
                    mtcmos_mm();
                } else if (input_2 == 2) {
                    LOGW("[MD2G MTCMOS Close in sleep]\n");
                    mtcmos_md2g();
                }
                break;
        }
        response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
        response.response_len = strlen((const char *) response.response_buf);
    }else if (strstr((char *) parse_cmd->string_ptr, "AT+SM=1") != NULL) {
        if (strstr((char *) parse_cmd->string_ptr, "AT+SM=1,0") != NULL) {
            LOGW("[Eint Setting] \n");
            sleep_driver_fast_wakeup_eint_setting(HAL_GPIO_4, HAL_EINT_NUMBER_3);
        } else if (strstr((char *) parse_cmd->string_ptr, "AT+SM=1,1") != NULL) {
            LOGW("[EVB Eint Setting] \n");
            sleep_driver_fast_wakeup_eint_setting(HAL_GPIO_3, HAL_EINT_NUMBER_14);
        } else if (strstr((char *) parse_cmd->string_ptr, "AT+SM=1,2") != NULL) {
        } else if (strstr((char *) parse_cmd->string_ptr, "AT+SM=1,3") != NULL) {
            LOGW("[Fast_wakeup]\n");
            FAST_WAKEUP_VERIFICATION = true;
            cm_bus_clk_26m();
            PMIC_VCORE_VOSEL_CONFIG(PMIC_PSI_HP, PMIC_VCORE_0P9V);
            PMIC_VCORE_VOSEL_CONFIG(PMIC_PSI_LP, PMIC_VCORE_0P9V);

            *((volatile uint32_t *) (0xA2020C18)) = 0xF0000000;
            *((volatile uint32_t *) (0xA2020004)) = 0x00008000;
            *((volatile uint32_t *) (0xA2020304)) = 0x00008000; //high
            hal_sleep_manager_unlock_sleep(at_cmd_lock_sleep_handle);
        } else if (strstr((char *) parse_cmd->string_ptr, "AT+SM=1,4") != NULL) {
            LOGW("[Normail_wakeup] \n");
            FAST_WAKEUP_VERIFICATION = true;
            //cm_bus_clk_26m();
            PMIC_VCORE_VOSEL_CONFIG(PMIC_PSI_HP, PMIC_VCORE_1P1V);
            PMIC_VCORE_VOSEL_CONFIG(PMIC_PSI_LP, PMIC_VCORE_1P1V);
            *((volatile uint32_t *) (0xA2020C18)) = 0xF0000000;
            *((volatile uint32_t *) (0xA2020004)) = 0x00008000;
            *((volatile uint32_t *) (0xA2020304)) = 0x00008000; //high

            hal_sleep_manager_unlock_sleep(at_cmd_lock_sleep_handle);
        }
    } else if (strstr((char *) parse_cmd->string_ptr, "AT+SM=2") != NULL) {
        if (strstr((char *) parse_cmd->string_ptr, "AT+SM=2,0") != NULL) {
            sys_topsm_debug_option(false);
            cm4_topsm_debug_option(false);
        } else if (strstr((char *) parse_cmd->string_ptr, "AT+SM=2,1") != NULL) {
            sys_topsm_debug_option(true);
            cm4_topsm_debug_option(true);
        } else {
            LOGW("[Error Command \n");
        }
        response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
        response.response_len = strlen((const char *)response.response_buf);
    } else if (strstr((char *) parse_cmd->string_ptr, "AT+SM=3") != NULL) {
        LOGW("=Show MTCMOS Status ==\n");
        sys_topsm_debug_log(HAL_SYS_TOPSM_MTCMOS_DEBUG);
        response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
        response.response_len = strlen((const char *)response.response_buf);
    } else if (strstr((char *) parse_cmd->string_ptr, "AT+SM=4") != NULL) {
        /*Sleep 15s*/
        if (strstr((char *) parse_cmd->string_ptr, "AT+SM=4,0") != NULL) {
            LOGW("=enter deep sleep ==\n");
            hal_cm4_topsm_register_suspend_cb(suspend_call_back_test, NULL);
            hal_cm4_topsm_register_resume_cb(resume_call_back_test, NULL);
            atsm_mask_pri = save_and_set_interrupt_mask();
            hal_sleep_manager_set_sleep_time(15000);
            cm4_topsm_enter_suspend(HAL_SLEEP_MODE_DEEP_SLEEP);
            vPortSetupTimerInterrupt();
            atci_deinit(HAL_UART_1); //if trun off infra power,after need reinit uart1(DMA mode).
            atci_init(HAL_UART_1);
            restore_interrupt_mask(atsm_mask_pri);
        } else if (strstr((char *) parse_cmd->string_ptr, "AT+SM=4,1") != NULL) {
            LOGW("=enter sleep ==\n");
            hal_cm4_topsm_register_suspend_cb(suspend_call_back_test, NULL);
            hal_cm4_topsm_register_resume_cb(resume_call_back_test, NULL);
            atsm_mask_pri = save_and_set_interrupt_mask();
            hal_sleep_manager_set_sleep_time(15000);
            cm4_topsm_enter_suspend(HAL_SLEEP_MODE_SLEEP);
            vPortSetupTimerInterrupt();
            atci_deinit(HAL_UART_1); //if trun off infra power,after need reinit uart1(DMA mode).
            atci_init(HAL_UART_1);
            restore_interrupt_mask(atsm_mask_pri);
        } else {
            LOGW("[Error Command ; Deep-sleep is AT+SM=4,0]\n");
        }
    } else if (strstr((char *) parse_cmd->string_ptr, "AT+SM=5") != NULL) {
        /*Power off*/
        hal_sleep_manager_enter_power_off_mode();
        response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
        response.response_len = strlen((const char *)response.response_buf);
    } else if (strstr((char *) parse_cmd->string_ptr, "AT+SM=6") != NULL) {
        if (strstr((char *) parse_cmd->string_ptr, "AT+SM=6,0") != NULL) {
            hal_sleep_manager_lock_sleep(at_cmd_lock_sleep_handle);
        }
        if (strstr((char *) parse_cmd->string_ptr, "AT+SM=6,1") != NULL) {
            hal_sleep_manager_unlock_sleep(at_cmd_lock_sleep_handle);
        }
        response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
        response.response_len = strlen((const char *)response.response_buf);
    } else if (strstr((char *) parse_cmd->string_ptr, "AT+SM=8") != NULL) {
        if (strstr((char *) parse_cmd->string_ptr, "AT+SM=8,0") != NULL) {
            LOGW("mtcmos_bt ==\n");
            mtcmos_bt();
        }else if (strstr((char *) parse_cmd->string_ptr, "AT+SM=8,1") != NULL) {
            LOGW("=mtcmos_mm ==\n");
            mtcmos_mm();
        }else if (strstr((char *) parse_cmd->string_ptr, "AT+SM=8,2") != NULL) {
            LOGW("=mtcmos_md2g ==\n");
            mtcmos_md2g();
        }else {
            LOGW("[Error Command]\n");
        }


        response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
        response.response_len = strlen((const char *)response.response_buf);
    } else if (strstr((char *) parse_cmd->string_ptr, "AT+SM=9") != NULL) {
        LOGW("[PMU LDO Setting]\n");
        end_pos = strchr(parse_cmd->string_ptr, ',');
        end_pos ++;
        input_value = atoi(end_pos);
        end_pos = NULL;

        if (reload_pmu_power_mode_table(input_value) == 0) {
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
            response.response_len = 0;
        } else {
            strcpy((char *)response.response_buf, "ERROR\r\n");
            response.response_len = strlen((const char *)response.response_buf);
        }
    } else {
        LOGW("=error command==\n");
        response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
        response.response_len = strlen((const char *)response.response_buf);
    }

    atci_send_response(&response);
    return ATCI_STATUS_OK;
}

atci_status_t atci_cmd_hdlr_freq_test(atci_parse_cmd_param_t *parse_cmd)
{
    LOGI("===Sleep Manager freq test AT Command====\n");

    response.response_flag = 0; // Command Execute Finish.
    if (strstr((char *) parse_cmd->string_ptr, "AT+FREQTEST=208") != NULL) {
        PSI_LP_RUN_AT_HP(SLEEP_PSI_MASTER_SW);
        PMIC_VCORE_VOSEL_CONFIG(PMIC_PSI_HP, PMIC_VCORE_1P3V);
        cm_bus_clk_208m();
        response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
        response.response_len = strlen((const char *)response.response_buf);
    } else if (strstr((char *) parse_cmd->string_ptr, "AT+FREQTEST=104") != NULL) {
        /* PSI init mode is LP */
        PMIC_VCORE_VOSEL_CONFIG(PMIC_PSI_LP, PMIC_VCORE_1P1V);
        cm_bus_clk_104m();
        response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
        response.response_len = strlen((const char *)response.response_buf);
    } else if (strstr((char *) parse_cmd->string_ptr, "AT+FREQTEST=26") != NULL) {
        /* PSI init mode is LP */
        cm_bus_clk_26m();
        PMIC_VCORE_VOSEL_CONFIG(PMIC_PSI_LP, PMIC_VCORE_0P9V);
        response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
        response.response_len = strlen((const char *)response.response_buf);
    } else {
        LOGE("atci_cmd_hdlr_freq_test , error code\n");
        /* others are invalid command format */
        strcpy((char *)response.response_buf, "ERROR\r\n");
        response.response_len = strlen((const char *)response.response_buf);
    }

    atci_send_response(&response);

    return ATCI_STATUS_OK;
}

static unsigned char pmu_power_table_0[10][8] = {
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, 0                      , 0	, 0	},	//VCLDO
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, 0                      , 0	, 0	},	//VIO18
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_ULP	, PMIC_LDO_PMOD_ULP	, PMIC_SLP_WKUP_SW_CTL	, 0	, 0	},	//VSF
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 0	, 0	},	//VIO28
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_LP       , PMIC_LDO_PMOD_LP       , PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, 0                      , 0	, 0	},	//VDIG18
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_HW_CTL	, 0	, 0	},	//VA18
    {	PMIC_VR_CTL_DISABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 0	, 0	},	//VUSB
    {	PMIC_VR_CTL_DISABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 2	, 2	},	//VBT
    {	PMIC_VR_CTL_DISABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 1	, 1	},	//VA28
    {	PMIC_VR_CTL_DISABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 0	, 0	}	//VMC
};

static unsigned char pmu_power_table_1[10][8] = {
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_ULP	, 0                      , 0	, 0	},	//VCLDO
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, 0                      , 0	, 0	},	//VIO18
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_ULP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 0	, 0	},	//VSF
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 0	, 0	},	//VIO28
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_LP       , PMIC_LDO_PMOD_LP       , PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, 0                      , 0	, 0	},	//VDIG18
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_HW_CTL	, 0	, 0	},	//VA18
    {	PMIC_VR_CTL_DISABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 0	, 0	},	//VUSB
    {	PMIC_VR_CTL_DISABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 2	, 2	},	//VBT
    {	PMIC_VR_CTL_DISABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 1	, 1	},	//VA28
    {	PMIC_VR_CTL_DISABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 0	, 0	}	//VMC
};

static unsigned char pmu_power_table_2[10][8] = {
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, 0                      , 0	, 0	},	//VCLDO
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_ULP	, 0                      , 0	, 0	},	//VIO18
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_ULP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 0	, 0	},	//VSF
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 0	, 0	},	//VIO28
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_LP       , PMIC_LDO_PMOD_LP       , PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, 0                      , 0	, 0	},	//VDIG18
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_HW_CTL	, 0	, 0	},	//VA18
    {	PMIC_VR_CTL_DISABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 0	, 0	},	//VUSB
    {	PMIC_VR_CTL_DISABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 2	, 2	},	//VBT
    {	PMIC_VR_CTL_DISABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 1	, 1	},	//VA28
    {	PMIC_VR_CTL_DISABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 0	, 0	}	//VMC
};

static unsigned char pmu_power_table_3[10][8] = {
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, 0                      , 0	, 0	},	//VCLDO
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, 0                      , 0	, 0	},	//VIO18
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_ULP	, PMIC_LDO_PMOD_ULP	, PMIC_SLP_WKUP_SW_CTL	, 0	, 0	},	//VSF
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 0	, 0	},	//VIO28
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_LP       , PMIC_LDO_PMOD_LP       , PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, 0                      , 0	, 0	},	//VDIG18
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_HW_CTL	, 0	, 0	},	//VA18
    {	PMIC_VR_CTL_DISABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 0	, 0	},	//VUSB
    {	PMIC_VR_CTL_DISABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 2	, 2	},	//VBT
    {	PMIC_VR_CTL_DISABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 1	, 1	},	//VA28
    {	PMIC_VR_CTL_DISABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 0	, 0	}	//VMC
};

static unsigned char pmu_power_table_4[10][8] = {
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, 0                      , 0	, 0	},	//VCLDO
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, 0                      , 0	, 0	},	//VIO18
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_ULP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 0	, 0	},	//VSF
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_ULP	, PMIC_SLP_WKUP_SW_CTL	, 0	, 0	},	//VIO28
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_LP       , PMIC_LDO_PMOD_LP       , PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, 0                      , 0	, 0	},	//VDIG18
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_HW_CTL	, 0	, 0	},	//VA18
    {	PMIC_VR_CTL_DISABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 0	, 0	},	//VUSB
    {	PMIC_VR_CTL_DISABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 2	, 2	},	//VBT
    {	PMIC_VR_CTL_DISABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 1	, 1	},	//VA28
    {	PMIC_VR_CTL_DISABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 0	, 0	}	//VMC
};

static unsigned char pmu_power_table_5[10][8] = {
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, 0                      , 0	, 0	},	//VCLDO
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, 0                      , 0	, 0	},	//VIO18
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_ULP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 0	, 0	},	//VSF
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 0	, 0	},	//VIO28
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_LP       , PMIC_LDO_PMOD_LP       , PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_ULP	, 0                      , 0	, 0	},	//VDIG18
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_HW_CTL	, 0	, 0	},	//VA18
    {	PMIC_VR_CTL_DISABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 0	, 0	},	//VUSB
    {	PMIC_VR_CTL_DISABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 2	, 2	},	//VBT
    {	PMIC_VR_CTL_DISABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 1	, 1	},	//VA28
    {	PMIC_VR_CTL_DISABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 0	, 0	}	//VMC
};

static unsigned char pmu_power_table_6[10][8] = {
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, 0                      , 0	, 0	},	//VCLDO
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, 0                      , 0	, 0	},	//VIO18
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_ULP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 0	, 0	},	//VSF
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 0	, 0	},	//VIO28
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_LP       , PMIC_LDO_PMOD_LP       , PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, 0                      , 0	, 0	},	//VDIG18
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_ULP	, PMIC_SLP_WKUP_HW_CTL	, 0	, 0	},	//VA18
    {	PMIC_VR_CTL_DISABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 0	, 0	},	//VUSB
    {	PMIC_VR_CTL_DISABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 2	, 2	},	//VBT
    {	PMIC_VR_CTL_DISABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 1	, 1	},	//VA28
    {	PMIC_VR_CTL_DISABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 0	, 0	}	//VMC
};

static unsigned char pmu_power_table_7[10][8] = {
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, 0                      , 0	, 0	},	//VCLDO
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, 0                      , 0	, 0	},	//VIO18
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_ULP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 0	, 0	},	//VSF
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 0	, 0	},	//VIO28
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_LP       , PMIC_LDO_PMOD_LP       , PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, 0                      , 0	, 0	},	//VDIG18
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_HW_CTL	, 0	, 0	},	//VA18
    {	PMIC_VR_CTL_DISABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_ULP	, PMIC_SLP_WKUP_SW_CTL	, 0	, 0	},	//VUSB
    {	PMIC_VR_CTL_DISABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 2	, 2	},	//VBT
    {	PMIC_VR_CTL_DISABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 1	, 1	},	//VA28
    {	PMIC_VR_CTL_DISABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 0	, 0	}	//VMC
};

static unsigned char pmu_power_table_8[10][8] = {
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, 0                      , 0	, 0	},	//VCLDO
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, 0                      , 0	, 0	},	//VIO18
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_ULP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 0	, 0	},	//VSF
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 0	, 0	},	//VIO28
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_LP       , PMIC_LDO_PMOD_LP       , PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, 0                      , 0	, 0	},	//VDIG18
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_HW_CTL	, 0	, 0	},	//VA18
    {	PMIC_VR_CTL_DISABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 0	, 0	},	//VUSB
    {	PMIC_VR_CTL_DISABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_ULP	, PMIC_SLP_WKUP_SW_CTL	, 2	, 2	},	//VBT
    {	PMIC_VR_CTL_DISABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 1	, 1	},	//VA28
    {	PMIC_VR_CTL_DISABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 0	, 0	}	//VMC
};

static unsigned char pmu_power_table_9[10][8] = {
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, 0                      , 0	, 0	},	//VCLDO
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, 0                      , 0	, 0	},	//VIO18
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_ULP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 0	, 0	},	//VSF
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 0	, 0	},	//VIO28
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_LP       , PMIC_LDO_PMOD_LP       , PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, 0                      , 0	, 0	},	//VDIG18
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_HW_CTL	, 0	, 0	},	//VA18
    {	PMIC_VR_CTL_DISABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 0	, 0	},	//VUSB
    {	PMIC_VR_CTL_DISABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 2	, 2	},	//VBT
    {	PMIC_VR_CTL_DISABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_ULP	, PMIC_SLP_WKUP_SW_CTL	, 1	, 1	},	//VA28
    {	PMIC_VR_CTL_DISABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 0	, 0	}	//VMC
};

static unsigned char pmu_power_table_10[10][8] = {
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, 0                      , 0	, 0	},	//VCLDO
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, 0                      , 0	, 0	},	//VIO18
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_ULP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 0	, 0	},	//VSF
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 0	, 0	},	//VIO28
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_LP       , PMIC_LDO_PMOD_LP       , PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, 0                      , 0	, 0	},	//VDIG18
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_HW_CTL	, 0	, 0	},	//VA18
    {	PMIC_VR_CTL_DISABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 0	, 0	},	//VUSB
    {	PMIC_VR_CTL_DISABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 2	, 2	},	//VBT
    {	PMIC_VR_CTL_DISABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 1	, 1	},	//VA28
    {	PMIC_VR_CTL_DISABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_ULP	, PMIC_SLP_WKUP_SW_CTL	, 0	, 0	}	//VMC
};

static unsigned char pmu_power_table_11[10][8] = {
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, 0                      , 0	, 0	},	//VCLDO
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, 0                      , 0	, 0	},	//VIO18
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_ULP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 0	, 0	},	//VSF
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 0	, 0	},	//VIO28
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_LP       , PMIC_LDO_PMOD_LP       , PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, 0                      , 0	, 0	},	//VDIG18
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_HW_CTL	, 0	, 0	},	//VA18
    {	PMIC_VR_CTL_DISABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 0	, 0	},	//VUSB
    {	PMIC_VR_CTL_DISABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 2	, 2	},	//VBT
    {	PMIC_VR_CTL_DISABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 1	, 1	},	//VA28
    {	PMIC_VR_CTL_DISABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 0	, 0	}	//VMC
};

static unsigned char pmu_power_table_12[10][8] = {
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_ULP	, 0                      , 0	, 0	},	//VCLDO
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_ULP	, 0                      , 0	, 0	},	//VIO18
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_ULP	, PMIC_LDO_PMOD_ULP	, PMIC_SLP_WKUP_SW_CTL	, 0	, 0	},	//VSF
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_ULP	, PMIC_SLP_WKUP_SW_CTL	, 0	, 0	},	//VIO28
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_LP       , PMIC_LDO_PMOD_LP       , PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_ULP	, 0                      , 0	, 0	},	//VDIG18
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_ULP	, PMIC_SLP_WKUP_HW_CTL	, 0	, 0	},	//VA18
    {	PMIC_VR_CTL_DISABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_ULP	, PMIC_SLP_WKUP_SW_CTL	, 0	, 0	},	//VUSB
    {	PMIC_VR_CTL_DISABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_ULP	, PMIC_SLP_WKUP_SW_CTL	, 2	, 2	},	//VBT
    {	PMIC_VR_CTL_DISABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_ULP	, PMIC_SLP_WKUP_SW_CTL	, 1	, 1	},	//VA28
    {	PMIC_VR_CTL_DISABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_ULP	, PMIC_SLP_WKUP_SW_CTL	, 0	, 0	}	//VMC
};

int reload_pmu_power_mode_table(unsigned int control)
{
    int result = 0;
    pmu_set_register_value(PMU_RG_VRC_CFG_KEY0_ADDR , PMU_RG_VRC_CFG_KEY0_MASK   , PMU_RG_VRC_CFG_KEY0_SHIFT  , 0xFC);
    pmu_set_register_value(PMU_RG_VRC_CFG_KEY1_ADDR , PMU_RG_VRC_CFG_KEY1_MASK   , PMU_RG_VRC_CFG_KEY1_SHIFT  , 0xDA);
    pmu_set_register_value(PMU_RG_PSIOVR_ADDR       , PMU_RG_PSIOVR_MASK         , PMU_RG_PSIOVR_SHIFT        , 1);

    //RG_S0_BGP_LP_EN
    pmu_set_register_value(0x42 , 0x01 , 0x00 , 1);
    //RG_S1_BGP_LP_EN
    pmu_set_register_value(0x42 , 0x01 , 0x01 , 1);

    pmu_set_register_value(PMU_RG_PSIOVR_ADDR       , PMU_RG_PSIOVR_MASK         , PMU_RG_PSIOVR_SHIFT        , 0x0);
    pmu_set_register_value(PMU_RG_VRC_CFG_KEY0_ADDR , PMU_RG_VRC_CFG_KEY0_MASK   , PMU_RG_VRC_CFG_KEY0_SHIFT  , 0x0);
    pmu_set_register_value(PMU_RG_VRC_CFG_KEY1_ADDR , PMU_RG_VRC_CFG_KEY1_MASK   , PMU_RG_VRC_CFG_KEY1_SHIFT  , 0x0);

    switch (control) {
        case 0:
            pmu_init_power_mode(pmu_power_table_0);
            break;
        case 1:
            pmu_init_power_mode(pmu_power_table_1);
            break;
        case 2:
            pmu_init_power_mode(pmu_power_table_2);
            break;
        case 3:
            pmu_init_power_mode(pmu_power_table_3);
            break;
        case 4:
            pmu_init_power_mode(pmu_power_table_4);
            break;
        case 5:
            pmu_init_power_mode(pmu_power_table_5);
            break;
        case 6:
            pmu_init_power_mode(pmu_power_table_6);
            break;
        case 7:
            pmu_init_power_mode(pmu_power_table_7);
            break;
        case 8:
            pmu_init_power_mode(pmu_power_table_8);
            break;
        case 9:
            pmu_init_power_mode(pmu_power_table_9);
            break;
        case 10:
            pmu_init_power_mode(pmu_power_table_10);
            break;
        case 11:
            pmu_init_power_mode(pmu_power_table_11);
            break;
        case 12:
            pmu_init_power_mode(pmu_power_table_12);
            break;

        default:
            result = -1;
            break;
    }

    return (result);
}
