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
 *  \file     resource_proxy.h
 *  \brief    Foundation Services Resource proxy interface
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#ifndef FSM_RESOURCE_PROXY_H_INC_
#define FSM_RESOURCE_PROXY_H_INC_

#include <string>

#include <fscfg_common/resource_bind.h>
#include <fscfg_common/generated/foundation-services-config-generated.h>

namespace fsm
{

class ConfigProxy;

/**
 * \brief Foundation Services Resource proxy interface
 */
class ResourceProxy : public virtual ResourceBind
{
public:
    /**
     * \brief ResourceProxy Constructor.
     *
     * \param[in] config_proxy config proxy object
     * \param[in] resource_dbus_obj Dbus resource object.
     */
    ResourceProxy(std::shared_ptr<fsm::ConfigProxy> config_proxy, _ConfigResource* resource_dbus_obj);

    /**
     * \brief ResourceProxy Destructor.
     */
    ~ResourceProxy();

    /**
     * \copydoc fsm::ResourceInterface::GetName
     */
    fscfg_ReturnCode GetName(std::string& name);

    /**
     * \copydoc fsm::ResourceInterface::GetSources
     */
    fscfg_ReturnCode GetSources(std::vector<std::shared_ptr<SourceInterface>>& sources);

    /**
     * \copydoc fsm::ResourceInterface::GetEncodings
     */
    fscfg_ReturnCode GetEncodings(std::vector<std::shared_ptr<EncodingInterface>>& encodings);

    /**
     * \copydoc fsm::ResourceInterface::GetPayload
     */
    fscfg_ReturnCode GetPayload(std::shared_ptr<EncodingInterface> encoding,
                                std::shared_ptr<SourceInterface> source,
                                std::vector<std::uint8_t>& payload);
protected:
    /**
     * \copydoc fsm::ResourceBind::OnExpired
     */
    fscfg_ReturnCode OnExpired(bool immediate);

    /**
     * \copydoc fsm::ResourceBind::OnUpdated
     */
    fscfg_ReturnCode OnUpdated(std::shared_ptr<SourceInterface> source);

    /**
     * \copydoc fsm::ResourceBind::OnSourcesChanged
     */
    fscfg_ReturnCode OnSourcesChanged(std::vector<std::shared_ptr<SourceInterface>> sources);

    /**
     * \copydoc fsm::ResourceBind::OnPayloadsChanged
     */
    fscfg_ReturnCode OnPayloadsChanged(std::vector<std::uint8_t> payload,
                                       std::shared_ptr<SourceInterface> source,
                                       std::shared_ptr<EncodingInterface> encoding);

    std::shared_ptr<fsm::ConfigProxy> config_proxy_;  ///< ConfigProxy object.

private:
    /**
     * \brief Register callback functions to Dbus signals.
     *
     * \return fscfg_kRcSuccess on success,
     *         fscfg_kRcBadState if Dbus object is null.
     *         fscfg_kRcError if failed to register callback.
     */
    fscfg_ReturnCode ConnectSignals();

    /**
     * \brief A callback function to handle "Updated" signal.
     *
     * \param[in] object Config resource dbus object.
     * \param[in] arg_source Source that triggered the change.
     * \param[in] resource_proxy Resource proxy object.
     *
     * \return TRUE on success,
     *         FALSE on Error
     */
    static gboolean OnUpdatedDbus(_ConfigResource* object,
                                  const gchar* arg_source,
                                  ResourceProxy* resource_proxy);

    /**
     * \brief A callback function to handle "Expired" signal.
     *
     * \param[in] object Config resource dbus object.
     * \param[in] arg_immediate Resource is to be immediately re-fetched or not.
     * \param[in] resource_proxy Resource proxy object.
     *
     * \return TRUE on success,
     *         FALSE on Error
     */
    static gboolean OnExpiredDbus(_ConfigResource* object,
                                  gboolean arg_immediate,
                                  ResourceProxy* resource_proxy);

    /**
     * \brief A callback function to handle "PayloadsChanged" signal.
     *
     * \param[in] object Config resource dbus object.
     * \param[in] encoding_path Encoding path in which the changed payload is described in.
     * \param[in] source_path Source path that triggered the payload change.
     * \param[in] g_payload Resource payload that has been changed.
     * \param[in] resource_proxy Resource proxy object.
     *
     * \return TRUE on success,
     *         FALSE on Error
     */
    static gboolean OnPayloadsChangedDbus(_ConfigResource* object,
                                          const gchar *encoding_path,
                                          const gchar *source_path,
                                          GVariant *g_payload,
                                          ResourceProxy* resource_proxy);
    /**
     * \brief Callback function when properties changed on Dbus.
     *
     * \param[in] proxy Dbus proxy object
     * \param[in] changed Properites that are changed
     * \param[in] invalidated Array of properties that were invalidated
     * \param[in] config_instance Object in which changed occured.
     */
    static void OnPropertiesChanged(GDBusProxy* proxy,
                                    GVariant* changed,
                                    GStrv invalidated,
                                    gpointer resource_instance);

    _ConfigResource* resource_dbus_obj_;    ///< Proxy-side DBus resource object.
};

} // namespace fsm

#endif // FSM_RESOURCE_PROXY_H_INC_

/** \}    end of addtogroup */
