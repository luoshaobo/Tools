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
 *  \file     config_interface.h
 *  \brief    Foundation Services Config subject interface
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#ifndef FSM_CONFIG_INTERFACE_H_INC_
#define FSM_CONFIG_INTERFACE_H_INC_

#include <memory>
#include <vector>

#include "encoding_interface.h"
#include "resource_interface.h"
#include "source_interface.h"

namespace fsm
{

/**
 * \brief Foundation Services Config subject interface.
*/
class ConfigInterface
{
public:

    /**
     * \brief BindEncodingsChanged event parameters.
     */
    struct EncodingsChangedEvent
    {
        std::vector<std::shared_ptr<EncodingInterface>> encodings;  ///< list of current encodings.
    };

    /**
     * \brief BindSourcesChanged event parameters.
     */
    struct SourcesChangedEvent
    {
        std::vector<std::shared_ptr<SourceInterface>> sources;      ///< list of current sources.
    };

    /**
     * \brief BindResourcesChanged event parameters.
     */
    struct ResourcesChangedEvent
    {
        std::vector<std::shared_ptr<ResourceInterface>> resources;  ///< list of current resources.
    };

    /**
     * \brief Encodings Changed Callback.
     */
    typedef Callback<EncodingsChangedEvent> EncodingsChangedCb;

    /**
     * \brief Sources Changed Callback.
     */
    typedef Callback<SourcesChangedEvent> SourcesChangedCb;

    /**
     * \brief Resources Changed Callback.
     */
    typedef Callback<ResourcesChangedEvent> ResourcesChangedCb;

    /**
     * \brief virtual Destructor.
     */
    virtual ~ConfigInterface() { }

    /**
     * \brief Registers a callback function which will get called when the encodings set changes.
     *
     * \param[in] func function object acting as callback.
     * \param[out] id unique binding identifier.
     *
     * \return fscfg_kRcSuccess on success.
     */
    virtual fscfg_ReturnCode BindEncodingsChanged(EncodingsChangedCb func, std::uint32_t& id) = 0;

    /**
     * \brief Registers a callback function which will get called when the sources set changes.
     *
     * \param[in] func function object acting as callback.
     * \param[out] id unique binding identifier.
     *
     * \return fscfg_kRcSuccess on success.
     */
    virtual fscfg_ReturnCode BindSourcesChanged(SourcesChangedCb func, std::uint32_t& id) = 0;

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
     * \brief GetAll retrieves all registered encoding instances.
     *
     * \param[out] encodings set of all registered encodings.
     *
     * \return fscfg_kRcSuccess on success.
     */
    virtual fscfg_ReturnCode GetAll(std::vector<std::shared_ptr<EncodingInterface>>& encodings) = 0;

    /**
     * \brief GetAll retrieves all registered source instances.
     *
     * \param[out] sources set of all registered sources.
     *
     * \return fscfg_kRcSuccess on success.
     */
    virtual fscfg_ReturnCode GetAll(std::vector<std::shared_ptr<SourceInterface>>& sources) = 0;

    /**
     * \brief GetAll retrieves all registered resource instances.
     *
     * \param[out] resources set of all registered resources.
     *
     * \return fscfg_kRcSuccess on success.
     */
    virtual fscfg_ReturnCode GetAll(std::vector<std::shared_ptr<ResourceInterface>>& resources) = 0;

    /**
     * \brief Retrieve an encoding by name.
     *
     * \param[in] name name of the encoding.
     * \param[out] encoding encoding instance.
     *
     * \return fscfg_kRcSuccess on success,\n
     *         fscfg_kRcBadResource if encoding was not found.
     */
    virtual fscfg_ReturnCode Get(const std::string& name, std::shared_ptr<EncodingInterface>& encoding) = 0;

    /**
     * \brief Retrieve a source by name.
     *
     * \param[in] name name of the source.
     * \param[out] source source instance.
     *
     * \return fscfg_kRcSuccess on success,\n
     *         fscfg_kRcBadResource if source was not found.
     */
    virtual fscfg_ReturnCode Get(const std::string& name, std::shared_ptr<SourceInterface>& source) = 0;

    /**
     * \brief Retrieve a resource by name.
     *
     * \param[in] name name of the resource.
     * \param[out] resource resource instance.
     *
     * \return fscfg_kRcSuccess on success,\n
     *         fscfg_kRcBadResource if encoding was not found.
     */
    virtual fscfg_ReturnCode Get(const std::string& name, std::shared_ptr<ResourceInterface>& resource) = 0;

};

} // namespace fsm

#endif // FSM_CONFIG_INTERFACE_H_INC_

/** \}    end of addtogroup */
