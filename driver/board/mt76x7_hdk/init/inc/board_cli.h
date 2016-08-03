
#ifndef __BOARD_CLI_H__
#define __BOARD_CLI_H__

#ifdef __cplusplus
extern "C" {
#endif

uint8_t board_cli_ver(uint8_t len, char *param[]);

uint8_t board_cli_reboot(uint8_t len, char *param[]);

uint8_t board_cli_reg_read(uint8_t len, char *param[]);

uint8_t board_cli_reg_write(uint8_t len, char *param[]);

#define TEST_REG_CLI_ENTRY  { "rr",    "read addr",   board_cli_reg_read,     NULL   }, \
                            { "wr",    "write addr",  board_cli_reg_write,    NULL   },


#define SDK_CLI_ENTRY       { "reboot", "reboot",        board_cli_reboot }, \
                            { "ver",    "f/w ver",       board_cli_ver    },

#define REBOOT_CLI_ENTRY { "reboot", "reboot",        board_cli_reboot },

#ifdef __cplusplus
}
#endif

#endif /* __BOARD_CLI_H__ */

