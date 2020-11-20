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
 *  \file     unittest_base_feature.cc
 *  \brief    Volvo On Call, unit tests, base feature
 *  \author   Piotr Tomaszewski
 *
 *  \addtogroup TestVolvoOnCall
 *  \{
 */


/******************************************************************************
 * INCLUDE FILES
 ******************************************************************************/
#include "voc_framework/transactions/transaction.h"
#include "voc_framework/transactions/transaction_id.h"
#include "voc_framework/features/feature.h"

#include <algorithm>
#include <chrono>
#include <ctime>
#include <mutex>
#include <random>
#include <thread>
#include <vector>
#include <string>

#include <gtest/gtest.h>

namespace voc_feature_test
{


class TestSignal : public fsm::Signal
{
public:
    int processing_delay; //time transaction's consumer will spend processing (waiting)
                          //before returning from HandleSignal
    bool stop;            //if true then this is the last message in transaction
                          //and transaction should complete afterwards
    int cleanup_delay;    //special case, used by feature. Delay introduced in handleSignal
                          //that gives transaction a chance to complete execution before
                          //the purging starts. Used on the last message in the test
                          //to help avoiding a race between purging operation
                          //and consumer threads in transactions

    TestSignal (fsm::TransactionId& transaction_id, bool stopper=false,
                        int cleanup=0, int processing=0) : fsm::Signal::Signal(transaction_id, kTypeUndefined)
                         {stop = stopper; cleanup_delay=cleanup; processing_delay=processing;}

    std::string ToString() { return "TestSignal"; }
};


class TestTransaction : public fsm::Transaction
{
public:

    TestTransaction () :
        Transaction ()
    {
    }

    bool WantsSignal (std::shared_ptr<fsm::Signal> signal)
    {
        return true;
    }

    bool HandleSignal (std::shared_ptr<fsm::Signal> signal)
    {

        std::shared_ptr<TestSignal> test_signal = std::static_pointer_cast<TestSignal>(signal);

        if (test_signal->processing_delay > 0)
        {
            std::default_random_engine generator;
            std::uniform_int_distribution<int> distribution(0, test_signal->processing_delay);
            std::chrono::milliseconds sleep_time(distribution( generator ));
            std::this_thread::sleep_for(sleep_time);
        }


        if (test_signal->stop )
        {
            return false; //last signal in the transaction
        }
        else
        {
            return true;
        }

        return true;
    }

    ~TestTransaction()
    {
        Terminate();
    }

};


class TestFeature : public fsm::Feature
{
    void HandleSignal(std::shared_ptr<fsm::Signal> signal)
    {

        //For every signal create a new transaction
        //and add it to the active transactions queue
        std::shared_ptr<TestTransaction> transaction =
            std::make_shared<TestTransaction>();
        AddActiveTransaction(transaction);
        transaction->EnqueueSignal(signal);

        //Sleeping here can be used to to assure that transaction's worker
        //thread is able to process the enqueued signal.
        //Otherwise there is a risk ending up in a race condition
        //between the worker thread and purging operation.
        //This is not desired in the case of TestFeature, as the
        //testing requires known state at the end of the test.
        std::shared_ptr<TestSignal> test_signal = std::static_pointer_cast<TestSignal>(signal);
        std::chrono::milliseconds sleep_time(test_signal->cleanup_delay);
        std::this_thread::sleep_for(sleep_time);
    }

  public:

    TestFeature() : fsm::Feature("TestFeature") {}

    /**
     * Check that the transaction recevied signals as expected
     */
    bool IsAsExpected (int expected_number_of_active_transactions)
    {
        //confirm that expected number of transactions on the
        //active transaction list
        if (active_transactions.size() != expected_number_of_active_transactions)
        {
            return false;
        }

        //make sure that all transactions on the active transaction list
        //are indeed active
        return (std::find_if(active_transactions.begin(),
                             active_transactions.end(),
                             [&](std::shared_ptr<fsm::Transaction> transaction)
                             {
                                 return !transaction->IsActive();
                             }) == active_transactions.end());
    }

    /**
     * Exposes possibility to add a unique transaction - for testing adding of unique transactions
     */
    void AddUniqueTransaction(std::shared_ptr<TestTransaction> transaction)
    {
        AddActiveTransactionIfUnique(transaction);
    }
};


void GenerateSignals(const int start_id, int max_iter, TestFeature& testFeature, int share_ongoing, int& counter)
{
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(1,10);

    for (int i =0; i<max_iter; i++) {

        fsm::IpCommandBrokerTransactionId transaction_id(start_id+i);
        //create signal that is NOT the last in a transaction
        bool stopper = (distribution( generator ) > share_ongoing);
        if (!stopper)
        {
            counter++;
        }
        std::shared_ptr<fsm::Signal> test_signal = std::make_shared<TestSignal>(transaction_id, stopper, 0, 15);

        testFeature.ProcessSignal(test_signal);
    }
}

void EnqueueUniqueTransaction(TestFeature& testFeature, std::shared_ptr<TestTransaction> transaction, int counter)
{
    for (int i =0; i<counter; i++) {
        testFeature.AddUniqueTransaction(transaction);
    }
}

//clean up signal giving threads time to finalize signal processing
//to avoid race conditions, will not increase the counter of active transactions
//as the transaction used here is final and should be removed
//testFeature - feature under test
//sleep - time given to the feature to perform cleanup
void GenerateFinalSignal(TestFeature& testFeature, int sleep)
{
    fsm::IpCommandBrokerTransactionId transaction_id(-1);
    std::shared_ptr<fsm::Signal> test_signal = std::make_shared<TestSignal>(transaction_id, true, sleep);
    testFeature.ProcessSignal(test_signal);
}

/******************************************************************************
 * TESTS
 ******************************************************************************/
TEST (BaseTransactionTest, ValidateCurrentDate)
{
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    struct tm *parts = std::localtime(&now_c);
    int year = 1900 + parts->tm_year;
    EXPECT_TRUE(year > 2000) << "Have you forgott to set the date/time?";
}

/**
 * Verify that transaction gets processed and removed from the transaction list
 *
 * TEST PASSES:
 *      Transaction not among active transactions.
 * TEST FAILS:
 *      Transaction among active transactions.
 */
TEST (BaseFeatureTest, PurgeSingleTransaction)
{

    TestFeature testFeature;

    GenerateFinalSignal(testFeature,50);
    EXPECT_TRUE(testFeature.IsAsExpected(0));
}

/**
 * Verify that transaction gets processed and NOT removed from the transaction list
 *
 * TEST PASSES:
 *      Transaction among active transactions.
 * TEST FAILS:
 *      Transaction not among active transactions.
 */
TEST (BaseFeatureTest, KeepSingleTransaction)
{
    fsm::IpCommandBrokerTransactionId transaction_id(1);

    //create signal that is NOT the last in a transaction
    std::shared_ptr<fsm::Signal> test_signal = std::make_shared<TestSignal>(transaction_id, false,100);

    TestFeature testFeature;

    testFeature.ProcessSignal(test_signal);

    EXPECT_TRUE(testFeature.IsAsExpected(1));

}

/**
 * Verify that transaction gets processed and removed from the transaction list
 *
 * TEST PASSES:
 *      Transaction not among active transactions.
 * TEST FAILS:
 *      Transaction among active transactions.
 */
TEST (BaseFeatureTest, PurgeMultipleTransactions)
{
    const int max_iter = 300;
    int counter = 0;

    TestFeature testFeature;

    GenerateSignals(0,max_iter,testFeature,3,counter);
    GenerateFinalSignal(testFeature,50);
    EXPECT_TRUE(testFeature.IsAsExpected(counter));

}

/**
 * Verify that transaction gets processed and removed from the transaction list
 * This test extends the previous one by adding a concurrency aspect to the problem
 *
 * TEST PASSES:
 *      Non-active transactions purged from the active transaction list.
 * TEST FAILS:
 *      Non-active transactions not purged from the active transaction list
 *      or active transactions purged from the active transaction list.
 */
TEST (BaseFeatureTest, PurgeMultipleTransactionsConcurrently)
{
    TestFeature testFeature;
    const int max_iter1 = 100;
    const int max_iter2 = 100;
    const int max_iter3 = 100;
    int counter1 = 0;
    int counter2 = 0;
    int counter3 = 0;
    std::thread thread1(GenerateSignals, 0, max_iter1, std::ref(testFeature), 5, std::ref(counter1));
    std::thread thread2(GenerateSignals, max_iter1, max_iter2, std::ref(testFeature), 2, std::ref(counter2));
    std::thread thread3(GenerateSignals, max_iter2, max_iter3, std::ref(testFeature), 7, std::ref(counter3));

    thread1.join();
    thread2.join();
    thread3.join();

    GenerateFinalSignal(testFeature,50);
    EXPECT_TRUE(testFeature.IsAsExpected(counter1+counter2+counter3));
}

/**
 * Verify that transaction gets only added once if it is added to a feature using AddActiveTransactionIfUnique
 *
 * TEST PASSES:
 *      Only one transaction added to the feature.
 * TEST FAILS:
 *      0 or more than one transactions added to the feature
 */
TEST (BaseFeatureTest, TestAddUniqueTransaction)
{
    TestFeature testFeature;
    std::shared_ptr<TestTransaction> transaction =
        std::make_shared<TestTransaction>();

    std::thread thread1(EnqueueUniqueTransaction, std::ref(testFeature), transaction, 90);
    std::thread thread2(EnqueueUniqueTransaction, std::ref(testFeature), transaction, 60);
    std::thread thread3(EnqueueUniqueTransaction, std::ref(testFeature), transaction, 70);

    thread1.join();
    thread2.join();
    thread3.join();

    EXPECT_TRUE(testFeature.IsAsExpected(1));
}


/** \}    end of addtogroup */

} // namespace
