/*
 * peterson.c
 * Module 2 - Peterson's Solution (Synchronization)
 *
 * Student: Moneab Milad
 * A#: 481583
 *
 * Demonstrates mutual exclusion for two processes using Peterson's algorithm.
 * Uses fork() to create two real processes sharing memory via shmget/shmat.
 * Each process loops through: ENTRY -> CRITICAL SECTION -> EXIT.
 * A shared counter and violation flag verify correctness.
 */

// Enables POSIX features (needed for shared memory, sleep, etc.)
#define _POSIX_C_SOURCE 200809L

// Standard libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Shared memory libraries
#include <sys/ipc.h>
#include <sys/shm.h>

// Process control libraries
#include <sys/wait.h>

// Time functions
#include <time.h>

// Logger interface to send messages to logger daemon
#include "logger_client.h"

// Unique key for shared memory
#define SHM_KEY 0x50455445

/*
 * Shared structure used by both processes
 * Stores flags, turn variable, counter, and violation tracking
 */
typedef struct {
    volatile int flag[2];     // Indicates if process wants to enter CS
    volatile int turn;        // Whose turn it is
    volatile int counter;     // Shared counter
    volatile int in_cs;       // Tracks if someone is in critical section
    volatile int violations;  // Counts violations of mutual exclusion
} Shared;

/*
 * Sleep for a given number of milliseconds
 */
static void sleep_ms(int ms) {
    struct timespec ts = { ms / 1000, (ms % 1000) * 1000000L };
    nanosleep(&ts, NULL);
}

/*
 * Function executed by each process
 * Runs Peterson’s algorithm loop
 */
static void run_process(int id, int iters, Shared *s) {
    int other = 1 - id;  // The other process (0 or 1)
    char buf[128];

    for (int i = 0; i < iters; i++) {

        /* ENTRY SECTION */
        // Indicate this process wants to enter CS and give turn to other
        printf("[P%d] ENTRY (iter %d/%d) - setting flag, yielding turn\n",
               id, i + 1, iters);
        s->flag[id] = 1;
        s->turn = other;

        // Wait while the other process wants to enter and it's their turn
        int spins = 0;
        while (s->flag[other] == 1 && s->turn == other) {
            spins++;
            sleep_ms(10);
        }

        // Print how long it waited (if any waiting happened)
        if (spins > 0)
            printf("[P%d] Waited %d spin(s)\n", id, spins);

        /* CRITICAL SECTION */
        // Check if another process is already inside CS (should not happen)
        if (s->in_cs) {
            s->violations++;
            printf("[P%d] VIOLATION - two processes in CS!\n", id);
        }

        // Mark that this process is now inside CS
        s->in_cs = 1;

        // Safely update shared counter
        printf("[P%d] CRITICAL SECTION - counter: %d -> ", id, s->counter);
        int old = s->counter;
        sleep_ms(50); // Simulate work
        s->counter = old + 1;
        printf("%d\n", s->counter);

        // Send log message to logger daemon
        snprintf(buf, sizeof(buf), "P%d in CS, counter=%d", id, s->counter);
        log_event("PETERSON", buf);

        // Exit critical section
        s->in_cs = 0;

        /* EXIT SECTION */
        // Release flag so other process can enter
        printf("[P%d] EXIT - releasing flag\n\n", id);
        s->flag[id] = 0;

        // Small delay before next iteration
        sleep_ms(20);
    }
}

/*
 * Creates shared memory and initializes it
 */
static Shared *shm_create(void) {
    int id = shmget(SHM_KEY, sizeof(Shared), IPC_CREAT | 0666);
    if (id == -1) { perror("shmget"); return NULL; }

    Shared *s = shmat(id, NULL, 0);
    if (s == (void *)-1) { perror("shmat"); return NULL; }

    memset((void *)s, 0, sizeof(Shared)); // Initialize memory
    return s;
}

/*
 * Frees (removes) shared memory after use
 */
static void shm_free(void) {
    int id = shmget(SHM_KEY, sizeof(Shared), 0666);
    if (id != -1) shmctl(id, IPC_RMID, NULL);
}

/*
 * Prints a simple explanation of Peterson's algorithm
 */
static void explain(void) {
    printf("\nPeterson's Algorithm:\n");
    printf("  Shared vars: flag[2], turn\n\n");
    printf("  ENTRY:    flag[i]=1; turn=j; while(flag[j] && turn==j);\n");
    printf("  CRITICAL: < one process at a time >\n");
    printf("  EXIT:     flag[i]=0;\n\n");
    printf("  Guarantees: mutual exclusion, progress, bounded waiting\n\n");
}

/*
 * Main menu-driven program
 * Lets user run demo, view explanation, or exit
 */
int main(void) {

    srand((unsigned)time(NULL));

    // Log that module started
    log_event("PETERSON", "Module started");

    int choice;

    while (1) {
        printf("\n--- Module 2: Peterson's Solution ---\n");
        printf("1. Run demo\n");
        printf("2. Explain algorithm\n");
        printf("3. Exit\n");
        printf("Choice: ");

        // Get user input
        if (scanf("%d", &choice) != 1) { while (getchar() != '\n'); continue; }

        if (choice == 3) break;

        if (choice == 2) { explain(); continue; }

        if (choice == 1) {

            int iters;

            // Ask how many times each process should run
            printf("Iterations per process (1-10): ");
            if (scanf("%d", &iters) != 1 || iters < 1 || iters > 10) {
                printf("Invalid. Using 3.\n");
                iters = 3;
            }

            printf("\nStarting: 2 processes x %d iterations\n\n", iters);

            log_event("PETERSON", "Demo starting");

            // Create shared memory
            Shared *s = shm_create();
            if (!s) continue;

            // Create child process
            pid_t child = fork();
            if (child == -1) { perror("fork"); shm_free(); continue; }

            // Child process runs as process 1
            if (child == 0) {
                run_process(1, iters, s);
                shmdt((void *)s);
                exit(0);
            } else {
                // Parent process runs as process 0
                run_process(0, iters, s);
                wait(NULL); // Wait for child to finish
            }

            // Print results after both processes finish
            printf("--- Results ---\n");
            printf("Final counter : %d (expected %d)\n", s->counter, iters * 2);
            printf("Violations    : %d\n", s->violations);
            printf("Mutual exclusion %s\n",
                   s->violations == 0 ? "MAINTAINED" : "VIOLATED");

            // Log results
            log_event("PETERSON", s->violations == 0 ? "No violations" : "Violations found");

            // Clean up shared memory
            shmdt((void *)s);
            shm_free();
        }
    }

    // Log exit
    log_event("PETERSON", "Module exited");

    return 0;
}
