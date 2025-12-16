#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <stdint.h>
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
static inline void* get_header(void* p){
    return (void*)((char*)p - sizeof(header));
}

static void* heap_start = NULL;

header* allocate(size_t size){
    if(heap_start == NULL){
        heap_start = sbrk(0);
    }
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

        new_chunk->isfree = 0;
        return get_payload(new_chunk);
    }
}

void my_free(void* ptr){
    if(ptr == NULL){
        printf("Attempted to free a NULL pointer :(\n");
        return;
    }
    header* trav = head;
    header* slow = NULL;
    header* to_free = get_header(ptr);

    while(trav != NULL && trav < to_free){
        slow = trav;
        trav = trav->next;
    }
    if(slow == NULL){
        head = to_free;
        to_free->next = trav;
    } else {
        slow->next = to_free;
        to_free->next = trav;
    }
    to_free->isfree = 1;

    // coalease with next
    if(trav != NULL && (char*)to_free + to_free->size == (char*)trav){
        to_free->size += trav->size;
        to_free->next = trav->next;
    }
    // coalease with previous
    if(slow != NULL && (char*)slow + slow->size == (char*)to_free){
        slow->size += to_free->size;
        slow->next = to_free->next;
    }
}
void* my_realloc(void* ptr, size_t size){
    if(ptr == NULL){
        return my_malloc(size);
    }
    header* old = get_header(ptr);
    // already fits
    if(old->size >= size + sizeof(header)){
        return ptr;
    }
    void* new_ptr = my_malloc(size);
    if(new_ptr == NULL){
        printf("realloc failed to allocate memory \n");
        return NULL;
    }
    memcpy(new_ptr, ptr, old->size - sizeof(header));
    my_free(ptr);
    return new_ptr;
}

void* my_calloc(size_t num, size_t size){
    size_t total_size = num * size;
    if(num !=0 && total_size / num != size){
        printf("calloc overflow \n");
        return NULL; // overflow
    }
    void* ptr = my_malloc(total_size);
    if(ptr != NULL){
        memset(ptr, 0, total_size);
    }
    return ptr; 
}

void print_free_list() {
    printf("--- Free List ---\n");
    header *curr = head;
    while (curr != NULL) {
        long offset = (char*)curr - (char*)heap_start;
        printf("Addr: (%ld) 0x%lx | Size: %zu | Next: (%ld) 0x%lx\n", 
               offset, offset, curr->size, 
               curr->next ? (long)((char*)curr->next - (char*)heap_start) : -1,
               curr->next ? (long)((char*)curr->next - (char*)heap_start) : 0);
               
        curr = curr->next;
    }
    printf("-----------------\n");
}