#ifndef ZPU_H
#define ZPU_H 1

#include <stdint.h>

#define ZPU_IO_BASE 0x80000000

typedef struct
{
    uint32_t pc;
    uint32_t sp;
    uint32_t ram_len;
    bool idim;
    bool error;
    void *userdata;
} zpuvm_t;

void zpuvm_init(zpuvm_t *vm, uint32_t ram_len, void *userdata);
void zpuvm_print_stack(zpuvm_t *vm);
int zpuvm_exec(zpuvm_t *vm);

void zpuvm_read(zpuvm_t *vm, uint32_t addr, uint8_t *buf, uint32_t len);
void zpuvm_write(zpuvm_t *vm, uint32_t addr, uint8_t *buf, uint32_t len);

uint32_t zpuvm_read_io32(zpuvm_t *vm, uint32_t addr);
void zpuvm_write_io32(zpuvm_t *vm, uint32_t addr, uint32_t val);

#endif

