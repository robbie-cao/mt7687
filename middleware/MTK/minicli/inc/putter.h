
#ifndef __PUTTER_H__
#define __PUTTER_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Print character c.
 */
void cli_putc(char c);


/**
 * Print a text string per input parameter.
 */
void cli_puts(char *s);


/**
 * Print a decimal value per input parameter.
 */
void cli_putd(int dec);


/**
 * Print a newline.
 */
void cli_putln(void);


/**
 * Print a space character.
 */
void cli_putsp(void);


/**
 * Print a hexadecimal value per input parameter.
 *
 * Print a hexadecimal value per input parameter and the hex characters will
 * be printed in upper case.
 *
 * @todo Not verified.
 */
void cli_putX(uint32_t hex);


/**
 * Print a hexadecimal value per input parameter.
 *
 * Print a hexadecimal value per input parameter and the hex characters will
 * be printed in lower case.
 *
 * @todo Not verified.
 */
void cli_putx(uint32_t hex);


/**
 * Print a hexadecimal value per input parameter.
 *
 * Print a hexadecimal value per input parameter and the hex characters will
 * be printed in lower case with '0x' as prefix.
 *
 * @todo Not verified.
 */
void cli_put0x(uint32_t hex);


/**
 * Print a hexadecimal value per input parameter.
 *
 * Print a hexadecimal value per input parameter and the hex characters will
 * be printed in upper case with '0x' as prefix.
 *
 * @todo Not verified.
 */
void cli_put0X(uint32_t hex);


#ifdef __cplusplus
}
#endif

#endif /* __PUTTER_H__ */
