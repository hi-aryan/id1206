#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>                                  // ADDED [1]

#define SIZE 1000301                               // ADDED [2]

int num_threads = 0;
float *array;                                      // ADDED [3]
double *partial;                                   // ADDED [4]

double get_time_now() {                                     // ADDED [5]
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec / 1e9;
}

void *thread_func(void *arg); /* the thread function DECLARATION */   // CHANGED [6]

int main(int argc, char *argv[])
{
     num_threads = atoi(argv[1]); // first command line arg to var

     /* Initialize an array of random values */
     array = malloc(SIZE * sizeof(float));                 // ADDED [7]
     for (int i = 0; i < SIZE; i++)                        // ADDED [7]
          array[i] = (float)rand() / RAND_MAX;             // ADDED [7]

     /* Perform Serial Sum */
     double sum_serial = 0.0;                              // CHANGED [8]
     double time_serial = 0.0;
     //Timer Begin
     double t = get_time_now();                                     // ADDED [9]
     for (int i = 0; i < SIZE; i++)                        // ADDED [10]
          sum_serial += array[i];                          // ADDED [10]
     //Timer End
     time_serial = get_time_now() - t;                              // ADDED [9]
     printf("Serial Sum = %f, time = %.6f \n", sum_serial, time_serial);   // CHANGED [11]

     /* Create a pool of num_threads workers and keep them in workers.
     allocates memory for a so far unknown number of threads (# is given from argv[1]) */
     pthread_t *workers = malloc(num_threads * sizeof(pthread_t));   // CHANGED [12]
     partial = malloc(num_threads * sizeof(double));                // ADDED [4]
     double time_parallel = 0.0;
     double sum_parallel = 0.0;

     //Timer Begin
     t = get_time_now();                                            // ADDED [9]

     for (long i = 0; i < num_threads; i++) {              // CHANGED [13]
          pthread_attr_t attr;
          pthread_attr_init(&attr);
          pthread_create(&workers[i], &attr, thread_func, (void *)i);   // CHANGED [13]
     }

     for (int i = 0; i < num_threads; i++)
          pthread_join(workers[i], NULL);

     for (int i = 0; i < num_threads; i++)                 // ADDED [14]
          sum_parallel += partial[i];                      // ADDED [14]

     //Timer End
     time_parallel = get_time_now() - t;                            // ADDED [9]
     printf("Parallel Sum = %f, time = %.6f \n", sum_parallel, time_parallel);   // CHANGED [11]

     /*free up resources properly */
     free(array);                                          // ADDED [15]
     free(workers);                                        // ADDED [15]
     free(partial);                                        // ADDED [15]
     return 0;                                             // ADDED [15]
}

void *thread_func(void *arg) {                             // CHANGED [6]
     /* Assign each thread an id so that they are unique in range [0, num_thread -1 ] */
     long my_id = (long)arg;                               // CHANGED [13]

     int chunk = SIZE / num_threads;                       // ADDED [16]
     int start = my_id * chunk;                            // ADDED [16]
     int end = (my_id == num_threads - 1) ? SIZE : start + chunk;   // ADDED [16]

     /* Perform Partial Parallel Sum Here */
     double my_sum = 0.0;                                  // CHANGED [8]
     for (int i = start; i < end; i++)                     // ADDED [16]
          my_sum += array[i];                              // ADDED [16]
     partial[my_id] = my_sum;                              // ADDED [4]

     printf("Thread %ld sum = %f\n", my_id, my_sum);       // CHANGED [13]
     pthread_exit(0);
}