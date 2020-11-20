/**
 * Copyright (C) 2017, 2018 Continental Automotive GmbH
 *
 * Licensed under the "Supplier unrestricted white box" category, as
 * described in the 20160322-1_SW-amendment TCAM.pdf agreement
 * between Volvo Cars and Continental Automotive GmbH.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *  \file     resource.h
 *  \brief    Foundation Services Resource interface
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#ifndef FSM_RESOURCE_H_INC_
#define FSM_RESOURCE_H_INC_

#include <string>
#include <memory>

#include <fscfg_common/resource_bind.h>
#include <fscfg_common/generated/foundation-services-config-generated.h>

#include "source.h"
#include "encoding.h"

namespace fsm
{

class Config;

/**
 * \brief Foundation Services Resource subject interface
 */
class Resource : public virtual ResourceBind
{
public:

    /**
     * \brief Resource Constructor.
     *
     * \param[in] name of the resource.
     * \param[in] config config container pointer.
     *
     */
    Resource(const std::string& name, std::shared_ptr<Config> config);

    /**
     * \brief Destructor.
     */
    ~Resource();

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

    /**
     * \brief Retrieves the D-Bus resource object.
     *
     * \param[out] resource reference to the D-Bus resource object.
     *
     * \return fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode GetObject(_ConfigResource*& resource);

    /**
     * \brief Tracks a payload owned by a given source.
     *
     * \param[in] encoding encoding of the payload added by source.
     * \param[in] source owning source.
     *
     * \return fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode AddPayloadEntry(std::shared_ptr<Encoding> encoding, std::shared_ptr<Source> source);

    /**
     * \brief Untracks a currently tracked payload.
     *
     * \param[in] encoding encoding of the payload added by source.
     * \param[in] source owning source.
     *
     * \return fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode RemovePayloadEntry(std::shared_ptr<Encoding> encoding, std::shared_ptr<Source> source);

    /**
     * \brief Marks a tracked payload as being changed.
     *
     * \param[in] encoding encoding of the payload added by source.
     * \param[in] source owning source.
     *
     * \return fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode SetPayloadEntryChanged(std::shared_ptr<Encoding> encoding, std::shared_ptr<Source> source);

    /**
     * \brief Publish all ongoing modifications since last update.
     *
     * \return fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode Update(std::shared_ptr<Source> source);

    /**
     * \brief Dispatches the expiry to all listeners.
     *
     * \param immediate eviction type, true if payload should be re-fetched, false if only cache invalidation.
     *
     * \return fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode ProcessExpiry(bool immediate);

    /**
     * \brief Connect DBus signals to static methods.
     *
     * \return fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode ConnectSignals();

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

    /**
      * \brief Handle GetPayload D-Bus method call.
      *
      * \param[in] resource_dbus_obj GDBus Resource Object.
      * \param[in] invoc GDBus method invocation.
      * \param[in] encoding_path payload encoding.
      * \param[in] source_path source to retrieve the payload from.
      * \param[in] Foundation Services resource.
      *
      * \return fscfg_kRcSuccess on success.
      */
     static gboolean OnGetPayloadDbus(_ConfigResource* resource_dbus_obj,
                                      GDBusMethodInvocation* invoc,
                                      const char* encoding_path,
                                      const char* source_path,
                                      Resource* resource);

    std::string name_;                      ///< resource name.
    std::shared_ptr<Config> config_;        ///< config container.
    _ConfigResource* resource_dbus_obj_;    ///< D-Bus resource object.

private:
    typedef std::map<std::string, std::map<std::string, bool>> PayloadMap;

    BufferedDataSet<PayloadMap> payloads_;                          ///< Payload status. (true is changed from last update).
    std::map<std::string, std::shared_ptr<Source>> sources_;        ///< All sources that hold resource.
    std::map<std::string, std::shared_ptr<Encoding>> encodings_;    ///< All payload encodings.
};

} // namespace fsm

#endif // FSM_RESOURCE_H_INC_

/** \}    end of addtogroup */
