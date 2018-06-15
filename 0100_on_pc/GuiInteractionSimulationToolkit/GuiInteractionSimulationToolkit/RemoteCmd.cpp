#include "RemoteCmd.h"

//////////////////////////////////////////////////////////////////////////////////////////////

namespace RemoteCmd {

const unsigned char PACKAGE_MAGIC[] = "!@#$%^&*";
const unsigned int PACKAGE_MAGIC_LEN = 8;

const char ARGUMENT_SEPERATOR_CHAR = '\1';

enum RequestId {
    REQID_EXECCMDLINE = 0,
    REQID_MAX
};

enum ResponseId {
    RESID_OUTTOSTDOUT = 0,
    RESID_OUTTOSTDERR,
    RESID_CMDLINEEND,
    RESID_MAX
};

struct PackageHead {
    unsigned char magic[PACKAGE_MAGIC_LEN];
    unsigned int nPackageSize;
};

struct RequestPackageBody {
    RequestId id;
    unsigned int nContentSize;
    unsigned char content[1];
};

struct ResponsePackageBody {
    ResponseId id;
    unsigned int nContentSize;
    unsigned char content[1];
};

TK_Tools::Socket *RemoteCmdServer::m_pCurrentCommSocket = NULL;

//////////////////////////////////////////////////////////////////////////////////////////////

RemoteCmdServer::RemoteCmdServer(const std::string &sServerAddr, unsigned short nServerPort) 
    : m_sServerAddr(sServerAddr), m_nServerPort(nServerPort), m_pCmdLineHandlerProc(NULL), m_socket(), m_socketComm()
{

}

RemoteCmdServer::~RemoteCmdServer()
{

}

bool RemoteCmdServer::SetCmdLineHandlerProc(CmdLineHandlerProc *pCmdLineHandlerProc)
{
    bool bSuc = true;

    if (bSuc) {
        m_pCmdLineHandlerProc = pCmdLineHandlerProc;
    }

    return bSuc;
}

bool RemoteCmdServer::Run()
{
    bool bSuc = true;
    bool bRunning = true;

    if (bSuc) {
        if (!m_socket.create()) {
            LOG_GEN();//////
            bSuc = false;
        }
    }

    if (bSuc) {
        m_socket.reuse_addr();
    }

    if (bSuc) {
        if (!m_socket.bind(m_nServerPort, m_sServerAddr.c_str())) {
            LOG_GEN();//////
            bSuc = false;
        }
    }

    while (bSuc && bRunning) {
        if (bSuc) {
            if (!m_socket.listen()) {
                LOG_GEN();//////
                bSuc = false;
            }
        }

        if (bSuc) {
            if (!m_socket.accept(m_socketComm)) {
                LOG_GEN();//////
                bSuc = false;
            } else {
                m_pCurrentCommSocket = &m_socketComm;
            }
        }
        
        if (bSuc) {
            if (!ProcessRequestPackages()) {
                LOG_GEN();//////
                bSuc = false;
            }
        }

        m_pCurrentCommSocket = NULL;
    }

    return bSuc;
}

bool RemoteCmdServer::OutputToClientStdout(const std::string &text)
{
    bool bSuc = true;
    PackageHead *pPackageHead = NULL;
    ResponsePackageBody *pResponsePackageBody = NULL;
    unsigned int nPackageSize = 0;

    if (bSuc) {
        if (m_pCurrentCommSocket == NULL) {
            LOG_GEN();//////
            bSuc = false;
        }
    }

    if (bSuc) {
        nPackageSize = sizeof(pPackageHead) + sizeof(ResponsePackageBody) - 1 + text.length();
        pPackageHead = (PackageHead *)new unsigned char [nPackageSize];
        if (pPackageHead == NULL) {
            LOG_GEN();//////
            bSuc = false;
        }
    }

    if (bSuc) {
        memset(pPackageHead, 0, nPackageSize);
        memcpy(&pPackageHead->magic, PACKAGE_MAGIC, PACKAGE_MAGIC_LEN);
        pPackageHead->nPackageSize = nPackageSize;

        pResponsePackageBody = (ResponsePackageBody *)&pPackageHead[1];
        pResponsePackageBody->id = RESID_OUTTOSTDOUT;
        pResponsePackageBody->nContentSize = text.length();
        memcpy(&pResponsePackageBody->content, text.c_str(), pResponsePackageBody->nContentSize);
    }

    if (bSuc) {
        size_t size = nPackageSize;
        if (!m_pCurrentCommSocket->write((void *)pPackageHead, size)) {
            LOG_GEN();//////
            bSuc = false;
        } else {
            if (size != nPackageSize) {
                LOG_GEN();//////
                bSuc = false;
            }
        }
    }

    if (pResponsePackageBody != NULL) {
        delete [] (unsigned char *)pPackageHead;
        pPackageHead = NULL;
        pResponsePackageBody = NULL;
    }

    return bSuc;
}

bool RemoteCmdServer::OutputToClientStderr(const std::string &text)
{
    bool bSuc = true;
    PackageHead *pPackageHead = NULL;
    ResponsePackageBody *pResponsePackageBody = NULL;
    unsigned int nPackageSize = 0;

    if (bSuc) {
        if (m_pCurrentCommSocket == NULL) {
            LOG_GEN();//////
            bSuc = false;
        }
    }

    if (bSuc) {
        nPackageSize = sizeof(pPackageHead) + sizeof(ResponsePackageBody) - 1 + text.length();
        pPackageHead = (PackageHead *)new unsigned char [nPackageSize];
        if (pPackageHead == NULL) {
            LOG_GEN();//////
            bSuc = false;
        }
    }

    if (bSuc) {
        memset(pPackageHead, 0, nPackageSize);
        memcpy(&pPackageHead->magic, PACKAGE_MAGIC, PACKAGE_MAGIC_LEN);
        pPackageHead->nPackageSize = nPackageSize;

        pResponsePackageBody = (ResponsePackageBody *)&pPackageHead[1];
        pResponsePackageBody->id = RESID_OUTTOSTDERR;
        pResponsePackageBody->nContentSize = text.length();
        memcpy(&pResponsePackageBody->content, text.c_str(), pResponsePackageBody->nContentSize);
    }

    if (bSuc) {
        size_t size = nPackageSize;
        if (!m_pCurrentCommSocket->write((void *)pPackageHead, size)) {
            LOG_GEN();//////
            bSuc = false;
        } else {
            if (size != nPackageSize) {
                LOG_GEN();//////
                bSuc = false;
            }
        }
    }

    if (pPackageHead != NULL) {
        delete [] (unsigned char *)pPackageHead;
        pPackageHead = NULL;
        pResponsePackageBody = NULL;
    }

    return bSuc;
}

bool RemoteCmdServer::SendResponsePackage_CMDLINEEND()
{
    bool bSuc = true;
    PackageHead *pPackageHead = NULL;
    ResponsePackageBody *pResponsePackageBody = NULL;
    unsigned int nPackageSize = 0;

    if (bSuc) {
        if (m_pCurrentCommSocket == NULL) {
            LOG_GEN();//////
            bSuc = false;
        }
    }

    if (bSuc) {
        nPackageSize = sizeof(pPackageHead) + sizeof(ResponsePackageBody) - 1;
        pPackageHead = (PackageHead *)new unsigned char [nPackageSize];
        if (pPackageHead == NULL) {
            LOG_GEN();//////
            bSuc = false;
        }
    }

    if (bSuc) {
        memset(pPackageHead, 0, nPackageSize);
        memcpy(&pPackageHead->magic, PACKAGE_MAGIC, PACKAGE_MAGIC_LEN);
        pPackageHead->nPackageSize = nPackageSize;

        pResponsePackageBody = (ResponsePackageBody *)&pPackageHead[1];
        pResponsePackageBody->id = RESID_CMDLINEEND;
        pResponsePackageBody->nContentSize = 0;
    }

    if (bSuc) {
        size_t size = nPackageSize;
        if (!m_pCurrentCommSocket->write((void *)pPackageHead, size)) {
            LOG_GEN();//////
            bSuc = false;
        } else {
            if (size != nPackageSize) {
                LOG_GEN();//////
                bSuc = false;
            }
        }
    }

    if (pPackageHead != NULL) {
        delete [] (unsigned char *)pPackageHead;
        pPackageHead = NULL;
        pResponsePackageBody = NULL;
    }

    return bSuc;
}

bool RemoteCmdServer::ProcessRequestPackages()
{
    bool bSuc = true;
    PackageHead packageHead;
    RequestPackageBody *pRequestPackageBody = NULL;
    bool bWaitingForInput = true;

    if (bSuc) {
        if (m_pCmdLineHandlerProc == NULL) {
            LOG_GEN();//////
            bSuc = false;
        }
    }

    while (bSuc && bWaitingForInput) {
        if (bSuc) {
            size_t size = sizeof(PackageHead);
            if (!m_socketComm.read((void *)&packageHead, size)) {
                LOG_GEN();//////
                bSuc = false;
            } else {
                if (size != sizeof(PackageHead)) {
                    LOG_GEN();//////
                    bSuc = false;
                } else {
                    if (memcmp(&packageHead.magic, PACKAGE_MAGIC, PACKAGE_MAGIC_LEN) != 0) {
                        LOG_GEN();//////
                        bSuc = false;
                    }
                }
            }
        }

        if (bSuc) {
            pRequestPackageBody = (RequestPackageBody *)new unsigned char [packageHead.nPackageSize - sizeof(PackageHead)];
            if (pRequestPackageBody == NULL) {
                LOG_GEN();//////
                bSuc = false;
            }
        }

        if (bSuc) {
            size_t size = packageHead.nPackageSize - sizeof(PackageHead);
            if (!m_socket.read((void *)pRequestPackageBody, size)) {
                int error = ::GetLastError();
                LOG_GEN();//////
                bSuc = false;
            } else {
                if (size != packageHead.nPackageSize - sizeof(PackageHead)) {
                    LOG_GEN();//////
                    bSuc = false;
                }
            }
        }

        if (bSuc) {
            switch (pRequestPackageBody->id) {
            case REQID_EXECCMDLINE:
                {
                    std::string s((const char *)&pRequestPackageBody->content, pRequestPackageBody->nContentSize);
                    std::vector<std::string> arguments = TK_Tools::SplitString(s, std::string() + ARGUMENT_SEPERATOR_CHAR);
                    int argc = arguments.size();
                    char** argv = new char * [argc];
                    for (int i = 0; i < argc; ++i) {
                        argv[i] = (char *)arguments[i].c_str();
                    }
                    m_pCmdLineHandlerProc(argc, argv);
                    delete [] argv;
                    SendResponsePackage_CMDLINEEND();
                    bWaitingForInput = false;
                }
                break;
            default:
                {
                    LOG_GEN();//////
                    bWaitingForInput = false;
                    bSuc = false;
                }
                break;
            }
        }

        if (pRequestPackageBody != NULL) {
            delete [] (unsigned char *)pRequestPackageBody;
            pRequestPackageBody = NULL;
        }
    }

    return bSuc;
}

//////////////////////////////////////////////////////////////////////////////////////////////

RemoteCmdClient::RemoteCmdClient(const std::string &sServerAddr, unsigned short nServerPort)
    : m_sServerAddr(sServerAddr), m_nServerPort(nServerPort), m_socket()
{

}

RemoteCmdClient::~RemoteCmdClient()
{

}

bool RemoteCmdClient::ConnectToServer()
{
    bool bSuc = true;

    if (bSuc) {
        if (!m_socket.create()) {
            LOG_GEN();//////
            bSuc = false;
        }
    }

    if (bSuc) {
        if (!m_socket.connect(m_sServerAddr.c_str(), m_nServerPort)) {
            LOG_GEN();//////
            bSuc = false;
        }
    }

    return bSuc;
}

bool RemoteCmdClient::ExecCmdLineOnServer(int argc, char* argv[])
{
    bool bSuc = true;
    std::string sArguments;
    int i;
    PackageHead *pPackage = NULL;
    unsigned int nPackageSize = 0;
    RequestPackageBody *pRequestPackageBody = NULL;

    if (bSuc) {
        for (i = 0; i < argc; ++i) {
            if (i > 0) {
                sArguments += ARGUMENT_SEPERATOR_CHAR;
            }
            sArguments += argv[i];
        }
    }

    if (bSuc) {
        if (!ConnectToServer()) {
            LOG_GEN();//////
            bSuc = false;
        }
    }

    if (bSuc) {
        nPackageSize = sizeof(PackageHead) + sizeof(RequestPackageBody) - 1 + sArguments.length();
        pPackage = (PackageHead *)new unsigned char [nPackageSize];
        if (pPackage == NULL) {
            LOG_GEN();//////
            bSuc = false;
        }
    }

    if (bSuc) {
        memset(pPackage, 0, nPackageSize);
        memcpy(&pPackage->magic, PACKAGE_MAGIC, PACKAGE_MAGIC_LEN);
        pPackage->nPackageSize = nPackageSize;

        pRequestPackageBody = (RequestPackageBody *)&pPackage[1];
        pRequestPackageBody->id = REQID_EXECCMDLINE;
        pRequestPackageBody->nContentSize = sArguments.length();
        memcpy(&pRequestPackageBody->content, sArguments.c_str(), pRequestPackageBody->nContentSize);
    }

    if (bSuc) {
        size_t size = nPackageSize;
        if (!m_socket.write((const void *)pPackage, size)) {
            LOG_GEN();//////
            bSuc = false;
        } else {
            if (size != nPackageSize) {
                LOG_GEN();//////
                bSuc = false;
            }
        }
    }

    if (pPackage != NULL) {
        delete [] (unsigned char *)pPackage;
        pPackage = NULL;
        pRequestPackageBody = NULL;
    }

    if (bSuc) {
        if (!ProcessResponsePackages()) {
            LOG_GEN();//////
            bSuc = false;
        }
    }

    return bSuc;
}

bool RemoteCmdClient::ProcessResponsePackages()
{
    bool bSuc = true;
    PackageHead packageHead;
    ResponsePackageBody *pResponsePackageBody = NULL;
    bool bWaitingForOutput = true;

    while (bSuc && bWaitingForOutput) {
        if (bSuc) {
            size_t size = sizeof(PackageHead);
            if (!m_socket.read((void *)&packageHead, size)) {
                LOG_GEN();//////
                bSuc = false;
            } else {
                if (size != sizeof(PackageHead)) {
                    LOG_GEN();//////
                    bSuc = false;
                } else {
                    if (memcmp(&packageHead.magic, PACKAGE_MAGIC, PACKAGE_MAGIC_LEN) != 0) {
                        LOG_GEN();//////
                        bSuc = false;
                    }
                }
            }
        }

        if (bSuc) {
            pResponsePackageBody = (ResponsePackageBody *)new unsigned char [packageHead.nPackageSize - sizeof(PackageHead)];
            size_t size = packageHead.nPackageSize - sizeof(PackageHead);
            if (!m_socket.read((void *)pResponsePackageBody, size)) {
                LOG_GEN();//////
                bSuc = false;
            } else {
                if (size != packageHead.nPackageSize - sizeof(PackageHead)) {
                    LOG_GEN();//////
                    bSuc = false;
                }
            }
        }

        if (bSuc) {
            switch (pResponsePackageBody->id) {
            case RESID_OUTTOSTDOUT:
                {
                    std::string s((const char *)&pResponsePackageBody->content, pResponsePackageBody->nContentSize);
                    fprintf(stdout, s.c_str());
                }
                break;
            case RESID_OUTTOSTDERR:
                {
                    std::string s((const char *)&pResponsePackageBody->content, pResponsePackageBody->nContentSize);
                    fprintf(stderr, s.c_str());
                }
                break;
            case RESID_CMDLINEEND:
                {
                    bWaitingForOutput = false;
                }
                break;
            default:
                {
                    LOG_GEN();//////
                    bWaitingForOutput = false;
                    bSuc = false;
                }
                break;
            }
        }

        if (pResponsePackageBody != NULL) {
            delete [] (unsigned char *)pResponsePackageBody;
            pResponsePackageBody = NULL;
        }
    }

    return bSuc;
}

} // namespace RemoteCmd {

//////////////////////////////////////////////////////////////////////////////////////////////

void local_fprintf(FILE *stream, const char *pFormat, ...)
{
    int nError = 0;
    va_list args;
    int len = 0;
    char *buffer = NULL;

    if (nError == 0) {
        if (pFormat == NULL) {
            nError = 1;
        }
    }

    if (nError == 0) {
        va_start(args, pFormat);
        len = vsnprintf(NULL, 0, pFormat, args);
        if (len <= 0) {
            len = 1024;
        }
        buffer = (char *)malloc((len + 1) * sizeof(char));
        if (buffer == NULL) {
           nError = -1;
        }
        va_end(args);
    }

    if (nError == 0) {
        va_start(args, pFormat);
        vsnprintf(buffer, len + 1, pFormat, args);
        buffer[len] = '\0';
        
        if (stream == stdout) {
            if (!RemoteCmd::RemoteCmdServer::OutputToClientStdout(buffer)) {
                fprintf(stdout, buffer);
            }
        } else if (stream == stderr) {
            if (!RemoteCmd::RemoteCmdServer::OutputToClientStderr(buffer)) {
                fprintf(stderr, buffer);
            }
        } else {
            if (!RemoteCmd::RemoteCmdServer::OutputToClientStderr(buffer)) {
                fprintf(stderr, buffer);
            }
        }

        va_end(args);
    }

    if (buffer != NULL) {
        free(buffer);
        buffer = NULL;
    }

    return;
}
