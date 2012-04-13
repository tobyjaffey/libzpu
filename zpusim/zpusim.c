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
    if (addr == ZPU_IO_PUTC)
        fprintf(stderr, "%c", val);
    else
        fprintf(stderr, "zpuvm_write_io32 addr=%08X val=%08X\n", addr, val);
}

uint32_t zpuvm_read_io32(zpuvm_t *vm, uint32_t addr)
{
    fprintf(stderr, "zpuvm_read_io32 addr=%08X\n", addr);
    return 0x00000000;
}


int main(int argc, char *argv[])
{
    uint8_t *image;
    size_t image_len;
    zpuvm_t vm;
    uint8_t ram[RAM_SIZE];

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

    while(1)
    {
        if (zpuvm_exec(&vm) < 0)
        {
            fprintf(stderr, "\nStopped\n");
            break;
        }
    }
    return 0;
}
