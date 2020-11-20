////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  @file postoffice.h
//  This file handles the communication with the VuC, using the OTP VCS-interface
 
// @project     GLY_TCAM
// @subsystem   VolvoOnCall
// @author      Jiaojiao Shen
// @Init date   12-Sep-2018
////////////////////////////////////////////////////////////////////////////

#include <algorithm>
#include <string>

#include <rms/util/utils.h>
#include <rms/sys/postoffice.h>
#include "dlt/dlt.h"

DLT_IMPORT_CONTEXT(dlt_voc);

//#define STR(x) #x

using namespace std;

////////////////////////////////////////////////////////////////////////////////
//! \fn Message::Message(const NotifyId& id, const Mailbox& mbox, const Payload& data)
//! \brief Message Constructor
//! \param[in] id  A notify id
//! \param[in] mbox Sender Mailbox
//! \param[in] data Message payload
//! \return  void
////////////////////////////////////////////////////////////////////////////////
Message::Message(const NotifyId& id, const Mailbox& mbox, const Payload& data) :
         mNotifyId(id), mMailbox(mbox), mPayload(data) {
}

////////////////////////////////////////////////////////////////////////////////
//! \fn Message::Message(const NotifyId& id, const Mailbox& mbox)
//! \brief Message Constructor
//! \param[in] id  A notify id
//! \param[in] mbox Sender Mailbox
//! \return  void
////////////////////////////////////////////////////////////////////////////////
Message::Message(const NotifyId& id, const Mailbox& mbox) :
        mNotifyId(id), mMailbox(mbox), mPayload(0) {
}

////////////////////////////////////////////////////////////////////////////////
//! \fn Message::Message(const NotifyId& id, const Payload& data)
//! \brief Message Constructor
//! \param[in] id  A notify id
//! \param[in] data Message payload
//! \return  void
////////////////////////////////////////////////////////////////////////////////
Message::Message(const NotifyId& id, const Payload& data) :
        mNotifyId(id), mMailbox(mailbox::EMPTY), mPayload(data) {
}

////////////////////////////////////////////////////////////////////////////////
//! \fn Message::Message(const NotifyId& id)
//! \brief Message Constructor
//! \param[in] id  A notify id
//! \return  void
////////////////////////////////////////////////////////////////////////////////
Message::Message(const NotifyId& id) :
        mNotifyId(id), mMailbox(mailbox::EMPTY), mPayload(0) {

}

////////////////////////////////////////////////////////////////////////////////
//! \fn Message::getMailbox()
//! \brief getter for Mailbox
//! \return mailboxId
////////////////////////////////////////////////////////////////////////////////
Mailbox Message::getMailbox() const {
    return mMailbox;
}

////////////////////////////////////////////////////////////////////////////////
//! \fn Message::setMailbox(const Mailbox& mbox)
//! \brief setter for Mailbox
//! \param[in] mbox value of mailboxId to set
//! \return void
////////////////////////////////////////////////////////////////////////////////
void Message::setMailbox(const Mailbox& mbox){
    mMailbox = mbox;
}

////////////////////////////////////////////////////////////////////////////////
//! \fn Message::setPayload(const Payload& pl)
//! \brief setter for Payload
//! \param[in] pl Payload to set
//! \return void
////////////////////////////////////////////////////////////////////////////////
void Message::setPayload(const Payload& pl) {
    mPayload = pl;
}

////////////////////////////////////////////////////////////////////////////////
//! \fn Message::getPayloadCopy()
//! \brief getter for Payload returning copy
//! \return Payload
////////////////////////////////////////////////////////////////////////////////
Payload Message::getPayloadCopy() const {
    return mPayload;
}

////////////////////////////////////////////////////////////////////////////////
//! \fn Message::getPayloadRef()
//! \brief getter for Payload returning reference
//! \return const reference to Payload
////////////////////////////////////////////////////////////////////////////////
inline const Payload& Message::getPayloadRef() const {
    return mPayload;
}

////////////////////////////////////////////////////////////////////////////////
//! \fn Message::getNotifyId()
//! \brief getter for NotifyId
//! \return NotifyId
////////////////////////////////////////////////////////////////////////////////
NotifyId Message::getNotifyId() const {
    return mNotifyId;
}

////////////////////////////////////////////////////////////////////////////////
//! \fn Message::getTypeStr()
//! \brief getter for type string
//! \return string
////////////////////////////////////////////////////////////////////////////////
const string Message::getTypeStr() const {
    return Message::typeStr();
}

////////////////////////////////////////////////////////////////////////////////
//! \fn Message::typeStr()
//! \brief getter for type string
//! \return string
////////////////////////////////////////////////////////////////////////////////
const std::string Message::typeStr() {
    return "Message";
}

////////////////////////////////////////////////////////////////////////////////
//! \fn Message::cast(const boost::shared_ptr<Notify>& notify)
//! \brief Makes cast Notify object to Meassage object
//! \param[in] notify Notify for casting
//! \return shared pointer to message object
////////////////////////////////////////////////////////////////////////////////
boost::shared_ptr<Message> Message::cast(const boost::shared_ptr<Notify>& notify)
{
    boost::shared_ptr<Message> obj;
    if (notify->getTypeStr() == typeStr()) {
        obj = boost::dynamic_pointer_cast<Message>((notify));
    }
    return obj;
}

////////////////////////////////////////////////////////////////////////////////
//! \fn Event::Event(const NotifyId& id, const Mailbox& mbox, const Payload& data)
//! \brief Constructor
//! \param[in] id  A notify id
//! \param[in] mbox Sender Mailbox
//! \param[in] data Message payload
//! \return void
////////////////////////////////////////////////////////////////////////////////
Event::Event(const NotifyId& id, const Mailbox& mbox, const Payload& data) :
        mNotifyId(id), mMailbox(mbox), mPayload(data) {
}

////////////////////////////////////////////////////////////////////////////////
//! \fn Event::Event(const NotifyId& id, const Mailbox& mbox)
//! \brief Constructor
//! \param[in] id  A notify id
//! \param[in] mbox Sender Mailbox
//! \return void
////////////////////////////////////////////////////////////////////////////////
Event::Event(const NotifyId& id, const Mailbox& mbox) :
        mNotifyId(id), mMailbox(mbox), mPayload(0) {
}


////////////////////////////////////////////////////////////////////////////////
//! \fn Event::Event(const NotifyId& id, const Payload& data)
//! \brief Constructor
//! \param[in] id  A notify id
//! \param[in] data Message payload
//! \return void
////////////////////////////////////////////////////////////////////////////////
Event::Event(const NotifyId& id, const Payload& data) :
        mNotifyId(id), mMailbox(mailbox::EMPTY), mPayload(data) {
}

////////////////////////////////////////////////////////////////////////////////
//! \fn Event::Event(const NotifyId& id)
//! \brief Constructor
//! \param[in] id  A notify id
//! \return void
////////////////////////////////////////////////////////////////////////////////
Event::Event(const NotifyId& id) :
        mNotifyId(id), mMailbox(mailbox::EMPTY), mPayload(0) {
}

////////////////////////////////////////////////////////////////////////////////
//! \fn Event::getMailbox()
//! \brief getter for Mailbox
//! \return mailboxId
////////////////////////////////////////////////////////////////////////////////
Mailbox Event::getMailbox() const {
    return mMailbox;
}

////////////////////////////////////////////////////////////////////////////////
//! \fn Event::setMailbox(const Mailbox& mbox)
//! \brief setter for Mailbox
//! \param[in] mbox value of mailboxId to set
//! \return void
////////////////////////////////////////////////////////////////////////////////
void Event::setMailbox(const Mailbox& mbox){
    mMailbox = mbox;
}

////////////////////////////////////////////////////////////////////////////////
//! \fn Event::setPayload(const Payload& pl)
//! \brief setter for Payload
//! \param[in] pl Payload to set
//! \return void
////////////////////////////////////////////////////////////////////////////////
void Event::setPayload(const Payload& pl) {
    mPayload = pl;
}

////////////////////////////////////////////////////////////////////////////////
//! \fn Event::getPayloadCopy()
//! \brief getter for Payload returning copy
//! \return Payload
////////////////////////////////////////////////////////////////////////////////
Payload Event::getPayloadCopy() const {
    return mPayload;
}

////////////////////////////////////////////////////////////////////////////////
//! \fn Event::getPayloadRef()
//! \brief getter for Payload returning reference
//! \return const reference to Payload
////////////////////////////////////////////////////////////////////////////////
const Payload& Event::getPayloadRef() const {
    return mPayload;
}


////////////////////////////////////////////////////////////////////////////////
//! \fn Event::getNotifyId()
//! \brief getter for NotifyId
//! \return NotifyId
////////////////////////////////////////////////////////////////////////////////
NotifyId Event::getNotifyId() const {
    return mNotifyId;
}


////////////////////////////////////////////////////////////////////////////////
//! \fn Event::getTypeStr()
//! \brief getter for type string
//! \return string
////////////////////////////////////////////////////////////////////////////////
const string Event::getTypeStr() const {
    return Event::typeStr();
}

////////////////////////////////////////////////////////////////////////////////
//! \fn Event::typeStr()
//! \brief getter for type string
//! \return string
////////////////////////////////////////////////////////////////////////////////
const std::string Event::typeStr() {
    return "Event";
}

////////////////////////////////////////////////////////////////////////////////
//! \fn Event::cast(const boost::shared_ptr<Notify>& notify)
//! \brief Makes cast Notify object to Event object
//! \param[in] notify Notify for casting
//! \return shared pointer to Event object
////////////////////////////////////////////////////////////////////////////////
boost::shared_ptr<Event> Event::cast(const boost::shared_ptr<Notify>& notify)
{
    boost::shared_ptr<Event> obj;
    if (notify->getTypeStr() == typeStr()) {
        obj = boost::dynamic_pointer_cast<Event>((notify));
    }
    return obj;
}

//==============================================================================
boost::shared_ptr<Postoffice> Postoffice::mSelf;

////////////////////////////////////////////////////////////////////////////////
//! \fn Postoffice::getInstance()
//! \brief Provide instance of Postoffice. If not exist create one
//! \return instance of Postoffice
////////////////////////////////////////////////////////////////////////////////
boost::shared_ptr<Postoffice> Postoffice::getInstance() {
    if (!mSelf) {
        static boost::mutex localmutex;
        boost::lock_guard<boost::mutex> lock(localmutex);
        if (!mSelf) {
            mSelf.reset(new Postoffice());
        }
    }
    return mSelf;
}

////////////////////////////////////////////////////////////////////////////////
//! \fn Postoffice::subscribe(const Mailbox& mbox, const boost::shared_ptr<Event>& event)
//! \brief Subscribe mailbox to receive event
//! \param[in] mbox Recipient Mailbox
//! \param[in] event for subscription
//! \return void
////////////////////////////////////////////////////////////////////////////////
void Postoffice::subscribe(const Mailbox& mbox, const boost::shared_ptr<Event>& event)
{
    boost::lock_guard<boost::mutex> lock(mSubscriberMutex);
    NotifyList& box = mSubscribers[mbox];
    box.insert(event);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: subscribe from %d of event = %d" , mbox, event->getNotifyId());
}

////////////////////////////////////////////////////////////////////////////////
//! \fn Postoffice::getMessage(const Mailbox& mbox, boost::shared_ptr<Notify>& notify)
//! \brief Get event from mailbox
//! \param[in] mbox Recipient Mailbox
//! \param[out] notify Pointer to store read Notify
//! \return Result
////////////////////////////////////////////////////////////////////////////////
Postoffice::Result Postoffice::getMessage(const Mailbox& mbox,
        boost::shared_ptr<Notify>& notify)
{
    Postoffice::Result rc = Postoffice::NONE;
    notify.reset();
    boost::shared_ptr<Postoffice::Content> boxData = getContent(mbox);
    boost::unique_lock<boost::mutex> ulock(boxData->msgMutex);

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: Wait nid=? to_mb=%d, queue_size=%d", mbox, boxData->msgQueue.size());
    boxData->cv.wait(ulock, Content::Pred(boxData->msgQueue));
    notify = boxData->msgQueue.front();
    boxData->msgQueue.pop();
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: Pop nid=%d, from_mb=%d, to_mb=%d, queue_size=%d", notify->getNotifyId(),
            notify->getMailbox(), mbox,
            boxData->msgQueue.size());
    rc = Postoffice::SUCCESSFUL;
    return rc;
}

#if BOOST_VERSION >= 105000
//------------------------------------------------------------------------------
Postoffice::Result Postoffice::getMessage(const Mailbox& mbox,
        boost::shared_ptr<Notify>& notify,
        const boost::chrono::milliseconds& ms) {
    Postoffice::Result rc = Postoffice::NONE;
    bool waitRc = false;
    notify.reset();
    boost::shared_ptr<Postoffice::Content> boxData = getContent(mbox);
    boost::unique_lock<boost::mutex> ulock(boxData->msgMutex);
    waitRc = boxData->cv.wait_for(ulock, ms, Content::Pred(boxData->msgQueue));
    if (waitRc) {
        notify = boxData->msgQueue.front();
        boxData->msgQueue.pop();
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: Pop from_mb=%d, nid=%d, queue_size=%d", mbox,
                notify->getNotifyId(), boxData->msgQueue.size());
        rc = Postoffice::SUCCESSFUL;
    }
    return rc;
}
#endif

////////////////////////////////////////////////////////////////////////////////
//! \fn Postoffice::sendMessage(const Mailbox& mbox, const boost::shared_ptr<Message>& msg)
//! \brief Send message
//! \param[in] mbox recipient Mailbox
//! \param[in] msg Message to be sent
//! \return Result
////////////////////////////////////////////////////////////////////////////////
Postoffice::Result Postoffice::sendMessage(const Mailbox& mbox,
        const boost::shared_ptr<Message>& msg) {
    return sendNotify(mbox, msg);
}

////////////////////////////////////////////////////////////////////////////////
//! \fn Postoffice::sendNotify(const Mailbox& mbox, const boost::shared_ptr<Notify>& msg)
//! \brief Send notification
//! \param[in] mbox recipient Mailbox
//! \param[in] msg notification to be sent
//! \return Result
////////////////////////////////////////////////////////////////////////////////
Postoffice::Result Postoffice::sendNotify(const Mailbox& mbox,
        const boost::shared_ptr<Notify>& msg)
{
    if (msg->getNotifyId() > NotifyId::MSG_FIRST) {
        boost::shared_ptr<Postoffice::Content> boxData = getContent(mbox);
        boost::unique_lock<boost::mutex> ulock(boxData->msgMutex);
        boxData->msgQueue.push(msg);
        if (MAX_QUEUE_SIZE < boxData->msgQueue.size()) {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: Pop from %d", mbox);
            boxData->msgQueue.pop();
        }
        boxData->cv.notify_one();
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: Push nid=%d, from_mb=%d, to_mb=%d, queue_size=%d",
                (int)msg->getNotifyId(), (int)msg->getMailbox(), mbox,
                boxData->msgQueue.size());
    } else {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "RMS: Invalid MSG_ID, below threshold");
    }
    return Postoffice::SUCCESSFUL;
}

////////////////////////////////////////////////////////////////////////////////
//! \fn Result Postoffice::sendEvent(const boost::shared_ptr<Event>& event)
//! \brief Send Event
//! \param[in] event notification to be sent
//! \return Result
////////////////////////////////////////////////////////////////////////////////
Postoffice::Result Postoffice::sendEvent(const boost::shared_ptr<Event>& event)
{
    return sendNotify(event);
}

////////////////////////////////////////////////////////////////////////////////
//! \fn Postoffice:::sendNotify(const boost::shared_ptr<Notify>& event)
//! \brief Send notification
//! \param[in] event notification to be sent
//! \return Result
////////////////////////////////////////////////////////////////////////////////
Postoffice::Result Postoffice::sendNotify( const boost::shared_ptr<Notify>& event )
{
    //bool cvnotify = false;
    if (event->getNotifyId() < NotifyId::EVT_LAST) {
        boost::lock_guard<boost::mutex> lock2(mSubscriberMutex);
        for (map<Mailbox, NotifyList>::iterator it = mSubscribers.begin();
                it != mSubscribers.end();it++) {
            NotifyList::iterator subscriberEventIt = find_if(
                    it->second.begin(), it->second.end(), NotifyPred(event));
            if (subscriberEventIt != it->second.end()) {
                boost::shared_ptr<Postoffice::Content> boxData = getContent(
                        it->first);
                boost::unique_lock<boost::mutex> ulock(boxData->msgMutex);
                boxData->msgQueue.push(event);
                if (MAX_QUEUE_SIZE < boxData->msgQueue.size()) {
                    boxData->msgQueue.pop();
                }
                boxData->cv.notify_one();

                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: Push nid=%d to_mb=%d, queue size=%d", event->getNotifyId(),
                        it->first, boxData->msgQueue.size());
            }
        }
    } else {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "RMS: Invalid MSG_ID, below threshold");
    }
    return Postoffice::SUCCESSFUL;
}

////////////////////////////////////////////////////////////////////////////////
//! \fn Postoffice::getContent(const Mailbox& mb)
//! \brief Provide access to message storage by mailbox
//! \param[in] mb Mailbox id of the message storage
//! \return message storage pointer
////////////////////////////////////////////////////////////////////////////////
boost::shared_ptr<Postoffice::Content> Postoffice::getContent(const Mailbox& mb)
{
    boost::lock_guard<boost::mutex> lock(mQueueMutex);
    boost::shared_ptr<Postoffice::Content>& sp = mPost[mb];
    if (!sp) {
        sp.reset(new Postoffice::Content());
    }
    return sp;
}
