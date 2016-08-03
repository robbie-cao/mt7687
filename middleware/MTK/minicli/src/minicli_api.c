

#include <string.h>

#include "cli.h"


#ifndef MTK_MINICLI_EXAMPLE_LOGIN_ID
#define MTK_MINICLI_EXAMPLE_LOGIN_ID        "a"
#endif

#ifndef MTK_MINICLI_EXAMPLE_LOGIN_PASSWD
#define MTK_MINICLI_EXAMPLE_LOGIN_PASSWD    "a"
#endif


/*
 * enable hardcode login credentials by default
 */
uint8_t cli_hardcode_login(char *id, char *pass)
{
    return (!strcmp(id,   MTK_MINICLI_EXAMPLE_LOGIN_ID) &&
            !strcmp(pass, MTK_MINICLI_EXAMPLE_LOGIN_PASSWD));
}
