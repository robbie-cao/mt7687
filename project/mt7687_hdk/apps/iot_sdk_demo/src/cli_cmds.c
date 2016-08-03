

#include <stdint.h>


#if defined(MTK_MINICLI_ENABLE)


#include "minicli_cmd_table.h"
#include "app_cli_table.h"
#include "wifi_cli.h"


static cli_t *_cli_ptr;

#ifndef GOTO_NORMAL_MODE_CLI_ENTRY
#define GOTO_NORMAL_MODE_CLI_ENTRY
#endif



/****************************************************************************
 *
 * NORMAL MODE
 *
 ****************************************************************************/


static cmd_t   _cmds_normal[] = {
    //GOTO_TEST_MODE_CLI_ENTRY
    //GOTO_EXAMPLE_MODE_CLI_ENTRY
    //MINICLI_NORMAL_MODE_CLI_CMDS
    APP_CLI_CMDS
    { NULL, NULL, NULL, NULL }
};


/****************************************************************************
 *
 * PUBLIC functions
 *
 ****************************************************************************/


void cli_cmds_init(cli_t *cli)
{
    _cli_ptr = cli;
    _cli_ptr->cmd = &_cmds_normal[0];
}


#endif /* #if defined(MTK_MINICLI_ENABLE) */
