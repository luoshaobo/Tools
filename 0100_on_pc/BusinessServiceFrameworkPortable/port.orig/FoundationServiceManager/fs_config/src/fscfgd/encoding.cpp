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
 *  \file     encoding.cpp
 *  \brief    Foundation Services Encoding implementation
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#include <fscfgd/encoding.h>

namespace fsm
{

Encoding::Encoding(const std::string& name)
 : name_(name),
   encoding_dbus_obj_(nullptr)
{
    encoding_dbus_obj_ = config_encoding_skeleton_new();

    if (encoding_dbus_obj_)
    {
        config_encoding_set_name(encoding_dbus_obj_, name_.c_str());
    }
}

Encoding::~Encoding()
{
    if (encoding_dbus_obj_)
    {
        g_object_unref(encoding_dbus_obj_);
    }
}

fscfg_ReturnCode Encoding::GetName(std::string& name)
{
    name.append(name_);

    return fscfg_kRcSuccess;
}

fscfg_ReturnCode Encoding::GetObject(_ConfigEncoding*& encoding)
{
    encoding = this->encoding_dbus_obj_;

    return fscfg_kRcSuccess;
}

fscfg_ReturnCode Encoding::Update()
{
    return fscfg_kRcSuccess;
}

} // namespace fsm
