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
 *
 * @file vpom_gdbus_client_proxy.hpp
 * @brief This file handles the client proxy interface for VPOM
 *
 */

#ifndef VPOM_GDBUS_CLIENT_PROXY_HPP
#define VPOM_GDBUS_CLIENT_PROXY_HPP

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <list>
#include <signal.h>
#include <limits.h>
#include <getopt.h>
#include <iostream>
#include <mutex>
extern "C"
{
#include "tpsys.h"
#include "tplog.h"
#include "../vpom-ifc-generated.h"
}

#include "vpom_IPositioningService.hpp"

#define UNUSED_VAR(x) (void)(x)

class VpomGdbusClientProxy : public VpomIPositioningService {
private:
    vpomManager *vpom_proxy_method_;
    vpomManager *vpom_proxy_sig_;

    void CallbacksRegisterCb(vpom::VpomRegisteredItem item, vpom::fgen cb);
    void CallbacksDeregisterCb(vpom::VpomRegisteredItem item);
    void CallbacksRequestAdd(vpom::VpomRegisteredItem item);
    vpom::fgen CallbacksResponseCbGet(vpom::VpomRegisteredItem item);
    vpom::fgen CallbacksCbGet(vpom::VpomRegisteredItem item);

    static void GNSSPositionDataRawResponseCb(vpomManager *ifc,
                                              uint32_t datastatus,
                                              uint32_t year,
                                              uint32_t month,
                                              uint32_t day,
                                              uint32_t hour,
                                              uint32_t minute,
                                              uint32_t second,
                                              uint32_t weekNumber,
                                              uint32_t timeOfWeek,
                                              int32_t longitude,
                                              int32_t latitude,
                                              int32_t altitude,
                                              uint32_t speed,
                                              uint32_t horizontalVelocity,
                                              int32_t verticalVelocity,
                                              uint32_t heading,
                                              uint32_t magnetic_heading,
                                              bool gpsIsUsed,
                                              bool glonassIsUsed,
                                              bool galileoIsUsed,
                                              bool sbasIsUsed,
                                              bool qzssL1IsUsed,
                                              bool qzssL1SAIFIsUsed,
                                              uint32_t fixType,
                                              bool dgpsIsUsed,
                                              bool selfEphemerisDataUsage,
                                              uint32_t visible_gps,
                                              uint32_t visible_glonass,
                                              uint32_t visible_galileo,
                                              uint32_t visible_sbas,
                                              uint32_t visible_qzssL1,
                                              uint32_t visible_qzssL1SAIF,
                                              uint32_t used_gps,
                                              uint32_t used_glonass,
                                              uint32_t used_galileo,
                                              uint32_t used_sbas,
                                              uint32_t used_qzssL1,
                                              uint32_t used_qzssL1SAIF,
                                              uint32_t hdop,
                                              uint32_t vdop,
                                              uint32_t pdop,
                                              uint32_t tdop,
                                              uint32_t min_size,
                                              uint32_t max_size,
                                              uint32_t actual_size,
                                              uint32_t data_array,
                                              uint64_t request_id);

    gulong signal_connect_gnsspositiondataraw_response_;

    //Callback array holding registered callbacks
    //TODO: filter notifications to users acc. to received subscribe request type
    std::mutex callbacksMutex_[vpom::VpomNrOfRegisteredItems];
    vpom::VpomCallbackType callbacks_[vpom::VpomNrOfRegisteredItems] = {};


public:
    VpomGdbusClientProxy();

    /**
     * @brief                                   Function called by client to initialize gdbus usage
     *
     * @return                                  True if initialization was ok.
     *                                          False otherwise.
     */
    bool Init();

    /**
     * @brief                                   Function called by client to de-initialize gdbus usage
     *
     * @return                                  True if de-initialization was ok.
     *                                          False otherwise.
     */
    bool Deinit();

    /**
     * @brief   Instance getter
     */
    static VpomGdbusClientProxy &getInstance();

    //----- Methods begin -----
    /**
     * @brief                                   Function called by client to send GNSSPositionDataRaw request
     *                                          Data is retrieved from Location Manager without IHU interaction
     *
     * @param[in] request_id                    Request Id
     *
     * @return                                  True if send operation was ok.
     *                                          False otherwise.
     */
    bool GNSSPositionDataRawRequest(uint64_t request_id);
    //----- Methods end -----

    //----- Methods Callback begin -----
    //----- Methods Callback end -----

    //----- Signals callbacks begin -----
    /**
     * @brief                                   Function called by client to register callback function for GNSSPositionDataRaw Response
     *
     * @param[in] cb                            GNSSPositionDataRawResponseCb_t
     *
     * @return                                  True if registration was ok.
     *                                          False otherwise.
     */
    bool RegisterGNSSPositionDataRawResponseCb(GNSSPositionDataRawResponseCb_t cb);

    /**
     * @brief                                   Function called by client to deregister callback function for GNSSPositionDataRaw Response
     *
     * @return                                  True if deregistration was ok.
     *                                          False otherwise.
     */
    bool DeregisterGNSSPositionDataRawResponseCb(void);
    //----- Signal callbacks end -----
};

#endif //VPOM_GDBUS_CLIENT_PROXY_HPP
