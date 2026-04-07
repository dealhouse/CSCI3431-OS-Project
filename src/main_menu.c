#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "logger_client.h"

int main()
{
    int choice = -1;

    pid_t logger_pid = fork();
    if (logger_pid == 0)
    {
        execl("./logger", "logger", NULL);
        perror("logger exec failed");
        exit(1);
    }

#define CYAN "\033[0;36m"
#define YELLOW "\033[0;33m"
#define RESET "\033[0m"

    sleep(1);
    while (1)
    {
        printf(CYAN "===========================\n");
        printf("        Main   Menu        \n");
        printf("===========================" RESET "\n");
        printf(YELLOW "1. Logging System\n");
        printf("2. Peterson's Solution\n");
        printf("3. File Management\n");
        printf("4. Memory Allocation\n");
        printf("5. Amdahl's Law\n");
        printf("0. Exit\n");
        printf("===========================\n" RESET);

        do
        {
            printf("Enter choice: ");
            if (scanf("%d", &choice) != 1 || choice < 0 || choice > 5)
            {
                printf("Invalid choice. Please try again.\n");
                while (getchar() != '\n')
                    ;
                choice = -1;
            }
        } while (choice < 0 || choice > 5);

        if (choice == 0)
        {
            log_event("MENU", "User selected exit.");
            printf("Exiting...\n");
            exit(0);
        }
        char log_msg[64];
        snprintf(log_msg, sizeof(log_msg), "User selected option %d", choice);
        log_event("MENU", log_msg);

        pid_t pid = fork();

        if (pid == 0)
        {
            printf("\033[?1049h\033[H");
            fflush(stdout);
            switch (choice)
            {
            case 1:
            {
                FILE *f = fopen("../logs/logs.txt", "r");
                if (!f)
                {
                    printf("No logs found.\n");
                    printf("\nPress enter to continue...\n");
                    getchar();
                    getchar();
                }
                else
                {
                    fclose(f);
                    execlp("less", "less", "../logs/logs.txt", NULL);
                }

                exit(0);
            }
            break;
            case 2:
                execl("./peterson", "peterson", NULL);
                break;
            case 3:
                execl("./file_management", "file_management", NULL);
                break;
            case 4:
                execl("./memory", "memory", NULL);
                break;
            case 5:
                execl("./amdahl", "amdahl", NULL);
                break;
            }

            perror("exec failed");
            exit(1);
        }
        else if (pid > 0)
        {
            wait(NULL);
            printf("\033[?1049l");
            fflush(stdout);
            printf("\033[2J\033[H");
            fflush(stdout);
        }
        else
        {
            perror("fork failed");
            exit(1);
        }
    }
    return 0;
}
