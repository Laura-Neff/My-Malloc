//
//  my_malloc.c
//  Lab1: Malloc
//

#include "my_malloc.h"

MyErrorNo my_errno=MYNOERROR;

void *find_chunk(uint32_t size) { //basic malloc method
static void *available_heap_start;
void *chunk_start = available_heap_start;
size += 4; //8 extra bytes for bookkeeping?
if(chunk_start + size > sbrk(0) ){
    sbrk(8192); //extend heap if necessary -- ask for more if not sufficient later
}
available_heap_start += size; //incr. free heap begin
*((uint32_t*)chunk_start) = size;  //store chunk size
return ((char*)chunk_start)+4;

//Put an if-statement for if heap doesn't have enough space,
//Look in the free list before system calling like sbrk

//Must allocate 4 more bytes for magic 

}


void *my_malloc(uint32_t size)
{

    static void *available_heap_start;
    void *chunk_start = available_heap_start;
    size += 4; //4 extra bytes for bookkeeping?
    if(chunk_start + size > sbrk(0) ){
        sbrk(8192); //extend heap if necessary -- ask for more if not sufficient later
    }
    available_heap_start += size; //incr. free heap begin
    *((uint32_t*)chunk_start) = size;  //store chunk size
    return ((char*)chunk_start)+4;
}
      
void my_free(void *ptr)
{
}

FreeListNode free_list_begin()
{
}

void coalesce_free_list()
{
}
