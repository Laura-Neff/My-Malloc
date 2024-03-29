//// THIS CODE IS MY OWN WORK, IT WAS WRITTEN WITHOUT CONSULTING
// A TUTOR OR CODE WRITTEN BY OTHER STUDENTS - LAURA NEFF


//  my_malloc.c
//  Lab1: Malloc
//

//#include <stdio.h> //for printf right now for debugging
#include "my_malloc.h"
#include <unistd.h> //for sbrk

#define MAGIC_NUMBER 333 //cause 3 is a magic number!
#define ALIGNMENTBITS 8 //for byte aligning
#define MIN_SBRK 8192 //minimum sbrk() system call allocation size
#define MIN_CHUNK 16 //minimum chunk size for any returned allocated memory
#define FLN_SIZE 12 //the size of a FreeListNode element

MyErrorNo my_errno=MYNOERROR;
FreeListNode head = 0; //This is the head that will be used for linked list
//Make head equal to the address of the linked list

void insert_node(FreeListNode n){
    FreeListNode temp = head; //flag to check last node
    n->flink = 0; //clear out flink just in case
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

FreeListNode split_chunk(FreeListNode chonk, uint32_t size) {
   
    if(chonk->size > size) { //If the chonk that we want to split is bigger than the space we wanna give a node
        uintptr_t remainder = chonk->size - (uintptr_t) size; //calculate the difference in size of the free list node and the size we want for new node
        FreeListNode newChonk = (FreeListNode) ((uintptr_t)chonk+ (uintptr_t)size);
        newChonk->flink = 0;
        chonk->size = (uint32_t) size; //Set the original free list node to the size that we want;
        newChonk->size = (uint32_t) remainder; //Set the size of the node to the difference in size of the free list node and the size we want for new node
        insert_node(newChonk); //Insert our new node to OUR FREE LIST of NON-ALLOCATED items.
    }
    return chonk;

}

void remove_node(FreeListNode chonk) {

    if (head==chonk){ //if the head node is the one to remove
        FreeListNode temp = head;
        temp = temp->flink;
        //hey does this change the line?
        chonk->flink = 0;
        head = temp;
        return;
    }

    FreeListNode temp = head; //for iterating over the list
    while(temp->flink != 0){ //List traversal: check middle nodes and last node
        if(temp->flink == chonk){ //if the next node (including the last node) is the one to remove
            temp->flink = temp->flink->flink; //jump over the next node in the linked list
            chonk->flink = 0;
            return;
        }
        temp = temp->flink; //else, continue to the next node
    }
    return;

}

FreeListNode find_chunk(uint32_t size) { 
    //returns address of appropriately sized chunk to use
    //In this implementation, I make it return 0 when it can't find any chunk


    FreeListNode temp = head;

    if(temp == 0){ //no usable chunk
        return 0;
    }

    while(temp != 0){ //List traversal
        if(temp->size >= size) {
            remove_node(temp);
            return temp;
        }
        if(temp->flink != 0){
            temp = temp->flink;
        } else {
            return 0;
        }
    }
    return 0; //no usable chunk found
}

void *my_malloc(uint32_t size)
{
    if(size ==0){
        return 0;
    }

    static uintptr_t available_heap_start; //beginning of free, never touched portion of heap
    static uintptr_t heap_end; //current end of heap
    size += CHUNKHEADERSIZE; //8 extra bytes for bookkeeping
    if(size % ALIGNMENTBITS != 0){
        size += ALIGNMENTBITS - (size % ALIGNMENTBITS); //align to a multiple of 8
    }

    if(size < MIN_CHUNK){ //ensure we are asking for at least 16 bytes
        size = MIN_CHUNK; 
    }
    
    if(available_heap_start == 0) { //if we have never allocated memory on the heap
        available_heap_start = (uintptr_t) sbrk(MIN_SBRK);
        heap_end = (uintptr_t) sbrk(0);
        if (available_heap_start == 0){ //no memory available
            my_errno = MYENOMEM;
            return 0;
        }
        uintptr_t remaining_free = heap_end - available_heap_start;
        FreeListNode freeChunk = (FreeListNode) (available_heap_start);
        freeChunk->flink = 0;
        freeChunk->size = (uint32_t) remaining_free;
        insert_node(freeChunk);
        available_heap_start += remaining_free;
    }

    FreeListNode freeChunk = find_chunk(size); //check the free list if there is a usable chunk

    uintptr_t chunk_start; //define where we will start the chunk
    if(freeChunk == 0){ //if no usable chunk
    
        //if there isn't enough free heap memory:
        //    A. if we're asking for more than 8KB, sbrk the exact size necessary, otherwise extend by MIN_SBRK
        if ((size + FLN_SIZE) > MIN_SBRK){ //8192 bytes must include allocation size and associated remaining FLN
            available_heap_start = (uintptr_t) sbrk(size);
            heap_end = (uintptr_t) sbrk(0);
        } else {
            available_heap_start = (uintptr_t) sbrk(MIN_SBRK); //extend heap if necessary -- ask for more if not sufficient later
            heap_end = (uintptr_t) sbrk(0);
        }
        if (available_heap_start == 0){ //no memory available
            my_errno = MYENOMEM;
            return 0;
        }
        //now there is usable space on the untouched portion of the heap (continue)
        chunk_start = available_heap_start;
        available_heap_start += (uintptr_t) size; //reduce size of untouched portion of the heap

        //NEW: return free portion to free list
        uintptr_t remaining_free = heap_end - available_heap_start;
        if(remaining_free >= FLN_SIZE){ //should always have at least 12 bytes remaining per above
            freeChunk = (FreeListNode) (available_heap_start);
            freeChunk->flink = 0;
            freeChunk->size = (uint32_t) remaining_free;
            insert_node(freeChunk);
            available_heap_start += remaining_free;
        }

    } else if(freeChunk->size > size) {
        FreeListNode splitChunkPiece = split_chunk(freeChunk, size);
        chunk_start = (uintptr_t) splitChunkPiece;
    } else if (freeChunk->size == size) {
        chunk_start = (uintptr_t) freeChunk;
    } else { //else if the freeChunk is somehow smaller even though we requested a bigger one
        my_errno = MYENOMEM; //something went horribly wrong
        return 0;
    } 
    
    //This is what gets returned from our malloc call.
    //It's the blueprint of how we structure the memory cells we are allocating

    //********************************************************//
    *((uint32_t*) chunk_start) = size;  //Store the size first in our header
    chunk_start = chunk_start+4; //Size takes up 4 bytes. Now store the MAGIC_NUMBER 4 bytes ahead
    *((uint32_t*) chunk_start) = MAGIC_NUMBER; //Store the MAGIC_NUMBER
    chunk_start = chunk_start+4; //Now move up 4 bytes to get to the meat of our memory cell
    //********************************************************//

    return ((char*)chunk_start); //return memory address

} //end my_malloc()
      
void my_free(void *ptr)
{
    //valid checksum:
        //Checks header (8 bytes before pointer)
        //Checks checksum in header (the magic number) to see if it is previously allocated chunk
            //if it was, it places the chunk on the free list
            //if not, it doesn't go any further because it wasn't properly allocated chunk

    if(ptr == 0){
        my_errno = MYBADFREEPTR;
        return;
    }
    
    void *tmp = ((char*)ptr)-CHUNKHEADERSIZE;

    uint32_t size = *((uint32_t*)tmp);
    uint32_t magic = *((uint32_t*)(tmp + 4));

    if(magic != MAGIC_NUMBER) {
        my_errno = MYBADFREEPTR;
        return;
    }
   
    //ptr points to the beginning of the chunk
    FreeListNode node;
    node = (FreeListNode) tmp; //point n to beginning of chunk
    node->flink = 0; //same as NULL in C
    node->size = size;
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
        uintptr_t next = (uintptr_t) temp->flink;
        uintptr_t last = (uintptr_t) temp + (uintptr_t) temp->size;
        if(last == next) { //Means that temp and temp->flink are touching because of how memory is organized
            FreeListNode next = temp->flink;
            temp->size = temp->size + next->size;
            temp->flink = next->flink;
        } else {
            temp = temp->flink;
        }
     }
    
}

/* int main(int argc, const char * argv[])
{   
    void* ptr1 = my_malloc(4088);
    if(ptr1){
        printf("returned something...\n");
        void * size = ptr1 - 8;
        void * magic = ptr1 - 4;
        printf("Check returned header: size set to:%d, magic number is: %d\n",*((uint32_t*) size), *((uint32_t*) magic));
    }
    void* ptr2 = my_malloc(2000);
    if(ptr2){
        printf("returned something...\n");
        void * size = ptr2 - 8;
        void * magic = ptr2 - 4;
        printf("Check returned header: size set to:%d, magic number is: %d\n",*((uint32_t*) size), *((uint32_t*) magic));
    }
    ptr2 = my_malloc(2000);
    if(ptr2){
        printf("returned something...\n");
        void * size = ptr2 - 8;
        void * magic = ptr2 - 4;
        printf("Check returned header: size set to:%d, magic number is: %d\n",*((uint32_t*) size), *((uint32_t*) magic));
    }
    ptr2 = my_malloc(2000);
    if(ptr2){
        printf("returned something...\n");
        void * size = ptr2 - 8;
        void * magic = ptr2 - 4;
        printf("Check returned header: size set to:%d, magic number is: %d\n",*((uint32_t*) size), *((uint32_t*) magic));
    }
    my_free(ptr1); //1040
    my_free(ptr2); //1040
    void* ptr3 = my_malloc(128);
    if(ptr3){
        printf("returned something...\n");
        void * size = ptr3 - 8;
        void * magic = ptr3 - 4;
        printf("Check returned header: size set to:%d, magic number is: %d\n",*((uint32_t*) size), *((uint32_t*) magic));
    }
    ptr3 = my_malloc(128);
    if(ptr3){
        printf("returned something...\n");
        void * size = ptr3 - 8;
        void * magic = ptr3 - 4;
        printf("Check returned header: size set to:%d, magic number is: %d\n",*((uint32_t*) size), *((uint32_t*) magic));
    }
    ptr3 = my_malloc(128);
    if(ptr3){
        printf("returned something...\n");
        void * size = ptr3 - 8;
        void * magic = ptr3 - 4;
        printf("Check returned header: size set to:%d, magic number is: %d\n",*((uint32_t*) size), *((uint32_t*) magic));
    }
    ptr3 = my_malloc(128);
    if(ptr3){
        printf("returned something...\n");
        void * size = ptr3 - 8;
        void * magic = ptr3 - 4;
        printf("Check returned header: size set to:%d, magic number is: %d\n",*((uint32_t*) size), *((uint32_t*) magic));
    }
    ptr3 = my_malloc(128);
    if(ptr3){
        printf("returned something...\n");
        void * size = ptr3 - 8;
        void * magic = ptr3 - 4;
        printf("Check returned header: size set to:%d, magic number is: %d\n",*((uint32_t*) size), *((uint32_t*) magic));
    }
    ptr3 = my_malloc(128);
    if(ptr3){
        printf("returned something...\n");
        void * size = ptr3 - 8;
        void * magic = ptr3 - 4;
        printf("Check returned header: size set to:%d, magic number is: %d\n",*((uint32_t*) size), *((uint32_t*) magic));
    }
    ptr3 = my_malloc(128);
    if(ptr3){
        printf("returned something...\n");
        void * size = ptr3 - 8;
        void * magic = ptr3 - 4;
        printf("Check returned header: size set to:%d, magic number is: %d\n",*((uint32_t*) size), *((uint32_t*) magic));
    }
    ptr3 = my_malloc(128);
    if(ptr3){
        printf("returned something...\n");
        void * size = ptr3 - 8;
        void * magic = ptr3 - 4;
        printf("Check returned header: size set to:%d, magic number is: %d\n",*((uint32_t*) size), *((uint32_t*) magic));
    }
    ptr3 = my_malloc(128);
    if(ptr3){
        printf("returned something...\n");
        void * size = ptr3 - 8;
        void * magic = ptr3 - 4;
        printf("Check returned header: size set to:%d, magic number is: %d\n",*((uint32_t*) size), *((uint32_t*) magic));
    }
    ptr3 = my_malloc(128);
    if(ptr3){
        printf("returned something...\n");
        void * size = ptr3 - 8;
        void * magic = ptr3 - 4;
        printf("Check returned header: size set to:%d, magic number is: %d\n",*((uint32_t*) size), *((uint32_t*) magic));
    }
    ptr3 = my_malloc(128);
    if(ptr3){
        printf("returned something...\n");
        void * size = ptr3 - 8;
        void * magic = ptr3 - 4;
        printf("Check returned header: size set to:%d, magic number is: %d\n",*((uint32_t*) size), *((uint32_t*) magic));
    }
    ptr3 = my_malloc(128);
    if(ptr3){
        printf("returned something...\n");
        void * size = ptr3 - 8;
        void * magic = ptr3 - 4;
        printf("Check returned header: size set to:%d, magic number is: %d\n",*((uint32_t*) size), *((uint32_t*) magic));
    }
    ptr3 = my_malloc(128);
    if(ptr3){
        printf("returned something...\n");
        void * size = ptr3 - 8;
        void * magic = ptr3 - 4;
        printf("Check returned header: size set to:%d, magic number is: %d\n",*((uint32_t*) size), *((uint32_t*) magic));
    }
    
    my_free(ptr3); //1040

    coalesce_free_list();
    return 0;
} */