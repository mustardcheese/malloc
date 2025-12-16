#include <stdio.h>
#include "my_malloc.h"

int main() {
    printf("Start of Main.\n");
    print_free_list(); // Should be empty (or NULL) initially

    // TEST 1: First Allocation (Scenario 1)
    // -------------------------------------
    printf("\n[1] Allocating 100 bytes...\n");
    void* p1 = my_malloc(100);
    printf("p1 allocated at: %p\n", p1);
    print_free_list(); 
    
    // TEST 2: Second Allocation (Scenario 2 - Split)
    // -------------------------------------
    printf("\n[2] Allocating 50 bytes...\n");
    void* p2 = my_malloc(50);
    printf("p2 allocated at: %p\n", p2);
    print_free_list();
    // EXPECT: The free block should be smaller now.

    
    // TEST 3: Freeing Memory
    // -------------------------------------
    printf("\n[3] Freeing p2...\n");
    my_free(p2);
    print_free_list();
    // EXPECT: A new block (size ~50 + header) should appear at the HEAD of the list.

    
    // TEST 4: Reuse Freed Memory
    // -------------------------------------
    printf("\n[4] Allocating 50 bytes again...\n");
    void* p3 = my_malloc(51);
    printf("p3 allocated at: %p\n", p3);
    print_free_list();
    // EXPECT: The block we just freed should be gone again. 
    // Ideally, p3 equals p2 (reusing the same address).

    return 0;
}