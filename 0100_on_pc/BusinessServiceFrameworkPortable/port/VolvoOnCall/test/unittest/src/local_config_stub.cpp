/**
* Copyright (C) 2018 Continental Automotive GmbH
*
* Licensed under the "Supplier unrestricted white box" category, as
* described in the 20160322-1_SW-amendment TCAM.pdf agreement
* between Volvo Cars and Continental Automotive GmbH.
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*/

/** @file local_config_stub.cpp
 * This file stubs handles parsing of local-config data-files
 *
 * @project     VCC
 * @subsystem   LocalConfig stub
 * @version     00.01
 * @author      Maksym Mozok
 * @date        19-Feb-2018
 ***************************************************************************/


#include "local_config_interface.hpp"
#include "local_config.hpp"
#include <transactions/device_pairing_transaction.h>

namespace lcfg {


LocalConfig::LocalConfig()
{
}

LocalConfig::~LocalConfig()
{
}


bool LocalConfig::GetInt(const std::string& coll_name, const std::string& par_name, int& value)
{
    bool rc = true;

    if (par_name == volvo_on_call::kPairingVisibilityTimeout)
    {
        value = 3;
    }
    else if (par_name == volvo_on_call::kRemotePairingVisibilityTimeout)
    {
        value = 1;
    }
    else
    {
        rc = false;
    }


    return rc;
}

bool LocalConfig::GetString(const std::string& coll_name, const std::string& par_name, std::string& value)
{
    return true;
}

// ILocalConfig
ILocalConfig *ILocalConfig::GetInstance()
{
    static LocalConfig lcfg_;

    return (ILocalConfig*) &lcfg_;
}

} //Namespace lcfg
