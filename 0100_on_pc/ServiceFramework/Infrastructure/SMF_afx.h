#ifndef SMF_AFX_H__782384237643289472376234623791489235234892385475
#define SMF_AFX_H__782384237643289472376234623791489235234892385475

#ifdef WIN32
#include <windows.h>
#else
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <semaphore.h>
#ifdef OSX64
#include <sys/msg.h>
#include <openssl/md5.h>
#else
#include <mqueue.h>
#endif // #ifdef OSX64
#include <pthread.h>
#include <signal.h>
#include <time.h>
#endif // #ifdef WIN32
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>
#include <map>
#include <set>

////////////////////////////////////////////////////////////////////////////////////////////////////
// helper macros
//
#define SMF_D_EVENT_ID(event_id)                    SMF_EVTID_ ## event_id
#define SMF_D_ENGINE_INSTANCE(engine_name)          EngineInstance_ ## engine_name
#define SMF_D_ENGINE_CLASS(engine_name)             Engine_ ## engine_name
#define SMF_D_ENGINE_FACTORY_CLASS(engine_name)     EngineFactory_ ## engine_name
#define SMF_D_STATE_INSTANCE(state_name)            StateInstance_ ## state_name
#define SMF_D_STATE_CLASS(state_name)               State_ ## state_name
#define SMF_D_STATE_FACTORY_CLASS(state_name)       StateFactory_ ## state_name

////////////////////////////////////////////////////////////////////////////////////////////////////
// constants
//
const unsigned int SMF_EVTID_SYS_BASE = 0;
const unsigned int SMF_EVTID_USR_BASE = 100;

typedef unsigned int SMF_EventId;
enum {
    SMF_D_EVENT_ID(_INVALID) = SMF_EVTID_SYS_BASE,
    SMF_D_EVENT_ID(_ENTRY),
    SMF_D_EVENT_ID(_EXIT),
    SMF_D_EVENT_ID(_GOTO_SELF),
    SMF_D_EVENT_ID(_GOTO_PARENT),
    SMF_D_EVENT_ID(_GOTO_DEF_CHILD),
    SMF_D_EVENT_ID(_GOTO_FIRST_CHILD),
};

const unsigned int SMF_ERR_SYS_CODE_BEGIN = 0;
const unsigned int SMF_ERR_USR_CODE_BEGIN = 1000;

typedef unsigned int SMF_ErrorCode;
enum {
    SMF_ERR_OK = SMF_ERR_SYS_CODE_BEGIN,
    SMF_ERR_FAILED,
    SMF_ERR_TIMEOUT,
    SMF_ERR_EAGAIN,
    SMF_ERR_PLATFORM_API_CALL_FAILED,
    SMF_ERR_NOT_PROCESSED,
    SMF_ERR_INVALID_HANDLE,
    SMF_ERR_WAIT_FAILED,
    SMF_ERR_FAILED_TO_CREATE_SEM,
    SMF_ERR_FAILED_TO_DESTROY_SEM,
    SMF_ERR_FAILED_TO_CREATE_SHM,
    SMF_ERR_FAILED_TO_DESTROY_SHM,
    SMF_ERR_FAILED_TO_CREATE_MQ,
    SMF_ERR_FAILED_TO_DESTROY_MQ,
    SMF_ERR_FAILED_TO_OPEN_MQ,
    SMF_ERR_FAILED_TO_CLOSE_MQ,
    SMF_ERR_FAILED_TO_CREATE_THREAD,
    SMF_ERR_FAILED_TO_STOP_THREAD,
    SMF_ERR_FAILED_TO_CREATE_TIMER,
    SMF_ERR_FAILED_TO_DESTROY_TIMER,
    SMF_ERR_FAILED_TO_START_TIMER,
    SMF_ERR_FAILED_TO_STOP_TIMER,
    SMF_ERR_FAILED_TO_CREATE_FILE,
    SMF_ERR_FAILED_TO_DELETE_FILE,
    SMF_ERR_EVENT_THREAD_NOT_READY,
    SMF_ERR_EVENT_THREAD_ALREADY_STARTED,
    SMF_ERR_FAILED_TO_POST_EVENT,
    SMF_ERR_NOT_SUPPORTED,
    SMF_ERR_NULL_PTR,
    SMF_ERR_EMPTY_BUFFER,
    SMF_ERR_TOO_SMALL_BUFFER,
    SMF_ERR_TOO_LARGE_BUFFER,
    SMF_ERR_NULL_OPERATION_PTR,
    SMF_ERR_NULL_CURRENT_STATE,
    SMF_ERR_NULL_DEST_STATE,
    SMF_ERR_NULL_PARENT_STATE,
    SMF_ERR_NULL_DEF_CHILD_STATE,
    SMF_ERR_INVALID_EVENT_ID,
    SMF_ERR_INVALID_STATE,
    SMF_ERR_INVALID_STATE_RELATIONSHIP,
    SMF_ERR_NO_CHILD_STATE,
    SMF_ERR_NO_DEF_CHILD_STATE,
    SMF_ERR_NO_FIRST_CHILD_STATE,
    SMF_ERR_UNHANDLED_EVENT,
    SMF_ERR_NOT_IN_EVENT_THREAD,
    SMF_ERR_FAILED_TO_ALLOC_MEM,
    SMF_ERR_DATA_SIZE_ERROR,
    SMF_ERR_ALREADY_ATTACHED_TO_THREAD,
    SMF_ERR_NOT_ATTACHED_TO_THREAD,
    SMF_ERR_ALREADY_ATTACHED_TO_EVENT_QUEUE,
    SMF_ERR_NOT_ATTACHED_TO_EVENT_QUEUE,
    SMF_ERR_ENGINE_TIMER_IS_NULL,
    SMF_ERR_FAILED_TO_CREATE_SOCKET,
    SMF_ERR_FAILED_TO_BIND_ADDR,
    SMF_ERR_INDEX_TOO_GREAT,
    SMF_ERR_INDEX_TOO_LITTLE,
    SMF_ERR_DATA_BUF_TOO_LONG,
    SMF_ERR_TOO_LISTEN_ON_A_SUBSCRIBER,
    SMF_ERR_FAILED_TO_ALLOC_MAIL_BOX,
    SMF_ERR_INVALID_MAIL_BOX_ID,
    SMF_ERR_MAIL_BOX_ALREADY_FREED,
    SMF_ERR_MAIL_BOX_IS_FULL,
    SMF_ERR_FAILED_TO_ALLOC_RPC_TUNNEL,
    SMF_ERR_INVALID_RPC_TUNNEL_ID,
    SMF_ERR_RPC_TUNNEL_ALREADY_FREED,
    SMF_ERR_EMPTY_RPC_TUNNEL,
    SMF_ERR_RPC_CALL_NOT_BEGIN,
    SMF_ERR_OUT_OF_BUFFER_CAPCITY,
    SMF_ERR_FAILED_TO_CREATE_PLATFORM_EVENT,
    SMF_ERR_FAILED_TO_CREATE_PLATFORM_LOCK,
    SMF_ERR_FAILED_TO_CREATE_PLATFORM_MSGQ,
    SMF_ERR_FAILED_TO_CREATE_PLATFORM_SHARED_MEM,
    SMF_ERR_FAILED_TO_CREATE_PLATFORM_THREAD,
    SMF_ERR_FAILED_TO_CREATE_PLATFORM_TIMER,
    SMF_ERR_NOT_SHARED_OBJECT,
    SMF_ERR_CYCLIC_BUF_NOT_INIT,
    SMF_ERR_CYCLIC_BUF_NOT_ENOUGH_SPACE_TO_WRITE,
    SMF_ERR_CYCLIC_BUF_NOT_ENOUGH_DATA_TO_READ,
    SMF_ERR_ARG_OUT_OF_RANGE,
};

enum {
    SMF_SENDER_ID_DEF = 0,
};

enum SMF_StateRelationship {
    SMF_SR_SELF = 0,
    SMF_SR_PARENT,
    SMF_SR_CHILD,
    SMF_SR_BROTHER,
    SMF_SR_OHTER,
    SMF_SR_INVALID = -1,
};

typedef unsigned int SMF_EngineFeature;
enum {
    SMF_EF_DEFAULT                                                                          = 0x00000000,
    // When SMF_EF_PROCESS_EVENT_IN_PARENT_STATE_IF_NOT_PROCESSED_IN_CURRENT_STATE is set, if the event
    // will not be processed by the current state, try process it in the parent state, or in the parent
    // of the parent state, ..., until it is processed.
    // When SMF_EF_PROCESS_EVENT_IN_PARENT_STATE_IF_NOT_PROCESSED_IN_CURRENT_STATE is not set, if the
    // event will be processed by the current state, try process it in the parent state, and in the
    // parent of the parent state, ..., until it is not processed.
    SMF_EF_PROCESS_EVENT_IN_PARENT_STATE_IF_NOT_PROCESSED_IN_CURRENT_STATE                  = 0x00000001,
    //
    // If an event will not trigger to state change, and if SMF_EF_PROCESS_INTERNAL_EVENT_IN_CURRENT_STATE
    // is set, process it in the current state.
    //
    SMF_EF_PROCESS_INTERNAL_EVENT_IN_CURRENT_STATE                                          = 0x00000002,
};

enum {
    SMF_INFINITE = 0xFFFFFFFF
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// basic types
//
typedef unsigned int        SMF_MACHINE_WORD_TYPE;

typedef signed int          SMF_INT;
typedef signed char         SMF_INT8;
typedef signed short        SMF_INT16;
typedef signed long         SMF_INT32;
typedef unsigned int        SMF_UINT;
typedef unsigned char       SMF_UINT8;
typedef unsigned short      SMF_UINT16;
typedef unsigned long       SMF_UINT32;
typedef signed char         SMF_BOOL;

typedef unsigned int        SMF_Condition;
typedef unsigned int        SMF_StateId;
typedef unsigned int        SMF_EventId;
typedef unsigned int        SMF_SenderId;
typedef unsigned int        SMF_EngineId;

////////////////////////////////////////////////////////////////////////////////////////////////////
// interface relevant
//
class SMF_BaseThread;

#ifndef interface
#define interface			                struct
#endif

class SMF_NonCopyable
{
protected:
    SMF_NonCopyable() {}

private:
    SMF_NonCopyable(const SMF_NonCopyable &);
    SMF_NonCopyable &operator =(const SMF_NonCopyable &);
};

interface SMF_BaseInterface
{
    virtual ~SMF_BaseInterface() {}
};

struct SMF_PlacementNewDelete
{
    void *operator new(std::size_t size, void *ptr) throw() { return ptr; }
    void operator delete(void *, void *) {}
    void operator delete(void *) {}
};

interface SMF_ThreadHandler : public SMF_BaseInterface
{
	virtual SMF_ErrorCode ThreadProc(SMF_BaseThread &rThread) = 0;
};

//
// NOTE:
// 1) Limitition:
//        IsRemote:  [0, 1]
//        IsDynamic: [0, 1]
//        CallId:    [0, 16383]
//        nArgsSize: [0, 32767]
//
#define SMF_GET_CALL_MASK_IS_REMOTE                                 0x00000001
#define SMF_GET_CALL_MASK_IS_DYNAMIC                                0x00000001
#define SMF_GET_CALL_MASK_CALL_ID                                   0x00003FFF
#define SMF_GET_CALL_MASK_REMOTE_WAITING                            0x00000001
#define SMF_GET_CALL_MASK_ARGS_SIZE                                 0x00007FFF

#define SMF_GET_CALL_SHIFT_IS_REMOTE                                31
#define SMF_GET_CALL_SHIFT_IS_DYNAMIC                               30
#define SMF_GET_CALL_SHIFT_CALL_ID                                  16
#define SMF_GET_CALL_SHIFT_REMOTE_WAITING                           15
#define SMF_GET_CALL_SHIFT_ARGS_SIZE                                0

#define SMF_MAKE_CALL_WPARAM(is_remote,remote_waiting,is_dynamic,call_id,args_size) \
    ( 0 \
      | (((is_remote)           & SMF_GET_CALL_MASK_IS_REMOTE)              << SMF_GET_CALL_SHIFT_IS_REMOTE) \
      | (((is_dynamic)          & SMF_GET_CALL_MASK_IS_DYNAMIC)             << SMF_GET_CALL_SHIFT_IS_DYNAMIC) \
      | (((call_id)             & SMF_GET_CALL_MASK_CALL_ID)                << SMF_GET_CALL_SHIFT_CALL_ID) \
      | (((remote_waiting)      & SMF_GET_CALL_MASK_REMOTE_WAITING)         << SMF_GET_CALL_SHIFT_REMOTE_WAITING) \
      | (((args_size)           & SMF_GET_CALL_MASK_ARGS_SIZE)              << SMF_GET_CALL_SHIFT_ARGS_SIZE) \
    )
#define SMF_GET_CALL_IS_REMOTE(wParam)                              (((wParam) >> SMF_GET_CALL_SHIFT_IS_REMOTE)             & SMF_GET_CALL_MASK_IS_REMOTE)
#define SMF_GET_CALL_IS_DYNAMIC(wParam)                             (((wParam) >> SMF_GET_CALL_SHIFT_IS_DYNAMIC)            & SMF_GET_CALL_MASK_IS_DYNAMIC)
#define SMF_GET_CALL_ID(wParam)                                     (((wParam) >> SMF_GET_CALL_SHIFT_CALL_ID)               & SMF_GET_CALL_MASK_CALL_ID)
#define SMF_GET_CALL_REMOTE_WAITING(wParam)                         (((wParam) >> SMF_GET_CALL_SHIFT_REMOTE_WAITING)        & SMF_GET_CALL_MASK_REMOTE_WAITING)
#define SMF_GET_CALL_ARGS_SIZE(wParam)                              (((wParam) >> SMF_GET_CALL_SHIFT_ARGS_SIZE)             & SMF_GET_CALL_MASK_ARGS_SIZE)

interface SMF_ThreadCallHandler : public SMF_BaseInterface
{
	typedef unsigned int CallId;
    enum {
        CALL_ID_BASE = 0,
        CALL_ID_USER_BASE = CALL_ID_BASE,
        CALL_ID_SYS_BASE = CALL_ID_BASE + 16000,
        CALL_ID_INVALID = SMF_GET_CALL_ID(SMF_INFINITE),
        CALL_ID_ALL = CALL_ID_INVALID,
    };
    struct ThreadCallArgs {
        ThreadCallArgs() : pInBuf(NULL), nInBufSize(0), pOutBuf(NULL), nOutBufSize(0), nOutBufRetSize(0), nRet(0) {}
        ThreadCallArgs(unsigned char *a_pInBuf, unsigned int a_nInBufSize, unsigned char *a_pOutBuf, unsigned int a_nOutBufSize)
            : pInBuf(a_pInBuf), nInBufSize(a_nInBufSize), pOutBuf(a_pOutBuf), nOutBufSize(a_nOutBufSize), nOutBufRetSize(0), nRet(0) {}

        unsigned char *pInBuf;
        unsigned int nInBufSize;
        unsigned char *pOutBuf;         /* out */
        unsigned int nOutBufSize;
        unsigned int nOutBufRetSize;    /* out */
        int nRet;                       /* out */
    };

	virtual SMF_ErrorCode ThreadCallProc(SMF_BaseThread &rThread, CallId nCallId, ThreadCallArgs &rThreadCallArgs) = 0;
};

//
// NOTE:
// 1) Limitition:
//        TimerId:  [0, 4294967295]
//
interface SMF_ThreadTimerHandler : public SMF_BaseInterface
{
	typedef unsigned int TimerId;
    enum {
        TIMER_ID_BASE = 0,
        TIMER_ID_USER_BASE = TIMER_ID_BASE,
        TIMER_ID_SYS_BASE = TIMER_ID_USER_BASE + 300000,
        TIMER_ID_INVALID = SMF_INFINITE,
        TIMER_ID_ALL = TIMER_ID_INVALID,
    };

	virtual SMF_ErrorCode ThreadTimerProc(SMF_BaseThread &rThread, TimerId nTimerId) = 0;
};

//
// NOTE:
// 1) Limitition:
//        IsRemote: [0, 1]
//        QueueId:  [0, 31]
//        EngineId: [0, 31]
//        SenderId: [0, 31]
//        DataSize: [0, 16383]
//        EventId:  [0, 4294967295]
//
#define SMF_GET_EQ_MASK_IS_REMOTE                                   0x00000001
#define SMF_GET_EQ_MASK_QUEUE_ID                                    0x0000001F
#define SMF_GET_EQ_MASK_ENGINE_ID                                   0x0000001F
#define SMF_GET_EQ_MASK_SENDER_ID                                   0x0000001F
#define SMF_GET_EQ_MASK_IS_DYNAMIC                                  0x00000001
#define SMF_GET_EQ_MASK_REMOTE_WAITING                              0x00000001
#define SMF_GET_EQ_MASK_DATA_SIZE                                   0x00003FFF

#define SMF_GET_EQ_SHIFT_IS_REMOTE                                  31
#define SMF_GET_EQ_SHIFT_QUEUE_ID                                   26
#define SMF_GET_EQ_SHIFT_ENGINE_ID                                  21
#define SMF_GET_EQ_SHIFT_SENDER_ID                                  16
#define SMF_GET_EQ_SHIFT_IS_DYNAMIC                                 15
#define SMF_GET_EQ_SHIFT_REMOTE_WAITING                             14
#define SMF_GET_EQ_SHIFT_DATA_SIZE                                  0

#define SMF_MAKE_EQ_WPARAM(is_remote,remote_waiting,is_dynamic, queue_id,engine_id,sender_id,data_size) \
    ( 0 \
      | (((is_remote)           & SMF_GET_EQ_MASK_IS_REMOTE)            << SMF_GET_EQ_SHIFT_IS_REMOTE) \
      | (((queue_id)            & SMF_GET_EQ_MASK_QUEUE_ID)             << SMF_GET_EQ_SHIFT_QUEUE_ID) \
      | (((engine_id)           & SMF_GET_EQ_MASK_ENGINE_ID)            << SMF_GET_EQ_SHIFT_ENGINE_ID) \
      | (((sender_id)           & SMF_GET_EQ_MASK_SENDER_ID)            << SMF_GET_EQ_SHIFT_SENDER_ID) \
      | (((is_dynamic)          & SMF_GET_EQ_MASK_IS_DYNAMIC)           << SMF_GET_EQ_SHIFT_IS_DYNAMIC) \
      | (((remote_waiting)      & SMF_GET_EQ_MASK_REMOTE_WAITING)       << SMF_GET_EQ_SHIFT_REMOTE_WAITING) \
      | (((data_size)           & SMF_GET_EQ_MASK_DATA_SIZE)            << SMF_GET_EQ_SHIFT_DATA_SIZE) \
    )
#define SMF_GET_EQ_IS_REMOTE(wParam)                                (((wParam) >> SMF_GET_EQ_SHIFT_IS_REMOTE)               & SMF_GET_EQ_MASK_IS_REMOTE)
#define SMF_GET_EQ_QUEUE_ID(wParam)                                 (((wParam) >> SMF_GET_EQ_SHIFT_QUEUE_ID)                & SMF_GET_EQ_MASK_QUEUE_ID)
#define SMF_GET_EQ_ENGINE_ID(wParam)                                (((wParam) >> SMF_GET_EQ_SHIFT_ENGINE_ID)               & SMF_GET_EQ_MASK_ENGINE_ID)
#define SMF_GET_EQ_SENDER_ID(wParam)                                (((wParam) >> SMF_GET_EQ_SHIFT_SENDER_ID)               & SMF_GET_EQ_MASK_SENDER_ID)
#define SMF_GET_EQ_IS_DYNAMIC(wParam)                               (((wParam) >> SMF_GET_EQ_SHIFT_IS_DYNAMIC)              & SMF_GET_EQ_MASK_IS_DYNAMIC)
#define SMF_GET_EQ_REMOTE_WAITING(wParam)                           (((wParam) >> SMF_GET_EQ_SHIFT_REMOTE_WAITING)          & SMF_GET_EQ_MASK_REMOTE_WAITING)
#define SMF_GET_EQ_DATA_SIZE(wParam)                                (((wParam) >> SMF_GET_EQ_SHIFT_DATA_SIZE)               & SMF_GET_EQ_MASK_DATA_SIZE)

interface SMF_ThreadEventQueueHandler : public SMF_BaseInterface
{
    typedef unsigned int QueueId;
    typedef unsigned int EngineId;
    typedef unsigned int SenderId;
    typedef unsigned int EventId;
    enum {
        QUEUE_ID_BASE = 0,
        QUEUE_ID_USER_BASE = QUEUE_ID_BASE,
        QUEUE_ID_SYS_BASE = QUEUE_ID_USER_BASE + 28,
        QUEUE_ID_INVALID = SMF_GET_EQ_QUEUE_ID(SMF_INFINITE),
        QUEUE_ID_ALL = QUEUE_ID_INVALID,
    };
    struct ThreadEventArgs {
        ThreadEventArgs() : pInBuf(NULL), nInBufSize(0), pOutBuf(NULL), nOutBufSize(0), nOutBufRetSize(0), nRet(0) {}
        ThreadEventArgs(unsigned char *a_pInBuf, unsigned int a_nInBufSize, unsigned char *a_pOutBuf, unsigned int a_nOutBufSize)
            : pInBuf(a_pInBuf), nInBufSize(a_nInBufSize), pOutBuf(a_pOutBuf), nOutBufSize(a_nOutBufSize), nOutBufRetSize(0), nRet(0) {}

        unsigned char *pInBuf;
        unsigned int nInBufSize;
        unsigned char *pOutBuf;         /* out */
        unsigned int nOutBufSize;
        unsigned int nOutBufRetSize;    /* out */
        int nRet;                       /* out */
    };
    struct EventData {
        EventData(EngineId a_nEngineId, SenderId a_nSenderId, EventId a_nEventId, ThreadEventArgs *a_pThreadEventArgs)
            : nEngineId(a_nEngineId), nSenderId(a_nSenderId), nEventId(a_nEventId), pThreadEventArgs(a_pThreadEventArgs) {}

        EngineId nEngineId;
        SenderId nSenderId;
        EventId nEventId;
        ThreadEventArgs *pThreadEventArgs;
    };

	virtual SMF_ErrorCode ThreadEventQueueProc(SMF_BaseThread &rThread, QueueId nQueueId, EventData &rEventData) = 0;
};

interface SMF_EngineEventHandler : public SMF_BaseInterface
{
    virtual SMF_ErrorCode HandleEvent(SMF_SenderId nSenderId, SMF_EventId nEventId, SMF_ThreadEventQueueHandler::ThreadEventArgs *pThreadEventArgs) = 0;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// types
//
class SMF_BaseState;
struct SMF_OpArg;

typedef SMF_ErrorCode (SMF_BaseState::*SMF_OperationFunc)(SMF_OpArg &rOpArg);

struct SMF_TransItem {
    SMF_TransItem() : nEventId(0), pOperation(NULL), pDestState(NULL) {}

    SMF_EventId nEventId;
    SMF_OperationFunc pOperation;
    SMF_BaseState *pDestState;
};

struct SMF_OpArg {
    SMF_OpArg(SMF_BaseState &a_rSrcState, SMF_BaseState &a_rDstState, SMF_EventId a_nEventId, SMF_ThreadEventQueueHandler::ThreadEventArgs *a_pThreadEventArgs, SMF_SenderId a_nSenderId)
        : rSrcState(a_rSrcState), rDstState(a_rDstState), nEventId(a_nEventId), pThreadEventArgs(a_pThreadEventArgs), nSenderId(a_nSenderId) {}

    SMF_BaseState &rSrcState;
    SMF_BaseState &rDstState;
    SMF_EventId nEventId;
    SMF_ThreadEventQueueHandler::ThreadEventArgs *pThreadEventArgs;
    SMF_SenderId nSenderId;
};

struct SMF_EventInfo {
    SMF_EventInfo(SMF_EventId a_nEventId, SMF_ThreadEventQueueHandler::ThreadEventArgs *a_pThreadEventArgs, SMF_SenderId a_nSenderId) : nEventId(a_nEventId), pThreadEventArgs(a_pThreadEventArgs), nSenderId(a_nSenderId) {}

    SMF_EventId nEventId;
    SMF_ThreadEventQueueHandler::ThreadEventArgs *pThreadEventArgs;
    SMF_SenderId nSenderId;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// tools templates
//
template<typename object_type, size_t object_count, class factory_cls>
class SMF_AutoReleaseObjectPool
{
public:
    SMF_AutoReleaseObjectPool() {}
    ~SMF_AutoReleaseObjectPool() {
        for (size_t i = 0; i < object_count; ++i) {
            object_type *pObject = m_arrObjects[i];
            if (pObject != NULL) {
                factory_cls::DeleteObject(pObject);
                m_arrObjects[i] = NULL;
            }
        }
    }

public:
    object_type &At(size_t nIndex) {
        if (nIndex >= object_count) {
            nIndex = 0;
        }
        while (m_arrObjects[nIndex] == NULL) {
            m_arrObjects[nIndex] = factory_cls::NewObject(nIndex);
        }
        return *m_arrObjects[nIndex];
    }

private:
    object_type *m_arrObjects[object_count];
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// tools macros
//
#define SMF_LOG_MSG(...)                            fprintf(stdout, __VA_ARGS__)
#define SMF_LOG_WRN(...)                            fprintf(stdout, __VA_ARGS__)
#define SMF_LOG_ERR(...)                            fprintf(stderr, __VA_ARGS__)

#define SMF_LOG_STATE() \
    printf("[STATE:#%lu: %s]: %s()\n", GetIndex(), GetStateName().c_str(), __FUNCTION__)

#define SMF_LOG_ENGINE() \
    do { \
        if (GetCurrentState() != NULL) { \
            printf("[STATE:#%lu: %s]: %s(%s)\n", GetIndex(), GetCurrentState()->GetStateName().c_str(), __FUNCTION__, GetEventNameById(rEventInfo.nEventId).c_str()); \
        } else { \
            printf("[STATE:#%lu: %s]: %s(%s)\n", GetIndex(), "(unkown_state)", __FUNCTION__, GetEventNameById(rEventInfo.nEventId).c_str()); \
        } \
    } while (false)

#define SMF_LOG_FUNC() \
    printf("%s()\n", __FUNCTION__)

#define SMF_ARR_SIZE(arr)                           (sizeof(arr) / sizeof(arr[0]))
#define SMF_ALIGNED(size)                           (((size) + sizeof(SMF_MACHINE_WORD_TYPE) - 1) / sizeof(SMF_MACHINE_WORD_TYPE) * sizeof(SMF_MACHINE_WORD_TYPE))
#define SMF_ALIGNED_SIZE(type)                      ((sizeof(type) + sizeof(SMF_MACHINE_WORD_TYPE) - 1) / sizeof(SMF_MACHINE_WORD_TYPE) * sizeof(SMF_MACHINE_WORD_TYPE))

//
// engine and state
//
#define SMF_DECLARE_ENGINE(engine_name) \
    class SMF_D_ENGINE_CLASS(engine_name); \
    SMF_D_ENGINE_CLASS(engine_name) &SMF_GetEngine_ ## engine_name(size_t nIndex);

#define SMF_DEFINE_ENGINE(engine_name,instance_count) \
    class SMF_D_ENGINE_FACTORY_CLASS(engine_name) \
    { \
    private: \
        static SMF_MACHINE_WORD_TYPE arrObjBuff[instance_count][SMF_ALIGNED_SIZE(SMF_D_ENGINE_CLASS(engine_name)) / sizeof(SMF_MACHINE_WORD_TYPE)]; \
    public: \
        static SMF_D_ENGINE_CLASS(engine_name) *NewObject(size_t nIndex) { \
            if (nIndex >= instance_count) { \
                nIndex = 0; \
            } \
            return new(arrObjBuff[nIndex]) SMF_D_ENGINE_CLASS(engine_name) (#engine_name, nIndex); \
        } \
        static void DeleteObject(SMF_D_ENGINE_CLASS(engine_name) *pObject) { \
            if (pObject != NULL) { \
                pObject->~SMF_D_ENGINE_CLASS(engine_name)(); \
                pObject->operator delete(pObject, pObject); \
            } \
        } \
    }; \
    SMF_MACHINE_WORD_TYPE SMF_D_ENGINE_FACTORY_CLASS(engine_name)::arrObjBuff[instance_count][SMF_ALIGNED_SIZE(SMF_D_ENGINE_CLASS(engine_name)) / sizeof(SMF_MACHINE_WORD_TYPE)]; \
    SMF_D_ENGINE_CLASS(engine_name) &SMF_GetEngine_ ## engine_name(size_t nIndex) \
    { \
        static SMF_AutoReleaseObjectPool<SMF_D_ENGINE_CLASS(engine_name), instance_count, SMF_D_ENGINE_FACTORY_CLASS(engine_name)> arop; \
        return arop.At(nIndex); \
    }

#define SMF_GET_ENGINE(engine_name,index) \
    SMF_GetEngine_ ## engine_name(index)

#define SMF_DECLARE_STATE(state_name) \
    class SMF_D_STATE_CLASS(state_name); \
    SMF_D_STATE_CLASS(state_name) &SMF_GetState_ ## state_name(size_t nIndex);

#define SMF_DEFINE_STATE(engine_name,state_name,instance_count) \
    class SMF_D_STATE_FACTORY_CLASS(state_name) \
    { \
    private: \
        static SMF_MACHINE_WORD_TYPE arrObjBuff[instance_count][SMF_ALIGNED_SIZE(SMF_D_STATE_CLASS(state_name)) / sizeof(SMF_MACHINE_WORD_TYPE)]; \
    public: \
        static SMF_D_STATE_CLASS(state_name) *NewObject(size_t nIndex) { \
            if (nIndex >= instance_count) { \
                nIndex = 0; \
            } \
            return new(arrObjBuff[nIndex]) SMF_D_STATE_CLASS(state_name) (SMF_GET_ENGINE(engine_name, nIndex), #state_name, nIndex); \
        } \
        static void DeleteObject(SMF_D_STATE_CLASS(state_name) *pObject) { \
            if (pObject != NULL) { \
                pObject->~SMF_D_STATE_CLASS(state_name)(); \
                pObject->operator delete(pObject, pObject); \
            } \
        } \
    }; \
    SMF_MACHINE_WORD_TYPE SMF_D_STATE_FACTORY_CLASS(state_name)::arrObjBuff[instance_count][SMF_ALIGNED_SIZE(SMF_D_STATE_CLASS(state_name)) / sizeof(SMF_MACHINE_WORD_TYPE)]; \
    SMF_D_STATE_CLASS(state_name) &SMF_GetState_ ## state_name(size_t nIndex) \
    { \
        static SMF_AutoReleaseObjectPool<SMF_D_STATE_CLASS(state_name), instance_count, SMF_D_STATE_FACTORY_CLASS(state_name)> arop; \
        return arop.At(nIndex); \
    }

#define SMF_GET_STATE(state_name,index) \
    SMF_GetState_ ## state_name(index)

#define SMF_STATE_NAME_EQUAL_TO(state_instance,state_name,index) \
    (state_instance).IsEqualTo(SMF_GET_STATE(state_name, index))

#define SMF_SET_STATE_MACHINE_TO_ENGINE(state_name,engine_name,index) \
    do { \
        SMF_GET_ENGINE(engine_name, index).SetStateMachine(&SMF_GET_STATE(state_name, index)); \
    } while (0)

#define SMF_ADD_STATE_TO_PARENT_STATE(state_name,parent_state_name,index) \
    do { \
        SMF_GET_STATE(state_name, index).SetParentState(&SMF_GET_STATE(parent_state_name, index)); \
    } while (0)

//
// state transition table
// NOTE:
// 1) Before the macros:
//        SMF_DEFINE_STATE_TRANS_TABLE_BEGIN
//        SMF_DEFINE_STATE_TRANS_ITEM
//    be used, the macro:
//        SMF_CURRENT_STATE_CLASS
//    should be defined like as below:
//        #define SMF_CURRENT_STATE_CLASS           SMF_D_STATE_CLASS(state_name)
//

#define SMF_DECLARE_STATE_TRANS_TABLE() \
    virtual SMF_ErrorCode GetTransItemByEventId(SMF_EventId nEventId, SMF_TransItem &oTransItem);

#define SMF_DEFINE_STATE_TRANS_TABLE_BEGIN() \
    SMF_ErrorCode SMF_CURRENT_STATE_CLASS::GetTransItemByEventId(SMF_EventId nEventId, SMF_TransItem &oTransItem) \
    { \
        SMF_ErrorCode nErrorCode = SMF_ERR_NOT_PROCESSED; \
        do { \

#define SMF_DEFINE_STATE_TRANS_TABLE_END() \
        } while (0); \
        return nErrorCode; \
    }

#define SMF_DEFINE_STATE_TRANS_ITEM(event_id,condition,operation,dest_state_name) \
        if (SMF_D_EVENT_ID(event_id) == nEventId && (condition)) { \
            oTransItem.nEventId = SMF_D_EVENT_ID(event_id); \
            oTransItem.pOperation = static_cast<SMF_OperationFunc>(&SMF_CURRENT_STATE_CLASS::operation); \
            oTransItem.pDestState = &SMF_GET_STATE(dest_state_name, GetIndex()); \
            nErrorCode = SMF_ERR_OK; \
            break; \
        }

////////////////////////////////////////////////////////////////////////////////////////////////////
// variables
//
extern unsigned int SMF_nPlatformErrorCode; // for debug

#ifdef WIN32
#define SMF_OS_PATH_SEP                 "\\"
#else
#define SMF_OS_PATH_SEP                 "/"
#endif

#ifdef WIN32
#define SMF_DEF_TEMP_DIR_ROOT           "C:\\tmp\\smf"
#else
#define SMF_DEF_TEMP_DIR_ROOT           "/tmp/smf"
#endif

#ifndef SMF_TEMP_DIR_ROOT
#define SMF_TEMP_DIR_ROOT               SMF_DEF_TEMP_DIR_ROOT
#endif // #ifndef SMF_TEMP_DIR_ROOT

#ifdef WIN32
#define SMF_DEF_TEMP_KEY_DIR_ROOT       "C:\\tmp\\smf_key"
#else
#define SMF_DEF_TEMP_KEY_DIR_ROOT       "/tmp/smf_key"
#endif

#ifndef SMF_TEMP_KEY_DIR_ROOT
#define SMF_TEMP_KEY_DIR_ROOT           SMF_DEF_TEMP_KEY_DIR_ROOT
#endif // #ifndef SMF_TEMP_KEY_DIR_ROOT

static const unsigned int SMF_THREAD_AWAKEN_INTERVAL_MILLISECONDS = 100;

#define SMF_MD5_DIGEST_LENGTH           16

std::string SMF_GetDomainStr(bool bSharedDomain = false);

#endif // #ifndef SMF_AFX_H__782384237643289472376234623791489235234892385475
