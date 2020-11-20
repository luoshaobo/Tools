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

#include "voc_framework/signals/location_data.h"

namespace fsm
{

static const double k2Pov32 = 0x100000000;

LocationData::GnssFixStatus LocationData::GNSSFixStatusFromGNSSFixType(const GnssFixType fix_type)
{
    GnssFixStatus ret_val = GnssFixStatus::GNSS_FIX_STATUS_UNDEFINED;

    // TODO (Dariusz): only two matching fields between LocationData::GnssFixStatus
    // and GnssFixType ?? What to do with the rest?
    if (fix_type == GnssFixType::fix2D)
        ret_val = GnssFixStatus::GNSS_FIX_STATUS_TWO_D_FIX;
    else if (fix_type == GnssFixType::fix3D)
        ret_val = GnssFixStatus::GNSS_FIX_STATUS_THREE_D_FIX;

    return ret_val;
}

int LocationData::NofUsedSatellitesFromGnssData(const GNSSData &data)
{
    int nr_of_satellites = 0;

    if (data.gnssStatus.gpsIsUsed)
        nr_of_satellites += data.satelliteInfo.nrOfSatellitesUsed.gps;
    if (data.gnssStatus.glonassIsUsed)
        nr_of_satellites += data.satelliteInfo.nrOfSatellitesUsed.glonass;
    if (data.gnssStatus.galileoIsUsed)
        nr_of_satellites += data.satelliteInfo.nrOfSatellitesUsed.galileo;

    return nr_of_satellites;
}

int LocationData::NofVisibleSatellitesFromGnssData(const GNSSData &data)
{
    int nr_of_satellites = 0;

    if (data.gnssStatus.gpsIsUsed)
        nr_of_satellites += data.satelliteInfo.nrOfSatellitesVisible.gps;
    if (data.gnssStatus.glonassIsUsed)
        nr_of_satellites += data.satelliteInfo.nrOfSatellitesVisible.glonass;
    if (data.gnssStatus.galileoIsUsed)
        nr_of_satellites += data.satelliteInfo.nrOfSatellitesVisible.galileo;

    return nr_of_satellites;
}

void LocationData::SetUsedSatelliteSystemsFromGNSSData(const GNSSData &data)
{
    if (data.gnssStatus.gpsIsUsed)
        SetUsedSatelliteSystem(SatelliteSystems::SATELLITE_SYSTEMS_GPS);
    if (data.gnssStatus.glonassIsUsed)
        SetUsedSatelliteSystem(SatelliteSystems::SATELLITE_SYSTEMS_GLONASS);
    if (data.gnssStatus.galileoIsUsed)
        SetUsedSatelliteSystem(SatelliteSystems::SATELLITE_SYSTEMS_GALILEO);
}

void LocationData::SetFromDRPositionData(const DRPositionData &data)
{
    // Position and movement
    SetLatitude(static_cast<double>(data.longlat.latitude) * 360 / k2Pov32);  // Convert from degrees/360*2^32
    SetLongitude(static_cast<double>(data.longlat.longitude) * 360 / k2Pov32);  // Convert from degrees/360*2^32
    SetHeading(data.heading);
    SetSpeed(data.speedKmph*1000/3600);  // Convert to m/s
    // Satellite info
    SetUsedSatellites(data.numSat);
    SetGNSSFixStatus(GNSSFixStatusFromGNSSFixType(data.fixType));
    SetDrStatus((data.drType == DeadReckonedType::drMapMatched) ? true : false);
    // Dilution
    SetHDop(data.hdopX10);
    // Date and time
    // TODO: do we need to check for overflow when converting uint32_t to int...
    struct tm new_value = {static_cast<int>(data.fixTime.second),
			   static_cast<int>(data.fixTime.minute),
			   static_cast<int>(data.fixTime.hour),
			   static_cast<int>(data.fixTime.day),
			   static_cast<int>(data.fixTime.month-1),  // Convert from [1..12] to [0..11]
			   static_cast<int>(data.fixTime.year-1900),  // Convert to Year-1900
                           0,  // Not used, Day of week [0..6]
                           0,  // Not used, Days in year [0-365]
                           0   // Not used, DST [-1,0,1]
                          };
    SetTimeStamp(new_value);
}

void LocationData::SetFromGNSSData(const GNSSData &data)
{
    // Position and movement
    SetLatitude(static_cast<double>(data.position.longlat.latitude) * 360 / k2Pov32);  // Convert from degrees/360*2^32
    SetLongitude(static_cast<double>(data.position.longlat.longitude) * 360 / k2Pov32);  // Convert from degrees/360*2^32
    SetAltitude(data.position.altitude/10);  // Convert from 1/10 m
    SetHeading(data.heading/100);  // Convert from 1/100 degrees
    SetSpeed(data.movement.speed/1000);  // Convert from 1/1000 m/s
    // Satellite info
    SetUsedSatelliteSystemsFromGNSSData(data);
    SetUsedSatellites(NofUsedSatellitesFromGnssData(data));
    SetVisibleSatellites(NofVisibleSatellitesFromGnssData(data));
    SetGNSSFixStatus(GNSSFixStatusFromGNSSFixType(data.positioningStatus.fixType));
    SetDrStatus(false);
    // Dilution
    SetHDop(data.precision.hdop);
    SetPDop(data.precision.pdop);
    // Date and time
    // TODO: do we need to check for overflow when converting uint32_t to int...
    struct tm new_value = {static_cast<int>(data.utcTime.second),
			   static_cast<int>(data.utcTime.minute),
			   static_cast<int>(data.utcTime.hour),
			   static_cast<int>(data.utcTime.day),
			   static_cast<int>(data.utcTime.month-1),  // Convert from [1..12] to [0..11]
			   static_cast<int>(data.utcTime.year-1900),  // Convert to Year-1900
			   0,  // Not used, Day of week [0..6]
			   0,  // Not used, Days in year [0-365]
                           0   // Not used, DST [-1,0,1]
                          };
    SetTimeStamp(new_value);
}

bool LocationData::GetPackedLocationData (Location_t* packed_location,
                                          MinimumSetOfLocationData_t* minimum_location,
                                          LocationData::LocationPr& location_pr)
{
    bool return_value = false;

    // Below calls will typically allocate dynamic memory even if they fail
    // we rely on the caller to "deep free".

    if (packed_location && minimum_location)
    {
        if (HasValidExtendedLocationData() && HasExtendedLocationOnlyData())
        {
            return_value = GetPackedExtendedLocation(packed_location);
            location_pr = LocationData::kLocationExtended;
        }
        else if (HasValidMinimumLocationData())
        {
            return_value = GetPackedMinimumLocation(minimum_location);
            location_pr = LocationData::kLocationMinimum;
        }
        // Mandatory extended location data set is smaller than minimum...
        else if (HasValidExtendedLocationData())
        {
            return_value = GetPackedExtendedLocation(packed_location);
            location_pr = LocationData::kLocationExtended;
        }
    }

    if (!return_value)
    {
        location_pr = LocationData::kLocationNothing;
    }

    return return_value;
}

bool LocationData::GetPackedExtendedLocation (Location_t* extended_location)
{
    bool return_value = true;

    // All of these should have been checked to be valid before calling this function.

    // coordiante
    extended_location->coordinate.latitude = latitude;
    extended_location->coordinate.longitude = longitude;

    // Set optional fields if valid

    // dataTimeStamp
    if (return_value && is_data_time_stamp_set)
    {
        // asn1c call, will try to allocate it and return 0 on failure
        extended_location->dataTimestamp = asn_time2GT(NULL,
                                                       &data_time_stamp,
                                                       1/*force into a GMT timezone*/);
        if (!extended_location->dataTimestamp)
        {
            return_value = false;
        }
    }

    // altitude
    if (return_value)
    {
        return_value = SetOptionalDoubleASN1CField(altitude, &(extended_location->altitude));
    }

    // heading
    if (return_value)
    {
        return_value = SetOptionalIntASN1CField(heading, &(extended_location->heading));
    }

    // speed
    if (return_value)
    {
        return_value = SetOptionalDoubleASN1CField(speed, &(extended_location->speed));
    }

    // climb
    if (return_value)
    {
        return_value = SetOptionalDoubleASN1CField(climb, &(extended_location->climb));
    }

    // rollRate
    if (return_value)
    {
        return_value = SetOptionalDoubleASN1CField(roll_rate, &(extended_location->rollRate));
    }

    // pitchRate
    if (return_value)
    {
        return_value = SetOptionalDoubleASN1CField(pitch_rate, &(extended_location->pitchRate));
    }

    // yawRate
    if (return_value)
    {
        return_value = SetOptionalDoubleASN1CField(yaw_rate, &(extended_location->yawRate));
    }

    // pdop
    if (return_value)
    {
        return_value = SetOptionalDoubleASN1CField(pdop, &(extended_location->pdop));
    }

    // hdop
    if (return_value)
    {
        return_value = SetOptionalDoubleASN1CField(hdop, &(extended_location->hdop));
    }

    // vdop
    if (return_value)
    {
        return_value = SetOptionalDoubleASN1CField(vdop, &(extended_location->vdop));
    }

    // usedSatellites
    if (return_value)
    {
        return_value = SetOptionalIntASN1CField(used_satellites, &(extended_location->usedSatellites));
    }

    // trackedSatellites
    if (return_value)
    {
        return_value = SetOptionalIntASN1CField(tracked_satellites, &(extended_location->trackedSatellites));
    }

    // visibleSatellites
    if (return_value)
    {
        return_value = SetOptionalIntASN1CField(visible_satellites, &(extended_location->visibleSatellites));
    }

    // sigmaHposition
    if (return_value)
    {
        return_value = SetOptionalDoubleASN1CField(sigma_h_position, &(extended_location->sigmaHposition));
    }

    // sigmaAltitude
    if (return_value)
    {
        return_value = SetOptionalDoubleASN1CField(sigma_altitude, &(extended_location->sigmaAltitude));
    }

    // sigmaSpeed
    if (return_value)
    {
        return_value = SetOptionalDoubleASN1CField(sigma_speed, &(extended_location->sigmaSpeed));
    }

    // sigmaClimb
    if (return_value)
    {
        return_value = SetOptionalDoubleASN1CField(sigma_climb, &(extended_location->sigmaClimb));
    }

    // gnssFixStatus
    if (return_value)
    {
        return_value = GnssFixStatus2ASN1C(gnss_fix_status, &(extended_location->gnssFixStatus));
    }

    // drStatus
    if (return_value && is_dr_status_set)
    {
        extended_location->drStatus = (BOOLEAN_t*) calloc(1, sizeof(BOOLEAN_t));
        if (!extended_location->drStatus)
        {
            return_value = false;
        }
        else
        {
            *extended_location->drStatus = dr_status;
        }
    }


    // usedSatelliteSystems
    if (return_value)
    {
        return_value = SatelliteSystems2ASN1C(used_satellite_systems,
                                              &(extended_location->usedSatelliteSystems));
    }


    // positionCanBeTrusted
    if (return_value && is_position_can_be_trusted_set)
    {
        extended_location->positionCanBeTrusted = (BOOLEAN_t*) calloc(1, sizeof(BOOLEAN_t));
        if (!extended_location->positionCanBeTrusted)
        {
            return_value = false;
        }
        else
        {
            *extended_location->positionCanBeTrusted = position_can_be_trusted;
        }
    }

    // pressure
    if (return_value)
    {
        return_value = SetOptionalDoubleASN1CField(pressure, &(extended_location->pressure));
    }

    // sigmaPressure
    if (return_value)
    {
        return_value = SetOptionalDoubleASN1CField(sigma_pressure, &(extended_location->sigmaPressure));
    }

    // sigmaHeading
    if (return_value)
    {
        return_value = SetOptionalIntASN1CField(sigma_heading, &(extended_location->sigmaHeading));
    }

    return return_value;
}

bool LocationData::GetPackedMinimumLocation (MinimumSetOfLocationData_t* minimum_location)
{
    bool return_value = true;

    // All of these should have been checked to be valid before calling this function.

    // positionCanBeTrusted
    minimum_location->positionCanBeTrusted = position_can_be_trusted;
    // vehicleLocation
    // should be encoded as millarcseconds per asn.1 schema
    minimum_location->vehicleLocation.positionLatitude = DegreesToMAS(latitude);
    minimum_location->vehicleLocation.positionLongitude = DegreesToMAS(longitude);
    // vehicleDirection
    minimum_location->vehicleDirection = heading;


    // Set optional fields if valid

    // recentVehicleLocationN1
    if (return_value && IsDoubleFieldValid(latitude_delta_1) &&
        IsDoubleFieldValid(longitude_delta_1))
    {
        minimum_location->recentVehicleLocationN1 =
            (VehicleLocationDelta_t*) calloc(1, sizeof(VehicleLocationDelta_t));

        if (!minimum_location->recentVehicleLocationN1)
        {
            return_value = false;
        }
        else
        {
            // location delta should be encoded in units of 100 milliarcseconds per asn.1 schema
            minimum_location->recentVehicleLocationN1->latitudeDelta
                = DegreesToMAS(latitude_delta_1 / 100.0);
            minimum_location->recentVehicleLocationN1->longitudeDelta
                = DegreesToMAS(longitude_delta_1 / 100.0);
        }
    }

    // recentVehicleLocationN2
    if (return_value && IsDoubleFieldValid(latitude_delta_2) && IsDoubleFieldValid(longitude_delta_2))
    {
        minimum_location->recentVehicleLocationN2 =
            (VehicleLocationDelta_t*) calloc(1, sizeof(VehicleLocationDelta_t));

        if (!minimum_location->recentVehicleLocationN2)
        {
            return_value = false;
        }
        else
        {
            // location delta should be encoded in units of 100 milliarcseconds per asn.1 schema
            minimum_location->recentVehicleLocationN2->latitudeDelta =
                DegreesToMAS(latitude_delta_2 / 100.0);
            minimum_location->recentVehicleLocationN2->longitudeDelta =
                DegreesToMAS(longitude_delta_2 / 100.0);
        }
    }

    return return_value;
}

bool LocationData::HasValidMinimumLocationData()
{
    bool valid = true;

    valid = valid && is_position_can_be_trusted_set;
    valid = valid && IsDoubleFieldValid(latitude);
    valid = valid && IsDoubleFieldValid(longitude);
    valid = valid && IsIntFieldValid(heading);

    return valid;
}

bool LocationData::HasValidExtendedLocationData()
{
    bool valid = true;

    // Fun fact of the day, ExtendedLocation has less mandatory data
    // than minimum.

    valid = valid && IsDoubleFieldValid(latitude);
    valid = valid && IsDoubleFieldValid(longitude);

    return valid;
}

bool LocationData::HasExtendedLocationOnlyData()
{
    bool extended_only = false;

    extended_only = extended_only || is_data_time_stamp_set;
    extended_only = extended_only || IsDoubleFieldValid(altitude);
    extended_only = extended_only || IsDoubleFieldValid(speed);
    extended_only = extended_only || IsDoubleFieldValid(climb);
    extended_only = extended_only || IsDoubleFieldValid(roll_rate);
    extended_only = extended_only || IsDoubleFieldValid(pitch_rate);
    extended_only = extended_only || IsDoubleFieldValid(yaw_rate);
    extended_only = extended_only || IsDoubleFieldValid(pdop);
    extended_only = extended_only || IsDoubleFieldValid(hdop);
    extended_only = extended_only || IsDoubleFieldValid(vdop);
    extended_only = extended_only || IsIntFieldValid(used_satellites);
    extended_only = extended_only || IsIntFieldValid(tracked_satellites);
    extended_only = extended_only || IsIntFieldValid(visible_satellites);
    extended_only = extended_only || IsDoubleFieldValid(sigma_h_position);
    extended_only = extended_only || IsDoubleFieldValid(sigma_altitude);
    extended_only = extended_only || IsDoubleFieldValid(sigma_speed);
    extended_only = extended_only || IsDoubleFieldValid(sigma_climb);
    extended_only = extended_only || IsDoubleFieldValid(sigma_pressure);
    extended_only = extended_only || IsIntFieldValid(sigma_heading);
    extended_only = extended_only ||
                        gnss_fix_status != GnssFixStatus::GNSS_FIX_STATUS_UNDEFINED;
    extended_only = extended_only || is_dr_status_set;
    extended_only = extended_only ||
                        used_satellite_systems != SatelliteSystems::SATELLITE_SYSTEMS_UNDEFINED;
    extended_only = extended_only || IsDoubleFieldValid(pressure);

    return extended_only;
}

bool LocationData::SetOptionalIntASN1CField(int value, long** asn1c_field)
{
    bool return_value = true;

    if (!asn1c_field)
    {
        return_value = false;
    }

    if (return_value && IsIntFieldValid(value))
    {
        long* new_int = NULL;
        new_int = (long*) calloc(1, sizeof(long));

        if (!new_int)
        {
            return_value = false;
        }
        else
        {
            *new_int = value;
            *asn1c_field = new_int;
        }
    }

    return return_value;
}

bool LocationData::SetOptionalDoubleASN1CField(double value, double** asn1c_field)
{
    bool return_value = true;

    if (!asn1c_field)
    {
        return_value = false;
    }

    if (return_value && IsDoubleFieldValid(value))
    {
        double* new_double = NULL;
        new_double = (double*) calloc(1, sizeof(double));

        if (!new_double)
        {
            return_value = false;
        }
        else
        {
            *new_double = value;
            *asn1c_field = new_double;
        }
    }

    return return_value;
}

bool LocationData::GnssFixStatus2ASN1C(GnssFixStatus status, GnssFixStatus_t** asn1c_status)
{

    bool return_value = true;

    if (!asn1c_status)
    {
        return_value = false;
    }

    if (return_value && status != GNSS_FIX_STATUS_UNDEFINED)
    {
        GnssFixStatus_t* new_status = (GnssFixStatus_t*) calloc(1, sizeof(e_GnssFixStatus));

        if (!new_status)
        {
            return_value = false;
        }
        else
        {
            switch (gnss_fix_status)
            {
            case GnssFixStatus::GNSS_FIX_STATUS_NO_FIX :
                *new_status = GnssFixStatus_noFix;
                break;
            case GnssFixStatus::GNSS_FIX_STATUS_TIME_FIX :
                *new_status = GnssFixStatus_timeFix;
                break;
            case GnssFixStatus::GNSS_FIX_STATUS_TWO_D_FIX :
                *new_status = GnssFixStatus_twoDFix;
                break;
            case GnssFixStatus::GNSS_FIX_STATUS_THREE_D_FIX :
                *new_status = GnssFixStatus_threeDdFix;
                break;
            case GnssFixStatus::GNSS_FIX_STATUS_UNDEFINED :
            default :
                //very strange
                free(new_status);
                new_status = NULL;
                return_value = false;
            }
        }

        if (new_status)
        {
            *asn1c_status = new_status;
        }
    }

    return return_value;
}

bool LocationData::SatelliteSystems2ASN1C(uint8_t systems, SatelliteSystem_t** asn1c_systems)
{
    bool return_value = true;

    if (!asn1c_systems)
    {
        return_value = false;
    }

    if (return_value && systems != SATELLITE_SYSTEMS_UNDEFINED)
    {
        SatelliteSystem_t* new_system = (SatelliteSystem_t*) calloc(1, sizeof(SatelliteSystem_t));

        if (!new_system)
        {
            return_value = false;
        }
        else
        {
            uint8_t* bit_string = (uint8_t*)calloc(1, sizeof(uint8_t));

            if (!bit_string)
            {
                free(new_system);
                new_system = NULL;
                return_value = false;
            }
            else
            {
                if (systems & SATELLITE_SYSTEMS_GPS)
                {
                    *bit_string |= 1 << (7 - SatelliteSystem_gps);
                }
                if (systems & SATELLITE_SYSTEMS_GLONASS)
                {
                    *bit_string |= 1 << (7 - SatelliteSystem_glonass);
                }
                if (systems & SATELLITE_SYSTEMS_GALILEO)
                {
                    *bit_string |= 1 << (7 - SatelliteSystem_galileo);
                }
                if (systems & SATELLITE_SYSTEMS_BEIDOU)
                {
                    *bit_string |= 1 << (7 - SatelliteSystem_beidou);
                }

                new_system->buf = bit_string;
                new_system->size = 1;
                new_system->bits_unused = 4;

                *asn1c_systems = new_system;
            }
        }
    }

    return return_value;
}

void LocationData::SetPositionCanBeTrusted (bool value_to_set)
{
    position_can_be_trusted = value_to_set;
    is_position_can_be_trusted_set = true;
}

bool LocationData::SetLatitude (double value_to_set)
{
    return SetField(latitude, value_to_set, -90.0, 90.0, kDefaultDoubleValue);
}

bool LocationData::SetLongitude (double value_to_set)
{
    return SetField(longitude, value_to_set, -180.0, 180.0, kDefaultDoubleValue);
}

bool LocationData::SetHeading (int value_to_set)
{
    return SetField(heading, value_to_set, 0, 179, kDefaultIntValue);
}

bool LocationData::SetLatitudeDelta1 (double value_to_set)
{
    return SetField(latitude_delta_1,
                    value_to_set,
                    -0.01422,
                    0.01419,
                    kDefaultDoubleValue);
}

bool LocationData::SetLatitudeDelta2 (double value_to_set)
{
    return SetField(latitude_delta_2,
                    value_to_set,
                    -0.01422,
                    0.01419,
                    kDefaultDoubleValue);
}

bool LocationData::SetLongitudeDelta1 (double value_to_set)
{
    return SetField(longitude_delta_1,
                    value_to_set,
                    -0.01422,
                    0.01419,
                    kDefaultDoubleValue);
}

bool LocationData::SetLongitudeDelta2 (double value_to_set)
{
    return SetField(longitude_delta_2,
                    value_to_set,
                    -0.01422,
                    0.01419,
                    kDefaultDoubleValue);
}

void LocationData::SetTimeStamp (struct tm data_time_stamp_to_set)
{
    data_time_stamp = data_time_stamp_to_set;
    is_data_time_stamp_set = true;
}

void LocationData::SetAltitude (double value_to_set)
{
    altitude = value_to_set;
}

bool LocationData::SetSpeed (double value_to_set)
{
    return SetField(speed,
                    value_to_set,
                    0.0,
                    200.0,
                    kDefaultDoubleValue);
}

bool LocationData::SetClimb (double value_to_set)
{
    return SetField(climb,
                    value_to_set,
                    -180.0,
                    180.0,
                    kDefaultDoubleValue);
}

bool LocationData::SetRollRate (double value_to_set)
{
    return SetField(roll_rate,
                    value_to_set,
                    -100.0,
                    100.0,
                    kDefaultDoubleValue);
}

bool LocationData::SetPitchRate (double value_to_set)
{
    return SetField(pitch_rate,
                    value_to_set,
                    -100.0,
                    100.0,
                    kDefaultDoubleValue);
}

bool LocationData::SetYawRate (double value_to_set)
{
    return SetField(yaw_rate,
                    value_to_set,
                    -100.0,
                    100.0,
                    kDefaultDoubleValue);
}

void LocationData::SetPDop(double value_to_set)
{
    pdop = value_to_set;
}

void LocationData::SetHDop(double value_to_set)
{
    hdop = value_to_set;
}

void LocationData::SetVDop(double value_to_set)
{
    vdop = value_to_set;
}

void LocationData::SetUsedSatellites(int value_to_set)
{
    used_satellites = value_to_set;
}

void LocationData::SetTrackedSatellites(int value_to_set)
{
    tracked_satellites = value_to_set;
}

void LocationData::SetVisibleSatellites(int value_to_set)
{
    visible_satellites = value_to_set;
}

void LocationData::SetSigmaHPosition(double value_to_set)
{
    sigma_h_position = value_to_set;
}

void LocationData::SetSigmaAltitude(double value_to_set)
{
    sigma_altitude = value_to_set;
}

void LocationData::SetSigmaSpeed(double value_to_set)
{
    sigma_speed = value_to_set;
}

void LocationData::SetSigmaClimb(double value_to_set)
{
    sigma_climb = value_to_set;
}

void LocationData::SetSigmaPressure(double value_to_set)
{
    sigma_pressure = value_to_set;
}

bool LocationData::SetSigmaHeading(int value_to_set)
{
    return SetField(sigma_heading,
                    value_to_set,
                    0,
                    180,
                    kDefaultIntValue);
}

bool LocationData::SetGNSSFixStatus(GnssFixStatus status)
{

    bool return_value = true;

    // Verify input is valid, if enume ever starts growing, consider
    // adding an end marker and chaking >= instead.
    switch (status)
    {
    case GNSS_FIX_STATUS_UNDEFINED :
    case GNSS_FIX_STATUS_NO_FIX :
    case GNSS_FIX_STATUS_TIME_FIX :
    case GNSS_FIX_STATUS_TWO_D_FIX :
    case GNSS_FIX_STATUS_THREE_D_FIX :
        return_value = true;
        break;
    default:
        return_value = false;
    }

    gnss_fix_status = status;

    return return_value;
}

void LocationData::SetDrStatus(bool status)
{
    dr_status = status;
    is_dr_status_set = false;
}

bool LocationData::SetUsedSatelliteSystem(SatelliteSystems system)
{
    bool return_value = true;

    // Verify input is valid, if enume ever starts growing, consider
    // adding an end marker and chaking >= instead.
    switch (system)
    {
    case SATELLITE_SYSTEMS_UNDEFINED :
    case SATELLITE_SYSTEMS_GPS :
    case SATELLITE_SYSTEMS_GLONASS :
    case SATELLITE_SYSTEMS_GALILEO :
    case SATELLITE_SYSTEMS_BEIDOU :
        return_value = true;
        break;
    default:
        return_value = false;
    }

    if (return_value)
    {
        if (system == SatelliteSystems::SATELLITE_SYSTEMS_UNDEFINED)
        {
            // reset
            used_satellite_systems = SatelliteSystems::SATELLITE_SYSTEMS_UNDEFINED;
        }
        else
        {
            // set the bit corresponding to the added system
            used_satellite_systems = used_satellite_systems | system;
        }
    }

    return return_value;
}

void LocationData::SetPressure(double value_to_set)
{
    pressure = value_to_set;
}

bool LocationData::operator== (const LocationData& other)
{

    // This relies on all parameters beeing assigned default values even if not set.

    bool return_value = true;

    return_value = return_value && this->position_can_be_trusted == other.position_can_be_trusted;
    return_value = return_value && this->latitude == other.latitude;
    return_value = return_value && this->longitude == other.longitude;
    return_value = return_value && this->heading == other.heading;
    return_value = return_value && this->latitude_delta_1 == other.latitude_delta_1;
    return_value = return_value && this->latitude_delta_2 == other.latitude_delta_2;
    return_value = return_value && this->longitude_delta_1 == other.longitude_delta_1;
    return_value = return_value && this->longitude_delta_2 == other.longitude_delta_2;

    // data_time_stamp
    // the two temp structs are needed as mktime may modify the input.
    struct tm this_tmp_data_time_stamp = this->data_time_stamp;
    struct tm other_tmp_data_time_stamp = other.data_time_stamp;
    time_t this_data_time_stamp = mktime(&this_tmp_data_time_stamp);
    time_t other_data_time_stamp = mktime(&other_tmp_data_time_stamp);
    return_value = return_value && this_data_time_stamp == other_data_time_stamp;

    return_value = return_value && this->altitude == other.altitude;
    return_value = return_value && this->speed == other.speed;
    return_value = return_value && this->climb == other.climb;
    return_value = return_value && this->roll_rate == other.roll_rate;
    return_value = return_value && this->pitch_rate == other.pitch_rate;
    return_value = return_value && this->yaw_rate == other.yaw_rate;
    return_value = return_value && this->pdop == other.pdop;
    return_value = return_value && this->hdop == other.hdop;
    return_value = return_value && this->vdop == other.vdop;
    return_value = return_value && this->used_satellites == other.used_satellites;
    return_value = return_value && this->tracked_satellites == other.tracked_satellites;
    return_value = return_value && this->visible_satellites == other.visible_satellites;
    return_value = return_value && this->sigma_h_position == other.sigma_h_position;
    return_value = return_value && this->sigma_altitude == other.sigma_altitude;
    return_value = return_value && this->sigma_speed == other.sigma_speed;
    return_value = return_value && this->sigma_climb == other.sigma_climb;
    return_value = return_value && this->sigma_pressure == other.sigma_pressure;
    return_value = return_value && this->sigma_heading == other.sigma_heading;
    return_value = return_value && this->gnss_fix_status == other.gnss_fix_status;
    return_value = return_value && this->dr_status == other.dr_status;
    return_value = return_value && this->used_satellite_systems == other.used_satellite_systems;
    return_value = return_value && this->pressure == other.pressure;

    return return_value;
}


#ifdef VOC_TESTS
bool LocationData::UnpackMinimumLocation(MinimumSetOfLocationData_t* minimum_location)
{
    bool return_value = true;

    // First do non optional fields

    if (return_value)
    {
        // returns void
        SetPositionCanBeTrusted(minimum_location->positionCanBeTrusted);
    }
    if (return_value)
    {
        return_value =
                SetLatitude(MASToDegrees(minimum_location->vehicleLocation.positionLatitude));
    }
    if (return_value)
    {
        return_value =
                SetLongitude(MASToDegrees(minimum_location->vehicleLocation.positionLongitude));
    }
    if (return_value)
    {
        return_value = SetHeading(minimum_location->vehicleDirection);
    }

    // Then do optional fields

    if (return_value && minimum_location->recentVehicleLocationN1)
    {
        return_value =
                SetLatitudeDelta1(
                    MASToDegrees(minimum_location->recentVehicleLocationN1->latitudeDelta * 100));
    }
    if (return_value && minimum_location->recentVehicleLocationN1)
    {
        return_value =
                SetLongitudeDelta1(
                    MASToDegrees(minimum_location->recentVehicleLocationN1->longitudeDelta * 100));
    }
    if (return_value && minimum_location->recentVehicleLocationN2)
    {
        return_value =
                SetLatitudeDelta2(
                    MASToDegrees(minimum_location->recentVehicleLocationN2->latitudeDelta * 100));
    }
    if (return_value && minimum_location->recentVehicleLocationN2)
    {
        return_value =
                SetLongitudeDelta2(
                    MASToDegrees(minimum_location->recentVehicleLocationN2->longitudeDelta * 100));
    }

    return return_value;
}

bool LocationData::UnpackExtendedLocation(Location_t* extended_location)
{
    bool return_value = true;

    // First do non optional fields

    if (return_value)
    {
        return_value = SetLatitude(extended_location->coordinate.latitude);
    }
    if (return_value)
    {
        return_value = SetLongitude(extended_location->coordinate.longitude);
    }

    // Then do optional fields

    if (return_value && extended_location->dataTimestamp)
    {
        struct tm timestamp = {};

        time_t ret = asn_GT2time(extended_location->dataTimestamp,
                                &timestamp,
                                1/*force into a GMT timezone*/);

        if (ret != -1)
        {
            // returns void
            SetTimeStamp(timestamp);
        }
        else
        {
            return_value = false;
        }
    }
    if (return_value && extended_location->altitude)
    {
        //returns void
        SetAltitude(*(extended_location->altitude));
    }
    if (return_value && extended_location->heading)
    {
        return_value = SetHeading(*(extended_location->heading));
    }
    if (return_value && extended_location->speed)
    {
        return_value = SetSpeed(*(extended_location->speed));
    }
    if (return_value && extended_location->climb)
    {
        return_value = SetClimb(*(extended_location->climb));
    }
    if (return_value && extended_location->rollRate)
    {
        return_value = SetRollRate(*(extended_location->rollRate));
    }
    if (return_value && extended_location->pitchRate)
    {
        return_value = SetPitchRate(*(extended_location->pitchRate));
    }
    if (return_value && extended_location->yawRate)
    {
        return_value = SetYawRate(*(extended_location->yawRate));
    }
    if (return_value && extended_location->pdop)
    {
        //returns void
        SetPDop(*(extended_location->pdop));
    }
    if (return_value && extended_location->hdop)
    {
        //returns void
        SetHDop(*(extended_location->hdop));
    }
    if (return_value && extended_location->vdop)
    {
        //returns void
        SetVDop(*(extended_location->vdop));
    }
    if (return_value && extended_location->usedSatellites)
    {
        //returns void
        SetUsedSatellites(*(extended_location->usedSatellites));
    }
    if (return_value && extended_location->trackedSatellites)
    {
        //returns void
        SetTrackedSatellites(*(extended_location->trackedSatellites));
    }
    if (return_value && extended_location->visibleSatellites)
    {
        //returns void
        SetVisibleSatellites(*(extended_location->visibleSatellites));
    }
    if (return_value && extended_location->sigmaHposition)
    {
        //returns void
        SetSigmaHPosition(*(extended_location->sigmaHposition));
    }
    if (return_value && extended_location->sigmaAltitude)
    {
        //returns void
        SetSigmaAltitude(*(extended_location->sigmaAltitude));
    }
    if (return_value && extended_location->sigmaSpeed)
    {
        //returns void
        SetSigmaSpeed(*(extended_location->sigmaSpeed));
    }
    if (return_value && extended_location->sigmaClimb)
    {
        //returns void
        SetSigmaClimb(*(extended_location->sigmaClimb));
    }
    if (return_value && extended_location->gnssFixStatus)
    {
        switch(*(extended_location->gnssFixStatus))
        {
        case GnssFixStatus_noFix :
            if (!SetGNSSFixStatus(GNSS_FIX_STATUS_NO_FIX))
            {
                return_value = false;
            }
            break;
        case GnssFixStatus_timeFix :
            if (!SetGNSSFixStatus(GNSS_FIX_STATUS_TIME_FIX))
            {
                return_value = false;
            }
            break;
        case GnssFixStatus_twoDFix :
            if (!SetGNSSFixStatus(GNSS_FIX_STATUS_TWO_D_FIX))
            {
                return_value = false;
            }
            break;
        case GnssFixStatus_threeDdFix :
            if (!SetGNSSFixStatus(GNSS_FIX_STATUS_THREE_D_FIX))
            {
                return_value = false;
            }
            break;
        default:
            return_value = false;
        }
    }
    if (return_value && extended_location->drStatus)
    {
        //returns void
        SetDrStatus(*(extended_location->drStatus));
    }
    if (return_value && extended_location->usedSatelliteSystems)
    {
        if ((extended_location->usedSatelliteSystems->buf) &&
                (extended_location->usedSatelliteSystems->size > 0))
        {
            if (extended_location->usedSatelliteSystems->buf[0] & (1 << (7 - SatelliteSystem_gps)))
            {
                if (!SetUsedSatelliteSystem(SATELLITE_SYSTEMS_GPS))
                {
                    return_value = false;
                }
            }
            if (extended_location->usedSatelliteSystems->buf[0] & (1 << (7 - SatelliteSystem_glonass)))
            {
                if (!SetUsedSatelliteSystem(SATELLITE_SYSTEMS_GLONASS))
                {
                    return_value = false;
                }
            }
            if (extended_location->usedSatelliteSystems->buf[0] & (1 << (7 - SatelliteSystem_galileo)))
            {
                if (!SetUsedSatelliteSystem(SATELLITE_SYSTEMS_GALILEO))
                {
                    return_value = false;
                }
            }
            if (extended_location->usedSatelliteSystems->buf[0] & (1 << (7 - SatelliteSystem_beidou)))
            {
                if (!SetUsedSatelliteSystem(SATELLITE_SYSTEMS_BEIDOU))
                {
                    return_value = false;
                }
            }
        }
        else
        {
            return_value = false;
        }
    }
    if (return_value && extended_location->positionCanBeTrusted)
    {
        //returns void
        SetPositionCanBeTrusted(*(extended_location->positionCanBeTrusted));
    }
    if (return_value && extended_location->pressure)
    {
        //returns void
        SetPressure(*(extended_location->pressure));
    }
    if (return_value && extended_location->sigmaPressure)
    {
        //returns void
        SetSigmaPressure(*(extended_location->sigmaPressure));
    }
    if (return_value && extended_location->sigmaHeading)
    {
        return_value = SetSigmaHeading(*(extended_location->sigmaHeading));
    }

    return return_value;
}
#endif

} // namespace fsm

/** \}    end of addtogroup */
