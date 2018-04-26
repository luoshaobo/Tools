#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "TK_sock.h"

#define P() printf("### %s: %d: %s\n", __FILE__, __LINE__, __FUNCTION__);
#define PE() printf("*** %s :%d: %s\n", __FILE__, __LINE__, __FUNCTION__);

#define TESTFILE                            "/dev/null"
#define UNIX_SOCKET_FD_PATH                 "/tmp/unix_socket__xxx___fd"
#define UNIX_SOCKET_CMD_PATH                "/tmp/unix_socket__xxx___cmd"

using namespace TK_Tools;

void output_file_info(const char *filename, int fd)
{
    struct stat s = {0};
    fstat(fd, &s);

    printf("stat: fd=0x%08X, mode=%09o, ino=%lu, dev=0x%08X, rdev=0x%08X, filename=\"%s\"\n",
        fd, s.st_mode, s.st_ino, (unsigned long)s.st_dev, (unsigned long)s.st_rdev, filename);
}

int client_main()
{
    bool bSuc = true;
    Socket cmdSockServer;
    Socket fdSockClient;
    int fd_to_send = open(TESTFILE, O_RDWR);

    P();
    if (bSuc) {
        P();
        output_file_info("TESTFILE", fd_to_send);
    }

    P();
    if (bSuc) {
        P();
        bSuc = cmdSockServer.create(SOCK_DGRAM, 0, AF_UNIX);
        if (!bSuc) {
            PE();
        }
    }

    P();
    if (bSuc) {
        P();
        bSuc = cmdSockServer.bind__unix(UNIX_SOCKET_CMD_PATH);
        if (!bSuc) {
            PE();
        }
    }

    P();
    if (bSuc) {
        P();
        bSuc = fdSockClient.create(SOCK_DGRAM, 0, AF_UNIX);
        if (!bSuc) {
            PE();
        }
    }

    P();
    if (bSuc) {
        unsigned char buf[1024];
        size_t size = 1024;
        P();
        bSuc = cmdSockServer.recvfrom__unix(buf, size);
        if (!bSuc) {
            PE();
        }
    }

    P();
    if (bSuc) {
        P();
        bSuc = fdSockClient.sendmsg_fd__unix(fd_to_send, UNIX_SOCKET_FD_PATH);
        if (!bSuc) {
            PE();
        }
    }

    P();
    return bSuc ? 0 : -1;
}

int server_main()
{
    bool bSuc = true;
    int fd_received = -1;
    Socket cmdSockClient;
    Socket fdSockSever;

    P();
    if (bSuc) {
        P();
        bSuc = cmdSockClient.create(SOCK_DGRAM, 0, AF_UNIX);
        if (!bSuc) {
            PE();
        }
    }

    P();
    if (bSuc) {
        P();
        bSuc = fdSockSever.create(SOCK_DGRAM, 0, AF_UNIX);
        if (!bSuc) {
            PE();
        }
    }

    P();
    if (bSuc) {
        P();
        bSuc = fdSockSever.bind__unix(UNIX_SOCKET_FD_PATH);
        if (!bSuc) {
            PE();
        }
    }

    usleep(100 * 1000); // wait for the command server to be available

    P();
    if (bSuc) {
        char buf[] = "x";
        size_t size = strlen(buf);
        P();
        bSuc = cmdSockClient.sendto__unix(buf, size, UNIX_SOCKET_CMD_PATH);
        if (!bSuc) {
            PE();
        }
    }

    P();
    if (bSuc) {
        P();
        bSuc = fdSockSever.recvmsg_fd__unix(fd_received);
        if (!bSuc) {
            PE();
        }
    }

    P();
    if (bSuc) {
        P();
        output_file_info("(fd_received)", fd_received);
    }

    P();
    return bSuc ? 0 : -1;
}

int main(int argc, char*argv[])
{
    pid_t child_pid;

    unlink(UNIX_SOCKET_FD_PATH);
    unlink(UNIX_SOCKET_CMD_PATH);

    child_pid = fork();
    if (child_pid == 0) {
        return client_main();
    } else {
        return server_main();
    }
}
