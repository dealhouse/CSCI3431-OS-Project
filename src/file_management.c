/*
 * file_management.c
 * Module 3 - File Management
 *
 * Student: Tania Terence
 * A#: A00468936
 *
 * Demonstrates file management operations: create, read, list, and delete files.
 * Logs each action using the logger_client module.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "logger_client.h"

//Create a file
void createFile()
{
    char filename[100];
    printf("Enter filename to create: ");
    scanf("%s", filename);

    FILE *file = fopen(filename, "w");
    if (!file)
    {
        printf("Error creating file.\n");
        log_event("FILE MGMT", "Failed to create file.");
        return;
    }

    fprintf(file, "This is a new file.\n");
    fclose(file);

    printf("File '%s' created successfully.\n", filename);

    char log_msg[150];
    sprintf(log_msg, "Created file: %s", filename);
    log_event("FILE MGMT", log_msg);
}

//Read a file
void readFile()
{
    char filename[100];
    printf("Enter filename to read: ");
    scanf("%s", filename);

    FILE *file = fopen(filename, "r");
    if (!file)
    {
        printf("Error: File does not exist.\n");
        log_event("FILE MGMT", "Failed to read file.");
        return;
    }

    printf("Contents of '%s':\n", filename);
    char ch;
    while ((ch = fgetc(file)) != EOF)
        putchar(ch);
    fclose(file);
    printf("\n");

    char log_msg[150];
    sprintf(log_msg, "Read file: %s", filename);
    log_event("FILE MGMT", log_msg);
}

//Delete a file
void deleteFile()
{
    char filename[100];
    printf("Enter filename to delete: ");
    scanf("%s", filename);

    if (remove(filename) == 0)
    {
        printf("File '%s' deleted successfully.\n", filename);
        char log_msg[150];
        sprintf(log_msg, "Deleted file: %s", filename);
        log_event("FILE MGMT", log_msg);
    }
    else
    {
        printf("Error: Could not delete file.\n");
        log_event("FILE MGMT", "Failed to delete file.");
    }
}

//List files in the current directory
void listFiles()
{
    printf("Files in current directory:\n");
    system("ls"); // For Windows use system("dir");
    log_event("FILE MGMT", "Listed files in directory.");
}

int main()
{
    int choice;

    do
    {
        printf("\n--- File Management ---\n");
        printf("1. Create File\n2. Read File\n3. List Files\n4. Delete File\n5. Exit\n");
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1)
        {
            printf("Invalid input. Enter an integer.\n");
            while (getchar() != '\n')
                ; // clear input buffer
            continue;
        }

        switch (choice)
        {
        case 1:
            createFile();
            break;
        case 2:
            readFile();
            break;
        case 3:
            listFiles();
            break;
        case 4:
            deleteFile();
            break;
        case 5:
            printf("Exiting file manager.\n");
            break;
        default:
            printf("Invalid choice.\n");
        }
    } while (choice != 5);

    return 0;
}
