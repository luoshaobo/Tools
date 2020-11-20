#ifndef VC_SESSION_LIST_HPP
#define VC_SESSION_LIST_HPP

/**
* Copyright (C) 2016 Continental Automotive GmbH
*
* Licensed under the "Supplier unrestricted white box" category, as
* described in the 20160322-1_SW-amendment TCAM.pdf agreement
* between Volvo Cars and Continental Automotive GmbH.
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*/

/** @file vc_session_list.hpp
 * This file handles session-lists to match two different IDs.
 *
 * @project     VCC
 * @subsystem   VehicleComm
 * @version     00.01
 * @author      Ola Lilja & Johan Bohlin
 * @date        25-Jan-2017
 ***************************************************************************/

#include <list>
#include <mutex>
#include <chrono>
#include <thread>
#include <atomic>
#include <sys/time.h>
#include "vc_common.hpp"
#include "vc_message_queue.hpp"
#include "vc_watchdog.hpp"

namespace vc {

typedef struct {
    int ack_tries;
    int ack_timeout_ms;
    int response_tries;
    int response_timeout_ms;
    int time_resolution_ms;
} SessionCfgValues;

const char *const SessionStateStr[] {
    "AWAITING_ACK",
    "AWAITING_RESPONSE",
    "ACK_RECEIVED_AWAITING_RESPONSE",
};

typedef enum {
    AWAITING_ACK,
    AWAITING_RESPONSE,
    ACK_RECEIVED_AWAITING_RESPONSE,
} SessionState;

typedef struct {
    int time_left;
    int ack_tries_left;
    int res_tries_left;
    SessionState state;
} SessionTimer;

typedef enum {
    VC_REQUEST_ID,
    VC_SESSION_ID,
    EXT_SESSION_ID,
} SessionType;


/**
    @brief A session-entry

    This struct represent one VC-request being sent to the external node of
    that VC-endpoint.
*/
typedef struct {
    int transfer_id;
    int ext_session_id;
    MessageBase *msg;
    SessionTimer session_timer;
    std::function<ReturnValue(MessageBase *)> resend_cb_;
} SessionKey;

/**
    @brief A class handling all requests being sent to an external node

    This class is used to keep track of all requests sent to an external node,
    as well as any response or event coming from the external node. It also
    handles timeouts/timers and resend-operations, which are executed in a
    separate thread.
*/
class SessionList {
  private:
    Watchdog& wd_;
    MessageQueue *mq_;
    std::atomic<bool> exit_;
    std::mutex list_mtx;                    /**< Used for runtime-handling */
    std::condition_variable cv_;            /**< Condition variable */
    int transfer_id_next_;                  /**< Counter for transfer-IDs used */
    std::list<SessionKey> sessions_;        /**< The list of active sessions */
    std::thread timer_thread_;              /**< Thread to handle resend/timeouts */
    std::mutex cond_mtx_;
    SessionCfgValues cfg_;
    WatchdogThreads wd_thread_;

    SessionKey *FindExternalSession(int ext_session_id);
    SessionKey *FindSession(RequestID request_id);
    int HandleTimeouts(int reduce_sleep_ms);
    void TimerLoop();

  public:
    SessionList();
    ~SessionList();

    ReturnValue Init(MessageQueue *mq, WatchdogThreads thread, SessionCfgValues& cfg);
    ReturnValue Start();
    ReturnValue Stop();
    ReturnValue AddSession(MessageBase *msg, int ext_session_id, SessionState initial_state,
                            std::function<ReturnValue(MessageBase*)> SendMessage);

    void AckRequest(int ext_session_id);
    MessageBase* RemoveSession(SessionType type, long id);
    long RemoveSession_ExtID(long ext_session_id);
    long RemoveSession_SessionID(long session_id);
    long RemoveSession_RequestID(RequestID request_id);
    void UpdateExtSessionID(RequestID request_id, uint16_t session_id);
    MessageBase *GetRequestMessage(RequestID request_id);
};

} // namespace vc

#endif // VC_SESSION_LIST
