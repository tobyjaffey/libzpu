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
#include <stdint.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

#include "util.h"
#include "zpu.h"
#include "io.h"

#define RAM_SIZE (2*1024*1024) 

void zpuvm_read(zpuvm_t *vm, uint32_t addr, uint8_t *buf, uint32_t len)
{
    uint8_t *ram = (uint8_t *)vm->userdata;
    memcpy(buf, ram + addr, len);
}

void zpuvm_write(zpuvm_t *vm, uint32_t addr, uint8_t *buf, uint32_t len)
{
    uint8_t *ram = (uint8_t *)vm->userdata;
    memcpy(ram + addr, buf, len);
}

void zpuvm_write_io32(zpuvm_t *vm, uint32_t addr, uint32_t val)
{
    switch(addr)
    {
        case ZPU_IO_UART_DATA:
            printf("%c", val);
            fflush(stdout);
        break;
        default:
            fprintf(stderr, "zpuvm_write_io32 addr=%08X val=%08X\n", addr, val);
        break;
    }
}

uint32_t zpuvm_read_io32(zpuvm_t *vm, uint32_t addr)
{
    switch(addr)
    {
        case ZPU_IO_UART_DATA:
            return vm->uart_in;
        break;
        default:
            fprintf(stderr, "zpuvm_read_io32 addr=%08X\n", addr);
        break;
    }

    return 0x00000000;
}

int main(int argc, char *argv[])
{
    uint8_t *image;
    size_t image_len;
    zpuvm_t vm;
    uint8_t ram[RAM_SIZE];
    fd_set rfds;
    struct timeval tv;
    int rc;

    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <filename.bin>\n", argv[0]);
        return 1;
    }

    if (NULL == (image = file_read(argv[1], &image_len)))
    {
        fprintf(stderr, "Can't read %s\n", argv[1]);
        return 1;
    }

    if (image_len > sizeof(ram))
    {
        fprintf(stderr, "image too big\n");
        return 1;
    }

    memset(ram, 0, sizeof(ram));
    memcpy(ram, image, image_len);

    zpuvm_init(&vm, sizeof(ram), (void *)ram);

    atexit(terminal_reset);
    terminal_init();

    while(1)
    {
        if (!(vm.syscontrol & ZPU_SYSCONTROL_SLEEP))
        {
            if (zpuvm_exec(&vm) < 0)
            {
                fprintf(stderr, "\nStopped\n");
                break;
            }
        }
        else
        {
            FD_ZERO(&rfds);
            FD_SET(0, &rfds);

            tv.tv_sec = 1;
            tv.tv_usec = 0;
            rc = select(1, &rfds, NULL, NULL, &tv);
            if (rc == -1)
            {
                fprintf(stderr, "Select error!\n");
                break;
            }
            else
            if (rc)
            {
                uint8_t c;
                if (1 == read(0, &c, 1))
                {
                    vm.uart_in = c;
                    if (vm.interrupt_config & ZPU_INTERRUPT_CONFIG_UART)
                    {
                        vm.syscontrol &= ~ZPU_SYSCONTROL_SLEEP;          // wake up
                        zpuvm_interrupt(&vm, ZPU_INTERRUPT_CONFIG_UART); // tell vm why
                    }
                }
            }
            else
            {
                // timeout
            }
        }
    }

    terminal_reset();
    return 0;
}

