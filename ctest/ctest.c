#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "io.h"

static void cons_putc(char c)
{
    volatile int *SYS_PUTC = (volatile int *)ZPU_IO_PUTC;
    *SYS_PUTC = c;
}

int _DEFUN (write, (fd, buf, nbytes), int fd _AND char *buf _AND int nbytes)
{
    int i;
    for (i = 0; i < nbytes; i++)
        cons_putc(buf[i]);
    return nbytes;
}

int fibonacci(int n)
{
    if (n == 0) 
    {
        return 0;
    } 
    if (n == 1)
    {
        return 1;
    }
    return fibonacci(n - 1) + fibonacci(n - 2);
}

int main(void)
{
    int i;
    
    printf("Hello world\n");

    for (i=0;i<20;i++)
        printf("%d ", fibonacci(i));
    printf("\n");

    return 0;
}

void _premain(void)
{
    main();
    fflush(stdout);
    __asm("breakpoint");
}

