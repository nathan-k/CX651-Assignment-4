#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

void generate_random_matrix(int rows, int cols, int *matrix) {
    for (int i = 0; i < rows * cols; i++) {
        matrix[i] = rand() % 100;
    }
}

void multiply_matrices(int rows1, int cols1, int *matrix1,
                       int rows2, int cols2, int *matrix2,
                       int *result) {

    for (int i = 0; i < rows1; i++) {
        for (int j = 0; j < cols2; j++) {
            int sum = 0;
            for (int k = 0; k < cols1; k++) {
                sum += matrix1[i * cols1 + k] * matrix2[k * cols2 + j];
            }
            result[i * cols2 + j] = sum;
        }
    }
}

void display_matrix(int rows, int cols, int *matrix) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%d ", matrix[i * cols + j]);
        }
        printf("\n");
    }
}

float do_job(int rows1, int cols1, int cols2, int forever) {
    int *matrix1 = malloc(sizeof(int) * rows1 * cols1);
    int *matrix2 = malloc(sizeof(int) * cols1 * cols2);
    int *result = malloc(sizeof(int) * rows1 * cols2);

    if (!matrix1 || !matrix2 || !result) {
        fprintf(stderr, "Failed to allocate matrices\n");
        free(matrix1);
        free(matrix2);
        free(result);
        return -1.0f;
    }

    srand(time(NULL));

    generate_random_matrix(rows1, cols1, matrix1);
    generate_random_matrix(cols1, cols2, matrix2);

    float total_time;
    do {
        struct timespec t0, t1;

        timespec_get(&t0, TIME_UTC);
        multiply_matrices(rows1, cols1, matrix1, cols1, cols2, matrix2, result);
        timespec_get(&t1, TIME_UTC);

        float dns = (float)(t1.tv_nsec - t0.tv_nsec) / 1000000000;
        float ds = (float)(t1.tv_sec - t0.tv_sec);
        total_time = dns + ds;

        if (forever) {
            time_t now = time(NULL);
            char *stamp = ctime(&now);
            stamp[strlen(stamp) - 1] = '\0';  // drop ctime's trailing newline
            printf("%s %dx%d matrices: %.6f seconds\n",
                   stamp, rows1, cols1, total_time);
            fflush(stdout);
        }
    } while (forever);

    free(matrix1);
    free(matrix2);
    free(result);

    return total_time;
}
