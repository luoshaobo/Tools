#ifndef APP_AFX_H__783472038978276523523489234023589239409237827834923234937842395
#define APP_AFX_H__783472038978276523523489234023589239409237827834923234937842395

#include "TK_Tools.h"
#include "SMF_afx.h"

#define APP_PACKAGE_SIZE_MAX                    4096
#define APP_PACKAGE_MAGIC                       0xFFFEFDFC
#define APP_FILE_PATH_LEN_MAX                   255

enum APP_PackageCommand {
    APP_CMD_NULL = 0,
    APP_CMD_PING_REQUEST,
    APP_CMD_PING_RESPONSE,
    APP_CMD_ECHO_REQUEST,
    APP_CMD_ECHO_RESPONSE,
    APP_CMD_INPUT_REQUEST,
    APP_CMD_INPUT_RESPONSE,
    APP_CMD_OUTPUT_REQUEST,
    APP_CMD_OUTPUT_RESPONSE,
    APP_CMD_GET_FILE_BEGIN_REQUEST,
    APP_CMD_GET_FILE_BEGIN_RESPONSE,
    APP_CMD_GET_FILE_DATA_REQUEST,
    APP_CMD_GET_FILE_DATA_RESPONSE,
    APP_CMD_GET_FILE_END_REQUEST,
    APP_CMD_GET_FILE_END_RESPONSE,
    APP_CMD_PUT_FILE_BEGIN_REQUEST,
    APP_CMD_PUT_FILE_BEGIN_RESPONSE,
    APP_CMD_PUT_FILE_DATA_REQUEST,
    APP_CMD_PUT_FILE_DATA_RESPONSE,
    APP_CMD_PUT_FILE_END_REQUEST,
    APP_CMD_PUT_FILE_END_RESPONSE,
    APP_CMD_COUNT
};

enum APP_PackageStatus {
    APP_STATUS_OK = 0,
    APP_STATUS_ERROR,
    APP_STATUS_COUNT
};

struct APP_PackageHeader {
    SMF_UINT32      nMagic;
    SMF_UINT32      nPackageSize;
    SMF_UINT32      nCheckSum;
    SMF_UINT32      nCommand;
    SMF_UINT32      nStatus;
    SMF_UINT32      nRequestSerialNo;
};

//
// NOTE:
// 1) The size of APP_Package must be less than or equal to APP_PACKAGE_SIZE_MAX bytes.
//
struct APP_Package {
    APP_PackageHeader header;
    union APP_PackageBody {
        struct BodyNull {
            SMF_UINT32 nReserved;
        } stBodyNull;

        struct BodyPingReq {
            SMF_UINT32 nReserved;
        } stBodyPingReq;
        struct BodyPingRes {
            SMF_UINT32 nReserved;
        } stBodyPingRes;

        struct BodyEchoReq {
            SMF_UINT32 nContentLen;
            char arrContent[1];
        } stBodyEchoReq;
        struct BodyEchoRes {
            SMF_UINT32 nContentLen;
            char arrContent[1];
        } stBodyEchoRes;

        struct BodyInputReq {
            SMF_UINT32 nReserved;
        } stBodyInputReq;
        struct BodyInputRes {
            SMF_UINT32 nContentLen;
            char arrContent[1];
        } stBodyInputRes;

        struct BodyOutputReq {
            SMF_UINT32 nContentLen;
            char arrContent[1];
        } stBodyOutputReq;
        struct BodyOutputRes {
            SMF_UINT32 nReserved;
        } stBodyOutputRes;

        struct BodyGetFileBeginReq {
            char szRemoteFilePath[APP_FILE_PATH_LEN_MAX];
        } stBodyGetFileBeginReq;
        struct BodyGetFileBeginRes {
            SMF_BOOL bCanSend;
            SMF_UINT32 nRemoteFileSize;
        } stBodyGetFileBeginRes;
        struct BodyGetFileDataReq {
            SMF_UINT32 nBeginPos;
            SMF_UINT32 nDataSize;
        } stBodyGetFileDataReq;
        struct BodyGetFileDataRes {
            SMF_BOOL bSentOK;
            SMF_UINT32 nBeginPos;
            SMF_UINT32 nDataSize;
            SMF_UINT8 arrData[1];
        } stBodyGetFileDataRes;
        struct BodyGetFileEndReq {
            SMF_UINT32 nReserved;
        } stBodyGetFileEndReq;
        struct BodyGetFileEndRes {
            SMF_UINT32 nReserved;
        } stBodyGetFileEndRes;

        struct BodyPutFileBeginReq {
            SMF_UINT32 nRemoteFileSize;
            char szRemoteFilePath[APP_FILE_PATH_LEN_MAX];
        } stBodyPutFileBeginReq;
        struct BodyPutFileBeginRes {
            SMF_BOOL bCanReceive;
            SMF_UINT32 nRecvBufSize;
        } stBodyPutFileBeginRes;
        struct BodyPutFileDataReq {
            SMF_UINT32 nBeginPos;
            SMF_UINT32 nDataSize;
        } stBodyPutFileDataReq;
        struct BodyPutFileDataRes {
            SMF_BOOL bReceivedOK;
        } stBodyPutFileDataRes;
        struct BodyPutFileEndReq {
            SMF_UINT32 nReserved;
        } stBodyPutFileEndReq;
        struct BodyPutFileEndRes {
            SMF_UINT32 nReserved;
        } stBodyPutFileEndRes;
    } body;
};

interface APP_IOHandler : protected SMF_NonCopyable
{
    virtual int LocalOutput(const std::vector<char> &arrContent, unsigned int nIOCallbackData) = 0;
    virtual int RemoteOutput(const std::vector<char> &arrContent, unsigned int nIOCallbackData) = 0;
};

void InitStateMachines();

#endif // #ifndef APP_AFX_H__783472038978276523523489234023589239409237827834923234937842395
