
#include <stdio.h>

#include "syslog.h"
#include "syslog_cli.h"

/****************************************************************************
 *
 * Actual CLI API
 *
 ****************************************************************************/

uint8_t syslog_cli_set_filter(uint8_t len, char *param[])
{
#if !defined (MTK_DEBUG_LEVEL_NONE)

    int i = 0;

    if ( len < 3 || len % 3 ) {
        printf("required parameters: <module_name> <log_switch> <print_level>\n");
        printf("<log_switch>   := on | off\n");
        printf("<print_level>  := info | warning | error\n");
        return 1;
    }

    while (i < len) {
        if (syslog_set_filter(param[i], param[i + 1], param[i + 2], (i == len - 3))) {
            printf("invalid %s %s %s\n", param[i], param[i + 1], param[i + 2]);
            return 2;
        }
        i += 3;
    }

    return 0;

#else

   printf("syslog not supported. check MTK_DEBUG_LEVEL in project's feature.mk\n");
   return 0;

#endif

}

uint8_t syslog_cli_show_config(uint8_t len, char *param[])
{
#if !defined (MTK_DEBUG_LEVEL_NONE)

    syslog_config_t   config;
    int               i   = 0;

    syslog_get_config(&config);

    printf("%s\t%s\t%s\n", "module", "on/off", "level");
    printf("%s\t%s\t%s\n", "------", "------", "-----");

    while (config.filters && config.filters[i] != NULL) {
        printf("%s\t%s\t%s\n",
               config.filters[i]->module_name,
               log_switch_to_str(config.filters[i]->log_switch),
               print_level_to_str(config.filters[i]->print_level));
        i++;
    }

    return 0;

#else

   printf("syslog not supported. check MTK_DEBUG_LEVEL in project's feature.mk\n");
   return 0;

#endif

}

#if defined(MTK_MINICLI_ENABLE)
cmd_t syslog_cli[] = {
    { "set",    "setup filter",  syslog_cli_set_filter,   NULL },
    { NULL,     NULL,            NULL,                    NULL }
};
#endif

