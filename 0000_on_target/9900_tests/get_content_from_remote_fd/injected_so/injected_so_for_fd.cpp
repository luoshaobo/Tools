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

void output_file_info(const char *filename, int fd)
{
    struct stat s = {0};
    fstat(fd, &s);

    printf("stat: fd=0x%08X, mode=%09o, ino=%lu, dev=0x%08X, rdev=0x%08X, filename=\"%s\"\n",
        fd, s.st_mode, s.st_ino, (unsigned long)s.st_dev, (unsigned long)s.st_rdev, filename);
}

static const char *get_basename(const char *path)
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

static bool get_current_exe_name(std::string &sExeName)
{
    bool bRet = true;
    int nRet;
    pid_t pid = -1;
    char exeLinkBuf[1024] = {0};

    sExeName.clear();

    if (bRet) {
         pid = ::getpid();
         nRet = ::readlink(FormatStr("/proc/%u/exe", pid).c_str(), exeLinkBuf, TK_ARR_LEN(exeLinkBuf));
         if (nRet < 0) {
             bRet = false;
         }
    }

    if (bRet) {
        sExeName = get_basename(exeLinkBuf);
        if (sExeName.empty()) {
            bRet = false;
        }
    }

    return bRet;
}

static std::string get_unix_socket_fd_path()
{
    std::string sResult;
    std::string sExeName;

    get_current_exe_name(sExeName);
    FormatStr(sResult, UNIX_SOCKET_FD_PATH_PATTERN, sExeName.c_str());

    return sResult;
}

static std::string get_unix_socket_cmd_path()
{
    std::string sResult;
    std::string sExeName;

    get_current_exe_name(sExeName);
    FormatStr(sResult, UNIX_SOCKET_CMD_PATH_PATTERN, sExeName.c_str());

    return sResult;
}

static void *thread_proc(void *)
{
    bool bSuc = true;
    std::string sUnixSocketCmdPath;
    std::string sUnixSocketFdPath;
    Socket cmdSockServer;
    Socket fdSockClient;

    sUnixSocketCmdPath = get_unix_socket_cmd_path();
    sUnixSocketFdPath = get_unix_socket_fd_path();

    printf("pid: %lu\n", getpid());
    printf("sUnixSocketCmdPath: %s\n", sUnixSocketCmdPath.c_str());
    printf("sUnixSocketFdPath: %s\n", sUnixSocketFdPath.c_str());

    unlink(sUnixSocketCmdPath.c_str());

    L();
    if (bSuc) {
        L();
        bSuc = cmdSockServer.create(SOCK_DGRAM, 0, AF_UNIX);
        if (!bSuc) {
            LE();
        }
    }

    L();
    if (bSuc) {
        L();
        bSuc = cmdSockServer.bind__unix(sUnixSocketCmdPath.c_str());
        if (!bSuc) {
            LE();
        }
    }

    L();
    if (bSuc) {
        L();
        bSuc = fdSockClient.create(SOCK_DGRAM, 0, AF_UNIX);
        if (!bSuc) {
            LE();
        }
    }

    while (1) {
        char buf[1024] = {0};
        size_t size = TK_ARR_LEN(buf);
        int fd_to_send = -1;

        L();
        if (bSuc) {
            L();
            bSuc = cmdSockServer.recvfrom__unix(buf, size);
            if (!bSuc) {
                LE();
            } else {
                L();
                fd_to_send = StrToL(buf);
                output_file_info("(fd_to_send)", fd_to_send);
            }
        }

        L();
        if (bSuc) {
            L();
            bSuc = fdSockClient.sendmsg_fd__unix(fd_to_send, sUnixSocketFdPath.c_str());
            if (!bSuc) {
                LE();
            }
        }

        if (!bSuc) {
            LE();
            sleep(1);
            bSuc = true;
        }
    }

    L();
    return NULL;
}

__attribute__((constructor))
void loadMsg()
{
    pthread_t thread;

    pthread_create(&thread, NULL, &thread_proc, NULL);
}
