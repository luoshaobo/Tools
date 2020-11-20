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
 *  \file     vsdpsource.h
 *  \brief    Foundation Services VsdpSource interface
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#ifndef FSM_VSDPSOURCE_H_INC_
#define FSM_VSDPSOURCE_H_INC_

#include <string>
#include <map>

#include <transfer_manager.h>

#include <fscfgd/provisioning.h>
#include <fscfgd/source.h>

#include <fscfgd/local_config.h>

namespace fsm
{

/**
 * \brief Foundation Services VsdpSource specialization interface.
 *
 * Publishes configuration resources available in the Volvo Cloud Server and provides access to resource content,
 * on demand.
*/
class VsdpSource : public Source
{
public:
    /**
     * \brief VsdpSource Constuctor.
     *
     * \param[in] provisioning provisioning container reference.
     * \param[in] local config reference.
     * \param[in] transfer_manager reference.
     */
    VsdpSource(std::shared_ptr<fsm::Provisioning> provisioning,
               std::shared_ptr<LocalConfig> local_config,
               std::shared_ptr<transfermanager::TransferManager> transfer_manager);

    /**
     * \brief VsdpSource Destructor.
     */
    ~VsdpSource();

    /**
     * \copydoc fsm::Source::GetPayload
     */
    fscfg_ReturnCode GetPayload(std::shared_ptr<ResourceInterface> resource,
                                std::shared_ptr<EncodingInterface> encoding,
                                std::vector<std::uint8_t>& payload);

    /**
     * \copydoc fsm::Source::HasResource
     */
    fscfg_ReturnCode HasResource(std::shared_ptr<ResourceInterface> resource, bool& has_resource);

    /**
     * \brief Maps a resource name to a given uri.
     * If a resource (by name) is requested to VsdpSource, VsdpSource will use the given uri to retrieve the resource.
     *
     * \param[in] uri URI to retrieve the resource from.
     * \param[in] resource_name resource name.
     *
     * \return fscfg_kRcBadState if instance is in a bad state and can't be operated on,\n
     *         fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode AddUriMapping(const std::string& uri, const std::string& resource_name);

    /**
     * \brief Removes the mapping between given resource and the previously added associated uri.
     *
     * \param[in] resource_name resource name.
     *
     * \return fscfg_kRcNotFound is resource name does not have an associated uri entry,\n
     *         fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode RemoveUriMapping(const std::string& resource_name);
    /**
     * \brief Retrieves the stored uri for a given resource name.
     *
     * \param[in] resource_name resource name.
     * \param[out] uri stored uri.
     *
     * \return fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode GetUriForResource(const std::string& resource_name, std::string& uri);

    /**
     * \brief Retrieves the encoding name for a given resource name.
     *
     * \param[in] resource_name resource name.
     * \param[out] encoding_encoding name.
     *
     * \return fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode GetEncodingName(const std::string& resource_name, std::string& encoding_name);
private:
    ///! resource mapping to URI-encoding pair.
    typedef std::map<std::string, std::pair<std::string, std::string>> UriMap;
    ///! HTTP parameter key-value pair.
    typedef std::pair<std::string, std::string> HttpParameter;

    /**
     * \brief Retrives the HTTP content-type for a given resource name.
     *
     * \param[in] resource_name resource name.
     * \param[out] content_type HTTP content-type for given resource name.
     *
     * \return fscfg_kRcNotFound if no content type can be deduced,\n
     *         fscfg_kRcSuccess otherwise.
     */
    fscfg_ReturnCode GetContentType(const std::string& resource_name, std::string& content_type);

    /**
     * \brief Retrieves the HTTP parameter list for a given resource name.
     *
     * \param[in] resource_name resource name.
     * \param[out] http_parameters associated HTTP parameters.
     *
     * \return fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode GetParameterList(const std::string& resource_name, std::vector<HttpParameter>& http_parameters);

    /**
     * \brief Computes a uri which contains the given uri adjusted with the associated http parameters for the given resource name.
     *
     * \param[in] resource_name resource name.
     * \param[in] uri resource uri.
     * \param[out] computed_uri given uri adjusted with the http parameter list.
     *
     * \return fscfg_kRcNotFound if resource is unknown,\n
     *         fscfg_kRcSuccess otherwise.
     */
    fscfg_ReturnCode ComputeUriWithParameters(const std::string& resource_name, const std::string& uri, std::string& computed_uri);

    /**
     * \brief Uses the underlying uri-mapping to retrieve the payload for a given resource name.
     * The payload is retrieved with the help of the provided TransferManager instance at the moment of instance construction.
     *
     * \param[in] resource_name resource name.
     * \param[in] resource_uri resource backend uri.
     * \param[in] encoding_name name of the encoding that the resource is to be requested.
     * \param[out] payload data beloning to the given resource.
     * \param[out] updated true if payload was retrieved from VSDP or false if cache was used and still valid.
     *
     * \return fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode Fetch(std::string resource_name,
                           std::string resource_uri,
                           std::string encoding_name,
                           std::vector<std::uint8_t>& payload,
                           bool& updated);

    std::shared_ptr<transfermanager::TransferManager> transfer_manager_;    ///< Transfer manager for HTTP requests.
    UriMap uri_map_;                                                        ///< Resource name to URI-encoding pair mapping.
    std::shared_ptr<fsm::Provisioning> provisioning_;                       ///< Foundation Services Provisioning container.
    std::shared_ptr<fsm::Config> config_;                                   ///< Foundation Services Config container.
    std::shared_ptr<LocalConfig> local_config_;                             ///< Foundation Services file-based local-config.
};
}

#endif // FSM_VSDPSOURCE_H_INC_

/** \}    end of addtogroup */
