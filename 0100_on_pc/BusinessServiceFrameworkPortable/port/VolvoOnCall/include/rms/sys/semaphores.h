///////////////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  @file Semaphores.h
//  This file handles the communication with the VuC, using the OTP VCS-interface
 
// @project     GLY_TCAM
// @subsystem   VolvoOnCall
// @author      Shen Jiaojiao
// @Init date   21-Sep-2018
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <boost/thread.hpp>

//! \class Semaphores
//! \brief Implementation of Semaphores based on mutexes and condition variables to manage reading and writing
//!        from multiple threads
class Semaphores {
private:

    //! \class Pred
    //! \brief Predicate class to use with condition variable
    class Pred {
    public:
        //! \fn Pred(int32_t& locks, int32_t unblockingThreshold)
        //! \brief Constructor of Pred objects
        //! \param[in] locks reference to variable with current number of locks
        //! \param[in] unblockingThreshold Threshold number of locks above which waiting thread is unblocked
        //! \return void
        Pred(int32_t& locks, int32_t unblockingThreshold):
            mLocks(locks), mUnblockingThreshold(unblockingThreshold) {
        }

        //! \fn operator()()
        //! \brief Function call operator implementation. Will be called when thread is waiting on condition variable
        //! \return true if number of locks exceeds threshold
        bool operator()() {
            return (mUnblockingThreshold < mLocks);
        }
    private:
        //! \var mLocks
        //! \brief reference to variable with current number of locks
        int32_t& mLocks;

        //! \var mUnblockingThreshold
        //! \brief Threshold number of locks above which waiting thread is unblocked
        int32_t mUnblockingThreshold;
    };

public:

    Semaphores(const int32_t locks);
    ~Semaphores();

    void lockRead();
    void unlockRead();
    void lockWrite();
    void unlockWrite();

private:
    //! \var mLocks
    //! \brief variable with current number of locks
    int32_t mLocks;

    //! \const mLocksMax
    //! \brief maximum number of locks or concurrent readers
    const int32_t mLocksMax;

    //! \var mMutex
    //! \brief mutex to use with condition variable and control access to lock counter
    boost::mutex mMutex;

    //! \var mCv
    //! \brief condition variable
    boost::condition_variable mCv;
};
