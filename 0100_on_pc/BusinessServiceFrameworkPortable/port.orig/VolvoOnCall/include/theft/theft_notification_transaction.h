///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental AG and subsidiaries
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// @file theft_notification_transaction.h
// geely theft transaction.

// @project         GLY_TCAM
// @subsystem   Application
// @author        Hu Tingting
// @Init date     3-Sep-2018
///////////////////////////////////////////////////////////////////


#ifndef VOC_TRANSACTIONS_THEFT_NOTIFICATION_TRANSACTION_H_
#define VOC_TRANSACTIONS_THEFT_NOTIFICATION_TRANSACTION_H_

#include "voc_framework/transactions/smart_transaction.h"
#include "signals/geely/gly_vds_theft_signal.h"
#include "timestamp.h"

namespace volvo_on_call
{

class TheftNotificationTransaction: public fsm::SmartTransaction
{
 public:
    /************************************************************/
    // @brief :Constructs signal.
    // @param[in]  initial_signal,fsm::Signal, which caused transaction to be created.
    // @param[in]  vdsService, vds msg.
    // @return
    // @author     Hu Tingting, 3-Sep-2018
    /************************************************************/
    TheftNotificationTransaction (std::shared_ptr<fsm::Signal> initial_signal);

    virtual ~TheftNotificationTransaction();

public:
    enum theftSignalTypes
    {
        kinit = 500,
        kFeedbackFromTsp,
        kMAX
    };

    class TheftACKSignal : public fsm::Signal
    {
        public:
            TheftACKSignal(): fsm::Signal(fsm::CCMTransactionId(), theftSignalTypes::kFeedbackFromTsp)
            {
            }
            std::string ToString()
            {
                return "TheftACKSignal";
            }
    };


    Timestamp::TimeVal GetUtcTime()
    {
        Timestamp now;
        return now.epochMicroseconds();//rerurn s and us
    }

    void MapTheftTransactionId(std::shared_ptr<fsm::Signal> signal)
    {
        MapTransactionId(signal->GetTransactionId().GetSharedCopy());
    }

 private:

     /************************************************************/
    // @brief :Enumerates the states this transaction can be in.
    // @author     Hu Tingting, 3-Sep-2018
    /************************************************************/
    enum States
    {
        kInit = 0, ///< Newly created.
        kActive, ///< handle active signal.
        kNotActive, ///< Handle not active signal
        kTeminate///< Finished
    };

    enum TheftStatus
    {
       NotActvd, //0x0:AntithftActvnNotif1_NotActvd
       Actvd //0x1:AntithftActvnNotif1_Actvd
    };

    fsm::TimeoutTransactionId m_theftActvd_tick_id;
    fsm::TimeoutTransactionId m_theftNotActvd_tick_id;
    fsm::TimeoutTransactionId m_theftKeepActvd_tick_id;

    const std::chrono::seconds theftActvdTime = std::chrono::seconds(0);
    const std::chrono::seconds theftNotActvdTime = std::chrono::seconds(0);
    const std::chrono::seconds theftKeepActvdTime = std::chrono::seconds(60);
    std::shared_ptr<GlyVdsTheftSignal> m_currTheftSignal;


    /************************************************************/
    // @brief :handleTheftSignal.
    // @param[in]  signal,fsm::Signal, which caused transaction to be created.
    // @return      True if transaction is not yet finished, false if it is finished.
    // @author     Hu Tingting, 3-Sep-2018
    /************************************************************/
    bool handleTheftSignal(std::shared_ptr<fsm::Signal> signal);


    /************************************************************/
    // @brief :checkEnableCondition.
    // @param[in]  signal,fsm::Signal, which caused transaction to be created.
    // @return      True if transaction is not yet finished, false if it is finished.
    // @author     Hu Tingting, 3-Sep-2018
    /************************************************************/
    bool checkEnableCondition();


    /************************************************************/
    // @brief : handleTimerOut.
    // @param[in]  signal,fsm::Signal, which caused transaction to be created.
    // @return      True if transaction is not yet finished, false if it is finished.
    // @author     Hu Tingting, 3-Sep-2018
    /************************************************************/
    bool handleTimerOut(std::shared_ptr<fsm::Signal> signal);


    /************************************************************/
    // @brief : uploadActiveToTsp.
    // @param[in]  signal,fsm::Signal, which caused transaction to be created.
    // @return      True if transaction is not yet finished, false if it is finished.
    // @author     Hu Tingting, 3-Sep-2018
    /************************************************************/
    bool uploadActiveToTsp(State old_state, State new_state);


    /************************************************************/
    // @brief : startKeepActiveTimer.
    // @param[in]  signal,fsm::Signal, which caused transaction to be created.
    // @return      True if transaction is not yet finished, false if it is finished.
    // @author     Hu Tingting, 3-Sep-2018
    /************************************************************/
    bool startKeepActiveTimer(std::shared_ptr<fsm::Signal> signal);


    /************************************************************/
    // @brief : killTheftN.
    // @param[in]  signal,fsm::Signal, which caused transaction to be created.
    // @return      True if transaction is not yet finished, false if it is finished.
    // @author     Hu Tingting, 3-Sep-2018
    /************************************************************/
    bool killTheftN(State old_state, State new_state);


    /************************************************************/
    // @brief : uploadNotActiveToTsp.
    // @param[in]  none
    // @return      True if transaction is not yet finished, false if it is finished.
    // @author     Hu Tingting, 3-Sep-2018
    /************************************************************/
    bool uploadNotActiveToTsp();

};

} // namespace volvo_on_call

#endif //VOC_TRANSACTIONS_THEFT_NOTIFICATION_TRANSACTION_H_
