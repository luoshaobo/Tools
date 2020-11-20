/**
 * Copyright (C) 2018 Continental Automotive GmbH
 *
 * Licensed under the "Supplier unrestricted white box" category, as
 * described in the 20160322-1_SW-amendment TCAM.pdf agreement
 * between Volvo Cars and Continental Automotive GmbH.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *  \file     resource_binding_handler.h
 *  \brief    Foundation Services Config resource binding handler abstract class interface.
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#ifndef FSM_RESOURCE_BINDING_HANDLER_H_INC_
#define FSM_RESOURCE_BINDING_HANDLER_H_INC_

#include <fscfgd/config.h>

namespace fsm
{

/**
 * \brief Foundation Services Config Deamon-side common resource binding mechanisms.
 */
class ResourceBindingHandler
{
public:
    /**
     * \brief ResourceBindingHandler constructor.
     *
     * \param[in] config config container to use for resource name lookup.
     */
    ResourceBindingHandler(std::shared_ptr<Config> config);

    /**
     * \brief ResourceBindingHandler destructor.
     *
     * Releases all known bindings.
     */
    virtual ~ResourceBindingHandler();

protected:
    /**
     * \brief Resource payload updated callback.
     *
     * \param[in] updated_event resource payload updated event parameter.
     *
     * \return fscfg_kRcSuccess on success.
     */
    virtual fscfg_ReturnCode OnBoundResourceUpdated(ResourceInterface::UpdatedEvent updated_event);

    /**
     * \brief Resource payload changed callback.
     *
     * \param[in] updated_event resource payload updated event parameter.
     *
     * \return fscfg_kRcSuccess on success.
     */
    virtual fscfg_ReturnCode OnBoundResourcePayloadChanged(ResourceInterface::PayloadsChangedEvent payloads_changed_event);

    /**
     * \brief Bind to the update callback for the given resource name.
     * Performs and stores the bind if it does not exist already.
     *
     * \param[in] resource_name name of the resource to bind for updates.
     *
     * \return fscfg_kRcBadState if config pointer provided at constructor is null,\n
     *         fscfg_kRcBadResource if no resource by parameter-provided name exists,\n
     *         fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode BindUpdate(const std::string& resource_name);


    /**
     * \brief Bind to the payload change callback for the given resource name.
     * Performs and stores the bind if it does not exist already.
     *
     * \param[in] resource_name name of the resource to bind for updates.
     *
     * \return fscfg_kRcBadState if config pointer provided at constructor is null,\n
     *         fscfg_kRcBadResource if no resource by parameter-provided name exists,\n
     *         fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode BindPayloadChange(const std::string& resource_name);

    /**
     * \brief Removes all callbacks callback for the given resource name, if they exists.
     *
     * \param[in] resource_name name of the resource to bind for updates.
     *
     * \return fscfg_kRcBadState if config pointer provided at constructor is null,\n
     *         fscfg_kRcBadResource if no resource by parameter-provided name exists,\n
     *         fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode UnbindUpdate(const std::string& resource_name);

private:
    ///! Disabled copy-constructor.
    ResourceBindingHandler(const ResourceBindingHandler& other);

    ///! Disabled assignment operator.
    ResourceBindingHandler& operator=(const ResourceBindingHandler& other);

    std::map<std::string, std::uint32_t> update_bindings_; ///< Resource name to update bind id.
    std::map<std::string, std::uint32_t> update_payload_bindings_; ///< PayloadChange bind id.

    std::shared_ptr<Config> config_;                       ///< Config container.
};

} // namespace fsm

#endif

/** \}    end of addtogroup */
