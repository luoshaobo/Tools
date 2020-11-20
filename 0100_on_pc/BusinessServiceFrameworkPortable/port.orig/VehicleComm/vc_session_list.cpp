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

/** @file vc_session_list.cpp
 * This file handles session-lists to match two different IDs.
 *
 * @project     VCC
 * @subsystem   VehicleComm
 * @version     00.01
 * @author      Ola Lilja & Johan Bohlin
 * @date        25-Jan-2017
 ***************************************************************************/

#include "vc_utility.hpp"

#include "include/vc_session_list.hpp"

namespace vc {

// Private

/**
    @brief Find a session in a session-list.

    This method is used to find a session with a specific external
    session id .

    @param[in]  ext_session_id         The external session id to be found
    @return     A pointer to the session-key containing the corresponding message
*/
SessionKey *SessionList::FindExternalSession(int ext_session_id)
{
    for (std::list<SessionKey>::iterator it = sessions_.begin(); it != sessions_.end(); ++it)
        if ((RequestID)it->ext_session_id == ext_session_id)
            return &*it;

    return NULL;
}

/**
    @brief Find a session in a session-list.

    This method is used to find a session with a specific request-ID
    in the session-list of the object.

    @param[in]  request_id          The request-ID to be found
    @return     A pointer to the session-key containing the corresponding message
*/
SessionKey *SessionList::FindSession(RequestID request_id)
{
    for (std::list<SessionKey>::iterator it = sessions_.begin(); it != sessions_.end(); ++it)
        if ((RequestID)it->msg->id_ == request_id)
            return &*it;

    return NULL;
}

/**
    @brief Check for timeouts in SessionList

    This method counts down the timers for each session that is added.
    If time left is zero and retries left also is zero it will call
    CreateErrorMsg that will send out an error message of type Timeout.

    @param[in]  reduce_sleep_ms          Time to reduce sleep with
    @return     Time used by previous calculations
*/
int SessionList::HandleTimeouts(int reduce_sleep_ms)
{
    LOG(LOG_INFO, "SessionList: %s (reduce_sleep_ms = %d ms).", __FUNCTION__, reduce_sleep_ms);
    struct timespec t1, t2;
    unsigned long t1msec, t2msec;
    int elapsedtime;

    std::this_thread::sleep_for(std::chrono::milliseconds(cfg_.time_resolution_ms - reduce_sleep_ms));

    list_mtx.lock();

    clock_gettime(CLOCK_MONOTONIC, &t1);
    t1msec = t1.tv_sec*1000L + (t1.tv_nsec / 1000000L);

    std::list<SessionKey>::iterator it = sessions_.begin();
    while (it != sessions_.end()) {
        it->session_timer.time_left = it->session_timer.time_left - cfg_.time_resolution_ms;
        LOG(LOG_INFO, "SessionList: %s (ext_session_id = %d, unique_id = %d, time_left = %d, state = %s)",
                __FUNCTION__, it->ext_session_id, it->msg->unique_id_, it->session_timer.time_left,
                SessionStateStr[it->session_timer.state]);
        if (it->session_timer.time_left <= 0) {
            if (it->session_timer.state == AWAITING_RESPONSE ||
                it->session_timer.state == ACK_RECEIVED_AWAITING_RESPONSE)
                it->session_timer.res_tries_left--;
            else
                it->session_timer.ack_tries_left--;

            if ((it->session_timer.ack_tries_left <= 0 && it->session_timer.state == AWAITING_ACK) ||
                    it->session_timer.res_tries_left <= 0) {
                LOG(LOG_DEBUG, "SessionList: %s: Create error-message, since no %s.", __FUNCTION__,
                        SessionStateStr[it->session_timer.state]);
                SendErrorMessage(mq_, it->msg, RET_ERR_TIMEOUT);
                delete(it->msg);
                it = sessions_.erase(it);
                continue;
            }
            else {
                LOG(LOG_DEBUG, "SessionList: %s: Resending (ext_session_id = %d, unique_id = %d, timeout_state = %s, transfer_id = %d, request_id = %d).",
                        __FUNCTION__, it->ext_session_id, it->msg->unique_id_, SessionStateStr[it->session_timer.state],
                        it->transfer_id, it->msg->id_);
                            
                if (it->session_timer.state == AWAITING_ACK ||
                    it->session_timer.state == ACK_RECEIVED_AWAITING_RESPONSE) {
                    it->session_timer.time_left = cfg_.ack_timeout_ms;
                    it->session_timer.state = AWAITING_ACK;
                }
                else
                    it->session_timer.time_left = cfg_.response_timeout_ms;

                ReturnValue ret = it->resend_cb_(it->msg);
                if (ret)
                    LOG(LOG_DEBUG, "SessionList: %s: Failed to resend message (error = %s)!", __FUNCTION__,
                            ReturnValueStr[ret]);
            }
        }
        ++it;
    }

    clock_gettime(CLOCK_MONOTONIC, &t2);
    t2msec = t2.tv_sec*1000L + (t2.tv_nsec / 1000000L);
    elapsedtime = (int)(t2msec - t1msec);
    if (elapsedtime < 0)
        elapsedtime = 0;
    else if (elapsedtime > cfg_.time_resolution_ms)
        elapsedtime = cfg_.time_resolution_ms;
    LOG(LOG_DEBUG, "SessionList: %s: Time difference: %d ms", __FUNCTION__, elapsedtime);

    list_mtx.unlock();

    return elapsedtime;
}


/**
    @brief Check for timeouts in SessionList

    This method will run until VehicleComm exit and calls SessionList::Stop.
    It will check if there are any sessions added in the list, if so it will
    execute HandleTimeouts to count down the time of the available sessions.
    If there are no sessions available it will sleep on the condition variable
    until AddSession is called.
*/
void SessionList::TimerLoop()
{
    LOG(LOG_INFO, "SessionList: %s.", __FUNCTION__);

    int reduce_sleep_time = 0;
    while (!exit_) {
        std::unique_lock<std::mutex> lck(cond_mtx_);
        if (sessions_.empty() && !exit_) {
            reduce_sleep_time = 0;
            wd_.SetState(wd_thread_, WD_THREAD_STATE_SLEEPING);
            cv_.wait(lck);
        }
        if (!exit_) {

            wd_.SetState(wd_thread_, WD_THREAD_STATE_RUNNING);
            reduce_sleep_time = HandleTimeouts(reduce_sleep_time);
        }
    }

    LOG(LOG_INFO, "SessionList: %s: Exiting session-loop...", __FUNCTION__);
}



// Public

/**
    @brief Constructor for SessionList

    Constructor for SessionList
*/
SessionList::SessionList() : wd_(Watchdog::GetInstance()),
                             exit_(false)
{
    LOG(LOG_DEBUG, "SessionList: %s .", __FUNCTION__);
    mq_ = NULL;
    transfer_id_next_ = 0;
    wd_thread_ = WD_THREAD_MAX;
    memset((void*)&cfg_, 0, sizeof(SessionCfgValues));
}

/**
    @brief Destructor for SessionList
*/
SessionList::~SessionList()
{
    LOG(LOG_DEBUG, "SessionList: %s.", __FUNCTION__);

    Stop();
};

/**
    @brief Initiate Sessionlist

    This method initiates the Sessionlist with necessary message queue
    callback pointer for doing resend and vehiclecomms watchdog thread.

    @param[in]  mq                  Pointer to VehicleComms message queue (needed to add error messages)
    @param[in]  resendmessage       Function pointer in order to be able to do resend in case of timeouts
    @param[in]  thread              VehicleComms watchdog thread, needed to check if TimerLoop freezes
    @return     A VC return value
*/
ReturnValue SessionList::Init(MessageQueue *mq, WatchdogThreads thread, SessionCfgValues& cfg)
{
    LOG(LOG_DEBUG, "SessionList: %s.", __FUNCTION__);
    wd_thread_ = thread;
    mq_ = mq;
    cfg_ = cfg;
    return RET_OK;
}

/**
    @brief Start Sessionlist

    Starts up the TimerLoop thread.
    @return     A VC return value
*/
ReturnValue SessionList::Start()
{
    LOG(LOG_DEBUG, "SessionList: %s.", __FUNCTION__);

    timer_thread_ = std::thread(&SessionList::TimerLoop, this);

    return RET_OK;
}

/**
    @brief Stop Sessionlist

    Stops the TimerLoop thread
    @return     A VC return value
*/
ReturnValue SessionList::Stop()
{
    LOG(LOG_DEBUG, "SessionList: %s.", __FUNCTION__);

    exit_ = true;
    cv_.notify_all();
    if (timer_thread_.joinable())
        timer_thread_.join();

    return RET_OK;
}

/**
    @brief Ack a request and wait for response instead

    This method is used to ack a request and switch timer
    to wait for response instead.

    @param[in]  ext_session_id          The external session id to be found
*/
void SessionList::AckRequest(int ext_session_id)
{
    list_mtx.lock();
    SessionKey *sk = FindExternalSession(ext_session_id);
    if (sk == NULL) {
        LOG(LOG_WARN, "SessionList: %s cannot find Request to acknowledge.", __FUNCTION__);
        list_mtx.unlock();
        return;
    }
    if (sk->session_timer.state == AWAITING_RESPONSE) {
        LOG(LOG_WARN, "SessionList: %s unexepected ACK to this message (was only prepared for RESPONSE).",
                __FUNCTION__, sk->ext_session_id);
        list_mtx.unlock();
        return;
    }
    LOG(LOG_DEBUG, "SessionList: %s acknowledging ext_session_id: %d unique_id: %d switching to wait for response.",
            __FUNCTION__, sk->ext_session_id, sk->msg->unique_id_);
    sk->session_timer.state = ACK_RECEIVED_AWAITING_RESPONSE;
    sk->session_timer.time_left = cfg_.response_timeout_ms;
    list_mtx.unlock();
}


/**
    @brief Add a session to the session-list.

    This method is called by the user to add a entry to the list.

    @param[in]  request_id          The request-ID for the new entry
    @param[in]  session_id          The session-ID for the new entry
    @return     A VC return value
*/
ReturnValue SessionList::AddSession(MessageBase *msg, int ext_session_id, SessionState initial_state,
                                    std::function<ReturnValue(MessageBase*)> SendMessage)
{
    if(initial_state == ACK_RECEIVED_AWAITING_RESPONSE) {
        LOG(LOG_DEBUG, "SessionList: %s ACK_RECEIVED_AWAITING_RESPONSE is not a valid initial state.");
        return RET_ERR_INTERNAL;
    }

    LOG(LOG_DEBUG, "SessionList: %s (transfer_id = %d, request_id = %d, session_id = %d).",
            __FUNCTION__, transfer_id_next_, (RequestID)msg->id_, (RequestID)msg->session_id_);

    list_mtx.lock();

    //If we cannot send an exact identifier to each message, only allow one message per message id in the queue
    if (ext_session_id <= 0) {
        SessionKey *session = FindSession((RequestID)msg->id_);
        if (session != NULL) {
            LOG(LOG_WARN, "SessionList: %s: Request %s(%d) already in list (transfer_id = %d, session_id = %d)!",
                __FUNCTION__, VCRequestStr[(RequestID)session->msg->id_], session->msg->id_, session->transfer_id,
                session->msg->session_id_);

            list_mtx.unlock();
            return RET_ERR_BUSY;
        }
    }
    MessageBase *m_cpy = CopyMessage(msg);
    if (m_cpy == NULL) {
        list_mtx.unlock();
        return RET_ERR_OUT_OF_MEMORY;
    }
    sessions_.push_back({.transfer_id = transfer_id_next_++,
                         .ext_session_id = ext_session_id,
                         .msg = m_cpy,
                         .session_timer = { .time_left = initial_state ? cfg_.response_timeout_ms : cfg_.ack_timeout_ms,
                                            .ack_tries_left = cfg_.ack_tries,
                                            .res_tries_left = cfg_.response_tries,
                                            .state = initial_state },
                         .resend_cb_ = SendMessage
                         });

    list_mtx.unlock();

    cv_.notify_all();

    return RET_OK;
}

/**
    @brief Remove a session from the session-list.

    This method is called by the user to remove an entry to the list, which has
    a session-ID that matches the provided session-ID.

    @param[in]  type                The type of session to remove
    @param[in]  id                  The ID corresponding to a session of session type
    @return     The message attached to the removed session-entry or NULL (if not found)
*/
MessageBase* SessionList::RemoveSession(SessionType type, long id)
{
    LOG(LOG_DEBUG, "SessionList: %s.", __FUNCTION__);
    MessageBase* m = NULL;
    list_mtx.lock();

    std::list<SessionKey>::iterator it = sessions_.begin();
    while (it != sessions_.end()) {
        bool match = false;
        switch (type) {
        case VC_REQUEST_ID: {
            if ((RequestID)it->msg->id_ == (RequestID)id) {
                match = true;
                LOG(LOG_DEBUG, "SessionList: %s: Removed session (transfer_id = %d, request_id = %d, session_id = %d).",
                            __FUNCTION__, it->transfer_id, it->msg->id_, it->msg->session_id_);
            }
            break;
        }
        case VC_SESSION_ID: {
            if (it->msg->session_id_ == id) {
                match = true;
                LOG(LOG_DEBUG, "SessionList: %s: Removed session (transfer_id = %d, request_id = %d, session_id = %d).",
                            __FUNCTION__, it->transfer_id, it->msg->id_, it->msg->session_id_);
            }
            break;
        }
        case EXT_SESSION_ID: {
            if (it->ext_session_id == id) {
                match = true;
                LOG(LOG_DEBUG, "SessionList: %s: Removed external session (transfer_id = %d, request_id = %d, ext_session_id = %d).",
                    __FUNCTION__, it->transfer_id, (RequestID)it->msg->id_, it->ext_session_id);
            }
            break;
        }
        default:
            LOG(LOG_WARN, "SessionList: %s: Unknown session-type: %d", type);
            list_mtx.unlock();
            return NULL;
        }

        if (match) {
            m = it->msg;
            it = sessions_.erase(it);
            list_mtx.unlock();
            return m;
        }
        ++it;
    }

    LOG(LOG_WARN, "SessionList: %s: Session not found (type = %d) (id = %d)!", __FUNCTION__, type, id);

    list_mtx.unlock();

    return m;
}

/**
    @brief Remove a session from the session-list.

    This method is called by the user to remove an entry to the list, which has
    a session-ID that matches the provided session-ID. It will also delete the
    message that is attached to the entry.

    @param[in]  ext_session_id      The ID corresponding to a session of session type
    @return     The session_id of the removed session-entry
*/
long SessionList::RemoveSession_ExtID(long ext_session_id)
{
    long session_id = -1;
    MessageBase* m = RemoveSession(EXT_SESSION_ID, ext_session_id);
    if (m != NULL) {
        session_id = m->session_id_;
        delete(m);
    }
    return session_id;
}

/**
    @brief Remove a session from the session-list.

    This method is called by the user to remove an entry to the list, which has
    a session-ID that matches the provided session-ID. It will also delete the
    message that is attached to the entry.

    @param[in]  session_id          The ID corresponding to a session of session type
    @return     The session_id of the removed session-entry
*/
long SessionList::RemoveSession_SessionID(long session_id)
{
    long session = -1;
    MessageBase* m = RemoveSession(VC_SESSION_ID, session_id);
    if (m != NULL) {
        session = m->session_id_;
        delete(m);
    }
    return session;
}

/**
    @brief Remove a session from the session-list.

    This method is called by the user to remove an entry to the list, which has
    a session-ID(request-ID) that matches the provided session-ID. It will also delete the
    message that is attached to the entry.

    @param[in]  request_id          The ID corresponding to a session of session type
    @return     The session_id of the removed session-entry
*/
long SessionList::RemoveSession_RequestID(RequestID request_id)
{
    long session_id = -1;
    MessageBase* m = RemoveSession(VC_REQUEST_ID, request_id);
    if (m != NULL) {
        session_id = m->session_id_;
        delete(m);
    }
    return session_id;
}


/**
    @brief Update external session id

    This method will update the external session ID. Note that this method
    must be called within the list mutex context.
    @param[in]  request_id          The request-ID to be found
    @param[in]  session_id          The new session id
*/
void SessionList::UpdateExtSessionID(RequestID request_id, uint16_t session_id)
{
    SessionKey *sk = FindSession((RequestID)request_id);
    if (sk != NULL) {
        LOG(LOG_DEBUG, "SessionList: Message sent update (ext session from:%d  to:%d).", sk->ext_session_id,
                session_id);
        sk->ext_session_id = session_id;
    }
}

MessageBase *SessionList::GetRequestMessage(RequestID request_id)
{
    SessionKey *sk = FindSession(request_id);
    if (sk == NULL) {
        LOG(LOG_DEBUG, "SessionList: Request %s not found!", VCRequestStr[request_id]);
        return NULL;
    }
    return sk->msg;
}

} // namespace vc
