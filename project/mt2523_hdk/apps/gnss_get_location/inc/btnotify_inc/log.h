/** @file log.h
 *
 *  debug log interface file
 *
 */

#ifndef __LOG_H__
#define __LOG_H__


#include <stdio.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

#define LOG_PRINT_LEVEL_INFO  (0)
#define LOG_PRINT_LEVEL_ERROR (1)
#define LOG_PRINT_LEVEL_FATAL (2)
#define LOG_PRINT_LEVEL_OFF   (3)


#define LOG_I(_module, _message, ...) \
    do { \
        printf("[M: %s C: info F: %s L: %d]", (_module), __FUNCTION__, __LINE__); \
        printf((_message), ##__VA_ARGS__); \
        printf("\r\n"); \
    } while (0)


#define LOG_E(_module, _message, ...) \
    do { \
        printf("[M: %s C: error F: %s L: %d]", (_module), __FUNCTION__, __LINE__); \
        printf((_message), ##__VA_ARGS__); \
        printf("\r\n"); \
    } while (0)


#define LOG_E(_module, _message, ...) \
    do { \
        printf("[M: %s C: fatal F: %s L: %d]", (_module), __FUNCTION__, __LINE__); \
        printf((_message), ##__VA_ARGS__); \
        printf("\r\n"); \
    } while (0)


#define log_dump(_module, _message, _data, _length, ...) \
    do { \
        printf("[M: %s F: %s L: %d]", (_module), __FUNCTION__, __LINE__); \
        printf((_message), ##__VA_ARGS__); \
        printf("\r\n"); \
        uint32_t index = 0; \
        for (index = 0; index < ((uint32_t)(_length)); index++) { \
            printf("%02X", (((uint8_t *)(_data))[index])); \
            if ((index+1)%16 == 0) { \
                printf("\r\n"); \
                continue; \
            } \
            if (index+1 != ((uint32_t)(_length))) { \
                printf(" "); \
            } \
        } \
        if (0 != index && 0 != index%16) { \
            printf("\r\n"); \
        } \
        printf("\r\n"); \
    } while (0)



#define COMMON_LOG_PRINT_LEVEL LOG_PRINT_LEVEL_INFO


#if COMMON_LOG_PRINT_LEVEL <= LOG_PRINT_LEVEL_INFO
#define dbg_info(_message, ...) \
    LOG_I"("common", (_message), ##__VA_ARGS__)
#else
#define dbg_info(_message, ...)
#endif


#if COMMON_LOG_PRINT_LEVEL <= LOG_PRINT_LEVEL_ERROR
#define dbg_error(_message, ...) \
    LOG_E("common", (_message), ##__VA_ARGS__)
#else
#define dbg_error(_message, ...)
#endif


#if COMMON_LOG_PRINT_LEVEL <= LOG_PRINT_LEVEL_FATAL
#define dbg_fatal(_message, ...) \
    LOG_E("common", (_message), ##__VA_ARGS__)
#else
#define dbg_fatal(_message, ...)
#endif


#if COMMON_LOG_PRINT_LEVEL <= LOG_PRINT_LEVEL_INFO
#define dbg_dump(_message, _data, _length, ...) \
    log_dump("common", (_message), (_data), (_length), ##__VA_ARGS__)
#else
#define dbg_dump(_message, _data, _length, ...)
#endif





#ifdef __cplusplus
}
#endif


#endif//__LOG_H__

