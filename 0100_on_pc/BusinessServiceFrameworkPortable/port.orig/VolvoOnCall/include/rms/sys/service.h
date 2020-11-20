///////////////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  @file service.h
//  This file handles the communication with the VuC, using the OTP VCS-interface
 
// @project     GLY_TCAM
// @subsystem   VolvoOnCall
// @author      Shen Jiaojiao
// @Init date   21-Sep-2018
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <rms/sys/thread.h>
#include <rms/sys/postoffice.h>

class TimerProp;

/*! \class Service
    \brief Base class which provides functionality for messages exchange between threads.

    Implements as a separate thread
*/
class Service : public Thread {
    Service();
public:

    Service(const Mailbox& mbox);
    virtual ~Service();
protected:

    template <typename Event, typename Object, typename Func>
    class EventHandler {
    public:
        EventHandler(const Event& ev, Object& obj,const Func& func) :
                mEv(ev), mObj(obj), mFunc(func) {
        }
        virtual ~EventHandler(){}

        template<typename Payload>
        void handle(Event event, const Payload& p) {
            if (event == mEv) {
                (mObj.*mFunc)(p);
            }
        }
    private:
        Event mEv;
        Object& mObj;
        Func mFunc;
    };

    void startTimer(const TimerProp& t) const;

    void stopTimer(const TimerProp& t) const;

    virtual void onStart();

    virtual void onStop();

    void subscribe(const boost::shared_ptr<Event>& event) const;

    bool getMessage(boost::shared_ptr<Notify>& notify) const;
#if BOOST_VERSION >= 105000
    bool getMessage(boost::shared_ptr<Notify>& notify,
            const boost::chrono::milliseconds& ms) const;
#endif

    bool sendMessage(const Mailbox& mbox,
            const boost::shared_ptr<Message>& msg) const;

    bool sendNotify(const Mailbox& mbox,
            const boost::shared_ptr<Notify>& msg) const;

    bool sendNotify(const boost::shared_ptr<Notify>& event) const;

    bool sendEvent(const boost::shared_ptr<Event>& event) const;

    const Mailbox getMailbox() const;

    //! \fn Service::handle(const boost::shared_ptr<Notify>& notify)
    //! \brief virtual method. Should be implemented handling of incoming messages
    //! \param[in] notify Incoming message
    //! \return void
    virtual void handle(const boost::shared_ptr<Notify>& notify) = 0;
private:

    void run();
protected:
    //! \var mMBox
    //! \brief MailBox of the tread.
    const Mailbox mMBox;
};


