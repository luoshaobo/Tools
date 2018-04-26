#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>

#define SIZE_1M         (1024 * 1024)

void usage(int argc, char *argv[])
{
    printf("Usage:\n");
    printf("    %s <size_mb>\n", argv[0]);
}

int main (int argc, char *argv[])
{
    unsigned long mem_size_mb = 1;
    int fd = -1;
    unsigned int i;
    
    //printf("INFO: begin...\n");

    if (argc >= 2) {
        mem_size_mb = strtoul(argv[1], NULL, 0);
    } else {
        usage(argc, argv);
        exit(0);
    }
    
    printf("INFO: %d MB to be allocated!\n", mem_size_mb);
    
    fd = shm_open("/test_111", O_CREAT|O_RDWR, 0777);
    if (fd < 0) {
        printf("*** ERROR: shm_open(\"/tmp/test_111\", O_CREAT|O_RDWR, 0777) is failed\n");
        exit(1);
    }
    ftruncate(fd, mem_size_mb);

    unsigned long mem_allocated_mb = 0;
    while (mem_allocated_mb < mem_size_mb) {
        void *p = mmap(NULL, SIZE_1M, PROT_READ, MAP_SHARED, fd, mem_allocated_mb * SIZE_1M);
        if (p == NULL) {
            printf("*** ERROR: malloc() is failed\n");
            break;
        }
        //printf("INFO: mem_allocated_mb=%d\n!\n", mem_allocated_mb);
        mem_allocated_mb++;
    }
    printf("INFO: mem_allocated_mb=%luMB\n", mem_allocated_mb);
    
    printf("Press any key to exit...");
    getchar();
    
    return 0;
}

