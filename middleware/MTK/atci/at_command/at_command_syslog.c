// For Register AT command handler
// System head file

#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "FreeRTOS.h"
#include "task.h"
#include "at_command.h"
#include "syslog.h"
#include "toi.h"

/*--- Function ---*/
atci_status_t atci_cmd_hdlr_syslog(atci_parse_cmd_param_t *parse_cmd);

/*

AT+SYSLOG=?
+SYSLOG:
AT+SYSLOG? Query for the current setting
AT+SYSLOG=<module>,<log_switch>,<print_level> Config module's setting
<log_switch>  := 0 (on) | 1 (off)
<print_level> := 0 (info) | 1 (warning) | 2 (error)
OK

AT+SYSLOG?
+SYSLOG: list of (<module>,<log_switch>,<print_level>)s
OK

AT+SYSLOG=<module>,<log_switch>,<print_level>
OK (or ERROR)

Defined values

<module>: string type; log module name

<log_switch>: integer type;
0 enable the logging for the module
1 disable the logging for the module

<print_level>: integer type; configured log level for the module
0 INFO
1 WARNING
2 ERROR

*/

static void syslog_show_usage(uint8_t *buf)
{
    int pos = 0;

    pos += snprintf((char *)(buf + pos),
                    ATCI_UART_TX_FIFO_BUFFER_SIZE - pos,
                    "%s",
                    "+SYSLOG:\r\n");
    pos += snprintf((char *)(buf + pos),
                    ATCI_UART_TX_FIFO_BUFFER_SIZE - pos,
                    "(AT+SYSLOG?, query for the current setting)\r\n");
    pos += snprintf((char *)(buf + pos),
                    ATCI_UART_TX_FIFO_BUFFER_SIZE - pos,
                    "(AT+SYSLOG=<module>,<log_switch>,<print_level>, config module's setting)\r\n");
    pos += snprintf((char *)(buf + pos),
                    ATCI_UART_TX_FIFO_BUFFER_SIZE - pos,
                    "(<log_switch> = 0|1(meaning on|off), <print_level=0|1|2(meaning I/W/E))\r\n");
}

static void syslog_show_log_config(uint8_t *buf)
{
    syslog_config_t   config;
    int               i = 0, pos = 0;

    pos += snprintf((char *)(buf + pos),
                    ATCI_UART_TX_FIFO_BUFFER_SIZE - pos,
                    "%s",
                    "+SYSLOG:");

    syslog_get_config(&config);
    while (config.filters && config.filters[i] != NULL) {
        pos += snprintf((char *)(buf + pos),
                        ATCI_UART_TX_FIFO_BUFFER_SIZE - pos,
                        "%c(\"%s\",%d,%d)",
                        (i == 0) ? ' ' : ',',
                        config.filters[i]->module_name,
                        config.filters[i]->log_switch,
                        config.filters[i]->print_level);
        i++;
    }
    snprintf((char *)(buf + pos),
             ATCI_UART_TX_FIFO_BUFFER_SIZE - pos,
             "\r\n");
}

static int parse_syslog_execute_command(char *cmd, char **pp1, int *pp2, int *pp3)
{
    uint8_t type;
    uint32_t value;
    char *pch, *p2, *p3;

    if (cmd[0] != '"') {
        return -1;
    }

    pch = cmd + 1;
    *pp1 = pch;
    pch = strchr(pch, '"');

    if (pch != NULL) {
        if ( *( pch + 1) != ',') {
            return -1;
        }
        *pch = 0;
        pch += 2;
        p2 = pch;
    } else {
        return -1;
    }

    pch = strchr(pch, ',');

    if (pch != NULL) {
        *pch = 0;
        pch++;
        p3 = pch;
    } else {
        return -1;
    }

    while (*pch && (*pch != '\n') && (*pch != '\r')) {
        pch++;
    }

    *pch = 0;

    value = toi(p2, &type);
    if (! (type == TOI_DEC && value <= 1) ) {
        return -2;
    }
    *pp2 = value;

    value = toi(p3, &type);
    if (! (type == TOI_DEC && value <= 2) ) {
        return -3;
    }
    *pp3 = value;

    return 0;
}

/* AT command handler  */
atci_status_t atci_cmd_hdlr_syslog(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {{0}, 0};
    char *p1;
    int  p2, p3, ret;

    response.response_flag = 0; /* Command Execute Finish. */

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING: /* rec: AT+SYSLOG=? */
            syslog_show_usage(response.response_buf);
            response.response_len = strlen((char *)response.response_buf);
            response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
            atci_send_response(&response);
            break;

        case ATCI_CMD_MODE_READ:    /* rec: AT+SYSLOG? */
            syslog_show_log_config(response.response_buf);
            response.response_len = strlen((char *)response.response_buf);
            response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
            atci_send_response(&response);
            break;

        case ATCI_CMD_MODE_EXECUTION: /* rec: AT+SYSLOG=<module>,<log_switch>,<print_level> */
            ret = parse_syslog_execute_command(parse_cmd->string_ptr + 10, &p1, &p2, &p3);
            if (ret == 0) {
                if (syslog_at_set_filter(p1, p2, p3, 1) == 0) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                } else {
                    strcpy((char *)response.response_buf, "<module> error\r\n");
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
            } else {
                if (ret == -1) {
                    strcpy((char *)response.response_buf, "command syntax error\r\n");
                } else {
                    snprintf((char *)response.response_buf,
                             ATCI_UART_TX_FIFO_BUFFER_SIZE,
                             "<%s> error\r\n",
                             (ret == -2) ? "log_switch" : "print_level");
                }
                response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
            }
            response.response_len = strlen((char *)response.response_buf);
            atci_send_response(&response);
            break;

        default :
            /* others are invalid command format */
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
            response.response_len = strlen((char *)response.response_buf);
            atci_send_response(&response);
            break;
    }
    return ATCI_STATUS_OK;
}
