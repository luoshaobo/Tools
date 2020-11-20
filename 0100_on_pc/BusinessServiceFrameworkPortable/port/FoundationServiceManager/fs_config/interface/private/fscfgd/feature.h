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
 *  \file     feature.h
 *  \brief    Foundation Services Feature interface
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#ifndef FSM_FEATURE_H_INC_
#define FSM_FEATURE_H_INC_

#include <fscfg_common/feature_bind.h>
#include <fscfg_common/generated/foundation-services-config-generated.h>

#include "provisionedresource.h"

namespace fsm
{

class Discovery;

/**
 * \brief Foundation Services Feature
*/
class Feature : public FeatureBind, public ProvisionedResource
{
public:

    /**
     * \brief Feature Constructor.
     *
     * \param[in] name of the feature.
     * \param[in] discovery reference to the discovery container.
     */
    Feature(const std::string& name, std::shared_ptr<Discovery> discovery);

    /**
     * \brief Destructor.
     */
    ~Feature();

    /**
     * \copydoc fsm::ProvisionedResourceInterface::GetName
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

    /**
     * \brief Retrieves the D-Bus resource object.
     *
     * \param[out] resource reference to the D-Bus resource object.
     *
     * \return fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode GetObject(_ConfigFeature*& resource);

    /**
     * \brief Feature state setter.
     *
     * \param[in] state feature state.
     *
     * \return fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode SetState(State state);

    /**
     * \brief Feature description setter.
     *
     * \param[in] description feature description.
     *
     * \return fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode SetDescription(const std::string& description);

    /**
     * \brief Feature uri setter.
     *
     * \param[in] uri feature uri.
     *
     * \return fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode SetUri(const std::string& uri);

    /**
     * \brief Feature icon setter.
     *
     * \param[in] icon feature icon.
     *
     * \return fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode SetIcon(const std::string& icon);

    /**
     * \brief Feature tags setter.
     *
     * \param[in] tags feature tags.
     *
     * \return fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode SetTags(const std::vector<std::string>& tags);

    /**
     * \brief Publish all ongoing modifications since last update.
     *
     * \return fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode Update();

protected:
    /**
     * \copydoc fsm::FeatureInterface::OnStateChanged
     */
    fscfg_ReturnCode OnStateChanged(State state);

    /**
     * \copydoc fsm::FeatureInterface::OnDescriptionChanged
     */
    fscfg_ReturnCode OnDescriptionChanged(std::string description);

    /**
     * \copydoc fsm::FeatureInterface::OnUriChanged
     */
    fscfg_ReturnCode OnUriChanged(std::string uri);

    /**
     * \copydoc fsm::FeatureInterface::OnIconChanged
     */
    fscfg_ReturnCode OnIconChanged(std::string icon);

    /**
     * \copydoc fsm::FeatureInterface::OnTagsChanged
     */
    fscfg_ReturnCode OnTagsChanged(std::vector<std::string> tags);

private:
    std::shared_ptr<Discovery> discovery_;              ///< discovery container reference.
    BufferedDataSet<State> state_;                      ///< feature state.
    BufferedDataSet<std::string> description_;          ///< feature description.
    BufferedDataSet<std::string> uri_;                  ///< feature uri.
    BufferedDataSet<std::string> icon_;                 ///< feature icon.
    BufferedDataSet<std::vector<std::string>> tags_;    ///< feature tags.

    _ConfigFeature* feature_dbus_obj_;                  ///< DBus feature object.
};

} // namespace fsm

#endif // FSM_FEATURE_H_INC_

/** \}    end of addtogroup */
