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
 *  \file     source.h
 *  \brief    Foundation Services Source interface
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#ifndef FSM_SOURCE_H_INC_
#define FSM_SOURCE_H_INC_

#include <fscfg/source_interface.h>

#include <fscfg_common/generated/foundation-services-config-generated.h>

#include <fscfg_common/source_bind.h>
#include <fscfg_common/binding_set.h>
#include <fscfg_common/buffered_data_set.h>

#include <fscfgd/encoding.h>

namespace fsm
{

class EncodingInterface;
class Resource;

/**
 * \brief Foundation Services Source interface
*/
class Source : public SourceBind
{
public:
    typedef std::vector<std::uint8_t> Payload;

    /**
     * \brief Source Constructor.
     *
     * \param[in] name source name.
     */
    Source(const std::string& name);

    /**
     * Source Destructor.
     */
    ~Source();

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
                                std::shared_ptr<EncodingInterface> encoding, Payload& payload);

    /**
     * \copydoc fsm::SourceInterface::HasResource
     */
    fscfg_ReturnCode HasResource(std::shared_ptr<ResourceInterface> resource, bool& has_resource);

    /**
     * \brief Retrieves the D-Bus source object.
     *
     * \param[out] source reference to the D-Bus source object.
     *
     * \return fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode GetObject(_ConfigSource*& source);


    /**
     * \brief Update all parent payloads and notify all listeners.
     *
     * \param[in] resource_name name of the child resource from which to trace the parent chain.
     *
     * \return fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode UpdateParentPayloads(const std::string& resource_name);


    /**
     * \brief Publish all ongoing modifications since last update.
     *
     * \return fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode Update();


    /**
     * \brief Retrieves the payload without dispatching update nofifications for an owned resource and given encoding.
     *
     * \param[in] resource a source-owned resource.
     * \param[in] encoding payload encoding.
     * \param[out] payload actual payload.
     *
     * \return fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode GetStoredPayload(std::shared_ptr<ResourceInterface> resource,
                                      std::shared_ptr<EncodingInterface> encoding,
                                      Payload& payload);


    /**
     * \brief Sets or adds the source as the owner of the given payload and tracks it internally.
     *
     * \param[in] resource resource the given payload belongs to.
     * \param[in] encoding payload encoding.
     * \param[in] payload payload blob.
     *
     * \return fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode SetPayloadEntry(std::shared_ptr<Resource> resource,
                                     std::shared_ptr<Encoding> encoding,
                                     const Payload& payload);

    /**
     * \brief Removes the ownership of the internally tracked payload for the given resource-encoding pair.
     *
     * \param[in] resource resource the given payload belongs to.
     * \param[in] encoding payload encoding.
     *
     * \return fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode RemovePayloadEntry(std::shared_ptr<Resource> resource, std::shared_ptr<Encoding> encoding);

    /**
     * \brief Enables or disables parent update when fetching a given resource. The updates involve retrieving the payload
     * for parent resources, which in turn activates any underlying validating layers.
     *
     * \param[in] update_parent_payloads true if parent update mechanism shall be active during payload fetch, false otherwise.
     *
     * \return fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode SetUpdateParentPayloads(bool update_parent_payloads);


protected:
    ///! Mapping between resource name and mapping between encoding name and payload.
    typedef std::map<std::string, std::map<std::string, Payload>> PayloadMap;

    /**
     * \copydoc fsm::SourceBind::OnUpdated
     */
    fscfg_ReturnCode OnUpdated(std::vector<std::shared_ptr<ResourceInterface>> resources);

    /**
     * \brief Retrieves all resources source has a payload for.
     *
     * \param[out] resources resources source has a payload for.
     *
     * \return fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode GetResources(std::vector<std::shared_ptr<ResourceInterface>>& resources);

    /**
     * \brief Retrieves all known source-owned payload encodings.
     *
     * \param[out] encodings all known source-owned payload encodings.
     *
     * \return fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode GetEncodings(std::vector<std::shared_ptr<EncodingInterface>>& encodings);

    /**
     * \brief Connect DBus signals to static methods.
     *
     * \return fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode ConnectSignals();

    /**
     * \brief Deduces if a resource described by it's relative name has a payload onwed by source.
     *
     * \param[in] resource_name resource name.
     *
     * \return fscfg_kRcSuccess on success.
     */
    bool HasResource(const std::string& resource_name);

    /**
     * \brief Handle Update D-Bus method call.
     *
     * \param[in] source_dbus_obj GDBus Source Object.
     * \param[in] invoc GDBus method invocation.
     * \param[in] source Foundation Services source.
     *
     * \return fscfg_kRcSuccess on success.
     */
    static gboolean OnUpdateDbus(_ConfigSource* source_dbus_obj, GDBusMethodInvocation* invoc, Source* source);

    /**
      * \brief Handle GetEncodings D-Bus method call.
      *
     * \param[in] source_dbus_obj GDBus Source Object.
     * \param[in] invoc GDBus method invocation.
     * \param[in] resource_path resource object path.
     * \param[in] source Foundation Services source.
     *
     * \return fscfg_kRcSuccess on success.
     */
    static gboolean OnGetEncodingsDbus(_ConfigSource* source_dbus_obj,
                                       GDBusMethodInvocation* invoc,
                                       const char* resource_path,
                                       Source* source);

    /**
      * \brief Handle GetPayload D-Bus method call.
      *
     * \param[in] source_dbus_obj GDBus Source Object.
     * \param[in] invoc GDBus method invocation.
     * \param[in] resource_path resource object path.
     * \param[in] encoding_path payload encoding object path.
     * \param[in] source Foundation Services source.
     *
     * \return fscfg_kRcSuccess on success.
     */
    static gboolean OnGetPayloadDbus(_ConfigSource* source_dbus_obj,
                                     GDBusMethodInvocation* invoc,
                                     const char* resource_path,
                                     const char* encoding_path,
                                     Source* source);

    /**
      * \brief Handle HasResource D-Bus method call.
      *
     * \param[in] source_dbus_obj GDBus Source Object.
     * \param[in] invoc GDBus method invocation.
     * \param[in] source Foundation Services source.
     *
     * \return fscfg_kRcSuccess on success.
     */
    static gboolean OnHasResourceDbus(_ConfigSource* source_dbus_obj,
                                      GDBusMethodInvocation* invoc,
                                      const char* resource_path,
                                      Source* source);

    std::string name_;                                                          ///< source name.
    _ConfigSource* source_dbus_obj_;                                            ///< D-Bus source object.
    BufferedDataSet<PayloadMap> payloads_;                                      ///< All payloads described by source.
    std::map<std::string, std::shared_ptr<Resource>> resources_;                ///< All associated resources.
    std::map<std::string, std::shared_ptr<Encoding>> encodings_;                ///< All encodings in source.
    bool update_parent_payloads_;                                               ///< true if resource's parents shall be updated while retrieving payload, false otherwise.
};

} // namespace fsm

#endif // FSM_SOURCE_H_INC_

/** \}    end of addtogroup */
