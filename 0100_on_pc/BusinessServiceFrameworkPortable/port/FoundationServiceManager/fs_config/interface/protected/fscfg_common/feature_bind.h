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
 *  \file     feature_bind.h
 *  \brief    Foundation Services Config Feature bindings handler interface.
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#ifndef FSM_FEATURE_BIND_H_INC_
#define FSM_FEATURE_BIND_H_INC_

#include <fscfg/feature_interface.h>

#include <fscfg_common/provisionedresource_bind.h>
#include <fscfg_common/utilities.h>

#include "multi_binding_set.h"

namespace fsm
{

/**
 * \brief Foundation Services Bind handler for the Feature resource.
 * Provides the house-keeping mechanism (manipulates all function callbacks) for binding sets related to the
 * Feature resource for both proxy daemon-side.
*/
class FeatureBind : public virtual FeatureInterface,
                    public virtual ProvisionedResourceBind
{
public:
    /**
     * \brief Constructor.
     */
    FeatureBind();

    /**
     * \copydoc fsm::FeatureInterface::BindStateChanged
     */
    fscfg_ReturnCode BindStateChanged(StateChangedCb func, std::uint32_t& id);

    /**
     * \copydoc fsm::FeatureInterface::BindDescriptionChanged
     */
    fscfg_ReturnCode BindDescriptionChanged(DescriptionChangedCb func, std::uint32_t& id);

    /**
     * \copydoc fsm::FeatureInterface::BindUriChanged
     */
    fscfg_ReturnCode BindUriChanged(UriChangedCb func, std::uint32_t& id);

    /**
     * \copydoc fsm::FeatureInterface::BindIconChanged
     */
    fscfg_ReturnCode BindIconChanged(IconChangedCb func, std::uint32_t& id);

    /**
     * \copydoc fsm::FeatureInterface::BindTagsChanged
     */
    fscfg_ReturnCode BindTagsChanged(TagsChangedCb func, std::uint32_t& id);

    /**
     * \copydoc fsm::FeatureInterface::Unbind
     */
    fscfg_ReturnCode Unbind(std::uint32_t id);

protected:
    /**
     * \brief Feature base binds
     */
    enum BindBase
    {
        kStateChanged,          ///< StateChanged binding set tuple member position.
        kDescriptionChanged,    ///< DescriptionChanged binding set tuple member position.
        kUriChanged,            ///< UriChanged binding set tuple member position.
        kIconChanged,           ///< IconChanged binding set tuple member position.
        kTagsChanged,           ///< TagsChanged binding set tuple member position.

        kBindBaseMax            ///< Maximum value in this enum. Keep this last.
    };

    /**
     * \brief Feature state changed D-Bus callback.
     *
     * \param[in] state feature state when change occured.
     *
     * \return fscfg_kRcSuccess on success.
     */
    virtual fscfg_ReturnCode OnStateChanged(State state);

    /**
     * \brief Feature description changed D-Bus callback.
     *
     * \param[in] description feature description when change occured.
     *
     * \return fscfg_kRcSuccess on success.
     */
    virtual fscfg_ReturnCode OnDescriptionChanged(std::string description);

    /**
     * \brief Feature uri changed D-Bus callback.
     *
     * \param[in] uri feature uri when change occured.
     *
     * \return fscfg_kRcSuccess on success.
     */
    virtual fscfg_ReturnCode OnUriChanged(std::string uri);

    /**
     * \brief Feature icon changed D-Bus callback.
     *
     * \param[in] icon feature icon when change occured.
     *
     * \return fscfg_kRcSuccess on success.
     */
    virtual fscfg_ReturnCode OnIconChanged(std::string icon);

    /**
     * \brief Feature tags changed D-Bus callback.
     *
     * \param[in] tags feature tags when change occured.
     *
     * \return fscfg_kRcSuccess on success.
     */
    virtual fscfg_ReturnCode OnTagsChanged(std::vector<std::string> tags);

    MultiBindingSet<StateChangedEvent,
                    DescriptionChangedEvent,
                    UriChangedEvent,
                    IconChangedEvent,
                    TagsChangedEvent> bind_base_;   ///< base bindings.
};

} // namespace fsm

#endif // FSM_FEATURE_BIND_H_INC_

/** \}    end of addtogroup */
