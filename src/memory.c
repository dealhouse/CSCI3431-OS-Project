#include <stdio.h>

#define MAX_BLOCKS 100

int m_id[MAX_BLOCKS];
int m_start[MAX_BLOCKS];
int m_size[MAX_BLOCKS];
int m_is_free[MAX_BLOCKS];

int total_blocks = 0;

// 1. Allocation (First-Fit)
void allocate(int proc_id, int request_size) {
    for (int i = 0; i < total_blocks; i++) {
        if (m_is_free[i] && m_size[i] >= request_size) {
            if (m_size[i] == request_size) {
                m_is_free[i] = 0;
                m_id[i] = proc_id;
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
            return;
        }
    }
}

// 2. Compaction
void compact() {
    int current_addr = 0;
    int write_index = 0;
    int total_free_space = 0;

    for (int i = 0; i < total_blocks; i++) {
        if (!m_is_free[i]) {
            m_start[i] = current_addr;
            current_addr += m_size[i];
            
            // Shift data left
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
}

