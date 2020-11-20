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

#include <gtest/gtest.h>

#include "voc_framework/transactions/smart_transaction.h"
#include "voc_framework/signals/signal.h"
#include "signals/signal_types.h"


#include <chrono>
#include <functional>
#include <iostream>

namespace voc_smart_transaction_test
{

using namespace fsm;
using namespace volvo_on_call;

class TestSignal : public fsm::Signal
{
public:

    TestSignal (fsm::TransactionId& transaction_id, fsm::Signal::SignalType type) : fsm::Signal::Signal(transaction_id, type) {}

    std::string ToString() { return "TestSignal"; };
};

class TestTransaction : public fsm::SmartTransaction
{
public:

    TestTransaction () :
        fsm::SmartTransaction()
    {
    }

    void AddState(State state, StateDescriptor& state_descriptor)
    {
        fsm::SmartTransaction::AddState(state, state_descriptor);
    }

    void SetStateMap(StateMap state_map)
    {
        fsm::SmartTransaction::SetStateMap(state_map);
    }

    bool SetState(State state)
    {
        return fsm::SmartTransaction::SetState(state);
    }

    /**
     * \brief Add a transaction id into the internal list
     *        of transactions ids mapped to this transaction.
     * \param[in] transaction_id The id to add.
     * \return None.
     */
    void MapTransactionId(std::shared_ptr<fsm::TransactionId> transaction_id)
    {
        Transaction::MapTransactionId(transaction_id);
    }
};

/******************************************************************************
 * TESTS
 ******************************************************************************/

// Statemachine used during test:
//
//         kNew
//        /
//       /
//      /
//     kStopped <-> kPlaying <-> kPaused
//

/**
 * States in test state machine.
 */
enum States
{
    kNew = 0,
    kPlaying,
    kPaused,
    kStopped,
    kNotValid
};

/**
 * States in the order tests will traverse them
 */
std::vector<fsm::SmartTransaction::State> states =
    {kNew, kStopped, kPlaying, kPaused, kPlaying, kStopped};
std::vector<std::vector<fsm::SmartTransaction::State>> valid_transitions =
    {{kStopped},
     {kStopped, kPaused},
     {kPlaying},
     {kPlaying}};
std::vector<std::vector<fsm::SmartTransaction::State>> invalid_transitions =
    {{kNew, kPlaying, kPaused, kNotValid},
     {kNew, kPlaying, kNotValid},
     {kNew, kPaused, kStopped, kNotValid},
     {kNew, kPaused, kStopped, kNotValid}};
fsm::SmartTransaction::StateMap state_map =
    {{kNew, {nullptr,
             fsm::SmartTransaction::SignalFunctionMap(),
             valid_transitions[kNew]}},
     {kPlaying, {nullptr,
                 fsm::SmartTransaction::SignalFunctionMap(),
                 valid_transitions[kPlaying]}},
     {kPaused, {nullptr,
                fsm::SmartTransaction::SignalFunctionMap(),
                valid_transitions[kPaused]}},
     {kStopped, {nullptr,
                 fsm::SmartTransaction::SignalFunctionMap(),
                 valid_transitions[kStopped]}}};

/**
 * Verify that state transitions work as described.
 */
TEST (SmartTransactionTest, StateTransitions1)
{
    TestTransaction test_transaction;
    test_transaction.SetStateMap(state_map);

    for (fsm::SmartTransaction::State state : states)
    {
        // Set new state
        ASSERT_TRUE(test_transaction.SetState(state));
        ASSERT_EQ(test_transaction.GetState(), state);

        // Try to set invalid states
        for (fsm::SmartTransaction::State invalid_state : invalid_transitions[state])
        {
            ASSERT_FALSE(test_transaction.SetState(invalid_state));
            ASSERT_EQ(test_transaction.GetState(), state);
        }
    }
}

/**
 * Verify that state transitions work as described.
 */
TEST (SmartTransactionTest, StateTransitions2)
{
    TestTransaction test_transaction;

    for (fsm::SmartTransaction::State state : states)
    {
        test_transaction.AddState(state, state_map[state]);
    }

    for (fsm::SmartTransaction::State state : states)
    {
        // Set new state
        ASSERT_TRUE(test_transaction.SetState(state));
        ASSERT_EQ(test_transaction.GetState(), state);

        // Try to set invalid states
        for (fsm::SmartTransaction::State invalid_state : invalid_transitions[state])
        {
            ASSERT_FALSE(test_transaction.SetState(invalid_state));
            ASSERT_EQ(test_transaction.GetState(), state);
        }
    }
}

fsm::SmartTransaction::State last_old_state = fsm::SmartTransaction::kUndefinedState;
fsm::SmartTransaction::State last_new_state = fsm::SmartTransaction::kUndefinedState;

void StateFunctionImpl(fsm::SmartTransaction::State old_state, fsm::SmartTransaction::State new_state)
{
    last_old_state = old_state;
    last_new_state = new_state;
}

/**
 * Verify that state functions are called as expected.
 */
TEST (SmartTransactionTest, StateFunctions)
{
    for (fsm::SmartTransaction::State state : states)
    {
        state_map[state].state_function = StateFunctionImpl;
    }

    TestTransaction test_transaction;
    test_transaction.SetStateMap(state_map);

    // Walk states and ensure state function was called.
    for (fsm::SmartTransaction::State state : states)
    {
        fsm::SmartTransaction::State old_state = test_transaction.GetState();

        ASSERT_TRUE(test_transaction.SetState(state));
        ASSERT_EQ(old_state, last_old_state);
        ASSERT_EQ(state, last_new_state);
    }

    for (fsm::SmartTransaction::State state : states)
    {
        state_map[state].state_function = nullptr;
    }

    last_old_state = fsm::SmartTransaction::kUndefinedState;
    last_new_state = fsm::SmartTransaction::kUndefinedState;
}


bool last_signal_call_ok = true;

bool SignalFunctionImpl(fsm::SmartTransaction::State expected_state,
                        fsm::Signal::SignalType expected_type,
                        fsm::SmartTransaction* transaction,
                        bool stop_transaction,
                        std::shared_ptr<fsm::Signal> signal)
{
    last_signal_call_ok = last_signal_call_ok &&
                          (expected_state == transaction->GetState()) &&
                          (expected_type == signal->GetSignalType());

    return stop_transaction;
}

bool VerifySignalFunction()
{
    std::chrono::milliseconds sleep_time(50);
    std::this_thread::sleep_for(sleep_time);

    bool was_ok = last_signal_call_ok;
    last_signal_call_ok = true;

    return was_ok;
}

/**
 * Verify that signals are handled as expected.
 */
TEST (SmartTransactionTest, Signals)
{
    using namespace std::placeholders;

    TestTransaction test_transaction;

    for (fsm::SmartTransaction::State state : states)
    {
        // setup kTestSignalId0 and 1 to not stop transaction
        state_map[state].signal_function_map[VocSignalTypes::kTestSignalId0] =
            std::bind(SignalFunctionImpl, state, VocSignalTypes::kTestSignalId0, &test_transaction, true, _1);
        state_map[state].signal_function_map[VocSignalTypes::kTestSignalId1] =
            std::bind(SignalFunctionImpl, state, VocSignalTypes::kTestSignalId1, &test_transaction, true, _1);

        // setup kTestSignalId2 to stop transaction
        state_map[state].signal_function_map[VocSignalTypes::kTestSignalId2] =
            std::bind(SignalFunctionImpl, state, VocSignalTypes::kTestSignalId2, &test_transaction, false, _1);
    }

    test_transaction.SetStateMap(state_map);

    std::shared_ptr<fsm::IpCommandBrokerTransactionId> transaction_id =
        std::make_shared<fsm::IpCommandBrokerTransactionId>();
    std::shared_ptr<TestSignal> test_signal_0 =
        std::make_shared<TestSignal>(*transaction_id, VocSignalTypes::kTestSignalId0);
    std::shared_ptr<TestSignal> test_signal_1 =
        std::make_shared<TestSignal>(*transaction_id, VocSignalTypes::kTestSignalId1);
    std::shared_ptr<TestSignal> test_signal_stopper =
        std::make_shared<TestSignal>(*transaction_id, VocSignalTypes::kTestSignalId2);
    std::shared_ptr<TestSignal> test_signal_unmapped_type =
        std::make_shared<TestSignal>(*transaction_id, VocSignalTypes::kTestSignalId3);

    IpCommandBrokerTransactionId unmapped_transaction_id;
    std::shared_ptr<TestSignal> test_signal_unmapped_id =
        std::make_shared<TestSignal>(unmapped_transaction_id, VocSignalTypes::kTestSignalId0);

    // Walk states, inject signals and verify signal functions are called correctly.
    for (fsm::SmartTransaction::State state : states)
    {
        test_transaction.MapTransactionId(transaction_id);

        ASSERT_TRUE(test_transaction.SetState(state));
        ASSERT_TRUE(test_transaction.EnqueueSignal(test_signal_0));
        ASSERT_TRUE(test_transaction.EnqueueSignal(test_signal_1));
        // kTestSignalId3 is not in signal_function_map so signal should be rejected.
        ASSERT_FALSE(test_transaction.EnqueueSignal(test_signal_unmapped_type));
        // unmapped_transaction_id has not been mapped so signal should be rejected.
        ASSERT_FALSE(test_transaction.EnqueueSignal(test_signal_unmapped_id));

        // Verify singal functions were called as expected.
        ASSERT_TRUE(VerifySignalFunction());
    }

    // Finally verify that we can stop transactions.
    ASSERT_TRUE(test_transaction.EnqueueSignal(test_signal_stopper));
    ASSERT_TRUE(VerifySignalFunction());
    ASSERT_FALSE(test_transaction.IsActive());
    ASSERT_TRUE(test_transaction.HasCompleted());

    for (fsm::SmartTransaction::State state : states)
    {
        state_map[state].signal_function_map.clear();
    }

    last_signal_call_ok = true;
}

/**
 * Verify that can be enqued in the wrong state but are not handled..
 */
TEST (SmartTransactionTest, BadState)
{
    using namespace std::placeholders;

    TestTransaction test_transaction;

    // setup kTestSignalId0 in state kPaused to stop transaction
    state_map[kPaused].signal_function_map[VocSignalTypes::kTestSignalId0] =
            std::bind(SignalFunctionImpl, kPaused, VocSignalTypes::kTestSignalId0, &test_transaction, false, _1);

    test_transaction.SetStateMap(state_map);

    std::shared_ptr<fsm::IpCommandBrokerTransactionId> transaction_id =
        std::make_shared<fsm::IpCommandBrokerTransactionId>();
    std::shared_ptr<TestSignal> test_signal_0 =
        std::make_shared<TestSignal>(*transaction_id, VocSignalTypes::kTestSignalId0);

    test_transaction.MapTransactionId(transaction_id);

    ASSERT_TRUE(test_transaction.SetState(kPlaying));
    ASSERT_TRUE(test_transaction.EnqueueSignal(test_signal_0));

    // Verify singal functions were called as expected.
    ASSERT_TRUE(VerifySignalFunction());

    // Finally verify that transaction was not stopped.
    ASSERT_TRUE(test_transaction.IsActive());
    ASSERT_FALSE(test_transaction.HasCompleted());

    state_map[kPaused].signal_function_map.clear();

    last_signal_call_ok = true;
}

/** \}    end of addtogroup */

} //namespace
