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
 *  \file     ip_command_broker_signal.h
 *  \brief    VOC Service IPCommandBroker signal base class.
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_FRAMEWORK_SIGNALS_IP_COMMAND_BROKER_SIGNAL_H_
#define VOC_FRAMEWORK_SIGNALS_IP_COMMAND_BROKER_SIGNAL_H_

#include "voc_framework/signals/signal.h"

#include <memory>
#include <type_traits>

#ifndef UNIT_TESTS
#include "ipcb_IGNSSService.hpp"
#include "ipcb_IPrivacyService.hpp"
#include "ipcb_ITelematicsService.hpp"
#else
// IpCommandBroker header pulls in most of OTP, simply duplicate the types here
// Copied from:
// package/vcc-tcam/IPCommandBroker/interface/ipcb_IGNSSService.hpp
// package/vcc-tcam/IPCommandBroker/interface/ipcb_IPrivacyService.hpp

typedef struct {
    bool CarStatUploadEn;
    bool LocationServicesEn;
} PrivacyNotificationType;

typedef struct {
    uint32_t min_size;
    uint32_t max_size;
    uint32_t actual_size;
    uint32_t data_array;
} ReceiverChannelData;

typedef struct {
    uint32_t hdop;
    uint32_t vdop;
    uint32_t pdop;
    uint32_t tdop;
} DOPValues;

typedef struct {
    uint32_t gps;
    uint32_t glonass;
    uint32_t galileo;
    uint32_t sbas;
    uint32_t qzssL1;
    uint32_t qzssL1SAIF;
} NrOfSatellitesPerSystem;

typedef struct {
    NrOfSatellitesPerSystem nrOfSatellitesVisible;
    NrOfSatellitesPerSystem nrOfSatellitesUsed;
} SatelliteUsage;

typedef enum {
    notAvailable = 0,
    noFix = 1,
    fix2D = 2,
    fix3D = 3,
    startupMode = 4,
} GnssFixType;

typedef struct {
    GnssFixType fixType;
    bool dgpsIsUsed;
    bool selfEphemerisDataUsage;
} GNSSStatus;

typedef struct {
    bool gpsIsUsed;
    bool glonassIsUsed;
    bool galileoIsUsed;
    bool sbasIsUsed;
    bool qzssL1IsUsed;
    bool qzssL1SAIFIsUsed;
} GNSSUsage;

typedef struct {
    uint32_t speed;
    uint32_t horizontalVelocity;
    int32_t verticalVelocity;
} Velocity;

typedef struct {
    int32_t longitude;
    int32_t latitude;
} CoordinatesLongLat;

typedef struct {
    CoordinatesLongLat longlat;
    int32_t altitude;
} GeographicalPosition;

typedef struct {
    uint32_t weekNumber;
    uint32_t timeOfWeek;
} GPSSystemTime;

typedef struct {
    uint32_t year;
    uint32_t month;
    uint32_t day;
    uint32_t hour;
    uint32_t minute;
    uint32_t second;
} DateTime;

typedef enum {
    GNSS_DATA_INVALID,
    GNSS_DATA_CURRENT,
    GNSS_DATA_AGED
} GnssDataStatus;

typedef struct {
    GnssDataStatus datastatus;
    DateTime utcTime;
    GPSSystemTime gpsTime;
    GeographicalPosition position;
    Velocity movement;
    uint32_t heading;
    uint32_t magnetic_heading;
    GNSSUsage gnssStatus;
    GNSSStatus positioningStatus;
    SatelliteUsage satelliteInfo;
    DOPValues precision;
    ReceiverChannelData receiverChannels;
} GNSSData;

//Payload values in DeadReckonedPosition RESPONSE
typedef enum {
    noDr = 0,
    drNoMapMatch = 1,
    drMapMatched = 2,
} DeadReckonedType;

typedef struct {
    CoordinatesLongLat longlat;
    uint32_t heading;
    uint32_t speedKmph;
    uint32_t hdopX10;
    uint32_t numSat;
    DateTime fixTime;
    GnssFixType fixType;
    DeadReckonedType drType;
    uint32_t drDistance;
} DRPositionData;

typedef struct {
    uint16_t noValidData;
    DRPositionData drPosition;
} DRVehiclePosition;

// End of copied types
#endif

typedef struct {
    bool antithftActvnNotif;
    uint32_t alrmTrgSrc;
    bool snsrSoundrBattBackedFailr;
    uint32_t alrmSt;
    bool snsrInclnFailr;
    bool snsrIntrScanrFailr;
}TheftNoticeData;

typedef struct {
    uint32_t bkpOfDstTrvld;
    uint32_t dstTrvldHiResl;
    uint32_t engFuCns;
    uint32_t fuCnsIndcdUnit;
    uint32_t fuCnsIndcdVal;
    uint32_t vehSpdIndcd1;
    uint32_t veSpdIndcdUnit;
    uint32_t vehSpdIndcdQly;
} TelmRoldTripData;

typedef struct {
uint32_t *arrydata;
uint32_t arryDataSize;
uint32_t hvBattIDc1;
uint32_t hvBattNr;
uint32_t hvBattUDc;
uint32_t hvBattVoltMaxSerlNr;
uint32_t hvBattVoltMinSerlNr;
uint32_t packNr;
} HvBattVoltageData;

typedef struct {
uint32_t *arrydata;
uint32_t arryDataSize;
uint32_t hvBattCellTNr;
uint32_t hvBattCellTSerlNr;
uint32_t hvBattNr;
uint32_t hvBattTMaxSerlNr;
uint32_t hvBattTMinSerlNr;
uint32_t packNr;
} HvBattTempData;

typedef struct {
    int32_t srvTrig;
    int32_t dayToSrv;
    int32_t engHrToSrv;
    int32_t dstToSrv;
    bool srvRst;
} VehSoHCheckData;

namespace fsm
{

template<typename T>
class IpCommandBrokerSignal : public Signal
{

 public:

    /**
     * \brief Create a IpCommandBrokerSignal.
     *
     * \param[in] payload The payload which this signal should wrap.
     * \param[in] transaction_id The transaction id attached to this
     *                           signal.
     * \param[in] signal_type This signals specific type.
     *
     * \todo would it make sense to add a check, whether the signal_type is a valid IpCommandBrokerSignal?
     *
     * \return Shared pointer to the signal if successful, or empty
     *         shared pointer if not.
     */
    static std::shared_ptr<IpCommandBrokerSignal<T>> CreateIpCommandBrokerSignal (
                                                         T payload,
                                                         TransactionId& transaction_id,
                                                         SignalType signal_type)
    {
        std::shared_ptr<IpCommandBrokerSignal<T>> return_signal;
        return_signal = std::shared_ptr<IpCommandBrokerSignal<T>>(
                            new IpCommandBrokerSignal<T>(payload,
                                                         transaction_id,
                                                         signal_type));
        return return_signal;
    }

    /**
     * \brief Get the payload contained in the signal.
     * \return the payload.
     */
    const T& GetPayload ()
    {
        return payload_;
    }


    /**
     * \brief Serialize to string, used for debug printouts. Must be implemented by subclasses.
     * \return Name or other text identifier of the signal.
     */
    std::string ToString ()
    {
        return ("IpCommandBrokerSignal, type: " + GetSignalType());
    }


 private:

    /**
     * \brief Private costructor, no subclasses, only way to create is via factory.
     * \param[in] payload        The payload which the Signal should contain.
     * \param[in] transaction_id Transaction id of the signal.
     * \param[in] signal_type    Type of the signal.
     */
    IpCommandBrokerSignal (T payload,
                           TransactionId& transaction_id,
                           SignalType signal_type) : Signal(transaction_id, signal_type),
                                                     payload_(payload) {};


    /**
     * \brief The payload contained in this signal.
     *        Typically a struct define by IPCommandBroker
     */
    const T payload_;


    //no default constructor
    IpCommandBrokerSignal () {};
};

// RMS service
typedef IpCommandBrokerSignal<OpRemoteMonitoringInfo_NotificationCyclic_Data> RemoteMonitoringInfoSignal;
typedef IpCommandBrokerSignal<OpHvBattVoltageData_Notification_Data> HvBattVoltageDataSignal;
typedef IpCommandBrokerSignal<OpHvBattTempData_Notification_Data> HvBattTempDataSignal;
typedef IpCommandBrokerSignal<OpHvBattCod_Notification_Data> HvBattCodSignal;
#ifndef UNIT_TESTS

typedef IpCommandBrokerSignal<PrivacyNotificationType> PrivacyNotificationSignal;
typedef IpCommandBrokerSignal<DRVehiclePosition> DeadReckonedPositionSignal;
typedef IpCommandBrokerSignal<GNSSData> GNSSPositionDataSignal;
typedef IpCommandBrokerSignal<TheftNoticeData> THEFTNotificationSignal;
typedef IpCommandBrokerSignal<TelmRoldTripData> TelmRoadTripSignal;
typedef IpCommandBrokerSignal<OpTelmDshb_NotificationCyclic_Data> TelmDshbSiganl;
typedef IpCommandBrokerSignal<RemoteMonitoringInfo_t> RMSNotificationSignal;
typedef IpCommandBrokerSignal<HvBattVoltageData> HvBattVoltageSignal;
typedef IpCommandBrokerSignal<HvBattTempData> HvBattTempSignal;
typedef IpCommandBrokerSignal<VehSoHCheckData> VehSoHCheckSignal;
// OTA service
typedef IpCommandBrokerSignal<OpOTAAssignmentNotification_Notification_Data> AssignmentNotificationDataSignal;
typedef IpCommandBrokerSignal<OpOTAQueryAvailableStorage_Response_Data> QueryAvailableStorageRespSignal;
typedef IpCommandBrokerSignal<OpOTAHMILanguageSettings_Notification_Data> OTAHMILanguageSettingsSignal;
typedef IpCommandBrokerSignal<OpDownloadConsent_Notification_Data> DownloadConsentSignal;
typedef IpCommandBrokerSignal<OpInstallationConsent_Notification_Data> InstallationConsentSignal;
typedef IpCommandBrokerSignal<OpOTAAssignBOOT_Response_Data> OTAAssignBOOTRespSignal;
typedef IpCommandBrokerSignal<OpOTAInstallationSummary_Notification_Data> InstallationSummaryNotificationSignal;
typedef IpCommandBrokerSignal<OpOTAWriteAssignmentData_Response_Data> OTAWriteAssignmentDataRespSignal;
typedef IpCommandBrokerSignal<OpOTAExceptionReports_Notification_Data> OTAExceptionReportNotificationSignal;

// RVDC service
typedef IpCommandBrokerSignal<OpRVDCMeasurementSynchronization_Request_Data> RVDCMeasurementSynchronizationRequestSignal;
typedef IpCommandBrokerSignal<OpRVDCAssignmentNotification_Notification_Data> RVDCAssignmentNotificaitonSignal;
typedef IpCommandBrokerSignal<OpRVDCMeasurementDataPackage_Request_Data> RVDCMeasurementDataPackageSignal;
typedef IpCommandBrokerSignal<OpGeneric_Request_Data> RVDCConnectivityStatusSignal;
typedef IpCommandBrokerSignal<OpGeneric_Request_Data> RVDCGPSTimeSignal;
typedef IpCommandBrokerSignal<OpGeneric_Request_Data> RVDCAuthorizationStatusRequestSignal;

// Generic data signal
typedef IpCommandBrokerSignal<OpGeneric_Request_Data> GenericRequestSignal;
typedef IpCommandBrokerSignal<OpGeneric_Response_Data> GenericResponseSignal;
#endif
} // namespace fsm

#endif // VOC_FRAMEWORK_SIGNALS_IP_COMMAND_BROKER_SIGNAL_H_

/** \}    end of addtogroup */
