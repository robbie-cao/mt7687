#include <errno.h>
#include <sys/stat.h>
 
#undef errno
extern int errno;

extern int __io_putchar(int ch) __attribute__((weak));
extern int __io_getchar(void) __attribute__((weak));

int _close(int file)
{
    return -1;
}

int _stat(char *file, struct stat *st)
{
	st->st_mode = S_IFCHR;
	return 0;
}

int _fstat(int file, struct stat *st)
{
	st->st_mode = S_IFCHR;
    return 0;
}

int _isatty(int file)
{
    return 1;
}

int _lseek(int file, int ptr, int dir)
{
    return 0;
}

int _open(const char *name, int flags, int mode)
{
    return -1;
}

int _read(int file, char *ptr, int len)
{
    int DataIdx;
    
    for (DataIdx = 0; DataIdx < len; DataIdx++){
        __io_putchar('+');
        *ptr++ = __io_getchar();
        __io_putchar('-');
    }

    return len;
}

int _write(int file, char *ptr, int len)
{
    int DataIdx;
    
    for (DataIdx = 0; DataIdx < len; DataIdx++){
       __io_putchar( *ptr++ );
    }
    return len;
}

caddr_t _sbrk_r (struct _reent *r, int incr)
{
    extern char   end asm ("end"); /* Defined by the linker.  */
    static char *heap_end;
    char         *prev_heap_end;
    char         *stack_ptr;

    __asm volatile ("MRS %0, msp\n" : "=r" (stack_ptr) );

    if (heap_end == NULL) {
        heap_end = & end;
    }

    prev_heap_end = heap_end;

    if (heap_end + incr > stack_ptr) {
        /* Some of the libstdc++-v3 tests rely upon detecting
        out of memory errors, so do not abort here.  */

        //errno = ENOMEM;
        return (caddr_t) - 1;
    }

    heap_end += incr;

    return (caddr_t) prev_heap_end;
}


int _execve(char *name, char **argv, char **env)
{
	errno = ENOTSUP;
	return -1;
}

int _fork(void)
{
	errno = ENOTSUP;
	return -1;
}
/*
 * * getpid -- only one process, so just return 1.
 * */
#define __MYPID 1
int _getpid()
{
    return __MYPID;
}


/*
 * * kill -- go out via exit...
 * */
int _kill(int pid, int sig)
{
	errno = ENOTSUP;
	return -1;
}

int _wait(int *status)
{
	errno = ENOTSUP;
	return -1;
}

int _exit (int status)
{
	errno = ENOTSUP;
	return -1;
}

int _link(char *old, char *new)
{
	errno = EMLINK;
	return -1;
}

int _unlink(char *name)
{
	errno = EMLINK;
	return -1;
}


#include "FreeRTOS.h"
#include "task.h"
#include <sys/time.h>
#include <sys/times.h>

int _gettimeofday(struct timeval *tv, void *ptz)
{
    int ticks = xTaskGetTickCount();
    if(tv!=NULL) {
        tv->tv_sec = (ticks/1000);
        tv->tv_usec = (ticks%1000)*1000;
        return 0;
    }

    return -1;
}

int _times(struct tms *buf)
{
	errno = ENOTSUP;
	return -1;
}

