/*
A004050815 Daniel Johnston
Amdahl's Law Calculator

This program is an Amdahl’s Law calculator that estimates the performance improvement of a program, 
when it is parallelized across multiple CPU cores. 
Also, it optionally compares execution times between serial and parallel versions.
*/

#include <stdio.h>
#include <stdlib.h>
#include "logger_client.h"

double calculate_speedup(double parallel_fraction, int cores)
{
    return 1.0 / ((1.0 - parallel_fraction) + (parallel_fraction / cores));
}

void compare_execution_times(double speedup)
{
    double serial_time;

    printf("Enter serial execution time (in seconds): ");
    scanf("%lf", &serial_time);
    if (serial_time < 0)
    {
        printf("Invalid input. Serial execution time must be a positive number.\n");
        return;
    }

    double parallel_time = serial_time / speedup;

    printf("\n--- Execution Comparison ---\n");
    printf("Serial Time   = %.4f seconds\n", serial_time);
    printf("Parallel Time = %.4f seconds\n", parallel_time);
    printf("Time Saved    = %.4f seconds\n", serial_time - parallel_time);
    printf("\nPress enter to exit...");
    
    getchar();
    getchar();
}

int main()
{
    log_event("AMDAHL", "Module started");
    int cores, choice;
    double parallel_fraction, speedup;

    printf("--- Amdahl's Law Calculator ---\n");

    printf("Enter parallel fraction (0 to 1): ");
    scanf("%lf", &parallel_fraction);
    if ((parallel_fraction < 0) || (1 < parallel_fraction))
    {
        printf("Invalid input. Parallel fraction must be between 0 and 1.\n");
        return 1;
    }
    log_event("AMDAHL", "Fraction selected.");
    printf("Enter number of cores: ");
    scanf("%d", &cores);
    if (cores <= 0)
    {
        printf("Invalid input. Must be a positive number of cores.\n");
        return 1;
    }
    log_event("AMDAHL", "Cores selected.");
    speedup = calculate_speedup(parallel_fraction, cores);
    log_event("AMDAHL", "Speedup calculated");
    printf("--- Result ---\n");
    printf("Speedup = %.4f\n", speedup);

    printf("Do you want to compare execution times? (1 = Yes, 0 = No): ");
    scanf(" %d", &choice);
    if (choice == 1)
    {
        log_event("AMDAHL", "Comparing execution times");
        compare_execution_times(speedup);
        log_event("AMDAHL", "Execution comparison done");
    }
    
    log_event("AMDAHL", "Module exited");
    return 0;
}
