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
 *  \file     device_pairing_id.cc
 *  \brief    VOC Service device pairing id data class.
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#include "signals/device_pairing_id.h"

namespace volvo_on_call
{

bool DevicePairingId::operator== (const DevicePairingId& other)
{
    return other.device_pairing_id_type_ == device_pairing_id_type_ &&
        other.device_pairing_id_ == device_pairing_id_;
}

void DevicePairingId::GetDevicePairingId(DevicePairingIdType& id_type, std::string& id)
{
    id_type = device_pairing_id_type_;
    id = device_pairing_id_;
}

bool DevicePairingId::SetDevicePairingId(DevicePairingIdType& id_type, std::string& id)
{
    bool return_value = true;

    if (id_type == DevicePairingIdType::kDevicePairingIdTypeVin &&
        id.size() != kVinLength)
    {
        return_value = false;
    }
    else if (id_type == DevicePairingIdType::kDevicePairingIdTypeUuid &&
             id.size() != kUuidLength)
    {
        return_value = false;
    }
    else if (id_type == DevicePairingIdType::kDevicePairingIdTypeUndefined)
    {
        return_value = false;
    }

    if (return_value)
    {
        device_pairing_id_type_ = id_type;
        device_pairing_id_ = id;
    }

    return return_value;
}

bool DevicePairingId::PackData(DevicePairingId_t* asn1c_device_pairing_id)
{
    bool return_value = false;

    if (asn1c_device_pairing_id)
    {
        if (device_pairing_id_type_ == DevicePairingIdType::kDevicePairingIdTypeVin)
        {
            asn1c_device_pairing_id->present = DevicePairingId_PR_vin;
            OCTET_STRING_fromBuf(&(asn1c_device_pairing_id->choice.vin),
                                 device_pairing_id_.data(),
                                 device_pairing_id_.size());

            return_value = true;
        }
        else if (device_pairing_id_type_ == DevicePairingIdType::kDevicePairingIdTypeUuid)
        {
            asn1c_device_pairing_id->present = DevicePairingId_PR_uuid;
            OCTET_STRING_fromBuf(&(asn1c_device_pairing_id->choice.uuid),
                                 device_pairing_id_.data(),
                                 device_pairing_id_.size());

            return_value = true;
        }
    }

    return return_value;
}

bool DevicePairingId::UnpackData(DevicePairingId_t* asn1c_device_pairing_id)
{
    bool return_value = false;

    if (asn1c_device_pairing_id)
    {
        if (asn1c_device_pairing_id->present == DevicePairingId_PR_vin)
        {
            device_pairing_id_type_ = DevicePairingIdType::kDevicePairingIdTypeVin;
            device_pairing_id_.assign(asn1c_device_pairing_id->choice.vin.buf,
                                      asn1c_device_pairing_id->choice.vin.buf + asn1c_device_pairing_id->choice.vin.size);

            return_value = true;
        }
        else if (asn1c_device_pairing_id->present == DevicePairingId_PR_uuid)
        {
            device_pairing_id_type_ = DevicePairingIdType::kDevicePairingIdTypeUuid;
            device_pairing_id_.assign(asn1c_device_pairing_id->choice.uuid.buf,
                                      asn1c_device_pairing_id->choice.uuid.buf + asn1c_device_pairing_id->choice.uuid.size);

            return_value = true;
        }
    }

    return return_value;
}

} // namespace volvo_on_call
/** \}    end of addtogroup */
