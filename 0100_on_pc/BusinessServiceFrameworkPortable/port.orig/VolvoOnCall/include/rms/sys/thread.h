////////////////////////////////////////////////////////////////////////////////
//! \file       Thread.h
//! \author     Mikhail Maslyukov
//! \brief      header for wrapper class over boost thread
//! \date       02/17/17
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <boost/thread.hpp>
#include <boost/smart_ptr.hpp>

//! \class Thread
//! \brief wrapper class over boost thread
class Thread
{

public:

    Thread();
    virtual ~Thread();

    void stop();

    void start();

    bool isRunning() const;

protected:

    //! \fn Thread::run()
    //! \brief virtual method. Should implement thread functionality
    //! \return void
    virtual void run() = 0;

    virtual void onStart();

    virtual void onStop();

    virtual void vstop();

    virtual void vstart();

private:
    Thread(Thread&);
    Thread& operator=(Thread&);

    static void _run(Thread* obj);

protected:
    //! \var mProccessing
    //! \brief using to check control thread running
    bool mProccessing;
    //! \var mThread
    //! \brief boost library thread
    boost::shared_ptr<boost::thread> mThread;
};
