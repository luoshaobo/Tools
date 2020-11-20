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
 *  \file     device_pairing_id.h
 *  \brief    VOC Service device pairing id data class.
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_SIGNALS_DEVICE_PAIRING_ID_H_
#define VOC_SIGNALS_DEVICE_PAIRING_ID_H_

#include "DevicePairingId.h"

#include <string>

namespace volvo_on_call
{

class DevicePairingId
{

 public:

    enum DevicePairingIdType
    {
        kDevicePairingIdTypeUndefined = 0,
        kDevicePairingIdTypeVin,
        kDevicePairingIdTypeUuid
    };

    /**
     * \brief Compare this DevicePairingId with another for equality.
     * \param[in] other the DevicePairingId ot compare with.
     * \return True if the the DevicePairingId holds the same data,
     *         otherwise false.
     */
    bool operator== (const DevicePairingId& other);

    /**
     * \brief Get the device pairing id in this signal.
     *
     * \param[in,out] id_type Will be set to the type of the id.
     * \param[in,out] id      The device paring id will be written to this.
     *
     * \return None.
     */
    void GetDevicePairingId(DevicePairingIdType& id_type, std::string& id);

    /**
     * \brief Set the device pairing id in the signal.
     *
     * \param[in] id_type The type of id beeing set.
     * \param[in] id      The id beeing set.
     *
     * \return True on success, false on failure.
     */
    bool SetDevicePairingId(DevicePairingIdType& id_type, std::string& id);

 protected:

    /**
     * \brief Packs the device pairing id into the provided
     *        ASN1C struct.
     * \param[out] asn1c_device_pairing_id The asn1c struct to pack the pairing id into.
     *                                     Caller must ensure this parameter is
     *                                     "deep freed" even if this call fails.
     *
     * \return True if succesfull, false otherwise. Parameters
     *         are not guaranteed to be unaffected in case of failure.
     */
    bool PackData(DevicePairingId_t* asn1c_device_pairing_id);

    /**
     * \brief Populates the device pairing id from the provided
     *        ASN1C struct.
     * \param[in] asn1c_device_pairing_id The asn1c struct to load the pairing id from.
     *
     * \return True if succesfull, false otherwise.
     */
    bool UnpackData(DevicePairingId_t* asn1c_device_pairing_id);

 private:

    /**
     * \brief Length of a VIN type identifier.
     */
    static const int kVinLength = 17;

    /**
     * \brief Length of a UUID type identifier.
     */
    static const int kUuidLength = 16;

    /**
     * \brief The type of the device pairing id.
     */
    DevicePairingIdType device_pairing_id_type_ = DevicePairingIdType::kDevicePairingIdTypeUndefined;

    /**
     * \brief the device pairing id.
     */
    std::string device_pairing_id_;

};

} // namespace volvo_on_call

#endif //VOC_SIGNALS_DEVICE_PAIRING_ID_H_

/** \}    end of addtogroup */
