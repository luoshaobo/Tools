/**
 * Copyright (C) 2016-2018 Continental Automotive GmbH
 *
 * Licensed under the "Supplier unrestricted white box" category, as
 * described in the 20160322-1_SW-amendment TCAM.pdf agreement
 * between Volvo Cars and Continental Automotive GmbH.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *  \file     signal.h
 *  \brief    VOC Service signal base class.
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_FRAMEWORK_SIGNALS_SIGNAL_H_
#define VOC_FRAMEWORK_SIGNALS_SIGNAL_H_

#include "voc_framework/transactions/transaction_id.h"

#include <memory>
#include <string>

#include "usermanager_types.h"

namespace fsm
{

class Signal
{
    /**
     * \brief Grants special access rights to the Car Access signal source class
     */
    friend class CarAccessSignalSource;
    /**
     * \brief Grants special access rights to SignalFactory.
     */
    friend class SignalFactory;
    /**
     * \brief Grants special access rights to CcmCodec.
     */
    friend class CcmCodec;

   
 public:
    /**
     * \brief Undefined priority, used for describing unknown MQTT topic priority
     */
    
    static const int kPriorityUndefined = -1;

    /**
     * \brief Returns priority of the MQTT topic the message came on.
     *
     * \return Priority of the MQTT topic the message came on. If the message
     *         did not arrive on any MQTT topic, or for some reason the topic
     *         priority was unknown, kPriorityUndefined is returned
     */
    int GetTopicPriority() {return topic_priority;} ;

    /**
     * \brief Returns the UserId of the sender of the message.
     *
     * \return UserId of the sender of the message. //TODO: Or kUSerIdUndefined if sender is not known.
     */
    //TODO: move into legacy signals
    virtual fsm::UserId GetSender() {return sender_;};

    /**
     * \brief TransactionId of the transaction that the signal belongs to
     * \return TransactionId of the signal
     */
    const TransactionId& GetTransactionId ();

    
    virtual ~Signal() {} //forces polymorphism

    /**
     * \brief Serialize to string, used for debug printouts. Must be implemented by subclasses.
     * \return Name or other text identifier of the signal
     */
    virtual std::string ToString() = 0;

    /**
     * \brief Signal type is defined as an integer to provide extensibility. There is a number
     *        of basic types (see BasicSignalTypes) defined in the library, but the application
     *        can add own types if needed.
     */
    typedef int SignalType;

    /**
     * \brief Enumeration of basic signal types. Basic signal types are types for which the implementation
     *        is provided in the library. Every application can extend this list and register own signal types.
     *        The basic types are mapped to numbers starting at 10000, the number space 0..9999 is reserved
     *        for the application specific types. Application specific signals are primarily CCM messages/signals,
     *        other signals (e.g. VehicleComm) require extensions of the unlying modules and are therefore provided
     *        in the library.
     */
    enum BasicSignalTypes
    {
        kTypeUndefined = 10000, //to make it possible to the apps to have numbering starting from 0
        kCarAccessSignal,
        kAssistanceCall,     //*< assistance call
        kEntryPointSignal,  //*< entry point
        kFeaturesSignal,  //*< feature list
        kCarMode,  //*< VehicleComm vc::RES_CARMODE
        kHornNLight,  //*< VehicleComm vc::RES_HORNNLIGHT
        kCarUsageMode,  //*<  VehicleComml vc::RES_CARUSAGEMODE
        kCodeConfirmationByUserNotify,   //*< notification that user has accepted shared secret derivative
        kRoleSelectedNotify, //*< notification that user has selected a role to pair in IHU
        kEndOfTrip,  //*< VehicleComm vc::EVENT_ENDOFTRIP, no payload!
        kPrivacyNotificationSignal,  //*< signal issued by IPCommandBroker
        kDeadReckonedPositionSignal,  //*< signal issued by IPCommandBroker
        kGNSSPositionDataSignal,  //*< signal issued by IPCommandBroker
        kXmlSignal,               //*< used for generic Xml signals
        // below are temporary signals used until VC provides them properly
        kDevicePairingConfirmationByAdminRequest, //*< request from IHU for admin approval for pairing
        kDevicePairingConfirmationByAdminDeviceResponse, //*< admin approval response from admin
        kListPairedDevicesRequest, //*< triggers list of paired users retrieval
        kNumberOfKeysInsideVehicleRequest, //* triggers search for keys inside vehicle
        kTimeout, //* Issued by timer signal source when timout occurs
        kKeysFound, //*< signal from CEM indicating how many keys found so far out of total registered
        kPairingVisibilityRequest, //*< request from IHU to go into pairing mode
        kBasicCarControl,
#if 1 //nieyj add for Geely
        kDoorLockStatusSignal,
        kWinOpenStatusSignal,
        kPMLvlStatusSignal,
        kVFCActivateSignal,
        kRvdcSignal,
        kRvdcReqMA,
        kRvdcNotifyAssign,
        kRvdcUploadMDP,
        kRvdcReqGPSTime,
        kRvdcReqConnectivitySts,
        kRvdcReqAuthorizationSts,
#endif
        //uia93888 add kRMTEngineSignal
        kRMTEngineSecurityRandom,  //*< Vuc sync security authentication random to Nad
        kRMTEngineSecurityResult,  //*< Vuc sync security authentication result to Nad
        kRMTEngineSignal, //*< signal from Vuc is remote start
        kRMTCarStatesSignal, //*< signal from data storage
        kAppsLcmResumeSignal, //*< signal from apps_lcm's ResumeNotification
        kElectEngLvlSignal, //*< signal from Vuc to get energy level
        kDelayEngineRinningtimeSignal, //*< signal of delay engine running timer signal
        kRemoteCliamteSignal, //*< signal of Remote climate
        kRemoteSeatHeatSignal, //*< signal of remote seat heat
        kRemoteSeatVentiSignal, //*< signal of remote seat ventilation
        kRemoteSteerWhlHeatSignal, //*< signal of remote steer wheel heat
        kParkingClimateOperSignal, //*< signal of parking climate operate response
        //uia93888 add end
        kTheftDataSignal, //*< signal issued by IPCommandBroker
        kStolenVehicleTracking,  //*< VehicleComm vc::Request_GetCarCfgStolenVehicleTracking
        kTelmDshb,
        kRMSDataSignal, //*< signal issued by IPCommandBroker
        kHvBattVoltageSignal,
        kHvBattTempSignal,
        kVehSoHCheckResponse, //response signal , state of Vehicle health remind from IPCommandBroker
        kVehSoHCheckNotify, //notify signal , state of Vehicle health remind from IPCommandBroker
        // RMS service Signals
        kRemoteMonitoringInfo,
        kHvBattVoltageData,
        kHvBattTempData,
        kHvBattCod,
        // OTA service Signals
        kOTAAssignmentNotification,
        kOTAQueryAvailableStorageResp,
        kOTAAssignmentSyncReq,
        kOTAHMILanguageSettingsNotification,
        kDownloadConsentNotification, //*< signal from IPCommandBroker
        kInstallationConsentNotification, //*< signal from IPCommandBroker
        kOTAAssignBOOTResp, //*< signal from IPCommandBroker
        kOTAInstallationSummaryNotification,
        kOTAWriteAssignmentDataResp,
        kOTAExceptionReportNotification,
        // others
        kSubscribeTopicSignal,
        kVINNumberSignal,
        kTcamHwVerSignal,
        kHttpSignal
    };

    /**
     * \brief Returns the signal type.
     * \return Type of the signal
     */
    virtual SignalType GetSignalType() {return my_signal_type;};

 protected:

    /**
     * \brief Protected costructor. It should only be possible to create
     *        instances of subclasses.
     * \param[in] transaction_id transaction id
     * \param[in] signal_type optional type of the signal
     *                        if not given GetSignalType should be overriden.
     */
    Signal (const TransactionId& transaction_id, const SignalType signal_type = kTypeUndefined);


 private:

    std::shared_ptr<TransactionId> transaction_id_;

    SignalType my_signal_type = kTypeUndefined;

    fsm::UserId sender_ = fsm::kUndefinedUserId;

    //no deafult constructor
    Signal ();

    int topic_priority = kPriorityUndefined;

    void SetTopicPriority(int priority) {topic_priority = priority;};

#ifdef VOC_TESTS
 public:
#endif
    //TODO: move into legacy signals
    void SetSender(fsm::UserId sender) {sender_ = sender;};
};




} // namespace fsm

#endif //VOC_FRAMEWORK_SIGNALS_SIGNAL_H_

/** \}    end of addtogroup */

