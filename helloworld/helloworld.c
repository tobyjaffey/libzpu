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

#include <stdlib.h>
#include "io.h"

#define WRITEREG(addr, val)     *((volatile int *)(addr)) = val

void main(void)
{
    WRITEREG(ZPU_IO_UART_DATA, 'H');
    WRITEREG(ZPU_IO_UART_DATA, 'e');
    WRITEREG(ZPU_IO_UART_DATA, 'l');
    WRITEREG(ZPU_IO_UART_DATA, 'l');
    WRITEREG(ZPU_IO_UART_DATA, 'o');
    WRITEREG(ZPU_IO_UART_DATA, ' ');
    WRITEREG(ZPU_IO_UART_DATA, 'W');
    WRITEREG(ZPU_IO_UART_DATA, 'o');
    WRITEREG(ZPU_IO_UART_DATA, 'r');
    WRITEREG(ZPU_IO_UART_DATA, 'l');
    WRITEREG(ZPU_IO_UART_DATA, 'd');
    WRITEREG(ZPU_IO_UART_DATA, '\r');
    WRITEREG(ZPU_IO_UART_DATA, '\n');
    __asm("breakpoint");
}

