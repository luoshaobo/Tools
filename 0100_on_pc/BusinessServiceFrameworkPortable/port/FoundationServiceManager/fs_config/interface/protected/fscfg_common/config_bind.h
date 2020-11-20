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
 *  \file     config_bind.h
 *  \brief    Foundation Services Config bindings handler interface.
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#ifndef FSM_CONFIG_BIND_H_INC_
#define FSM_CONFIG_BIND_H_INC_

#include <fscfg/config_interface.h>

#include "multi_binding_set.h"

namespace fsm
{

/**
 * \brief Foundation Services Bind handler for the Config container.
 * Provides the house-keeping mechanism (manipulates all function callbacks) for binding sets related to the
 * Config container for both proxy daemon-side.
*/
class ConfigBind : public ConfigInterface
{
public:
    /**
     * \brief Constructor.
     */
    ConfigBind();

    /**
     * \copydoc fsm::ConfigInterface::BindEncodingsChanged
     */
    fscfg_ReturnCode BindEncodingsChanged(EncodingsChangedCb func, std::uint32_t& id);

    /**
     * \copydoc fsm::ConfigInterface::BindSourcesChanged
     */
    fscfg_ReturnCode BindSourcesChanged(SourcesChangedCb func, std::uint32_t& id);

    /**
     * \copydoc fsm::ConfigInterface::BindResourcesChanged
     */
     fscfg_ReturnCode BindResourcesChanged(ResourcesChangedCb func, std::uint32_t& id);

     /**
      * \copydoc fsm::ConfigInterface::Unbind
      */
     fscfg_ReturnCode Unbind(std::uint32_t id);

protected:
    /**
     * \brief Encodings set changed D-Bus callback.
     *
     * \param[in] encodings set of encodings present on the bus after change occured.
     *
     * \return fscfg_kRcSuccess on success.
     */
    virtual fscfg_ReturnCode OnEncodingsChanged(std::vector<std::shared_ptr<EncodingInterface>> encodings);

    /**
     * \brief Sources set changed D-Bus callback.
     *
     * \param[in] sources set of sources present on the bus after change occured.
     *
     * \return fscfg_kRcSuccess on success.
     */
    virtual fscfg_ReturnCode OnSourcesChanged(std::vector<std::shared_ptr<SourceInterface>> sources);

    /**
     * \brief Resources set changed D-Bus callback.
     *
     * \param[in] resources set of resources present on the bus after change occured.
     *
     * \return fscfg_kRcSuccess on success.
     */
    virtual fscfg_ReturnCode OnResourcesChanged(std::vector<std::shared_ptr<ResourceInterface>> resources);

     /**
      * \brief Config base binds.
      */
    enum BindBase
    {
        kEncodingsChanged,          ///< EncodingsChanged binding set tuple member position.
        kSourcesChanged,            ///< SourcesChanged binding set tuple member position.
        kResourcesChanged,          ///< ResourcesChanged binding set tuple member position.

        kBindBaseMax                ///< Maximum value in this enum. Keep this last.
    };

    MultiBindingSet<EncodingsChangedEvent, SourcesChangedEvent, ResourcesChangedEvent> bind_base_; ///< base bindings.
};

} // namespace fsm

#endif // FSM_CONFIG_BIND_H_INC_

/** \}    end of addtogroup */
