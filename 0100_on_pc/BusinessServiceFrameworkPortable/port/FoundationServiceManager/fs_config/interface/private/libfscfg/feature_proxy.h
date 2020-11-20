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
 *  \file     feature_proxy.h
 *  \brief    Foundation Services Feature proxy interface
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#ifndef FSM_FEATURE_PROXY_H_INC_
#define FSM_FEATURE_PROXY_H_INC_

#include <string>

#include <fscfg_common/feature_bind.h>

#include "provisionedresource_proxy.h"

namespace fsm
{

class DiscoveryProxy;

/**
 * \brief Foundation Services Feature proxy
 */
class FeatureProxy : public FeatureBind, public ProvisionedResourceProxy
{
public:
    /**
     * \brief FeatureProxy Constructor.
     *
     * \param[in] discovery_proxy discovery proxy object
     * \param[in] base base class ProvisionedResourceProxy object
     * \param[in] feature_dbus_obj Dbus feature object.
     */
    FeatureProxy(std::shared_ptr<fsm::DiscoveryProxy> discovery_proxy,
                 std::shared_ptr<ProvisionedResourceProxy> base,
                 _ConfigFeature* feature_dbus_obj);

    /**
     * DiscoveryProxy Destructor.
     */
    ~FeatureProxy();

    /**
     * \copydoc fsm::FeatureInterface::GetName
     */
    fscfg_ReturnCode GetName(std::string& name);

    /**
     * \copydoc fsm::FeatureInterface::GetState
     */
    fscfg_ReturnCode GetState(State& state);

    /**
     * \copydoc fsm::FeatureInterface::GetDescription
     */
    fscfg_ReturnCode GetDescription(std::string& description);

    /**
     * \copydoc fsm::FeatureInterface::GetUri
     */
    fscfg_ReturnCode GetUri(std::string& uri);

    /**
     * \copydoc fsm::FeatureInterface::GetIcon
     */
    fscfg_ReturnCode GetIcon(std::string& icon);

    /**
     * \copydoc fsm::FeatureInterface::GetTags
     */
    fscfg_ReturnCode GetTags(std::vector<std::string>& tags);

protected:
    /**
     * \copydoc fsm::FeatureBind::OnStateChanged
     */
    fscfg_ReturnCode OnStateChanged(State state);

    /**
     * \copydoc fsm::FeatureBind::OnDescriptionChanged
     */
    fscfg_ReturnCode OnDescriptionChanged(std::string description);

    /**
     * \copydoc fsm::FeatureBind::OnUriChanged
     */
    fscfg_ReturnCode OnUriChanged(std::string uri);

    /**
     * \copydoc fsm::FeatureBind::OnIconChanged
     */
    fscfg_ReturnCode OnIconChanged(std::string icon);

    /**
     * \copydoc fsm::FeatureBind::OnTagsChanged
     */
    fscfg_ReturnCode OnTagsChanged(std::vector<std::string> tags);

private:
    /**
     * \brief Register callback function to Dbus signal.
     *
     * \return fscfg_kRcSuccess on success,
     *         fscfg_kRcBadState if Dbus object is null.
     *         fscfg_kRcError if failed to register callback.
     */
    fscfg_ReturnCode ConnectSignals();

    /**
     * \brief Callback function when properties changed on Dbus.
     *
     * \param[in] proxy Dbus proxy object
     * \param[in] changed properites that are changed
     * \param[in] invalidated Array of properties that was invalidated
     * \param[in] config_instance Object in which changed occured.
     */
    static void OnPropertiesChanged(GDBusProxy* proxy,
                                    GVariant* changed,
                                    GStrv invalidated,
                                    gpointer config_instance);

    _ConfigFeature* feature_dbus_obj_;                ///< Proxy-side DBus feature object.
    std::shared_ptr<ProvisionedResourceProxy> base_;  ///< ProvisionedResourceProxy object.
};

} // namespace fsm

#endif // FSM_FEATURE_PROXY_H_INC_

/** \}    end of addtogroup */
