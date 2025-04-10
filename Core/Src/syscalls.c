#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>

// Định nghĩa các hàm syscall cần thiết
void _exit(int status)
{
    while(1) {}
}

int _close(int file)
{
    return -1;
}

int _read(int file, char *ptr, int len)
{
    return 0;
}

int _write(int file, char *ptr, int len)
{
    return 0;
}

int _lseek(int file, int ptr, int dir)
{
    return 0;
}

caddr_t _sbrk(int incr)
{
    extern char end asm("end");
    static char *heap_end;
    char *prev_heap_end;

    if (heap_end == 0)
        heap_end = &end;

    prev_heap_end = heap_end;
    heap_end += incr;

    return (caddr_t) prev_heap_end;
} 