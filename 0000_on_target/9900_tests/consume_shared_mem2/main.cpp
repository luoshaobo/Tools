#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>

#define SIZE_1M         (1024 * 1024)

void usage(int argc, char *argv[])
{
    printf("Usage:\n");
    printf("    %s <size_mb> [<key_path>]\n", argv[0]);
}

int main (int argc, char *argv[])
{
    unsigned long mem_size_mb = 1;
    const char *key_path = "/tmp/test_111";
    key_t key;
    int fd = -1;
    unsigned int i;
    char cmdBuf[1024] = {0};
    
    //printf("INFO: begin...\n");

    if (argc >= 2) {
        mem_size_mb = strtoul(argv[1], NULL, 0);
    } else {
        usage(argc, argv);
        exit(0);
    }
    
    if (argc >= 3) {
        key_path = argv[2];
    }
    
    printf("INFO: %d MB to be allocated!\n", mem_size_mb);
    
    snprintf(cmdBuf, sizeof(cmdBuf), "touch %s", key_path);
    system(cmdBuf);
    
    key = ftok(key_path, 1);
    if (key == -1) {
        printf("*** ERROR: ftok(\"%s\", 1) is failed. error: %s\n", key_path, strerror(errno));
        exit(1);
    }
    
    {
        shmctl(key, IPC_RMID, NULL);
    }
    
    fd = shmget(key, mem_size_mb * SIZE_1M, IPC_CREAT | IPC_EXCL);
    if (fd < 0) {
        printf("*** ERROR: shmget(key, mem_size_mb, IPC_CREAT | IPC_EXCL) is failed. error: %s\n", strerror(errno));
        exit(1);
    }
    ftruncate(fd, mem_size_mb);

    //unsigned long mem_allocated_mb = 0;
    //while (mem_allocated_mb < mem_size_mb) {
        void *p = shmat(fd, NULL, SHM_RDONLY);
        //void *p = mmap(NULL, SIZE_1M, PROT_READ, MAP_SHARED, fd, mem_allocated_mb * SIZE_1M);
        if (p == NULL) {
            printf("*** ERROR: shmat() is failed\n");
            //break;
            exit(1);
        }
        //printf("INFO: mem_allocated_mb=%d\n!\n", mem_allocated_mb);
        //mem_allocated_mb++;
    //}
    //printf("INFO: mem_allocated_mb=%luMB\n", mem_allocated_mb);
    printf("INFO: mem_size_mb=%luMB\n", mem_size_mb);
    
    printf("Press any key to exit...");
    getchar();
    
    return 0;
}

