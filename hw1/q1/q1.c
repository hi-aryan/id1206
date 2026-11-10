#include <stdio.h>
#include <unistd.h>

#define N 3

int main(void) {
    for (int i = 0; i < N; i++) {
        fork();
        fork();
    }
    printf("PID %d, parent %d\n", getpid(), getppid());
    sleep(2);   // keep all processes alive briefly so ps/pstree can see them
    return 0;
}