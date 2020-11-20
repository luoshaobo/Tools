/**
 * Copyright (C) 2017, 2018 Continental Automotive GmbH
 *
 * Licensed under the "Supplier unrestricted white box" category, as
 * described in the 20160322-1_SW-amendment TCAM.pdf agreement
 * between Volvo Cars and Continental Automotive GmbH.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *  \file     discovery_interface.h
 *  \brief    Foundation Services Discovery subject interface
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#ifndef FSM_DISCOVERY_INTERACE_H_INC_
#define FSM_DISCOVERY_INTERACE_H_INC_

#include <memory>
#include <string>
#include <vector>

#include <fscfg/fscfg_types.h>

#include <fscfg/feature_interface.h>

namespace fsm
{

/**
 * \brief Foundation Services Discovery subject interface.
*/
class DiscoveryInterface
{
public:

    /**
     * \brief BindResourcesChanged event parameters.
     */
    struct ResourcesChangedEvent
    {
        std::vector<std::shared_ptr<FeatureInterface>> features;   ///< List of current features.
    };
    /**
     * \brief Resources Changed Callback.
     */
    typedef Callback<ResourcesChangedEvent> ResourcesChangedCb;

    /**
     * \brief virtual Destructor.
     */
    virtual ~DiscoveryInterface() { }

    /**
     * \brief Registers a callback function which will get called when the features set changes.
     *
     * \param[in] func function object acting as callback.
     * \param[out] id unique binding identifier.
     *
     * \return fscfg_kRcSuccess on success.
     */
    virtual fscfg_ReturnCode BindResourcesChanged(ResourcesChangedCb func, std::uint32_t& id) = 0;

    /**
     * \brief Unbind existing callback function.
     *
     * \param[in] id binding unique identifier.
     *
     * \return fscfg_kRcSuccess on success.
     */
    virtual fscfg_ReturnCode Unbind(std::uint32_t id) = 0;

    /**
     * \brief retrieves all registered feature instances.
     *
     * \param[out] features set of all registered encodings.
     *
     * \return fscfg_kRcSuccess on success.
     */
    virtual fscfg_ReturnCode GetAll(std::vector<std::shared_ptr<FeatureInterface>>& features) = 0;

    /**
     * \brief retrieve feature by name.
     *
     * \param[in] name name of the encoding.
     * \param[out] feature feature instance.
     *
     * \return fscfg_kRcSuccess on success,\n
     *         fscfg_kRcBadResource if feature was not found.
     */
    virtual fscfg_ReturnCode Get(const std::string& name, std::shared_ptr<FeatureInterface>& feature) = 0;

    /**
     * \brief Retrieve Cloud connection status details.
     *
     * \param[out] certificate_validity certificate validity status.
     * \param[out] central_connect_status last connection status for central node.
     * \param[out] central_server_uri central node server URI.
     * \param[out] central_date_time last successful connection to central node date and time.
     * \param[out] regional_connect_status last connection status for central node.
     * \param[out] regional_server_uri central node server URI.
     * \param[out] regional_date_time last successful connection to central node date and time.
     *
     * \return fscfg_kRcSuccess on success.
     */
    virtual fscfg_ReturnCode GetCloudConnectionStatus(char& certificate_validity,
                                                      char& central_connect_status,
                                                      std::string& central_server_uri,
                                                      std::string& central_date_time,
                                                      char& regional_connect_status,
                                                      std::string& regional_server_uri,
                                                      std::string& regional_date_time) = 0;
};

} // namespace fsm

#endif // FSM_DISCOVERY_INTERACE_H_INC_

/** \}    end of addtogroup */
