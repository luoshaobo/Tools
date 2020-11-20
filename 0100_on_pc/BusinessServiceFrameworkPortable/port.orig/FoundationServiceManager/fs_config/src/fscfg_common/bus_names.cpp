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
 *  \file     bus_names.cpp
 *  \brief    Foundation Services Config bus-names retrieval implementation
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#include <fscfg/bus_names.h>

#include <string>
#include <vector>
#include <map>

namespace fsm
{

///! constants for all known encoding names.
const char* fscfg_kXmlEncoding = "xml";
const char* fscfg_kCcmEncoding = "CCM";

///! constants for all known resource names.
const char* fscfg_kEntryPointResource = "EntryPoint";
const char* fscfg_kFeaturesResource = "Features";
const char* fscfg_kBasicCarControlResource = "BasicCarControl";
const char* fscfg_kCarAccessResource = "CarAccess";
const char* fscfg_kAssistanceCallResource = "AssistanceCall";
const char* fscfg_kAssistanceCallCallCenterSettingsResource = "AssistanceCallCallCenterSettings";
const char* fscfg_kExternalDiagnosticsResource = "ExternalDiagnostics";
const char* fscfg_kExternalDiagnosticsRemoteSessionResource = "ExternalDiagnosticsRemoteSession";
const char* fscfg_kExternalDiagnosticsClientCapabilitiesResource = "ExternalDiagnosticsClientCapabilities";

///! Static configuration of all know resource dependencies.
const std::vector<std::string> kEntryPointDependency = {};
const std::vector<std::string> kFeaturesDependency = {fscfg_kEntryPointResource};
const std::vector<std::string> kBasicCarControlDependency = {fscfg_kEntryPointResource, fscfg_kFeaturesResource};
const std::vector<std::string> kCarAccessDependency = {fscfg_kEntryPointResource, fscfg_kFeaturesResource};
const std::vector<std::string> kAssistanceCallDependency = {fscfg_kEntryPointResource, fscfg_kFeaturesResource};
const std::vector<std::string> kAssistanceCallCallCenterSettingsDependency = {fscfg_kEntryPointResource,
                                                                              fscfg_kFeaturesResource,
                                                                              fscfg_kAssistanceCallResource};
const std::vector<std::string> kExternalDiagnosticsResourceDependency = {fscfg_kEntryPointResource,
                                                                         fscfg_kFeaturesResource};
const std::vector<std::string> kExternalDiagnosticsRemoteSessionResourceDependency = {fscfg_kEntryPointResource,
                                                                                      fscfg_kFeaturesResource,
                                                                                      fscfg_kExternalDiagnosticsResource};
const std::vector<std::string> kExternalDiagnosticsClientCapabilitiesResourceDependency = {fscfg_kEntryPointResource,
                                                                                           fscfg_kFeaturesResource,
                                                                                           fscfg_kExternalDiagnosticsResource};

const std::map<std::string, std::vector<std::string>> kResourceDependency =
{
    std::make_pair(fscfg_kEntryPointResource, kEntryPointDependency),
    std::make_pair(fscfg_kFeaturesResource, kFeaturesDependency),
    std::make_pair(fscfg_kBasicCarControlResource, kBasicCarControlDependency),
    std::make_pair(fscfg_kCarAccessResource, kCarAccessDependency),
    std::make_pair(fscfg_kAssistanceCallResource, kAssistanceCallDependency),
    std::make_pair(fscfg_kAssistanceCallCallCenterSettingsResource, kAssistanceCallCallCenterSettingsDependency),
    std::make_pair(fscfg_kExternalDiagnosticsResource, kExternalDiagnosticsResourceDependency),
    std::make_pair(fscfg_kExternalDiagnosticsRemoteSessionResource, kExternalDiagnosticsRemoteSessionResourceDependency),
    std::make_pair(fscfg_kExternalDiagnosticsClientCapabilitiesResource, kExternalDiagnosticsClientCapabilitiesResourceDependency)
};

fscfg_ReturnCode fscfg_GetBusName(const char** bus_name)
{
    fscfg_ReturnCode rc = fscfg_kRcBadParam;

    if (bus_name)
    {
        static const char global_bus_name[] = "com.contiautomotive.tcam.FoundationServices.Config";
        *bus_name = global_bus_name;

        rc = fscfg_kRcSuccess;
    }

    return rc;
}

fscfg_ReturnCode fscfg_GetBaseBusPath(const char** base_bus_path)
{
    fscfg_ReturnCode rc = fscfg_kRcBadParam;

    if (base_bus_path)
    {
        static const char global_bus_path[] = "/com/contiautomotive/tcam/FoundationServices/Config";
        *base_bus_path = global_bus_path;

        rc = fscfg_kRcSuccess;
    }

    return rc;
}

fscfg_ReturnCode fscfg_GetObjectPath(const char** object_path, BusObjectType bot)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    const static std::string kBotRelLookup[] =
    {
        "Undefined",

        "Config",
        "Provisioning",
        "Discovery",

        "Encodings",
        "Sources",
        "Resources",

        "ProvisionedResource",
        "Feature",

        "Max"
    };

    static std::vector<std::string> bot_abs_lookup(fscfg_kBotMax);

    if (!(bot > fscfg_kBotUndefined && bot < fscfg_kBotMax))
    {
        rc = fscfg_kRcBadParam; // object type out of expected bounds.
    }
    else
    {
        const static std::string kEmptyString("");

        if (bot_abs_lookup[bot] == kEmptyString)
        {
            const char* base_bus_path;

            rc = fscfg_GetBaseBusPath(&base_bus_path);

            if (rc != fscfg_kRcSuccess)
            {
                rc = fscfg_kRcError;
            }
            else
            {
                bot_abs_lookup[bot] = std::string(base_bus_path) + "/" + kBotRelLookup[bot];
            }
        }

        if (rc == fscfg_kRcSuccess)
        {
            *object_path = bot_abs_lookup[bot].c_str();
        }
    }

    return rc;
}

fscfg_ReturnCode fscfg_GetParentResourceNames(const char* resource_name, std::vector<std::string>& parent_resource_names)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    std::map<std::string, std::vector<std::string>>::const_iterator it = kResourceDependency.find(resource_name);

    if (it == kResourceDependency.end())
    {
        rc = fscfg_kRcNotFound;
    }
    else
    {
        parent_resource_names.insert(parent_resource_names.end(), it->second.begin(), it->second.end());
    }

    return rc;
}

fscfg_ReturnCode fscfg_GetTopResourceName(const char* resource_name, std::string& top_resource_name)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    std::map<std::string, std::vector<std::string>>::const_iterator it = kResourceDependency.find(resource_name);

    if (it == kResourceDependency.end())
    {
        rc = fscfg_kRcNotFound;
    }
    else
    {
        top_resource_name.append(*it->second.begin());
    }

    return rc;
}

} // namespace fsm

/** \}    end of addtogroup */
