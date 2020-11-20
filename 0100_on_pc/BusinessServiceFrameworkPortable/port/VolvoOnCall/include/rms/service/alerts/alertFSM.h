////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  @file alertManager.h
//  This file handles the communication with the VuC, using the OTP VCS-interface
 
// @project     GLY_TCAM
// @subsystem   VolvoOnCall
// @author      Shen Jiaojiao
// @Init date   21-Sep-2018
////////////////////////////////////////////////////////////////////////////

#pragma once

#include <boost/statechart/state_machine.hpp>
#include <boost/statechart/state.hpp>
#include <boost/statechart/transition.hpp>
#include <boost/smart_ptr.hpp>

#include <rms/service/network/networkClient.h>
#include <rms/service/alerts/storage.h>
#include <rms/sys/postoffice.h>
#include <rms/config/settings.h>
#include "dlt/dlt.h"

DLT_IMPORT_CONTEXT(dlt_voc);

namespace afsm {

namespace sc = boost::statechart;
//------------------------------------------------------------------------------
// Events definition of Alert State Machine
//------------------------------------------------------------------------------
// Active events:
namespace aev {
struct EventPublish;
struct EventHeartbeat;
struct EventLogin;
struct EventLogout;
struct EventDisconnect;
}
// Passive events:
namespace pev {
struct EventConnected;
struct EventConnectionLost;
struct EventDisconnected;
struct EventSendCompleted;
}
// Internal events:
namespace iev {
struct EventReconnect;
struct EventResponseSuccess;
struct EventResponseFailure;
struct EventResponseTimeout;
}

//------------------------------------------------------------------------------
// States definition of Alert State Machine
//------------------------------------------------------------------------------
struct StateIdle;
struct StateConnecting;
struct StateLogin;
struct StateReady;
struct StateSending;
struct StateLogout;
struct StateDisconnecting;

//------------------------------------------------------------------------------
// Callback Interface
//------------------------------------------------------------------------------

//! \struct AlertCallbackInterface
//! \brief interface defining callbacks from AlertFSM
struct AlertCallbackInterface {

    //! \fn ~AlertCallbackInterface()
    //! \brief virtual destructor
    //! \return void
    virtual ~AlertCallbackInterface() {}

    //! \fn onConnectSuccess()
    //! \brief callback that is called when connection succeeded
    //! \return void
    virtual void onConnectSuccess() = 0;

    //! \fn onConnectFailure()
    //! \brief callback that is called when connection failed
    //! \return void
    virtual void onConnectFailure() = 0;

    //! \fn onPublishSuccess()
    //! \brief callback that is called when publishing message succeeded
    //! \return void
    virtual void onPublishSuccess() = 0;

    //! \fn onPublishFailure()
    //! \brief callback that is called when publishing message failed
    //! \return void
    virtual void onPublishFailure() = 0;

    //! \fn onPublishFailureAck()
    //! \brief callback that is called when receive publishing message failed
    //! \return void
    virtual void onPublishFailureAck() = 0;

    //! \fn onHeartbeatAck()
    //! \brief callback that is called when valid response to heartbeat is received
    //! \return void
    virtual void onHeartbeatAck() = 0;

    //! \fn onHeartbeatNAck()
    //! \brief callback that is called when valid response to heartbeat is not received
    //! \return void
    virtual bool onHeartbeatNAck() = 0;

    //! \fn onLoginSuccess()
    //! \brief callback that is called when valid response to login request is received
    //! \return void
    virtual void onLoginSuccess() = 0;

    //! \fn onLoginFailure()
    //! \brief callback that is called when valid response to login request is not received
    //! \return void
    virtual void onLoginFailure() = 0;

    //! \fn onLogoutFinish()
    //! \brief callback that is called when response to logout request is received
    //! \return void
    virtual void onLogoutFinish() = 0;

    //! \fn connect(const std::string& url)
    //! \brief callback that is called to initiate connection
    //! \param[in] url URL to connect to
    //! \return void
    virtual void connect(const std::string& url) = 0;

    //! \fn disconnect()
    //! \brief callback that is called to initiate disconnection
    //! \return void
    virtual void disconnect() = 0;

    //! \fn postpone(const boost::shared_ptr<Notify>& notify);
    //! \brief callback that is called to postpone some action
    //! \param[in] notify message with information on action to be postponed
    //! \return void
    virtual void postpone(const boost::shared_ptr<Notify>& notify) = 0;

    //! \fn postponeAlert(const boost::shared_ptr<nvm::Data>& xevnotify);
    //! \brief callback that is called to postpone publishing alert
    //! \param[in] xevnotify alert data
    //! \return void
    virtual void postponeAlert(const boost::shared_ptr<nvm::Data>& xevnotify) = 0;

    //! \fn sendData(const Payload& data, uint8_t timeout = 0)
    //! \brief callback that is called to send data
    //! \param[in] data Data to be sent
    //! \param[in] timeout sending timeout
    //! \return void
    virtual void sendData(const Payload& data, uint8_t timeout = 0) = 0;

    //! \fn getWarningMode()
    //! \brief callback that is called to get warning mode state
    //! \return bool
    virtual bool getWarningMode() = 0;
};


//------------------------------------------------------------------------------
// Alert State Machine definition
//------------------------------------------------------------------------------
/*! \class AlertFSM
    \brief FSM responsible for managing lifecycle of connection session to the government server: sending login, logout, 
           alert data, heartbeat, reactions to connection state events
*/
class AlertFSM:
        public boost::statechart::state_machine<AlertFSM, StateIdle> {
public:
    
    AlertFSM(AlertCallbackInterface& parent);

    void connect(const aev::EventLogin& ev);

    void disconnect(const aev::EventDisconnect& ev);

    void reconnect(const iev::EventReconnect& ev);

    void login(const aev::EventLogin& ev);

    void logout(const aev::EventLogout& ev);

    void connected(const pev::EventConnected& ev);

    void disconnected(const pev::EventDisconnected& ev);

    void connectionLost(const pev::EventConnectionLost& ev);



    void validatePublishResponse(const pev::EventSendCompleted& ev);

    void validateLoginResponse(const pev::EventSendCompleted& ev);

    void validateLogoutResponse(const pev::EventSendCompleted& ev);



    void publishDefer(const aev::EventPublish& ev);

    void publish(const aev::EventPublish& ev);

    void heartbeat(const aev::EventHeartbeat& ev);


    void postpone(const aev::EventLogout& ev);

    void postponeAlert(const aev::EventPublish& ev);

//    void postpone(const aev::EventPublish& ev);

    virtual ~AlertFSM();

private:

    bool validateResponseIntegrity(const Payload& resp);


    CommandId getResponseCmdType(const Payload& resp);

    ResponseMark getResponseMarkType(const Payload& resp);


    void doPublish(const boost::shared_ptr<nvm::Data>& info);

    void doLogin();

    void doHeartbeat();

    void doLogout();

    uint8_t calcCrc(const Payload& pkt);

    //! \var mParent

    //! \brief reference to parent whose callback methods will be called
    AlertCallbackInterface& mParent;

    //! \struct LoginData
    //! \brief struct for storing login data
    //! \var mLoginData
    //! \brief variable storing login data
    struct LoginData {
        uint32_t sn;
        Time time;
    } mLoginData;

    //! \var mLastSentCmd

    //! \brief variable with id of last sent command
    CommandId mLastSentCmd;

    //! \var loginSnFile

    //! \brief static variable with name of file where last login serial number is stored
    static std::string loginSnFile;
};


//------------------------------------------------------------------------------
// Events implementation of Alert State Machine
//------------------------------------------------------------------------------
// Active events:
namespace aev {

//! \struct EventPublish
//! \brief struct to represent publish event
struct EventPublish: public sc::event<EventPublish> {

    //! \fn EventPublish(const boost::shared_ptr<nvm::Data>& xevinfo)

    //! \brief A constructor of EventPublish object
    //! \param[in] xevinfo A reference to boost::shared_ptr<nvm::Data> object with alert data
    //! \return void
    EventPublish(const boost::shared_ptr<nvm::Data>& xevinfo):
        xevinfo(xevinfo) { }


    //! \var xevinfo

    //! \brief boost::shared_ptr<nvm::Data> object with alert data
    boost::shared_ptr<nvm::Data> xevinfo;
};
//struct EventRetry: public sc::event<EventRetry> {
//    EventRetry(const boost::shared_ptr<Notify>& notify):
//        notify(notify) { }
//    boost::shared_ptr<Notify> notify;
//};

//! \struct EventLogin
//! \brief struct to represent login event
struct EventLogin: public sc::event<EventLogin> {};

//! \struct EventHeartbeat
//! \brief struct to represent heartbeat event
struct EventHeartbeat: public sc::event<EventHeartbeat> {};

//! \struct EventDisconnect
//! \brief struct to represent disconnect event
struct EventDisconnect: public sc::event<EventDisconnect> {};

//! \struct EventLogout
//! \brief struct to represent logout event
struct EventLogout: public sc::event<EventLogout> {

    //! \fn EventLogout(const boost::shared_ptr<Notify>& notify)

    //! \brief A constructor of EventLogout object
    //! \param[in] notify A reference to boost::shared_ptr<Notify> object
    //! \return void
    EventLogout(const boost::shared_ptr<Notify>& notify):
        notify(notify) { }

    //! \var notify

    //! \brief boost::shared_ptr<Notify> object
    boost::shared_ptr<Notify> notify;
};
}
// Passive events:
namespace pev {

//! \struct EventConnected
//! \brief struct to represent connected event
struct EventConnected: public sc::event<EventConnected> {};

//! \struct EventConnectionLost
//! \brief struct to represent connection lost event
struct EventConnectionLost: public sc::event<EventConnectionLost> {};

//! \struct EventDisconnected
//! \brief struct to represent disconnected event
struct EventDisconnected: public sc::event<EventDisconnected> {};

//! \struct EventSendCompleted
//! \brief struct to represent send completed event
struct EventSendCompleted: public sc::event<EventSendCompleted> {

    //! \fn EventSendCompleted(const boost::shared_ptr<Notify>& notify)

    //! \brief A constructor of EventSendCompleted object
    //! \param[in] notify A reference to boost::shared_ptr<Notify> object
    //! \return void
    EventSendCompleted(const boost::shared_ptr<Notify>& notify):
        notify(notify) { }

    //! \var notify

    //! \brief boost::shared_ptr<Notify> object
    boost::shared_ptr<Notify> notify;
};
}
// Internal events:
namespace iev {

//! \struct EventResponseSuccess
//! \brief struct to represent response success event
struct EventResponseSuccess: public sc::event<EventResponseSuccess> {};

//! \struct EventResponseFailure
//! \brief struct to represent response  failure event
struct EventResponseFailure: public sc::event<EventResponseFailure> {};

//! \struct EventResponseTimeout
//! \brief struct to represent response timeout event
struct EventResponseTimeout: public sc::event<EventResponseTimeout> {};

//! \struct EventReconnect
//! \brief struct to represent reconnect event
struct EventReconnect: public sc::event<EventReconnect> {};
}


//------------------------------------------------------------------------------
// States implementation of Alert State Machine
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// State: Idle
//------------------------------------------------------------------------------

//! \struct StateIdle
//! \brief struct to represent idle state of FSM with events that should be processed in it and possible transitions
struct StateIdle : public sc::simple_state<StateIdle, AlertFSM> {
    StateIdle() {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_DEBUG, "RMS: StateIdle");
    }
    ~StateIdle() {
    }
    typedef boost::mpl::list<
        sc::transition<aev::EventLogin,   StateConnecting, AlertFSM, &AlertFSM::connect>,
        sc::transition<aev::EventPublish, StateIdle,       AlertFSM, &AlertFSM::publishDefer>
    > reactions;
};

//------------------------------------------------------------------------------
// State: Connecting to a server
//------------------------------------------------------------------------------

//! \struct StateConnecting
//! \brief struct to represent connecting state of FSM with events that should be processed in it and possible transitions
struct StateConnecting : public sc::simple_state<StateConnecting, AlertFSM> {
    StateConnecting() {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_DEBUG, "RMS: StateConnecting");
    }
    ~StateConnecting() {
    }
    //--------------------------------------------------------------------------
    // Transition table: Event, Next State, Context, Action
    //--------------------------------------------------------------------------
    typedef boost::mpl::list<
        sc::transition<aev::EventLogout,           StateLogout,     AlertFSM, &AlertFSM::logout>,
        sc::transition<pev::EventConnected,        StateLogin,      AlertFSM, &AlertFSM::connected>,
        sc::transition<pev::EventDisconnected,     StateIdle,       AlertFSM, &AlertFSM::disconnected>,
        sc::transition<pev::EventConnectionLost,   StateIdle,       AlertFSM, &AlertFSM::connectionLost>,
        sc::transition<aev::EventPublish,          StateConnecting, AlertFSM, &AlertFSM::publishDefer>,
        sc::transition<aev::EventDisconnect,       StateDisconnecting, AlertFSM, &AlertFSM::disconnect>
    > reactions;
};

//------------------------------------------------------------------------------
// State: Login to a server
//------------------------------------------------------------------------------

//! \struct StateLogin
//! \brief struct to represent login state of FSM with events that should be processed in it and possible transitions
struct StateLogin : public sc::simple_state<StateLogin, AlertFSM> {
    StateLogin() {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_DEBUG, "RMS: StateLogin");
    }
    ~StateLogin() {
    }
    //--------------------------------------------------------------------------
    // Transition table: Event, Next State, Context, Action
    //--------------------------------------------------------------------------
    typedef boost::mpl::list<
        sc::transition<iev::EventResponseSuccess,  StateReady>,
        //sc::transition<iev::EventResponseFailure,  StateLogin>,
        sc::transition<aev::EventDisconnect,       StateDisconnecting, AlertFSM, &AlertFSM::disconnect>,
        sc::transition<aev::EventLogout,           StateLogout, AlertFSM, &AlertFSM::logout>,
        sc::transition<aev::EventLogin,            StateLogin,  AlertFSM, &AlertFSM::login>,
        sc::transition<pev::EventSendCompleted,    StateLogin,  AlertFSM, &AlertFSM::validateLoginResponse>,
        sc::transition<pev::EventDisconnected,     StateIdle,   AlertFSM, &AlertFSM::disconnected>,
        sc::transition<pev::EventConnectionLost,   StateIdle,   AlertFSM, &AlertFSM::connectionLost>,
        sc::transition<aev::EventPublish,          StateLogin,  AlertFSM, &AlertFSM::publishDefer>
    > reactions;
};

//------------------------------------------------------------------------------
// State: Ready for publish
//------------------------------------------------------------------------------

//! \struct StateReady
//! \brief struct to represent ready state of FSM with events that should be processed in it and possible transitions
struct StateReady : public sc::simple_state<StateReady, AlertFSM> {
    StateReady() {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_DEBUG, "RMS: StateReady");
    }
    ~StateReady() {
    }
    //--------------------------------------------------------------------------
    // Transition table: Event, Next State, Context, Action
    //--------------------------------------------------------------------------
    typedef boost::mpl::list<
        sc::transition<iev::EventReconnect,       StateReady,    AlertFSM, &AlertFSM::reconnect>,
        sc::transition<aev::EventLogout,          StateLogout,   AlertFSM, &AlertFSM::logout>,
        sc::transition<aev::EventHeartbeat,       StateSending,  AlertFSM, &AlertFSM::heartbeat>,
        sc::transition<aev::EventPublish,         StateSending,  AlertFSM, &AlertFSM::publish>,
        sc::transition<pev::EventDisconnected,    StateIdle,     AlertFSM, &AlertFSM::disconnected>,
        sc::transition<pev::EventConnectionLost,  StateIdle,     AlertFSM, &AlertFSM::connectionLost>,
        sc::transition<aev::EventDisconnect,      StateDisconnecting, AlertFSM, &AlertFSM::disconnect>
    > reactions;
};

//------------------------------------------------------------------------------
// State: Sending data to a server
//------------------------------------------------------------------------------

//! \struct StateSending
//! \brief struct to represent sending state of FSM with events that should be processed in it and possible transitions
struct StateSending : public sc::simple_state<StateSending, AlertFSM> {
    StateSending() {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_DEBUG, "RMS: StateSending");
    }
    ~StateSending() {
    }
    //--------------------------------------------------------------------------
    // Transition table: Event, Next State, Context, Action
    //--------------------------------------------------------------------------
    typedef boost::mpl::list<
        sc::transition<aev::EventDisconnect,      StateDisconnecting, AlertFSM, &AlertFSM::disconnect>,
        sc::transition<iev::EventReconnect,       StateReady,         AlertFSM, &AlertFSM::reconnect>,
        sc::transition<iev::EventResponseSuccess, StateReady>,
        sc::transition<iev::EventResponseFailure, StateReady>, // retry
        sc::transition<pev::EventSendCompleted,   StateSending,  AlertFSM, &AlertFSM::validatePublishResponse>,
//        sc::transition<aev::EventPublish,         StateSending,  AlertFSM, &AlertFSM::postpone>, // call Postpone
        sc::transition<aev::EventPublish,         StateSending,  AlertFSM, &AlertFSM::postponeAlert>,
        sc::transition<aev::EventLogout,          StateSending,  AlertFSM, &AlertFSM::postpone>, // call Postpone
        sc::transition<pev::EventDisconnected,    StateIdle,     AlertFSM, &AlertFSM::disconnected>,
        sc::transition<pev::EventConnectionLost,  StateIdle,     AlertFSM, &AlertFSM::connectionLost>
    > reactions;
};

//------------------------------------------------------------------------------
// State: Logging out from a server
//------------------------------------------------------------------------------

//! \struct StateLogout
//! \brief struct to represent logout state of FSM with events that should be processed in it and possible transitions
struct StateLogout : public sc::simple_state<StateLogout, AlertFSM> {
    StateLogout() {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_DEBUG, "RMS: StateLogout");
    }
    ~StateLogout() {
    }
    //--------------------------------------------------------------------------
    // Transition table: Event, Next State, Context, Action
    //--------------------------------------------------------------------------
    typedef boost::mpl::list<
        sc::transition<iev::EventResponseSuccess,   StateDisconnecting>, // send network manager disconnect
        sc::transition<iev::EventResponseFailure,   StateDisconnecting>, // TODO add retries to logout?
        sc::transition<pev::EventSendCompleted,     StateLogout, AlertFSM, &AlertFSM::validateLogoutResponse>, //call Response validation
        sc::transition<pev::EventDisconnected,      StateIdle>,
        sc::transition<pev::EventConnectionLost,    StateIdle>,
        sc::transition<aev::EventPublish,           StateLogout, AlertFSM, &AlertFSM::publishDefer>
    > reactions;
};

//------------------------------------------------------------------------------
// State: Disconnecting from a server
//------------------------------------------------------------------------------

//! \struct StateDisconnecting
//! \brief struct to represent disconnecting state of FSM with events that should be processed in it and possible transitions
struct StateDisconnecting : public sc::simple_state<StateDisconnecting, AlertFSM> {
    StateDisconnecting() {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_DEBUG, "RMS: StateDisconnecting");
    }
    ~StateDisconnecting() {
    }
    //--------------------------------------------------------------------------
    // Transition table: Event, Next State, Context, Action
    //--------------------------------------------------------------------------
    typedef boost::mpl::list<
        sc::transition<pev::EventDisconnected,   StateIdle>,
        sc::transition<pev::EventConnectionLost, StateIdle>,
        sc::transition<aev::EventPublish,        StateDisconnecting, AlertFSM, &AlertFSM::publishDefer>
    > reactions;
};

};

