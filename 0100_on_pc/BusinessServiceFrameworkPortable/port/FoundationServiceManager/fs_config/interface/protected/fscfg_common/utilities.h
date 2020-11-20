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
 *  \file     utilities.h
 *  \brief    Foundation Services Config utilities interface
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#ifndef FSM_UTILITIES_H_INC_
#define FSM_UTILITIES_H_INC_

#include <string>
#include <vector>
#include <map>
#include <set>
#include <memory>

#include <fscfg_common/generated/foundation-services-config-generated.h>
#include <fscfg_common/binding_set.h>

#include <fscfg/fscfg_types.h>


namespace fsm
{

/**
 * \brief Gets the base name for the given path.
 *
 * \param[in] path A dbus object path
 * \param[out] basename base name extracted from the given path
 *
 * \return fscfg_kRcSuccess on success
 */
fscfg_ReturnCode GetBasename(const std::string& path, std::string& basename);

/**
 * \brief Gets the base name for the given path.
 *
 * \param[in] path A dbus object path
 * \param[out] basename base name extracted from the given path
 *
 * \return fscfg_kRcSuccess on success
 */
fscfg_ReturnCode GetBasename(const char* path, std::string& basename);

/**
 * \brief Gets the base names for the given paths.
 *
 * \param[in] paths List of dbus object paths
 * \param[out] basenames base names extracted from the given paths
 *
 * \return fscfg_kRcSuccess on success
 */
fscfg_ReturnCode GetBasenames(const std::vector<std::string>& paths, std::vector<std::string>& basenames);

/**
 * \brief Gets the base names for the given paths.
 *
 * \param[in] paths List of dbus object paths
 * \param[out] basenames base names extracted from the given paths
 *
 * \return fscfg_kRcSuccess on success
 */
fscfg_ReturnCode GetBasenames(const char* const * paths, std::vector<std::string>& basenames);

/**
 * \brief Construct full paths for the given names and the base.
 *
 * \param[in] names list of dbus object names
 * \param[in] base base path
 * \param[out] full_paths_storage list of full paths in string.
 * \param[out] full_paths list of full paths in char.
 *
 * \return fscfg_kRcSuccess on success
 */
fscfg_ReturnCode ConvertNames(const std::vector<std::string> names,
                              const std::string& base,
                              std::vector<std::string>& full_paths_storage,
                              std::vector<const char*>& full_paths);

/**
 * \brief Convert G Variant payload type to vector of uint8_t.
 *
 * \param[in] g_payload pointer to g_variant payload.
 * @param[out] payload payload to be filled with bytes.
 *
 * \return fscfg_kRcSuccess on success
 */
fscfg_ReturnCode ConvertPayload(GVariant* g_payload, std::vector<std::uint8_t>& payload);

/**
 * \brief extract set of keys from map.
 *
 * \tparam K map key
 * \tparam T map value
 *
 * \param[in] elm_map map to extract keys from.
 * \param[out] elm_set set to add the extracted keys to.
 */
template <typename K, typename T>
void MapToKeySet(std::map<K, T>& elm_map, std::set<K>& elm_set)
{
    for (const auto& pair : elm_map)
    {
        elm_set.insert(pair.first);
    }
}

/**
 * \brief extract the values from the map.
 *
 * \tparam K map key
 * \tparam T map value
 *
 * \param[in] elm_map map to extract values from.
 * \param[out] elm_vec A vector to add the extracted values.
 */
template <typename K, typename T>
void MapToValuesVector(const std::map<K, T>& elm_map, std::vector<T>& elm_vec)
{
    for (const auto& pair : elm_map)
    {
        elm_vec.push_back(pair.second);
    }
}

/**
 * \tparam T Foundation Services Resource type.
 */
template <typename T>
struct less_by_name
{
    /**
     * \brief Compare the names if less by name.
     *
     * \param[in] lhs A pointer to left hand side.
     * \param[in] rhs A pointer to right hand side.
     *
     * \return True if lhs is less than rhs,\n
     *         False if lhs not less than rhs.
     */
    bool operator()(const std::shared_ptr<T>& lhs, const std::shared_ptr<T>& rhs)
    {
        std::string lhs_name;
        std::string rhs_name;

        lhs->GetName(lhs_name);
        rhs->GetName(rhs_name);

        return lhs_name < rhs_name;
    }
};

template <typename T>
void CallBindings(BindingSet<T>& binds, T& ev)
{
    binds.GetSync()->lock();

    for (auto& pair_id_func : binds.GetBindings())
    {
        pair_id_func.second(ev);
    }

    binds.GetSync()->unlock();
}

} // namespace fsm

#endif // FSM_UTILITIES_H_INC_

/** \}    end of addtogroup */
