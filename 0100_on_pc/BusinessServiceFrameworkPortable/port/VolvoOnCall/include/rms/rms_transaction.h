////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  @file rms_transcation.h
//  This file handles the communication with the VuC, using the OTP VCS-interface
 
// @project     GLY_TCAM
// @subsystem   VolvoOnCall
// @author      Jiaojiao Shen
// @Init date   12-Sep-2018
////////////////////////////////////////////////////////////////////////////

#ifndef VOC_TRANSACTIONS_RMS_TRANSACTION_H_
#define VOC_TRANSACTIONS_RMS_TRANSACTION_H_

#include "voc_framework/transactions/smart_transaction.h"
#include "timestamp.h"
#include <rms/dbus_interface/xEVDBusStuctures.h>

namespace volvo_on_call
{

class RmsTransaction: public fsm::SmartTransaction
{
public:
    RmsTransaction ();
    ~RmsTransaction ();

    class StartSignal : public fsm::Signal
    {
    public:
        StartSignal(): fsm::Signal(fsm::CCMTransactionId(), sStart)
        {
        }
        std::string ToString()
        {
            return "StartSignal";
        }
    };

private:
    // State Machine
    enum States
    {
        kStart = 0,
        kCollectData,
        kStop
    };

    // internal Signal
    enum RmsSignalTypes
    {
        sInit = 600,        // keep this one first
        sStart,
        sStop,
        sMAX                // keep this one last
    };

    bool handleCollectTelmDshbData(std::shared_ptr<fsm::Signal> signal);
    bool handleSetConnectionParams(std::shared_ptr<fsm::Signal> signal);
    bool handleSetConfigurationParam(std::shared_ptr<fsm::Signal> signal);
    bool handleSetPeriodicDBUSData(std::shared_ptr<fsm::Signal> signal);
    bool handleStart(std::shared_ptr<fsm::Signal> signal);
    bool handleStop(std::shared_ptr<fsm::Signal> signal);
    bool HandleUploadData(std::shared_ptr<fsm::Signal> signal);
    bool HandleTimeOut(std::shared_ptr<fsm::Signal> signal);

private:
    std::shared_ptr<PeriodicDBUSData> mLastUploadData; 

    fsm::TimeoutTransactionId m_RmsCycleUploadTimerId;

    bool isLoginStart;
    bool iswarningOccur;
    bool isRMSDataSignal;
    bool isBattVoltageSignal;
    bool isBattTempSignal;
    bool isBattCodSignal;
    bool isVINInfoSet;
    bool isICCIDInfoSet;
};

} // namespace volvo_on_call

#endif //VOC_TRANSACTIONS_RMS_TRANSACTION_H_
