#include <stdio.h>
#include "vm.h"

int main() 
{
    vm_t *new_vm = vm_create("/home/roikramer/Desktop/vm/test/bytecode_test", 0, 0, NULL);

    return 0;
}