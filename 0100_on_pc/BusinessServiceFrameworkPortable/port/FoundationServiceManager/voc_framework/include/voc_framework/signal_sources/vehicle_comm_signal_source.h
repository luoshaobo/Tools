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
 *  \file     vehicle_comm_signal_source.h
 *  \brief    Vehicle Comm signal source class.
 *  \author   Florian Schindler
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_FRAMEWORK_SIGNAL_SOURCES_VEHICLE_COMM_SIGNAL_SOURCE_H_
#define VOC_FRAMEWORK_SIGNAL_SOURCES_VEHICLE_COMM_SIGNAL_SOURCE_H_


#include "voc_framework/signal_sources/signal_source.h"
#include "vehiclecomm/vehicle_comm_interface.hpp"


namespace fsm
{

/*!
* \class VehicleCommSignalSource
* \brief Implements a signal source class for callbacks from Vehicle Service
* For sending request functions in vehicle_comm_client.hpp shall be used directly, e.g. Request_HornAndLight
*
* \author Florian Schindler
*/
class VehicleCommSignalSource : public SignalSource
{
    public:

        /*!
         * \fn     GetInstance
         * \brief  returns instance of this singleton object
         *
         * \author Florian Schindler
         *
         * \return  VehicleCommSignalSource reference
        */
        static VehicleCommSignalSource& GetInstance();

        /*!
         * \fn     GetVehicleCommClientObject
         * \brief  returns instance of the encapsulated VehicleCommClient object
         *
         * \author Florian Schindler
         *
         * \return  VehicleCommClient reference
        */
        vc::IVehicleComm& GetVehicleCommClientObject();

        VehicleCommSignalSource(VehicleCommSignalSource const&) = delete;  //!< shall not be used
        void operator = (VehicleCommSignalSource const&) = delete;  //!< shall not be used

    private:

        /*!
         * \fn     VehicleCommSignalSource
         * \brief  constructor
         *
         * \author Florian Schindler
        */
        VehicleCommSignalSource();

        /*!
         * \fn     ~VehicleCommSignalSource
         * \brief  destructor
         *
         * \author Florian Schindler
        */
        ~VehicleCommSignalSource();

        /*!
         * \fn     VehicleCommSignalFactory
         * \brief  Creates a specific Signal from the provided VehicleComm data
         *
         * \param[in]  vcmessagebase  message with contents from Vehicle Comm
         * \param[in]  status  status provided by Vehicle Comm, \ref vc::ReturnValue
         *
         * \return signal pointer with payload; NULL pointer (or empty signal) in case of error
        */
        std::shared_ptr<Signal> VehicleCommSignalFactory(vc::MessageBase* vcmessagebase, vc::ReturnValue status);

        /*!
         * \fn     ResponseCallback
         * \brief  Callback method to be registered at Vehicle Comm for responses
         *
         * \author Florian Schindler
         *
         * \param[in]  vcmessagebase  message with contents from Vehicle Comm
         * \param[in]  status  status provided by Vehicle Comm, \ref vc::ReturnValue
        */
        void ResponseCallback(vc::MessageBase* vcmessagebase, vc::ReturnValue status);

        /*!
         * \fn     EventCallback
         * \brief  Callback method to be registered at Vehicle Comm for events
         *
         * \author Florian Schindler
         *
         * \param[in]  vcmessagebase  message with contents from Vehicle Comm
        */
        void EventCallback(vc::MessageBase* vcmessagebase);

        vc::IVehicleComm& ivc_;

};

} // namespace fsm

#endif  // VOC_FRAMEWORK_SIGNAL_SOURCES_VEHICLE_COMM_SIGNAL_SOURCE_H_

/** \}    end of addtogroup */
