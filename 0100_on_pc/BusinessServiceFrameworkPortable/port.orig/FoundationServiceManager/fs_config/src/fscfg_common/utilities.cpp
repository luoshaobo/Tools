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
 *  \brief    Foundation Services Config utilities implementation
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#include <fscfg_common/utilities.h>

#include <algorithm>

namespace fsm
{


struct PathSeparator
{
    bool operator()( char character ) const
    {
        return character == '/';
    }
};

fscfg_ReturnCode GetBasename(const std::string& path, std::string& basename)
{

    std::string actual_basename = std::string(std::find_if(path.rbegin(), path.rend(),PathSeparator() ).base(),
                                              path.end());
    basename.append(actual_basename);

    return fscfg_kRcSuccess;
}

fscfg_ReturnCode GetBasename(const char* path, std::string& basename)
{
    fscfg_ReturnCode rc = fscfg_kRcBadParam;

    if (path)
    {
        std::string full_path(path);

        rc = GetBasename(full_path, basename);
    }

    return rc;
}

fscfg_ReturnCode GetBasenames(const std::vector<std::string>& paths, std::vector<std::string>& basenames)
{
    for (const std::string& path : paths)
    {
        std::string basename;

        GetBasename(path, basename);

        basenames.push_back(basename);
    }

    return fscfg_kRcSuccess;
}

fscfg_ReturnCode GetBasenames(const char* const * paths, std::vector<std::string>& basenames)
{
    std::vector<std::string> full_paths;

    while (paths)
    {
        std::string full_path(*paths);
        full_paths.push_back(full_path);

        ++paths;
    }

    return GetBasenames(full_paths, basenames);
}

fscfg_ReturnCode ConvertNames(const std::vector<std::string> names,
                              const std::string& base,
                              std::vector<std::string>& full_paths_storage,
                              std::vector<const char*>& full_paths)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    for (const std::string& name : names)
    {
        std::string full_path = base + "/" + name;
        full_paths_storage.push_back(full_path);

        full_paths.push_back(full_paths_storage.rbegin()->c_str());
    }

    full_paths.push_back(nullptr);

    return rc;
}

fscfg_ReturnCode ConvertPayload(GVariant* g_payload, std::vector<std::uint8_t>& payload)
{
    GVariantIter *iter = nullptr;
    guchar str;

    g_variant_get(g_payload, "ay", &iter);

    while (g_variant_iter_loop (iter, "y", &str))
    {
        payload.push_back(str);
    }

    return fscfg_kRcSuccess;
}

} // namespace fsm

/** \}    end of addtogroup */
