/*
 * logger_client.h
 *
 * Student: Moneab Milad
 * A#: 481583
 *
 * Include this in any module to send a log entry to the logger daemon.
 * Call: log_event("MODULE_NAME", "your message");
 * If the daemon is not running, the call does nothing.
 */

#ifndef LOGGER_CLIENT_H
#define LOGGER_CLIENT_H

// Enables POSIX features (needed for FIFO and system calls)
#define _POSIX_C_SOURCE 200809L

// Standard libraries for input/output, strings, and system calls
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

// Path to the named pipe used to communicate with the logger daemon
#define PIPE_PATH   "/tmp/os_log_pipe"

// Maximum length of a log message
#define LOG_MSG_LEN 256

/*
 * Sends a log message to the logger daemon through the named pipe
 * - Formats the message with module name
 * - Writes it into the FIFO
 * - If daemon is not running, it safely does nothing
 */
static inline void log_event(const char *module, const char *msg) {

    // Ensure the named pipe exists
    mkfifo(PIPE_PATH, 0666);

    // Open the pipe for writing (non-blocking)
    int fd = open(PIPE_PATH, O_WRONLY | O_NONBLOCK);

    // If pipe can't be opened (daemon not running), exit silently
    if (fd == -1) return;

    // Buffer to store the formatted log message
    char buf[LOG_MSG_LEN];

    // Format message as: [MODULE] message
    snprintf(buf, sizeof(buf), "[%s] %s\n", module, msg);

    // Write message into the pipe
    write(fd, buf, strlen(buf));

    // Close the pipe after writing
    close(fd);
}

#endif
