////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  @file networkFSM.h
//  This file handles the communication with the VuC, using the OTP VCS-interface
 
// @project     GLY_TCAM
// @subsystem   VolvoOnCall
// @author      Jiaojiao Shen
// @Init date   12-Sep-2018
////////////////////////////////////////////////////////////////////////////

#include <rms/service/network/networkFSM.h>
#include <rms/sys/postoffice.h>
#include <rms/config/settings.h>
#include <rms/gbt/gbt32960_3_simple.h>
#include "dlt/dlt.h"

DLT_IMPORT_CONTEXT(dlt_voc);

namespace nfsm {

////////////////////////////////////////////////////////////////////////////////
//! \fn NetworkFSM::NetworkFSM(const NetworkCallbackInterface& parent)
//! \brief NetworkFSM Constructor
//! \param[in] parent Rererance to call back interface for processing states
//! \return  void
////////////////////////////////////////////////////////////////////////////////
NetworkFSM::NetworkFSM(const NetworkCallbackInterface& parent) :
        mParent(parent), mMaxPacketSize(0) {

}

////////////////////////////////////////////////////////////////////////////////
//! \fn NetworkFSM::~NetworkFSM()
//! \brief NetworkFSM destructor
//! \return  void
////////////////////////////////////////////////////////////////////////////////
NetworkFSM::~NetworkFSM(){

}

////////////////////////////////////////////////////////////////////////////////
//! \fn NetworkFSM::handleConnect(const EventConnect& ev)
//! \brief handle connection to server
//! \param[in] ev EventConnect
//! \return void
////////////////////////////////////////////////////////////////////////////////
void NetworkFSM::handleConnect(const EventConnect& ev) {
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: %s", __FUNCTION__);
    bool rc = false;
    try {
        rc = mNc.connect(ev.url);
    } catch (const boost::exception& e) {
        rc = false;
    }
    if (rc) {
        mParent.onConnected();
    } else {
        post_event(EventDisconnect());
    }
}

////////////////////////////////////////////////////////////////////////////////
//! \fn NetworkFSM::handleDisconnect(const EventDisconnect& ev)
//! \brief handle disconnection from server
//! \param[in] ev EventDisconnect
//! \return void
////////////////////////////////////////////////////////////////////////////////
void NetworkFSM::handleDisconnect(const EventDisconnect& ev) {
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: %s", __FUNCTION__);
    mNc.disconnect();
    mParent.onDisconnected();
}

////////////////////////////////////////////////////////////////////////////////
//! \fn NetworkFSM::handleSend(const EventSend& ev)
//! \brief handle send data to server
//! \param[in] ev EventSend
//! \return void
////////////////////////////////////////////////////////////////////////////////
void NetworkFSM::handleSend(const EventSend& ev) {
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: %s", __FUNCTION__);
    size_t size = 0;
    size_t temp = ev.notify->getPayloadRef().size();
    mCurPacket = ev.notify;
    mMaxPacketSize = mMaxPacketSize < temp ? temp : mMaxPacketSize;
    size = mNc.write(mCurPacket->getPayloadRef());
    if (!mNc.isConnected() || size != mCurPacket->getPayloadRef().size()) {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: mCurPacket->getPayloadRef().size() = %d",
                mCurPacket->getPayloadRef().size());
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: mNc.isConnected() = %d",mNc.isConnected());
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: size = %d", size);
        mParent.onConnectionLost(mCurPacket);
        post_event(EventDisconnect());
    } else {
        post_event(EventReadResp(ev.timeout));
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: Sent %d bytes", size);
    }
}

////////////////////////////////////////////////////////////////////////////////
//! \fn NetworkFSM::handleSendNoResp(const EventSendNoResp& ev)
//! \brief handle no response event
//! \param[in] ev EventSendNoResp
//! \return void
////////////////////////////////////////////////////////////////////////////////
void NetworkFSM::handleSendNoResp(const EventSendNoResp& ev) {
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: %s", __FUNCTION__);
    size_t size = 0;
    size_t temp = ev.notify->getPayloadRef().size();
    mCurPacket = ev.notify;
    mMaxPacketSize = mMaxPacketSize < temp ? temp : mMaxPacketSize;
    size = mNc.write(mCurPacket->getPayloadRef());
    if (!mNc.isConnected() || size != mCurPacket->getPayloadRef().size()) {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: mCurPacket->getPayloadRef().size() = %d",
                mCurPacket->getPayloadRef().size());
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: mNc.isConnected() = %d",mNc.isConnected());
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: size = %d", size);
        mParent.onConnectionLost(mCurPacket);
        post_event(EventDisconnect());
    } else {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: Sent %d bytes", size);
        mParent.onSendCompleted(std::vector<uint8_t>());
    }
}

////////////////////////////////////////////////////////////////////////////////
//! \fn NetworkFSM::handleReadResp(const EventReadResp& ev)
//! \brief handle read response from server
//! \param[in] ev EventReadResp
//! \return void
////////////////////////////////////////////////////////////////////////////////
void NetworkFSM::handleReadResp(const EventReadResp& ev) {
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: %s", __FUNCTION__);
    size_t size = 0;
    size = mNc.read(mBuffer,
          // Expected response size is less or equal to sent previously messages
            mMaxPacketSize, ev.timeout);
    if (!mNc.isConnected()) {
        mParent.onConnectionLost(mCurPacket);
        post_event(EventDisconnect());
    } else {
        post_event(EventReadComplete());
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: Read %d bytes", size);
    }
}

////////////////////////////////////////////////////////////////////////////////
//! \fn NetworkFSM::handleReadCompleted(const EventReadComplete& ev)
//! \brief handle completer read response from server
//! \param[in] ev EventReadComplete
//! \return void
////////////////////////////////////////////////////////////////////////////////
void NetworkFSM::handleReadCompleted(const EventReadComplete& ev) {
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: %s", __FUNCTION__);
    std::vector<uint8_t> resp(mBuffer.begin(), mBuffer.end());
    // add member buffer to class for collecting data from network
    // cut packets from buffer start from 'start character'(##)
    mParent.onSendCompleted(resp);
}

};
