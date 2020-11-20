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
 *  \file     bus_names.h
 *  \brief    Foundation Services Config bus-names retrieval interface
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#ifndef FSM_BUS_NAMES_H_INC_
#define FSM_BUS_NAMES_H_INC_

#include <fscfg/fscfg_types.h>

#include <vector>

namespace fsm
{

/**
 * \brief The DBus object paths
 */
enum BusObjectType
{
    fscfg_kBotUndefined,                ///< Undefined.

    fscfg_kBotConfig,                   ///< Config interface.
    fscfg_kBotProvisioning,             ///< Provisioning interface.
    fscfg_kBotDiscovery,                ///< Discovery interface.
    fscfg_kBotConfigEncoding,           ///< Config Encoding Object Manager path.
    fscfg_kBotConfigSource,             ///< Config Source Object Manager path.
    fscfg_kBotConfigResource,           ///< Config Resource Object Manager path.
    fscfg_kBotProvisioningResource,     ///< Provisioning ProvisionedResource Object Manager path.
    fscfg_kBotDiscoveryFeature,         ///< Discovery Feature Object Manager path.

    fscfg_kBotMax
};

/**
 * \brief D-bus name getter for Configuration Daemon service.
 *
 * \param[out] bus_name null-terminated d-bus name string.
 *
 * \return fscfg_kRcSuccess on success,\n
 *         fscfg_kRcBadParam if bus_name is null
 */
fscfg_ReturnCode fscfg_GetBusName(const char** bus_name);

/**
 * \brief D-bus base-path getter for Configuration Daemon service.
 * All exported "fs_config" objects have the retrieved base-path in common.
 *
 * \param[out] base_bus_path null-terminated d-bus name string.
 *
 * \return fscfg_kRcSuccess on success,\n
 *         fscfg_kRcBadParam if bus_name is null
 */
fscfg_ReturnCode fscfg_GetBaseBusPath(const char** base_bus_path);

/**
 * \brief D-bus base-path getter for Configuration Daemon service.
 * Gets the full path of the given object type.
 *
 * \param[out] object_path The object path for given object type
 * \param[in] bot The object type
 *
 * \return fscfg_kRcSuccess on success,\n
 *         fscfg_kRcBadParam object type out of expected bounds,\n
 *         fscfg_kRcError on error.
 */
fscfg_ReturnCode fscfg_GetObjectPath(const char** object_path, BusObjectType bot);


/**
 * Foundation Services Config Bus encoding names.
 */
///! XML encoding identifier.
extern const char* fscfg_kXmlEncoding;

///! CCM encoding identifier.
extern const char* fscfg_kCcmEncoding;

/**
 * Foundation Services Config Bus resource names.
 */
///! EntryPoint resource identifier.
extern const char* fscfg_kEntryPointResource;
///! Features resource identifier.
extern const char* fscfg_kFeaturesResource;
///! CarAccess resource identifier.
extern const char* fscfg_kCarAccessResource;
///!  BasicCarControl resource identifier.
extern const char* fscfg_kBasicCarControlResource;
///! AssistanceCall resource identifer.
extern const char* fscfg_kAssistanceCallResource;
///! AssistanceCall.CallCenterSettings resource identifier.
extern const char* fscfg_kAssistanceCallCallCenterSettingsResource;
///! ExternalDiagnostics resource identifier.
extern const char* fscfg_kExternalDiagnosticsResource;
///! ExternalDiagnostics.RemoteSession resource identifier.
extern const char* fscfg_kExternalDiagnosticsRemoteSessionResource;
///! ExternalDiagnostics.ClientCapabilities resource identifier.
extern const char* fscfg_kExternalDiagnosticsClientCapabilitiesResource;

/**
 * \brief Retrives the chain of resources that given resource depends on.
 *
 * \param[in] resource_name resource to retrieve the chain for.
 * \param[out] parent_resource_names chain of resources that given resource depends on.
 *
 * \return fscfg_kRcNotFound if configuration for given resource is not known\n,
 *         fscfg_kRcSuccess on success.
 */
fscfg_ReturnCode fscfg_GetParentResourceNames(const char* resource_name, std::vector<std::string>& parent_resource_names);
/**
 * \brief Retrives the top of the chain of resources that given resource depends on.
 *
 * \param[in] resource_name resource to retrieve the top of the chain for.
 * \param[out] top_resource_name top of the chain of resources that given resource depends on.
 *
 * \return fscfg_kRcNotFound if configuration for given resource is not known\n,
 *         fscfg_kRcSuccess on success.
 */
fscfg_ReturnCode fscfg_GetTopResourceName(const char* resource_name, std::string& top_resource_name);
}


#endif // FSM_FSCFG_BUS_NAMES_H_INC_

/** \}    end of addtogroup */
