///////////////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  @file networkFSM.h
//  This file handles the communication with the VuC, using the OTP VCS-interface
 
// @project     GLY_TCAM
// @subsystem   VolvoOnCall
// @author      Shen Jiaojiao
// @Init date   21-Sep-2018
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <boost/statechart/state_machine.hpp>
#include <boost/statechart/state.hpp>
#include <boost/statechart/transition.hpp>
#include <boost/smart_ptr.hpp>

#include <rms/service/network/networkClient.h>
#include <rms/sys/postoffice.h>
#include "dlt/dlt.h"

DLT_IMPORT_CONTEXT(dlt_voc);

namespace nfsm {

namespace sc = boost::statechart;

//------------------------------------------------------------------------------
// Events definition of Network State Machine
//------------------------------------------------------------------------------
struct EventConnect;
struct EventDisconnect;
struct EventSend;
struct EventSendNoResp;
struct EventReadResp;
struct EventReadComplete;

//------------------------------------------------------------------------------
// States definition of Network State Machine
//------------------------------------------------------------------------------
struct StateIdle;
struct StateReady;
struct StateSending;
struct StateWaitResp;

//------------------------------------------------------------------------------
// Callback Interface
//------------------------------------------------------------------------------
//! \interface NetworkCallbackInterface
//! \brief Interface for connection events
struct NetworkCallbackInterface {
    virtual ~NetworkCallbackInterface() {}

    //! \fn onConnected()
    //! \brief call back for connection established
    //! \return void
    virtual void onConnected() const = 0;

    //! \fn onDisconnected()
    //! \brief call back for disconnection
    //! \return void
    virtual void onDisconnected() const = 0;

    //! \fn onConnectionLost(const boost::shared_ptr<Notify>& notify)
    //! \brief call back for connection lost
    //! \param[in] notify Last data packet which was not send
    //! \return void
    virtual void onConnectionLost(const boost::shared_ptr<Notify>& notify) const = 0;

    //! \fn onSendCompleted(const std::vector<uint8_t>& resp)
    //! \brief call back successfully sent data
    //! \param[in] resp Response from server
    //! \return void
    virtual void onSendCompleted(const std::vector<uint8_t>& resp) const = 0;
};

//------------------------------------------------------------------------------
// Network State Machine definition
//------------------------------------------------------------------------------
//! \class NetworkFSM
//! \brief Network State Machine
class NetworkFSM:
    public boost::statechart::state_machine<NetworkFSM, StateIdle> {
public:
    NetworkFSM(const NetworkCallbackInterface& parent);
    virtual ~NetworkFSM();

    void handleConnect(const EventConnect& ev);

    void handleDisconnect(const EventDisconnect& ev);

    void handleSend(const EventSend& ev);

    void handleSendNoResp(const EventSendNoResp& ev);

    void handleReadResp(const EventReadResp& ev);

    void handleReadCompleted(const EventReadComplete& ev);
private:
    boost::shared_ptr<Notify> mCurPacket;
    std::vector<uint8_t> mBuffer;
    const NetworkCallbackInterface& mParent;
    size_t mMaxPacketSize;
    NetworkClient mNc;
};


//------------------------------------------------------------------------------
// Events implementation of Network State Machine
//------------------------------------------------------------------------------
struct EventConnect: public sc::event<EventConnect> {
    virtual ~EventConnect() {}
    EventConnect(const std::string& url) : url(url) {}
    std::string url;
};
struct EventDisconnect: public sc::event<EventDisconnect> {};
struct EventSend: public sc::event<EventSend> {
    virtual ~EventSend() {}
    EventSend(const boost::shared_ptr<Notify>& notify, uint32_t timeout) :
        notify(notify), timeout(timeout) {}
    boost::shared_ptr<Notify> notify;
    uint32_t timeout;
};
struct EventSendNoResp: public sc::event<EventSendNoResp> {
    virtual ~EventSendNoResp() {}
    EventSendNoResp(const boost::shared_ptr<Notify>& notify) :
        notify(notify) {}
    boost::shared_ptr<Notify> notify;
};
struct EventReadResp: public sc::event<EventReadResp> {
    EventReadResp(uint32_t tm): timeout(tm) {}
    uint32_t timeout;
};
struct EventReadComplete: public sc::event<EventReadComplete> {};



//------------------------------------------------------------------------------
// States implementation of Network State Machine
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// State: Idle
//------------------------------------------------------------------------------
struct StateIdle : public sc::simple_state<StateIdle, NetworkFSM> {
    StateIdle() {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_DEBUG, "RMS: StateIdle");
    }
    ~StateIdle() {
    }
    //--------------------------------------------------------------------------
    // Transition table: Event, Next State, Context, Action
    //--------------------------------------------------------------------------
    typedef boost::mpl::list<
        sc::transition<EventConnect,  StateReady,  NetworkFSM, &NetworkFSM::handleConnect>
    > reactions;
};
//------------------------------------------------------------------------------
// State: Ready for send data to a server
//------------------------------------------------------------------------------
struct StateReady : public sc::simple_state<StateReady, NetworkFSM> {
    StateReady() {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_DEBUG, "RMS: StateReady");
    }
    ~StateReady() {
    }
    //--------------------------------------------------------------------------
    // Transition table: Event, Next State, Context, Action
    //--------------------------------------------------------------------------
    typedef boost::mpl::list<
        sc::transition<EventSend,       StateWaitResp, NetworkFSM, &NetworkFSM::handleSend>,
        sc::transition<EventSendNoResp, StateReady,    NetworkFSM, &NetworkFSM::handleSendNoResp>,
        sc::transition<EventDisconnect, StateIdle,     NetworkFSM, &NetworkFSM::handleDisconnect>
    > reactions;
};
//------------------------------------------------------------------------------
// State: Sending response from a server
//------------------------------------------------------------------------------
struct StateWaitResp : public sc::simple_state<StateWaitResp, NetworkFSM> {
    StateWaitResp(){
        DLT_LOG_STRING(dlt_voc, DLT_LOG_DEBUG, "RMS: StateWaitResp");
    }
    ~StateWaitResp() {
    }
    //--------------------------------------------------------------------------
    // Transition table: Event, Next State, Context, Action
    //--------------------------------------------------------------------------
    typedef boost::mpl::list<
        sc::transition<EventReadResp,     StateWaitResp, NetworkFSM, &NetworkFSM::handleReadResp>,
        sc::transition<EventReadComplete, StateReady,    NetworkFSM, &NetworkFSM::handleReadCompleted>,
        sc::transition<EventDisconnect,   StateIdle,     NetworkFSM, &NetworkFSM::handleDisconnect>
    > reactions;
};

};




