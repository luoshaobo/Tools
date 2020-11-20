////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  @file networkManager.h
//  This file handles the communication with the VuC, using the OTP VCS-interface
 
// @project     GLY_TCAM
// @subsystem   VolvoOnCall
// @author      Jiaojiao Shen
// @Init date   12-Sep-2018
////////////////////////////////////////////////////////////////////////////

#include <rms/config/settings.h>
#include <rms/service/network/networkManager.h>
#include <rms/service/network/networkDatatype.h>
#include "dlt/dlt.h"

DLT_IMPORT_CONTEXT(dlt_voc);

using namespace nfsm;
////////////////////////////////////////////////////////////////////////////////
//! \fn NetworkManager::NetworkManager(const Mailbox& mbox)
//! \brief NetworkManager Constructor
//! \param[in] mbox mailbox of the NetworkManager service
//! \return  void
////////////////////////////////////////////////////////////////////////////////
NetworkManager::NetworkManager(const Mailbox& mbox) :
        Service(mbox), mFsm(*this)
{
    mFsm.initiate();
}

////////////////////////////////////////////////////////////////////////////////
//! \fn NetworkManager::~NetworkManager()
//! \brief NetworkManager destructor
//! \param[in] mbox mailbox of the NetworkManager service
//! \return  void
////////////////////////////////////////////////////////////////////////////////
NetworkManager::~NetworkManager() {

}
////////////////////////////////////////////////////////////////////////////////
//! \fn NetworkManager::handleConnect(const boost::shared_ptr<Notify>& notify)
//! \brief handle connection to server from NetworkClient and redirect to FSM
//! \param[in] notify Event about connection
//! \return void
////////////////////////////////////////////////////////////////////////////////
void NetworkManager::handleConnect(const boost::shared_ptr<Notify>& notify)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: %s", __FUNCTION__);
    if (notify) {
        std::string str(
            reinterpret_cast<const char*>(
                notify->getPayloadRef().data()),
                notify->getPayloadRef().size());
        mFsm.process_event(EventConnect(str));
    }
}

////////////////////////////////////////////////////////////////////////////////
//! \fn NetworkManager::handleDisconnect(const boost::shared_ptr<Notify>& notify)
//!\brief handle disconnection to server from NetworkClient and redirect to FSM
//!\param[in] notify Event about disconnection
//!\return void
////////////////////////////////////////////////////////////////////////////////
void NetworkManager::handleDisconnect(const boost::shared_ptr<Notify>& notify)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: %s", __FUNCTION__);
    mFsm.process_event(EventDisconnect());
}

////////////////////////////////////////////////////////////////////////////////
//! \fn NetworkManager::handleSend(const boost::shared_ptr<Notify>& notify)
//! \brief handle send data to server
//! \param[in] notify Event
//! \return void
////////////////////////////////////////////////////////////////////////////////
void NetworkManager::handleSend(const boost::shared_ptr<Notify>& notify)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: %s", __FUNCTION__);
    mFsm.process_event(EventSendNoResp(notify));
}

////////////////////////////////////////////////////////////////////////////////
//! \fn NetworkManager::handleSendTm(const boost::shared_ptr<Notify>& notify)
//! \brief handle send data timeout
//! \param[in] notify Event
//! \return void
////////////////////////////////////////////////////////////////////////////////
void NetworkManager::handleSendTm(const boost::shared_ptr<Notify>& notify)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: %s", __FUNCTION__);
    Payload temp(notify->getPayloadRef());
    uint8_t timeout = ndt::getTimeout(temp);
    notify->setPayload(ndt::cutTimeout(temp));
    if (timeout) {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: Sending %d bytes with awaitining for a response with timeout %d",
                temp.size(), timeout);
        mFsm.process_event(EventSend(notify, timeout));
    } else {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: Sending %d bytes without awaitening for a response", temp.size());
        mFsm.process_event(EventSendNoResp(notify));
    }
}

////////////////////////////////////////////////////////////////////////////////
//! \fn NetworkManager::onConnected()
//! \brief process call back for connection established and send EVT_NM_CONNECTED event
//! return void
////////////////////////////////////////////////////////////////////////////////
void NetworkManager::onConnected() const
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: %s", __FUNCTION__);
    sendEvent(boost::make_shared<Event>(Event(NotifyId::EVT_NM_CONNECTED)));
}

////////////////////////////////////////////////////////////////////////////////
//! \fn NetworkManager::onDisconnected()
//! \brief process call back for disconnection and send EVT_NM_DISCONNECTED event
//! return void
////////////////////////////////////////////////////////////////////////////////
void NetworkManager::onDisconnected() const
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: %s", __FUNCTION__);
    sendEvent(boost::make_shared<Event>(Event(NotifyId::EVT_NM_DISCONNECTED)));
}

////////////////////////////////////////////////////////////////////////////////
//! \fn NetworkManager::onConnectionLost(const boost::shared_ptr<Notify>& notify)
//! \brief process call back for connection lost and send EVT_NM_CONNECTION_LOST event
//! \param[in] notify Last data packet which was not send
//! \return void

////////////////////////////////////////////////////////////////////////////////
void NetworkManager::onConnectionLost(const boost::shared_ptr<Notify>& notify) const
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: %s", __FUNCTION__);
    sendEvent(boost::make_shared<Event>(
            Event(NotifyId::EVT_NM_CONNECTION_LOST)));
}

////////////////////////////////////////////////////////////////////////////////
//! \fn NetworkManager::onSendCompleted(const std::vector<uint8_t>& resp) const
//! \brief process call back successfully sent data and send EVT_NM_SEND_COMPLETED event
//! \param[in] resp Response from server
//! \return void
////////////////////////////////////////////////////////////////////////////////
void NetworkManager::onSendCompleted(const std::vector<uint8_t>& resp) const
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: %s", __FUNCTION__);
    sendEvent(boost::make_shared<Event>(
            Event(NotifyId::EVT_NM_SEND_COMPLETED,
                    Payload(resp.begin(), resp.end()))));
}

////////////////////////////////////////////////////////////////////////////////
//! \fn NetworkManager::handle(const boost::shared_ptr<Notify>& notify)
//! \brief Process incoming messages to Network Manager mail box
//! \param[in] notify Incoming message
//! \return void
////////////////////////////////////////////////////////////////////////////////
void NetworkManager::handle(const boost::shared_ptr<Notify>& notify)
{
    typedef EventHandler<
            NotifyId,
            NetworkManager,
            void (NetworkManager::*)(const boost::shared_ptr<Notify>&)
            >hr;
    static hr nmh[] = {
        hr(NotifyId::MSG_NM_CONNECT,     *this, &NetworkManager::handleConnect),
        hr(NotifyId::MSG_NM_DISONNECT,   *this, &NetworkManager::handleDisconnect),
        hr(NotifyId::MSG_NM_SEND_RAW,    *this, &NetworkManager::handleSend),
        hr(NotifyId::MSG_NM_SEND_TM,     *this, &NetworkManager::handleSendTm),
    };

    for (uint32_t i = 0; i < sizeof(nmh)/ sizeof(nmh[0]); i++ ) {
        nmh[i].handle(notify->getNotifyId(), notify);
    }
}

//------------------------------------------------------------------------------
void NetworkManager::onStart() {
    // Nothing to do
}

//------------------------------------------------------------------------------
void NetworkManager::onStop() {
    // Nothing to do
}

////////////////////////////////////////////////////////////////////////////////
//! \fn NetworkManager::getInstance()
//! \brief Provide instance of NetworkManager. If not exist create one
//! \return instance of NetworkManager
////////////////////////////////////////////////////////////////////////////////
NetworkManager* NetworkManager::getInstance() {
    static NetworkManager nm(Mailbox::SERVICE_NETWORK_MANAGER);
    return &nm;
}
