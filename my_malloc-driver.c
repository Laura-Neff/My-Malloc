//
//  my_malloc-driver.c
//  Lab1: Malloc
//


#include "my_malloc.h"
#include <stdio.h> //for printf

int main(int argc, const char * argv[])
{   
    printf("Test 1: Malloc 128 bytes...\n");
    void * ptr1 = my_malloc(128);
    return 0;
}
