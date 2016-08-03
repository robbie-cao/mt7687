
#include <stdio.h>
#include <stdlib.h>

#include "sie.h"
#include "sie_cli.h"

static uint8_t _sie_unit_test(uint8_t len, char *param[]);

const cmd_t sie_cmds[] = {
    { "test", "unit test", _sie_unit_test, NULL },
    { NULL, NULL, NULL, NULL }
};

static uint8_t _sie_unit_test(uint8_t len, char *param[])
{
    int             i;
    sie_request_t   *request = sie_request_init();
    sie_response_t  *response;
    char            payload[4] = "abcd";
    int             l;
    sie_validate_t  v;

    printf("** create request\n");
    if (NULL == request) {
        printf("request allocation failed\n");
        return 1;
    }

    for (i = 0; i < sizeof(*request); i++) {
        printf("%02x ", ((char *)request)[i] & 0xFF);
    }
    printf("\n");

    printf("** create response\n");
    response = sie_response_init(request, &payload[0], sizeof(payload));
    l        = sizeof(*response) + sizeof(payload);

    if (NULL == response) {
        printf("response allocation failed\n");
        return 2;
    }

    for (i = 0; i < l; i++) {
        printf("%02x ", ((char *)response)[i] & 0xFF);
    }
    printf("\n");

    printf("** decrypt response\n");

    l = sizeof(payload) + sizeof(*response);
    v = sie_response_decrypt(response,
                             l,
                             request);
    printf("result: %s\n", v == SIE_VALIDATE_SUCCESS ? "success" : "failure");

    for (i = 0; i < l; i++) {
        printf("%02x ", ((char *)response)[i] & 0xFF);
    }
    printf("\n");

    sie_request_deinit(request);
    sie_response_deinit(response, l);

    return 0;
}

