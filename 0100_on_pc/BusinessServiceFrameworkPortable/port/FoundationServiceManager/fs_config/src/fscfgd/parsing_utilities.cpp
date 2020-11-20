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
 *  \file     parsing_utilities.cpp
 *  \brief    Foundation Services Config parsing utilities implementation.
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#include <fscfgd/parsing_utilities.h>

#include <sstream>

#include <dlt/dlt.h>

#include <xml.hpp>

DLT_IMPORT_CONTEXT(dlt_fscfgd)

namespace fsm
{

namespace
{
    const std::string kWhitespaceChars = " \t\r\n";
}

std::string StringTrimLeft(const std::string& input)
{
    // Find first charater that is not a whitespace char
    // and trim to that character.
    size_t first_non_whitespace = input.find_first_not_of(kWhitespaceChars);

    std::string trimmed_left = input;

    if (first_non_whitespace != std::string::npos)
    {
        trimmed_left = trimmed_left.substr(first_non_whitespace);
    }

    return trimmed_left;
}

std::string StringTrimRight(const std::string& input)
{
    // Find last character that is not a whitespace char
    // and trim up-to and including that character.
    size_t last_non_whitespace = input.find_last_not_of(kWhitespaceChars);

    std::string trimmed_right = input;

    if (last_non_whitespace != std::string::npos)
    {
        trimmed_right = trimmed_right.substr(0, last_non_whitespace + 1);
    }

    return trimmed_right;
}

std::string StringTrim(const std::string& input)
{
    // Left and right trim.
    return StringTrimRight(StringTrimLeft(input));
}

void StringSplit(const std::string& input, char separator, std::vector<std::string>& splits)
{
    std::stringstream input_stream(input);
    std::string token;

    while (std::getline(input_stream, token, separator))
    {
        splits.push_back(token);
    }
}

bool ParseEntryPoint(const std::vector<std::uint8_t>& payload, EntryPointData& entry_point_data)
{
    bool parse_success = true;

    xmlDocPtr xml_document = nullptr;
    std::vector<std::uint8_t> prepared_payload = payload;

    // Zero-terminate the payload as it is required for parsing.
    prepared_payload.push_back(0);

    xmlInitParser();

    parse_success = xmlif::parseDocument(&xml_document, prepared_payload);

    const std::string kNamespacePrefix = "conncar";
    const std::string kNamespaceUri = "http://schemas.volvocars.biz/conncar/foundation_services/entry_point";

    if (!parse_success)
    {
        DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_ERROR, "failed to parse EntryPointData");
    }
    else
    {
        parse_success = xmlif::GetElementData(xml_document,
                                              kNamespacePrefix,
                                              kNamespaceUri,
                                              "/conncar:entry_point/conncar:this",
                                              &entry_point_data.this_uri);

        parse_success = parse_success ? xmlif::GetElementData(xml_document,
                                                              kNamespacePrefix,
                                                              kNamespaceUri,
                                                              "/conncar:entry_point/conncar:specification_version",
                                                              &entry_point_data.specification_version) : parse_success;

        parse_success = parse_success ? xmlif::GetElementData(xml_document,
                                                              kNamespacePrefix,
                                                              kNamespaceUri,
                                                              "/conncar:entry_point/conncar:implementation_version",
                                                              &entry_point_data.implementation_version) : parse_success;

        parse_success = parse_success ? xmlif::GetElementData(xml_document,
                                                              kNamespacePrefix,
                                                              kNamespaceUri,
                                                              "/conncar:entry_point/conncar:client_uri",
                                                              &entry_point_data.client_uri) : parse_success;

        parse_success = parse_success ? xmlif::GetElementData(xml_document,
                                                              kNamespacePrefix,
                                                              kNamespaceUri,
                                                              "/conncar:entry_point/conncar:host",
                                                              &entry_point_data.host) : parse_success;

        parse_success = parse_success ? xmlif::GetElementData(xml_document,
                                                              kNamespacePrefix,
                                                              kNamespaceUri,
                                                              "/conncar:entry_point/conncar:port",
                                                              &entry_point_data.port) : parse_success;

        parse_success = parse_success ? xmlif::GetElementData(xml_document,
                                                              kNamespacePrefix,
                                                              kNamespaceUri,
                                                              "/conncar:entry_point/conncar:signal_service_uri",
                                                              &entry_point_data.signal_service_uri) : parse_success;

        if (!parse_success)
        {
            DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_ERROR, "Failed to extract EntryPointData elements");
        }

        if (xml_document)
        {
            xmlFreeDoc(xml_document);
        }

        xmlCleanupParser();
    }

    return parse_success;
}

bool ParseFeatures(const std::vector<std::uint8_t>& payload, std::vector<FeatureData>& features_data)
{
    bool parse_success = true;

    xmlDocPtr xml_document = nullptr;
    std::vector<std::uint8_t> prepared_payload = payload;

    const std::string kNamespacePrefix = "conncar";
    std::string kNamespaceUri = "http://schemas.volvocars.biz/conncar/foundation_services/features";

    // Zero-terminate the payload as it is required for parsing.
    prepared_payload.push_back(0);

    xmlInitParser();

    if (!xmlif::parseDocument(&xml_document, prepared_payload))
    {
        DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_ERROR, "failed to parse Features");
        parse_success = false;
    }
    else
    {
        bool feature_found = true;
        uint32_t feature_index = 1;

        while (feature_found)
        {
            FeatureData feature_data;

            feature_found &= xmlif::GetElementData(xml_document,
                                                   kNamespacePrefix,
                                                   kNamespaceUri,
                                                   "/conncar:features/conncar:feature["+std::to_string(feature_index)+"]/conncar:name",
                                                   &feature_data.name);

            feature_found &= xmlif::GetElementData(xml_document,
                                           kNamespacePrefix,
                                           kNamespaceUri,
                                           "/conncar:features/conncar:feature["+std::to_string(feature_index)+"]/conncar:enabled",
                                           &feature_data.enabled);

            feature_found &= xmlif::GetElementData(xml_document,
                                                   kNamespacePrefix,
                                                   kNamespaceUri,
                                                   "/conncar:features/conncar:feature["+std::to_string(feature_index)+"]/conncar:visible",
                                                   &feature_data.visible);

            xmlif::GetElementData(xml_document,
                                  kNamespacePrefix,
                                  kNamespaceUri,
                                  "/conncar:features/conncar:feature["+std::to_string(feature_index)+"]/conncar:uri",
                                  &feature_data.uri);

            xmlif::GetElementData(xml_document,
                                  kNamespacePrefix,
                                  kNamespaceUri,
                                  "/conncar:features/conncar:feature["+std::to_string(feature_index)+"]/conncar:description",
                                  &feature_data.description);

            xmlif::GetElementData(xml_document,
                                  kNamespacePrefix,
                                  kNamespaceUri,
                                  "/conncar:features/conncar:feature["+std::to_string(feature_index)+"]/conncar:icon",
                                  &feature_data.icon);

            uint32_t tag_index = 1;
            bool tag_found = true;

            //parse one tag at a time until we fail to find one
            while (tag_found)
            {
                std::string tag;

                tag_found = xmlif::GetElementData(xml_document,
                                                  kNamespacePrefix,
                                                  kNamespaceUri,
                                                  "/conncar:features/conncar:feature["
                                                  + std::to_string(feature_index) +
                                                  "]/conncar:tags/conncar:tag["+ std::to_string(tag_index)+ "]",
                                                  &tag);

                if (tag_found)
                {
                    ++tag_index;
                    feature_data.tags.push_back(tag);
                }
            }

            if (feature_found)
            {
                ++feature_index;

                features_data.push_back(feature_data);

                DLT_LOG_STRINGF(dlt_fscfgd,
                                DLT_LOG_INFO,
                                "    '%s', '%s'",
                                feature_data.name.c_str(),
                                feature_data.uri.c_str());
            }
        }

        if (feature_index == 1)
        {
            // No features found.
            DLT_LOG_STRING(dlt_fscfgd,
                           DLT_LOG_ERROR,
                           "ParseFeatures: No features found!");

            parse_success = false;
        }

        if (xml_document)
        {
            xmlFreeDoc(xml_document);
        }

        xmlCleanupParser();
    }


    return parse_success;
}

bool ParseExternalDiagnostics(const std::vector<std::uint8_t>& payload, ExternalDiagnosticsData& external_diagnostics_data)
{
    bool parse_success = true;

    xmlDocPtr xml_document = nullptr;
    std::vector<std::uint8_t> prepared_payload = payload;

    // Zero-terminate the payload as it is required for parsing.
    prepared_payload.push_back(0);

    xmlInitParser();

    parse_success = xmlif::parseDocument(&xml_document, prepared_payload);

    const std::string kNamespacePrefix = "conncar";
    const std::string kNamespaceUri = "http://schemas.volvocars.biz/conncar/foundation_services/external_diagnostics";

    if (!parse_success)
    {
        DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_ERROR, "ParseExternalDiagnostics: failed to parse ExternalDiagnostics");
    }
    else
    {
        parse_success = xmlif::GetElementData(xml_document,
                                              kNamespacePrefix,
                                              kNamespaceUri,
                                              "/conncar:external_diagnostics/conncar:client_capabilities",
                                              &external_diagnostics_data.client_capabilities_uri);

        bool remote_session_exists = parse_success ? xmlif::GetElementData(xml_document,
                                                                           kNamespacePrefix,
                                                                           kNamespaceUri,
                                                                           "/conncar:external_diagnostics/conncar:remote_session",
                                                                           &external_diagnostics_data.remote_session_uri)
                                                   : parse_success;

        external_diagnostics_data.remote_session_exists = remote_session_exists;
    }

    if (xml_document)
    {
        xmlFreeDoc(xml_document);
    }

    xmlCleanupParser();

    return parse_success;
}

} // namespace fsm

/** \}    end of addtogroup */
