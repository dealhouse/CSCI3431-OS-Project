/*
A00469340 Xavier Mcdonald
Memory Simulation using First-Fit Approach

This program implements a first-fit algorithm by checking each memory segment sequentially. 
When space is allocated, the block is split to track the remaining available memory. 
Merging adjacent free blocks is not automatic and requires executing the defragmentation function.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "logger_client.h"

#define MAX_SEGMENTS 100

int segment_pid[MAX_SEGMENTS];
int base_address[MAX_SEGMENTS];
int segment_length[MAX_SEGMENTS];
int is_unallocated[MAX_SEGMENTS];

int active_segments = 0;

// Display Memory Layout
void print_mem_map() {
    log_event("MEMORY", "Action: User requested to display memory map.");
    printf("\n--- Current Memory Layout ---\n");
    printf("Idx\tOwner\tBase\tLength\tState\n");
    for (int i = 0; i < active_segments; i++) {
        if (is_unallocated[i]) {
            printf("%d\t-\t%d\t%d\tAvailable\n", i, base_address[i], segment_length[i]);
        } else {
            printf("%d\tP%d\t%d\t%d\tIn Use\n", i, segment_pid[i], base_address[i], segment_length[i]);
        }
    }
    printf("-----------------------------\n");
}

// Allocation First-Fit Algo
void grant_space(int target_id, int needed_space) {
    char log_buffer[256];
    
    for (int i = 0; i < active_segments; i++) {
        if (is_unallocated[i] && segment_length[i] >= needed_space) {
            if (segment_length[i] == needed_space) {
                is_unallocated[i] = 0;
                segment_pid[i] = target_id;
                
                snprintf(log_buffer, sizeof(log_buffer), "Action: Exact fit assignment for P%d (Size: %d).", target_id, needed_space);
                log_event("MEMORY", log_buffer);
                printf("Space granted to P%d.\n", target_id);
                return;
            }
            
            // Shift elements right across all arrays
            for (int k = active_segments; k > i; k--) {
                segment_pid[k] = segment_pid[k-1];
                base_address[k] = base_address[k-1];
                segment_length[k] = segment_length[k-1];
                is_unallocated[k] = is_unallocated[k-1];
            }
            
            // Assign allocated block
            segment_pid[i] = target_id;
            is_unallocated[i] = 0;
            segment_length[i] = needed_space;
            
            // Update the remaining free block
            base_address[i+1] = base_address[i] + needed_space;
            segment_length[i+1] -= needed_space;
            active_segments++;
            
            snprintf(log_buffer, sizeof(log_buffer), "Action: Split and assigned block for P%d (Size: %d).", target_id, needed_space);
            log_event("MEMORY", log_buffer);
            printf("Space granted to P%d.\n", target_id);
            return;
        }
    }
    snprintf(log_buffer, sizeof(log_buffer), "Action: Space request denied for P%d (Size: %d) - Insufficient contiguous block.", target_id, needed_space);
    log_event("MEMORY", log_buffer);
    printf("Request denied: Insufficient contiguous space.\n");
}

// Deallocation
void release_memory(int target_id) {
    int is_located = 0;
    char log_buffer[256];

    for (int i = 0; i < active_segments; i++) {
        if (!is_unallocated[i] && segment_pid[i] == target_id) {
            is_unallocated[i] = 1;
            segment_pid[i] = -1;
            is_located = 1;
            
            snprintf(log_buffer, sizeof(log_buffer), "Action: Freed memory for P%d.", target_id);
            log_event("MEMORY", log_buffer);
            printf("Freed P%d.\n", target_id);
        }
    }
    if (!is_located) {
        snprintf(log_buffer, sizeof(log_buffer), "Action: Free failed - P%d missing.", target_id);
        log_event("MEMORY", log_buffer);
        printf("Free failed: P%d not located.\n", target_id);
    }
}

// Compaction
void defragment_memory() {
    int running_address = 0;
    int shift_ptr = 0;
    int accumulated_free_mem = 0;

    for (int i = 0; i < active_segments; i++) {
        if (!is_unallocated[i]) {
            base_address[i] = running_address;
            running_address += segment_length[i];
            
            // Shift allocated data left
            segment_pid[shift_ptr] = segment_pid[i];
            base_address[shift_ptr] = base_address[i];
            segment_length[shift_ptr] = segment_length[i];
            is_unallocated[shift_ptr] = 0;
            shift_ptr++;
        } else {
            accumulated_free_mem += segment_length[i];
        }
    }
    
    // Add consolidated free block at the end
    if (accumulated_free_mem > 0) {
        segment_pid[shift_ptr] = -1;
        base_address[shift_ptr] = running_address;
        segment_length[shift_ptr] = accumulated_free_mem;
        is_unallocated[shift_ptr] = 1;
        active_segments = shift_ptr + 1;
    } else {
        active_segments = shift_ptr;
    }
    
    log_event("MEMORY", "Action: Ran memory defragmentation.");
    printf("Memory defragmented.\n");
}

// Main Driver
int main() {
    int starting_capacity;
    char log_buffer[256];
    
    log_event("MEMORY", "System: Memory Allocation module initialized.");

    printf("Enter initial memory capacity (total space): ");
    if (scanf("%d", &starting_capacity) != 1 || starting_capacity <= 0) {
        log_event("MEMORY", "Error: Invalid initial memory size entered. Exiting.");
        printf("Invalid capacity. Exiting.\n");
        return 1;
    }

    snprintf(log_buffer, sizeof(log_buffer), "Input: Initial memory size set to %d.", starting_capacity);
    log_event("MEMORY", log_buffer);

    // Initialize the starting state as one large free block
    segment_pid[0] = -1;
    base_address[0] = 0;
    segment_length[0] = starting_capacity;
    is_unallocated[0] = 1;
    active_segments = 1;

    int menu_selection, input_pid, input_length;

    while (1) {
        printf("\n--- Allocation Interface ---\n");
        printf("1. Grant Memory\n");
        printf("2. Release Memory\n");
        printf("3. Defragment Memory\n");
        printf("4. Print Memory Layout\n");
        printf("5. Quit\n");
        printf("Choose an option: ");
        
        if (scanf("%d", &menu_selection) != 1) {
            log_event("MEMORY", "Error: Non-integer input detected at main menu.");
            printf("Invalid input. Please enter a valid number.\n");
            while (getchar() != '\n'); 
            continue;
        }

        snprintf(log_buffer, sizeof(log_buffer), "Input: User selected menu option %d.", menu_selection);
        log_event("MEMORY", log_buffer);

        switch (menu_selection) {
            case 1:
                printf("Enter Target Process ID (integer): ");
                if (scanf("%d", &input_pid) != 1) { while(getchar() != '\n'); break; }
                printf("Enter Required Size: ");
                if (scanf("%d", &input_length) != 1) { while(getchar() != '\n'); break; }
                
                snprintf(log_buffer, sizeof(log_buffer), "Input: Requested allocation for P%d with size %d.", input_pid, input_length);
                log_event("MEMORY", log_buffer);
                
                grant_space(input_pid, input_length);
                break;
            case 2:
                printf("Enter Process ID to release: ");
                if (scanf("%d", &input_pid) != 1) { while(getchar() != '\n'); break; }
                
                snprintf(log_buffer, sizeof(log_buffer), "Input: Requested deallocation for P%d.", input_pid);
                log_event("MEMORY", log_buffer);
                
                release_memory(input_pid);
                break;
            case 3:
                defragment_memory();
                break;
            case 4:
                print_mem_map();
                break;
            case 5:
                log_event("MEMORY", "System: Exiting Memory Allocation module.");
                printf("Shutting down interface...\n");
                return 0;
            default:
                printf("Invalid selection. Please choose 1-5.\n");
        }
    }
    return 0;
}
