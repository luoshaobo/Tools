/**
 * Copyright (C) 2017 Continental Automotive GmbH
 *
 * Licensed under the "Supplier unrestricted white box" category, as
 * described in the 20160322-1_SW-amendment TCAM.pdf agreement
 * between Volvo Cars and Continental Automotive GmbH.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *  \file     unittest_signal_source.cc
 *  \brief    Volvo On Call, signal_source, Test Cases
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup TestVolvoOnCall
 *  \{
 */

/******************************************************************************
 * INCLUDE FILES
 ******************************************************************************/

#include <gtest/gtest.h>
#include <vector>
#include <mutex>


#include "voc_framework/features/feature.h"
#include "voc_framework/signal_sources/signal_source.h"
#include "voc_framework/signal_sources/timeout_signal_source.h"
#include "voc_framework/signal_sources/timeout_receiver.h"
#include "voc_framework/signals/signal.h"
#include "voc_framework/signals/timeout_signal.h"
#include "voc_framework/transactions/transaction.h"
#include "voc_framework/transactions/transaction_id.h"

namespace voc_signal_source_test
{


/******************************************************************************
 * GLOBALS
 ******************************************************************************/

class TestSignal : public fsm::Signal
{
public:

    TestSignal (fsm::TransactionId& transaction_id) : fsm::Signal::Signal(transaction_id, kTypeUndefined) {}

    std::string ToString() { return "TestSignal"; };
};

class TestTransaction : public fsm::Transaction
{
    std::vector<std::shared_ptr<fsm::TransactionId>> trans_ids_wants;
    std::vector<std::shared_ptr<fsm::TransactionId>> trans_ids_handle;

    //compare internal (shared pointers) and external (transaction ids) vectors
    bool CompareVectors(std::vector<std::shared_ptr<fsm::TransactionId>> internal, std::vector<fsm::TimeoutTransactionId> external)
    {
        bool equal = false;

        if (internal.size()==external.size())
        {
            equal = true;

            for (int i = 0;i<internal.size(); i++)
            {
                if (*internal[i]!=external[i])
                {
                    equal = false;
                }
            }
        }

        return equal;

    }

public:
    //check if we got expected signals in expected order in Wants method
    bool CompareWanted(std::vector<fsm::TimeoutTransactionId> expected)
    {
        return CompareVectors(trans_ids_wants, expected);

    }

    //check if we got expected signals in expected order in Handle method
    bool CompareHandled(std::vector<fsm::TimeoutTransactionId> expected)
    {
        return CompareVectors(trans_ids_handle, expected);

    }

    bool WantsSignal (std::shared_ptr<fsm::Signal> signal)
    {
        //store and confirm
        trans_ids_wants.push_back(signal->GetTransactionId().GetSharedCopy());
        return true;
    }


    bool HandleSignal (std::shared_ptr<fsm::Signal> signal)
    {
        //store and confirm
        trans_ids_handle.push_back(signal->GetTransactionId().GetSharedCopy());
        return true;
    }


    std::string ToString()
    {
        return "TestTransaction";
    }

    fsm::TimeoutTransactionId RequestTimeout (std::chrono::seconds delay)
    {
        //expose superclasses method
       return fsm::TimeoutReceiver::RequestTimeout(delay);
    }

};


class TestFeature : public fsm::Feature
{
public:

    std::vector<std::shared_ptr<fsm::Signal>> received_signals;

    TestFeature() : fsm::Feature("TestFeature")
    {

    }

    void Init(fsm::SignalSource& signal_source)
    {
        signal_source.AddFeature(*this);
    }

    void HandleSignal (std::shared_ptr<fsm::Signal> signal)
    {
        std::lock_guard<std::mutex> lock(signal_list);

        received_signals.push_back(signal);
    }

    std::vector<std::shared_ptr<fsm::Signal>>& GetReceivedSignals ()
    {
        return received_signals;
    }
private:
    std::mutex  signal_list;
};


class TestTimeoutReceiver: public fsm::TimeoutReceiver
{

    std::shared_ptr<fsm::TransactionId> received_id;
    bool timeout_type = true;

    int counter = 0;

public:

    void HandleTimeout (std::shared_ptr<fsm::TimeoutSignal> timeout_signal)
    {
        received_id = timeout_signal->GetTransactionId().GetSharedCopy();

        //make sure that signal  and transaction ids are of timeout types
        timeout_type = timeout_type && (timeout_signal->GetSignalType() == fsm::Signal::kTimeout);
        timeout_type = timeout_type && (received_id->GetType() == fsm::TransactionId::kTransactionIdTypeTimeout);

        //increase the counter
        counter ++;
    }

    fsm::TimeoutTransactionId RequestTimeout (std::chrono::seconds delay, bool periodic = false)
    {
        //forward to the superclass, this is to make this call available from outside
        //in superclass it is protected
        return TimeoutReceiver::RequestTimeout(delay, periodic);
    }

    //checks that we got appropriate transaction id, signal and transaction id type and signal count
    bool CheckCorrectness(fsm::TimeoutTransactionId id, int count)
    {
        return (*received_id==id && counter == count && timeout_type);
    }


};


class TestSignalSource : public fsm::SignalSource {};

/**
 * \brief Broadcasts a signal to a signal source for a configurable number of times
 * \param[in] amount The number of times the signal should be broadcaster
 * \param[in] signal_source The signal source to perform the operation on
 * \param[in] signal The signal to be broadcasted
 * \return None.
 */
void BroadcastSignal (int amount,  TestSignalSource& signal_source, std::shared_ptr<fsm::Signal> signal)
{

    for (int i = 0; i < amount; i++)
    {
        signal_source.BroadcastSignal(signal);
    }

}

/**
 * \brief Creates a list of 10 features that first register to the given signal source and then
 *        are removed from it. This add-remove cycle is repeated for a configurable number of times.
 * \param[in] rounds The number of times the cycle should be repeated
 * \param[in] signal_source The signal source to perform the operations on
 * \return None.
 */
void FlipFeatures (int rounds,  TestSignalSource& signal_source)
{
    TestFeature test_feature_list[10];

    for (int i=0; i<10; i++)
    {
        test_feature_list[i].Init(signal_source);
    }


    for (int j = 0; j < rounds; j++)
    {
        for (int i=0; i<10; i++) signal_source.AddFeature(test_feature_list[i]);
    }

    for (int j = 0; j < rounds; j++)
    {
        for (int i=0; i<10; i++) signal_source.RemoveFeature(test_feature_list[i]);
    }

}

/******************************************************************************
 * TESTS
 ******************************************************************************/

/**
 * Verify that injected signals are delivered to registered Features.
 *
 * TEST PASSES:
 *      Injected signal is delivered to registered feature.
 * TEST FAILS:
 *      Injected signal is not delivered to registered feature.
 */
TEST (SignalSourceTest, InjectSignal)
{
    fsm::IpCommandBrokerTransactionId transaction_id_1(1);
    fsm::IpCommandBrokerTransactionId transaction_id_2(2);
    fsm::IpCommandBrokerTransactionId transaction_id_3(3);

    std::shared_ptr<fsm::Signal> test_signal_1 = std::make_shared<TestSignal>(transaction_id_1);
    std::shared_ptr<fsm::Signal> test_signal_2 = std::make_shared<TestSignal>(transaction_id_2);
    std::shared_ptr<fsm::Signal> test_signal_3 = std::make_shared<TestSignal>(transaction_id_3);

    TestSignalSource test_signal_source;
    fsm::SignalSource& signal_source = test_signal_source;

    TestFeature test_feature;

    test_feature.Init(signal_source);

    signal_source.BroadcastSignal(test_signal_1);
    signal_source.BroadcastSignal(test_signal_2);
    signal_source.BroadcastSignal(test_signal_3);

    std::vector<std::shared_ptr<fsm::Signal>>& received_signals = test_feature.GetReceivedSignals();

    ASSERT_EQ(received_signals.size(), 3);

    const fsm::IpCommandBrokerTransactionId& received_id_1 = static_cast<const fsm::IpCommandBrokerTransactionId&>(received_signals[0]->GetTransactionId());
    const fsm::IpCommandBrokerTransactionId& received_id_2 = static_cast<const fsm::IpCommandBrokerTransactionId&>(received_signals[1]->GetTransactionId());
    const fsm::IpCommandBrokerTransactionId& received_id_3 = static_cast<const fsm::IpCommandBrokerTransactionId&>(received_signals[2]->GetTransactionId());
    EXPECT_EQ(received_id_1, transaction_id_1);
    EXPECT_EQ(received_id_2, transaction_id_2);
    EXPECT_EQ(received_id_3, transaction_id_3);
}

/**
 * Verify that injected signals are delivered to registered fsm::Feature when executed in parallel.
 * At the same time features are added and removed from the SS to detect any concurrency issues.
 *
 * TEST PASSES:
 *      Injected signal is delivered to registered feature.
 * TEST FAILS:
 *      Injected signal is not delivered to registered feature.
 */
TEST (SignalSourceTest, InjectSignalsConcurrently)
{


    TestSignalSource test_signal_source;
    fsm::SignalSource& signal_source = test_signal_source;

    TestFeature test_feature;
    test_feature.Init(signal_source);

    fsm::IpCommandBrokerTransactionId transaction_id_1(1);
    fsm::IpCommandBrokerTransactionId transaction_id_2(2);
    fsm::IpCommandBrokerTransactionId transaction_id_3(3);
    std::shared_ptr<fsm::Signal> test_signal_1 = std::make_shared<TestSignal>(transaction_id_1);
    std::shared_ptr<fsm::Signal> test_signal_2 = std::make_shared<TestSignal>(transaction_id_2);
    std::shared_ptr<fsm::Signal> test_signal_3 = std::make_shared<TestSignal>(transaction_id_3);

    std::thread thread_1(BroadcastSignal, 200, std::ref(test_signal_source), std::ref(test_signal_1));
    std::thread thread_2(FlipFeatures, 190, std::ref(test_signal_source));
    std::thread thread_3(BroadcastSignal, 170, std::ref(test_signal_source), std::ref(test_signal_2));
    std::thread thread_4(FlipFeatures, 150, std::ref(test_signal_source));
    std::thread thread_5(BroadcastSignal, 130, std::ref(test_signal_source), std::ref(test_signal_3));
    std::thread thread_6(FlipFeatures, 120, std::ref(test_signal_source));

    thread_1.join();
    thread_2.join();
    thread_3.join();
    thread_4.join();
    thread_5.join();
    thread_6.join();

    std::vector<std::shared_ptr<fsm::Signal>>& received_signals = test_feature.GetReceivedSignals();

    ASSERT_EQ(received_signals.size(), 500);
}


/**
 * Request a timeout, make sure we get it and it has right values
 *
 * TEST PASSES:
 *      Requested timeout correct arrived
 * TEST FAILS:
 *      Requested timeout not correct or not arrived
 */
TEST (SignalSourceTest, TestTimeoutReceiverInterfaceOneshot)
{
    std::shared_ptr<TestTimeoutReceiver> timeout_receiver = std::make_shared<TestTimeoutReceiver>();
    std::chrono::seconds offset(1);

    fsm::TimeoutTransactionId transaction_id1 = timeout_receiver->RequestTimeout(offset);

    sleep(2);
    ASSERT_TRUE(timeout_receiver->CheckCorrectness(transaction_id1, 1));
}

/**
 * Request a number of timeouts using signal source interface, check if they arrive in the right order
 *
 * TEST PASSES:
 *      Requested timeouts arrive in the right order and right time
 * TEST FAILS:
 *      Requested timeouts do not arrive in correct order and time.
 */
TEST (SignalSourceTest, TestTimeoutReceiverInterfacePeriodic)
{
    std::shared_ptr<TestTimeoutReceiver> timeout_receiver = std::make_shared<TestTimeoutReceiver>();
    std::chrono::seconds offset(2);

    fsm::TimeoutTransactionId transaction_id1 = timeout_receiver->RequestTimeout(offset, true);

    sleep(5); //2 timeouts should make it
    ASSERT_TRUE(timeout_receiver->CheckCorrectness(transaction_id1, 2));
}


//Request a number of timeouts and check if they arrive to the transaction in the correct order
//directly steers if timeout signal source interface (true) or transaction interface (false) is used
void TestTimeout(bool directly)
{

    std::shared_ptr<TestTransaction> trans = std::make_shared<TestTransaction>();
    std::chrono::seconds offset1(1);
    std::chrono::seconds offset2(2);
    std::chrono::seconds offset3(5);
    std::chrono::seconds offset4(6);


    fsm::TimeoutTransactionId transaction_id1;
    fsm::TimeoutTransactionId transaction_id2;
    fsm::TimeoutTransactionId transaction_id3;
    fsm::TimeoutTransactionId transaction_id4;
    fsm::TimeoutTransactionId transaction_id5;

    //access time signal source directly
    if (directly)
    {
        fsm::TimeoutSignalSource& timeout_signal_source = fsm::TimeoutSignalSource::GetInstance();

        transaction_id1 = timeout_signal_source.RequestTimeout(trans,  offset4);
        transaction_id2 = timeout_signal_source.RequestTimeout(trans,  offset1);
        transaction_id3 = timeout_signal_source.RequestTimeout(trans,  offset3);
        transaction_id4 = timeout_signal_source.RequestTimeout(trans,  offset3);
        transaction_id5 = timeout_signal_source.RequestTimeout(trans,  offset2);
    }
    else
    {
        //use the wrapper in transaction
        transaction_id1 = trans->RequestTimeout( offset4);
        transaction_id2 = trans->RequestTimeout( offset1);
        transaction_id3 = trans->RequestTimeout( offset3);
        transaction_id4 = trans->RequestTimeout( offset3);
        transaction_id5 = trans->RequestTimeout( offset2);
    }

    std::vector<fsm::TimeoutTransactionId> early_list;
    std::vector<fsm::TimeoutTransactionId> late_list;

    early_list.push_back(transaction_id2);
    early_list.push_back(transaction_id5); //should arive withing 2-3 seconds

    late_list.push_back(transaction_id2);
    late_list.push_back(transaction_id5);
    late_list.push_back(transaction_id3);
    late_list.push_back(transaction_id4);
    late_list.push_back(transaction_id1);

    sleep(3);
    ASSERT_TRUE(trans->CompareWanted(early_list));

    sleep(5);
    ASSERT_TRUE(trans->CompareWanted(late_list));
}

/**
 * Request a number of timeouts using signal source interface, check if they arrive in the right order
 *
 * TEST PASSES:
 *      Requested timeouts arrive in the right order and right time
 * TEST FAILS:
 *      Requested timeouts do not arrive in correct order and time.
 */
TEST (SignalSourceTest, TestTimeoutDirectly)
{
     TestTimeout(true);
}


/**
 * Request a number of timeouts using transaction interface, check if they arrive in the right order
 *
 * TEST PASSES:
 *      Requested timeouts arrive in the right order and right time
 * TEST FAILS:
 *      Requested timeouts do not arrive in correct order and time.
 */
TEST (SignalSourceTest, TestTimeoutThroughTransaction)
{

     TestTimeout(false);
}


/**
 * Make sure timeout removal through transaction id works
 *
 * TEST PASSES:
 *      No timeout arrives if removed in time
 * TEST FAILS:
 *      Timeout arrives
 */
TEST (SignalSourceTest, TestTimeoutRemoveUsingTransactionId)
{
    std::shared_ptr<TestTransaction> trans = std::make_shared<TestTransaction>();
    std::chrono::seconds offset(2);

    fsm::TimeoutSignalSource& timeout_signal_source = fsm::TimeoutSignalSource::GetInstance();
    fsm::TimeoutTransactionId transaction_id1 = timeout_signal_source.RequestTimeout(trans,  offset);
    timeout_signal_source.RemoveTimeout(transaction_id1);
    sleep(3);

    std::vector<fsm::TimeoutTransactionId> list;

    ASSERT_TRUE(trans->CompareWanted(list));
}

/**
 * Make sure timeout removal through transaction id works
 *
 * TEST PASSES:
 *      No timeout arrives if removed in time
 * TEST FAILS:
 *      Timeout arrives
 */
TEST (SignalSourceTest, TestTimeoutRemoveUsingTransaction)
{
    std::shared_ptr<TestTransaction> trans = std::make_shared<TestTransaction>();
    std::chrono::seconds offset(2);

    fsm::TimeoutSignalSource& timeout_signal_source = fsm::TimeoutSignalSource::GetInstance();
    fsm::TimeoutTransactionId transaction_id1 = timeout_signal_source.RequestTimeout(trans,  offset);
    fsm::TimeoutTransactionId transaction_id2 = timeout_signal_source.RequestTimeout(trans,  offset);
    fsm::TimeoutTransactionId transaction_id3 = timeout_signal_source.RequestTimeout(trans,  offset);

    timeout_signal_source.RemoveTimeouts(trans);
    sleep(3);

    std::vector<fsm::TimeoutTransactionId> list; //the list should be empty, the signal should have been removed

    ASSERT_TRUE(trans->CompareWanted(list));
}

/**
 * Make sure timeout requests are not causing crashes when transaction is deleted
 *
 * TEST PASSES:
 *      No crash
 * TEST FAILS:
 *      crash
 */
TEST (SignalSourceTest, TestTimeoutRemoveForDeletedTransaction)
{
    fsm::TimeoutSignalSource& timeout_signal_source = fsm::TimeoutSignalSource::GetInstance();

    {
        std::shared_ptr<TestTransaction> trans = std::make_shared<TestTransaction>();
        std::chrono::seconds offset(2); //set long offset to manage to delete in time
        fsm::TimeoutTransactionId transaction_id1 = timeout_signal_source.RequestTimeout(trans,  offset, true);
        fsm::TimeoutTransactionId transaction_id2 = timeout_signal_source.RequestTimeout(trans,  offset, true);
        fsm::TimeoutTransactionId transaction_id3 = timeout_signal_source.RequestTimeout(trans,  offset, true);
    }

    sleep(3); //failure if crash here


}


/**
 * Make sure we get periodic timeout
 *
 * TEST PASSES:
 *      Timeout arrived expected number of times
 * TEST FAILS:
 *      Timeout not arrived expected number of times
 */
TEST (SignalSourceTest, TestTimeoutPeriodic)
{
    std::shared_ptr<TestTransaction> trans = std::make_shared<TestTransaction>();
    std::chrono::seconds offset(2);

    fsm::TimeoutSignalSource& timeout_signal_source = fsm::TimeoutSignalSource::GetInstance();
    fsm::TimeoutTransactionId transaction_id = timeout_signal_source.RequestTimeout(trans,  offset, true);

    sleep(5);

    std::vector<fsm::TimeoutTransactionId> list;
    list.push_back(transaction_id);
    list.push_back(transaction_id);

    ASSERT_TRUE(trans->CompareWanted(list));
}

/** \}    end of addtogroup */


/** \}    end of addtogroup */

} // namesapce
