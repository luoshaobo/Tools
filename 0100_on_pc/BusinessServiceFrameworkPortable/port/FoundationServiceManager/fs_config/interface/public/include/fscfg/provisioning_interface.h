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
 *  \file     provisioning_interface.h
 *  \brief    Foundation Services Provisioning subject interface
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#ifndef FSM_PROVISIONING_INTERACE_H_INC_
#define FSM_PROVISIONING_INTERACE_H_INC_

#include <memory>
#include <string>
#include <vector>

#include <fscfg/fscfg_types.h>

namespace fsm
{

class SourceInterface;
class ResourceInterface;
class ProvisionedResourceInterface;
class EncodingInterface;

/**
 * \brief Foundation Services Provisioning subject interface.
*/
class ProvisioningInterface
{
public:

    /**
     * \brief OnResourcesChanged event parameters.
     */
    struct ResourcesChangedEvent
    {
        std::vector<std::shared_ptr<ProvisionedResourceInterface>> resources;   ///< list of current resources.
    };


    /**
     * \brief Resources Changed Callback.
     */
    typedef Callback<ResourcesChangedEvent> ResourcesChangedCb;

    /**
     * \brief virtual Destructor.
     */
    virtual ~ProvisioningInterface() { }

    /**
     * \brief Registers a callback function which will get called when the resources set changes.
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
     * \brief Retrieves the highest priority source for a given resource.
     *
     * \param[in] resource given resource.
     * \param[out] source highest priority source.
     *
     * \return fscfg_kRcSuccess on success.
     */
    virtual fscfg_ReturnCode GetSource(std::shared_ptr<ProvisionedResourceInterface> resource,
                                       std::shared_ptr<SourceInterface>& source) = 0;
    /**
     * \brief Retrieves the highest priority encoding for a given resource.
     *
     * \param[in] resource given resource.
     * \param[out] encoding highest priority encoding.
     *
     * \return fscfg_kRcSuccess on success.
     */
    virtual fscfg_ReturnCode GetEncoding(std::shared_ptr<ProvisionedResourceInterface> resource,
                                         std::shared_ptr<EncodingInterface>& encoding) = 0;

    /**
     * \brief Retrieves all registered resource instances.
     *
     * \param[out] resources set of all registered resources.
     *
     * \return fscfg_kRcSuccess on success.
     */
    virtual fscfg_ReturnCode GetAll(std::vector<std::shared_ptr<ProvisionedResourceInterface>>& resources) = 0;

    /**
     * \brief Retrieve a resource by name.
     *
     * \param[in] name name of the resource.
     * @param[out] resource resource instance.
     *
     * \return fscfg_kRcSuccess on success,\n
     *         fscfg_kRcBadResource if encoding was not found.
     */
    virtual fscfg_ReturnCode Get(const std::string& name, std::shared_ptr<ProvisionedResourceInterface>& resource) = 0;
};

} // namespace fsm

#endif // FSM_PROVISIONING_INTERACE_H_INC_

/** \}    end of addtogroup */
