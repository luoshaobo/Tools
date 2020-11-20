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
 *  \file     unittest_internal_signal_source.cc
 *  \brief    FoundationServiceManager, voc_framework, inernal signal source, Test Cases
 *  \author   Piotr Tomaszewski
 *
 *  \addtogroup unittest
 *  \{
 */

/******************************************************************************
 * INCLUDE FILES
 ******************************************************************************/

#include <gtest/gtest.h>


#include "voc_framework/signal_sources/internal_signal_source.h"
#include "voc_framework/signal_sources/signal_receiver_interface.h"
#include "voc_framework/signals/internal_signal.h"




/******************************************************************************
 * Test classes
 ******************************************************************************/

//example object carrying data. Carries one integer value
class Data
{
public:
    Data(int i)
    {
        i_ = i;
    }

    int GetI()
    {
        return i_;
    }

private:
    int i_;
};

//test internal signal, carries object of a Data type
class TestInternalSignal: public fsm::InternalSignal<Data>
{
public:
    TestInternalSignal (const fsm::TransactionId& transaction_id,
                        const fsm::Signal::SignalType signal_type,
                        const std::shared_ptr<Data> payload) : InternalSignal<Data>(transaction_id,
                                                                                 signal_type,
                                                                                 payload)
    {

    }
};

//test signal listener class that only stores received signal
class TestSignalListener: public fsm::SignalReceiverInterface
{
public:

    TestSignalListener()
    {
        signal_ = nullptr;
    }

    void ProcessSignal (std::shared_ptr<fsm::Signal> signal)
    {
        signal_ = signal;
    }

    std::shared_ptr<fsm::Signal> signal_;
};

/******************************************************************************
 * TESTS
 ******************************************************************************/

/**
 * Verify that Internal signal source can successfully accept registartion and broadcast
 * an internal signal to registered listeners
 *
 * TEST PASSES:
 *      signal broadcasted.
 * TEST FAILS:
 *      signal not broadcasted.
 */
TEST (InternalSignalSourceTest, SendInternalSignalTest)
{
    TestSignalListener listener;

    int value = 2354234;
    fsm::CCMTransactionId transaction_id;
    fsm::Signal::SignalType signal_type = 234234;

    fsm::InternalSignalSource& internal_signal_source = fsm::InternalSignalSource::GetInstance();

    std::shared_ptr<Data> data = std::make_shared<Data>(value);

    std::shared_ptr<TestInternalSignal> signal = std::make_shared<TestInternalSignal>(transaction_id,
                                                                                      signal_type,
                                                                                      data);

    internal_signal_source.RegisterSignalReceiver(listener);

    internal_signal_source.BroadcastSignal(signal);

    //remove the subscribtion as not needed anymore (internal signal source is a singleton, may survive
    //individual test cases
    internal_signal_source.UnregisterSignalReceiver(listener);

    //make sure we have received a signal
    ASSERT_TRUE(listener.signal_ != nullptr);

    //make sure the signal received is of a right type
    EXPECT_TRUE(listener.signal_->GetSignalType() == signal_type);

    //get TestInternalSignal
    std::shared_ptr<TestInternalSignal> casted_signal = std::static_pointer_cast<TestInternalSignal>(listener.signal_);
    ASSERT_TRUE( casted_signal != nullptr);

    //retrieve data package
    std::shared_ptr<Data> data_received = casted_signal->GetPayload();

    //make sure that we have received data
    ASSERT_TRUE(data_received != nullptr);

    EXPECT_TRUE(data_received->GetI() == value);
}



/** \}    end of addtogroup */
