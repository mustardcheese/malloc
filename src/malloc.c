#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#define INITIAL_CHUNK_SIZE 4096

//points to first free block
header* head = NULL;

typedef struct header{
    size_t size;
    int isfree;
    header* next;
} header;

header* allocate(size_t size){
    void* mem_raw = sbrk(size);
    
    //sbrk will return (void*)-1 on failure
    if(mem_raw == (void*)-1){
        return NULL;
    }

    //create and initialize every value of the header
    header* block_header = (header*) mem_raw;
    block_header->size = size;
    block_header->isfree = 1;
    block_header->next = NULL;
    return block_header;
}

void* my_malloc(size_t user_request_size){
    size_t total_size = user_request_size + sizeof(header);
    // Scenario 1: Nothing allocated yet
    if(head == NULL){
        // Allocate a large chunk (4096 bytes)
        header* new_chunk = allocate(INITIAL_CHUNK_SIZE);

        // Set free pointer to past total size of block used
        header* new_free = (header*)((char*)new_chunk + total_size);

        // Initialize free block
        new_free->size = INITIAL_CHUNK_SIZE - total_size;
        new_free->isfree = 1;
        new_free->next = NULL;

        // Initialize the new chunk
        new_chunk->size = total_size;
        new_chunk->isfree = 0;
        new_chunk->next = NULL;
        
        head = new_free;
        // Return past the header to usable space
        return (void*)((char*)new_chunk + sizeof(header));
    }

    // Scenario 2: There is space for the request
    header* traverse = head;
    while(traverse != NULL){
        if(total_size < traverse->size){
            header* new_chunk = traverse;
            header* new_free = NULL;
            
            // Is this even worth creating a new free block for?
            // yes -> new_free block is created of size free-used
            // no -> move on

            // doesn't that mean this memory literally just sits here if not enough space??? idk
            if(traverse->size - total_size > sizeof(header)){
                new_free = (header*)((char*)traverse + total_size);
                new_free->size = traverse->size - total_size;
                new_free->isfree = 1;
                new_free->next = traverse->next;
            }
    
            new_chunk->size = total_size;
            new_chunk->isfree = 0;
            new_chunk->next = NULL;

            return (void*)((char*)new_chunk + sizeof(header));
        }
        traverse = traverse->next;
    }

    // Scenario 3: There is no space for the request
    if(traverse == NULL){

    }

}   