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
 *  \file     unittest_mta_transaction.cc
 *  \brief    Volvo On Call, unit tests, base transaction
 *  \author   Jianhui Li
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

#include "mta/mta_transaction.h"
#include "voc_framework/signals/internal_signal.h"
#include "voc_framework/signal_sources/vpom_signal_source.h"
#include "voc_framework/signal_sources/vocmo_signal_source.h"
#include "signals/geely/gly_vds_mta_signal.h"
#include "powermgr.h"
#include "fake/fake_control.h"

#include "vehicle_comm_types.hpp"
#include "voc_framework/signals/vehicle_comm_signal.h"

namespace voc_mta_transaction_test
{

using namespace fsm;
using namespace volvo_on_call;

class MTA_TEST : public testing::Test 
{
public:
   static void SetUpTestCase() 
   {
      //printf("SetUpTestCase\n");
      FakeVCProxyControl::Init();
      FakeVocPersistDataControl::Init();
      trans = new MTATransaction();
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

   static MTATransaction* trans;
};

MTATransaction* MTA_TEST::trans = nullptr;

// test default state
TEST_F(MTA_TEST, StateTest)
{
   ASSERT_EQ( trans->GetState(), MTATransaction::kStart);
}

// // test force upload flag and uploaded flag
// TEST_F(MTA_TEST, HandleStartUploadMTA)
// {
  // fsm::InternalSignalTransactionId transaction_id;
  // std::shared_ptr<bool> forceUploadMta = std::make_shared<bool>();
 
  // std::shared_ptr<fsm::InternalSignal<bool>> configChange =
  // std::make_shared<fsm::InternalSignal<bool>>(transaction_id,
                                             // fsm::Signal::kSubscribeTopicSignal,
                                             // forceUploadMta );
  // {
    // // kStart -> kCollection
    // *forceUploadMta = true; // force upload
    // trans->HandleStartUploadMTA(configChange);
    // ASSERT_EQ( trans->GetState(), MTATransaction::kCollectData);
  // }

  // {
    // // kStart -> kStop
    // MTATransaction trans2;
    // *forceUploadMta = false; // not force upload
    // FakeVocPersistDataControl::Set_MTA_Uploaded_Flag(1); // have uploaded, so stop it
    // trans2.HandleStartUploadMTA(configChange);
    // ASSERT_EQ( trans2.GetState(), 0);
    // trans2.Stop();
  // }
 
  // {
    // // kStart -> kCollection
    // MTATransaction trans2;
    // *forceUploadMta = false; // not force upload
    // FakeVocPersistDataControl::Set_MTA_Uploaded_Flag(0); // have not uploaded, so upload
    // trans2.HandleStartUploadMTA(configChange);
    // ASSERT_EQ( trans2.GetState(), MTATransaction::kCollectData);
    // trans2.Stop();
  // }

  // {
    // // kStart -> kUpload default value
    // MTATransaction trans2;
    // *forceUploadMta = true; // force upload
    // FakeVCProxyControl::Set_Request_VINNumber_Success_Or_Fail(false); // requset fail
    // FakeVCProxyControl::Set_Request_GetTcamHwVersion_Success_Or_Fail(false); // requset fail
    // trans2.HandleStartUploadMTA(configChange);
    
    // std::chrono::milliseconds sleep_time(1000);

    // ASSERT_EQ( trans2.GetState(), MTATransaction::kStop); // kUpload and then call HandleUploadMTA automatically
    // trans2.Stop();

  // }
// }

// // test RequestVinNumber
// TEST_F(MTA_TEST, RequestVinNumber)
// {
   // {
     // FakeVCProxyControl::Set_Request_VINNumber_Success_Or_Fail(false); // requset fail
     // MTATransaction trans2;
     // ASSERT_FALSE(trans2.RequestVinNumber());
     // trans2.Stop();
   // }

   // {
     // FakeVCProxyControl::Set_Request_VINNumber_Success_Or_Fail(true); // requset fail
     // MTATransaction trans2;
     // ASSERT_TRUE(trans2.RequestVinNumber());
     // trans2.Stop();
   // }

   // ASSERT_EQ( trans->GetState(), MTATransaction::kCollectData);
// }

// // test RequesTcamHwVersion
// TEST_F(MTA_TEST, RequesTcamHwVersion)
// {
   // {
     // FakeVCProxyControl::Set_Request_GetTcamHwVersion_Success_Or_Fail(false); // requset fail
     // MTATransaction trans2;
     // ASSERT_FALSE(trans2.RequesTcamHwVersion());
     // trans2.Stop();
   // }
   // {
     // FakeVCProxyControl::Set_Request_GetTcamHwVersion_Success_Or_Fail(true); // requset fail
     // MTATransaction trans2;
     // ASSERT_TRUE(trans2.RequesTcamHwVersion());
     // trans2.Stop();
   // }
// }

// // test HandleVINNumber
// TEST_F(MTA_TEST, HandleVINNumber)
// {
  // ASSERT_TRUE(trans->HandleVINNumber(nullptr));
  // vc::ResVINNumber* obj = new vc::ResVINNumber();
  // strcpy(obj->vin_number,"12345678901234567");
  // VehicleCommTransactionId transaction_id(0x11);
  // vc::ReturnValue status;
  // status = vc::RET_OK; 
  // VINNumberSignal* sig = new VINNumberSignal(Signal::kVINNumberSignal, transaction_id, *obj, status);
  // std::shared_ptr<Signal> signal = std::shared_ptr<Signal>(sig);
  // ASSERT_TRUE(trans->HandleVINNumber(signal));

  // ASSERT_EQ(trans->GetState(), MTATransaction::kCollectData);
// }

// // test HandleGetTcamHwVer & and upload
// TEST_F(MTA_TEST, HandleGetTcamHwVer)
// {
  // {
     // MTATransaction trans2;
     // ASSERT_TRUE(trans2.HandleGetTcamHwVer(nullptr));
  // }

  // vc::ResGetTcamHwVer* obj = new vc::ResGetTcamHwVer();
  // obj->eol_volvo_ecu_serial_number[0]=10;
  // obj->eol_volvo_ecu_serial_number[1]=11;
  // obj->eol_volvo_ecu_serial_number[2]=12;
  // obj->eol_volvo_ecu_serial_number[3]=13;

  // VehicleCommTransactionId transaction_id(0x22);
  // vc::ReturnValue status;
  // status = vc::RET_OK; 
  // TcamHwVerSignal* sig = new TcamHwVerSignal(Signal::kTcamHwVerSignal, transaction_id, *obj, status);
  // std::shared_ptr<Signal> signal = std::shared_ptr<Signal>(sig);
  // ASSERT_TRUE(trans->HandleGetTcamHwVer(signal));
  
  // std::chrono::milliseconds sleep_time(50); // kUpload and then call HandleUploadMTA automatically

  // ASSERT_EQ(trans->GetState(), MTATransaction::kStop); 
// }

// test HandleGetTcamHwVer & and upload
TEST_F(MTA_TEST, HandleStopMTAService)
{
   ASSERT_TRUE(trans->HandleStopMTAService());
}

} //namespace
