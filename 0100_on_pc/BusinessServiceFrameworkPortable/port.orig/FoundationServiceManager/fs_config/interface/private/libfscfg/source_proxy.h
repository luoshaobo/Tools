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
 *  \file     source_proxy.h
 *  \brief    Foundation Services Source proxy
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#ifndef FSM_SOURCE_PROXY_H_INC_
#define FSM_SOURCE_PROXY_H_INC_

#include <string>

#include <fscfg/source_interface.h>

#include<fscfg_common/source_bind.h>

#include <fscfg_common/generated/foundation-services-config-generated.h>

namespace fsm
{

class ConfigProxy;

/**
 * \brief Foundation Services Source proxy
 */
class SourceProxy : public SourceBind
{
public:
    /**
     * \brief SourceProxy Constructor.
     *
     * \param[in] config_proxy   config proxy object
     * \param[in] source_dbus_obj   Dbus source object.
     */
    SourceProxy(std::shared_ptr<fsm::ConfigProxy> config_proxy, ConfigSource* source_dbus_obj);

    /**
     * \brief SourceProxy Destructor.
     */
    ~SourceProxy();

    /**
     * \copydoc fsm::SourceInterface::GetName
     */
    fscfg_ReturnCode GetName(std::string& name);

    /**
     * \copydoc fsm::SourceInterface::GetEncodings
     */
    fscfg_ReturnCode GetEncodings(std::shared_ptr<ResourceInterface> resource,
                                  std::vector<std::shared_ptr<EncodingInterface>>& encodings);

    /**
     * \copydoc fsm::SourceInterface::GetPayload
     */
    fscfg_ReturnCode GetPayload(std::shared_ptr<ResourceInterface> resource,
                                std::shared_ptr<EncodingInterface> encoding,
                                std::vector<std::uint8_t>& payload);

    /**
     * \copydoc fsm::SourceInterface::HasResource
     */
    fscfg_ReturnCode HasResource(std::shared_ptr<ResourceInterface> resource, bool& has_resource);

    /**
     * \brief Register callback function to Dbus signal.
     *
     * \return fscfg_kRcSuccess on success,
     *         fscfg_kRcBadState if Dbus object is null.
     *         fscfg_kRcError if failed to register callback.
     */
    fscfg_ReturnCode ConnectSignals();

protected:
    /**
     * \copydoc fsm::SourceInterface::OnUpdated
     */
    fscfg_ReturnCode OnUpdated(std::vector<std::shared_ptr<ResourceInterface>> resources);

private:
    SourceProxy(const SourceProxy& other);
    SourceProxy& operator=(const SourceProxy& other);
    /**
     * \brief A callback function to handle "Updated" signal.
     *
     * \param[in] proxy Dbus proxy object.
     * \param[in] arg_resources List of resources that have been changed.
     * \param[in] source_proxy source proxy object.
     *
     * \return TRUE on success,
     *         FALSE on error
     */
    static gboolean OnUpdatedDbus(_ConfigSource* object,
                                  const gchar *const *arg_resources,
                                  SourceProxy* source_proxy);

    std::shared_ptr<ConfigProxy> config_proxy_;       ///< config proxy object.
    _ConfigSource* source_dbus_obj_;                ///< Proxy-side DBus resource object.
};

} // namespace fsm

#endif // FSM_SOURCE_PROXY_H_INC_

/** \}    end of addtogroup */
