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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "zpu.h"
#include "io.h"

// a precompiled Hello world
static const uint8_t image[] =
{
    0x80, 0x3d, 0x0d, 0xf8, 0x80, 0x80, 0x82, 0x80, 0x51, 0x80, 0xc8, 0x71, 0x0c, 0x80, 0xe5, 0x71,
    0x0c, 0x80, 0xec, 0x71, 0x0c, 0x80, 0xec, 0x71, 0x0c, 0x80, 0xef, 0x71, 0x0c, 0xa0, 0x71, 0x0c,
    0x80, 0xd7, 0x71, 0x0c, 0x80, 0xef, 0x71, 0x0c, 0x80, 0xf2, 0x71, 0x0c, 0x80, 0xec, 0x71, 0x0c,
    0x80, 0xe4, 0x71, 0x0c, 0x8d, 0x71, 0x0c, 0x8a, 0x71, 0x0c, 0x00, 0x82, 0x3d, 0x0d, 0x04
};

static uint8_t ram[72];



void zpuvm_read(zpuvm_t *vm, uint32_t addr, uint8_t *buf, uint32_t len)
{
    memcpy(buf, ram + addr, len);
}

void zpuvm_write(zpuvm_t *vm, uint32_t addr, uint8_t *buf, uint32_t len)
{
    memcpy(ram + addr, buf, len);
}

void zpuvm_write_io32(zpuvm_t *vm, uint32_t addr, uint32_t val)
{
    if (addr == ZPU_IO_UART_DATA)
        putchar((char)val);
}

uint32_t zpuvm_read_io32(zpuvm_t *vm, uint32_t addr)
{
    if (addr == ZPU_IO_UART_DATA)
        return vm->uart_in;
    return 0;
}

int main(int argc, char *argv[])
{
    zpuvm_t vm;

    memcpy(ram, image, sizeof(image));
    zpuvm_init(&vm, sizeof(ram), NULL);

    while(0 == zpuvm_exec(&vm))
    {
        // ...
    }

    return 0;
}

