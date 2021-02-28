//
//  my_malloc.c
//  Lab1: Malloc
//

#include "my_malloc.h"

MyErrorNo my_errno=MYNOERROR;


//This is already implemented in my_malloc.h

// typedef struct freelistnode{
//     struct freelistnode *flink;
//     size_t size; //Don't worry about this -- defined in malloc.h
//     } * FreeListNode; 

FreeListNode head = 0; //This is the head that will be used for linked list
//Make head equal to the address of the linked list

//Cast the free heap beginning to a free list node
    //This is the head
    //Take into account the struct's place in memory


void insert_node(FreeListNode n){
    FreeListNode temp = head; //flag to check last node
    //static void *available_heap_start;
    n->flink = 0; //New node will be last node
    if(head == 0) {
        head = n;
        //head = (FreeListNode) available_heap_start;
        //head->flink = n;
        //n->flink = 0;

    }
    else {
        while(temp->flink != 0){ //List traversal
            temp = temp->flink;
        }
        temp->flink = n;

        //find last node
        //make last node next new node
    }

}

void *find_chunk(uint32_t size) { 
    //returns address of appropriately sized chunk to use

    //In this implementation, I may make it return 0 when it can't find any chunk

}


void *my_malloc(uint32_t size)
{

    static void *available_heap_start;
    void *chunk_start = available_heap_start;
    size += CHUNKHEADERSIZE; //4 extra bytes for bookkeeping?
    if(chunk_start + size > sbrk(0)) {
        void* freeChunk = find_chunk(size);
        if(freeChunk == 0){
            sbrk(8192); //extend heap if necessary -- ask for more if not sufficient later
            available_heap_start += size; //incr. free heap begin
            *((uint32_t*)chunk_start) = size;//store chunk size
            return ((char*)chunk_start)+CHUNKHEADERSIZE;
        }
        else if(freeChunk > size) //Are these variable types equivalent?
        {
                 uint32_t* splitChunkPiece = split_chunk();
                 *((uint32_t*)splitChunkPiece);
                 return ((char*)splitChunkPiece)+CHUNKHEADERSIZE;
                 //Add header bytes?
        }
        else if (freeChunk == size) 
        {
            *((uint32_t*)freeChunk);
            return ((char*)freeChunk)+CHUNKHEADERSIZE;
            //Add header bytes?
        }
        else if (size > 8192) //Make this else-if statement better
        { 
            sbrk(size);
            available_heap_start += size; //incr. free heap begin
            *((uint32_t*)chunk_start) = size;//store chunk size
            return ((char*)chunk_start)+4;
        }
        }

    }

    // if(chunk_start + size > sbrk(0) ){
    //     sbrk(8192); //extend heap if necessary -- ask for more if not sufficient later
    // }

   

    //Must allocate 4 more bytes for magic 

      
void my_free(void *ptr)
{

    //Don't forget to check for valid checksum
    //have address of chunks in increasing order

    

    //ptr points to the beginning of the chunk
    FreeListNode node;
    node = (FreeListNode)ptr; //point n to beginning of chunk
    //Cast ptr as a FreeListNode for p
    FreeListNode p = (FreeListNode) ptr;
    //Find out what chunk's size is through casting
    node->size = (FreeListNode)(p->size);
    //Erase the chunk

    //Perhaps change where flink points to
    //This implementation implies n will be last node in list
    //Is that what we want?
    node->flink = 0; //same as NULL in C
    insert_node(node); //Insert node we erased into free list
}

FreeListNode free_list_begin()
{
    static void *available_heap_start;
    return head; //???
}

void coalesce_free_list()
{
}
