#include <stdio.h>
#include "vm.h"

int main(int argc, char *argv[]) 
{
    if (argc < 2)
    {
        puts("[-] missing file name!");
    }
    else 
    {
        vm_t *new_vm = vm_create(argv[1], 0, 0, stdin, stdout, stderr);
        if (NULL != new_vm)
        {
            vm_run(new_vm);
            vm_free(new_vm);
        }
    }

    return 0;
}