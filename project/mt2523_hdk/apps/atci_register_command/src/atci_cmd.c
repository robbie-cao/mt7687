// For Register AT command handler
#include "atci.h"
// System head file
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "syslog.h"

//log_create_module(atcmd, PRINT_LEVEL_INFO);
#define LOGE(fmt,arg...)   LOG_E(atcmd, "ATCI project: "fmt,##arg)
#define LOGW(fmt,arg...)   LOG_W(atcmd, "ATCI project: "fmt,##arg)
#define LOGI(fmt,arg...)   LOG_I(atcmd, "ATCI project: "fmt,##arg)
/*
 * sample code
*/

/*--- Function ---*/
atci_status_t atci_cmd_hdlr_testcmd(atci_parse_cmd_param_t *parse_cmd);


/*---  Variant ---*/
atci_cmd_hdlr_item_t item_table[] = {
    {"AT+TESTCMD",         atci_cmd_hdlr_testcmd,         0, 0}
    };

 
void atci_example_init()
{
    atci_status_t ret = ATCI_STATUS_REGISTRATION_FAILURE;

    LOGW("atci_example_init\r\n");
        
    // --------------------------------------------------------- //
    // ------- Test Scenario: register AT handler in CM4 ------- //
    // --------------------------------------------------------- //
    ret = atci_register_handler(item_table, sizeof(item_table) / sizeof(atci_cmd_hdlr_item_t));
    if (ret == ATCI_STATUS_OK) {
		LOGW("at_example_init register success\r\n");
	} else {
		LOGW("at_example_init register fail\r\n");
	}

}

// AT command handler
atci_status_t atci_cmd_hdlr_testcmd(atci_parse_cmd_param_t *parse_cmd)
{
    static int test_param1 = 0;
    atci_response_t response = {{0}};
    char *param = NULL;
    int  param1_val = -1;

    LOGW("atci_cmd_hdlr_testcmd\n");
    
    response.response_flag = 0; // Command Execute Finish.

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:    // rec: AT+TESTCMD=?
            LOGW("AT Test OK.\n");
            strcpy((char *)response.response_buf, "+TESTCMD:(0,1)\r\nOK\r\n");
            response.response_len = strlen((char *)response.response_buf);
            atci_send_response(&response);
            break;

        case ATCI_CMD_MODE_READ:    // rec: AT+TESTCMD?
            LOGW("AT Read done.\n");
            sprintf((char *)response.response_buf,"+TESTCMD:%d\r\n", test_param1);
            response.response_len = strlen((char *)response.response_buf);
            response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;  // ATCI will help append OK at the end of resonse buffer
            atci_send_response(&response);
            break;
            
        case ATCI_CMD_MODE_ACTIVE:  // rec: AT+TESTCMD
            LOGW("AT Active OK.\n");
            // assume the active mode is invalid and we will return "ERROR"
            strcpy((char *)response.response_buf, "+TESTCMD:HELLO.OK\r\n");
            response.response_len = strlen((char *)response.response_buf);
            atci_send_response(&response);
            break;
            
        case ATCI_CMD_MODE_EXECUTION: // rec: AT+TESTCMD=<p1>  the handler need to parse the parameters
            LOGW("AT Executing...\r\n");
            //parsing the parameter
            param = strtok(parse_cmd->string_ptr, ",\n\r");
	        param = strtok(parse_cmd->string_ptr, "AT+TESTCMD=");
            param1_val = atoi(param);
		
	    if (param != NULL && (param1_val == 0 || param1_val == 1)){
                
                // valid parameter, update the data and return "OK"
                response.response_len = 0;
                response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK; // ATCI will help append "OK" at the end of resonse buffer 
            } else {
                // invalide parameter, return "ERROR"
                response.response_len = 0;
                response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR; // ATCI will help append "ERROR" at the end of resonse buffer
            };
            atci_send_response(&response);
			param = NULL;
            break;
            
        default :
            strcpy((char *)response.response_buf, "ERROR\r\n");
            response.response_len = strlen((char *)response.response_buf);
            atci_send_response(&response);
            break;
    }
    return ATCI_STATUS_OK;
}

