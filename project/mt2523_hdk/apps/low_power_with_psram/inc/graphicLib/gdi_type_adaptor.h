#ifndef __GDI_TYPE_ADAPTOR_H__
#define __GDI_TYPE_ADAPTOR_H__
#if defined(_MSC_VER)
#else
#include "stdint.h"
#include "stdlib.h"
#endif

#ifndef _MSC_VER
typedef unsigned char   U8;
typedef signed char     S8;
typedef unsigned int    U32;
typedef signed int      S32;
typedef unsigned short  U16;
typedef signed short    S16;
typedef int             MMI_BOOL;
typedef unsigned int kal_uint32;
typedef unsigned short kal_uint16;
typedef unsigned char kal_uint8;

typedef unsigned char kal_bool;

typedef long kal_int32;
typedef short kal_int16;
typedef char kal_int8;
typedef char kal_char;

typedef unsigned short WCHAR;

typedef char BOOL;

#define KAL_TRUE 1
#define KAL_FALSE 0

#ifndef NULL
#define NULL  (void *)0           /*  NULL    :   Null pointer */
#endif

#define TRUE 1
#define FALSE 0

#else
typedef unsigned char   U8;
typedef signed char     S8;
typedef unsigned int    U32;
typedef signed int      S32;
typedef unsigned short  U16;
typedef signed short    S16;
typedef int             MMI_BOOL;
typedef unsigned int kal_uint32;
typedef unsigned short kal_uint16;
typedef unsigned char kal_uint8;

typedef unsigned char kal_bool;

typedef long kal_int32;
typedef short kal_int16;
typedef char kal_int8;
typedef char kal_char;

typedef unsigned short WCHAR;

typedef char BOOL;

#define KAL_TRUE 1
#define KAL_FALSE 0

#ifndef NULL
#define NULL  (void *)0           /*  NULL    :   Null pointer */
#endif

#define TRUE 1
#define FALSE 0



typedef signed int int32_t;
typedef unsigned int uint32_t;
typedef signed short int16_t;
typedef unsigned short uint16_t;
typedef signed char int8_t;
typedef unsigned char uint8_t;

typedef char bool;
#define false 0
#define true 1

#endif

#endif
