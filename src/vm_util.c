#include <assert.h>    /* assert    */
#include <stdio.h>     /* printf    */
#include <sys/mman.h>  /* mmap      */
#include <sys/stat.h>  /* fstat     */
#include <fcntl.h>     /* O_RDONLY  */
#include <stdlib.h>    /* free      */
#include <string.h>    /* strlen    */

#include "vm_util.h"

#define FILE_PERM O_RDONLY
#define MAP_PERM PROT_READ

int validate_magic_number(vm_t *instance)
{
    assert(instance && instance->code);

    return instance->magic_num != read_int_value(instance);
}

void print_vm_value(vm_value_t *vm_value)
{
    assert(vm_value);

    switch (vm_value->type)
    {
        case INTEGER_TYPE:
            printf("{ type: int, value: %d}\n", vm_value->value.integer_value);
            break;

        case FLOAT_TYPE:
            printf("{ type: float, value: %f}\n", vm_value->value.float_value);
            break;

        case STRING_TYPE:
            printf("{ type: string, value: \"%s\"}\n", vm_value->value.string_value);
            break;

        case REFERENCE_TYPE:
            printf("{ type: ref, value: %p}\n", vm_value->value.reference_value);
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

int read_int_value(vm_t *instance)
{
    int val = 0;
    
    assert(instance);

    val = *(int *)&instance->code[instance->ip];
    instance->ip += sizeof(int);

    return val;
}

char *read_string_value(vm_t *instance)
{
    int len = 0;
    char *string = NULL;

    assert(instance);

    len = strlen(&instance->code[instance->ip]);
    string = malloc(sizeof(char) * (len + 1));
    if (NULL == string)
    {
        return NULL;
    }
    strcpy(string, &instance->code[instance->ip]);
    instance->ip += (len + 1);

    return string;
}

char read_opcode(vm_t *instance)
{
    assert(instance);

    return instance->code[instance->ip++];
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

void free_constant_pool(vm_t *instance) 
{
    vm_value_t cur_value = {0};

    assert(instance);

    for (int i = 0; i < instance->constant_pool_size; ++i)
    {
        cur_value = instance->constant_pool[i];
        if (STRING_TYPE == cur_value.type) // TODO: add reference type support 
        {
            free(cur_value.value.string_value);
            cur_value.value.string_value = NULL;
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

    instance->code = NULL;
}