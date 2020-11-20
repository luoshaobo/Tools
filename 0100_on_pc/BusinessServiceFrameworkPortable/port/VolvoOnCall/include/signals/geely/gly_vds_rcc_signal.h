///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    @file gly_vds_rcc_signal.h
//    geely rcc signal.

// @project        GLY_TCAM
// @subsystem    Application
// @author        uia93888
// @Init date    20-Mar-2019
///////////////////////////////////////////////////////////////////

#ifndef VOC_SIGNAL_GLY_VDS_RCC_SIGNAL_H_
#define VOC_SIGNAL_GLY_VDS_RCC_SIGNAL_H_

#include "signals/signal_types.h"
#include "voc_framework/signals/signal.h"
#include "voc_framework/signals/signal_pack.h"
#include "voc_framework/signals/vdmsg_signal.h"
#include "voc_framework/signals/vdmsg_signal_types.h"
#include "voc_framework/signal_sources/vehicle_comm_signal_source.h"

#include "voc_framework/signal_adapt/app_data.h"

class SignalAdaptBase;

namespace volvo_on_call
{

typedef enum{
    kParkingClimate_StartService     = 0,   // start service
    kParkingClimate_RequestData      = 1,   // the request data
    kParkingClimate_ResponseData     = 2,   // the response data
    kParkingClimate_ExecutionAction  = 4,   // the execution action
    kParkingClimate_TerminateService = 20   // terminate service
}ParkingCliamte_ServiceCmd_E;

typedef struct{
    long serviceId;
    ParkingCliamte_ServiceCmd_E serviceCommand;
    int timerId; //range: 0~49
    int timerActivation; //1: activate, 0: inactivate
    int duration; //default: 15mins
    std::string dayofWeek;
    std::string startTimeofDay; //00:00

    int rec_temp = -1;   //climate temperature
    int rec_Level = -1;  //climate level
    int rec_heat_1 = -1;   //include seat and steer wheel heating
    int rec_heat_2 = -1;
}Parking_ClimateRequest;

typedef struct{
    bool operationSuccessed;
    long errorCode;
    long vehicleErrorCode = -1;
    std::string message;
    
    bool preClimateActive;  //true: on, false: off
    
} Parking_ClimateResult;


class GlyVdsRccSignal: public fsm::VdmSignal, public fsm::SignalPack
{
public:
    ////////////////////////////////////////////////////////////
    // @brief :Constructs signal.
    // @param[in]  VdServiceTransactionId, vds transactionid.
    // @param[in]  vdsService, vds msg.
    // @return     
    // @author     uia93888, 20-Mar-2019
    ////////////////////////////////////////////////////////////
    GlyVdsRccSignal(fsm::VdServiceTransactionId& transaction_id, void* vdsService);

    ////////////////////////////////////////////////////////////
    // @brief : create rcc signal
    // @param[in]  VdServiceTransactionId, vds transactionid.
    // @param[in]  vdsService, vds msg.
    // @return     
    // @author     uia93888, 20-Mar-2019
    ////////////////////////////////////////////////////////////
    static std::shared_ptr<fsm::Signal> CreateGlyVdsRccSignal(fsm::VdServiceTransactionId& transaction_id, void* vdsService);

    ~GlyVdsRccSignal();
    
    std::string ToString(){ return "GlyVdsRccSignal"; }

    ////////////////////////////////////////////////////////////
    // @brief : get signal pointer
    // @return     
    // @author     uia93888, 20-Mar-2019
    ////////////////////////////////////////////////////////////
    fsm::Signal* GetSignal()
    {
        return this;
    }

    ////////////////////////////////////////////////////////////
    // @brief :PackGeelyAsn
    // @param       void* vds request
    // @return     fsm::Signal *
    // @author     uia93888, 20-Mar-2019
    ////////////////////////////////////////////////////////////
    virtual int PackGeelyAsn(void *vdServiceRequest);
    
    ////////////////////////////////////////////////////////////
    // @brief :pack ResServiceResult_t into ASN1C structure.
    // @param[in]  response, ResServiceResult_t structure .
    //
    // @return     True if successfully set, false otherwise
    // @author     uia93888, 20-Mar-2019
    ////////////////////////////////////////////////////////////
    bool SetResultPayload(Parking_ClimateResult result);
    
    ////////////////////////////////////////////////////////////
    // @brief :pack RequestHeader.
    // @return     True if successfully set, false otherwise
    // @author     uia93888, 20-Mar-2019
    ////////////////////////////////////////////////////////////
    bool SetHeadPayload();
    
    ////////////////////////////////////////////////////////////
    // @brief :get packed ASN1C structure.
    // @return     pointer to asn1c struct containing payload, caller must free
    // @author     uia93888, 20-Mar-2019
    ////////////////////////////////////////////////////////////
    void *GetPackPayload()
    {
        return (void *)NULL;
    }
    
    ////////////////////////////////////////////////////////////
    // @brief :get unpacked ASN1C structure.
    // @return     unpacked structure
    // @author     uia93888, 20-Mar-2019
    ////////////////////////////////////////////////////////////
    std::shared_ptr<Parking_ClimateRequest> GetRequestPayload();    

private:
    ////////////////////////////////////////////////////////////
    // @brief :unpack ASN1C structure.
    // @return     True if successfully set, false otherwise
    // @author     uia93888, 20-Mar-2019
    ////////////////////////////////////////////////////////////
    bool UnpackPayload(void* vdsServiceRequest);
    
private:
    std::shared_ptr<fsm::Parking_ClimateRequest_t> m_pRequest;
    std::shared_ptr<fsm::Parking_CliamteStrtStopResult_t> m_pStrtStopResult;
    std::shared_ptr<fsm::Parking_ErrorResult_t> m_pErrResult;
    std::shared_ptr<Parking_ClimateRequest> m_pLocalRequest;
};

}

#endif //VOC_SIGNAL_GLY_VDS_RCC_SIGNAL_H_