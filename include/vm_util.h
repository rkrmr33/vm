#ifndef VM_UTIL_H
#define VM_UTIL_H

#include "vm_impl.h"

int validate_magic_number(vm_t *instance);

void print_vm_value(vm_value_t *vm_value);

int read_int_value(vm_t *instance);

char *read_string_value(vm_t *instance);

char read_opcode(vm_t *instance);

int load_bytecode_from_file(const char *file_path, vm_t *instance);

void free_heap(vm_t *instance);

void free_stack(vm_t *instance);

void free_constant_pool(vm_t *instance);

void free_code(vm_t *instance);

#endif