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

#include "zpu.h"

#define DBG(...) fprintf(stderr, __VA_ARGS__)

//#define DEBUG_RAM 1
//#define DEBUG_PC 1
//#define DEBUG_OPCODES 1
//#define DEBUG_STACK 1
//#define DEBUG_POP 1

#define ZPU_INTERRUPT_VECTOR    0x00000020

#define ZPU_OP_IM               0x80
#define ZPU_OP_BREAKPOINT       0x00
#define ZPU_OP_PUSHSP           0x02
#define ZPU_OP_POPPC            0x04
#define ZPU_OP_ADD              0x05
#define ZPU_OP_AND              0x06
#define ZPU_OP_OR               0x07
#define ZPU_OP_LOAD             0x08
#define ZPU_OP_NOT              0x09
#define ZPU_OP_FLIP             0x0A
#define ZPU_OP_NOP              0x0B
#define ZPU_OP_STORE            0x0C
#define ZPU_OP_POPSP            0x0D
#define ZPU_OP_ADDSP            0x10
#define ZPU_OP_EMULATE          0x20
#define ZPU_OP_CONFIG           0x3A
#define ZPU_OP_STORESP          0x40
#define ZPU_OP_LOADSP           0x60
#define ZPU_OP_STORESP_MASK     0xE0
#define ZPU_OP_EMULATE_MASK     0xE0
#define ZPU_OP_LOADSP_MASK      0xE0
#define ZPU_OP_ADDSP_MASK       0xF0


void zpuvm_init(zpuvm_t *vm, uint32_t ram_len, void *userdata)
{
    vm->pc = 0;
    vm->idim = 0;
    vm->ram_len = ram_len;
    vm->sp = ram_len;
    vm->interrupt_status = 0;
    vm->error = false;
    vm->userdata = userdata;
}

int32_t zpuvm_read32(zpuvm_t *vm, uint32_t addr)
{
    int32_t val;
    uint8_t buf[4];

    if (addr >= ZPU_IO_BASE)
    {
        switch(addr)
        {
            case ZPU_IO_INTERRUPT_STATUS:
                val = vm->interrupt_status;
            break;
            default:
                val = (int32_t)zpuvm_read_io32(vm, addr);
        }
    }
    else
    {
        if (addr > vm->ram_len-4)
        {
            fprintf(stderr, "MEMFAULT! addr=%08X\n", addr);
            vm->error = true;
            return 0xDEADBEEF;
        }

        zpuvm_read(vm, addr, buf, 4);

#ifdef ZPU_LITTLE_ENDIAN
        val = (buf[0]) |
              (buf[1] <<  8) |
              (buf[2] << 16) |
              (buf[3] << 24);
#else
        val = (buf[0] << 24) |
              (buf[1] << 16) |
              (buf[2] <<  8) |
              (buf[3]);
#endif
    }
#ifdef DEBUG_RAM
    DBG("  rd mem[%08X] (=0x%08X)\n", addr, val);
#endif
    return val;
}

void zpuvm_write32(zpuvm_t *vm, uint32_t addr, int32_t val)
{
    uint8_t buf[4];

#ifdef DEBUG_RAM
    DBG("  mem[%08X]<-%08X\n", addr, val);
#endif
    
#ifdef ZPU_LITTLE_ENDIAN
    buf[0] = val;
    buf[1] = val >>  8;
    buf[2] = val >> 16;
    buf[3] = val >> 24;
#else
    buf[0] = val >> 24;
    buf[1] = val >> 16;
    buf[2] = val >>  8;
    buf[3] = val;
#endif

    if (addr >= ZPU_IO_BASE)
    {
        switch(addr)
        {
            case ZPU_IO_INTERRUPT_STATUS:
                vm->interrupt_status = val;
            break;

            default:
                zpuvm_write_io32(vm, addr, (uint32_t)val);
        }
    }
    else
    {
        if (addr > vm->ram_len-4)
        {
            fprintf(stderr, "MEMFAULT! addr=%08X\n", addr);
            vm->error = true;
        }
        else
        {
            zpuvm_write(vm, addr, buf, 4);
        }
    }
}

int32_t zpuvm_pop(zpuvm_t *vm)
{
    int32_t val;
    if (vm->sp + 4 > vm->ram_len)
    {
        fprintf(stderr, "\nStack underflow!\n");
        vm->error = true;
        return 0xDEADBEEF;
    }
    val = zpuvm_read32(vm, vm->sp);
    vm->sp += 4;
#ifdef DEBUG_POP
    DBG(" popped 0x%08X\n", val);
#endif
    return val;
}

void zpuvm_push(zpuvm_t *vm, int32_t val)
{
    if (vm->sp < 4)
    {
        fprintf(stderr, "\nStack overflow!\n");
        vm->error = true;
        return;
    }
    vm->sp -= 4;
    zpuvm_write32(vm, vm->sp, val);
}

void zpuvm_print_stack(zpuvm_t *vm)
{
    int32_t i;
    int n = 8;

    DBG("[");
    for (i=vm->sp;i<vm->ram_len;i+=4)
    {
        int32_t val;
        zpuvm_read(vm, i, (uint8_t *)&val, 4);
        DBG(" %08X", val);
        n--;
        if (n == 0)
        {
            DBG("...");
            break;
        }
    }
    DBG(" ] (sp=0x%08X)\n", vm->sp);
}

void zpuvm_interrupt(zpuvm_t *vm, uint32_t mask)
{
    vm->interrupt_status |= mask;
    zpuvm_push(vm, vm->pc);
    vm->pc = ZPU_INTERRUPT_VECTOR;
}

int zpuvm_exec(zpuvm_t *vm)
{
    uint8_t opcode;
    int32_t newpc = 0xFFFFFFFF;
    
    if (vm->error)
        return -1;

    if (vm->pc >= vm->ram_len)
    {
        vm->error = true;
        return -1;
    }

    zpuvm_read(vm, vm->pc, &opcode, 1);

#ifdef DEBUG_PC
    DBG("%X: %02X ", vm->pc, opcode);
#ifndef DEBUG_OPCODES
    DBG("\n");
#endif
#endif

#ifdef DEBUG_STACK
    zpuvm_print_stack(vm);
#endif

    vm->pc++;

    if ((opcode & ZPU_OP_IM) == ZPU_OP_IM)
    {
        int8_t val = opcode & ~ZPU_OP_IM;
        if (val & 0x40)
            val = -(128-val);
#ifdef DEBUG_OPCODES
        DBG("IM %d\n", val);
#endif
        if (vm->idim)
        {
            val = opcode & ~ZPU_OP_IM;
            int32_t a = (zpuvm_pop(vm) << 7) + (val);
            zpuvm_push(vm, a);
        }
        else
        {
            zpuvm_push(vm, val);
        }
        vm->idim = true;
    }
    else
    {
        vm->idim = false;

        switch(opcode)
        {
            case ZPU_OP_NOP:
#ifdef DEBUG_OPCODES
                DBG("NOP\n");
#endif
            break;
            case ZPU_OP_CONFIG:
            {
                int32_t id;
#ifdef DEBUG_OPCODES 
                DBG("CONFIG\n");
#endif
                id = zpuvm_pop(vm);
                (void)id;
            }
            break;
            case ZPU_OP_POPPC:
                newpc = zpuvm_pop(vm);
#ifdef DEBUG_OPCODES
                DBG("POPPC 0x%08X\n", newpc);
#endif
            break;
            case ZPU_OP_BREAKPOINT:
#ifdef DEBUG_OPCODES
                DBG("BREAKPOINT\n");
#endif
                vm->error = true;
            break;
            case ZPU_OP_LOAD:
            {
                int32_t a;
#ifdef DEBUG_OPCODES
                DBG("LOAD\n");
#endif
                a = zpuvm_pop(vm);
                if (a & 3)
                {
                    fprintf(stderr, "MEMFAULT\n");
                    vm->error = true;
                }
                zpuvm_push(vm, zpuvm_read32(vm, a & ~3));
            }
            break;
            case ZPU_OP_STORE:
            {
                int32_t addr, val;
#ifdef DEBUG_OPCODES
                DBG("STORE\n");
#endif
                addr = zpuvm_pop(vm);
                val = zpuvm_pop(vm);
                if (addr & 3)
                {
                    fprintf(stderr, "MEMFAULT\n");
                    vm->error = true;
                }
                zpuvm_write32(vm, addr & ~3, val);
            }
            break;
            case ZPU_OP_PUSHSP:
#ifdef DEBUG_OPCODES
                DBG("PUSHSP\n");
#endif
                zpuvm_push(vm, vm->sp);
            break;
            case ZPU_OP_POPSP:
#ifdef DEBUG_OPCODES
                DBG("POPSP\n");
#endif
                vm->sp = zpuvm_pop(vm);
            break;
            case ZPU_OP_ADD:
            {
                int32_t a, b;
                a = zpuvm_pop(vm);
                b = zpuvm_pop(vm);
#ifdef DEBUG_OPCODES
                DBG("ADD %08X %08X\n", a, b);
#endif
                zpuvm_push(vm, a+b);
            }
            break;
            case ZPU_OP_AND:
            {
                int32_t a, b;
#ifdef DEBUG_OPCODES
                DBG("AND\n");
#endif
                a = zpuvm_pop(vm);
                b = zpuvm_pop(vm);
                zpuvm_push(vm, a&b);
            }
            break;
            case ZPU_OP_OR:
            {
                int32_t a, b;
#ifdef DEBUG_OPCODES
                DBG("OR\n");
#endif
                a = zpuvm_pop(vm);
                b = zpuvm_pop(vm);
                zpuvm_push(vm, a|b);
            }
            break;
            case ZPU_OP_NOT:
            {
                int32_t a;
#ifdef DEBUG_OPCODES
                DBG("NOT\n");
#endif
                a = zpuvm_pop(vm);
                zpuvm_push(vm, ~a);
            }
            break;
            case ZPU_OP_FLIP:
            {
                int32_t a, b = 0;
                int i;
#ifdef DEBUG_OPCODES
                DBG("FLIP\n");
#endif
                a = zpuvm_pop(vm);
                for (i=0;i<32;i++)
                {
                    if (a & (1<<i))
                    {
                        b |= (1<<(31-i));
                    }
                }
                zpuvm_push(vm, b);
            }
            break;

            default:
                if ((opcode & ZPU_OP_STORESP_MASK) == ZPU_OP_STORESP)
                {
                    int32_t a;
                    int32_t sp = vm->sp;
                    int32_t offset = ((opcode & ~ZPU_OP_STORESP_MASK) ^ 0x10) * 4;
                    a = zpuvm_pop(vm);
#ifdef DEBUG_OPCODES
                    DBG("STORESP %d @%08X=%08X\n", offset, sp + offset, a);
#endif
                    zpuvm_write32(vm, sp + offset, a);
                }
                else
                if ((opcode & ZPU_OP_LOADSP_MASK) == ZPU_OP_LOADSP)
                {
                    int32_t a;
                    int32_t offset = ((opcode & ~ZPU_OP_LOADSP_MASK) ^ 0x10) * 4;

                    a = zpuvm_read32(vm, vm->sp + offset);
#ifdef DEBUG_OPCODES
                    DBG("LOADSP %d val=0x%08X\n", offset, a);
#endif
                    zpuvm_push(vm, a);
                }
                else
                if ((opcode & ZPU_OP_ADDSP_MASK) == ZPU_OP_ADDSP)
                {
                    int32_t a;
                    int32_t sp = vm->sp;
                    int32_t offset = (opcode & ~ZPU_OP_ADDSP_MASK) * 4;
#ifdef DEBUG_OPCODES
                    DBG("ADDSP %d\n", opcode & ~ZPU_OP_ADDSP_MASK);
#endif
                    a = zpuvm_pop(vm);
                    zpuvm_push(vm, a + zpuvm_read32(vm, sp + offset));
                }
                else
                if ((opcode & ZPU_OP_EMULATE_MASK) == ZPU_OP_EMULATE)
                {
#ifdef DEBUG_OPCODES
                    DBG("EMULATE %d\n", opcode & ~ZPU_OP_EMULATE_MASK);
#endif
                    zpuvm_push(vm, vm->pc);
                    newpc = (opcode & ~ZPU_OP_EMULATE_MASK) * 32;
                }
                else
                {
                    fprintf(stderr, "unknown opcode 0x%02X\n", opcode);
                    vm->error = true;
                    return -1;
                }
            break;
        }
    }

    if (newpc != 0xFFFFFFFF)
        vm->pc = newpc;

    return 0;
}


