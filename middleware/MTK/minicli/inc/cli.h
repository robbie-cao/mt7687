/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#ifndef __CLI_H__
#define __CLI_H__

#include <stdint.h>

#include "putter.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 *@addtogroup MiniCLI
 *@{
 * @brief This section introduces the API and usage of MiniCLI.
 *
 * MiniCLI is a CLI engine designed with the footprint in mind for memory
 * and resource constrained systems. The code size is less than 3KB with
 * the support of command history and line editing.
 *
 *
 * @section Terms_Chapter Terms and acronyms
 *
 * |        Terms         |           Details                              |
 * |----------------------|------------------------------------------------|
 * |\b CLI                | Command line interface, a kind of method to interact with users. |
 *
 *
 * @section MiniCLI_HOWTO How to start using MiniCLI
 *
 * To use MiniCLI, two functions and one control block with configuration
 * are needed: \link #cli_init \endlink cli_init and \link #cli_task \endlink .
 * Below is an simple example to illustrate the sequence.
 *
 *    @code
 *      cli_t cli_config = { ... <configuration omitted> ... };
 *
 *      cli_init(&cli_config);
 *
 *      while (1) {
 *          cli_task();
 *      }
 *    @endcode
 *
 * 1. Firstly, the function \link #cli_init \endlink consumes the
 *    configuration before returning.
 * 2. The second step is to call \link #cli_task \endlink which
 *    reads typed characters from user and calls corresponding handler
 *    function that developers have implemented.
 * 3. The function \link #cli_task \endlink repeats until the user logged
 *    out, which may or may not have been implemented by developer.
 * 4. The control is returned to the caller if log out is requested by
 *    the user. This step may never be reached if log out is not implemented.
 *
 * The configuration in this example is omitted and explained in the next
 * section.
 *
 *
 * @section MiniCLI_Configuration Configuration
 *
 * Below is an example of MiniCLI configuration, the actual handlers
 * functions are listed up front: cli_setup_ip, cli_setup_mac, and cli_ver.
 *
 * The command handlers are listed and linked together using C structure
 * \link #cmd_t \endlink . In this example, the array cli_cmds is the root
 * of the commands, that is, there are two commands at the top-level:
 * <b>set</b> and <b>ver</b>. The command <b>set</b> does not work alone. It
 * needs a sub-command. In this example, the sub-command could be <b>ip</b>
 * or <b>mac</b>.
 *
 * If "ver" is supplied on input source (by calling <b>__io_getchar</b> to
 * get them), <b>cli_ver</b> will be called. The version information will be
 * printed to the output using <b>__io_putchar</b>.
 *
 * What is supplied to CLI engine will be parsed into tokens similarly to
 * a typical main function. The only difference is, in a typical <b>main</b>
 * function invokation, the first parameter is the name of the executable.
 *
 * In MiniCLI's case, the parameter <b>len</b> indicates how many tokens
 * were supplied to the handler. If some parameters were supplied by the
 * user, the first parameter on the command line is supplied to the handler
 * in first element of the <b>param</b> array, as can be accessed using
 * <b>param[0]</b>.
 *
 * <b>main()</b> that the parameters to
 *
 *    @code
 *
 *      uint8_t cli_setup_ip(uint8_t len, char *param[])
 *      {
 *          if (len != 1) {
 *              printf("need an IP address\n");
 *              return 0;
 *          }
 *
 *          // call function to process the IP address
 *          sys_setup_ip(param[0]);
 *
 *          return 0;
 *      }
 *
 *      uint8_t cli_setup_mac(uint8_t len, char *param[])
 *      {
 *          if (len != 1) {
 *              printf("need a mac address\n");
 *              return 0;
 *          }
 *
 *          // call function to process the MAC address
 *          sys_setup_mac(param[0]);
 *
 *          return 0;
 *      }
 *
 *      uint8_t cli_ver(uint8_t len, char *param[])
 *      {
 *          printf("version: 2016-05-20\n");
 *          return 0;
 *      }
 *
 *      cmd_t cli_setup[] = {
 *          { "ip",   "ip address",   cli_setup_ip,  NULL },
 *          { "mac",  "mac address",  cli_setup_mac, NULL },
 *          { NULL,   NULL,           NULL,          NULL }
 *      };
 *
 *      cmd_t  cli_cmds[] = {
 *          { "set", "setup",        NULL,    &cli_setup[0] },
 *          { "ver", "show version", cli_ver, NULL          },
 *          { NULL,  NULL,           NULL,    NULL          }
 *      };
 *
 *      static cli_t cli_config = {
 *          .state  = 1,
 *          .echo   = 0,
 *          .get    = __io_getchar,
 *          .put    = __io_putchar,
 *          .knock  = cli_hardcode_login,
 *          .cmd    = cli_cmds
 *      };
 *    @endcode
 *
 * In <b>cli_config</b>, a member <b>state</b> specified the initial
 * authentication status. If <b>state</b> is set to 1, the CLI engine will
 * skip the login function. If it is set to 0, the CLI engine will call
 * <b>cli_hardcode_login</b> with the <b>user id</b> and <b>password</b>
 * and let cli_hardcode_login decide whether the pair of ID and password
 * are allowed to access via CLI.
 *
 * In <b>cli_config</b>, the member <b>echo</b> controls what to be
 * displayed in return if something is received. It is used when typing
 * password. Set to 0 is the default.
 *
 * This section explains the key fields in the configuration structure.
 * For a complete and working example code, search for cli_def.c in the
 * released source code.
 *
 * @note    for the handler functions, implementations should always return 0
 *          for now. MiniCLI may be enhanced in the future to support the
 *          processing of different return values.
 */

/****************************************************************************
 *
 * Constants.
 *
 ****************************************************************************/


/**
 * Controls how many tokens can be parsed in a single command.
 *
 * @note    The current limit of 20 may be not enough for some CLI commands,
 *          however, it is been designed to be fixed to reduce the code size
 *          and reduce the need of dynamic memory allocation.
 */
#define CLI_MAX_TOKENS      (20)


/****************************************************************************
 *
 * Types.
 *
 ****************************************************************************/


/**
 * The function pointer prototype to get a characeter from input source.
 *
 * Generally, standard getchar() can be used for this purpose. But other
 * implementation can also be used. For example, in semihosting mode, the
 * standard getchar() is replaced by debug adapter. But to make CLI work as
 * normal over telnet, a replacement getchar, which talks to telnet, can be
 * used.
 *
 * @note    The standard getch() returns int, which may return someothing
 *          other than a character from user. To reduce the CPU consumption
 *          of mini-CLI, the input source must be configured/written carefully
 *          to not return non-characters.
 */
typedef int    (*getch_fptr)(void);


/**
 * The function pointer prototype to put a characeter to an output target.
 *
 * Generally, standard putchar() can be used for this purpose. But other
 * implementation can also be used. For example, in semihosting mode, the
 * standard putchar() is replaced by debug adapter. But to make CLI work as
 * normal over telnet, a replacement putchar, which talks to telnet, can be
 * used.
 *
 * @note
 */
typedef int    (*putch_fptr)(int);


/**
 * If login is enabled and hardcode is not used. This is the callback function
 * that MiniCLI calls to authenicate the user.
 *
 * @retval  0   if validation of the combination of 'id' and 'pass' failed.
 *              other values if succeeded.
 */
typedef uint8_t (*knock_fptr)(char *id, char *pass);


/**
 * Function pointer type of CLI command handlers.
 *
 * All CLI handlers must follow this prototype. An example is cli_logout().
 *
 * @note    Currently, the return value of CLI handlers are ignore. However,
 *          to keep backward compatiblity, CLI handlers must return 0.
 *
 */
typedef uint8_t (*fp_t)(uint8_t len, char *param[]);


/**
 * Forward declare the type of cmd_t such that ancient compilers won't
 * complain, see \link #cmd_s \endlink for more information.
 */
typedef struct cmd_s cmd_t;


/**
 * @brief The Mini CLI command declaration structure.
 *
 * The strcut cmd_s is defined to aggregate the function pointer, the help
 * messsage and sub commands for Mini CLI engine.
 *
 * @warning To reduce code size, duplicate commands are not detected by Mini
 *          CLI engine. It is developer's task to ensure there is no
 *          duplication in the command declaration.
 */
struct cmd_s {
    char    *cmd;   ///< The string of the command.
    char    *help;  ///< The help message if user type '?' to ask for help.
                    ///< @note Prefixing an ASCII value 01 to this help
                    ///<       message will effectively make this command
                    ///<       hidden. User won't see this command with '?'.
    fp_t    fptr;   ///< The function pointer to call when a match to this
                    ///< command is found by Mini CLI.
    cmd_t   *sub;   ///< Sub-commands.
};


#if !defined(CLI_DISABLE_LINE_EDIT) && !defined(CLI_DISABLE_HISTORY)
/**
 * Forward declare the type of cli_history_t such that ancient compilers
 * won't complain, see \link #cli_history_s \endlink for more information.
 */
typedef struct cli_history_s cli_history_t;

/**
 * The Mini CLI control block definition.
 *
 * The strcut cli_history_s and its aliased type cli_history_t is defined to aggregate the
 * parameters for Mini CLI to work correctly.
 */
struct cli_history_s {
    /**
     * The pointers to <i>history_max</i> lines of buffer for input.
     */
    char			**history;

    /**
     * used to save the current input before pressing "up" the first time
     */
    char            *input;

    /**
     * In cli.c, _cli_do_cmd() will result in the history command to be parsed
     * into tokens, therefore we need an additional array to be as the parameter
     * for _cli_do_cmd() for the history command to be stay unchanged
     */
    char            *parse_token;

    /**
     * The lines of <i>history</i>. Each line must share the sized as
     * line_max to be able to take care
     */
    uint16_t	    history_max;

    /**
     * The size of one history <i>line</i>.
     */
    uint16_t	    line_max;

    /**
     * when the user types a command and presses enter, index
     * is the index that will be saved in
     */
    uint16_t	    index;

    /**
     * when the user presses up/down, position is the history index
     * that the terminal now shows
     */
    uint16_t        position;

    /**
     * Represent whether the history commands have exceed HISTORY_LINES
     * if full == 0, history commands are not full, else full
     */
    uint8_t         full;
};
#endif /* !CLI_DISABLE_LINE_EDIT && !CLI_DISABLE_HISTORY */


/**
 * Forward declare the type of cli_t such that ancient compilers won't
 * complain, see \link #cli_s \endlink for more information.
 */
typedef struct cli_s cli_t;

/**
 * The Mini CLI control block definition.
 *
 * The strcut cli_s and its aliased type cli_t is defined to aggregate the
 * parameters for Mini CLI to work correctly.
 */
struct cli_s {
    /**
     * The variable for Mini CLI login support.
     * 0 if not logged in. 1 if logged in.
     */
    uint8_t         state;

    /**
     * The command tree for CLI commands. The pointer assigned here is the
     * root of all other commands.
     */
    cmd_t           *cmd;

    /**
     * The char to be displayed in place of input character when it is set.
     * This is useful when something like a password is been typed.
     */
    char            echo;

    /**
     * The function pointer to get a characeter from input source.
     */
    getch_fptr      get;

    /**
     * The function pointer to put a characeter to output sink.
     */
    putch_fptr      put;

    /**
     * The authentication function pointer.
     */
    knock_fptr      knock;

    /**
     * The tokenized input command. Note that there is a limit of 20 tokens.
     */
    char            *tok[CLI_MAX_TOKENS];

#if !defined(CLI_DISABLE_LINE_EDIT) && !defined(CLI_DISABLE_HISTORY)
    cli_history_t   history;
#endif /* !CLI_DISABLE_LINE_EDIT && !CLI_DISABLE_HISTORY */
};


/****************************************************************************
 *
 * Function prototypes.
 *
 ****************************************************************************/


/**
 * Mini CLI initialization.
 *
 * The initialization must be done before calling any other Mini CLI
 * functions. To initialize, one parameter containing a declared table and
 * the chains of commands is required. See cli_t for more information.
 *
 * @param cli MiniCLI configuration.
 *
 * @note    the memory pointed by <b>cli</b> must be writable and allocated
 *          solely for the use MiniCLI.
 */
void cli_init(cli_t *cli);


/**
 * Process one line of command in plain text format.
 *
 * @param line a string with command in it.
 */
void cli_line(char *line);


/**
 * Process a tokenized command in array of string pointers format.
 *
 * @param argc  the number of elements in argv[].
 * @PARAM argv  the array of string pointers.
 */
void cli_tokens(uint8_t argc, char *argv[]);


/**
 * The top-level function of the actual CLI.
 *
 * This function will never exit unless the user logged out.
 */
void cli_task(void);


/**
 * The function that implements the logout function.
 *
 * @param   len the number of pointers in <b>param</b>.
 * @param   param the array of pointers pointing to the parameters.
 *
 * @note    Implemented in mini-CLI to reduce the need for mini-CLI users
 *          (developers) because it is a must.
 */
uint8_t cli_logout(uint8_t len, char *param[]);


/**
 * A sample login function implementation.
 *
 * The login ID and password were both hardcoded in source code as '1'.
 *
 * @note Implemented in Mini-CLI as an example to let developers minic and
 *       replace they versions.
 */
uint8_t cli_hardcode_login(char *id, char *pass);


/**
 *@}
 */
#ifdef __cplusplus
}
#endif

#endif /* __CLI_H__ */

