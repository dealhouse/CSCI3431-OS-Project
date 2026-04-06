#include <stdio.h>
#include <stdlib.h>

#define MAX_BLOCKS 100

int m_id[MAX_BLOCKS];
int m_start[MAX_BLOCKS];
int m_size[MAX_BLOCKS];
int m_is_free[MAX_BLOCKS];

int total_blocks = 0;

// 1. Display Memory Map
void display() {
    printf("\n--- Memory Map ---\n");
    printf("Index\tID\tStart\tSize\tStatus\n");
    for (int i = 0; i < total_blocks; i++) {
        // Use ID -1 to represent Free space visually
        if (m_is_free[i]) {
            printf("%d\t-\t%d\t%d\tFree\n", i, m_start[i], m_size[i]);
        } else {
            printf("%d\tP%d\t%d\t%d\tAllocated\n", i, m_id[i], m_start[i], m_size[i]);
        }
    }
    printf("------------------\n");
}

// 2. Allocation (First-Fit)
void allocate(int proc_id, int request_size) {
    for (int i = 0; i < total_blocks; i++) {
        if (m_is_free[i] && m_size[i] >= request_size) {
            if (m_size[i] == request_size) {
                m_is_free[i] = 0;
                m_id[i] = proc_id;
                printf("Successfully allocated P%d.\n", proc_id);
                return;
            }
            
            // Shift elements right across all arrays
            for (int j = total_blocks; j > i; j--) {
                m_id[j] = m_id[j-1];
                m_start[j] = m_start[j-1];
                m_size[j] = m_size[j-1];
                m_is_free[j] = m_is_free[j-1];
            }
            
            // Assign allocated block
            m_id[i] = proc_id;
            m_is_free[i] = 0;
            m_size[i] = request_size;
            
            // Update the remaining free block
            m_start[i+1] = m_start[i] + request_size;
            m_size[i+1] -= request_size;
            total_blocks++;
            printf("Successfully allocated P%d.\n", proc_id);
            return;
        }
    }
    printf("Allocation failed: Not enough contiguous space.\n");
}

// 3. Deallocation
void deallocate(int proc_id) {
    int found = 0;
    for (int i = 0; i < total_blocks; i++) {
        if (!m_is_free[i] && m_id[i] == proc_id) {
            m_is_free[i] = 1;
            m_id[i] = -1;
            found = 1;
            printf("Deallocated P%d.\n", proc_id);
        }
    }
    if (!found) {
        printf("Deallocation failed: P%d not found.\n", proc_id);
    }
}

// 4. Compaction
void compact() {
    int current_addr = 0;
    int write_index = 0;
    int total_free_space = 0;

    for (int i = 0; i < total_blocks; i++) {
        if (!m_is_free[i]) {
            m_start[i] = current_addr;
            current_addr += m_size[i];
            
            // Shift allocated data left
            m_id[write_index] = m_id[i];
            m_start[write_index] = m_start[i];
            m_size[write_index] = m_size[i];
            m_is_free[write_index] = 0;
            write_index++;
        } else {
            total_free_space += m_size[i];
        }
    }
    
    // Add consolidated free block at the end
    if (total_free_space > 0) {
        m_id[write_index] = -1;
        m_start[write_index] = current_addr;
        m_size[write_index] = total_free_space;
        m_is_free[write_index] = 1;
        total_blocks = write_index + 1;
    } else {
        total_blocks = write_index;
    }
    printf("Memory compacted.\n");
}

// 5. Main Driver
int main() {
    int initial_memory;
    
    printf("Enter initial memory size (total space): ");
    if (scanf("%d", &initial_memory) != 1 || initial_memory <= 0) {
        printf("Invalid memory size. Exiting.\n");
        return 1;
    }

    // Initialize the starting state as one large free block
    m_id[0] = -1;
    m_start[0] = 0;
    m_size[0] = initial_memory;
    m_is_free[0] = 1;
    total_blocks = 1;

    int choice, proc_id, size;

    while (1) {
        printf("\n--- Memory Manager ---\n");
        printf("1. Allocate Memory\n");
        printf("2. Deallocate Memory\n");
        printf("3. Compact Memory\n");
        printf("4. Display Memory Map\n");
        printf("5. Exit\n");
        printf("Select an option: ");
        
        // Input validation to prevent infinite loops on non-integer input
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            while (getchar() != '\n'); 
            continue;
        }

        switch (choice) {
            case 1:
                printf("Enter Process ID (integer): ");
                if (scanf("%d", &proc_id) != 1) { while(getchar() != '\n'); break; }
                printf("Enter Size: ");
                if (scanf("%d", &size) != 1) { while(getchar() != '\n'); break; }
                allocate(proc_id, size);
                break;
            case 2:
                printf("Enter Process ID to deallocate: ");
                if (scanf("%d", &proc_id) != 1) { while(getchar() != '\n'); break; }
                deallocate(proc_id);
                break;
            case 3:
                compact();
                break;
            case 4:
                display();
                break;
            case 5:
                printf("Exiting Memory Manager...\n");
                return 0;
            default:
                printf("Invalid choice. Select 1-5.\n");
        }
    }
    return 0;
}
