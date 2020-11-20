////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  @file alterManager.h
//  This file handles the communication with the VuC, using the OTP VCS-interface
 
// @project     GLY_TCAM
// @subsystem   VolvoOnCall
// @author      Shen Jiaojiao
// @Init date   20-Sep-2018
////////////////////////////////////////////////////////////////////////////

#pragma once

#include <map>
#include <deque>

#include <rms/service/alerts/storage.h>
#include <rms/sys/service.h>
#include <rms/sys/timerManager.h>
#include <rms/service/alerts/alertFSM.h>

//! \class AlertManager
//! \brief Responsible for managing lifecycle of session to the government server: login, logout,
//!        sending data, sending heartbeats, determination of health of a connection. Responsible
//!        for managing collection of alerts, determine transitions from realtime to supplementary
//!        data types. Stores alert to the flash for future sending to government server or for
//!        cable data retrieval.
class AlertManager:
    public Service,
    public afsm::AlertCallbackInterface
{
    //! \enum IntCmd
    //! \brief set of internal command types
    typedef enum {
        DISCONNECT,
        XEVINFO_CHECK,
        MOVE2SUPP,
    } IntCmd;

    //! \struct IntHeader
    //! \brief struct for storing internal command ID and size of additional data that could be placed after it
    struct IntHeader {
        IntHeader(IntCmd cmd): cmd(cmd), size(0) {}
        IntCmd cmd;
        uint32_t size;
        uint8_t data[];
    };

    struct IntCmdData {
        struct Move2Supp {
            uint32_t queueSize;
        };
    };

    AlertManager();
    AlertManager(const Mailbox& mbox);

public:
    
    virtual ~AlertManager();
    static AlertManager* getInstance();

    virtual bool getWarningMode();

protected:
    
    //--------------------------------------------------------------------------
    // afsm::AlertCallbackInterface
    //--------------------------------------------------------------------------
    virtual void onConnectSuccess();
    virtual void onConnectFailure();
    virtual void onPublishSuccess();
    virtual void onPublishFailure();
    virtual void onPublishFailureAck();
    virtual void onHeartbeatAck();
    virtual bool onHeartbeatNAck();
    virtual void onLoginSuccess();
    virtual void onLoginFailure();
    virtual void onLogoutFinish();
    virtual void connect(const std::string& url);
    virtual void disconnect();
    virtual void postpone(const boost::shared_ptr<Notify>& notify);
    virtual void postponeAlert(const boost::shared_ptr<nvm::Data>& xevnotify);
    virtual void sendData(const Payload& data, uint8_t timeout);

    //--------------------------------------------------------------------------
    // Asynchronous Message/Event handlers
    //--------------------------------------------------------------------------
    void handleDBUSStart(const boost::shared_ptr<Notify>& notify);
    void handleDBUSStop(const boost::shared_ptr<Notify>& notify);
    void handleTimers(const boost::shared_ptr<Notify>& notify);
    void handleLogin(const boost::shared_ptr<Notify>& notify);
//    void handlePublish(const boost::shared_ptr<Notify>& notify);
    void handleLogout(const boost::shared_ptr<Notify>& notify);
    void handleConnected(const boost::shared_ptr<Notify>& notify);
    void handleConnectionLost(const boost::shared_ptr<Notify>& notify);
    void handleDisconnected(const boost::shared_ptr<Notify>& notify);
    void handleSendCompleted(const boost::shared_ptr<Notify>& notify);
    void handleSupplementData(const boost::shared_ptr<Notify>& notify);
    void handleRealtimeData(const boost::shared_ptr<Notify>& notify);
    void handleWarningData(const boost::shared_ptr<Notify>& notify);
    void handleWarningStart(const boost::shared_ptr<Notify>& notify);
    void handleWarningStop(const boost::shared_ptr<Notify>& notify);
    void handleReadReq(const boost::shared_ptr<Notify>& notify);
    void handleReadAck(const boost::shared_ptr<Notify>& notify);

    //--------------------------------------------------------------------------
    // Asynchronous Internal Messages handlers
    //--------------------------------------------------------------------------
    void handleInt(const boost::shared_ptr<Notify>& notify);
    void handleIntDisconnect(const boost::shared_ptr<Notify>& notify);
    void handleIntxEvcdmInfo(const boost::shared_ptr<Notify>& notify);

    //--------------------------------------------------------------------------
    // Timers timeout handlers
    //--------------------------------------------------------------------------
    void handleLoginTm(const TimerProp& tp);
    void handlePublishTm(const TimerProp& tp);
    void handleHeartbeatTm(const TimerProp& tp);


    //--------------------------------------------------------------------------
    // Service
    //--------------------------------------------------------------------------
    virtual void handle(const boost::shared_ptr<Notify>& notify);
    virtual void onStart();
    virtual void onStop();

    bool sendMessageSelf(IntCmd cmd) const {
        IntHeader ih(cmd);
        return sendMessage(mMBox, boost::make_shared<Message>(
                Message(
                    NotifyId::MSG_AM_INTERNAL,
                    Payload((uint8_t*)&ih, ((uint8_t*)&ih + sizeof(ih))))));
    }

    template<typename T> bool sendMessageSelf(IntCmd cmd, const T& t) const {
        Payload p(sizeof(IntHeader) + sizeof(T), 0);
        IntHeader* ih = reinterpret_cast<IntHeader*>(p.data());
        ih->cmd = cmd;
        ih->size = sizeof(T);
        memcpy(ih->data, &t, ih->size);
        return sendMessage(mMBox, boost::make_shared<Message>(
                Message( NotifyId::MSG_AM_INTERNAL, p)));
    }

    template<typename T> bool sendMessageSelf(T* t) const {
        if (!t)
            return false;
        return sendMessage(mMBox, boost::make_shared<Message>(
                Message(
                    NotifyId::MSG_AM_INTERNAL,
                    Payload((uint8_t*)t, ((uint8_t*)t + sizeof(T)+t->size)))));
    }

private:
    
    void move2SupplementaryReq(const boost::shared_ptr<Notify>& notify);
    void move2SupplementaryCnt(uint32_t toMove);

    //! \enum TimerId
    //! \brief IDs of timers 
    typedef enum {
        RETRY_LOGIN_SHORT,
        RETRY_LOGIN_LONG,
        RETRY_PUBLISH_SHORT,
        RETRY_PUBLISH_LONG,
        HEART_BEAT,
    } TimerId;

    //! \enum Constants
    //! \brief set of constants
    typedef enum {
        RETRY_LOGIN = 3,
        RETRY_PUBLISH = 3,
    } Constants;

    //! \var mWarningMode
    //! \brief flag indicating whether we are in warning mode
    bool mWarningMode;

    //! \var mFsm
    //! \brief alert FSM
    afsm::AlertFSM mFsm;

    //! \var mRetryLogin
    //! \brief counter of login retries left
    mutable uint32_t mRetryLogin;

    //! \var mRetryPublish
    //! \brief counter of publish retries left
    mutable uint32_t mRetryPublish;

    //! \var mHeartBeatCnt
    //! \brief counter of heartbeats to which valid response was not received
    uint32_t mHeartBeatCnt;

    //! \var mStorage
    //! \brief nvm::Storage oject responsible for storing data to NVM
    nvm::Storage mStorage;

    //! \var mTimers
    //! \brief set of TimerProp values. These are timers used by AlertManager
    std::set<TimerProp> mTimers;

    //! \var mNetworkAlert
    //! \brief current alert for which sending over wireless network is in progress
    boost::shared_ptr<nvm::Data> mNetworkAlert;

    //! \var mWiredAlert
    //! \brief current alert for which sending over wired connection is in progress
    boost::shared_ptr<nvm::Data> mWiredAlert;

    //! \var mQueue
    //! \brief map with queues of alerts of different types (real-time/supplementary) that should be sent
    std::map<nvm::Data::Type, nvm::Storage::AlertQueue > mQueue;
    
    //! \var mLastWarningData
    //! \brief the temp deque save 30s data before warning mode happen
    nvm::Storage::AlertQueue mLastWarningData;
};
