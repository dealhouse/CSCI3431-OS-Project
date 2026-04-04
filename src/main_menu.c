#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
    int choice = -1;

    while (1)
    {
        printf("\n====================\n");
        printf("    Main  Menu    \n");
        printf("====================\n");
        printf("1. Logging System\n");
        printf("2. Peterson's Solution\n");
        printf("3. File Management\n");
        printf("4. Memory Allocation\n");
        printf("5. Amdahl's Law\n");
        printf("0. Exit\n");
        printf("===========================\n");

        do
        {
            printf("Enter choice: ");
            if (scanf("%d", &choice) != 1 || choice < 0 || choice > 5)
            {
                printf("Invalid choice. Please try again.\n");
                while (getchar() != '\n')
                    ;
            }
        } while (choice < 0 || choice > 5);

        if (choice == 0)
        {
            printf("Exiting...\n");
            exit(0);
        }

        pid_t pid = fork();

        if (pid == 0)
        {
            printf("\033[?1049h\033[H");
            fflush(stdout);
            switch (choice)
            {
            case 1:
                execl("./logger", "logger", NULL);
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
        }
        else
        {
            perror("fork failed");
            exit(1);
        }
    }
    return 0;
}
