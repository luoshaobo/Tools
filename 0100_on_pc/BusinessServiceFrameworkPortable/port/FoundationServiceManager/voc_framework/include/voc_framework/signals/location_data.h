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
 *  \file     location_data.h
 *  \brief    VOC Service location data class.
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_FRAMEWORK_SIGNALS_LOCATION_DATA_H_
#define VOC_FRAMEWORK_SIGNALS_LOCATION_DATA_H_

#include "GnssFixStatus.h"
#include "SatelliteSystem.h"
#include "Location.h"
#include "MinimumSetOfLocationData.h"
#include "voc_framework/signals/ip_command_broker_signal.h"

#include <cmath>
#include <ctime>
#include <limits>

namespace fsm
{

class LocationData
{

 public:

    /**
     * \brief Default value used for unassigned int fields.
     */
    static const int kDefaultIntValue = std::numeric_limits<int>::lowest();
    /**
     * \brief Default value used for unassigned double fields.
     */
    static constexpr double kDefaultDoubleValue = std::numeric_limits<double>::lowest();

    /**
     * \brief Enumeration of GNSS fix statuses.
     */
    enum GnssFixStatus
    {
        GNSS_FIX_STATUS_UNDEFINED,
        GNSS_FIX_STATUS_NO_FIX,
        GNSS_FIX_STATUS_TIME_FIX,
        GNSS_FIX_STATUS_TWO_D_FIX,
        GNSS_FIX_STATUS_THREE_D_FIX
    };

    enum LocationPr
    {
        kLocationNothing,
        kLocationMinimum,
        kLocationExtended
    };

    /**
     * \brief Enumeration of satellite systems that might be used.
     *        Each entry corresponds to a bit in a bitfield.
     */
    enum SatelliteSystems
    {
        SATELLITE_SYSTEMS_UNDEFINED = 0x0,
        SATELLITE_SYSTEMS_GPS = 0x1,
        SATELLITE_SYSTEMS_GLONASS = 0x2,
        SATELLITE_SYSTEMS_GALILEO = 0x4,
        SATELLITE_SYSTEMS_BEIDOU = 0x8
    };

    /**
     * \brief Converts degrees to milliarcseconds.
     * \param[in] degrees The number of degrees to convert
     * \return degrees converted into milliarcseconds.
     */
    static long DegreesToMAS(double degrees) {return std::lround(degrees * kMASPerDegree);}

    /**
     * \brief Converts milliarcseconds to degrees.
     * \param[in] mas The number of milliarcseconds to convert
     * \return milliarcseconds converted into degrees.
     */
    static double MASToDegrees(long mas) {return (static_cast<double>(mas) /
                                                  static_cast<double>(kMASPerDegree));}

    /**
     * \brief Sets the position_can_be_trusted field.
     * \param[in] value_to_set The value to set.
     * \return None
     */
    void SetPositionCanBeTrusted (bool value_to_set);

    /**
     * \brief Sets the latitude field.
     * \param[in] value_to_set The value to set, in degrees.
     *                         Valid values are from -90 to 90 degrees.
     *                         kDefaultDoubleValue can be provided
     *                         to unset the field.
     * \return True if successfully set, false otherwise.
     */
    bool SetLatitude (double value_to_set);

    /**
     * \brief Sets the longitude field.
     * \param[in]  value_to_set The value to set, in degrees.
     *                          Valid values are from -180 to 180 degrees.
     *                          kDefaultDoubleValue can be provided
     *                          to unset the field.
     * \return True if successfully set, false otherwise.
     */
    bool SetLongitude (double value_to_set);

    /**
     * \brief Sets the heading field.
     * \param[in]  value_to_set The value to set, in 2° steps from magnetic north.
     *             Valid values are 0 to 179.
     *             kDefaultIntValue can be provided
     *             to unset the field.
     * \return True if successfully set, false otherwise.
     */
    bool SetHeading (int value_to_set);

    /**
     * \brief Sets the latitude_delta_1 field.
     * \param[in]  value_to_set The value to set, in degrees.
     *                          Valid values are from -0.01422 to 0.01419 degrees.
     *                          kDefaultDoubleValue can be provided
     *                          to unset the field.
     * \return True if successfully set, false otherwise.
     */
    bool SetLatitudeDelta1 (double value_to_set);

    /**
     * \brief Sets the latitude_delta_2 field.
     * \param[in]  value_to_set The value to set, in degrees.
     *                          Valid values are from -0.01422 to 0.01419 degrees.
     *                          kDefaultDoubleValue can be provided
     *                          to unset the field.
     * \return True if successfully set, false otherwise.
     */
    bool SetLatitudeDelta2 (double value_to_set);

    /**
     * \brief Sets the longitude_delta_1 field.
     * \param[in]  value_to_set The value to set, in degrees.
     *                          Valid values are from -0.01422 to 0.01419 degrees.
     *                          kDefaultDoubleValue can be provided
     *                          to unset the field.
     * \return True if successfully set, false otherwise.
     */
    bool SetLongitudeDelta1 (double value_to_set);

    /**
     * \brief Sets the longitude_delta_2 field.
     * \param[in]  value_to_set The value to set, in degrees.
     *                          Valid values are from -0.01422 to 0.01419 degrees.
     *                          kDefaultDoubleValue can be provided
     *                          to unset the field.
     * \return True if successfully set, false otherwise.
     */
    bool SetLongitudeDelta2 (double value_to_set);

    /**
     * \brief Sets the data_time_stamp field.
     * \return None.
     */
    void SetTimeStamp (struct tm data_time_stamp_to_set);

    /**
     * \brief Sets the altitude field.
     * \param[in] value_to_set The value to set, in m.
     *                         kDefaultDoubleValue can be provided
     *                         to unset the field.
     * \return None.
     */
    void SetAltitude (double value_to_set);

    /**
     * \brief Sets the speed field.
     * \param[in] value_to_set The value to set, in m/s.
     *                         Valid values are from 0 to 200.
     *                         kDefaultDoubleValue can be provided
     *                         to unset the field.
     * \return True if successfully set, false otherwise.
     */
    bool SetSpeed (double value_to_set);

//TODO: unit unknown, seek clarification from writers of specifications.
    /**
     * \brief Sets the climb field.
     * \param[in] value_to_set The value to set, in TODO:?.
     *                         Valid values are from -180 to 180.
     *                         kDefaultDoubleValue can be provided
     *                         to unset the field.
     * \return True if successfully set, false otherwise.
     */
    bool SetClimb (double value_to_set);

//TODO: unit unknown, seek clarification from writers of specifications.
    /**
     * \brief Sets the roll_rate field.
     * \param[in] value_to_set The value to set, in ?.
     *                         Valid values are from -100 to 100.
     *                         kDefaultDoubleValue can be provided
     *                         to unset the field.
     * \return True if successfully set, false otherwise.
     */
    bool SetRollRate (double value_to_set);

//TODO: unit unknown, seek clarification from writers of specifications.
    /**
     * \brief Sets the pitch_rate field.
     * \param[in] value_to_set The value to set, in ?.
     *                         Valid values are from -100 to 100.
     *                         kDefaultDoubleValue can be provided
     *                         to unset the field.
     * \return True if successfully set, false otherwise.
     */
    bool SetPitchRate (double value_to_set);

//TODO: unit unknown, seek clarification from writers of specifications.
    /**
     * \brief Sets the yaw_rate field.
     * \param[in] value_to_set The value to set, in ?.
     *                         Valid values are from -100 to 100.
     *                         kDefaultDoubleValue can be provided
     *                         to unset the field.
     * \return True if successfully set, false otherwise.
     */
    bool SetYawRate (double value_to_set);

    /**
     * \brief Sets the pdop field.
     * \param[in] value_to_set The value to set.
     *                         kDefaultDoubleValue can be provided
     *                         to unset the field.
     * \return None.
     */
    void SetPDop (double value_to_set);

    /**
     * \brief Sets the hdop field.
     * \param[in] value_to_set The value to set.
     *                         kDefaultDoubleValue can be provided
     *                         to unset the field.
     * \return None.
     */
    void SetHDop (double value_to_set);

    /**
     * \brief Sets the vdop field.
     * \param[in] value_to_set The value to set.
     *                         kDefaultDoubleValue can be provided
     *                         to unset the field.
     * \return None.
     */
    void SetVDop (double value_to_set);

    /**
     * \brief Sets the used_satellite field.
     * \param[in] value_to_set The value to set.
     *                         kDefaultIntValue can be provided
     *                         to unset the field.
     * \return None.
     */
    void SetUsedSatellites(int value_to_set);

    /**
     * \brief Sets the tracked_satellite field.
     * \param[in] value_to_set The value to set.
     *                         kDefaultIntValue can be provided
     *                         to unset the field.
     * \return None.
     */
    void SetTrackedSatellites(int value_to_set);

    /**
     * \brief Sets the visible_satellite field.
     * \param[in] value_to_set The value to set.
     *                         kDefaultIntValue can be provided
     *                         to unset the field.
     * \return None.
     */
    void SetVisibleSatellites(int value_to_set);

    /**
     * \brief Sets the sigma_h_position field.
     * \param[in] value_to_set The value to set.
     *                         kDefaultDoubleValue can be provided
     *                         to unset the field.
     * \return None.
     */
    void SetSigmaHPosition(double value_to_set);

    /**
     * \brief Sets the sigma_altitude field.
     * \param[in] value_to_set The value to set.
     *                         kDefaultDoubleValue can be provided
     *                         to unset the field.
     * \return None.
     */
    void SetSigmaAltitude(double value_to_set);

    /**
     * \brief Sets the sigma_speed field.
     * \param[in] value_to_set The value to set.
     *                         kDefaultDoubleValue can be provided
     *                         to unset the field.
     * \return None.
     */
    void SetSigmaSpeed(double value_to_set);

    /**
     * \brief Sets the sigma_climb field.
     * \param[in] value_to_set The value to set.
     *                         kDefaultDoubleValue can be provided
     *                         to unset the field.
     * \return None.
     */
    void SetSigmaClimb(double value_to_set);

    /**
     * \brief Sets the sigma_pressure field.
     * \param[in] value_to_set The value to set.
     *                         kDefaultDoubleValue can be provided
     *                         to unset the field.
     * \return None.
     */
    void SetSigmaPressure(double value_to_set);

    /**
     * \brief Sets the sigma_heading field.
     * \param[in] value_to_set The value to set in degrees.
     *                         Valid values are from 0 to 180.
     *                         kDefaultIntValue can be provided
     *                         to unset the field.
     * \return True if successfully set, false otherwise.
     */
    bool SetSigmaHeading(int value_to_set);

    /**
     * \brief Sets the gnss_fix_status field.
     * \param[in] status The value to set.
     * \return True if successfully set, false otherwise.
     */
    bool SetGNSSFixStatus(GnssFixStatus status);

    /**
     * \brief Sets the dr_status field.
     * \param[in] status The value to set.
     * \return None.
     */
    void SetDrStatus(bool status);

    /**
     * \brief Adds a satellite system to the used_satellite_systems
     *        field.
     * \param[in] status The value to set.
     * \return True if successfully set, false otherwise.
     */
    bool SetUsedSatelliteSystem(SatelliteSystems system);

//TODO: unit unknown, seek clarification from writers of specifications.
    /**
     * \brief Sets the pressure field in ?.
     * \param[in] status The value to set.
     * \return None.
     */
    void SetPressure(double value_to_set);

    /**
     * \brief Sets all the fields that can be mapped from DRPositionData.
     * \param[in] data The value to set.
     * \return None.
     */
    void SetFromDRPositionData(const DRPositionData &data);

    /**
     * \brief Sets all the fields that can be mapped from GNSSData.
     * \param[in] data The value to set.
     * \return None.
     */
    void SetFromGNSSData(const GNSSData &data);

    /**
     * \brief Compare two LocationData for equality.
     * \param[in] lhs Left hand side argument for comaprison.
     * \param[in] rhs Right Hand Side argument for comparison.
     * \return True if the two LocationData hold the same data,
     *         Otherwise false.
     */
    bool operator== (const LocationData& other);

 protected:

    /**
     * \brief Packs the locations data into the provided
     *        ASN1C struct.
     * \param[out] extended_location The asn1c struct to pack the extended location into.
     *                              Caller must ensure this parameter is
     *                              "deep freed" even if this call fails.
     * \param[out] minimum_location The asn1c struct to pack the minimum location into.
     *                              Caller must ensure this parameter is
     *                              "deep freed" even if this call fails.
     * \param[out] location_pr Indicates which location type was encoded extender or
     *                         minimum. Will be set to kLocationNothing if operation
     *                         failed.
     *
     * \return True if succesfull, falses otherwise. Parameters
     *         are not guaranteed to be unaffected in case of failure.
     */
    bool GetPackedLocationData (Location_t* extended_location,
                                MinimumSetOfLocationData_t* minimum_location,
                                LocationPr& location_pr);

#ifdef VOC_TESTS
    bool UnpackMinimumLocation(MinimumSetOfLocationData_t* minimum_location);

    bool UnpackExtendedLocation(Location_t* extended_location);
#endif

 private:

    /**
     * \brief Number of milliarcseconds equal to one degree.
     */
    static const int kMASPerDegree = 3600000;

    /**
     * \brief positionCanBeTrusted in minimumLocation and extendedLocation
     */
    bool position_can_be_trusted = false;
    /**
     * \brief tracks whether postion_can_be_trusted has been set
     */
    bool is_position_can_be_trusted_set = false;

    /**
     * \brief vehicleLocation in minumumLocation, coordinate extendedLocation
     *        degrees
     */
    double latitude = kDefaultDoubleValue;
    /**
     * \brief vehicleLocation in minumumLocation, coordinate extendedLocation
     *        degrees
     */
    double longitude = kDefaultDoubleValue;

    /**
     * \brief direction in minimumLocation, heading in extendedLocation
     *        degrees
     */
    int heading = kDefaultIntValue;

    /**
     * \brief delta in minimumLocation
     *        degrees
     */
    double latitude_delta_1 = kDefaultDoubleValue;
    /**
     * \brief delta in minimumLocation
     *        degrees
     */
    double latitude_delta_2 = kDefaultDoubleValue;
    /**
     * \brief delta in minimumLocation
     *        degrees
     */
    double longitude_delta_1 = kDefaultDoubleValue;
    /**
     * \brief delta in minimumLocation
     *        degrees
     */
    double longitude_delta_2 = kDefaultDoubleValue;

    /**
     * \brief dataTimestamp in extendedLocation
     */
    struct tm data_time_stamp = {};
    /**
     * \brief tracks whether data_time_Stamp ahs been set
     */
    bool is_data_time_stamp_set = false;

    /**
     * \brief altitude in extendedLocation
     *        in meters
     */
    double altitude = kDefaultDoubleValue;

    /**
     * \brief speed in extendedLocation
     * m/s
     */
    double speed = kDefaultDoubleValue;

    /**
     * \brief climb in extendedLocation
     */
    double climb = kDefaultDoubleValue;

    /**
     * \brief rollRate in extendedLocation
     */
    double roll_rate = kDefaultDoubleValue;

    /**
     * \brief pitchRate in extendedLocation
     */
    double pitch_rate = kDefaultDoubleValue;

    /**
     * \brief yawRate in extendedLocation
     */
    double yaw_rate = kDefaultDoubleValue;

    /**
     * \brief pdop in extendedLocation
     */
    double pdop = kDefaultDoubleValue;

    /**
     * \brief hdop in extendedLocation
     */
    double hdop = kDefaultDoubleValue;

    /**
     * \brief vdop in extendedLocation
     */
    double vdop = kDefaultDoubleValue;

    /**
     * \brief usedSatelites in extendedLocation
     */
    int used_satellites = kDefaultIntValue;

    /**
     * \brief trackedSatelites in extendedLocation
     */
    int tracked_satellites = kDefaultIntValue;

    /**
     * \brief visibleSatelites in extendedLocation
     */
    int visible_satellites = kDefaultIntValue;

    /**
     * \brief sigmaHPosition in extendedLocation
     *        in meters
     */
    double sigma_h_position = kDefaultDoubleValue;

    /**
     * \brief sigmaAltitude in extendedLocation
     *        in meters
     */
    double sigma_altitude = kDefaultDoubleValue;

    /**
     * \brief sigmaSpeed in extendedLocation
     *        in meters
     */
    double sigma_speed = kDefaultDoubleValue;

    /**
     * \brief sigmaClimb in extendedLocation
     *        in meters
     */
    double sigma_climb = kDefaultDoubleValue;

    /**
     * \brief sigmaPressure in extendedLocation
     */
    double sigma_pressure = kDefaultDoubleValue;

    /**
     * \brief sigmaHeading in extendedLocation
     */
    int sigma_heading = kDefaultIntValue;

    /**
     * \brief gnssFixStatus in extendedLocation
     */
    GnssFixStatus gnss_fix_status = GnssFixStatus::GNSS_FIX_STATUS_UNDEFINED;

    /**
     * \brief drStatus in extendedLocation
     */
    bool dr_status = false;
    /**
     * \brief tracks whether dr_Status has been set
     */
    bool is_dr_status_set = false;

    /**
     * \brief usedSatelliteSystems in extendedLocation
     */
    uint8_t used_satellite_systems = 0;

    /**
     * \brief pressure in extendedLocation
     */
    double pressure = kDefaultDoubleValue;

    /**
     * \brief Packs the locations data as a extendedLocation
     *        into the provided ASN1C struct.
     * \return True if data was packed successfully, false otherwise.
     */
    bool GetPackedExtendedLocation (Location_t* extended_location);

    /**
     * \brief Packs the locations data as a minimumLocation
     *        into the provided ASN1C struct. Assumes all required
     *        data for minimumLocartion is valid.
     * \return True if data was packed successfully, false otherwise.
     */
    bool GetPackedMinimumLocation (MinimumSetOfLocationData_t* minimum_location);

    /**
     * \brief Check if a int field has been assigend a valid value.
     * \param[in] field The field to check.
     * \return True if the field has a valid value, false otherwise.
     */
    bool IsIntFieldValid(int field) {return field != kDefaultIntValue;}

    /**
     * \brief Check if a int field has been assigend a valid value.
     * \param[in] field The field to check.
     * \return True if the field has a valid value, false otherwise.
     */
    bool IsDoubleFieldValid(double field) {return field != kDefaultDoubleValue;}

    /**
     * \brief Check if this LocationData has enough valid data to pack a minimumLocation.
     * \return True if a minimumLocation should be possible to pack, false otherwise.
     */
    bool HasValidMinimumLocationData();

    /**
     * \brief Check if this LocationData has enough valid data to pack a minimumLocation.
     * \return True if a minimumLocation should be possible to pack, false otherwise.
     */
    bool HasValidExtendedLocationData();

    /**
     * \brief Check if this LocationData has valid data which can only be encoded
     *        as a extendedLocation.
     * \return True if LocationData contains data whcih can only be packed as a
     *         extendedLocation, false otherwise.
     */
    bool HasExtendedLocationOnlyData();

    /**
     * \brief Sets a optional field in a ASN1C struct if the provided value is valid.
     * \param[in] value The value to set.
     * \param[out] asn1c_field Pointer to the optional asn1c_field
     *                         (which is in itself by definition a pointer).
     * \return True if operation was successful, false otherwise.
     */
    bool SetOptionalIntASN1CField(int value, long** asn1c_field);

    /**
     * \brief Sets a optional field in a ASN1C struct if the provided value is valid.
     * \param[in] value The value to set.
     * \param[out] asn1c_field Pointer to the optional asn1c_field
     *                         (which is in itself by definition a pointer).
     * \return True if operation was successful, false otherwise.
     */
    bool SetOptionalDoubleASN1CField(double value, double** asn1c_field);

    /**
     * \brief Sets a optional field in a ASN1C struct if the provided value is valid.
     * \param[in] status The value to set.
     * \param[out] asn1c_status Pointer to the optional asn1c_field
     *                         (which is in itself by definition a pointer).
     * \return True if operation was successful, false otherwise.
     */
    bool GnssFixStatus2ASN1C(GnssFixStatus status, GnssFixStatus_t** asn1c_status);

    /**
     * \brief Sets a optional field in a ASN1C struct if the provided value is valid.
     * \param[in] system The value to set.
     * \param[out] asn1c_system Pointer to the optional asn1c_field
     *                         (which is in itself by definition a pointer).
     * \return True if operation was successful, false otherwise.
     */
    bool SatelliteSystems2ASN1C(uint8_t systems, SatelliteSystem_t** asn1c_systems);

    /**
     * \brief Converts GnssFixType to GnssFixStatus.
     * \param[in] fix_type The value to convert from.
     * \return The converted value of GnssFixStatus type.
     */
    GnssFixStatus GNSSFixStatusFromGNSSFixType(const GnssFixType fix_type);

    /**
     * \brief Calculates the number of used satellites from GNSSData struct.
     * \param[in] data Contains the data to calculate from.
     * \return The calculated number of satellites.
     */
    int NofUsedSatellitesFromGnssData(const GNSSData &data);

    /**
     * \brief Calculates the number of visible satellites from GNSSData struct.
     * \param[in] data Contains the data to calculate from.
     * \return The calculated number of satellites.
     */
    int NofVisibleSatellitesFromGnssData(const GNSSData &data);

    /**
     * \brief Sets the used satellites systems from GNSSData struct.
     * \param[in] data Contains the data to set from.
     * \return None.
     */
    void SetUsedSatelliteSystemsFromGNSSData(const GNSSData &data);

    /**
     * \brief Sets a field.
     * \param[in] field The field to set.
     * \param[in] value The value to set.
     * \param[in] low_boundry Lowest acceptable value.
     * \param[in] low_boundry Highest acceptable value.
     * \param[in] unset_value Value to accept as "deafult" value indicating field is unset.
     * \return False if value it outside boundry and not equal to unset_value, otherwise true.
     */
    template <typename T>
    bool SetField (T& field,
                   T value,
                   T low_boundry,
                   T high_boundry,
                   T unset_value)
    {
        bool return_value = true;

        if (value == unset_value)
        {
            field = value;
        }
        else if (value < low_boundry || value > high_boundry)
        {
            return_value = false;
        }
        else
        {
            field = value;
        }

        return return_value;
    }
};

} // namespace fsm

#endif //VOC_FRAMEWORK_SIGNALS_LOCATION_DATA_H_

/** \}    end of addtogroup */
