#include <assert.h>    /* assert    */
#include <stdio.h>     /* perror    */
#include <stdlib.h>    /* malloc    */
#include <sys/mman.h>  /* mmap      */
#include <sys/stat.h>  /* fstat     */
#include <fcntl.h>     /* O_RDONLY  */

#include "opcodes.h"   /* opcodes */
#include "vm_impl.h"   /* private vm header */

#include "vm.h"        /* public vm header */

#define FILE_PERM O_RDONLY
#define MAP_PERM PROT_READ

#define DEFAULT_ERR_HANDLER default_err_handler

static int load_bytecode_from_file(const char *file_path, vm_t *instance);
static void default_err_handler(const char *message);
static void print_vm_value(vm_value_t *vm_value);

vm_t *vm_create(const char *file_path,
                unsigned int stack_size,
                size_t heap_size,
                err_handler handler)
{
    int res = 0;
    vm_t *new_instance = NULL;

    assert(NULL != file_path);
    
    new_instance = (vm_t *)malloc(sizeof(vm_t));
    if (NULL == new_instance)
    {
        return NULL;
    }

    if (NULL == handler)
    {
        new_instance->error_handler = DEFAULT_ERR_HANDLER;
    }

    res = load_bytecode_from_file(file_path, new_instance);
    if (0 != res)
    {
        return NULL;
    }

    new_instance->constant_pool_size = *(unsigned int *)new_instance->code;
    printf("[+] constantpool_size: %u\n", new_instance->constant_pool_size);

    new_instance->constant_pool = (vm_value_t *)((int *)new_instance->code + 1);

    print_vm_value(&new_instance->constant_pool[0]);

    return new_instance;
}

void vm_free(vm_t *instance)
{   
    int res = 0;

    res = munmap(instance->code, instance->code_size);
    if (0 != res)
    {
        instance->error_handler("error: could not unmap file");
    }
    instance->code = NULL;

    free(instance);
    instance = NULL;
}

int vm_run(vm_t *instance);


/* STATIC FUNCTIONS */
static int load_bytecode_from_file(const char *file_path, vm_t *instance)
{
    int res = 0, fd = 0;
    size_t file_size = 0;
    struct stat file_stat = {0};

    assert(NULL != file_path);
    assert(NULL != instance);

    fd = open(file_path, FILE_PERM);
    if (-1 == fd)
    {
        instance->error_handler("error: could not open file:");
        instance->error_handler(file_path);

        return -1;
    }

    res = fstat(fd, &file_stat);
    if (0 != res)
    {
        instance->error_handler("error: could not read file:");
        instance->error_handler(file_path);

        return -1;
    }

    file_size = file_stat.st_size;

    instance->code = (char *)mmap(NULL, file_size, MAP_PERM, MAP_PRIVATE, fd, 0);
    if (MAP_FAILED == instance->code)
    {
        instance->error_handler("error: could not map file:");
        instance->error_handler(file_path);

        return -1;
    }
    instance->code_size = file_size;

    return 0;
}

static void default_err_handler(const char *message)
{
    printf("[-] %s\n", message);
}

static void print_vm_value(vm_value_t *vm_value)
{
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