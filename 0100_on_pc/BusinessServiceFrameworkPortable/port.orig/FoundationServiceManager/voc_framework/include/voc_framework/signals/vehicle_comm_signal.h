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
 *  \file     vehicle_comm_signal.h
 *  \brief    Signals from Vehicle Comm Client
 *  \author   Florian Schindler
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_FRAMEWORK_SIGNALS_VC_SIGNAL_H_
#define VOC_FRAMEWORK_SIGNALS_VC_SIGNAL_H_

#include "voc_framework/signals/signal.h"
#include "vehicle_comm_interface.hpp"  // for vc::ReturnValue

#include "dlt/dlt.h"

#include <memory>  // std::shared_ptr

DLT_IMPORT_CONTEXT(dlt_libfsm);


namespace fsm
{


//ToDo: This is test code only, until we get a proper message from VehicleComm
typedef struct
{
    bool confirmation;  //! true if 6 digit code was confirmed, false if not confirmed.
}  ResCodeConfirmationByUserNotify;


//ToDo: This is test code only, until we get a proper message from VehicleComm
typedef struct
{
    bool role_admin;  //! true if role is "admin", false if role is "user"
}  ResRoleSelectedNotify;


/*!
* \class VehicleCommSignalExt
* \brief Implements a signal coming from Vehicle Comm Service
*/
template<typename T> class VehicleCommSignalExt : public Signal
{
    protected:

        std::shared_ptr<T> data_ = NULL;  //!< contains data from Vehicle Comm (=payload)
        vc::ReturnValue status_ = vc::RET_ERR_EXTERNAL;  //!< default status value; be pessimistic
        std::string name_;  //!< name of message

    public:

        /*!
         * \fn     VehicleCommSignalExt
         * \brief  constructor
         *
         * \author Florian Schindler
         *
         * \param[in] signal_type  signal type, see \ref SignalType
         * \param[in] transaction_id  a unique transaction ID created using class \ref VCTransactionId
         * \param[in] data  payload provided by Vehicle Comm, which is copied into the signal and forwarded to the recipients
         * \param[in] status  status as provided by Vehicle Comm, which is copied and forwarded to the recipients
        */
        VehicleCommSignalExt<T>(SignalType signal_type, VehicleCommTransactionId& transaction_id, T data,
            vc::ReturnValue status) :
            Signal(transaction_id, signal_type), data_(std::make_shared<T>(data)), status_(status)
        {
            switch (signal_type)
            {
                case Signal::kCarMode:
                {
                    name_ = "kCarMode";
                    break;
                }
                case Signal::kHornNLight:
                {
                    name_ = "kHornNLight";
                    break;
                }
                case Signal::kCarUsageMode:
                {
                    name_ = "kCarUsageMode";
                    break;
                }
                case Signal::kCodeConfirmationByUserNotify:
                {
                    name_ = "kCodeConfirmationByUserNotify";
                    break;
                }
                case Signal::kEndOfTrip:
                {
                    name_ = "kEndOfTrip";
                    break;
                }
                case Signal::kPairingVisibilityRequest:
                {
                    name_ = "kPairingVisibilityRequest";
                    break;
                }
                case Signal::kRoleSelectedNotify:
                {
                    name_ = "kRoleSelectedNotify";
                    break;
                }
                //uia93888 remote start engine
                case Signal::kRMTEngineSecurityRandom:{
                    name_ = "kRMTEngineSecurityRandom";
                    break;
                }
                case Signal::kRMTEngineSecurityResult:{
                    name_ = "kRMTEngineSecurityResult";
                    break;
                }
                case Signal::kRMTEngineSignal:
                {
                    name_ = "kRMTEngineSignal";
                    break;
                }
                case Signal::kRMTCarStatesSignal:
                {
                    name_ = "kRMTCarStateSignal";
                    break;
                }
                case Signal::kVINNumberSignal:
                {
                    name_ = "kVINNumberSignal";
                    break;
                }
                case Signal::kTcamHwVerSignal:
                {
                    name_ = "kTcamHwVerSignal";
                    break;
                }
                case Signal::kElectEngLvlSignal:
                {
                    name_ = "kElectEngLvlSignal";
                    break;
                }
                case Signal::kDelayEngineRinningtimeSignal:
                {
                    name_ = "kDelayEngineRinningtimeSignal";
                    break;
                }
                case Signal::kRemoteCliamteSignal:
                {
                    name_ = "kRemoteCliamteSignal";
                    break;
                }
                case Signal::kRemoteSeatHeatSignal:{
                    name_ = "kRemoteSeatHeatSignal";
                    break;
                }
                case Signal::kRemoteSeatVentiSignal:
                {
                    name_ = "kRemoteSeatVentiSignal";
                    break;
                }
                case Signal::kRemoteSteerWhlHeatSignal:{
                    name_ = "kRemoteSteerWhlHeatSignal";
                    break;
                }
                case Signal::kParkingClimateOperSignal:{
                    name_ = "kParkingClimateOperSignal";
                    break;
                }
                default:
                {
                    name_ = "unknown VehicleCommSignal";
                    break;
                }
            }
        };

        /*!
         * \fn     GetData
         * \brief  returns payload structure as provided by VehicleComm
         *
         * \return  pointer to std::shared_ptr<vc::MessageBase>
        */
        virtual std::shared_ptr<T> GetData() { return data_; };

        #ifdef ENABLE_SIGNAL_INJECTION
            /*!
             * \fn     SetData
             * \brief  sets payload structure
             *
             * \param[in]  pointer to data
            */
            virtual void SetData(const T &data) { data_ = std::make_shared<T>(data); };
        #endif


        /*!
         * \fn     ToString
         * \brief  returns signal name (see \ref SignalType)
         *
         * \return  name as a string
        */
        virtual std::string ToString() { return name_; };

        /*!
         * \fn     GetVcStatus
         * \brief  returns returned \ref vc::ReturnValue from Vehicle Comm Client. For events it is always RET_OK.
         *
         * \return  status_; for details see \ref VCRetVal in file vehicle_comm_client.hpp
        */
        virtual const vc::ReturnValue GetVcStatus() { return status_; };

    private:
};

typedef class VehicleCommSignalExt<vc::ResCarMode>  CarModeSignal;
typedef class VehicleCommSignalExt<vc::ResHornNLight>  HornNLightSignal;
typedef class VehicleCommSignalExt<vc::ResCarUsageMode>  CarUsageModeSignal;
typedef class VehicleCommSignalExt<ResCodeConfirmationByUserNotify>  CodeConfirmationByUserNotifySignal;  //ToDo: replace with proper strucrure from VehicleComm, when available!
typedef class VehicleCommSignalExt<vc::Empty>  EndOfTripSignal;
typedef class VehicleCommSignalExt<ResRoleSelectedNotify>  RoleSelectedSignal;  //ToDo: replace with proper strucrure from VehicleComm, when available!
typedef class VehicleCommSignalExt<vc::Empty>  PairingVisibilityRequestSignal;
#if 1 // nieyj
typedef class VehicleCommSignalExt<vc::EventDoorLockState>  DoorLockStatusSignal;
typedef class VehicleCommSignalExt<vc::EventWinOpenState>  WinOpenStatusSignal;
typedef class VehicleCommSignalExt<vc::EventPMLevelState>  PMLvlStatusSignal;
typedef class VehicleCommSignalExt<vc::ResVFCActivate>  VFCActivateSignal;
#endif
//uia93888 rmote start engine
typedef class VehicleCommSignalExt<vc::Res_RMTEngineSecurityRandom> RMTEngineSecurityRandomSignal; //Vuc --> APP
typedef class VehicleCommSignalExt<vc::Res_RMTEngineSecurityResult> RMTEngineSercurityResultSignal; // Vuc --> APP
typedef class VehicleCommSignalExt<vc::Res_RMTEngine>  RMTEngineSignal;  //Vuc --> APP
typedef class VehicleCommSignalExt<vc::ResRMTStatus>  RMTCarStatesSignal; // data storage ---> RMT_Engine
typedef class VehicleCommSignalExt<vc::ResElectEngLvl> ElectEngLvlSignal;  // date storage --> Remote Start basic
typedef class VehicleCommSignalExt<vc::Res_DelayEngRunngTime> DelayEngRunngTimeSignal; //delay engine eunning time
typedef class VehicleCommSignalExt<vc::Res_OperateRMTClimate> OperateRMTClimateSignal; //remote climate start/stop response
typedef class VehicleCommSignalExt<vc::Res_OperateRMTSeatHeat_S> OperateRMTSeatHeatSignal; //remote seat heat start/stop response
typedef class VehicleCommSignalExt<vc::Res_OperateRMTSeatVenti_S> OperateRMTSeatVentiSignal; //remote seat ventilation start/stop response
typedef class VehicleCommSignalExt<vc::Res_OperateRMTSteerWhlHeat_S> OperateRMTSteerWhlHeatSignal; //remote steer wheel heat start/stop response
typedef class VehicleCommSignalExt<vc::Res_ParkingClimateOper_S> ParkingClimateOperSignal; //parking climate start/stop response

//uia93888 add end
typedef class VehicleCommSignalExt<vc::ResGetStolenVehicleTracking>  StolenVehicleTrackingSignal;

typedef class VehicleCommSignalExt<vc::ResVINNumber>  VINNumberSignal;
typedef class VehicleCommSignalExt<vc::ResGetTcamHwVer>  TcamHwVerSignal;

} // namespace fsm

#endif  // VOC_FRAMEWORK_SIGNALS_VC_SIGNAL_H_

/** \}    end of addtogroup */
