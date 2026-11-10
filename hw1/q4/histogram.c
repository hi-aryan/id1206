#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int num_threads, size, bins; // size and bins get_time_now come from argv
float *array;
int *partial; // one histogram per thread, stored back to back

double get_time_now() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec / 1e9;
}

int bin_of(float x) { // which bin does x belong to?
    int b = x * bins;
    return (b == bins) ? bins - 1 : b; // x = 1.0 goes in the last bin
}

void print_hist(int *h) { // print every bin's range and count
    for (int b = 0; b < bins; b++)
        printf("bin %2d [%.4f, %.4f]: %d\n", b, (double)b / bins, (double)(b + 1) / bins, h[b]);
}

void *thread_func(void *arg) {
    long my_id = (long)arg;
    int chunk = size / num_threads;
    int start = my_id * chunk;
    int end = (my_id == num_threads - 1) ? size : start + chunk;

    int *my_hist = &partial[my_id * bins]; // this thread's own histogram
    for (int i = start; i < end; i++)
        my_hist[bin_of(array[i])]++;
    return NULL;
}

int main(int argc, char *argv[]) {
    num_threads = atoi(argv[1]);
    size = atoi(argv[2]);
    bins = atoi(argv[3]);

    array = malloc(size * sizeof(float));
    for (int i = 0; i < size; i++)
        array[i] = (float)rand() / RAND_MAX;

    /* serial histogram */
    int *hist_serial = calloc(bins, sizeof(int));
    double t = get_time_now();
    for (int i = 0; i < size; i++)
        hist_serial[bin_of(array[i])]++;
    double time_serial = get_time_now() - t;
    printf("Serial histogram:\n");
    print_hist(hist_serial);
    printf("Serial time = %.6f s\n\n", time_serial);

    /* parallel histogram */
    pthread_t *workers = malloc(num_threads * sizeof(pthread_t));
    partial = calloc(num_threads * bins, sizeof(int));
    int *hist_parallel = calloc(bins, sizeof(int));

    t = get_time_now();
    for (long i = 0; i < num_threads; i++)
        pthread_create(&workers[i], NULL, thread_func, (void *)i);
    for (int i = 0; i < num_threads; i++)
        pthread_join(workers[i], NULL);

    for (int i = 0; i < num_threads; i++) // add the partial histograms bin by bin
        for (int b = 0; b < bins; b++)
            hist_parallel[b] += partial[i * bins + b];
    double time_parallel = get_time_now() - t;
    printf("Parallel histogram:\n");
    print_hist(hist_parallel);
    printf("Parallel time = %.6f s\n", time_parallel);

    free(array); free(partial); free(workers);
    free(hist_serial); free(hist_parallel);
    return 0;
}