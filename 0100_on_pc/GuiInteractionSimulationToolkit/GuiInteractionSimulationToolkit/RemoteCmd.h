#pragma once
#include "stdafx.h"
#include <string.h>
#include <vector>
#include <string>
#include "TK_Tools.h"
#include "TK_sock.h"

namespace RemoteCmd {

typedef int CmdLineHandlerProc(int argc, char* argv[]);

class RemoteCmdServer
{
public:
    RemoteCmdServer(const std::string &sServerAddr, unsigned short nServerPort);
    ~RemoteCmdServer();

    bool SetCmdLineHandlerProc(CmdLineHandlerProc *pCmdLineHandlerProc);
    bool Run();

    static bool OutputToClientStdout(const std::string &text);
    static bool OutputToClientStderr(const std::string &text);

private:
    bool ProcessRequestPackages();
    bool SendResponsePackage_CMDLINEEND();

public:
    std::string m_sServerAddr;
    unsigned short m_nServerPort;
    CmdLineHandlerProc *m_pCmdLineHandlerProc;
    TK_Tools::Socket m_socket;
    TK_Tools::Socket m_socketComm;
    static TK_Tools::Socket *m_pCurrentCommSocket;
};

class RemoteCmdClient
{
public:
    RemoteCmdClient(const std::string &sServerAddr, unsigned short nServerPort);
    ~RemoteCmdClient();

    bool ExecCmdLineOnServer(int argc, char* argv[]);

private:
    bool ConnectToServer();
    bool ProcessResponsePackages();

private:
    std::string m_sServerAddr;
    unsigned short m_nServerPort;
    TK_Tools::Socket m_socket;
};

} // namespace RemoteCmd {
