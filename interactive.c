#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <limits.h>
#include "matrix.h"
#include "scheduler.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <FIFO/SJF> <job_sizes_comma_separated> \n", argv[0]);
        return 1;
    }

    char *policy = argv[1];

    int capacity = 1;
    for (char *c = argv[2]; *c; c++) {
        if (*c == ',') {
            capacity++;
        }
    }

    int *jobs = malloc(sizeof(int) * capacity);
    if (jobs == NULL) {
        printf("Failed to allocate job list.\n");
        return 1;
    }

    int size = 0;
    for (char *tok = strtok(argv[2], ","); tok != NULL; tok = strtok(NULL, ",")) {
        char *end;
        long n = strtol(tok, &end, 10);

        if (end == tok || *end != '\0' || n <= 0 || n > INT_MAX) {
            printf("Invalid job size '%s'. Job sizes must be positive integers.\n", tok);
            free(jobs);
            return 1;
        }

        jobs[size++] = (int)n;
    }

    if (size == 0) {
        printf("No jobs given.\n");
        free(jobs);
        return 1;
    }

    float avg_response = strcmp(policy, "SJF") == 0 ? SJF(jobs, size)
                                                    : FIFO(jobs, size);
    free(jobs);

    if (avg_response < 0.0f) {
        printf("Scheduling failed.\n");
        return 1;
    }

    printf("%s average response time: %.6f seconds\n", policy, avg_response);
    return 0;
}
