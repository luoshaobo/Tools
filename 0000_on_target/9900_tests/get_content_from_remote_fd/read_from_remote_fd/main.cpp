#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <dlfcn.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include "TK_sock.h"
#include "TK_Tools.h"

#define L() printf("### %s: %d: %s\n", __FILE__, __LINE__, __FUNCTION__);
#define LE() printf("*** %s :%d: %s\n", __FILE__, __LINE__, __FUNCTION__);

#define UNIX_SOCKET_FD_PATH_PATTERN                 "/tmp/unix_socket__%s__fd"
#define UNIX_SOCKET_CMD_PATH_PATTERN                "/tmp/unix_socket__%s__cmd"

using namespace TK_Tools;

static std::string sRemoteExeName;

void output_file_info(const char *filename, int fd)
{
    struct stat s = {0};
    fstat(fd, &s);

    printf("stat: fd=0x%08X, mode=%09o, ino=%lu, dev=0x%08X, rdev=0x%08X, filename=\"%s\"\n",
        fd, s.st_mode, s.st_ino, (unsigned long)s.st_dev, (unsigned long)s.st_rdev, filename);
}

static std::string get_unix_socket_fd_path()
{
    std::string sResult;

    FormatStr(sResult, UNIX_SOCKET_FD_PATH_PATTERN, sRemoteExeName.c_str());

    return sResult;
}

static std::string get_unix_socket_cmd_path()
{
    std::string sResult;

    FormatStr(sResult, UNIX_SOCKET_CMD_PATH_PATTERN, sRemoteExeName.c_str());

    return sResult;
}

const char *basename(const char *path)
{
    const char *pRet = path;

    if (path != NULL) {
        char *p = strrchr((char *)path, '/');
        if (p != NULL) {
            pRet = (const char *)(p + 1);
        }
    }

    return pRet;
}

void usage(int argc, char *argv[])
{
    TK_UNUSED_VAR(argc);

    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "    %s <remote_exe_name> <remote_fd> <file_path_to_write>"
            "\n",
            basename(argv[0]));
}

int main(int argc, char *argv[])
{
    bool bSuc = true;
    int fd_received = -1;
    std::string sUnixSocketCmdPath;
    std::string sUnixSocketFdPath;
    Socket cmdSockClient;
    Socket fdSockSever;
    std::string remote_exe_name;
    int remote_fd = -1;
    std::string file_path_to_write;
    std::vector<unsigned char> vFileContent;

    if (argc < 4) {
        usage(argc, argv);
        exit(1);
    }

    remote_exe_name = argv[1];
    remote_fd = StrToL(argv[2]);
    file_path_to_write = argv[3];

    fd_received = remote_fd;

    sRemoteExeName = remote_exe_name;
    sUnixSocketCmdPath = get_unix_socket_cmd_path();
    sUnixSocketFdPath = get_unix_socket_fd_path();

    printf("sUnixSocketCmdPath: %s\n", sUnixSocketCmdPath.c_str());
    printf("sUnixSocketFdPath: %s\n", sUnixSocketFdPath.c_str());

    unlink(sUnixSocketFdPath.c_str());

    L();
    if (bSuc) {
        L();
        bSuc = cmdSockClient.create(SOCK_DGRAM, 0, AF_UNIX);
        if (!bSuc) {
            LE();
        }
    }

    L();
    if (bSuc) {
        L();
        bSuc = fdSockSever.create(SOCK_DGRAM, 0, AF_UNIX);
        if (!bSuc) {
            LE();
        }
    }

    L();
    if (bSuc) {
        L();
        bSuc = fdSockSever.bind__unix(sUnixSocketFdPath.c_str());
        if (!bSuc) {
            LE();
        }
    }

    L();
    if (bSuc) {
        char buf[1024] = {0};
        snprintf(buf, TK_ARR_LEN(buf), "%ld", fd_received);
        size_t size = strlen(buf);
        L();
        bSuc = cmdSockClient.sendto__unix(buf, size, sUnixSocketCmdPath.c_str());
        if (!bSuc) {
            LE();
        }
    }

    L();
    if (bSuc) {
        L();
        bSuc = fdSockSever.recvmsg_fd__unix(fd_received);
        if (!bSuc) {
            LE();
        }
    }

    L();
    if (bSuc) {
        L();
        output_file_info("(fd_received)", fd_received);
    }

    L();
    if (bSuc) {
        L();
        bSuc = GetContentFromFile(fd_received, vFileContent);
        if (!bSuc) {
            LE();
        }
    }

    L();
    if (bSuc) {
        L();
        bSuc = SaveContentToFile(file_path_to_write, vFileContent);
        if (!bSuc) {
            LE();
        }
    }

    L();

    return bSuc ? 0 : -1;
}
