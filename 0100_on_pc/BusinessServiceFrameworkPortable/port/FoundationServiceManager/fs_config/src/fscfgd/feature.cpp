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
 *  \file     feature.cpp
 *  \brief    Foundation Services Feature implementation
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#include <fscfgd/feature.h>

#include <dlt/dlt.h>

#include <fscfgd/discovery.h>

DLT_IMPORT_CONTEXT(dlt_fscfgd)

namespace fsm
{

Feature::Feature(const std::string& name, std::shared_ptr<Discovery> discovery)
 : ProvisionedResource(name, discovery->GetProvisioning()),
   discovery_(discovery),
   state_(),
   description_(),
   uri_(),
   icon_(),
   tags_(),
   feature_dbus_obj_(nullptr)
{
    feature_dbus_obj_ = config_feature_skeleton_new();
}

Feature::~Feature()
{
    if (feature_dbus_obj_)
    {
        g_object_unref(feature_dbus_obj_);
    }
}

fscfg_ReturnCode Feature::GetName(std::string& name)
{
    name.append(name_);

    return fscfg_kRcSuccess;
}

fscfg_ReturnCode Feature::GetState(State& state)
{
    state = state_.Get();

    return fscfg_kRcSuccess;
}

fscfg_ReturnCode Feature::GetDescription(std::string& description)
{
    description = description_.Get();

    return fscfg_kRcSuccess;
}

fscfg_ReturnCode Feature::GetUri(std::string& uri)
{
    uri = uri_.Get();

    return fscfg_kRcSuccess;
}

fscfg_ReturnCode Feature::GetIcon(std::string& icon)
{
    icon = icon_.Get();

    return fscfg_kRcSuccess;
}

fscfg_ReturnCode Feature::GetTags(std::vector<std::string>& tags)
{
    tags = tags_.Get();

    return fscfg_kRcSuccess;
}

fscfg_ReturnCode Feature::GetObject(_ConfigFeature*& resource)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!feature_dbus_obj_)
    {
        rc = fscfg_kRcBadState;
    }
    else
    {
        resource = feature_dbus_obj_;
    }

    return rc;
}

fscfg_ReturnCode Feature::SetState(State state)
{
    state_.Get() = state;

    return fscfg_kRcSuccess;
}

fscfg_ReturnCode Feature::SetDescription(const std::string& description)
{
    description_.Get() = description;

    return fscfg_kRcSuccess;
}

fscfg_ReturnCode Feature::SetUri(const std::string& uri)
{
    uri_.Get() = uri;

    return fscfg_kRcSuccess;
}

fscfg_ReturnCode Feature::SetIcon(const std::string& icon)
{
    icon_.Get() = icon;

    return fscfg_kRcSuccess;
}

fscfg_ReturnCode Feature::SetTags(const std::vector<std::string>& tags)
{
    tags_.Get() = tags;

    return fscfg_kRcSuccess;
}

fscfg_ReturnCode Feature::Update()
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    OnStateChanged(state_.Get());
    OnDescriptionChanged(description_.Get());
    OnUriChanged(uri_.Get());
    OnIconChanged(icon_.Get());
    OnTagsChanged(tags_.Get());

    state_.Update();
    description_.Update();
    uri_.Update();
    icon_.Update();
    tags_.Update();

    return rc;
}

fscfg_ReturnCode Feature::OnStateChanged(State state)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!feature_dbus_obj_)
    {
        rc = fscfg_kRcBadState; // feature_dbus_obj is null.
    }
    else
    {
        if (state != state_.GetBack())
        {
            config_feature_set_state(feature_dbus_obj_, static_cast<guint>(state));

            // call bindings.
            rc =  FeatureBind::OnStateChanged(state);

            // Logging.
            DLT_LOG_STRINGF(dlt_fscfgd,
                            DLT_LOG_INFO,
                            "Feature(%s): State changed %u->%u",
                            name_.c_str(),
                            static_cast<unsigned>(state_.GetBack()),
                            static_cast<unsigned>(state));
        }
    }

    return rc;
}

fscfg_ReturnCode Feature::OnDescriptionChanged(std::string description)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!feature_dbus_obj_)
    {
        rc = fscfg_kRcBadState; // feature_dbus_obj is null.
    }
    else
    {
        if (description != description_.GetBack())
        {
            config_feature_set_description(feature_dbus_obj_ , description.c_str());

            // call bindings.
            rc = FeatureBind::OnDescriptionChanged(description);

            // Logging.
            DLT_LOG_STRINGF(dlt_fscfgd,
                            DLT_LOG_INFO,
                            "Feature(%s): Description changed %s->%s",
                            name_.c_str(),
                            description_.GetBack().c_str(),
                            description.c_str());
        }
    }

    return rc;
}


fscfg_ReturnCode Feature::OnUriChanged(std::string uri)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!feature_dbus_obj_)
    {
        rc = fscfg_kRcBadState; // feature_dbus_obj is null.
    }
    else
    {
        if (uri != uri_.GetBack())
        {
            config_feature_set_description(feature_dbus_obj_ , uri.c_str());

            // call bindings.
            rc = FeatureBind::OnUriChanged(uri);

            // Logging.
            DLT_LOG_STRINGF(dlt_fscfgd,
                            DLT_LOG_INFO,
                            "Feature(%s): Uri changed %s->%s",
                            name_.c_str(),
                            uri_.GetBack().c_str(),
                            uri.c_str());
        }
    }

    return rc;
}

fscfg_ReturnCode Feature::OnIconChanged(std::string icon)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!feature_dbus_obj_)
    {
        rc = fscfg_kRcBadState; // feature_dbus_obj is null.
    }
    else
    {

        if (icon != icon_.GetBack())
        {
            config_feature_set_icon(feature_dbus_obj_ , icon.c_str());

            // call bindings.
            rc = FeatureBind::OnIconChanged(icon);

            // Logging.
            DLT_LOG_STRINGF(dlt_fscfgd,
                            DLT_LOG_INFO,
                            "Feature(%s): Icon changed %s->%s",
                            name_.c_str(),
                            icon_.GetBack().c_str(),
                            icon.c_str());
        }
    }

    return rc;
}

fscfg_ReturnCode Feature::OnTagsChanged(std::vector<std::string> tags)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!feature_dbus_obj_)
    {
        rc = fscfg_kRcBadState; // feature_dbus_obj is null.
    }
    else
    {
        if (tags != tags_.GetBack())
        {
            std::vector<std::string> tags_storage;
            std::vector<const char*> bus_tags;

            ConvertNames(tags, "", tags_storage, bus_tags);

            config_feature_set_tags(feature_dbus_obj_, &bus_tags[0]);

            // call bindings.
            rc = FeatureBind::OnTagsChanged(tags);

            // Logging.
            DLT_LOG_STRINGF(dlt_fscfgd, DLT_LOG_INFO, "Feature(%s): Start tags changed...", name_.c_str());

            for (auto& tag : tags)
            {
                DLT_LOG_STRINGF(dlt_fscfgd, DLT_LOG_INFO, "Feature(%s): tag: %s", name_.c_str(), tag.c_str());
            }

            DLT_LOG_STRINGF(dlt_fscfgd, DLT_LOG_INFO, "Feature(%s): End tags changed", name_.c_str());
        }
    }

    return rc;
}

} // namespace fsm

/** \}    end of addtogroup */
