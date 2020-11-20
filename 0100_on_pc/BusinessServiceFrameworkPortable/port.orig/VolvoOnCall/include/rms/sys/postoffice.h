///////////////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  @file postoffice.h
//  This file handles the communication with the VuC, using the OTP VCS-interface
 
// @project     GLY_TCAM
// @subsystem   VolvoOnCall
// @author      Shen Jiaojiao
// @Init date   21-Sep-2018
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <vector>
#include <queue>
#include <map>
#include <set>
#include <memory>
#include <mutex>
#include <list>
#include <boost/smart_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>

namespace mailbox {

//! \enum mailbox::Id
//! \brief mailboxes list ids
enum Id {
    EMPTY = 0,
    SERVICE_TIMER,
    SERVICE_NETWORK_MANAGER,
    SERVICE_ALERT_MANAGER,
    SERVICE_DBUS_MANAGER,
    SERVICE_DATA_PROCESSOR,
    TEST1 = 0x100,
    TEST2,
};
}
typedef mailbox::Id Mailbox;

namespace notifyid {

//! \enum notifyid::Id
//! \brief internal event list
enum  Id {
    EVT_FIRST = 0x10,
    EVT_DBUS_START,
    EVT_DBUS_STOP,
    EVT_NM_CONNECTED,
    EVT_NM_CONNECTION_LOST,
    EVT_NM_DISCONNECTED,
    EVT_NM_SEND_COMPLETED,
    EVT_DP_WARNING_MODE_START,
    EVT_DP_WARNING_MODE_STOP,
    EVT_DBUS_PERODIC_DATA,
    EVT_DBUS_WARNING_DATA,
    EVT_DBUS_CONN_PARAM,
    EVT_DBUS_SETTINGS_UPDATE,
    EVT_DBUS_ENTER_WARNING_MODE,
    EVT_LAST,


    MSG_FIRST = 0x100,
    MSG_REQ_TIMER_START,
    MSG_REQ_TIMER_STOP,
    MSG_TIMER_EXPIRED,

    MSG_NM_CONNECT,
    MSG_NM_DISONNECT,
    MSG_NM_SEND_RAW,
    MSG_NM_SEND_TM,

    MSG_AM_READ_REQ,
    MSG_AM_READ_RES,
    MSG_AM_READ_ACK,
    MSG_AM_INTERNAL,
    MSG_AM_LOGIN,
    MSG_AM_PUBLISH,
    MSG_AM_LOGOUT,

    MSG_DP_REALTIME_DATA,
    MSG_DP_SUPPLEMENT_DATA,
    MSG_DP_FAULT_DATA,

    MSG_LAST,
};
};

typedef notifyid::Id NotifyId;

typedef std::vector<uint8_t> Payload;

/*! \struct Notify
    \brief Base interface description for events and messages
*/
struct Notify {
    virtual ~Notify() {}
    /// setter for Mailbox
    virtual void setMailbox(const Mailbox& mbox) = 0;
    /// getter for Mailbox
    virtual Mailbox getMailbox() const = 0;
    /// getter for Notification ID
    virtual NotifyId getNotifyId() const = 0;
    /// getter for payload
    virtual Payload getPayloadCopy() const = 0;
    /// setter for payload
    virtual void setPayload(const Payload& pl) = 0;
    /// getter for payload
    virtual const Payload& getPayloadRef() const = 0;
    /// return message type in string
    virtual const std::string getTypeStr() const = 0;

    inline bool operator==(const Notify& b) const {
        return this->getNotifyId() == b.getNotifyId();
    }
};

//! \class Message
//! \brief store information about message. Purpose of message is send data to Mailbox
class Message : public Notify {
public:

    Message(const NotifyId& id, const Mailbox& mbox, const Payload& data);

    Message(const NotifyId& id, const Mailbox& mbox);

    Message(const NotifyId& id, const Payload& data);

    Message(const NotifyId& id);
    virtual void setMailbox(const Mailbox& mbox);
    virtual Mailbox getMailbox() const;
    virtual void setPayload(const Payload& pl);
    virtual Payload getPayloadCopy() const;
    virtual const Payload& getPayloadRef() const;
    virtual NotifyId getNotifyId() const;
    virtual const std::string getTypeStr() const;
    static const std::string typeStr();

    static boost::shared_ptr<Message> cast(const boost::shared_ptr<Notify>& notify);
private:
    //! \var mNotifyId
    //! \brief NotifyId value with ID of message
    const NotifyId mNotifyId;

    //! \var mMailbox
    //! \brief mailbox ID to which message should be sent
    Mailbox mMailbox;

    //! \var mPayload
    //! \brief stores message data
    Payload mPayload;

};

//! \class Event
//! \brief store information about Event. Purpose of Event is inform subscribed clients
class Event : public Notify {
public:

    Event(const NotifyId& id, const Mailbox& mbox, const Payload& data);

    Event(const NotifyId& id, const Mailbox& mbox);

    Event(const NotifyId& id, const Payload& data);

    Event(const NotifyId& id);
    virtual void setMailbox(const Mailbox& mbox);
    virtual Mailbox getMailbox() const;
    virtual Payload getPayloadCopy() const;
    virtual void setPayload(const Payload& pl);
    virtual const Payload& getPayloadRef() const;
    virtual NotifyId getNotifyId() const;
    virtual const std::string getTypeStr() const;
    static const std::string typeStr();
    static boost::shared_ptr<Event> cast(const boost::shared_ptr<Notify>& notify);
private:
    //! \var mNotifyId
    //! \brief NotifyId value with ID of event
    const NotifyId mNotifyId;

    //! \var mMailbox
    //! \brief mailbox ID to which event should be sent
    Mailbox mMailbox;

    //! \var mPayload
    //! \brief stores event data
    Payload mPayload;
};

//! \class NotifyPred
//! \brief predicate to find item with specific notifyId
class NotifyPred {
public:
    NotifyPred(const boost::shared_ptr<Notify>& notify):
        notify(notify) { }
    bool operator()(const boost::shared_ptr<Notify>& notify) {
        return *this->notify == *notify;
    }
private:
    const boost::shared_ptr<Notify> notify;
};


typedef std::set<boost::shared_ptr<Notify> > NotifyList ;
typedef std::queue<boost::shared_ptr<Notify> > NotifyQueue;

//! \class Postoffice
//! \brief Implementation logic of processing messages and events
class Postoffice {

    //! \enum Settings
    //! \brief set of const settings
    enum Settings {
        MAX_QUEUE_SIZE = 512
    };

    //! \struct Content
    //!  \brief struct for storing messages
    typedef struct Content {
        Content() {}

        //! \var msgQueue
        //! \brief notify queue
        NotifyQueue msgQueue;

        //! \var msgMutex
        //! \brief mutex for safe multithreaded access to message queue
        boost::mutex msgMutex;

        //! \var msgQueue
        //! \brief condition_variable for safe multithreaded access to message queue
        boost::condition_variable cv;

        //! \class Pred
        //! \brief predicate to wait for new messages in a queue
        struct Pred {
            bool operator()() {
                return !msgQueue.empty();
            }
            Pred(const NotifyQueue& queue):
                msgQueue(queue) {}
        private:
            const NotifyQueue& msgQueue;
        };
    private:
        Content(const Content&);
    } Content;



    Postoffice() { }
public:
    //! \enum Result
    //! \brief enumeration for operation result
    typedef enum {
        SUCCESSFUL,
        NONE,
        ERROR
    } Result;

    Result getMessage(const Mailbox& mbox, boost::shared_ptr<Notify>& notify);
#if BOOST_VERSION >= 105000
    Result getMessage(const Mailbox& mbox, boost::shared_ptr<Notify>& notify,
            const boost::chrono::milliseconds& ms);
#endif

    Result sendMessage(const Mailbox& mbox, const boost::shared_ptr<Message>& msg);

    Result sendNotify(const Mailbox& mbox, const boost::shared_ptr<Notify>& msg);

    Result sendNotify(const boost::shared_ptr<Notify>& event);

    Result sendEvent(const boost::shared_ptr<Event>& event);

    void subscribe(const Mailbox& ownMbox, const boost::shared_ptr<Event>& event);

    static boost::shared_ptr<Postoffice> getInstance();
private:
    boost::shared_ptr<Content> getContent(const Mailbox& mb);
private:

    //! \var mPost
    //! \brief map with mailbox Id and message storage
    std::map<Mailbox, boost::shared_ptr<Content> > mPost;

    //! \var mQueueMutex
    //! \brief Mutex for thread safe working with messages queue
    boost::mutex mQueueMutex;

    //! \var mSubscriberMutex
    //! \brief Mutex for thread safe working with subscription list
    boost::mutex mSubscriberMutex;

    //! \var mSubscribers
    //! \brief subscription list
    std::map<Mailbox, NotifyList> mSubscribers;

    //! \var mSelf
    //! \brief pointer to myself
    static boost::shared_ptr<Postoffice> mSelf;
};

