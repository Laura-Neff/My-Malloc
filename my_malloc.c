//
//  my_malloc.c
//  Lab1: Malloc
//

#include "my_malloc.h"

MyErrorNo my_errno=MYNOERROR;

typedef struct freelistnode{
    struct freelistnode *flink;
    size_t size; //Don't worry about this -- defined in malloc.h
    } * FreeListNode;



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
    *((uint32_t*)chunk_start) = size;//store chunk size
    return ((char*)chunk_start)+4;

    //Must allocate 4 more bytes for magic 
}
      
void my_free(void *ptr)
{
    //ptr points to the beginning of the chunk
    FreeListNode n;
    n = (FreeListNode)ptr; //point n to beginning of chunk
    //Cast ptr as a FreeListNode for p
    FreeListNode p = (FreeListNode) ptr;
    //Find out what chunk's size is through casting
    n->size = (FreeListNode)(p->size);
    //Erase the chunk

    //Perhaps change where flink points to
    //This implementation implies n will be last node in list
    //Is that what we want?
    n->flink = 0; //same as NULL in C
    insert_node(n); //Insert node we erased into free list
}

FreeListNode free_list_begin()
{
}

void coalesce_free_list()
{
}
