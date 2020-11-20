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
 *  \file     ca_dpa_001_signal.cc
 *  \brief    CA_DPA_001 signal
 *  \author   Piotr Tomaszewski
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#include "signals/ca_dpa_001_signal.h"
#include "signals/signal_types.h"

#include "dlt/dlt.h"

DLT_IMPORT_CONTEXT(dlt_voc);

namespace volvo_on_call
{

const std::string CaDpa001Signal::oid_ = "1.3.6.1.4.1.37916.3.8.7.0.0.1";

/**************************
 * public member functions
 **************************/

CaDpa001Signal::CaDpa001Signal (fsm::CCMTransactionId& transaction_id,
                                uint16_t message_id) : fsm::CCM::CCM(transaction_id, message_id),
                                                       fsm::Signal::Signal(transaction_id, VocSignalTypes::kCaDpa001Signal)
{}


CaDpa001Signal::~CaDpa001Signal ()
{
    // free any instance data that needs freeing
}

/**
 * \brief Set command to start
 */
void CaDpa001Signal::SetCommandStart()
{
    command_ = kCommandStart;
}

/**
 * \brief Set command to stop
 */
void CaDpa001Signal::SetCommandStop()
{
    command_ = kCommandStop;
}

/**
 * \brief Set the remote connection timeout
 *
 * \param[in] timeout remote connection timeout
 */
void CaDpa001Signal::SetRemoteConnectionTimeout (struct tm timeout)
{
    remote_connection_timeout_ = timeout;
    remote_connection_timeout_set_ = true;
}

/**
 * \brief Set the pairing timeout (optional)
 *
 * \param[in] timeout pairing timeout
 */
void CaDpa001Signal::SetPairingTimeout (struct tm timeout)
{
    pairing_timeout_ = timeout;
    pairing_timeout_set_ = true;
}


/**
 * \brief Set the vehicle info (optional)
 *
 * \param[in] vin   VIN of the car
 * \param[in] model model of the car.
 * \param[in] model_year model year of the car
 */
void CaDpa001Signal::SetVehicleInfo (std::string vin, long model, long model_year)
{
    vin_ = stol(vin);
    model_ = model;
    model_year_ = model_year;
    vehicle_info_set_ = true;
}

/**************************
 * protected member functions
 **************************/

void* CaDpa001Signal::GetPackedPayload()
{

    CA_DPA_001_t* asn1c_ca_dpa_001 = (CA_DPA_001_t*) calloc(1, sizeof(CA_DPA_001_t));
    if (asn1c_ca_dpa_001)
    {
        bool success = true;

        // check and encode mandatory parameters. call super class to pack device_pairing_id
        if ((command_ == kCommandUndefined) || !DevicePairingId::PackData(&(asn1c_ca_dpa_001->id)))
        {
            success = false;
        }
        else
        {
            if (command_ == kCommandStart)
            {
                asn1c_ca_dpa_001->command.present = command_PR::command_PR_start;
                asn1c_ca_dpa_001->command.choice.start = 0;
            }
            else
            {
                asn1c_ca_dpa_001->command.present = command_PR_stop;
                asn1c_ca_dpa_001->command.choice.stop = 0;
            }
        }

        //encode optional parameters
        if (success)
        {
            //remote connection timeout
            if (remote_connection_timeout_set_)
            {
                asn1c_ca_dpa_001->remoteConnectionTimeout = asn_time2GT(NULL,
                                                                        &remote_connection_timeout_,
                                                                        1/*GMT timezone*/);
                success = (asn1c_ca_dpa_001->remoteConnectionTimeout != NULL);
            }

            //pairing timeout
            if (success && pairing_timeout_set_)
            {
                asn1c_ca_dpa_001->pairingTimeout = asn_time2GT(NULL,
                                                               &pairing_timeout_,
                                                               1/*GMT timezone*/);
                success = (asn1c_ca_dpa_001->pairingTimeout != NULL);
            }

            //vehicle info
            if (success && vehicle_info_set_)
            {
                asn1c_ca_dpa_001->vehicleInfo = (struct VehicleInfo*) calloc(1, sizeof(struct VehicleInfo));
                if (asn1c_ca_dpa_001->vehicleInfo)
                {
                    asn1c_ca_dpa_001->vehicleInfo->vin = vin_;
                    asn1c_ca_dpa_001->vehicleInfo->model = model_;
                    asn1c_ca_dpa_001->vehicleInfo->modelyear = model_year_;
                }
                else
                {
                    success = false;
                }
            }
        }

        if (!success)
        {
            asn_DEF_CA_DPA_001.free_struct(&asn_DEF_CA_DPA_001, asn1c_ca_dpa_001, 0);
            asn1c_ca_dpa_001 = nullptr;
        }
    }

    return (void*)asn1c_ca_dpa_001;
}


/**************************
 * private member functions
 **************************/

#ifdef VOC_TESTS

bool CaDpa001Signal::UnpackPayload(CA_DPA_001_t* asn1c_ca_dpa_001)
{
    bool return_value = true;

    //check if resources are available
    if (asn1c_ca_dpa_001)
    {
        // call super class to unpack device pairing id
        return_value = DevicePairingId::UnpackData(&(asn1c_ca_dpa_001->id));

        if (return_value)
        {
            switch (asn1c_ca_dpa_001->command.present)
            {
              case command_PR_NOTHING: command_ = kCommandUndefined;
                                       break;
              case command_PR_start  : command_ = kCommandStart;
                                       break;
              case command_PR_stop   : command_ = kCommandStop;
                                       break;
              default                : return_value = false;
            }
        }

        if (return_value && asn1c_ca_dpa_001-> remoteConnectionTimeout)
        {
            asn_GT2time(asn1c_ca_dpa_001-> remoteConnectionTimeout,
                        &remote_connection_timeout_,
                        1);
            remote_connection_timeout_set_ = true;

        }

        if (return_value && asn1c_ca_dpa_001-> pairingTimeout)
        {
            asn_GT2time(asn1c_ca_dpa_001-> pairingTimeout,
                        &pairing_timeout_,
                        1);
            pairing_timeout_set_ = true;
        }

        if (return_value && asn1c_ca_dpa_001-> vehicleInfo)
        {
            vin_        = asn1c_ca_dpa_001->vehicleInfo->vin;
            model_      = asn1c_ca_dpa_001->vehicleInfo->model;
            model_year_ = asn1c_ca_dpa_001->vehicleInfo->modelyear;
            vehicle_info_set_ = true;

        }

    }

    return return_value;
}

CaDpa001Signal::CaDpa001Signal (ccm_Message* ccm,
                                fsm::TransactionId& transaction_id) : fsm::CCM::CCM(ccm),
                                                                      fsm::Signal::Signal(transaction_id,
                                                                                          VocSignalTypes::kCaDpa001Signal)
{
    //asn1c struct to hold decoded data
    CA_DPA_001_t* asn1c_ca_dpa_001 = NULL;

    fs_VersionInfo version_info = GetContentVersion();

    //call super decode with input data from here
    valid_ = DecodePayload((void**)&asn1c_ca_dpa_001, &asn_DEF_CA_DPA_001, version_info);

    if (valid_)
    {
        // extract the data and store in class members
        valid_ = UnpackPayload(asn1c_ca_dpa_001);
    }

    // free decoded asn1c struct
    asn_DEF_CA_DPA_001.free_struct(&asn_DEF_CA_DPA_001, asn1c_ca_dpa_001, 0);
}


std::shared_ptr<CaDpa001Signal> CaDpa001Signal::CreateCaDpa001Signal(ccm_Message* ccm,
                                                                     fsm::TransactionId& transaction_id)
{
    CaDpa001Signal* signal = new CaDpa001Signal(ccm, transaction_id);
    if (!signal->valid_)
    {
        delete signal;
        return std::shared_ptr<CaDpa001Signal>();
    }
    else
    {
        return std::shared_ptr<CaDpa001Signal>(signal);
    }
}

#endif

} // namespace volvo_on_call

/** \}    end of addtogroup */
