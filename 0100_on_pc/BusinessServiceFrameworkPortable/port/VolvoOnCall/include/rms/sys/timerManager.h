///////////////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  @file timerManager.h
//  This file handles the communication with the VuC, using the OTP VCS-interface
 
// @project     GLY_TCAM
// @subsystem   VolvoOnCall
// @author      Shen Jiaojiao
// @Init date   21-Sep-2018
/////////////////////////////////////////////////////////////////////////////

#pragma once


#include <boost/thread.hpp>
#include <boost/chrono.hpp>

#include <rms/sys/service.h>

/*!
  \struct TimerProp
  \breif store timer properties: ID, timeout, cyclic property
 */
struct TimerProp{
    TimerProp(const TimerProp& tp):
        id(tp.id), timeout(tp.timeout), cyclic(tp.cyclic) {
    }
    TimerProp(const uint32_t id):
        id(id), timeout(0), cyclic(false) {
    }
    TimerProp(const uint32_t id, const uint32_t timeout) :
            id(id), timeout(timeout), cyclic(false) {
    }
    TimerProp(uint32_t id, uint32_t timeout, bool cyclic) :
            id(id), timeout(timeout), cyclic(cyclic) {
    }
    bool operator ==(const TimerProp& a) const {
        return id == a.id;
    }
    TimerProp& operator =(const TimerProp& a) {
        this->cyclic = a.cyclic;
        this->timeout = a.timeout;
        return *this;
    }
    bool operator <(const TimerProp& a) const {
        return id < a.id;
    }
    const uint32_t id;
    mutable uint32_t timeout;
    bool cyclic;
};
/*!
  \class TimerManager
  \brief Class which manage timers
 */
class TimerManager {

    /*!
      \class TimerThread
      \brief tread which process timer and sends event when timer expired
      */
    class TimerThread : public Thread {
    public:

        TimerThread(TimerManager& parent, const Mailbox& mb, const TimerProp& timerProp);
        virtual ~TimerThread();

        TimerProp& getTimerProp();

        void setTimerProp(const TimerProp& timerProp);

        bool isActive() const;

    private:
        virtual void vstop();
        virtual void vstart();
        virtual void onStop();
        virtual void run();
    private:
        TimerManager& mParent;
        TimerProp mTimerProp;
        Mailbox mClientMBox;
        boost::mutex mMutex;
        boost::condition_variable mCv;
        bool mActive;
#if BOOST_VERSION >= 105000
        boost::cv_status mStatus;
#else
        bool mStopRequested;
#endif
    };

    typedef std::map<
            Mailbox, std::list<boost::shared_ptr<TimerThread> >
    > TimerClientCollection;

public:
    TimerManager() {}
    virtual ~TimerManager() {}

    bool isActive(const Mailbox& mbox, const TimerProp& timerProp);

    void startTimer(const Mailbox& mbox, const TimerProp& timerProp);

    void stopTimer(const Mailbox& mbox, const TimerProp& timerProp);

    static TimerManager* getInstance();

private:

    void stoppedTimer(const Mailbox& mbox, const TimerProp& timerProp);

private:
    boost::mutex mTimerAccessMutex;
    TimerClientCollection mTimerThreadStorage;
    std::list<boost::shared_ptr<TimerThread> > mTimerThreadTrash;
};

