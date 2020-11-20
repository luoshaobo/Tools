///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental AG and subsidiaries
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// @file gly_vds_theft_signal.h
// geely theft signal.

// @project       GLY_TCAM
// @subsystem     Application
// @author        Nie Yujin
// @Init date     20-Sep-2018
///////////////////////////////////////////////////////////////////

#ifndef GLY_VDS_REMOTECTRL_BASIC_SIGNAL_H_
#define GLY_VDS_REMOTECTRL_BASIC_SIGNAL_H_

#include "signals/signal_types.h"
#include "voc_framework/signals/signal.h"
#include "voc_framework/signals/signal_pack.h"
#include "voc_framework/signals/vdmsg_signal.h"
#include "voc_framework/signals/vdmsg_signal_types.h"

#include "asn.h"

#include <memory>

namespace volvo_on_call
{
    #define CMPMTAIRPMLVL_INVALID   -1
    enum RHLType
    {
        None = 0, ///< Newly created
        Honk,
        Flash,
        HonkAndFlash
    };

    enum RWSType
    {
        //    None = 0, ///< Newly created
        CloseWindowOnly = 1,
        CloseSunroofOnly,
        CloseBoth,
        OpenWindowOnly,
        OpenSunroofOnly,
        OpenBoth,
        OpenSunshade,
        CloseSunshade,
        Ventilate
    };

    enum DoorOpenCloseStatusType
    {
        DoorStsUnknown = 0,
        DoorStsOpen,
        DoorStsClose
    };

    enum WinOpenCloseStatusType
    {
        WinStsUnknown = 0,
        WinStsOpened,
        WinStsClosed,
        WinStsRaised,
        WinStsFullyOpend
    };

    typedef struct 
    {
        long serviceId;
        long serviceCommand;
        long timeStart;
        long timeWindow;
        int  value;

    } RemoteCtrlBasicRequest_t;

    typedef struct {
        long latitude;
        long longitude;
        long altitude;
        int posCanBeTrusted; //BOOLEAN_t
        int carLocatorStatUploadEn; //BOOLEAN_t
        int marsCoordinates; //BOOLEAN_t
    } RemoteCtrlPosition_t;

    typedef struct 
    {
        RemoteCtrlPosition_t position;
        long vehSpdIndcd;       //kmph
        long vehSpdIndcdQly;    //0x0:valid; 0x1:Invalid
    
        long doorOpenStatusDriver;
        long doorOpenStatusPassenger;
        long doorOpenStatusDriverRear;
        long doorOpenStatusPassengerRear;

        long doorLockStatusDriver;
        long doorLockStatusPassenger;
        long doorLockStatusDriverRear;
        long doorLockStatusPassengerRear;

        long trunkOpenStatus;
        long trunkLockStatus;

        long centralLockingStatus;
        long hoodSts;         //0 Ukwn; 1 Opend; 2 Clsd
        long lockgCenStsForUsrFb; //0 LockStsUkwn; 1 Unlock; 2 Lockd; 3 SafeLockd

        long winStatusDriver;
        long winStatusPassenger;
        long winStatusDriverRear;
        long winStatusPassengerRear;

        long sunroofOpenStatus;

        long winPosDriver;
        long winPosPassenger;
        long winPosDriverRear;
        long winPosPassengerRear;

        long ventilateStatus;

        long interiorPM25;
        long exteriorPM25;
    } RemoteCtrlBasicStatus_t;
    
    typedef struct 
    {
        int operationSucceeded;
        long errorCode;
        long vehicleErrorCode;
        std::string message;

    } RemoteCtrlBasicResult_t;

    class GlyVdsRemoteCtrlBasicSignal: public fsm::VdmSignal, public fsm::SignalPack
    {

    public:

        /************************************************************/
        // @brief :Constructs function.
        // @param[in]  VdServiceTransactionId, vds transactionid.
        // @param[in]  vdsService, vds msg.
        // @return
        // @author     Nie Yujin
        /************************************************************/
        GlyVdsRemoteCtrlBasicSignal (fsm::VdServiceTransactionId& transaction_id,VDServiceRequest* vdsService, const SignalType signal_type);

        /************************************************************/
        // @brief :create rdl signal.
        // @param[in]  VdServiceTransactionId, vds transactionid.
        // @param[in]  vdsService, vds msg.
        // @return
        // @author     Nie Yujin
        /************************************************************/  
        static std::shared_ptr<fsm::Signal> CreateGlyVdsRDLSignal (fsm::VdServiceTransactionId& transaction_id,VDServiceRequest* vdsService);

        /************************************************************/
        // @brief :create rdu signal.
        // @param[in]  VdServiceTransactionId, vds transactionid.
        // @param[in]  vdsService, vds msg.
        // @return
        // @author     Nie Yujin
        /************************************************************/  
        static std::shared_ptr<fsm::Signal> CreateGlyVdsRDUSignal (fsm::VdServiceTransactionId& transaction_id,VDServiceRequest* vdsService);

        /************************************************************/
        // @brief :create rtl signal.
        // @param[in]  VdServiceTransactionId, vds transactionid.
        // @param[in]  vdsService, vds msg.
        // @return
        // @author     Nie Yujin
        /************************************************************/  
        static std::shared_ptr<fsm::Signal> CreateGlyVdsRTLSignal (fsm::VdServiceTransactionId& transaction_id,VDServiceRequest* vdsService);

        /************************************************************/
        // @brief :create rtu signal.
        // @param[in]  VdServiceTransactionId, vds transactionid.
        // @param[in]  vdsService, vds msg.
        // @return
        // @author     Nie Yujin
        /************************************************************/  
        static std::shared_ptr<fsm::Signal> CreateGlyVdsRTUSignal (fsm::VdServiceTransactionId& transaction_id,VDServiceRequest* vdsService);

        /************************************************************/
        // @brief :create rws signal.
        // @param[in]  VdServiceTransactionId, vds transactionid.
        // @param[in]  vdsService, vds msg.
        // @return
        // @author     Nie Yujin
        /************************************************************/  
        static std::shared_ptr<fsm::Signal> CreateGlyVdsRWSSignal (fsm::VdServiceTransactionId& transaction_id,VDServiceRequest* vdsService);

        /************************************************************/
        // @brief :create rpp signal.
        // @param[in]  VdServiceTransactionId, vds transactionid.
        // @param[in]  vdsService, vds msg.
        // @return
        // @author     Nie Yujin
        /************************************************************/  
        static std::shared_ptr<fsm::Signal> CreateGlyVdsRPPSignal (fsm::VdServiceTransactionId& transaction_id,VDServiceRequest* vdsService);

        /************************************************************/
        // @brief :create rhl signal.
        // @param[in]  VdServiceTransactionId, vds transactionid.
        // @param[in]  vdsService, vds msg.
        // @return
        // @author     Nie Yujin
        /************************************************************/  
        static std::shared_ptr<fsm::Signal> CreateGlyVdsRHLSignal (fsm::VdServiceTransactionId& transaction_id,VDServiceRequest* vdsService);

        /************************************************************/
        // @brief :destruct function.
        // @param[in]  none
        // @return
        // @author     Nie Yujin
        /************************************************************/
        ~GlyVdsRemoteCtrlBasicSignal ();

        /**
        * \brief String identifier for printouts
        */
        std::string ToString() {return "GlyVdsRemoteCtrlBasicSignal"; }

        /************************************************************/
        // @brief :pack RemoteCtrlBasicResult_t into ASN1C structure. upload body msg.
        // @param[in]  response, RemoteCtrlBasicResult_t structure .
        // @return     True if successfully set, false otherwise
        // @author     Nie Yujin
        /************************************************************/  
        bool SetResultPayload(RemoteCtrlBasicResult_t response);

        /************************************************************/
        // @brief :pack RemoteCtrlBasicStatus_t into ASN1C structure. upload body msg.
        // @param[in]  response, RemoteCtrlBasicStatus_t structure .
        // @return     True if successfully set, false otherwise
        // @author     Nie Yujin
        /************************************************************/ 
        bool SetDoorStatusPayload(RemoteCtrlBasicResult_t result, RemoteCtrlBasicStatus_t response);
        bool SetWinStatusPayload(RemoteCtrlBasicResult_t result, RemoteCtrlBasicStatus_t response);
        bool SetPollutionStatusPayload(RemoteCtrlBasicResult_t result, RemoteCtrlBasicStatus_t response);
        /************************************************************/
        // @brief :pack RequestHeader.upload head msg.
        // @param[in]  
        // @return     True if successfully set, false otherwise
        // @author     Nie Yujin
        /************************************************************/ 
        bool SetHeadPayload();

        /************************************************************/
        // @brief :get packed ASN1C structure. Default encode upload func.
        // @return     pointer to asn1c struct containing payload, caller must free
        // @author     Nie Yujin
        /************************************************************/
        void *GetPackPayload()
        {
            return (void *)m_vdsResponse;
        }

        fsm::Signal *GetSignal()
        {
    	    return this;
        }
        /************************************************************/
        // @brief :get unpacked ASN1C structure.used to return ack msg with own struct.
        // @return     unpacked structure
        // @author     Nie Yujin
        /************************************************************/
        std::shared_ptr<RemoteCtrlBasicRequest_t> GetRequestPayload()
        {
            return m_rcbRequest;
        }

    private:
        /************************************************************/
        // @brief :unpack ASN1C structure.Currently used to receive ack msg.
        // @return     True if successfully set, false otherwise
        // @author     Nie Yujin
        /************************************************************/
        bool UnpackPayload();

        bool CreateBody(VDServiceRequest* req);
        bool CreateServiceData(VDServiceRequest* req);
        bool CreateVehicleStatus(VDServiceRequest* req);
        bool CreateBasicVehicleStatus(VehicleStatus* vehicleStatus);
        bool CreateAdditionalVehicleStatus(VehicleStatus* vehicleStatus);
        bool CreateDrivingSafetyStatus(AdditionalVehicleStatus* additionalVehicleStatus);
        bool CreateClimateStatus(AdditionalVehicleStatus* additionalVehicleStatus);
        bool CreatePollutionStatus(AdditionalVehicleStatus* additionalVehicleStatus);
        bool SetBasicVehicleStatus(BasicVehicleStatus* basicVehicleStatus, RemoteCtrlBasicStatus_t response);
        bool SetDoorsStatus(DrivingSafetyStatus* drivingSafetyStatus, RemoteCtrlBasicStatus_t response);
        bool SetWinStatus(ClimateStatus* climateStatus, RemoteCtrlBasicStatus_t response);
        bool SetPollutionStatus(PollutionStatus* pollutionStatus, RemoteCtrlBasicStatus_t response);
        bool CheckFieldsInAsnScope_long(long inputValue, long minValue, long maxValue);
        bool CheckFieldsInAsnScope_int(int inputValue, int minValue, int maxValue);

    private:

        VDServiceRequest* m_vdsService;        //fsm asn1 signal VDServiceRequest
        VDServiceRequest* m_vdsResponse;       //upload signal,default
        std::shared_ptr<RemoteCtrlBasicRequest_t> m_rcbRequest; //change VDServiceRequest to own struct

    };

} // namespace volvo_on_call

#endif //GLY_VDS_REMOTECTRL_BASIC_SIGNAL_H_

