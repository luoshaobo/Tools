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
 *  \file     encoding_proxy.cpp
 *  \brief    Foundation Services Encoding proxy
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#include <libfscfg/encoding_proxy.h>

#include <libfscfg/config_proxy.h>

namespace fsm
{

EncodingProxy::EncodingProxy(std::shared_ptr<fsm::ConfigProxy> config_proxy, _ConfigEncoding* encoding_dbus_obj_)
 : encoding_dbus_obj__(encoding_dbus_obj_)
{
}

EncodingProxy::~EncodingProxy()
{
}

fscfg_ReturnCode EncodingProxy::GetName(std::string& name)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!encoding_dbus_obj__)
    {
        rc = fscfg_kRcBadState;
    }
    else
    {
        name += config_encoding_get_name(encoding_dbus_obj__);
    }

    return rc;
}

} // namespace fsm

/** \}    end of addtogroup */
