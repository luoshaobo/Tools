///////////////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  @file service.cc
//  This file handles the communication with the VuC, using the OTP VCS-interface
 
// @project     GLY_TCAM
// @subsystem   VolvoOnCall
// @author      Shen Jiaojiao
// @Init date   12-Sep-2018
/////////////////////////////////////////////////////////////////////////////

#include <rms/sys/service.h>
#include <rms/sys/timerManager.h>
#include "dlt/dlt.h"

DLT_IMPORT_CONTEXT(dlt_voc);

#define STR(x) #x

//////////////////////////////////////////////////////////////////////////////////
//! \fn Service::Service(const Mailbox& mbox)
//! \brief Constructor
//! \param[in] mbox Mailbox of the service
//! \return void
//////////////////////////////////////////////////////////////////////////////////
Service::Service(const Mailbox& mbox) :
        mMBox(mbox)
{
}

//////////////////////////////////////////////////////////////////////////////////
//! \fn Service::~Service()
//! \brief destructor
//! \return void
//////////////////////////////////////////////////////////////////////////////////
Service::~Service()
{
}

//////////////////////////////////////////////////////////////////////////////////
//! \fn Service::subscribe(const boost::shared_ptr<Event>& event)
//! \brief subscribe on events
//! \param[in] event An event for subscription
//! \return void
//////////////////////////////////////////////////////////////////////////////////
void Service::subscribe(const boost::shared_ptr<Event>& event) const
{
    Postoffice::getInstance()->subscribe(mMBox, event);
}


//////////////////////////////////////////////////////////////////////////////////
//! \fn Service::run()
//! \brief Start service
//! \return void
//////////////////////////////////////////////////////////////////////////////////
void Service::run()
{
    bool rc;
    boost::shared_ptr<Notify> notify;

    while (mProccessing) {
        do {
            rc = getMessage(notify);
            if (!rc) {
                break;
            }
            handle(notify);
        } while (rc);
    }
}

//////////////////////////////////////////////////////////////////////////////////
//! \fn Service::getMessage(boost::shared_ptr<Notify>& notify)
//! \brief get message from mail box
//! \param[out] notify Pointer to message
//! \return true if success
//////////////////////////////////////////////////////////////////////////////////
bool Service::getMessage(boost::shared_ptr<Notify>& notify) const
{
    return Postoffice::getInstance()->getMessage(mMBox, notify)
            == Postoffice::Result::SUCCESSFUL;
}
#if BOOST_VERSION >= 105000
//------------------------------------------------------------------------------
bool Service::getMessage(boost::shared_ptr<Notify>& notify,
        const boost::chrono::milliseconds& ms) const {
    return Postoffice::getInstance()->getMessage(mMBox, notify, ms)
            == Postoffice::Result::SUCCESSFUL;
}
#endif
//////////////////////////////////////////////////////////////////////////////////
//! \fn Service::sendMessage(const Mailbox& mbox, const boost::shared_ptr<Message>& msg)
//! \brief Send message to mailbox
//! \param[in] mbox recipient Mailbox
//! \param[in] msg Message to be sent
//! \return true if success
//////////////////////////////////////////////////////////////////////////////////
bool Service::sendMessage(const Mailbox& mbox,
        const boost::shared_ptr<Message>& msg) const {
    msg->setMailbox(mMBox);
    bool rc = false;
    rc = (Postoffice::getInstance()->sendMessage(mbox, msg)
            == Postoffice::Result::SUCCESSFUL);
    if (!rc) {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "RMS: Fail to send message %d to mbox %d", msg->getNotifyId(), mbox);
    }
    return rc;
}

//////////////////////////////////////////////////////////////////////////////////
//! \fnService::sendNotify(const Mailbox& mbox,const boost::shared_ptr<Notify>& msg)
//! \brief Send notification
//! \param[in] mbox recipient Mailbox
//! \param[in] msg notification to be sent
//! \return true if success
//////////////////////////////////////////////////////////////////////////////////
bool Service::sendNotify(const Mailbox& mbox,
        const boost::shared_ptr<Notify>& msg) const
{
    bool rc = false;
    rc = (Postoffice::getInstance()->sendNotify(mbox, msg)
            == Postoffice::Result::SUCCESSFUL);
    if (!rc) {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "RMS: Fail to send notification %d to mbox %d", msg->getNotifyId(), mbox);
    }
    return rc;
}

//////////////////////////////////////////////////////////////////////////////////
//! \fn Service::sendEvent(const boost::shared_ptr<Event>& event)
//! \brief Send event
//! \param[in] event An event to be sent
//! \return true if success
//////////////////////////////////////////////////////////////////////////////////
bool Service::sendEvent(const boost::shared_ptr<Event>& event) const
{
    event->setMailbox(mMBox);
    bool rc = false;
    rc = (Postoffice::getInstance()->sendEvent(event)
            == Postoffice::Result::SUCCESSFUL);
    if (!rc) {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "RMS: Fail to send event %d to subscribers", event->getNotifyId());
    }
    return rc;
}

//////////////////////////////////////////////////////////////////////////////////
//! \fn Service::sendNotify(const boost::shared_ptr<Notify>& event)
//! \brief Send notification
//! \param[in] event notification to be sent
//! \return true if success
//////////////////////////////////////////////////////////////////////////////////
bool Service::sendNotify(const boost::shared_ptr<Notify>& event) const {
    bool rc = false;
    rc = (Postoffice::getInstance()->sendNotify(event)
            == Postoffice::Result::SUCCESSFUL);
    if (!rc) {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "RMS: Fail to send notification %d to subscribers", event->getNotifyId());
    }
    return rc;
}
//////////////////////////////////////////////////////////////////////////////////
//! \fn Mailbox Service::getMailbox()
//! \brief getter fo mail box
//! \return service mailbox
//////////////////////////////////////////////////////////////////////////////////
const Mailbox Service::getMailbox() const
{
    return mMBox;
}
//////////////////////////////////////////////////////////////////////////////////
//! \fn Service::startTimer(const TimerProp& t)
//! \brief wrapper over start timer
//! \param[in] t A reference to timer object
//! \return void
//////////////////////////////////////////////////////////////////////////////////
void Service::startTimer(const TimerProp& t) const
{
    TimerManager::getInstance()->startTimer(mMBox, t);
}

//////////////////////////////////////////////////////////////////////////////////
//! \fn Service::stopTimer(const TimerProp& t)
//! \brief wrapper over stop timer
//! \param[in] t A reference to timer object
//! \return void
//////////////////////////////////////////////////////////////////////////////////
void Service::stopTimer(const TimerProp& t) const
{
    TimerManager::getInstance()->stopTimer(mMBox, t);
}

///////////////////////////////////////////////////////////////////////////////
//! \fn Service::onStart()
//! \brief Function will be called when service is started.
//! \return void
///////////////////////////////////////////////////////////////////////////////
void Service::onStart(){}

///////////////////////////////////////////////////////////////////////////////
//! \fn Thread::onStop()
//! \brief Function will be called when service is stopped.
//! \return void
///////////////////////////////////////////////////////////////////////////////
void Service::onStop(){}
