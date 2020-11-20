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
 *  \file     feature_interface.h
 *  \brief    Foundation Services Feature subject interface
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#ifndef FSM_FEATURE_INTERFACE_H_INC_
#define FSM_FEATURE_INTERFACE_H_INC_

#include <cstdint>

#include <vector>
#include <string>
#include <memory>
#include <functional>

#include "fscfg_types.h"

namespace fsm
{

/**
 * \brief Foundation Services Feature subject interface
*/
class FeatureInterface
{
public:

    /**
     * \brief Permutation of 'enabled' and 'visible' attributes from the "Feature list"
     */
    enum class State
    {
        kUndefined,             ///< State is undefined.

        kEnabledVisible,        ///< both enabled and visible are set to true.
        kEnabledInvisible,      ///< enabled true, visible false.
        kDisabledVisible,       ///< enabled false, visible true.
        kDisabledInvisible,     ///< enabled false, visible false.

        kMax                    ///< Maximum value type should hold. Keep this last.
    };

    /**
     * \brief BindStateChanged event parameters.
     */
    struct StateChangedEvent
    {
        std::shared_ptr<FeatureInterface> feature;  ///< feature instance that triggered the state change.
        State state;                                ///< feature current state.
    };

    /**
     * \brief BindDescriptionChanged event parameters.
     */
    struct DescriptionChangedEvent
    {
        std::shared_ptr<FeatureInterface> feature;  ///< feature instance that triggered the description change.
        std::string description;                    ///< feature current description.
    };

    /**
     * \brief BindUriChanged event parameters.
     */
    struct UriChangedEvent
    {
        std::shared_ptr<FeatureInterface> feature;  ///< feature instance that triggered the uri change.
        std::string uri;                            ///< feature current uri.
    };

    /**
     * \brief BindIconChanged event parameters.
     */
    struct IconChangedEvent
    {
        std::shared_ptr<FeatureInterface> feature;  ///< feature instance that triggered the icon change.
        std::string icon;                           ///< feature current icon.
    };

    /**
     * \brief BindTagsChanged event parameters.
     */
    struct TagsChangedEvent
    {
        std::shared_ptr<FeatureInterface> feature;  ///< feature instance that triggered the tags change.
        std::vector<std::string> tags;              ///< feature current tags.
    };

    /**
     * \brief State Changed Callback
     */
    typedef Callback<StateChangedEvent> StateChangedCb;

    /**
     * \brief Description Changed Callback
     */
    typedef Callback<DescriptionChangedEvent> DescriptionChangedCb;

    /**
     * \brief Uri Changed Callback
     */
    typedef Callback<UriChangedEvent> UriChangedCb;

    /**
     * \brief Icon Changed Callback
     */
    typedef Callback<IconChangedEvent> IconChangedCb;

    /**
     * \brief Tags Changed Callback
     */
    typedef Callback<TagsChangedEvent> TagsChangedCb;

    /**
     * \brief virtual Destructor.
     */
    virtual ~FeatureInterface() { }

    /**
     * \brief Registers a callback function which will get called when the feature state changes.
     *
     * \param[in] func function object acting as callback.
     * \param[out] id unique binding identifier.
     *
     * \return fscfg_kRcSuccess on success.
     */
    virtual fscfg_ReturnCode BindStateChanged(StateChangedCb func, std::uint32_t& id) = 0;

    /**
     * \brief Registers a callback function which will get called when the feature description changes.
     *
     * \param[in] func function object acting as callback.
     * \param[out] id unique binding identifier.
     *
     * \return fscfg_kRcSuccess on success.
     */
    virtual fscfg_ReturnCode BindDescriptionChanged(DescriptionChangedCb func, std::uint32_t& id) = 0;

    /**
     * \brief Registers a callback function which will get called when the feature uri changes.
     *
     * \param[in] func function object acting as callback.
     * \param[out] id unique binding identifier.
     *
     * \return fscfg_kRcSuccess on success.
     */
    virtual fscfg_ReturnCode BindUriChanged(UriChangedCb func, std::uint32_t& id) = 0;

    /**
     * \brief Registers a callback function which will get called when the feature icon changes.
     *
     * \param[in] func function object acting as callback.
     *
     * \return fscfg_kRcSuccess on success.
     */
    virtual fscfg_ReturnCode BindIconChanged(IconChangedCb func, std::uint32_t& id) = 0;

    /**
     * \brief Registers a callback function which will get called when the feature tags changes.
     *
     * \param[in] func function object acting as callback.
     *
     * \return fscfg_kRcSuccess on success.
     */
    virtual fscfg_ReturnCode BindTagsChanged(TagsChangedCb func, std::uint32_t& id) = 0;
    /**
     * \brief Unbind existing callback function.
     *
     * \param[in] id binding unique identifier.
     *
     * \return fscfg_kRcSuccess on success.
     */
    virtual fscfg_ReturnCode Unbind(std::uint32_t id) = 0;

    /**
     * \brief Retrieves the name of the feature instance.
     *
     * \param[out] name name of the feature instance.
     *
     * \return fscfg_kRcSuccess on success.
     */
    virtual fscfg_ReturnCode GetName(std::string& name) = 0;

    /**
     * \brief Retrieves the state of the feature instance.
     *
     * \param[out] state state of the feature instance.
     *
     * \return fscfg_kRcSuccess on success.
     */
    virtual fscfg_ReturnCode GetState(State& state) = 0;

    /**
     * \brief Retrieves the description of the feature instance.
     *
     * \param[out] description description of the feature instance.
     *
     * \return fscfg_kRcSuccess on success.
     */
    virtual fscfg_ReturnCode GetDescription(std::string& description) = 0;

    /**
     * \brief Retrieves the uri of the feature instance.
     *
     * \param[out] uri uri of the feature instance.
     *
     * \return fscfg_kRcSuccess on success.
     */
    virtual fscfg_ReturnCode GetUri(std::string& uri) = 0;

    /**
     * \brief Retrieves the icon of the feature instance.
     *
     * \param[out] icon icon of the feature instance.
     *
     * \return fscfg_kRcSuccess on success.
     */
    virtual fscfg_ReturnCode GetIcon(std::string& icon) = 0;

    /**
     * \brief Retrieves the tags of the feature instance.
     *
     * \param[out] tags tags of the feature instance.
     *
     * \return fscfg_kRcSuccess on success.
     */
    virtual fscfg_ReturnCode GetTags(std::vector<std::string>& tags) = 0;
};

} // namespace fsm

#endif // FSM_FEATURE_INTERFACE_H_INC_

/** \}    end of addtogroup */

