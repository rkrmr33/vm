#ifndef VM_H
#define VM_H

#include <stddef.h> /* size_t */

typedef struct vm vm_t;

typedef void (*err_handler)(const char *message);

enum vm_state
{
    VM_INIT,
    VM_READY,
    VM_RUNNING,
    VM_HALT,
    VM_FINISHED
};

vm_t *vm_create(const char *file_path,
                unsigned int stack_size,
                size_t heap_size,
                FILE *output,
                FILE *input,
                FILE *err);

void vm_free(vm_t *instance);

int vm_run(vm_t *instance);

#endif // VM_H