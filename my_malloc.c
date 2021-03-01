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
   
    if(chonk->size > size) { //If the chonk that we want to split is bigger than the space we wanna give a node
        uint32_t remainder = chonk->size - size; //calculate the difference in size of the free list node and the size we want for new node
        FreeListNode newChonk;
        chonk->size = size; //Set the original free list node to the size that we want;
        newChonk->size = remainder; //Set the size of the node to the difference in size of the free list node and the size we want for new node
        insert_node(newChonk); //Insert our new node to OUR FREE LIST of NON-ALLOCATED items.
    }
    return chonk;

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


    FreeListNode temp = head;

    while(temp->size < size){ //List traversal
            if(temp->flink == 0) {
                return 0; //This is a surprise tool that will be helpful later
            }
            temp = temp->flink;
            
    }
    return temp;



    //returns address of appropriately sized chunk to use

    //In this implementation, I make it return 0 when it can't find any chunk

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

    FreeListNode freeChunk = find_chunk(size);

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

    else if(freeChunk->size > size) 
        {
            uint32_t* splitChunkPiece = split_chunk(freeChunk, size);
                    
            *chunk_start = splitChunkPiece;
            *((uint32_t*) chunk_start) = size; 
            chunk_start = chunk_start+4;
            *((uint32_t*) chunk_start) = MAGIC_NUMBER; 
            chunk_start = chunk_start+4;

            return ((char*)chunk_start);
            //Add header bytes?
        }
    else if (freeChunk->size == size) 
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
    //valid checksum:
        //Checks header (8 bytes before pointer)
        //Checks checksum in header (the magic number) to see if it is previously allocated chunk
            //if it was, it places the chunk on the free list
            //if not, it doesn't go any further because it wasn't properly allocated chunk

    if(ptr == 0){
        my_errno = MYBADFREEPTR;
        return 0;
    }
    
    void *tmp = ((char*)ptr)-CHUNKHEADERSIZE;

    uint32_t* size = tmp;
    uint32_t* magic = tmp + 4;

    if(*(magic) != MAGIC_NUMBER) {
        my_errno = MYBADFREEPTR;
        return 0;
    }
   
    //ptr points to the beginning of the chunk
    FreeListNode node;
    node = (FreeListNode)tmp; //point n to beginning of chunk
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
