////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  @file alertManager.cc
//  This file handles the communication with the VuC, using the OTP VCS-interface

// @project     GLY_TCAM
// @subsystem   VolvoOnCall
// @author      Shen Jiaojiao
// @Init date   12-Sep-2018
////////////////////////////////////////////////////////////////////////////

#include <boost/chrono.hpp>

#include <rms/service/network/networkDatatype.h>
#include <rms/util/utils.h>
#include <rms/config/settings.h>
#include <rms/service/alerts/alertManager.h>
#include "dlt/dlt.h"

#define STR(x) #x

using namespace boost;
using namespace afsm;
using namespace nvm;

DLT_IMPORT_CONTEXT(dlt_voc);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn AlertManager::AlertManager(const Mailbox& mbox)
//! \brief The constructor for the AlertManager class. As AlertManager is implemented as singleton, the AlertManager constructor is private
//!        and called once when static variable within AlertManager::getInstance() function is constructed
//! \param[in] mbox A reference to mailbox that will be assigned to AlertManager object and through which messages will be received
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
AlertManager::AlertManager(const Mailbox& mbox):
    Service(mbox), mWarningMode(false), mFsm(*this),
    mRetryLogin(Constants::RETRY_LOGIN),
    mRetryPublish(Constants::RETRY_PUBLISH),
    mStorage(Settings::getInstance()->getWorkdir())
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: %s", __FUNCTION__);
    mHeartBeatCnt = 0;
    mFsm.initiate();
    mTimers.insert(TimerProp(TimerId::RETRY_LOGIN_SHORT,
            Settings::getInstance()->getRetryLoginShort(), false));
    mTimers.insert(TimerProp(TimerId::RETRY_LOGIN_LONG,
            Settings::getInstance()->getRetryLoginLong(), false));
    mTimers.insert(TimerProp(TimerId::RETRY_PUBLISH_SHORT,
            Settings::getInstance()->getRetryPublishShort(), false));
    mTimers.insert(TimerProp(TimerId::RETRY_PUBLISH_LONG,
            Settings::getInstance()->getRetryPublishLong(), false));
    mTimers.insert(TimerProp(TimerId::HEART_BEAT,
            Settings::getInstance()->getDataCollectionParam().heartBeatPeriod,
            true));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn AlertManager::~AlertManager()
//! \brief The destructor for the AlertManager class which is called when the AlertManager object is destroyed. It currently does nothing.
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
AlertManager::~AlertManager() {
}

//------------------------------------------------------------------------------
// afsm::AlertCallbackInterface
//------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn AlertManager::onConnectSuccess()
//! \brief callback that is called when connection succeeded
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AlertManager::onConnectSuccess() {
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: %s", __FUNCTION__);

//    TimerManager* tm = TimerManager::getInstance();
//    mRetryLogin = Constants::RETRY_LOGIN;
//    tm->stopTimer(mMBox, *mTimers.find(TimerId::RETRY_LOGIN_LONG));
//    tm->stopTimer(mMBox, *mTimers.find(TimerId::RETRY_LOGIN_SHORT));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn AlertManager::onConnectFailure()
//! \brief callback that is called when connection failed
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AlertManager::onConnectFailure() {
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: %s", __FUNCTION__);
    TimerManager* tm = TimerManager::getInstance();
    const TimerProp& tpls = *mTimers.find(TimerId::RETRY_LOGIN_SHORT);
    const TimerProp& tpll = *mTimers.find(TimerId::RETRY_LOGIN_LONG);

    if (!(tm->isActive(mMBox, tpls) || tm->isActive(mMBox, tpll))) {
        if (!--mRetryLogin) {
            mRetryLogin = Constants::RETRY_LOGIN;
            tm->startTimer(mMBox, *mTimers.find(TimerId::RETRY_LOGIN_LONG));
            DLT_LOG_STRING(dlt_voc, DLT_LOG_DEBUG, "RMS: Start retry login long timer");
        } else {
            tm->startTimer(mMBox, *mTimers.find(TimerId::RETRY_LOGIN_SHORT));
            DLT_LOG_STRING(dlt_voc, DLT_LOG_DEBUG, "RMS: Start retry login short timer");
        }
    }

    // If connection was lost and alert still is awaitening to be sent
    mNetworkAlert.reset();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn AlertManager::onPublishSuccess()
//! \brief callback that is called when publishing message succeeded
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AlertManager::onPublishSuccess() {
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: %s", __FUNCTION__);

    TimerManager* tm = TimerManager::getInstance();
    do {
        if (!mNetworkAlert) {
            break;
        }

        if (mQueue[mNetworkAlert->getType()].empty()) {
            break;
        }

        if (!mQueue[mNetworkAlert->getType()].front()) {
            mQueue[mNetworkAlert->getType()].pop_front();
            DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "RMS: Unexpected behavior: Alert from Queue is NULL");
            break;
        }

        if (!(*mNetworkAlert == *mQueue[mNetworkAlert->getType()].front())) {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "RMS: Unexpected behavior: Alerts should be the same");
            break;
        }

        mQueue[mNetworkAlert->getType()].pop_front();
        if (!mNetworkAlert->getPath().empty()) {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: %s: Erase The %s data from %s file",__FUNCTION__,mNetworkAlert->getTypeName(),mNetworkAlert->getPath().generic_string().c_str());
            mStorage.erase(mNetworkAlert);
        }

        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: The %s data has been sent. Queue has %d messages",
            mNetworkAlert->getTypeName(),
            mQueue[mNetworkAlert->getType()].size());
    } while(false);

    mNetworkAlert.reset();
//    tm->stopTimer(mMBox, *mTimers.find(TimerId::RETRY_PUBLISH_LONG)); // delete
    tm->stopTimer(mMBox, *mTimers.find(TimerId::RETRY_PUBLISH_SHORT));
    mRetryPublish = Constants::RETRY_PUBLISH;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn AlertManager::onPublishFailure()
//! \brief callback that is called when publishing message failed
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AlertManager::onPublishFailure() {
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: %s", __FUNCTION__);

    TimerManager* tm = TimerManager::getInstance();
    if (!--mRetryPublish) {
        mRetryPublish = Constants::RETRY_PUBLISH;
//        tm->startTimer(mMBox, *mTimers.find(TimerId::RETRY_PUBLISH_LONG)); // delete
        mNetworkAlert.reset();
        IntCmdData::Move2Supp m2s;
        m2s.queueSize = mQueue.at(DataRealTime::type()).size();
        sendMessageSelf(IntCmd::MOVE2SUPP, m2s);
    } else {
        tm->startTimer(mMBox, *mTimers.find(TimerId::RETRY_PUBLISH_SHORT));
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: Publish retries left %d/%d",
            mRetryPublish,
            Constants::RETRY_PUBLISH);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn AlertManager::onPublishFailureAck()
//! \brief callback that is called when receive publishing message failed
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AlertManager::onPublishFailureAck() {
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: %s", __FUNCTION__);

    TimerManager* tm = TimerManager::getInstance();
    if (!--mRetryPublish) {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: Publish retries left %d/%d, abandon message",
            mRetryPublish,
            Constants::RETRY_PUBLISH);
        do {
            if (!mNetworkAlert) {
                break;
            }

            if (mQueue[mNetworkAlert->getType()].empty()) {
                break;
            }

            if (!mQueue[mNetworkAlert->getType()].front()) {
                mQueue[mNetworkAlert->getType()].pop_front();
                DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "RMS: Unexpected behavior: Alert from Queue is NULL");
                break;
            }

            if (!(*mNetworkAlert == *mQueue[mNetworkAlert->getType()].front())) {
                DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "RMS: Unexpected behavior: Alerts should be the same");
                break;
            }

            mQueue[mNetworkAlert->getType()].pop_front();
            if (!mNetworkAlert->getPath().empty()) {
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: %s: Erase The %s data from %s file",__FUNCTION__,mNetworkAlert->getTypeName(),mNetworkAlert->getPath().generic_string().c_str());
                mStorage.erase(mNetworkAlert);
            }

            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: The %s data has been abandon. Queue has %d messages",
                mNetworkAlert->getTypeName(),
                mQueue[mNetworkAlert->getType()].size());
        } while (false);

        mNetworkAlert.reset();
        tm->stopTimer(mMBox, *mTimers.find(TimerId::RETRY_PUBLISH_SHORT));
        mRetryPublish = Constants::RETRY_PUBLISH;
    } else {
        tm->startTimer(mMBox, *mTimers.find(TimerId::RETRY_PUBLISH_SHORT));
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: Publish retries left %d/%d",
            mRetryPublish,
            Constants::RETRY_PUBLISH);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn AlertManager::onHeartbeatAck()
//! \brief callback that is called when valid response to heartbeat is received
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AlertManager::onHeartbeatAck() {
    mHeartBeatCnt = 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn AlertManager::onHeartbeatNAck()
//! \brief callback that is called when valid response to heartbeat is not received
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool AlertManager::onHeartbeatNAck() {
    bool rc = true;
    if (!(++mHeartBeatCnt < Settings::getInstance()->getTimeouts().keepaliveHB)) {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: Heartbeat has not got ack for %d/%d times. Consider as Connection lost.",
            mHeartBeatCnt,
            Settings::getInstance()->getTimeouts().keepaliveHB);

        mHeartBeatCnt = 0;
        rc = false;
    }

    return rc;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn AlertManager::onLoginSuccess()
//! \brief callback that is called when valid response to login request is received
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AlertManager::onLoginSuccess() {
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: %s", __FUNCTION__);

    TimerManager* tm = TimerManager::getInstance();
    mRetryLogin = Constants::RETRY_LOGIN;
    mRetryPublish = Constants::RETRY_PUBLISH;
    tm->stopTimer(mMBox, *mTimers.find(TimerId::RETRY_LOGIN_LONG));
    tm->stopTimer(mMBox, *mTimers.find(TimerId::RETRY_LOGIN_SHORT));
    tm->startTimer(mMBox, *mTimers.find(TimerId::HEART_BEAT));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn AlertManager::onLoginFailure()
//! \brief callback that is called when valid response to login request is not received
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AlertManager::onLoginFailure() {
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: %s", __FUNCTION__);

    TimerManager* tm = TimerManager::getInstance();
    if (!--mRetryLogin) {
        mRetryLogin = Constants::RETRY_LOGIN;
        tm->startTimer(mMBox, *mTimers.find(TimerId::RETRY_LOGIN_LONG));
        sendMessageSelf(IntCmd::DISCONNECT);
    } else {
        tm->startTimer(mMBox, *mTimers.find(TimerId::RETRY_LOGIN_SHORT));
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: Login retries left %d/%d",
            mRetryLogin,
            Constants::RETRY_LOGIN);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn AlertManager::onLogoutFinish()
//! \brief callback that is called when response to logout request is received
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AlertManager::onLogoutFinish() {
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: %s", __FUNCTION__);

    // If logged out before receiving a response
    mNetworkAlert.reset();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn AlertManager::connect(const std::string& url)
//! \brief callback that is called to initiate connection
//! \param[in] url URL to connect to
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AlertManager::connect(const std::string& url) {
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: %s", __FUNCTION__);

    std::string str = url;
    sendMessage(Mailbox::SERVICE_NETWORK_MANAGER,
        boost::make_shared<Message>(Message(NotifyId::MSG_NM_CONNECT,
            Payload(str.begin(), str.end()))));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn AlertManager::disconnect()
//! \brief callback that is called to initiate disconnection
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AlertManager::disconnect() {
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: %s", __FUNCTION__);

    sendMessage(Mailbox::SERVICE_NETWORK_MANAGER,
        boost::make_shared<Message>(Message(NotifyId::MSG_NM_DISONNECT)));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn AlertManager::postpone(const boost::shared_ptr<Notify>& notify);
//! \brief callback that is called to postpone some action
//! \param[in] notify message with information on action to be postponed
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AlertManager::postpone(const boost::shared_ptr<Notify>& notify) {
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: %s", __FUNCTION__);

    sendNotify(mMBox, notify);
    boost::this_thread::sleep_for(boost::chrono::seconds(1));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn AlertManager::postponeAlert(const boost::shared_ptr<nvm::Data>& xevnotify);
//! \brief callback that is called to postpone publishing alert
//! \param[in] xevnotify alert data
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AlertManager::postponeAlert(const boost::shared_ptr<nvm::Data>& xevnotify) {
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: %s", __FUNCTION__);

    boost::this_thread::sleep_for(boost::chrono::seconds(1));
    sendMessageSelf(IntCmd::XEVINFO_CHECK);
    mNetworkAlert.reset();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn AlertManager::sendData(const Payload& data, uint8_t timeout = 0)
//! \brief callback that is called to send data
//! \param[in] data Data to be sent
//! \param[in] timeout sending timeout
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AlertManager::sendData(const Payload& data, uint8_t timeout) {
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: %s", __FUNCTION__);

    NotifyId notifyId;
    Payload temp(data);

    if (timeout) {
        temp = ndt::putTimeout(temp, timeout);
        notifyId = NotifyId::MSG_NM_SEND_TM;
    } else {
        notifyId = NotifyId::MSG_NM_SEND_RAW;
    }

    sendMessage(Mailbox::SERVICE_NETWORK_MANAGER,
            boost::make_shared<Message>(Message(notifyId, temp)));
}

//------------------------------------------------------------------------------
// Asynchronous Message/Event handlers
//------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn AlertManager::handleLogin(const boost::shared_ptr<Notify>& notify)
//! \brief initiate establishing connection connection and send login
//! \param[in] notify internal triggering message
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AlertManager::handleLogin(const boost::shared_ptr<Notify>& notify) {
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: %s", __FUNCTION__);

    //TimerManager* tm = TimerManager::getInstance();
    auto iterTimer = mTimers.find(TimerId::HEART_BEAT);
    if(iterTimer != mTimers.end())
    {
        iterTimer->timeout =
                Settings::getInstance()->getDataCollectionParam().heartBeatPeriod;
    }
    mFsm.process_event(aev::EventLogin());
    mHeartBeatCnt = 0;
}

//------------------------------------------------------------------------------
//void AlertManager::handlePublish(const boost::shared_ptr<Notify>& notify) {
////    shared_ptr<Data> xevinfo(new DataRealTime(notify->getPayloadRef()));
////    mQueue[DataRealTime::type()].push_back(xevinfo);
////    sendMessageSelf(IntHeader(XEVINFO));
////    mCurrentAlert = notify();
////    mFsm.process_event(aev::EventPublish(mCurrentAlert));
//}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn AlertManager::handleLogout(const boost::shared_ptr<Notify>& notify)
//! \brief initiate sending logout
//! \param[in] notify internal triggering message
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AlertManager::handleLogout(const boost::shared_ptr<Notify>& notify) {
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: %s", __FUNCTION__);

    mFsm.process_event(aev::EventLogout(notify));
    TimerManager* tm = TimerManager::getInstance();
    tm->stopTimer(mMBox, *mTimers.find(TimerId::HEART_BEAT));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn AlertManager::handleConnected(const boost::shared_ptr<Notify>& notify)
//! \brief handling establishing connection notification from NetworkManager
//! \param[in] notify incoming message
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AlertManager::handleConnected(const boost::shared_ptr<Notify>& notify) {
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: %s", __FUNCTION__);

    mFsm.process_event(pev::EventConnected());
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn AlertManager::handleConnectionLost(const boost::shared_ptr<Notify>& notify)
//! \brief handling loss of connection notification from NetworkManager
//! \param[in] notify incoming message
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AlertManager::handleConnectionLost(const boost::shared_ptr<Notify>& notify) {
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: %s", __FUNCTION__);

    mFsm.process_event(pev::EventConnectionLost());
    TimerManager* tm = TimerManager::getInstance();
    tm->stopTimer(mMBox, *mTimers.find(TimerId::HEART_BEAT));
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn AlertManager::handleDisconnected(const boost::shared_ptr<Notify>& notify)
//! \brief handling disconnection notification from NetworkManager
//! \param[in] notify incoming message
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AlertManager::handleDisconnected(const boost::shared_ptr<Notify>& notify) {
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: %s", __FUNCTION__);

    mFsm.process_event(pev::EventDisconnected());
    TimerManager* tm = TimerManager::getInstance();
    tm->stopTimer(mMBox, *mTimers.find(TimerId::HEART_BEAT));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn AlertManager::handleSendCompleted(const boost::shared_ptr<Notify>& notify)
//! \brief handling sending completed notification from NetworkManager
//! \param[in] notify incoming message
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AlertManager::handleSendCompleted(const boost::shared_ptr<Notify>& notify){
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: %s", __FUNCTION__);

    mFsm.process_event(pev::EventSendCompleted(notify));
    // Check if any alert are in pending state
    sendMessageSelf(IntCmd::XEVINFO_CHECK);
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn AlertManager::handleSupplementData(const boost::shared_ptr<Notify>& notify)
//! \brief handling new supplementary data from DataProcessor
//! \param[in] notify incoming message
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AlertManager::handleSupplementData(const boost::shared_ptr<Notify>& notify) {

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: %s: mWarningMode is %d", __FUNCTION__, mWarningMode);

    if(mWarningMode)
    {
        shared_ptr<Data> xevinfo(new DataWarning(notify->getPayloadRef()));
        //mQueue[DataWarning::type()].push_back(xevinfo);
        mLastWarningData.push_back(xevinfo);

        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: The %s data has been arrived before 30s. Queue has %d messages",
            DataWarning::typeName(),
            mLastWarningData.size());

        mStorage.push(xevinfo);
        DLT_LOG_STRING(dlt_voc, DLT_LOG_DEBUG, "RMS: handleSupplementData Save Warning data to file.");
    }else
    {
        shared_ptr<Data> xevinfo(new DataSupplementary(notify->getPayloadRef()));
        mQueue[DataSupplementary::type()].push_back(xevinfo);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: The %s data has been arrived. Queue has %d messages",
            DataSupplementary::typeName(),
            mQueue[DataSupplementary::type()].size());
    }

    // Check if any alert are in pending state
    sendMessageSelf(IntCmd::XEVINFO_CHECK);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn AlertManager::handleRealtimeData(const boost::shared_ptr<Notify>& notify)
//! \brief handling new real-time data from DataProcessor
//! \param[in] notify incoming message
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AlertManager::handleRealtimeData(const boost::shared_ptr<Notify>& notify) {

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: %s: mWarningMode is %d", __FUNCTION__, mWarningMode);

    if(mWarningMode)
    {
        shared_ptr<Data> xevinfo(new DataWarning(notify->getPayloadRef()));
        mQueue[DataWarning::type()].push_back(xevinfo);

        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: The %s data has been arrived. Queue has %d messages",
            DataWarning::typeName(),
            mQueue[DataWarning::type()].size());

        mStorage.push(xevinfo);
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "RMS: handleRealtimeData Save Warning data to file.");
    }else
    {
        shared_ptr<Data> xevinfo(new DataRealTime(notify->getPayloadRef()));
        mQueue[DataRealTime::type()].push_back(xevinfo);

        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: The %s data has been arrived. Queue has %d messages",
            DataRealTime::typeName(),
            mQueue[DataRealTime::type()].size());
    }

    // Check if any alert are in pending state
    sendMessageSelf(IntCmd::XEVINFO_CHECK);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn AlertManager::handleWarningData(const boost::shared_ptr<Notify>& notify)
//! \brief handling new warning data from DataProcessor
//! \param[in] notify incoming message
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AlertManager::handleWarningData(const boost::shared_ptr<Notify>& notify) {
    shared_ptr<Data> xevinfo(new DataWarning(notify->getPayloadRef()));
    if (mQueue[DataWarning::type()].size() < Settings::getInstance()->getMaxWarningAlerts()) {
        mQueue[DataWarning::type()].push_back(xevinfo);
    }

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: The %s data has been arrived. Queue has %d messages",
        DataWarning::typeName(),
        mQueue[DataWarning::type()].size());

    sendMessageSelf(IntCmd::XEVINFO_CHECK);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn AlertManager::handleWarningStart(const boost::shared_ptr<Notify>& notify)
//! \brief handling start of warning mode notification
//! \param[in] notify incoming message
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AlertManager::handleWarningStart(const boost::shared_ptr<Notify>& notify) {
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: %s", __FUNCTION__);

    mWarningMode = true;
    TimerManager::getInstance()->stopTimer(mMBox, *mTimers.find(TimerId::HEART_BEAT));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn AlertManager::handleWarningStop(const boost::shared_ptr<Notify>& notify)
//! \brief handling end of warning mode notification
//! \param[in] notify incoming message
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AlertManager::handleWarningStop(const boost::shared_ptr<Notify>& notify) {
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: %s", __FUNCTION__);

    mWarningMode = false;
    while(!mLastWarningData.empty())
    {
        mQueue[DataWarning::type()].push_back(mLastWarningData.front());
        mLastWarningData.pop_front();
    }

    TimerManager::getInstance()->startTimer(mMBox, *mTimers.find(TimerId::HEART_BEAT));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn AlertManager::handleDBUSStart(const boost::shared_ptr<Notify>& notify)
//! \brief handling EVT_DBUS_START event from DBusManager, starting operation
//! \param[in] notify incoming message
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AlertManager::handleDBUSStart(const boost::shared_ptr<Notify>& notify) {
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: %s", __FUNCTION__);

    mStorage.sync(mQueue);
    sendMessage(mMBox, make_shared<Message>(Message(NotifyId::MSG_AM_LOGIN)));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn AlertManager::handleDBUSStop(const boost::shared_ptr<Notify>& notify)
//! \brief handling EVT_DBUS_STOP event from DBusManager, stopping operation
//! \param[in] notify incoming message
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AlertManager::handleDBUSStop(const boost::shared_ptr<Notify>& notify) {
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: %s", __FUNCTION__);

    move2SupplementaryCnt(mQueue[DataRealTime::type()].size());
    mStorage.sync(mQueue);
    for (std::set<TimerProp>::iterator it = mTimers.begin(); it != mTimers.end(); ++it) {
        stopTimer(*it);
    }

    sendMessage(mMBox, make_shared<Message>(Message(NotifyId::MSG_AM_LOGOUT)));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn AlertManager::handleReadReq(const boost::shared_ptr<Notify>& notify)
//! \brief handling request to read data through wired connection
//! \param[in] notify incoming message
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AlertManager::handleReadReq(const boost::shared_ptr<Notify>& notify) {
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: %s", __FUNCTION__);
    std::vector<uint8_t> xevData;
    if (!mQueue[DataSupplementary::type()].empty()) {
        mWiredAlert = mQueue[DataSupplementary::type()].front();
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: First available is %s at \"%s\"",
            DataSupplementary::typeName(),
            mWiredAlert->getPath().generic_string().c_str());
        xevData = mWiredAlert->getData();
    }
    printDump(xevData.data(), xevData.size());
    sendMessage(Mailbox::SERVICE_DBUS_MANAGER,
            make_shared<Message>(Message(NotifyId::MSG_AM_READ_RES, xevData)));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn AlertManager::handleReadAck(const boost::shared_ptr<Notify>& notify)
//! \brief handling ACK for data that was read through wired connection
//! \param[in] notify incoming message
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AlertManager::handleReadAck(const boost::shared_ptr<Notify>& notify) {

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: %s", __FUNCTION__);

    do {
        if (!mWiredAlert) {
            break;
        }

        if (mQueue[mWiredAlert->getType()].empty()) {
            break;
        }

        if (!mQueue[mWiredAlert->getType()].front()) {
            mQueue[mWiredAlert->getType()].pop_front();
            DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "RMS: Unexpected behavior: Alert from Queue is NULL");
            break;
        }

        if (!(*mWiredAlert == *mQueue[mWiredAlert->getType()].front())) {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "RMS: Unexpected behavior: Alerts should be the same");
            break;
        }

        mQueue[mWiredAlert->getType()].pop_front();
        if (!mWiredAlert->getPath().empty()) {
            mStorage.erase(mWiredAlert);
        }

        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: The %s data has been sent. Queue has %d messages",
            mWiredAlert->getTypeName(),
            mQueue[mWiredAlert->getType()].size());
    } while(false);

    mWiredAlert.reset();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn AlertManager::handleTimers(const boost::shared_ptr<Notify>& notify)
//! \brief dispatching timeout notifications from timers
//! \param[in] notify timeout message
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AlertManager::handleTimers(const boost::shared_ptr<Notify>& notify) {
    typedef EventHandler<
            TimerId,
            AlertManager,
            void (AlertManager::*)(const TimerProp&)
            > hr;

    static hr th[] = {
        hr(TimerId::RETRY_LOGIN_SHORT,   *this, &AlertManager::handleLoginTm),
        hr(TimerId::RETRY_LOGIN_LONG,    *this, &AlertManager::handleLoginTm),
        hr(TimerId::RETRY_PUBLISH_SHORT, *this, &AlertManager::handlePublishTm),
        hr(TimerId::RETRY_PUBLISH_LONG,  *this, &AlertManager::handlePublishTm),
        hr(TimerId::HEART_BEAT,          *this, &AlertManager::handleHeartbeatTm),
    };
    const TimerProp* t = reinterpret_cast<const TimerProp*>(
            notify->getPayloadRef().data());
    if (t) {
        for (uint32_t i = 0; i < sizeof(th)/ sizeof(th[0]); i++ ) {
            th[i].handle(static_cast<TimerId>(t->id), *t);
        }
    }
}

//------------------------------------------------------------------------------
// Asynchronous Internal Messages handlers
//------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn AlertManager::handleInt(const boost::shared_ptr<Notify>& notify)
//! \brief dispatching internal commands
//! \param[in] notify incoming message
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AlertManager::handleInt(const boost::shared_ptr<Notify>& notify) {

    typedef EventHandler<
            IntCmd,
            AlertManager,
            void (AlertManager::*)(const boost::shared_ptr<Notify>&)
            > hr;

    static hr amh[] = {
        hr(IntCmd::DISCONNECT,     *this, &AlertManager::handleIntDisconnect),
        hr(IntCmd::XEVINFO_CHECK,  *this, &AlertManager::handleIntxEvcdmInfo),
        hr(IntCmd::MOVE2SUPP,      *this, &AlertManager::move2SupplementaryReq),
    };

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: %s", __FUNCTION__);
    const IntHeader* h = reinterpret_cast<const IntHeader*>(
            notify->getPayloadRef().data());
    if (h) {
        for (uint32_t i = 0; i < sizeof(amh)/ sizeof(amh[0]); i++ ) {
            amh[i].handle(h->cmd, notify);
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn AlertManager::handleIntDisconnect(const boost::shared_ptr<Notify>& notify)
//! \brief handler for IntCmd::DISCONNECT internal command
//! \param[in] notify incoming message
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AlertManager::handleIntDisconnect(const boost::shared_ptr<Notify>& notify) {
    mFsm.process_event(aev::EventDisconnect());
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn AlertManager::handleIntxEvcdmInfo(const boost::shared_ptr<Notify>& notify)
//! \brief handler for IntCmd::XEVINFO_CHECK internal command
//! \param[in] notify incoming message
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AlertManager::handleIntxEvcdmInfo(const boost::shared_ptr<Notify>& notify) {
    if (!mNetworkAlert) {
        if (!mQueue[DataRealTime::type()].empty()) {
            mNetworkAlert = mQueue[DataRealTime::type()].front();
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: First available is %s",
                DataRealTime::typeName());
            mFsm.process_event(aev::EventPublish(mNetworkAlert));
        }else if (!mQueue[DataWarning::type()].empty()) {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: First available is %s",
                DataWarning::typeName());

            mNetworkAlert = mQueue[DataWarning::type()].front();
            mFsm.process_event(aev::EventPublish(mNetworkAlert));
        }else if (!mQueue[DataSupplementary::type()].empty()) {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: First available is %s",
                DataSupplementary::typeName());
            if (!mWarningMode) {
                mNetworkAlert = mQueue[DataSupplementary::type()].front();
                mFsm.process_event(aev::EventPublish(mNetworkAlert));
            } else {
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: Warning mode. Will wait for %s",
                    DataRealTime::typeName());
            }
        }
    } else {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "RMS: Another alert in sending process");
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn AlertManager::move2SupplementaryReq(const boost::shared_ptr<Notify>& notify)
//! \brief handler for IntCmd::MOVE2SUPP internal command
//! \param[in] notify incoming message
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AlertManager::move2SupplementaryReq(const boost::shared_ptr<Notify>& notify) {
    const IntHeader* ih = reinterpret_cast<const IntHeader*>(
            notify->getPayloadRef().data());
    Storage::AlertQueue& rtQueue = mQueue[DataRealTime::type()];
    Storage::AlertQueue& spQueue = mQueue[DataSupplementary::type()];
    uint32_t toMove = rtQueue.size();

    if (ih && ih->size == sizeof(IntCmdData::Move2Supp)) {
        toMove = reinterpret_cast<const IntCmdData::Move2Supp*>(
                ih->data)->queueSize;
    }

    move2SupplementaryCnt(toMove);

    while (spQueue.size() > Settings::getInstance()->getMaxTotalAlerts()) {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: Supplementary alerts limit has been reached, remove oldest %s",
                            spQueue.front()->getPath().c_str());
        mStorage.erase(spQueue.front());
        spQueue.pop_front();
    }

    for (Storage::AlertQueue::iterator it = spQueue.begin();
            it != spQueue.end(); ++it) {
        if ((*it)->getPath().empty()) {
            mStorage.push(*it);
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn AlertManager::move2SupplementaryCnt(uint32_t toMove)
//! \brief implementation of moving/converting specified number of items from real-time type to supplementary type
//! \param[in] toMove number of items to move
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AlertManager::move2SupplementaryCnt(uint32_t toMove) {
    Storage::AlertQueue& rtQueue = mQueue[DataRealTime::type()];
    Storage::AlertQueue& spQueue = mQueue[DataSupplementary::type()];
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: Move %d alert(s) to the %s",
        toMove,
        DataSupplementary::typeName());
    while (toMove-- && rtQueue.size()) {
        spQueue.push_back(convert2Supplementary(rtQueue.front()));
        rtQueue.pop_front();
    }
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: The %s queue has %d alerts",
        DataSupplementary::typeName(),
        spQueue.size());
}

//------------------------------------------------------------------------------
// Timers timeout handlers
//------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn AlertManager::handleLoginTm(const TimerProp& tp)
//! \brief handle timeout notifications of login timers
//! \param[in] tp timer properties struct
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AlertManager::handleLoginTm(const TimerProp& tp) {
    mFsm.process_event(aev::EventLogin());
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn AlertManager::handlePublishTm(const TimerProp& tp)
//! \brief handle timeout notifications of publishing timers
//! \param[in] tp timer properties struct
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AlertManager::handlePublishTm(const TimerProp& tp) {
    if (mNetworkAlert) {
        mFsm.process_event(aev::EventPublish(mNetworkAlert));
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn AlertManager::handleHeartbeatTm(const TimerProp& tp)
//! \brief handle timeout notifications of heartbeat timer
//! \param[in] tp timer properties struct
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AlertManager::handleHeartbeatTm(const TimerProp& tp) {
    DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "RMS: Heartbeat has been arrived");
    mFsm.process_event(aev::EventHeartbeat());
}

//------------------------------------------------------------------------------
// Service
//------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn AlertManager::handle(const boost::shared_ptr<Notify>& notify)
//! \brief method for handling incoming messages
//! \param[in] notify incoming message
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AlertManager::handle(const boost::shared_ptr<Notify>& notify) {
    typedef EventHandler<
            NotifyId,
            AlertManager,
            void (AlertManager::*)(const boost::shared_ptr<Notify>&)
            > hr;

    static hr amh[] = {
        hr(NotifyId::MSG_AM_INTERNAL,        *this, &AlertManager::handleInt),
        hr(NotifyId::MSG_TIMER_EXPIRED,      *this, &AlertManager::handleTimers),
        hr(NotifyId::MSG_AM_LOGIN,           *this, &AlertManager::handleLogin),
        hr(NotifyId::EVT_DBUS_START,         *this, &AlertManager::handleDBUSStart),
//        hr(NotifyId::MSG_AM_PUBLISH,         *this, &AlertManager::handlePublish),
        hr(NotifyId::MSG_AM_LOGOUT,          *this, &AlertManager::handleLogout),
        hr(NotifyId::EVT_DBUS_STOP,          *this, &AlertManager::handleDBUSStop),
        hr(NotifyId::MSG_DP_FAULT_DATA,      *this, &AlertManager::handleWarningData),
        hr(NotifyId::MSG_DP_REALTIME_DATA,   *this, &AlertManager::handleRealtimeData),
        hr(NotifyId::MSG_DP_SUPPLEMENT_DATA, *this, &AlertManager::handleSupplementData),
        hr(NotifyId::EVT_NM_CONNECTED,       *this, &AlertManager::handleConnected),
        hr(NotifyId::EVT_NM_CONNECTION_LOST, *this, &AlertManager::handleConnectionLost),
        hr(NotifyId::EVT_NM_DISCONNECTED,    *this, &AlertManager::handleDisconnected),
        hr(NotifyId::EVT_NM_SEND_COMPLETED,  *this, &AlertManager::handleSendCompleted),
        hr(NotifyId::EVT_DP_WARNING_MODE_STOP,  *this, &AlertManager::handleWarningStop),
        hr(NotifyId::EVT_DP_WARNING_MODE_START, *this, &AlertManager::handleWarningStart),
        hr(NotifyId::MSG_AM_READ_REQ,  *this, &AlertManager::handleReadReq),
        hr(NotifyId::MSG_AM_READ_ACK,  *this, &AlertManager::handleReadAck),
    };

    for (uint32_t i = 0; i < sizeof(amh)/ sizeof(amh[0]); i++ ) {
        amh[i].handle(notify->getNotifyId(), notify);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn AlertManager::onStart()
//! \brief Function will be called when AlertManager is started. It subscribes for events that need to be processed
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AlertManager::onStart() {
    subscribe(make_shared<Event>(Event(NotifyId::EVT_NM_CONNECTED)));
    subscribe(make_shared<Event>(Event(NotifyId::EVT_NM_CONNECTION_LOST)));
    subscribe(make_shared<Event>(Event(NotifyId::EVT_NM_DISCONNECTED)));
    subscribe(make_shared<Event>(Event(NotifyId::EVT_NM_SEND_COMPLETED)));
    subscribe(make_shared<Event>(Event(NotifyId::EVT_DBUS_START)));
    subscribe(make_shared<Event>(Event(NotifyId::EVT_DBUS_STOP)));
    subscribe(make_shared<Event>(Event(NotifyId::EVT_DP_WARNING_MODE_START)));
    subscribe(make_shared<Event>(Event(NotifyId::EVT_DP_WARNING_MODE_STOP)));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn
//! \brief Function will be called when AlertManager is stopped. It currently does nothing.
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AlertManager::onStop() {
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn AlertManager::getInstance()
//! \brief  Function for getting AlertManager object.
//! \return pointer to AlertManager object
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
AlertManager* AlertManager::getInstance() {
    static AlertManager am(Mailbox::SERVICE_ALERT_MANAGER);
    return &am;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn
//! \brief Get warningMode
//! \return warningMode
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool AlertManager::getWarningMode()
{
    return mWarningMode;
}

