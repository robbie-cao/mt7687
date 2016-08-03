// For Register AT command handler
// System head file

#include "FreeRTOS.h"
#include "task.h"
#include <string.h>
#include <stdio.h>
#include "at_command.h"
#ifdef HAL_PMU_MODULE_ENABLED
#include "hal_pmu_internal.h"
#include "hal_pmu.h"
/*
 * sample code
*/


/*--- Function ---*/
atci_status_t atci_cmd_hdlr_vibrator(atci_parse_cmd_param_t *parse_cmd);

/*
AT+EVIB=<op>	            |   "OK"
AT+EVIB=?                   |   "+EVIB=(0,1)","OK"


*/
/* AT command handler  */
atci_status_t atci_cmd_hdlr_vibrator(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t resonse = {{0}};
    char *param = NULL;
    char param_val;

    printf("atci_cmd_hdlr_vibrator \r\n");

    resonse.response_flag = 0; /*    Command Execute Finish.  */

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:    /* rec: AT+EVIB=?   */
            strcpy((char *)resonse.response_buf, "+EVIB=(\"0: turn off\",\"1: turn on 2.8V\",\"2: turn on 3.0V\",\"3: turn on 3.3V\")\r\nOK\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_EXECUTION: /* rec: AT+ELED=<op>  the handler need to parse the parameters  */
            param = strtok(parse_cmd->string_ptr, ",\n\r");
            param = strtok(parse_cmd->string_ptr, "AT+EVIB=");
            param_val = atoi(param);


            if ((0 != param_val) && (1 != param_val) && (2 != param_val) && (3 != param_val)) {
                strcpy((char *)resonse.response_buf, "ERROR\r\n");
                resonse.response_len = strlen((char *)resonse.response_buf);
                atci_send_response(&resonse);
                break;
            }

            if (0 == param_val) {

		  PMIC_VR_CONTROL(PMIC_VIBR, PMIC_VR_CTL_DISABLE);
				
            }
           else if (1 == param_val) {
                      
                     PMIC_VR_VOSEL_INIT_CONFIG(PMIC_VIBR, PMIC_LDO_2P8V);
                     PMIC_VR_CONTROL(PMIC_VIBR, PMIC_VR_CTL_ENABLE);
               
            }
           else if (2 == param_val) {
                      
                     PMIC_VR_VOSEL_INIT_CONFIG(PMIC_VIBR, PMIC_LDO_3P0V);
                     PMIC_VR_CONTROL(PMIC_VIBR, PMIC_VR_CTL_ENABLE);
               
            }
            else {
                      
                     PMIC_VR_VOSEL_INIT_CONFIG(PMIC_VIBR, PMIC_LDO_3P3V);
                     PMIC_VR_CONTROL(PMIC_VIBR, PMIC_VR_CTL_ENABLE);
               
            }

            strcpy((char *)resonse.response_buf, "OK\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);

            break;

        default :
            /* others are invalid command format */
            strcpy((char *)resonse.response_buf, "ERROR\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;
    }
    return ATCI_STATUS_OK;
}

#endif

