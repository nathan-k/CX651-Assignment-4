#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <limits.h>
#include "matrix.h"

static float elapsed_since(struct timespec *start) {
    struct timespec now;
    timespec_get(&now, TIME_UTC);

    float dns = (float)(now.tv_nsec - start->tv_nsec) / 1000000000;
    float ds = (float)(now.tv_sec - start->tv_sec);

    return dns + ds;
}

static float run_jobs(int *jobs, int size) {
    if (jobs == NULL || size <= 0) {
        return -1.0f;
    }

    struct timespec start;
    timespec_get(&start, TIME_UTC);

    float total_response = 0.0f;

    for (int i = 0; i < size; i++) {
        float response = elapsed_since(&start);
        total_response += response;

        float mm_time = do_job(jobs[i], jobs[i], jobs[i], 0);
        if (mm_time < 0.0f) {
            return -1.0f;
        }

        printf("Job %d: %dx%d matrices: %.6f seconds (response time %.6f seconds)\n",
               i + 1, jobs[i], jobs[i], mm_time, response);
    }

    return total_response / size;
}

static int compare_ints(const void *a, const void *b) {
    int x = *(const int *)a;
    int y = *(const int *)b;

    return (x > y) - (x < y);
}

float SJF(int* jobs, int size) {
    if (jobs == NULL || size <= 0) {
        return -1.0f;
    }

    int *sorted = malloc(sizeof(int) * size);
    if (sorted == NULL) {
        return -1.0f;
    }
    memcpy(sorted, jobs, sizeof(int) * size);
    qsort(sorted, size, sizeof(int), compare_ints);

    float avg_response = run_jobs(sorted, size);

    free(sorted);
    return avg_response;
}

float FIFO(int* jobs, int size) {
    return run_jobs(jobs, size);
}
