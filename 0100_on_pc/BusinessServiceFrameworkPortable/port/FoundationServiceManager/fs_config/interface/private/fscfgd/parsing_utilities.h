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
 *  \file     parsing_utilities.h
 *  \brief    Foundation Services Config parsing utilities interface.
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#ifndef FSM_PARSING_UTILITIES_H_INC_
#define FSM_PARSING_UTILITIES_H_INC_


#include <string>
#include <vector>

namespace fsm
{

/**
 * \brief EntryPoint resource parsed data.
 */
struct EntryPointData
{
    std::string this_uri;                   ///< Request URI.
    std::string specification_version;      ///< Specification version.
    std::string implementation_version;     ///< Implementation version.
    std::string client_uri;                 ///< Client URI.
    std::string host;                       ///< Default host.
    std::string port;                       ///< Host port.
    std::string signal_service_uri;         ///< MQTT broker URI.
};

/**
 * \brief Features resource parsed data.
 */
struct FeatureData
{
    std::string name = "";              ///< Feature name.
    bool enabled = false;               ///< Enabled flag.
    bool visible = false;               ///< Visible flag.
    std::string uri = "";               ///< Feature URI.
    std::string description = "";       ///< Feature description.
    std::string icon = "";              ///< Feature icon URI.
    std::vector<std::string> tags;      ///< Feature tags.
};

/**
 * \brief External Diagnostics feature parsed data.
 */
struct ExternalDiagnosticsData
{
    std::string remote_session_uri;         ///< Remote Session payload URI.
    bool remote_session_exists;             ///< as field is optional, this flag indicates the existence of the field.
    std::string client_capabilities_uri;    ///< Client Capabilities payload URI.
};

/**
 * \brief Trims a given string of whitespace characters from the left hand-side.
 *
 * \param[in] input string to be trimmed from left hand-side.
 *
 * \return left hand-side trimmed string.
 */
std::string StringTrimLeft(const std::string& input);

/**
 * \brief Trims a given string of whitespace characters from the right hand-side.
 *
 * \param[in] input string to be trimmed from right hand-side.
 *
 * \return  right hand-side trimmed string.
 */
std::string StringTrimRight(const std::string& input);

/**
 * \brief Trims a given string of whitespace characters from both left and right-hand side.
 *
 * \param[in] input string to be trimmed.
 *
 * \return trimmed string.
 */
std::string StringTrim(const std::string& input);

/**
 * \brief Splits a string into tokens by given character separator.
 *
 * \param[in] input string to be tokenized.
 * \param[in] separator character separator to tokenize by.
 * \param[out] vector of tokens.
 */
void StringSplit(const std::string& input, char separator, std::vector<std::string>& splits);

/**
 * \brief Processes the given payload and extracts the relevant EntryPoint resource data.
 *
 * \param[in] payload resource payload.
 * \param[out] entry_point_data entry point data fields.
 *
 * \return true on success, false otherwise.
 */
bool ParseEntryPoint(const std::vector<std::uint8_t>& payload, EntryPointData& entry_point_data);

/**
 * \brief Processes the given payload and extracts the relevant Features resource data.
 *
 * \param[in] payload resource payload.
 * \param[out] features_data vector of feature data fields.
 *
 * \return true on success, false otherwise.
 */
bool ParseFeatures(const std::vector<std::uint8_t>& payload, std::vector<FeatureData>& features_data);

/**
 * \brief Processes the given payload and extracts the client capabilities and remote session fields from the External
 * Diagnostics feature.
 *
 * \param[in] payload resource payload.
 * \param[out] external_diagnostics_data external diagnostics data fields.
 *
 * \return true on success, false otherwise.
 */
bool ParseExternalDiagnostics(const std::vector<std::uint8_t>& payload, ExternalDiagnosticsData& external_diagnostics_data);

} // namespace fsm

#endif // FSM_PARSING_UTILITIES_H_INC_

/** \}    end of addtogroup */
