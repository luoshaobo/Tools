////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  @file timerManager.h
//  This file handles the communication with the VuC, using the OTP VCS-interface
 
// @project     GLY_TCAM
// @subsystem   VolvoOnCall
// @author      Jiaojiao Shen
// @Init date   12-Sep-2018
////////////////////////////////////////////////////////////////////////////

#include <ctime>
#include <iostream>
#include <unistd.h>

#include <rms/util/utils.h>
#include <rms/sys/timerManager.h>
#include "dlt/dlt.h"

DLT_IMPORT_CONTEXT(dlt_voc);

#define STR(x) #x

using namespace std;

///////////////////////////////////////////////////////////////////////////////
//! \fn TimerManager::TimerThread::TimerThread(TimerManager& parent, const Mailbox& mb, const TimerProp& timerProp)
//! \brief Constructor for timer thread
//! \param[in] parent TimerManager of the timer
//! \param[in] mb Mailbox of the timer expir=erd event,
//! \param[in] timerProp Timer property
//! \return void
///////////////////////////////////////////////////////////////////////////////
TimerManager::TimerThread::TimerThread(TimerManager& parent, const Mailbox& mb,
        const TimerProp& timerProp) :
        mParent(parent), mTimerProp(timerProp), mClientMBox(mb), mActive(true) {
#if BOOST_VERSION >= 105000
    mStatus = boost::cv_status::no_timeout;
#else
    mStopRequested = false;
#endif
}
///////////////////////////////////////////////////////////////////////////////
//! \fn TimerManager::TimerThread::~TimerThread()
//! \brief destructor for timer thread
//! \return void
///////////////////////////////////////////////////////////////////////////////
TimerManager::TimerThread::~TimerThread() {
}

///////////////////////////////////////////////////////////////////////////////
//! \fn TimerManager::TimerThread::vstop()
//! \brief performs thread initialization when it was started
//! \return void
///////////////////////////////////////////////////////////////////////////////
void TimerManager::TimerThread::vstop() {
#if BOOST_VERSION >= 105000
    mCv.notify_one();
#else
    mStopRequested = true;
    mThread->interrupt();
#endif
}

///////////////////////////////////////////////////////////////////////////////
//! \fn TimerManager::TimerThread::onStop()
//! \brief performs thread cleanup when it was stopped
//! \return void
///////////////////////////////////////////////////////////////////////////////
void TimerManager::TimerThread::onStop() {
    mParent.stoppedTimer(mClientMBox, mTimerProp);
}

///////////////////////////////////////////////////////////////////////////////
//! \fn TimerManager::TimerThread::vstart()
//! \brief inform when timer thread was started
//! \return void
///////////////////////////////////////////////////////////////////////////////
void TimerManager::TimerThread::vstart()
{
}


///////////////////////////////////////////////////////////////////////////////
//! \fn TimerManager::TimerThread::run()
//! \brief timer logic
//! \return void
///////////////////////////////////////////////////////////////////////////////
void TimerManager::TimerThread::run() {
#if 0
    uint64_t timeout_ms = static_cast<uint64_t>(mTimerProp.timeout * 1000);
    uint64_t timeoutTimestamp_ms = static_cast<uint64_t>(
            mTimerProp.timeout * 1000);
    uint64_t baseTime = boost::chrono::time_point_cast<boost::chrono::milliseconds>(
                boost::chrono::steady_clock::now()).time_since_epoch().count();
#endif
    do {
        bool flagSendMsg = false;
#if 0
        uint64_t now = boost::chrono::time_point_cast<boost::chrono::milliseconds>(
                    boost::chrono::steady_clock::now()).time_since_epoch().count();
        uint64_t targetTime = baseTime + timeoutTimestamp_ms;
        uint64_t us = (targetTime - now) * 1000;
#endif
        uint64_t us = static_cast<uint64_t>(mTimerProp.timeout * 1000) * 1000;
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: us:%llu", us);
#if BOOST_VERSION >= 105000
        boost::unique_lock<boost::mutex> lock(mMutex);
        mStatus = mCv.wait_for(lock, boost::chrono::microseconds(us));
        flagSendMsg = (mStatus == boost::cv_status::timeout);
#else
        usleep(us);
        flagSendMsg = !mStopRequested;
#endif
        // This line solves issue when MSG_TIMER_EXPIRED was sent, but
        // TimerManager::isActive anyway will tell that timer is active
        mActive = mTimerProp.cyclic;
        //timeoutTimestamp_ms += timeout_ms;
        if (flagSendMsg) {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: Timer expired Mailbox=%d(%Xh); Id=%d(%Xh);",
                    (int)mClientMBox, (int)mClientMBox,
                    (int)mTimerProp.id, (int)mTimerProp.id);
            Postoffice::getInstance()->sendMessage(mClientMBox,
                boost::make_shared<Message>(NotifyId::MSG_TIMER_EXPIRED,
                Payload((uint8_t*)&mTimerProp,
                (uint8_t*)&mTimerProp + sizeof(mTimerProp))));
        }
    } while (mProccessing && mActive);
}

///////////////////////////////////////////////////////////////////////////////
//! \fn TimerManager::TimerThread::getTimerProp()
//! \brief getter for timer property
//! \return timer property
///////////////////////////////////////////////////////////////////////////////
TimerProp& TimerManager::TimerThread::getTimerProp() {
    return mTimerProp;
}

///////////////////////////////////////////////////////////////////////////////
//! \fn TimerManager::TimerThread::getTimerProp()
//! \brief setter for timer property
//! \param[in] timerProp Timer property
//! \return void
///////////////////////////////////////////////////////////////////////////////
void TimerManager::TimerThread::setTimerProp(const TimerProp& timerProp) {
    mTimerProp = timerProp;
}
///////////////////////////////////////////////////////////////////////////////
//!\fn TimerManager::TimerThread::isActive()
//! \brief check is timer active
//! \return true if timer active
///////////////////////////////////////////////////////////////////////////////
bool TimerManager::TimerThread::isActive() const {
    return mActive;
}

///////////////////////////////////////////////////////////////////////////////
//! \fn TimerManager::isActive(const Mailbox& mbox, const TimerProp& timerProp)
//! \brief check is timer active by mailbox and timer property
//! \param[in] mbox Owner Mailbox
//! \param[in] timerProp Timer properties
//! \return true if active
///////////////////////////////////////////////////////////////////////////////
bool TimerManager::isActive(const Mailbox& mbox, const TimerProp& timerProp) {
    bool rc = false;
    boost::lock_guard<boost::mutex> lock(mTimerAccessMutex);
    std::list<boost::shared_ptr<TimerThread> >& timers = mTimerThreadStorage[mbox];
    for (std::list<boost::shared_ptr<TimerThread> >::iterator it = timers.begin();
            it != timers.end(); it++) {
        if ((*it)->getTimerProp().id == timerProp.id) {
            rc = (*it)->isActive();
            break;
        }
    }
    return rc;
}

///////////////////////////////////////////////////////////////////////////////
//! \fn TimerManager::startTimer(const Mailbox& mbox, const TimerProp& timerProp)
//! \brief Start timer
//! \param[in] mbox Owner Mailbox
//! \param[in] timerProp Timer properties
//! \return void
///////////////////////////////////////////////////////////////////////////////
void TimerManager::startTimer(const Mailbox& mbox, const TimerProp& timerProp) {
    boost::lock_guard<boost::mutex> lock(mTimerAccessMutex);
    list<boost::shared_ptr<TimerThread> >& timers = mTimerThreadStorage[mbox];
    boost::shared_ptr<TimerThread> timer;
    std::list<boost::shared_ptr<TimerThread> >::iterator it;
    for (it = timers.begin(); it != timers.end(); it++) {
        if ((*it)->getTimerProp().id == timerProp.id) {
            (*it)->stop();
        }
    }
    // Clear trash
    mTimerThreadTrash.clear();

    timer.reset(new TimerThread(*this, mbox, timerProp));
    timers.push_back(timer);
    timers.back()->start();
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: Start timer Mailbox=%d(%Xh); Id=%d(%Xh); timeout=%d; "
            "timers_qty=%d",
            (int)mbox, (int)mbox, (int)timerProp.id, (int)timerProp.id,
            (int)timerProp.timeout, (int)timers.size());
}

///////////////////////////////////////////////////////////////////////////////
//! \fn TimerManager::stopTimer(const Mailbox& mbox, const TimerProp& timerProp)
//! \brief stop timer
//! \param[in] mbox Owner Mailbox
//! \param[in] timerProp Timer properties
//! \return void
///////////////////////////////////////////////////////////////////////////////
void TimerManager::stopTimer(const Mailbox& mbox, const TimerProp& timerProp) {
    boost::lock_guard<boost::mutex> lock(mTimerAccessMutex);
    std::list<boost::shared_ptr<TimerThread> >& timers = mTimerThreadStorage[mbox];
    for (std::list<boost::shared_ptr<TimerThread> >::iterator it = timers.begin();
            it != timers.end(); it++) {
        if ((*it)->getTimerProp().id == timerProp.id) {
            (*it)->stop();
        }
    }
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: Want to stop timer Mailbox=%d(%Xh); Id=%d(%Xh);"
            " timers_qty=%d",
            (int)mbox, (int)mbox, (int)timerProp.id, (int)timerProp.id,
            (int)timers.size());
}

////////////////////////////////////////////////////////////////////////////////
//! \fn TimerManager::stoppedTimer(const Mailbox& mbox, const TimerProp& timerProp)
//! \brief when timer stopped this function will be coled
//! \param[in] mbox Mailbox of thread where event oboute timer expired will be sent
//! \param[in] timerProp Timer property
//! \return void
////////////////////////////////////////////////////////////////////////////////
void TimerManager::stoppedTimer(const Mailbox& mbox, const TimerProp& timerProp)
{
    boost::lock_guard<boost::mutex> lock(mTimerAccessMutex);
    list<boost::shared_ptr<TimerThread> >& timers = mTimerThreadStorage[mbox];
    for (std::list<boost::shared_ptr<TimerThread> >::iterator it = timers.begin();
            it != timers.end(); it++) {
        if ((*it)->getTimerProp().id == timerProp.id) {
            mTimerThreadTrash.push_back(*it);
            timers.erase(it);
            break;
        }
    }
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: Stopped timer Mailbox=%d(%Xh); Id=%d(%Xh);"
            " timers_qty=%d",
            (int)mbox, (int)mbox, (int)timerProp.id, (int)timerProp.id,
            (int)timers.size());
}

////////////////////////////////////////////////////////////////////////////////
//! \fn TimerManager::getInstance()
//! \brief Provide instance of TimerManager. If not exist create one
//! \return pointer to TimerManager object
////////////////////////////////////////////////////////////////////////////////
TimerManager* TimerManager::getInstance() {
    static TimerManager tm;
    return &tm;
}

