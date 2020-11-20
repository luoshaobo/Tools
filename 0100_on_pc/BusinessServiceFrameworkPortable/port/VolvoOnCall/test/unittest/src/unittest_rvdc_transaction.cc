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
 *  \file     unittest_rvdc_transaction.cc
 *  \brief    Volvo On Call, unit tests, base transaction
 *  \author   Nie Yujin
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
#include "fs_config_mock.h"

#include <chrono>
#include <functional>
#include <iostream>

#include <sstream>
#include <iomanip>

#include "rvdc/rvdc.h"
#include "voc_framework/signals/internal_signal.h"
#include "voc_framework/signal_sources/vpom_signal_source.h"
#include "voc_framework/signal_sources/vocmo_signal_source.h"
#include "signals/geely/gly_vds_rvdc_signal.h"
#include "powermgr.h"
#include "fake/fake_control.h"

#include "vehicle_comm_types.hpp"
#include "voc_framework/signals/vehicle_comm_signal.h"
#include "voc_framework/signals/ip_command_broker_signal.h"

namespace voc_mta_transaction_test
{

using namespace fsm;
using namespace volvo_on_call;

class RVDC_TEST : public testing::Test 
{
public:
   static void SetUpTestCase() 
   {
      //printf("SetUpTestCase\n");
//      FakeVCProxyControl::Init();
//      FakeVocPersistDataControl::Init();
      trans = new Rvdc();
   }
   static void TearDownTestCase() 
   {
     //printf("TearDownTestCase\n");
     delete trans;
   }
   virtual void SetUp() 
   {   
      //printf("a TEST_F start\n");
   }
   virtual void TearDown()
   {
      //printf("a TEST_F end\n");
   }

   static Rvdc* trans;
};

Rvdc* RVDC_TEST::trans = nullptr;

// test default state

TEST_F(RVDC_TEST, StateTest)
{
   ASSERT_EQ( trans->GetState(), Rvdc::kIdle);
}

TEST_F(RVDC_TEST, HandleNotifyAssignSignal)
{
    IpCommandBrokerTransactionId transaction_id;
    OpRVDCAssignmentNotification_Notification_Data *pData = new OpRVDCAssignmentNotification_Notification_Data();
    pData->maid = 1;
    pData->maversion = 2;
    pData->timestamp.timestamp1 = "abc";
    pData->newstatus = "bcd";
    pData->reason_presence = true;
    pData->reason = "fine";
    Signal::SignalType signal_type = Signal::kRvdcNotifyAssign;
    std::shared_ptr<Signal> signal = fsm::RVDCAssignmentNotificaitonSignal::CreateIpCommandBrokerSignal(*pData, transaction_id, signal_type);
	
    ASSERT_TRUE(trans->HandleNotifyAssignSignal(signal));
}
TEST_F(RVDC_TEST, HandleReqSignal)
{
    IpCommandBrokerTransactionId transaction_id;
    OpRVDCAssignmentNotification_Notification_Data *pData = new OpRVDCAssignmentNotification_Notification_Data();
    pData->maid = 1;
    pData->maversion = 2;
    pData->timestamp.timestamp1 = "abc";
    pData->newstatus = "bcd";
    pData->reason_presence = true;
    pData->reason = "fine";
    Signal::SignalType signal_type = Signal::kRvdcNotifyAssign;
    std::shared_ptr<Signal> signal = fsm::RVDCAssignmentNotificaitonSignal::CreateIpCommandBrokerSignal(*pData, transaction_id, signal_type);
	
    ASSERT_TRUE(trans->HandleReqSignal(signal));
}
TEST_F(RVDC_TEST, HandleNewReqSignal)
{
    IpCommandBrokerTransactionId transaction_id;
    OpRVDCAssignmentNotification_Notification_Data *pData = new OpRVDCAssignmentNotification_Notification_Data();
    pData->maid = 1;
    pData->maversion = 2;
    pData->timestamp.timestamp1 = "abc";
    pData->newstatus = "bcd";
    pData->reason_presence = true;
    pData->reason = "fine";
    Signal::SignalType signal_type = Signal::kRvdcNotifyAssign;
    std::shared_ptr<Signal> signal = fsm::RVDCAssignmentNotificaitonSignal::CreateIpCommandBrokerSignal(*pData, transaction_id, signal_type);
	
    ASSERT_TRUE(trans->HandleNewReqSignal(signal));
}
TEST_F(RVDC_TEST, HandleExecReqSignal)
{
    IpCommandBrokerTransactionId transaction_id;
    OpRVDCAssignmentNotification_Notification_Data *pData = new OpRVDCAssignmentNotification_Notification_Data();
    pData->maid = 1;
    pData->maversion = 2;
    pData->timestamp.timestamp1 = "abc";
    pData->newstatus = "bcd";
    pData->reason_presence = true;
    pData->reason = "fine";
    Signal::SignalType signal_type = Signal::kRvdcNotifyAssign;
    std::shared_ptr<Signal> signal = fsm::RVDCAssignmentNotificaitonSignal::CreateIpCommandBrokerSignal(*pData, transaction_id, signal_type);
	
    ASSERT_TRUE(trans->HandleExecReqSignal(signal));
}
TEST_F(RVDC_TEST, SendMDPUploadReq)
{
    ASSERT_TRUE(trans->SendMDPUploadReq());
}
TEST_F(RVDC_TEST, SendMASyncReq)
{
    OpRVDCMeasurementSynchronization_Request_Data data;
    MA_Data ma;
    ma.maid = 0;
    ma.maversion = 1;
    data.ma_presence = true;
    data.ma.push_back(ma);
    data.storagequota = 0;
    data.maformatversion = 0;
    ASSERT_TRUE(trans->SendMASyncReq(data));
}
TEST_F(RVDC_TEST, SendMASyncRes)
{
    OpRVDCMeasurementSynchronization_Response_Data data;
    MA_Data ma;
    ma.maid = 0;
    ma.maversion = 1;
    data.removemas_presence = true;
    data.removemas.push_back(ma);
    data.newmas_presence = false;
    ASSERT_TRUE(trans->SendMASyncRes(data));
}
TEST_F(RVDC_TEST, HandleReqMASignal)
{
    IpCommandBrokerTransactionId transaction_id;
    OpRVDCMeasurementSynchronization_Request_Data *pData = new OpRVDCMeasurementSynchronization_Request_Data();
    MA_Data ma;
    ma.maid = 0;
    ma.maversion = 1;
    pData->ma_presence = true;
    pData->ma.push_back(ma);
    pData->storagequota = 0;
    pData->maformatversion = 0;
    Signal::SignalType signal_type = Signal::kRvdcReqMA;
    std::shared_ptr<Signal> signal = fsm::RVDCMeasurementSynchronizationRequestSignal::CreateIpCommandBrokerSignal(*pData, transaction_id, signal_type);
	
    ASSERT_TRUE(trans->HandleReqMASignal(signal));
}
TEST_F(RVDC_TEST, HandleUploadMDPSignal)
{
    IpCommandBrokerTransactionId transaction_id;
    OpRVDCMeasurementDataPackage_Request_Data *pData = new OpRVDCMeasurementDataPackage_Request_Data();
    pData->telematicsparameters.timetolive = 0;
    pData->telematicsparameters.uploadpriority = 1;
    pData->payload.mdpformatversion = 0;
    pData->payload.vin = "vin";
    pData->payload.maid = 5;
    pData->payload.maversion = 6;
    pData->payload.sequencenumber = 7;
    pData->payload.timestampreadoutstart.timestamp1 = "timestamp";
    pData->payload.udstriggerid = "udstriggerid";
    pData->payload.diagnosticresponse = "diagnosticresponse";
    Signal::SignalType signal_type = Signal::kRvdcReqMA;
    std::shared_ptr<Signal> signal = fsm::RVDCMeasurementDataPackageSignal::CreateIpCommandBrokerSignal(*pData, transaction_id, signal_type);
	
    ASSERT_TRUE(trans->HandleUploadMDPSignal(signal));
}
TEST_F(RVDC_TEST, HandleReqGPSTimeSignal)
{
    ASSERT_TRUE(trans->HandleReqGPSTimeSignal());
}
TEST_F(RVDC_TEST, HandleReqConnectivityStsSignal)
{
    ASSERT_TRUE(trans->HandleReqConnectivityStsSignal());
}
TEST_F(RVDC_TEST, HandleReqAuthorizationStsSignal)
{
    ASSERT_TRUE(trans->HandleReqAuthorizationStsSignal());
}
#if 0
TEST_F(RVDC_TEST, HandleVDServiceRvcSignal)
{
    fsm::InternalSignalTransactionId transaction_id;
    fsm::RVDC_Data data;
    data.maBusinessType = App_MaBusinessType::app_MaBusinessType_maSynchronization;
    
//    std::shared_ptr<fsm::InternalSignal<fsm::RVDC_Data>> signal =
//    std::make_shared<fsm::InternalSignal<fsm::RVDC_Data>>(transaction_id,
//                                             fsm::Signal::kSubscribeTopicSignal,
//                                             &data );
    GlyVdsRvdcSignal* signal = new GlyVdsRvdcSignal(transaction_id, (void *)(&data));
//    std::shared_ptr<GlyVdsRvdcSignal>(signal);

    ASSERT_TRUE(trans->HandleVDServiceRvcSignal((fsm::Signal)signal));
}


// test force upload flag and uploaded flag
TEST_F(RVDC_TEST, HandleStartUploadMTA)
{
  fsm::InternalSignalTransactionId transaction_id;
  std::shared_ptr<bool> forceUploadMta = std::make_shared<bool>();

  OpRVDCAssignmentNotification_Notification_Data data;
  data.maid = 1;
  data.maversion = 2;
  data.timestamp.timestamp1 = "abc";
  data.newstatus = "bcd";
  data.reason_presence = true;
  data.reason = "fine";
//  std::shared_ptr<OpRVDCAssignmentNotification_Notification_Data> = 
  std::shared_ptr<fsm::InternalSignal<OpRVDCAssignmentNotification_Notification_Data>> configChange =
  std::make_shared<fsm::InternalSignal<OpRVDCAssignmentNotification_Notification_Data>>(transaction_id,
                                             fsm::Signal::kRvdcNotifyAssign,
                                             &data );
  {
    // kStarting -> kRunning
    *forceUploadMta = true; // force upload
    bool ret = trans->HandleNotifyAssignSignal(configChange);
    ASSERT_EQ( ret, true);
  }

  {
    // kStarting -> kStop
    Rvdc trans2;
    *forceUploadMta = false; // not force upload
    FakeVocPersistDataControl::Set_MTA_Uploaded_Flag(1); // have uploaded, so stop it
    trans2.HandleStartUploadMTA(configChange);
    ASSERT_EQ( trans2.GetState(), Rvdc::kStop);
    trans2.Stop();
  }
 
  {
    // kStarting -> kRunning
    Rvdc trans2;
    *forceUploadMta = false; // not force upload
    FakeVocPersistDataControl::Set_MTA_Uploaded_Flag(0); // have not uploaded, so upload
    trans2.HandleStartUploadMTA(configChange);
    ASSERT_EQ( trans2.GetState(), Rvdc::kRunning);
    trans2.Stop();
  }

  {
    // kStarting -> kUpload default value
    Rvdc trans2;
    *forceUploadMta = true; // force upload
    FakeVCProxyControl::Set_Request_VINNumber_Success_Or_Fail(false); // requset fail
    FakeVCProxyControl::Set_Request_GetTcamHwVersion_Success_Or_Fail(false); // requset fail
    trans2.HandleStartUploadMTA(configChange);
    
    std::chrono::milliseconds sleep_time(1000);

    ASSERT_EQ( trans2.GetState(), Rvdc::kStop); // kUpload and then call HandleUploadMTA automatically
    trans2.Stop();
  }
}
#endif

} //namespace
