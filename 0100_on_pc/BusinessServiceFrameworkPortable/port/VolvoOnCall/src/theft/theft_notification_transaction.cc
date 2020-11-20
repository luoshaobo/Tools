///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental AG and subsidiaries
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// @file theft_notification_transaction.cc
// geely theft transaction.

// @project         GLY_TCAM
// @subsystem   Application
// @author        Hu Tingting
// @Init date     3-Sep-2018
///////////////////////////////////////////////////////////////////


#include "theft/theft_notification_transaction.h"
#include "dlt/dlt.h"

#include "signals/signal_types.h"
//#include "voc_framework/signals/vpom_signal.h"
//#include "voc.h"
#include "voc_framework/signals/ip_command_broker_signal.h"
#include "voc_framework/signal_sources/ip_command_broker_signal_source.h"
#include "voc_framework/signal_sources/vocmo_signal_source.h"
#include "voc_framework/transactions/transaction_id.h"

DLT_IMPORT_CONTEXT(dlt_voc);

namespace volvo_on_call
{


/************************************************************/
// @brief :Constructs signal.
// @param[in]  initial_signal,fsm::Signal, which caused transaction to be created.
// @param[in]  vdsService, vds msg.
// @return
// @author     Hu Tingting, 3-Sep-2018
/************************************************************/
TheftNotificationTransaction::TheftNotificationTransaction(std::shared_ptr<fsm::Signal> initial_signal) :
    fsm::SmartTransaction(kInit)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "theft:%s: %s", __FUNCTION__, typeid(*initial_signal).name());

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "theft:%s()m_currTheftSignal = %d", __FUNCTION__, (void *) m_currTheftSignal.get());
    fsm::VdServiceTransactionId vdsTransId;
    asn_wrapper::VDServiceRequest *vdmsg = nullptr;
    m_currTheftSignal =  std::dynamic_pointer_cast<GlyVdsTheftSignal> (GlyVdsTheftSignal::CreateGlyVdsTheftSignal(vdsTransId, vdmsg));
    if (m_currTheftSignal.get() == nullptr)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "theft:%s() m_currTheftSignal.get() == nullptr", __FUNCTION__);

    }

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "theft:%s()m_currTheftSignal = %d", __FUNCTION__, (void *) m_currTheftSignal.get());
    StateFunction upload_active_toTsp =
        std::bind(&TheftNotificationTransaction::uploadActiveToTsp,
                  this,
                  std::placeholders::_1,
                  std::placeholders::_2);

    StateFunction kill_Theft_Notification =
        std::bind(&TheftNotificationTransaction::killTheftN,
                  this,
                  std::placeholders::_1,
                  std::placeholders::_2);

    StateMap state_map = {{kInit, {nullptr,                      // state function
                                          SignalFunctionMap(),     // signal function map
                                        {kActive, kTeminate}}}, // valid transitions
                                       {kActive, {upload_active_toTsp,                         // state function
                                          SignalFunctionMap(),             // signal function map
                                        {kNotActive}}},  // valid transitions
                                       {kNotActive, {nullptr,                    // state function
                                          SignalFunctionMap(),   // signal function map
                                         {kTeminate}}},               // valid transitions
                                       {kTeminate, {kill_Theft_Notification,             // state function
                                          SignalFunctionMap(), // signal function map
                                         {}}}};               // valid transitions

    state_map[kInit].signal_function_map[fsm::Signal::kTheftDataSignal] =
        std::bind(&TheftNotificationTransaction::handleTheftSignal,
                  this,
                  std::placeholders::_1);
/*
    state_map[kInit].signal_function_map[fsm::Signal::kReceivetheftActvd] =
        std::bind(&TheftNotificationTransaction::checkEnableCondition,
                  this,
                  std::placeholders::_1);
*/
    state_map[kInit].signal_function_map[fsm::Signal::kTimeout] =
        std::bind(&TheftNotificationTransaction::handleTimerOut,
                  this,
                  std::placeholders::_1);

    state_map[kActive].signal_function_map[theftSignalTypes::kFeedbackFromTsp] =
        std::bind(&TheftNotificationTransaction::startKeepActiveTimer,
                  this,
                  std::placeholders::_1);//instead of feedback from tsp

    state_map[kNotActive].signal_function_map[fsm::Signal::kTimeout] =
        std::bind(&TheftNotificationTransaction::handleTimerOut,
                  this,
                  std::placeholders::_1);

    state_map[kNotActive].signal_function_map[fsm::Signal::kTheftDataSignal] =
        std::bind(&TheftNotificationTransaction::handleTheftSignal,
                  this,
                  std::placeholders::_1);

    SetStateMap(state_map);
    MapSignalType(theftSignalTypes::kFeedbackFromTsp);
    if (initial_signal)
    {
        MapTransactionId(initial_signal->GetTransactionId().GetSharedCopy());
        EnqueueSignal(initial_signal);
    }
}

TheftNotificationTransaction::~TheftNotificationTransaction()
 {
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "theft:%s()", __FUNCTION__);
 }

/************************************************************/
// @brief :handleTheftSignal.
// @param[in]  signal,fsm::Signal, which caused transaction to be created.
// @return      True if transaction is not yet finished, false if it is finished.
// @author     Hu Tingting, 3-Sep-2018
/************************************************************/
bool TheftNotificationTransaction::handleTheftSignal(std::shared_ptr<fsm::Signal> signal)
{
    bool return_value = true;
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "theft: %s received signal: %s in state %d", __FUNCTION__, signal->ToString().c_str(), GetState());
    std::shared_ptr<fsm::THEFTNotificationSignal> theft_signal = std::static_pointer_cast<fsm::THEFTNotificationSignal>(signal);
    TheftNoticeData theft_data = theft_signal->GetPayload();
    if((Actvd == theft_data.antithftActvnNotif) && (kInit == GetState()))
    {
       return_value = checkEnableCondition();

    if(return_value == true)
      {
         if(theftActvdTime > std::chrono::seconds(0))
         {
             m_theftActvd_tick_id = RequestTimeout(theftActvdTime, false);
             MapTransactionId(m_theftActvd_tick_id.GetSharedCopy());
             //to do: print timeid
             DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "theft: Timer timeOut %i s started", m_theftActvd_tick_id);
         }
         else
         {
             SetState(kActive);
         }

      }
      else
      {
         SetState(kTeminate);
      }
    }
    else if((NotActvd == theft_data.antithftActvnNotif) && (kNotActive == GetState()))
    {
         RemoveTimeout(m_theftKeepActvd_tick_id);

          if(theftNotActvdTime > std::chrono::seconds(0))
         {
             m_theftNotActvd_tick_id = RequestTimeout(theftNotActvdTime, false);
             MapTransactionId(m_theftNotActvd_tick_id.GetSharedCopy());
             //to do: print timeid
             DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "theft: Timer timeOut %i s started", m_theftNotActvd_tick_id);
         }
         else
         {
             uploadNotActiveToTsp();
         }

    }
    else
    {
       SetState(kTeminate);
    }
    return return_value;
}


/************************************************************/
// @brief :checkEnableCondition.
// @param[in]  signal,fsm::Signal, which caused transaction to be created.
// @return      True if transaction is not yet finished, false if it is finished.
// @author     Hu Tingting, 3-Sep-2018
/************************************************************/
bool TheftNotificationTransaction::checkEnableCondition()
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "theft: %s received signal: in state %d", __FUNCTION__, GetState());
    bool return_value = true;
/*
    if(return_value == true)
    {
       //RemoveTimeout(m_kWaitForGPS_tick_id);
       m_theftActvd_tick_id = RequestTimeout(static_cast<std::chrono::seconds>(theftActvdTime), false);
       MapTransactionId(m_theftActvd_tick_id.GetSharedCopy());
    }
    else
    {
       SetState(kTeminate);
    }
    */
    return return_value;
}


/************************************************************/
// @brief : handleTimerOut.
// @param[in]  signal,fsm::Signal, which caused transaction to be created.
// @return      True if transaction is not yet finished, false if it is finished.
// @author     Hu Tingting, 3-Sep-2018
/************************************************************/
bool TheftNotificationTransaction::handleTimerOut(std::shared_ptr<fsm::Signal> signal)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "theft: %s received signal: %s in state %d", __FUNCTION__, signal->ToString().c_str(), GetState());

    if(signal->GetTransactionId() == m_theftActvd_tick_id)
    {
       SetState(kActive);
    }
    else if(signal->GetTransactionId() == m_theftKeepActvd_tick_id)
    {
       SetState(kNotActive);
       uploadNotActiveToTsp();
    }
    else if(signal->GetTransactionId() == m_theftNotActvd_tick_id)
    {
       uploadNotActiveToTsp();
    }
    else
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "theft: transactionid error.\n");
    }

    return true;
}


/************************************************************/
// @brief : uploadActiveToTsp.
// @param[in]  signal,fsm::Signal, which caused transaction to be created.
// @return      True if transaction is not yet finished, false if it is finished.
// @author     Hu Tingting, 3-Sep-2018
/************************************************************/
bool TheftNotificationTransaction::uploadActiveToTsp(State old_state, State new_state)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "theft: %s: old state is %d, and new state is %d", __FUNCTION__, old_state, new_state);

    TheftUpload_t theft_upload;

    Timestamp::TimeVal utcTime = GetUtcTime();

    theft_upload.TimeSeconds= utcTime/1000000;
    theft_upload.TimeMilliseconds= (utcTime%1000000)/1000;

    theft_upload.activated = Actvd;

    m_currTheftSignal->SetTheftPayload(theft_upload);

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "theft:%s\n, upload signal is :%d, second is :%d",
        __FUNCTION__,theft_upload.activated,theft_upload.TimeSeconds);

    fsm::VocmoSignalSource vocmo_signal_source;
    if (vocmo_signal_source.GeelySendMessage(m_currTheftSignal))
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "theft: upload actvd ok.");
    }
    else
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "theft: upload actvd fail.");
    }
    std::shared_ptr<fsm::Signal> ackSignal = std::make_shared<TheftACKSignal>();
    EnqueueSignal(ackSignal);

    return true;
}


/************************************************************/
// @brief : startKeepActiveTimer.
// @param[in]  signal,fsm::Signal, which caused transaction to be created.
// @return      True if transaction is not yet finished, false if it is finished.
// @author     Hu Tingting, 3-Sep-2018
/************************************************************/
bool TheftNotificationTransaction::startKeepActiveTimer(std::shared_ptr<fsm::Signal> signal)
{

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "theft: %s received signal: %s in state %d", __FUNCTION__, signal->ToString().c_str(), GetState());

     if(theftKeepActvdTime > std::chrono::seconds(0))
     {
         m_theftKeepActvd_tick_id = RequestTimeout(theftKeepActvdTime, false);
         MapTransactionId(m_theftKeepActvd_tick_id.GetSharedCopy());
         //to do: print timeid
         DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "theft: Timer timeOut %i s started", m_theftKeepActvd_tick_id);
     }

    SetState(kNotActive);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "theft: notice! %s:state is %d", __FUNCTION__, GetState());


    return true;
}


/************************************************************/
// @brief : startKeepActiveTimer.
// @param[in]  signal,fsm::Signal, which caused transaction to be created.
// @return      True if transaction is not yet finished, false if it is finished.
// @author     Hu Tingting, 3-Sep-2018
/************************************************************/
bool TheftNotificationTransaction::killTheftN(State old_state, State new_state)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "theft: %s: old state is %d, and new state is %d", __FUNCTION__, old_state, new_state);
    //to do kill thread
    Stop();
    return true;
}


/************************************************************/
// @brief : uploadNotActiveToTsp.
// @param[in]  none
// @return      True if transaction is not yet finished, false if it is finished.
// @author     Hu Tingting, 3-Sep-2018
/************************************************************/
bool TheftNotificationTransaction::uploadNotActiveToTsp()
{

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "theft: %s", __FUNCTION__);

    TheftUpload_t theft_upload;

    Timestamp::TimeVal utcTime = GetUtcTime();

    theft_upload.TimeSeconds= utcTime/1000000;
    theft_upload.TimeMilliseconds= (utcTime%1000000)/1000;
    theft_upload.activated = NotActvd;

    m_currTheftSignal->SetTheftPayload(theft_upload);

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "theft: %s\n, upload signal is :%d, second is :%d",
        __FUNCTION__,theft_upload.activated,theft_upload.TimeSeconds);

    fsm::VocmoSignalSource vocmo_signal_source;
    if (vocmo_signal_source.GeelySendMessage(m_currTheftSignal))
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "theft: upload NotActvd signal ok.");

    }
    else
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "theft: upload NotActvd signal fail.");
    }

    SetState(kTeminate);

    return true;
}

} // namespace volvo_on_call

