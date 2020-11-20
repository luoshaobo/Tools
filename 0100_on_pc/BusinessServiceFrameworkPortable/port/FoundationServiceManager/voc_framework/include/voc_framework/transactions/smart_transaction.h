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
 *  \file     smart_transaction.h
 *  \brief    VOC Service smart transaction class.
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup SW_package
 *  \{
 */

#ifndef VOC_FRAMEWORK_TRANSACTIONS_SMART_TRANSACTION_H_
#define VOC_FRAMEWORK_TRANSACTIONS_SMART_TRANSACTION_H_

#include "voc_framework/signals/signal.h"
#include "voc_framework/transactions/transaction.h"

#include <unordered_map>
#include <vector>

namespace fsm
{

class SmartTransaction : public Transaction
{

    /**
     * \brief A smart transaction is a transaction with a state machine like
     *        concept added allowing the derived  specific transaction to
     *        focus only on the functions to handle Signal X in State Y.
     *        Derived class will set a number of States with associated
     *        StateDescriptors which defines what the SmartTransaction
     *        should do when Signal X ins received in State Y.
     */

 public:

    /**
     * \brief State type, defined to int on assumption that deriving classes will
     *       typically want to define states as enums.
     */
    typedef int State;

    /**
     * \brief Indicates State is not defined.
     */
    static const int kUndefinedState = -1;

    /**
     * \brief A function called when entering a given state.
     */
    typedef std::function<void(State old_state, State new_state)> StateFunction;

    /**
     * \brief A function handling a given signal in a given state.
     */
    typedef std::function<bool(std::shared_ptr<Signal>)> SignalFunction;

#if __GNUC__ < 6
    // Encountered compiler which did not have "built in" hash() for enums when using enum as key in map.
    struct SignalTypeHash
    {
        std::size_t operator()(Signal::SignalType signal_type) const
        {
            return static_cast<std::size_t>(signal_type);
        }
    };

    /**
     * \brief A map of SignalFunctions per SignalType.
     */
    typedef std::unordered_map<Signal::SignalType, SignalFunction, SignalTypeHash> SignalFunctionMap;
#else
    /**
     * \brief A map of SignalFunctions per SignalType.
     */
    typedef std::unordered_map<Signal::SignalType, SignalFunction> SignalFunctionMap;
#endif

    /**
     * \brief Descibes what SmartTransaction should do in a state.
     */
    typedef struct StateDescriptor
    {
        StateFunction state_function; ///< Called when entering state if set.
        SignalFunctionMap signal_function_map; ///< Handler functions per signal.
        std::vector<State> legal_transitions; ///< List of legal states to transition to.
    } StateDescriptor;

    /**
     * \brief Map of states with descriptions. This is how the
     *        internal "state machine" is represented.
     */
    typedef std::unordered_map<State, StateDescriptor> StateMap;

    /**
     * \brief Get the current state.
     *
     * \return The current State, or kUndefinedState if not set.
     */
    State GetState();

 protected:

    /**
     * \brief Constructor is hidden, as only subclasses should be instantiated.
     *        Subclasses must make sure to execute this constructor as well.
     *        Please note that the StateMap must be populated before any signals
     *        can be processed.
     * \param[in] initial_state Initial state of the transaction.
     */
    SmartTransaction (State intial_state = kUndefinedState);

    /**
     * \brief Adds a state to the smart transaction, or replaces
     *        description if state already exists.
     *
     * \param[in] state The state beeing added or updated.
     * \param[in] state_descriptor Descriptor for the state.
     *
     * \return None.
     */
    void AddState(State state, StateDescriptor& state_descriptor);

    /**
     * \brief Sets the internal state map, will replace any previously set
     *        state descriptions.
     *
     * \param[in] state_map The StateMap to set.
     *
     * \return None.
     */
    void SetStateMap(StateMap state_map);

    /**
     * \brief Set the current state, transition will be verified to be legal.
     *
     * \param[in] state State to set.
     *
     * \return True if state set successfully, false otherwise.
     */
    bool SetState(State state);

    /**
     * \brief Decides if a signal is relevant for the SmartTransaction.
     *        Will verify that the signal is present in the transactions
     *        state_map and that the transaction id or session id or singal type
     *        is mapped. If all checks pass the Signal will be considered
     *        wanted.
     *
     *  \param[in]  signal Signal for processing.
     *  \return True if signal is wanted, false otherwise.
     */
    bool WantsSignal (std::shared_ptr<Signal> signal);

    /**
     * \brief Signal handling logic. Will verify the signal is still wanted
     *        (state changes may have occured since enqueue) and then call
     *        the signal function defined for the recevied signal in the
     *        current state in the current states state descriptor.
     * \param[in]  signal Signal for processing.
     * \return true if further signals are expected, false when no more signals
     *         are expected and the transaction can be terminated. Propagated
     *         from the signal function.
     */
    bool HandleSignal (std::shared_ptr<Signal> signal);

 private:

    /**
     * \brief the current state.
     */
    State state_;

    /**
     * \brief descriptor map of all known states.
     */
    StateMap state_map_;

    /**
     * \brief Decides if a signal is relevant for the SmartTransaction.
     *        Will verify that the signal is present in the current states
     *        StateDescriptor SignalFunctionMap and that the transaction
     *        id or session id or singal type is mapped.
     *        If all checks pass the Signal will be considered
     *        wanted.
     *
     * \param[in] signal         Signal for processing.
     * \param[in] consider_state Controls whether currente state should be considered.
     *
     * \return True if signal is wanted, false otherwise.
     */
    bool WantsSignalInternal (std::shared_ptr<Signal> signal, bool consider_state);
};

} // namespace fsm

#endif //VOC_FRAMEWORK_TRANSACTIONS_SMART_TRANSACTION_H_

/** \}    end of addtogroup */
