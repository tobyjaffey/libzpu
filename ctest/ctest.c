/*
* Copyright (c) 2012, Toby Jaffey <trj-zpu@hodgepig.org>
*
* Permission to use, copy, modify, and distribute this software for any
* purpose with or without fee is hereby granted, provided that the above
* copyright notice and this permission notice appear in all copies.
*
* THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
* WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
* ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
* WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
* ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
* OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/


#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "io.h"

#define WRITEREG(addr, val)     *((volatile int *)(addr)) = val
#define READREG(addr)           *((volatile int *)(addr))

volatile uint8_t uart_in = 0;

void _zpu_interrupt(void)
{
    uint32_t interrupt_status = READREG(ZPU_IO_INTERRUPT_STATUS);

    if (interrupt_status & ZPU_INTERRUPT_CONFIG_UART)
        uart_in = READREG(ZPU_IO_UART_DATA);
}

void sleep_on(uint32_t interrupt_mask)
{
    uint32_t old_config = READREG(ZPU_INTERRUPT_CONFIG);
    WRITEREG(ZPU_INTERRUPT_CONFIG, interrupt_mask);
    WRITEREG(ZPU_SYSCONTROL, ZPU_SYSCONTROL_SLEEP);
    // we're now asleep, zzz...
    WRITEREG(ZPU_INTERRUPT_CONFIG, old_config);
}

int _DEFUN (write, (fd, buf, nbytes), int fd _AND char *buf _AND int nbytes)
{
    int i;
    for (i = 0; i < nbytes; i++)
        WRITEREG(ZPU_IO_UART_DATA, buf[i]);
    return nbytes;
}

int _DEFUN (read, (fd, buf, nbytes), int fd _AND char *buf _AND int nbytes)
{
    sleep_on(ZPU_INTERRUPT_CONFIG_UART);
    buf[0] = uart_in;
    return 1;
}

int fibonacci(int n)
{
    if (n == 0) 
        return 0;
    if (n == 1)
        return 1;
    return fibonacci(n - 1) + fibonacci(n - 2);
}

int main(void)
{
    int i;
    char s[256];

    printf("Hello. What is your name? ");

    fgets(s, sizeof(s), stdin);

    printf("Hi %s\n", s);

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

