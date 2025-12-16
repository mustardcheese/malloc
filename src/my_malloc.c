#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include "my_malloc.h"

#define INITIAL_CHUNK_SIZE 4096

typedef struct header{
    size_t size;
    struct header* next;
    int isfree;
} header;

//points to first free block
header* head = NULL;

static inline void* get_payload(header* h){
    return (void*)((char*)h + sizeof(header));
}

header* allocate(size_t size){
    void* mem_raw = sbrk(size);
    
    //sbrk returns (void*)-1 on failure
    if(mem_raw == (void*)-1){
        printf("sbrk failed to allocate memory \n");
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
    size_t req_total_size = user_request_size + sizeof(header);

    // Scenario 1: Nothing allocated yet
    if(head == NULL){
        // Allocate a large chunk (4096 bytes)
        header* new_chunk = allocate(INITIAL_CHUNK_SIZE);

        // Set free pointer to past total size of block used
        header* new_free = (header*)((char*)new_chunk + req_total_size);

        // Initialize free block
        new_free->size = INITIAL_CHUNK_SIZE - req_total_size;
        new_free->isfree = 1;
        new_free->next = NULL;

        // Initialize the new chunk
        new_chunk->size = req_total_size;
        new_chunk->isfree = 0;
        new_chunk->next = NULL;
        
        head = new_free;
        // Return past the header to usable space
        return get_payload(new_chunk);
    }

    // Scenario 2: There is space for the request
    header* curr = head;
    header* slow = NULL;

    while(curr != NULL){
        if(curr->size >= req_total_size){

            // Give entire block if too small to split
            if(curr->size - req_total_size < sizeof(header) + 1){
                curr->isfree = 0;
                if(slow == NULL){
                    head = curr->next;
                }
                else{
                    slow->next = curr->next;
                    curr->next = NULL;
                }
                return get_payload(curr);
            }

            // Split
            header* new_free = (header*)((char*)curr + req_total_size);
            new_free->size = curr->size - req_total_size;
            new_free->isfree = 1;
            new_free->next = curr->next;

            if(slow == NULL){
                head = new_free;
            } else {
                slow->next = new_free;
            }

            curr->size = req_total_size;
            curr->isfree = 0;
            curr->next = NULL;
           
            return get_payload(curr);
        }
        slow = curr;
        curr = curr->next;
    }

    // Scenario 3: There is no space for the request
    if(curr == NULL){
        // alloc new chunk and split
        header* new_chunk = allocate(req_total_size);
        if(new_chunk == NULL){
            printf("Allocation failed \n");
            return NULL;
        }

        header* new_free = (header*)((char*)new_chunk + req_total_size);
        new_free->size = new_chunk->size - req_total_size;
        new_free->isfree = 1;
        new_free->next = NULL;

        new_chunk->size = req_total_size;
        new_chunk->isfree = 0;
        new_chunk->next = NULL;
        slow->next = new_free;
        return get_payload(new_chunk);
    }
}

// lazy implementation; insert @ head
void my_free(void* ptr){
    if(ptr == NULL){
        printf("Attempted to free a NULL pointer :(\n");
        return;
    }
    header* block_header = (header*)((char*)ptr - sizeof(header));
    block_header->isfree = 1;

    //add to free list
    block_header->next = head;
    head = block_header;
}

void print_free_list() {
    printf("--- Free List ---\n");
    header *curr = head;
    while (curr != NULL) {
        // We cast to (void*) just to print the memory address nicely
        printf("Addr: %p | Size: %zu | Next: %p\n", 
               (void*)curr, curr->size, (void*)curr->next);
        curr = curr->next;
    }
    printf("-----------------\n");
}