/*
 * logger.c
 * Module 1 - Logging System (IPC)
 *
 * Student: Moneab Milad
 * A#: 481583
 *
 * Uses two IPC mechanisms:
 *   1. Named pipe (FIFO) - other modules write log messages into it
 *   2. Shared memory ring - stages messages before flushing to logs/logs.txt
 *
 * Usage:
 *   ./logger                        - start daemon
 *   ./logger stop                   - stop daemon
 *   ./logger log MODULE "message"   - write one log entry
 */

// Enables POSIX features (like FIFO, shared memory, etc.)
#define _POSIX_C_SOURCE 200809L

// Standard libraries for input/output, memory, strings, etc.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Libraries for file control and permissions
#include <fcntl.h>
#include <sys/stat.h>

// Libraries for shared memory (IPC)
#include <sys/ipc.h>
#include <sys/shm.h>

// Libraries for time and error handling
#include <time.h>
#include <errno.h>

// Constants for pipe location, log file, shared memory key, etc.
#define PIPE_PATH  "/tmp/os_log_pipe"
#define LOG_FILE "logs.txt"
#define SHM_KEY    0x4C4F4720
#define RING_SIZE  16
#define MSG_LEN    256
#define STOP_MSG   "##STOP##"

// Ring buffer structure to store messages in shared memory
typedef struct {
    char slots[RING_SIZE][MSG_LEN];
    int head, tail, count;
} Ring;

/*
 * Gets the current timestamp and formats it into a string
 */
static void get_timestamp(char *buf, size_t len) {
    time_t t = time(NULL);
    strftime(buf, len, "%Y-%m-%d %H:%M:%S", localtime(&t));
}

/*
 * Adds a message into the shared memory ring buffer
 */
static void ring_push(Ring *r, const char *msg) {
    if (r->count >= RING_SIZE) return;  // Do nothing if buffer is full
    strncpy(r->slots[r->head], msg, MSG_LEN - 1);
    r->head = (r->head + 1) % RING_SIZE; // Move head forward (circular)
    r->count++;
}

/*
 * Removes a message from the ring buffer and copies it into 'out'
 */
static int ring_pop(Ring *r, char *out) {
    if (r->count == 0) return 0; // Nothing to pop
    strncpy(out, r->slots[r->tail], MSG_LEN - 1);
    r->tail = (r->tail + 1) % RING_SIZE; // Move tail forward
    r->count--;
    return 1;
}

/*
 * Creates or connects to shared memory and initializes the ring buffer
 */
static Ring *shm_open_ring(void) {
    int id = shmget(SHM_KEY, sizeof(Ring), IPC_CREAT | 0666);
    if (id == -1) { perror("shmget"); return NULL; }

    Ring *r = shmat(id, NULL, 0);
    if (r == (void *)-1) { perror("shmat"); return NULL; }

    memset(r, 0, sizeof(Ring)); // Initialize memory to zero
    return r;
}

/*
 * Deletes the shared memory segment when the daemon stops
 */
static void shm_destroy(void) {
    int id = shmget(SHM_KEY, sizeof(Ring), 0666);
    if (id != -1) shmctl(id, IPC_RMID, NULL);
}

/*
 * Main daemon loop:
 * - Reads messages from pipe
 * - Adds timestamp
 * - Stores them in shared memory
 * - Writes them to log file
 */
static void run_daemon(void) {

    // Create named pipe (FIFO) for communication
    mkfifo(PIPE_PATH, 0666);

    // Initialize shared memory ring buffer
    Ring *ring = shm_open_ring();
    if (!ring) exit(1);

    // Open log file in append mode
    FILE *fp = fopen(LOG_FILE, "a");
    if (!fp) { perror("fopen"); exit(1); }

    printf("Logger daemon started. Writing to %s\n", LOG_FILE);

    // Open pipe for reading
    int pfd = open(PIPE_PATH, O_RDONLY);
    if (pfd == -1) { perror("open pipe"); exit(1); }

    char line[MSG_LEN], ts[32], entry[MSG_LEN + 40];

    // Infinite loop to continuously read messages
    while (1) {
        int i = 0;
        char c;

        // Read message character by character until newline
        while (i < MSG_LEN - 1) {
            int n = (int)read(pfd, &c, 1);

            // If pipe is closed, reopen it
            if (n <= 0) {
                close(pfd);
                pfd = open(PIPE_PATH, O_RDONLY);
                if (pfd == -1) goto done;
                continue;
            }

            if (c == '\n') break;
            line[i++] = c;
        }

        line[i] = '\0';

        // Skip empty lines
        if (i == 0) continue;

        // Stop daemon if stop message received
        if (strcmp(line, STOP_MSG) == 0) break;

        // Add timestamp to log entry
        get_timestamp(ts, sizeof(ts));
        snprintf(entry, sizeof(entry), "[%s] %s", ts, line);

        // Push message into ring buffer
        ring_push(ring, entry);

        // Pop all messages from buffer and write to file
        char out[MSG_LEN];
        while (ring_pop(ring, out))
            fprintf(fp, "%s\n", out);

        fflush(fp); // Ensure data is written to file
    }

done:
    // Cleanup resources when daemon stops
    close(pfd);
    fclose(fp);
    shmdt(ring);
    shm_destroy();
    unlink(PIPE_PATH);

    printf("Logger stopped.\n");
}

/*
 * Sends a log message to the daemon via the named pipe
 */
void log_event(const char *module, const char *msg) {
    mkfifo(PIPE_PATH, 0666);

    int fd = open(PIPE_PATH, O_WRONLY | O_NONBLOCK);
    if (fd == -1) return;

    char buf[MSG_LEN];
    snprintf(buf, sizeof(buf), "[%s] %s\n", module, msg);

    write(fd, buf, strlen(buf));
    close(fd);
}

/*
 * Main function:
 * - "stop" → sends stop signal to daemon
 * - "log"  → sends a log message
 * - default → starts daemon
 */
int main(int argc, char *argv[]) {

    // Stop command
    if (argc >= 2 && strcmp(argv[1], "stop") == 0) {
        int fd = open(PIPE_PATH, O_WRONLY | O_NONBLOCK);
        if (fd == -1) { printf("Daemon not running.\n"); return 0; }

        char msg[MSG_LEN];
        snprintf(msg, sizeof(msg), "%s\n", STOP_MSG);

        write(fd, msg, strlen(msg));
        close(fd);

        printf("Stop signal sent.\n");
        return 0;
    }

    // Log command
    if (argc >= 4 && strcmp(argv[1], "log") == 0) {
        log_event(argv[2], argv[3]);
        return 0;
    }

    // Default: run the logger daemon
    run_daemon();
    return 0;
}
