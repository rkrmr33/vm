#include <assert.h>    /* assert    */
#include <stdio.h>     /* perror    */
#include <stdlib.h>    /* malloc    */
#include <string.h>    /* strlen    */

#include "opcodes.h"   /* opcodes */
#include "vm_impl.h"   /* private vm header */
#include "vm_util.h"   /* utility functions */

#include "vm.h"        /* public vm header */

#define DEFAULT_ERR_HANDLER default_err_handler
#define MAGIC_NUM 0xBABEFACE
#define DEFAULT_HEAP_SIZE 1000000 // 1mb
#define DEFAULT_STACK_SIZE 100000 // 100kb 

static void default_err_handler(const char *message);
static int init_vm_fields(vm_t *instance, err_handler handler);
static int build_constant_pool(vm_t *instance);

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

    res = init_vm_fields(new_instance, handler);
    if (0 != res)
    {
        vm_free(new_instance);

        return NULL;
    }

    res = load_bytecode_from_file(file_path, new_instance);
    if (0 != res)
    {
        vm_free(new_instance);

        return NULL;
    }

    res = validate_magic_number(new_instance);
    if (0 != res)
    {
        new_instance->error_handler("file with wrong magic number!");
        new_instance->error_handler(file_path);
        vm_free(new_instance);

        return NULL;
    }

    res = build_constant_pool(new_instance);
    if (0 != res) 
    {
        vm_free(new_instance);

        return NULL;
    }

    new_instance->state = VM_READY;

    return new_instance;
}

void vm_free(vm_t *instance)
{   
    assert(instance);

    free_heap(instance);
    free_stack(instance);
    free_constant_pool(instance);
    free_code(instance);

    free(instance);
    instance = NULL;
}

int vm_run(vm_t *instance)
{
    int cur_opcode = 0;
    int res = 0;

    assert(instance);

    if (VM_READY != instance->state)
    {
        instance->error_handler("vm is not at ready state");
    }

    instance->state = VM_RUNNING;

    while (VM_RUNNING == instance->state)
    {
        cur_opcode = read_opcode(instance);
        res = instance->opcode_handlers[cur_opcode](instance);
    }

    return 0;
}


/* STATIC FUNCTIONS */
static int build_constant_pool(vm_t *instance)
{
    char cur_opcode = 0;
    vm_value_t *cur_value = NULL;
    int size = 0;

    assert(instance);

    cur_opcode = read_opcode(instance);

    if (OP_CONST != cur_opcode) {
        return -1;
    }

    instance->constant_pool_size = read_int_value(instance);
    printf("[+] constant_pool_size: %u\n", instance->constant_pool_size);

    instance->constant_pool = (vm_value_t *)malloc(instance->constant_pool_size * sizeof(vm_value_t));
    if (NULL == instance->constant_pool)
    {
        return -1;
    }

    for (int i = 0; i < instance->constant_pool_size; ++i)
    {
        cur_opcode = read_opcode(instance);
        cur_value = &(instance->constant_pool[i]);
        
        switch (cur_opcode)
        {
            case OP_SCONST: // STRING_TYPE constant
                cur_value->type = STRING_TYPE;
                cur_value->value.string_value = read_string_value(instance);
                if (NULL == cur_value->value.string_value)
                {
                    return -1;
                }
                break;
            case OP_ICONST: // INTEGER_TYPE constant
                cur_value->type = INTEGER_TYPE;
                cur_value->value.integer_value = read_int_value(instance);
                break;
            default:
                instance->error_handler("constant_pool_load: unknown type");
                return -1;
        }

        print_vm_value(cur_value);
    }
    
    return 0;
}

static int init_vm_fields(vm_t *instance, err_handler handler)
{
    assert(instance);

    memset(instance, 0, sizeof(vm_t));

    instance->state = VM_INIT;
    instance->magic_num = MAGIC_NUM;
    instance->heap_size = DEFAULT_HEAP_SIZE;
    instance->stack_size = DEFAULT_STACK_SIZE;
    instance->error_handler = (NULL == handler ? DEFAULT_ERR_HANDLER : handler);

    instance->heap = malloc(sizeof(char) * instance->heap_size);
    if (NULL == instance->heap)
    {
        return -1;
    }

    instance->stack = malloc(sizeof(char) * instance->stack_size);
    if (NULL == instance->stack)
    {
        return -1;
    }

    init_opcode_handlers(instance->opcode_handlers);

    return 0;
}

static void default_err_handler(const char *message)
{
    assert(message);

    printf("[-] %s\n", message);
}

