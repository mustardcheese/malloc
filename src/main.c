#include <stdio.h>

int main() {
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
    void* p3 = my_malloc(50);
    printf("p3 allocated at: %p\n", p3);
    print_free_list();
    // EXPECT: The block we just freed should be gone again. 
    // Ideally, p3 equals p2 (reusing the same address).

    // TEST 5: The "Sandwich" Coalescing Test
    // -------------------------------------
    printf("\n[5] Allocating A, B, C at 100 bytes each...\n");
    void* a = my_malloc(100);
    void* b = my_malloc(100);
    void* c = my_malloc(100);
    print_free_list();
    
    printf("\n[6] Freeing A and C (Creating gaps)...\n");
    my_free(a);
    my_free(c);
    print_free_list();
    // EXPECT: List should have two blocks (Block A and Block C) separated by B.

    printf("\n[7] Freeing B (middle)...\n");
    my_free(b);
    print_free_list();
    // EXPECT: A, B, and C should merge into ONE massive block. 
    // If you see 2 blocks, Left Merge failed.

    // TEST 8: Realloc (Growing and Moving)
    // ------------------------------------
    printf("\n[8] Testing Realloc...\n");
    int* numbers = (int*)my_malloc(2 * sizeof(int));
    numbers[0] = 42;
    numbers[1] = 99;
    printf("Original numbers: %d, %d (at %p)\n", numbers[0], numbers[1], (void*)numbers);
    
    // Grow the array to hold 100 ints (forcing a move)
    numbers = (int*)my_realloc(numbers, 100 * sizeof(int));
    
    printf("New address: %p\n", (void*)numbers);
    printf("Verifying data survived: %d, %d\n", numbers[0], numbers[1]);
    
    if (numbers[0] == 42 && numbers[1] == 99) {
        printf("SUCCESS: Data preserved!\n");
    } else {
        printf("FAILURE: Data corrupted!\n");
    }
    
    my_free(numbers);
    print_free_list();

    // TEST 9: Calloc (Zero Initialization)
    // -------------------------------------
    printf("\n[9] Testing Calloc (5 ints)...\n");
    
    // Allocate space for 5 integers (5 * 4 bytes = 20 bytes payload)
    int* zero_arr = (int*)my_calloc(5, sizeof(int));
    
    if (zero_arr == NULL) {
        printf("FAILURE: Calloc returned NULL.\n");
    } else {
        printf("ptr allocated at: %p\n", (void*)zero_arr);

        // CHECK: Are they all zero?
        int is_clean = 1;
        for (int i = 0; i < 5; i++) {
            if (zero_arr[i] != 0) {
                is_clean = 0;
                printf("FAILURE: Index %d is not zero! Value: %d\n", i, zero_arr[i]);
            }
        }

        if (is_clean) {
            printf("SUCCESS: Memory is zeroed out.\n");
        }
        
        // Cleanup
        my_free(zero_arr);
        print_free_list();
    }
    return 0;
}