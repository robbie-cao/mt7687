
#ifndef __CLI_CLI_H__
#define __CLI_CLI_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


uint8_t cli_cli_fork(uint8_t len, char *param[]);


#define FORK_DESC "create a new task to run the specified command: " \
                  "[stack size in bytes] [task priority] " \
                  "<command line>"


#define CLI_CLI_FORK_ENTRY { "fork", FORK_DESC, cli_cli_fork, NULL },


#ifdef __cplusplus
}
#endif

#endif /* __CLI_CLI_H__ */
