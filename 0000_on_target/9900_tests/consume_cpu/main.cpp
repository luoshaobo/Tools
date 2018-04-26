#define __USE_GNU

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <sched.h>

#define SIZE_1M         (1024 * 1024)

void usage(int argc, char *argv[])
{
    printf("Usage:\n");
    printf("    %s <thread_count> <nice> <cpu_index>\n", argv[0]);
}

int nice_value = 0;
unsigned int cpu_index = -1;

void *WorkThreadProc(void *)
{
    unsigned int i;
    
    nice(nice_value);
    
    if (cpu_index != -1) {
        int cpu_id = cpu_index;
        cpu_set_t mask;
        
        CPU_ZERO(&mask);
        CPU_SET(cpu_id, &mask);
        if (sched_setaffinity(0, sizeof(mask), &mask) < 0) {
            fprintf(stderr, "*** sched_setaffinity() is failed. Error: %s\n", strerror(errno));
        }
    }
    
    for (;;) {
        i++;
    }
    return (void *)i;
}

int main (int argc, char *argv[])
{
    unsigned long thread_count = 1;
    
    if (argc >= 2) {
        thread_count = strtoul(argv[1], NULL, 0);
        printf("INFO: thread_count=%d\n", thread_count);
    } else {
        usage(argc, argv);
        exit(0);
    }
    
    if (argc >= 3) {
        nice_value = strtol(argv[2], NULL, 0);
        printf("INFO: nice_value=%d\n", nice_value);
    }
    
    if (argc >= 4) {
        cpu_index = strtoul(argv[3], NULL, 0);
        printf("INFO: cpu_index=%d\n", cpu_index);
    }
    
    for (int i = 0; i < thread_count; i++) {
        pthread_t thread;
        pthread_create(&thread, NULL, &WorkThreadProc, NULL);
    }

    for (;;) {
        sleep(1);
    }
    
    return 0;
}

