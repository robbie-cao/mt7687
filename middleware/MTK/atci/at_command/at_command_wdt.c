// For Register AT command handler
// System head file

#include "FreeRTOS.h"
#include "task.h"
#include <string.h>
#include <stdio.h>
#include "at_command.h"
#include <stdlib.h>
#ifdef HAL_WDT_MODULE_ENABLED
#include "hal_wdt.h"
/*
 * sample code
*/


/*--- Function ---*/
atci_status_t atci_cmd_hdlr_wdt(atci_parse_cmd_param_t *parse_cmd);

/*
AT+EWDT=<op>                |   "OK"
AT+EWDT=?                   |   "+EWDT=(1)","OK"


*/
/* AT command handler  */
atci_status_t atci_cmd_hdlr_wdt(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t resonse = {{0}};
    char *param = NULL;
    char param_val;
    hal_wdt_config_t wdt_config;

    printf("atci_cmd_hdlr_wdt \r\n");

    resonse.response_flag = 0; /*    Command Execute Finish.  */

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:    /* rec: AT+EWDT=?   */
            strcpy((char *)resonse.response_buf, "+EWDT=(\"1: trigger wdt reset\")\r\nOK\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_EXECUTION: /* rec: AT+EWDT=<op>  the handler need to parse the parameters  */
            param = strtok(parse_cmd->string_ptr, ",\n\r");
            param = strtok(parse_cmd->string_ptr, "AT+EWDT=");
            param_val = atoi(param);


            if (1 != param_val) {
                strcpy((char *)resonse.response_buf, "ERROR\r\n");
                resonse.response_len = strlen((char *)resonse.response_buf);
                atci_send_response(&resonse);
                break;
            }

            if (1 == param_val) {
                wdt_config.mode = HAL_WDT_MODE_RESET;
                wdt_config.seconds = 1;
                hal_wdt_init(&wdt_config);  
                hal_wdt_software_reset();
                while(1); /* waiting the system reboot */
            }

            //strcpy((char *)resonse.response_buf, "OK\r\n");
            //resonse.response_len = strlen((char *)resonse.response_buf);
            //atci_send_response(&resonse);

            break;

        default :
            /* others are invalid command format */
            strcpy((char *)resonse.response_buf, "ERROR Command.\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;
    }
    return ATCI_STATUS_OK;
}

#endif

