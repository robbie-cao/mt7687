/*
 * OS specific functions for FreeRTOS systems
 * Copyright (c) 2005-2009, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

//#include "includes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>


#include "os.h"

#include "type_def.h"


TEXT_IN_RAM void * os_memcpy(void *dest, const void *src, size_t n)
{
    char *d = dest;
    const char *s = src;
    while (n--) {
        *d++ = *s++;
    }
    return dest;
}

void *os_memmove(void *dest, const void *src, size_t n)
{
    if (dest < src) {
        os_memcpy(dest, src, n);
    } else {
        /* overlapping areas */
        char *d = (char *) dest + n;
        const char *s = (const char *) src + n;
        while (n--) {
            *--d = *--s;
        }
    }
    return dest;
}

TEXT_IN_RAM void * os_memset(void *s, int c, size_t n)
{
    char *p = s;
    while (n--) {
        *p++ = c;
    }
    return s;
}

int os_memcmp(const void *s1, const void *s2, size_t n)
{
    const unsigned char *p1 = s1, *p2 = s2;

    if (n == 0) {
        return 0;
    }

    while (*p1 == *p2) {
        p1++;
        p2++;
        n--;
        if (n == 0) {
            return 0;
        }
    }

    return *p1 - *p2;
}

size_t os_strlen(const char *s)
{
    const char *p = s;
    while (*p) {
        p++;
    }
    return p - s;
}



int os_strcasecmp(const char *s1, const char *s2)
{
    return strcasecmp(s1, s2);
}

int os_strncasecmp(const char *s1, const char *s2, size_t n)
{
    return strncasecmp(s1, s2, n);
}

char *os_strchr(const char *s, int c)
{
    return strchr(s, c);
}

char *os_strrchr(const char *s, int c)
{
    return strrchr(s, c);
}

int os_strcmp(const char *s1, const char *s2)
{
    return strcmp(s1, s2);
}

int os_strncmp(const char *s1, const char *s2, size_t n)
{
    return strncmp(s1, s2, n);
}

char *os_strncpy(char *dest, const char *src, size_t n)
{
    return strncpy(dest, src, n);
}

char *os_strstr(const char *haystack, const char *needle)
{
    return strstr(haystack, needle);
}

int os_snprintf(char *str, size_t size, const char *format, ...)
{
    int sz;

    va_list ap;

    va_start(ap, format);
    sz = vsnprintf(str, size, format, ap);
    va_end(ap);

    return sz;
}


int os_daemonize(const char *pid_file)
{
    return -1;
}


void os_daemonize_terminate(const char *pid_file)
{
}


char *os_rel2abs_path(const char *rel_path)
{
    return NULL; /* strdup(rel_path) can be used here */
}


int os_program_init(void)
{
    return 0;
}


void os_program_deinit(void)
{
}


int os_setenv(const char *name, const char *value, int overwrite)
{
    return -1;
}


int os_unsetenv(const char *name)
{
    return -1;
}


char *os_readfile(const char *name, size_t *len)
{
    return NULL;
}


size_t os_strlcpy(char *dest, const char *src, size_t size)
{
    const char *s = src;
    size_t left = size;

    if (left) {
        /* Copy string up to the maximum size of the dest buffer */
        while (--left != 0) {
            if ((*dest++ = *s++) == '\0') {
                break;
            }
        }
    }

    if (left == 0) {
        /* Not enough room for the string; force NUL-termination */
        if (size != 0) {
            *dest = '\0';
        }
        while (*s++)
            ; /* determine total src string length */
    }

    return s - src - 1;
}



size_t os_memrlen(const void *in_src, size_t in_max_len)
{
    const unsigned char *const ptr = (const unsigned char *) in_src;
    size_t                      i;

    for (i = in_max_len; (i > 0) && (ptr[ i - 1 ] == 0); --i) {}
    return (i);
}



