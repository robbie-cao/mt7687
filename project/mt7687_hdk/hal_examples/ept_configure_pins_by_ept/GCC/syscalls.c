#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>

extern int __io_putchar(int ch) __attribute__((weak));
extern int __io_getchar(void) __attribute__((weak));


int _close(int file)
{
    return 0;
}

int _fstat(int file, struct stat *st)
{
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
    return 0;
    int DataIdx;

    for (DataIdx = 0; DataIdx < len; DataIdx++) {
        __io_putchar('+');
        *ptr++ = __io_getchar();
        __io_putchar('-');
    }

    return len;
}


int _write(int file, char *ptr, int len)
{
    int i;

    for (i = 0; i < len; i++) {
        __io_putchar( *ptr++ );
    }

    return len;
}


/* Register name faking - works in collusion with the linker.  */
register char *stack_ptr asm ("sp");

caddr_t _sbrk_r (struct _reent *r, int incr)
{
    extern char   end asm ("end"); /* Defined by the linker.  */
    static char *heap_end;
    char *prev_heap_end;

    if (heap_end == NULL) {
        heap_end = & end;
    }

    prev_heap_end = heap_end;

    /* workaround: skip the check if executing in thread mode (psp) */
    if ((heap_end < stack_ptr) && (heap_end + incr > stack_ptr)) {
        /* Some of the libstdc++-v3 tests rely upon detecting
        out of memory errors, so do not abort here.  */
#if 0
        extern void abort (void);

        _write (1, "_sbrk: Heap and stack collision\n", 32);

        abort ();
#else
        //errno = ENOMEM;
        return (caddr_t) - 1;
#endif
    }

    heap_end += incr;

    return (caddr_t) prev_heap_end;
}

void _exit(int __status)
{
    printf("_exit\n");
    while (1) {
        ;
    }
}


int _kill(int pid, int sig)
{
    printf("_kill %d\n", sig);
    return -1;
}


pid_t _getpid(void)
{
    printf("_getpid\n");
    return 0;
}
