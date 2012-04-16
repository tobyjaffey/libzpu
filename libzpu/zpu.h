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
#ifndef ZPU_H
#define ZPU_H 1

#include <stdint.h>

#define ZPU_IO_BASE 0x80000000
#define ZPU_IO_USER_BASE            (ZPU_IO_BASE + 0x00000100)

#define ZPU_IO_INTERRUPT_STATUS     (ZPU_IO_BASE+0)
#define ZPU_INTERRUPT_CONFIG        (ZPU_IO_BASE+4)
#define ZPU_SYSCONTROL              (ZPU_IO_BASE+8)

typedef struct
{
    uint32_t pc;
    uint32_t sp;
    uint32_t ram_len;
    bool idim;
    bool error;
    void *userdata;

    uint32_t interrupt_status;
    uint32_t interrupt_config;
    uint32_t syscontrol;    // system control reg
    uint8_t uart_in;        // uart input buffer
} zpuvm_t;

void zpuvm_init(zpuvm_t *vm, uint32_t ram_len, void *userdata);
void zpuvm_print_stack(zpuvm_t *vm);
int zpuvm_exec(zpuvm_t *vm);
void zpuvm_interrupt(zpuvm_t *vm, uint32_t mask);

void zpuvm_read(zpuvm_t *vm, uint32_t addr, uint8_t *buf, uint32_t len);
void zpuvm_write(zpuvm_t *vm, uint32_t addr, uint8_t *buf, uint32_t len);

uint32_t zpuvm_read_io32(zpuvm_t *vm, uint32_t addr);
void zpuvm_write_io32(zpuvm_t *vm, uint32_t addr, uint32_t val);

#endif

