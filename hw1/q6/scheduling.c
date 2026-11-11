#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

#define NUM_THREADS 8

typedef struct {
    int id;
    int priority;      // lower is more important
    int burst_time;    // CPU burst time
    int remaining;     // remaining time
} thread_info_t;

thread_info_t threads[NUM_THREADS];

// Global scheduler lock: only one thread "on CPU" at a time
pthread_mutex_t sched_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  sched_cond = PTHREAD_COND_INITIALIZER;
int current_running_tid = -1;   // the id of thread allowed to run

int clock_time = 0; // simulated clock (time units)
int finish_time[NUM_THREADS]; // when each thread finished

int pick_next() {
    int best = -1;
    for (int i = 0; i < NUM_THREADS; i++) {
        if (threads[i].remaining > 0) {
            // TODO: choose the thread with the highest priority.
            if (best == -1 || threads[i].priority < threads[best].priority)
                best = i;
        }
    }
    return best;
}

void schedule() {
    pthread_mutex_lock(&sched_lock);
    current_running_tid = pick_next();
    pthread_cond_broadcast(&sched_cond);
    pthread_mutex_unlock(&sched_lock);
}

void *thread_func(void *arg) {
    thread_info_t *t = (thread_info_t *)arg;

    while (1) {
        pthread_mutex_lock(&sched_lock);
        while (current_running_tid != t->id && t->remaining > 0)
            pthread_cond_wait(&sched_cond, &sched_lock);

        if (t->remaining <= 0) {
            pthread_mutex_unlock(&sched_lock);
            break;
        }

        printf("Thread %d (prio %d) runs for %d units\n",
               t->id, t->priority, t->remaining);
        clock_time += t->remaining; // time moves forward by the burst
        finish_time[t->id] = clock_time; // record when this thread finished
        t->remaining = 0;
        pthread_mutex_unlock(&sched_lock);

        schedule();  // pick the next thread
    }
    return NULL;
}

int main() {
    pthread_t tids[NUM_THREADS];
    int prios[]  = {3, 3, 2, 2, 2, 6, 7, 1}; // lower has higher priority
    int bursts[] = {7, 6, 2, 8, 7, 1, 1, 9};

    for (int i = 0; i < NUM_THREADS; i++) {
        threads[i].id = i;
        threads[i].priority = prios[i];
        threads[i].burst_time = threads[i].remaining = bursts[i];
        pthread_create(&tids[i], NULL, thread_func, &threads[i]);
    }

    sleep(1);
    schedule();     // kick off scheduling

    for (int i = 0; i < NUM_THREADS; i++)
        pthread_join(tids[i], NULL);

    // TODO: print each thread's waiting time and turnaround time.
    double total_wait = 0, total_tat = 0;
    for (int i = 0; i < NUM_THREADS; i++) {
        int tat  = finish_time[i];               // turnaround = finish - arrival (0)
        int wait = tat - threads[i].burst_time;  // waiting = turnaround - burst
        printf("Thread %d: waiting = %d, turnaround = %d\n", i, wait, tat);
        total_wait += wait;  
        total_tat  += tat; 
    }

    // TODO 3: print the average waiting time and turnaround time.
    printf("Average waiting = %.2f, average turnaround = %.2f\n",
           total_wait / NUM_THREADS, total_tat / NUM_THREADS);

    return 0;
}