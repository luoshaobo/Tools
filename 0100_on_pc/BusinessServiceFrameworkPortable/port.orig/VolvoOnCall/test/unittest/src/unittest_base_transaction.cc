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
 *  \file     unittest_base_transaction.cc
 *  \brief    Volvo On Call, unit tests, base transaction
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup TestVolvoOnCall
 *  \{
 */


/******************************************************************************
 * INCLUDE FILES
 ******************************************************************************/

#include <algorithm>
#include <chrono>
#include <mutex>
#include <random>
#include <thread>
#include <vector>

#include <gtest/gtest.h>

#include "voc_framework/transactions/transaction.h"
#include "voc_framework/transactions/transaction_id.h"
#include "voc_framework/signals/signal.h"

namespace voc_transaction_test
{

class TestSignal : public fsm::Signal
{
public:

    enum TestContent
    {
        kTestContentNone,
        kTestContentStop,
        kTestContentSleepLong,
        kTestContentSleepShortRandom
    };

    TestSignal (fsm::TransactionId& transaction_id) : fsm::Signal::Signal(transaction_id, kTypeUndefined) {}

    TestContent content = kTestContentNone;

    std::string ToString() { return "TestSignal"; };
};

class TestTransaction : public fsm::Transaction
{
public:

    static const unsigned char kVerificationPolicyAll = 0x00;
    static const unsigned char kVerificationPolicyNone = 0x01;
    static const unsigned char kVerificationPolicyInOrder = 0x02;

    std::vector<std::shared_ptr<fsm::Signal>> received_signals;
    std::vector<std::shared_ptr<fsm::TransactionId>> accepted_transaction_ids;

    enum AcceptancePolicy
    {
        kAcceptancePolicyAll,
        kAcceptancePolicyNone,
        kAcceptancePolicySelective
    };

    AcceptancePolicy acceptance_policy = kAcceptancePolicyAll;

    TestTransaction () :
        Transaction ()
    {
    }

    bool WantsSignal (std::shared_ptr<fsm::Signal> signal)
    {

        bool wants = false;

        if (acceptance_policy == kAcceptancePolicyAll)
            wants = true;
        else if (acceptance_policy == kAcceptancePolicyNone)
            wants = false;
        else if (acceptance_policy == kAcceptancePolicySelective)
        {
            const fsm::TransactionId& signal_id = signal->GetTransactionId();

            if (std::find_if(accepted_transaction_ids.begin(),
                             accepted_transaction_ids.end(),
                             [&](std::shared_ptr<fsm::TransactionId> const& accepted_id) {
                                 return *accepted_id == signal_id;})
                != accepted_transaction_ids.end())
                wants = true;
            else
                wants = false;
        }
        else
        {
            // not right, not right at all.
            wants = false;
        }

        return wants;
    }

    bool HandleSignal (std::shared_ptr<fsm::Signal> signal)
    {
        received_signals.push_back(signal);

        std::shared_ptr<TestSignal> test_signal = std::static_pointer_cast<TestSignal>(signal);

        if (test_signal->content == TestSignal::TestContent::kTestContentStop)
        {
            return false;
        }
        else if (test_signal->content == TestSignal::TestContent::kTestContentSleepLong)
        {
            std::chrono::milliseconds sleep_time(1000);
            std::this_thread::sleep_for(sleep_time);
        }
        else if (test_signal->content == TestSignal::TestContent::kTestContentSleepShortRandom)
        {
            std::default_random_engine generator;
            std::uniform_int_distribution<int> distribution(1,50);
            std::chrono::milliseconds sleep_time(distribution(generator));
            std::this_thread::sleep_for(sleep_time);
        }

        return true;
    }

    /**
     * Set policy controlling which transaction ids the
     * transaction will accept
     */
    void SetAcceptancePolicy (AcceptancePolicy policy)
    {
        this->acceptance_policy=policy;
    }

    /**
     * Add a transactionId which the transaction should accept.
     * Valid when using policy kAcceptancePolicySelective.
     */
    void AddAcceptedTransactionId (std::shared_ptr<fsm::TransactionId> id)
    {
        accepted_transaction_ids.push_back(id);
    }

    /**
     * Check that the transaction recevied signals as expected
     */
    bool IsAsExpected (std::vector<std::shared_ptr<fsm::Signal>> signals, const char policy)
    {

        // TODO: these are all a bit dodgy with regards to duplicate signals and such
        //       for now all tests are written such that its not an issue

        bool is_as_expected = true;

        if (policy == kVerificationPolicyAll)
        {
            if (signals.size() != received_signals.size())
            {
                is_as_expected = false;
            }

            for (std::shared_ptr<fsm::Signal> signal:signals)
            {
                if (std::find(received_signals.begin(), received_signals.end(), signal)
                    == received_signals.end())
                {
                    is_as_expected = false;
                    break;
                }
            }

        }
        else if (policy == kVerificationPolicyNone)
        {
            for (std::shared_ptr<fsm::Signal> signal:signals)
            {
                if (std::find(received_signals.begin(), received_signals.end(), signal)
                    != received_signals.end())
                {
                    is_as_expected = false;
                    break;
                }
            }
        }
        else if (policy == kVerificationPolicyInOrder)
        {
            if (signals.size() == received_signals.size())
            {
                for (int i = 0; i < signals.size(); i++)
                {
                    if (signals[i] != received_signals[i])
                    {
                        is_as_expected = false;
                        break;
                    }
                }
            } else {
                is_as_expected = false;
            }
        }
        else
        {
            // so strange
            is_as_expected = false;
        }

        return is_as_expected;
    }

    void reset()
    {
        char acceptance_policy = kAcceptancePolicyAll;
        accepted_transaction_ids.clear();
        received_signals.clear();
    }

    //expose session handling methods
    void MapSessionId(std::string session_id)
    {
       Transaction::MapSessionId(session_id);
    }

    bool IsMappedSessionId(const std::string session_id)
    {
       return Transaction::IsMappedSessionId(session_id);
    }

    bool RemoveMappedSessions()
    {
       Transaction::RemoveMappedSessions();
    }

    ~TestTransaction()
    {
        Terminate();
    }
};


void MakeStopSignal(std::shared_ptr<fsm::Signal> signal)
{
    std::shared_ptr<TestSignal> test_signal = std::static_pointer_cast<TestSignal>(signal);
    test_signal->content = TestSignal::TestContent::kTestContentStop;
}

void MakeLongSleepSignal(std::shared_ptr<fsm::Signal> signal)
{
    std::shared_ptr<TestSignal> test_signal = std::static_pointer_cast<TestSignal>(signal);
    test_signal->content = TestSignal::TestContent::kTestContentSleepLong;
}

void MakeShorRandomSleepSignal(std::shared_ptr<fsm::Signal> signal)
{
    std::shared_ptr<TestSignal> test_signal = std::static_pointer_cast<TestSignal>(signal);
    test_signal->content = TestSignal::TestContent::kTestContentSleepShortRandom;
}


/**
 * Enqueue a sequance of signals. Intended to be used for threaded tests.
 * Safe to call from several threads.
 * Usage example:
 *               num_singals = 4, id_start_offset = 2, is_increment = 2
 *               intial_sleep = 100ms, sleep_time 100ms
 *               => 2@100ms, 4@200ms, 6@300ms, 8@400ms
 * \param[in] num_singals number of signals to enqueue
 * \param [in] id_start_offset, initial value of the signal transaction id
 * \param [in] id_increment, amount to increase the signal transaction id
 * \param [in] initial_sleep time to sleep before starting to enqueue signals in milliseconds
 * \param [in] sleep_time time to sleep between enqueueing each signal in the sequence in milliseconds
 * \param [in] random_sleep_signal whether singal should have content type kTestContentSleepShortRandom
 * \param [in] enqueued_signals vector into which enqueued singals will be stored
 * \param [in] enqueued_signals_mutex mutex used to protect signal insertion into enqueued_signals
 * \param [in] transaction transaction to enqueue singals in
 * \return None
 */
void EnqueueSignal (int num_signals, int id_start_offset, int id_increment,
                    int initial_sleep, int sleep_time,
                    bool random_sleep_signal,
                    std::vector<std::shared_ptr<fsm::Signal>>& enqueued_signals, std::mutex& enqueued_signals_mutex,
                    TestTransaction& transaction)
{

    std::chrono::milliseconds my_initial_sleep(initial_sleep);
    std::chrono::milliseconds my_sleep_time(sleep_time);

    std::this_thread::sleep_for(my_initial_sleep);

    for (int i = 0; i < num_signals; i++)
    {
        fsm::IpCommandBrokerTransactionId transaction_id(id_start_offset + (id_increment*i));
        std::shared_ptr<fsm::Signal> signal = std::make_shared<TestSignal>(transaction_id);

        if (random_sleep_signal)
        {
            MakeShorRandomSleepSignal(signal);
        }

        // protect access to signal vector
        {
            std::lock_guard<std::mutex> lock(enqueued_signals_mutex);
            enqueued_signals.push_back(signal);
        }

        transaction.EnqueueSignal(signal);

        std::this_thread::sleep_for(my_sleep_time);
    }

}


/******************************************************************************
 * TESTS
 ******************************************************************************/
/**
 * Verify that a enqueued signal is delivered
 * when transactions wants it.
 *
 * TEST PASSES:
 *      Enqueued signals are handled in order.
 * TEST FAILS:
 *      Enqueued signals are not handled in order.
 */
TEST (BaseTransactionTest, EnqueueWantedSignal)
{

    std::chrono::milliseconds sleep_time(50);

    fsm::IpCommandBrokerTransactionId initial_transaction_id(1);

    TestTransaction transaction;

    transaction.SetAcceptancePolicy(TestTransaction::AcceptancePolicy::kAcceptancePolicyAll);

    fsm::IpCommandBrokerTransactionId transaction_id(1);
    std::shared_ptr<fsm::TransactionId> transaction_id_wanted = std::make_shared<fsm::IpCommandBrokerTransactionId>(1);
    std::shared_ptr<fsm::Signal> test_signal = std::make_shared<TestSignal>(transaction_id);

    std::vector<std::shared_ptr<fsm::Signal>> signals;
    signals.push_back(test_signal);

    transaction.EnqueueSignal(test_signal);

    std::this_thread::sleep_for(sleep_time);  // ensure transaction has time to handle signals

    EXPECT_TRUE(transaction.IsAsExpected(signals, TestTransaction::kVerificationPolicyAll));

    transaction.reset();

    transaction.SetAcceptancePolicy(TestTransaction::AcceptancePolicy::kAcceptancePolicySelective);
    transaction.AddAcceptedTransactionId(transaction_id_wanted);

    MakeStopSignal(test_signal);

    transaction.EnqueueSignal(test_signal);

    std::this_thread::sleep_for(sleep_time);  // ensure transaction has time to handle signals

    EXPECT_TRUE(transaction.IsAsExpected(signals, TestTransaction::kVerificationPolicyAll));

}

/**
 * Verify that a enqueued signal is not delivered
 * when transactions does not want it.
 *
 * TEST PASSES:
 *      Enqueued signals are handled in order.
 * TEST FAILS:
 *      Enqueued signals are not handled in order.
 */
TEST (BaseTransactionTest, EnqueueUnWantedSignal)
{

    std::chrono::milliseconds sleep_time(50);

    fsm::IpCommandBrokerTransactionId initial_transaction_id(1);
    fsm::IpCommandBrokerTransactionId transaction_id(1);
    std::shared_ptr<fsm::TransactionId> transaction_id_unwanted = std::make_shared<fsm::IpCommandBrokerTransactionId>(2);

    TestTransaction transaction;

    transaction.SetAcceptancePolicy(TestTransaction::AcceptancePolicy::kAcceptancePolicyNone);

    std::shared_ptr<fsm::Signal> test_signal = std::make_shared<TestSignal>(transaction_id);

    std::vector<std::shared_ptr<fsm::Signal>> signals;
    signals.push_back(test_signal);

    transaction.EnqueueSignal(test_signal);

    std::this_thread::sleep_for(sleep_time);  // ensure transaction has time to handle signals

    EXPECT_TRUE(transaction.IsAsExpected(signals, TestTransaction::kVerificationPolicyNone));

    transaction.reset();

    transaction.SetAcceptancePolicy(TestTransaction::AcceptancePolicy::kAcceptancePolicySelective);
    transaction.AddAcceptedTransactionId(transaction_id_unwanted);

    MakeStopSignal(test_signal);

    transaction.EnqueueSignal(test_signal);

    std::this_thread::sleep_for(sleep_time);  // ensure transaction has time to handle signals

    EXPECT_TRUE(transaction.IsAsExpected(signals, TestTransaction::kVerificationPolicyNone));

}

/**
 * Verify that a sequence of enqueued signals
 * are handled in order.
 *
 * TEST PASSES:
 *      Enqueued signals are handled in order.
 * TEST FAILS:
 *      Enqueued signals are not handled in order.
 */
TEST (BaseTransactionTest, EnqueueSequenceOfSignals)
{

    std::chrono::milliseconds sleep_time(50);

    const int kNumIterations = 100;

    fsm::IpCommandBrokerTransactionId initial_transaction_id(1);

    TestTransaction transaction;

    transaction.SetAcceptancePolicy(TestTransaction::AcceptancePolicy::kAcceptancePolicyAll);

    std::vector<std::shared_ptr<fsm::Signal>> signals;

    for (int i = 1; i <= kNumIterations; i++)
    {
        fsm::IpCommandBrokerTransactionId transaction_id(i);
        std::shared_ptr<fsm::Signal> signal = std::make_shared<TestSignal>(transaction_id);

        if (i == kNumIterations)
        {
            MakeStopSignal(signal);
        }

        signals.push_back(signal);
        transaction.EnqueueSignal(signal);
    }

    std::this_thread::sleep_for(sleep_time);  // ensure transaction has time to handle signals

    EXPECT_TRUE(transaction.IsAsExpected(signals, TestTransaction::kVerificationPolicyInOrder));

}

/**
 * Verify that a transactions state is as expected after
 * transaction completes.
 *
 * Transaction::IsActive returns true before and false after
 * derived transaction returns false from HandleSignal.
 *
 * Transaction::IsCompleted returns false before and true after
 * derived transaction returns false from HandleSignal.
 *
 * TEST PASSES:
 *      Transaction state is as expected
 * TEST FAILS:
 *      Transaction state is not as expected
 */
TEST (BaseTransactionTest, CompleteTransaction)
{
    std::chrono::milliseconds sleep_time(50);

    const int kNumIterations = 10;

    fsm::IpCommandBrokerTransactionId initial_transaction_id(1);

    TestTransaction transaction;

    transaction.SetAcceptancePolicy(TestTransaction::AcceptancePolicy::kAcceptancePolicyAll);

    EXPECT_TRUE(transaction.IsActive());
    EXPECT_FALSE(transaction.HasCompleted());

    for (int i = 1; i <= kNumIterations; i++)
    {
        fsm::IpCommandBrokerTransactionId transaction_id(i);
        std::shared_ptr<fsm::Signal> signal = std::make_shared<TestSignal>(transaction_id);

        if (i == kNumIterations)
        {
            MakeStopSignal(signal);
        }

        transaction.EnqueueSignal(signal);
    }

    transaction.Stop();

    std::this_thread::sleep_for(sleep_time);  // ensure transaction has time to handle signals

    EXPECT_FALSE(transaction.IsActive());
    EXPECT_TRUE(transaction.HasCompleted());
}

/**
 * Verify that a transactions state is as expected after stop.
 *
 * Transaction::IsActive returns true before and false after
 * Transaction::Stop is called.
 *
 * Transaction::HasCompleted returns false before and true after
 * Transaction::Stop is called.
 *
 * TEST PASSES:
 *      Transaction state is as expected
 * TEST FAILS:
 *      Transaction state is not as expected
 */
TEST (BaseTransactionTest, StopTransaction)
{
    std::chrono::milliseconds sleep_time(50);

    const int kNumIterations = 10;

    fsm::IpCommandBrokerTransactionId initial_transaction_id(1);

    TestTransaction transaction;

    transaction.SetAcceptancePolicy(TestTransaction::AcceptancePolicy::kAcceptancePolicyAll);

    EXPECT_TRUE(transaction.IsActive());
    EXPECT_FALSE(transaction.HasCompleted());

    for (int i = 1; i <= kNumIterations; i++)
    {
        fsm::IpCommandBrokerTransactionId transaction_id(i);
        std::shared_ptr<fsm::Signal> signal = std::make_shared<TestSignal>(transaction_id);

        transaction.EnqueueSignal(signal);
    }

    transaction.Stop();

    std::this_thread::sleep_for(sleep_time);  // ensure transaction has time to handle signals

    EXPECT_FALSE(transaction.IsActive());
    EXPECT_TRUE(transaction.HasCompleted());
}

/**
 * Verify that a transaction stops handling signals
 * after Transaction::Stop is called.
 *
 * TEST PASSES:
 *      Enqueued signals are handled in order.
 * TEST FAILS:
 *      Enqueued signals are not handled in order.
 */
TEST (BaseTransactionTest, EnqueueAfterStop)
{

    std::chrono::milliseconds sleep_time(50);

    const int kNumIterations = 10;

    fsm::IpCommandBrokerTransactionId initial_transaction_id(1);

    TestTransaction transaction;

    transaction.SetAcceptancePolicy(TestTransaction::AcceptancePolicy::kAcceptancePolicyAll);

    std::vector<std::shared_ptr<fsm::Signal>> signals;

    for (int i = 1; i <= kNumIterations; i++)
    {
        fsm::IpCommandBrokerTransactionId transaction_id(i);
        std::shared_ptr<fsm::Signal> signal = std::make_shared<TestSignal>(transaction_id);

        signals.push_back(signal);
        transaction.EnqueueSignal(signal);
    }

    std::this_thread::sleep_for(sleep_time);  // ensure transaction has time to handle signals

    EXPECT_TRUE(transaction.IsAsExpected(signals, TestTransaction::kVerificationPolicyInOrder));

    transaction.Stop(); //no more signals should be accepted


    for (int i = 1; i <= kNumIterations; i++)
    {
        fsm::IpCommandBrokerTransactionId transaction_id(i);
        std::shared_ptr<fsm::Signal> signal = std::make_shared<TestSignal>(transaction_id);

        signals.push_back(signal);
        transaction.EnqueueSignal(signal);
    }

    EXPECT_FALSE(transaction.IsAsExpected(signals, TestTransaction::kVerificationPolicyInOrder));

}

/**
 * Verify that signals enqueued from different
 * threads are handled.
 *
 * TEST PASSES:
 *      Enqueued signals are handled.
 * TEST FAILS:
 *      Enqueued signals are not handled.
 */
TEST (BaseTransactionTest, EnqueueConcurrently)
{
    std::chrono::milliseconds sleep_time(50);

    fsm::IpCommandBrokerTransactionId initial_transaction_id(1);
    TestTransaction transaction;
    transaction.SetAcceptancePolicy(TestTransaction::AcceptancePolicy::kAcceptancePolicyAll);

    std::vector<std::shared_ptr<fsm::Signal>> signals;
    std::mutex signal_mutex;

    std::thread thread_one(EnqueueSignal, 10, 1, 3, 10, 30, false, std::ref(signals), std::ref(signal_mutex), std::ref(transaction));
    std::thread thread_two(EnqueueSignal, 10, 2, 3, 20, 30, false, std::ref(signals), std::ref(signal_mutex), std::ref(transaction));
    std::thread thread_three(EnqueueSignal, 10, 3, 3, 20, 30, false, std::ref(signals), std::ref(signal_mutex), std::ref(transaction));

    thread_one.join();
    thread_two.join();
    thread_three.join();

    fsm::IpCommandBrokerTransactionId transaction_id(1000000);
    std::shared_ptr<fsm::Signal> test_signal = std::make_shared<TestSignal>(transaction_id);
    MakeStopSignal(test_signal);
    signals.push_back(test_signal);
    transaction.EnqueueSignal(test_signal);

    std::this_thread::sleep_for(sleep_time);

    EXPECT_TRUE(transaction.IsAsExpected(signals, TestTransaction::kVerificationPolicyAll));
}

/**
 * Stress the transaction and verify all signals were handled
 * within a certain time interval.
 *
 * TEST PASSES:
 *      Enqueued signals are handled within time interval.
 * TEST FAILS:
 *      Enqueued signals are not handled within time interval.
 */
TEST (BaseTransactionTest, StressEnqueue)
{

    std::chrono::milliseconds sleep_time(100);

    fsm::IpCommandBrokerTransactionId initial_transaction_id(1);
    TestTransaction transaction;
    transaction.SetAcceptancePolicy(TestTransaction::AcceptancePolicy::kAcceptancePolicyAll);

    std::vector<std::shared_ptr<fsm::Signal>> signals;
    std::mutex signal_mutex;
    std::vector<std::shared_ptr<std::thread>> threads;

    for (int i = 1; i < 50; i++)
    {
        std::default_random_engine generator;
        std::uniform_int_distribution<int> distribution(1,50);

        int num_signals = distribution(generator);
        int initial_sleep = distribution(generator);
        int period_sleep = distribution(generator);

        std::shared_ptr<std::thread> thread = std::make_shared<std::thread>(EnqueueSignal, num_signals, i, 50,
                                                                            initial_sleep, period_sleep,
                                                                            false,
                                                                            std::ref(signals),
                                                                            std::ref(signal_mutex),
                                                                            std::ref(transaction));
        threads.push_back(thread);
    }

    for (std::shared_ptr<std::thread> thread:threads)
    {
        thread->join();
    }

    fsm::IpCommandBrokerTransactionId transaction_id(1000000);
    std::shared_ptr<fsm::Signal> test_signal = std::make_shared<TestSignal>(transaction_id);
    MakeStopSignal(test_signal);
    signals.push_back(test_signal);
    transaction.EnqueueSignal(test_signal);

    std::this_thread::sleep_for(sleep_time);

    EXPECT_TRUE(transaction.IsAsExpected(signals, TestTransaction::kVerificationPolicyAll));

}

/**
 * Stress the transaction and verify all signals were handled
 * even if handler was busy.
 *
 * TEST PASSES:
 *      All Enqueued signals are handled.
 * TEST FAILS:
 *      Not all Enqueued signals are handled.
 */
TEST (BaseTransactionTest, StressEnqueueWithBlockingHandler)
{

    std::chrono::milliseconds sleep_time(25000); //10*50*50

    fsm::IpCommandBrokerTransactionId initial_transaction_id(1);
    TestTransaction transaction;
    transaction.SetAcceptancePolicy(TestTransaction::AcceptancePolicy::kAcceptancePolicyAll);

    std::vector<std::shared_ptr<fsm::Signal>> signals;
    std::mutex signal_mutex;
    std::vector<std::shared_ptr<std::thread>> threads;

    for (int i = 1; i < 10; i++)
    {
        std::default_random_engine generator;
        std::uniform_int_distribution<int> distribution(1,50);

        int num_signals = distribution(generator);
        int initial_sleep = distribution(generator);
        int period_sleep = distribution(generator);

        std::shared_ptr<std::thread> thread = std::make_shared<std::thread>(EnqueueSignal, num_signals, i, 10,
                                                                            initial_sleep, period_sleep,
                                                                            true,
                                                                            std::ref(signals),
                                                                            std::ref(signal_mutex),
                                                                            std::ref(transaction));
        threads.push_back(thread);
    }

    for (std::shared_ptr<std::thread> thread:threads)
    {
        thread->join();
    }

    fsm::IpCommandBrokerTransactionId transaction_id(1000000);
    std::shared_ptr<fsm::Signal> test_signal = std::make_shared<TestSignal>(transaction_id);
    MakeStopSignal(test_signal);
    signals.push_back(test_signal);
    transaction.EnqueueSignal(test_signal);

    std::this_thread::sleep_for(sleep_time);

    EXPECT_TRUE(transaction.IsAsExpected(signals, TestTransaction::kVerificationPolicyAll));

}

/**
 * Verify that we can destruct a active transaction.
 *
 * TEST PASSES:
 *      Destructor is run without crash.
 * TEST FAILS:
 *      Test crashes.
 */
TEST (BaseTransactionTest, DestroyActiveTransaction)
{

    {  //we create the transaction in its own scope
        const int kNumIterations = 10;

        fsm::IpCommandBrokerTransactionId initial_transaction_id(1);

        TestTransaction transaction;

        transaction.SetAcceptancePolicy(TestTransaction::AcceptancePolicy::kAcceptancePolicyAll);

        for (int i = 1; i <= kNumIterations; i++)
        {
            fsm::IpCommandBrokerTransactionId transaction_id(i);
            std::shared_ptr<fsm::Signal> signal = std::make_shared<TestSignal>(transaction_id);

            transaction.EnqueueSignal(signal);
        }
    }

    EXPECT_TRUE(true);
}

/**
 * Verify that we can destruct a active transaction
 * which is busy in a handler.
 *
 * TEST PASSES:
 *      Destructor is run without crash.
 * TEST FAILS:
 *      Test crashes.
 */
TEST (BaseTransactionTest, DestroyActiveBlockedTransaction)
{

    {  //we create the transaction in its own scope
        const int kNumIterations = 10;

        fsm::IpCommandBrokerTransactionId initial_transaction_id(1);

        TestTransaction transaction;

        transaction.SetAcceptancePolicy(TestTransaction::AcceptancePolicy::kAcceptancePolicyAll);

        for (int i = 1; i <= kNumIterations; i++)
        {
            fsm::IpCommandBrokerTransactionId transaction_id(i);
            std::shared_ptr<fsm::Signal> signal = std::make_shared<TestSignal>(transaction_id);

            MakeLongSleepSignal(signal);

            transaction.EnqueueSignal(signal);
        }
    }

    EXPECT_TRUE(true);
}

/**
 * Verify that we handle session mapping correctly in a transaction.
 *
 * TEST PASSES:
 *      Destructor is run without crash.
 * TEST FAILS:
 *      Test crashes.
 */
TEST (BaseTransactionTest, TestMappingSessionId)
{

    {   std::string session_id = "some_session_id";
        std::string session_id2 = "some_session_id2";

        TestTransaction transaction;

        EXPECT_FALSE(transaction.IsMappedSessionId(session_id));

        transaction.MapSessionId(session_id);
        EXPECT_TRUE(transaction.IsMappedSessionId(session_id));
        EXPECT_FALSE(transaction.IsMappedSessionId("some_session_id3"));
        EXPECT_FALSE(transaction.IsMappedSessionId("some_session_i"));
        EXPECT_FALSE(transaction.IsMappedSessionId("other sessions id"));

        transaction.MapSessionId(session_id2);
        EXPECT_TRUE(transaction.IsMappedSessionId(session_id));
        EXPECT_TRUE(transaction.IsMappedSessionId(session_id2));
        EXPECT_FALSE(transaction.IsMappedSessionId("some_session_id3"));

        transaction.RemoveMappedSessions();
        EXPECT_FALSE(transaction.IsMappedSessionId(session_id));
        EXPECT_FALSE(transaction.IsMappedSessionId(session_id2));



    }

    EXPECT_TRUE(true);
}

/** \}    end of addtogroup */

} //namespace
