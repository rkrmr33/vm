#include <assert.h>    /* assert    */
#include <stdio.h>     /* printf    */
#include <sys/mman.h>  /* mmap      */
#include <sys/stat.h>  /* fstat     */
#include <fcntl.h>     /* O_RDONLY  */
#include <stdlib.h>    /* free      */
#include <string.h>    /* strlen    */

#include "vm_impl.h"   /* vm_method_meta_t */

#include "vm_util.h"

#define FILE_PERM O_RDONLY
#define MAP_PERM PROT_READ

int validate_magic_number(vm_t *instance)
{
    assert(instance && instance->code);

    return (instance->magic_num != read_int_value(instance));
}

int check_main_method(vm_t *instance, 
                      const char *main_method_name, 
                      vm_method_meta_t *method_meta)
{
    assert(instance && main_method_name && method_meta);

    if (0 == strcmp(main_method_name, method_meta->name))
    {
        return open_stack_frame(instance, method_meta);
    }

    return 0;
}

void print_vm_value(vm_value_t *vm_value)
{
    assert(vm_value);

    switch (vm_value->type)
    {
        case VM_TYPE_BYTE:
            printf("{ type: byte, value: %d}\n", vm_value->value.byte_value);
            break;

        case VM_TYPE_INTEGER:
            printf("{ type: int, value: %d}\n", vm_value->value.integer_value);
            break;

        case VM_TYPE_FLOAT:
            printf("{ type: float, value: %f}\n", vm_value->value.float_value);
            break;
        
        case VM_TYPE_LONG:
            printf("{ type: long, value: %ld}\n", vm_value->value.long_value);
            break;

        case VM_TYPE_DOUBLE:
            printf("{ type: double, value: %lf}\n", vm_value->value.double_value);
            break;

        case VM_TYPE_STRING:
            printf("{ type: string, value: \"%s\"}\n", vm_value->value.string_value);
            break;

        case VM_TYPE_REFERENCE:
            printf("{ type: ref, value: %p}\n", vm_value->value.reference_value);
            break;
        
        case VM_TYPE_METHOD:
            printf("{ type: method, value: \"%s\"}\n", vm_value->value.method_value->name);
            break;
    
        default:
            break;
    }
}

void print_error(vm_t *instance, const char *message)
{
    assert(instance && instance->err);

    fprintf(instance->err, "[-] %s\n", message);
}

void print_output(vm_t *instance, const char *message)
{
    assert(instance && instance->output);

    fprintf(instance->output, "%s", message);
}

int read_byte_value(vm_t *instance)
{
    char *reader = NULL;

    assert(instance && instance->code);

    reader = &((char *)instance->code)[instance->ip];
    ++instance->ip;

    return *reader;
}

int read_int_value(vm_t *instance)
{
    int *reader = NULL;
    
    assert(instance);

    reader = &((int *)instance->code)[instance->ip];
    instance->ip += sizeof(int);

    return *reader;
}

vm_instruction_t *read_next_instruction(vm_t *instance)
{
    vm_instruction_t *instruction = NULL;

    assert(instance && instance->code);

    instruction = &instance->instructions[instance->ip];
    ++instance->ip;

    return instruction;
}

char *read_string_value(vm_t *instance)
{
    int str_len = 0;
    char *str = NULL;
    char *reader = NULL;

    assert(instance);

    reader = &instance->code[instance->ip];
    str_len = strlen(reader);
    str = (char *)malloc(sizeof(char) * (str_len + 1));
    if (NULL == str)
    {
        return NULL;
    }
    strcpy(str, reader);
    instance->ip += (str_len + 1);

    return str;
}

vm_value_t *get_local_var(vm_t *instance, int index)
{
    assert(instance);

    return ((vm_value_t *)&instance->stack[instance->sp] - 1 - index);
}

vm_value_t *get_constant_var(vm_t *instance, int index)
{
    assert(instance);

    return &instance->constant_pool[index];
}

int open_stack_frame(vm_t *instance, vm_method_meta_t *method_meta)
{
    unsigned int old_sp = 0;
    vm_stack_trace_t *new_stack_trace = NULL;

    assert(instance && method_meta);

    old_sp = instance->sp;

    instance->sp = instance->osp + method_meta->num_locals;
    instance->stack[instance->sp].type = VM_TYPE_INTEGER;
    instance->stack[instance->sp].value.integer_value = old_sp;

    instance->lap = instance->osp - method_meta->num_params;
    instance->osp = instance->sp + 1;

    new_stack_trace = (vm_stack_trace_t *)malloc(sizeof(vm_stack_trace_t));
    if (NULL == new_stack_trace)
    {
        return -1;
    }
    new_stack_trace->current_method_meta = method_meta;
    new_stack_trace->prev = instance->stack_trace;
    instance->stack_trace = new_stack_trace;

    return 0;
}

int load_bytecode_from_file(const char *file_path, vm_t *instance)
{
    int res = 0, fd = 0;
    size_t file_size = 0;
    struct stat file_stat = {0};

    assert(NULL != file_path);
    assert(NULL != instance);

    fd = open(file_path, FILE_PERM);
    if (-1 == fd)
    {
        print_error(instance, "error: could not open file:");
        print_error(instance, file_path);

        return -1;
    }

    res = fstat(fd, &file_stat);
    if (0 != res)
    {
        print_error(instance, "error: could not read file:");
        print_error(instance, file_path);

        return -1;
    }

    file_size = file_stat.st_size;

    instance->code = (char *)mmap(NULL, file_size, MAP_PERM, MAP_PRIVATE, fd, 0);
    if (MAP_FAILED == instance->code)
    {
        print_error(instance, "error: could not map file:");
        print_error(instance, file_path);

        return -1;
    }
    instance->code_size = file_size;

    return 0;
}

void free_heap(vm_t *instance) 
{
    assert(instance);

    free(instance->heap);
    instance->heap = NULL;
}

void free_stack(vm_t *instance) 
{
    assert(instance);

    free(instance->stack);
    instance->stack = NULL;
}

void free_stack_trace(vm_t *instance)
{
    vm_stack_trace_t *prev = NULL;
    assert(instance);

    while (NULL != instance->stack_trace)
    {
        prev = instance->stack_trace->prev;
        free(instance->stack_trace);
        instance->stack_trace = prev;
    }
}

void free_constant_pool(vm_t *instance) 
{
    vm_value_t cur_value = {0};

    assert(instance);

    for (int i = 0; i < instance->constant_pool_size; ++i)
    {
        cur_value = instance->constant_pool[i];
        if (VM_TYPE_STRING == cur_value.type) // TODO: add reference type support 
        {
            free(cur_value.value.string_value);
            cur_value.value.string_value = NULL;
        }
        else if (VM_TYPE_METHOD == cur_value.type)
        {
            free(cur_value.value.method_value->name);
            cur_value.value.method_value->name = NULL;
            free(cur_value.value.method_value->local_types);
            cur_value.value.method_value->local_types = NULL;
            free(cur_value.value.method_value->param_types);
            cur_value.value.method_value->param_types = NULL;
            free(cur_value.value.method_value);
            cur_value.value.method_value = NULL;
        }
    }

    free(instance->constant_pool);
    instance->constant_pool = NULL;
}

void free_code(vm_t *instance) 
{
    int res = 0;
    assert(instance);

    if (NULL != instance->code) 
    {
        res = munmap(instance->code, instance->code_size);
        if (0 != res)
        {
            print_error(instance, "error: could not unmap file");
        }
    }
    instance->instructions = NULL;
    instance->code = NULL;
}