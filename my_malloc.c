//
//  my_malloc.c
//  Lab1: Malloc
//

#include "my_malloc.h"

#define MAGIC_NUMBER 333 

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
    n->flink = 0; //New node will be last node
    if(head == 0) {
        head = n;
    return;
    }
    else {
        if(n < head){
            n->flink = head; //Make n's flink point to head
            head = n;
        return;
        }

        while(temp->flink != 0){ //List traversal
            if(n < temp->flink){
                n->flink = temp->flink;
                temp->flink = n;
            return;
            }
            temp = temp->flink;
        }
        temp->flink = n;
        return;
        //find last node
        //make last node next new node
    }

}

void *split_chunk(FreeListNode chonk, uint32_t size) {
   
 FreeListNode temp = head;

     while(temp->size < size){ //List traversal
            if(temp->flink == 0) {
                return 0; //This is a surprise tool that will be helpful later
            }
            temp = temp->flink;
            
    }

    if(chonk->size != size) {

    }

}

void remove_node(FreeListNode chonk) {

FreeListNode temp = head;

     while(temp != 0){ //List traversal
            if(temp->flink == chonk){
                temp->flink = chonk->flink;
            }
            temp = temp->flink;

        }
        return;

}


void *find_chunk(uint32_t size) { 

    // static void *available_heap_start;
    // head = (FreeListNode) available_heap_start;

    FreeListNode temp = head;

    while(temp->size < size){ //List traversal
            if(temp->flink == 0) {
                return 0; //This is a surprise tool that will be helpful later
            }
            temp = temp->flink;
            
    }
    return temp;



    //returns address of appropriately sized chunk to use

    //In this implementation, I may make it return 0 when it can't find any chunk

}


void *my_malloc(uint32_t size)
{

    static void *available_heap_start;
    void *chunk_start = available_heap_start;
    size += CHUNKHEADERSIZE; //4 extra bytes for bookkeeping?
    static void *heap_end;
    heap_end = sbrk(0);


    if(available_heap_start = 0) {
        available_heap_start = sbrk(8192);
        heap_end = sbrk(0);

    }

    void* freeChunk = find_chunk(size);

     if(freeChunk == 0){
        if(chunk_start + size > heap_end) {

            if (size > 8192) //Make this else-if statement better
            { 
                available_heap_start = sbrk(size);


                //This is what gets returned to our malloc call.
                //It's the blueprint of how we structure the memory cells we are allocating

                //********************************************************//
                void *chunk_start = available_heap_start; //Re-assign because chunk_start may have old data
                *((uint32_t*) chunk_start) = size;  //Store the size first in our header
                chunk_start = chunk_start+4; //Size takes up 4 bytes. Now store the MAGIC_NUMBER 4 bytes ahead
                *((uint32_t*) chunk_start) = MAGIC_NUMBER; //Store the MAGIC_NUMBER
                chunk_start = chunk_start+4; //Now move up 4 bytes to get to the meat of our memory cell
                //********************************************************//

                available_heap_start += size + CHUNKHEADERSIZE; //keeps track of how much free heap space we have left

                heap_end = sbrk(0);

                return ((char*)chunk_start);
            }

            else {
                available_heap_start = sbrk(8192); //extend heap if necessary -- ask for more if not sufficient later
                

                *chunk_start = available_heap_start; 
                *((uint32_t*) chunk_start) = size; 
                chunk_start = chunk_start+4;
                *((uint32_t*) chunk_start) = MAGIC_NUMBER; 
                chunk_start = chunk_start+4;
                
                available_heap_start += size + CHUNKHEADERSIZE; //incr. free heap begin

                heap_end = sbrk(0);

                return ((char*)chunk_start);
            }

        }
         else {
            *chunk_start = available_heap_start;
            *((uint32_t*) chunk_start) = size; 
            chunk_start = chunk_start+4;
            *((uint32_t*) chunk_start) = MAGIC_NUMBER; 
             chunk_start = chunk_start+4;

            available_heap_start += size + CHUNKHEADERSIZE; //incr. free heap begin

            return ((char*)chunk_start);

        }

    }

    else if(freeChunk > size) //Are these variable types equivalent?
        {
            uint32_t* splitChunkPiece = split_chunk(size);
                    
            *chunk_start = splitChunkPiece;
            *((uint32_t*) chunk_start) = size; 
            chunk_start = chunk_start+4;
            *((uint32_t*) chunk_start) = MAGIC_NUMBER; 
            chunk_start = chunk_start+4;

            return ((char*)chunk_start);
            //Add header bytes?
        }
    else if (freeChunk == size) 
        {
            *chunk_start = freeChunk;
            *((uint32_t*) chunk_start) = size; 
            chunk_start = chunk_start+4;
            *((uint32_t*) chunk_start) = MAGIC_NUMBER; 
            chunk_start = chunk_start+4;

            return ((char*)chunk_start);
        }


    
    }


      
void my_free(void *ptr)
{

    //Don't forget to check for valid checksum
    //have address of chunks in increasing order

    //valid checksum:
        //Checks header (8 bytes before pointer)
        //Checks checksum in header (the magic number) to see if it is previously allocated chunk
            //if it was, it places the chunk on the free list
            //if not, it doesn't go any further because it wasn't properly allocated chunk
    
    void *tmp = ((char*)ptr)-CHUNKHEADERSIZE;

    FreeListNode temp = 0;

    
    

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
    //static void *available_heap_start;
    return head; 
}

void coalesce_free_list()
{
    FreeListNode temp = head;

     while(temp->flink != 0){ //List traversal

        if(temp + temp->size == temp->flink) { //Means that temp and temp->flink are touching because of how memory is organized
            FreeListNode next = temp->flink;

            temp->flink = next->flink;

            temp->size = temp->size + next->size;

        }
        temp = temp->flink;


        }
    
}
