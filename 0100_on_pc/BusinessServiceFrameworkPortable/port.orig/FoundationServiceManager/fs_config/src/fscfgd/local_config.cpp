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
 *  \file     local_config.cpp
 *  \brief    Foundation Services Service File-based local-config implementation.
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */
#include <fscfgd/local_config.h>

#include <algorithm>
#include <fstream>

#include <dlt/dlt.h>

#include <fscfgd/parsing_utilities.h>
#include "local_config_interface.hpp"

DLT_IMPORT_CONTEXT(dlt_fscfgd)

namespace fsm
{

namespace
{
    const std::string kEntryPointLocalConfigSection = "FoundationServiceManager";
    const std::string kEntryPointLocalConfigKey = "GlobalEntryPointUri";
}

LocalConfig::LocalConfig()
{

}

fscfg_ReturnCode LocalConfig::GetEntryPointUrl(std::string& entry_point_url)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;
    bool lc_rc = true;
    lcfg::ILocalConfig *localconfig(lcfg::ILocalConfig::GetInstance());

    // Extract the keys we are interested about.
    lc_rc = localconfig->GetString(kEntryPointLocalConfigSection,
                                   kEntryPointLocalConfigKey,
                                   entry_point_url);
    if(!lc_rc)
    {
        DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_ERROR,
                       "LocalConfig::ILocalConfig can't read kEntryPointLocalConfigKey");
        rc = fscfg_kRcError;
    }

    return rc;
}

fscfg_ReturnCode LocalConfig::GetRunUnsecure(bool& run_unsecure)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    std::string entry_point_uri;

    rc = GetEntryPointUrl(entry_point_uri);


    if (rc == fscfg_kRcSuccess)
    {
        //convert to lower case for matching
        std::transform(entry_point_uri.begin(), entry_point_uri.end(), entry_point_uri.begin(), ::tolower);

        size_t pos = entry_point_uri.find("https");
        run_unsecure = (pos != 0);
    }

    return rc;
}


} // namespace fsm

/** \}    end of addtogroup */
