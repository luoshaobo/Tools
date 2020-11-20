/*
* Copyright (C) 2018 Continental Automotive GmbH
*
* Licensed under the "Supplier unrestricted white box" category, as
* described in the 20160322-1_SW-amendment TCAM.pdf agreement
* between Volvo Cars and Continental Automotive GmbH.
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*/
/**
 * @file vpom_IPositioningService.hpp
 * @brief Positioning interface
 *
 *  \addtogroup vpom_IPositioningService
 *  \{
 */

#ifndef VPOM_IPOS_SERVICE_HPP
#define VPOM_IPOS_SERVICE_HPP

#include "vpom_common_service.hpp"

namespace vpom
{

//Payload values in GNSSPositionData RESPONSE
typedef struct {
    uint32_t min_size;      ///< ReceiverChannelData min size
    uint32_t max_size;      ///< ReceiverChannelData max size
    uint32_t actual_size;   ///< ReceiverChannelData actual size
    uint32_t data_array;    ///< ReceiverChannelData data array
} ReceiverChannelData;

typedef struct {
    uint32_t hdop;   ///< INTEGER(0..255), HDOP in 1/10. 255=unknown or >25.4
    uint32_t vdop;   ///< INTEGER(0..255), VDOP in 1/10. 255=unknown or >25.4
    uint32_t pdop;   ///< INTEGER(0..255), PDOP in 1/10. 255=unknown or >25.4
    uint32_t tdop;   ///< INTEGER(0..255), TDOP in 1/10. 255=unknown or >25.4
} DOPValues;

typedef struct {
    uint32_t gps;          ///< INTEGER(0..31), nbr of GPS satellites, 31=unknown
    uint32_t glonass;      ///< INTEGER(0..31), nbr of GLONASS satellites, 31=unknown
    uint32_t galileo;      ///< INTEGER(0..31), nbr of Galileo satellites, 31=unknown
    uint32_t sbas;         ///< INTEGER(0..31), nbr of SBAS satellites, 31=unknown
    uint32_t qzssL1;       ///< INTEGER(0..31), nbr of QZSS L1 satellites, 31=unknown
    uint32_t qzssL1SAIF;   ///< INTEGER(0..31), nbr of QZSS L1-SAIF satellites, 31=unknown
} NrOfSatellitesPerSystem;

typedef struct {
    NrOfSatellitesPerSystem nrOfSatellitesVisible;   ///< Nr of visible satellites
    NrOfSatellitesPerSystem nrOfSatellitesUsed;      ///< Nr of used satellites
} SatelliteUsage;

typedef enum {
    notAvailable = 0,   ///< No GNSS information available (e.g no GNSS antenna connection, GNSS receiver failure)
    noFix = 1,          ///< No Fix: satellites received - no position available
    fix2D = 2,          ///< 2D fix : satellites received : 2 dimensional position fix
    fix3D = 3,          ///< 3D fix : satellites received : 3 dimensional position fix
    startupMode = 4,    ///< Based on a position saved after last driving cycle (Cold Start Fix)
} GnssFixType;

typedef struct {
    GnssFixType fixType;           ///< Specifies the current GNSS fix
    bool dgpsIsUsed;               ///< DGPS used(True)/not used(False)
    bool selfEphemerisDataUsage;   ///< Self ephemeris data used(True)/not used(False)
} GNSSStatus;

typedef struct {
    bool gpsIsUsed;          ///< used(True)/not used(False)
    bool glonassIsUsed;      ///< used(True)/not used(False)
    bool galileoIsUsed;      ///< used(True)/not used(False)
    bool sbasIsUsed;         ///< used(True)/not used(False)
    bool qzssL1IsUsed;       ///< used(True)/not used(False)
    bool qzssL1SAIFIsUsed;   ///< used(True)/not used(False)
} GNSSUsage;

typedef struct {
    uint32_t speed;                ///< INTEGER(0..100000), Speed in 1/1000 m/s (mm/s)
    uint32_t horizontalVelocity;   ///< INTEGER(0..100000), Horizontal velocity in 1/1000 m/s (mm/s)
    int32_t verticalVelocity;      ///< INTEGER(-100000..100000), Vertical velocity in 1/1000 m/s (mm/s)
} Velocity;

typedef struct {
    int32_t longitude;   ///< INTEGER(-2147483648..2147483647)
    int32_t latitude;    ///< INTEGER(-1073741824..1073741824)
} CoordinatesLongLat;

typedef struct {
    CoordinatesLongLat longlat;
    int32_t altitude;             ///< Altitude above mean sea level in 1/10 m
} GeographicalPosition;

typedef struct {
    uint32_t weekNumber;   ///< The GPS week number
    uint32_t timeOfWeek;   ///< Time of week in 1/1000 s
} GPSSystemTime;

typedef struct {
    uint32_t year;     ///< INTEGER(2000..2127)
    uint32_t month;    ///< INTEGER(1..12)
    uint32_t day;      ///< INTEGER(1..31)
    uint32_t hour;     ///< INTEGER(0..23)
    uint32_t minute;   ///< INTEGER(0..59)
    uint32_t second;   ///< INTEGER(0..59)
} DateTime;

typedef enum {
    GNSS_DATA_INVALID,
    GNSS_DATA_CURRENT,
    GNSS_DATA_AGED
} GnssDataStatus;

typedef struct {
    GnssDataStatus datastatus;              ///< The Data status
    DateTime utcTime;                       ///< The UTC time
    GPSSystemTime gpsTime;                  ///< The GPS time
    GeographicalPosition position;          ///< The position in 3D
    Velocity movement;                      ///< Speed and velocity
    uint32_t heading;                       ///< Heading from true north in 1/100 deg
    uint32_t magnetic_heading;              ///< Magnetic Heading from true north in 1/100 deg
    GNSSUsage gnssStatus;                   ///< Indicates the GNSS technologies used for positioning
    GNSSStatus positioningStatus;           ///< The positioning status
    SatelliteUsage satelliteInfo;           ///< Info on number of satellites visible and used
    DOPValues precision;                    ///< Dilution of precision values
    ReceiverChannelData receiverChannels;   ///< Array of data of the receiver channels (Note: not fully implemented yet)
} GNSSData;

} //namespace vpom


/**
 * @brief                                   Callback function type for GNSSPositionDataRawResponseCb_t
 *
 * @param[in] param                         GNSSPositionDataResponse parameters
 * @param[in] request_id                    Same id that was sent in request
 *
 */
typedef void (*GNSSPositionDataRawResponseCb_t)(vpom::GNSSData *param, uint64_t request_id);

/**
 * @brief    Interface providing access to the Positioning functions of IPCommand :
 *           - GNSS raw data from the TCAM (including the geographical position (WGS-84) of the vehicle)
 *           - DeadReckoned position provided by IHU
 */
class VpomIPositioningService {
public:
    //Methods begin
    /**
     * @brief                                   Function called by client to send GNSSPositionDataRaw request
     *                                          Data is retrieved from Location Manager without IHU interaction
     *
     * @param[in] request_id                    Request Id
     *
     * @return                                  True if send operation was ok.
     *                                          False otherwise.
     */
    virtual bool GNSSPositionDataRawRequest(uint64_t request_id) = 0;
    //Methods end

    //Methods callbacks begin
    //Methods callbacks end

    //Signal callback begin
    /**
    * @brief                                   Function called by client to register callback function for GNSSPositionDataRaw Response
    *
    * @param[in] cb                            GNSSPositionDataRawResponseCb_t
    *
    * @return                                  True if registration was ok.
    *                                          False otherwise.
    */
   virtual bool RegisterGNSSPositionDataRawResponseCb(GNSSPositionDataRawResponseCb_t cb) = 0;

   /**
    * @brief                                   Function called by client to deregister callback function for GNSSPositionDataRaw Response
    *
    * @return                                  True if deregistration was ok.
    *                                          False otherwise.
    */
   virtual bool DeregisterGNSSPositionDataRawResponseCb(void) = 0;

//Signal callback end

};

#endif //VPOM_IPOS_SERVICE_HPP

/** \}    end of addtogroup */
