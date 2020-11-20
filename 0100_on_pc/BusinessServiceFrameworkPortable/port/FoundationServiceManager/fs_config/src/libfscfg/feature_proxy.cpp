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
 *  \file     feature_proxy.cpp
 *  \brief    Foundation Services Config proxy implementation
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#include <libfscfg/feature_proxy.h>

#include <dlt/dlt.h>

DLT_IMPORT_CONTEXT(dlt_libfscfg);

namespace fsm
{

FeatureProxy::FeatureProxy(std::shared_ptr<fsm::DiscoveryProxy> discovery_proxy,
                           std::shared_ptr<ProvisionedResourceProxy> base,
                           _ConfigFeature* feature_dbus_obj)
    : ProvisionedResourceProxy(*base),
      feature_dbus_obj_(feature_dbus_obj),
      base_(base)
{
    fscfg_ReturnCode rc = ConnectSignals();

    if (rc != fscfg_kRcSuccess)
    {
        DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_ERROR,
                       "Failed to connect FeatureProxy signals on Dbus");
    }
}

FeatureProxy::~FeatureProxy()
{
}

fscfg_ReturnCode FeatureProxy::OnStateChanged(State state)
{
    return FeatureBind::OnStateChanged(state);
}

fscfg_ReturnCode FeatureProxy::OnDescriptionChanged(std::string description)
{
    return FeatureBind::OnDescriptionChanged(description);
}

fscfg_ReturnCode FeatureProxy::OnUriChanged(std::string uri)
{
    return FeatureBind::OnUriChanged(uri);
}

fscfg_ReturnCode FeatureProxy::OnIconChanged(std::string icon)
{
    return FeatureBind::OnIconChanged(icon);
}

fscfg_ReturnCode FeatureProxy::OnTagsChanged(std::vector<std::string> tags)
{
    return FeatureBind::OnTagsChanged(tags);
}

fscfg_ReturnCode FeatureProxy::GetName(std::string& name)
{
    return base_->GetName(name);
}

fscfg_ReturnCode FeatureProxy::GetState(State& state)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!feature_dbus_obj_)
    {
        rc = fscfg_kRcBadState;
    }
    else
    {
        state = static_cast<State>(config_feature_get_state(feature_dbus_obj_));
    }

    return rc;
}

fscfg_ReturnCode FeatureProxy::GetDescription(std::string& description)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!feature_dbus_obj_)
    {
        rc = fscfg_kRcBadState;
    }
    else
    {
        const char* current_description = config_feature_get_description(feature_dbus_obj_);

        if (!current_description)
        {
            DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_ERROR,
                           "Failed to get feature description from Dbus.");
            rc = fscfg_kRcError;
        }
        else
        {
            description.append(current_description);

            std::string feature_name;
            GetName(feature_name);

            DLT_LOG_STRINGF(dlt_libfscfg,
                            DLT_LOG_INFO,
                            "FeatureProxy(%s)::GetDescription: %s",
                            feature_name.c_str(),
                            current_description);
        }
    }

    return rc;
}

fscfg_ReturnCode FeatureProxy::GetUri(std::string& uri)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!feature_dbus_obj_)
    {
        rc = fscfg_kRcBadState;
    }
    else
    {
        const char* current_uri = config_feature_get_uri(feature_dbus_obj_);

        if (!current_uri)
        {
            DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_ERROR, "Failed to get feature uri from Dbus.");
            rc = fscfg_kRcError;
        }
        else
        {
            uri.append(current_uri);

            std::string feature_name;
            GetName(feature_name);

            DLT_LOG_STRINGF(dlt_libfscfg,
                            DLT_LOG_INFO,
                            "FeatureProxy(%s)::GetUri: %s",
                            feature_name.c_str(),
                            current_uri);
        }
    }
    return rc;
}

fscfg_ReturnCode FeatureProxy::GetIcon(std::string& icon)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!feature_dbus_obj_)
    {
        rc = fscfg_kRcBadState;
    }
    else
    {
        const char* current_icon = config_feature_get_icon(feature_dbus_obj_);

        if (!current_icon)
        {
            DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_ERROR, "Failed to get feature icon from Dbus.");
            rc = fscfg_kRcError;
        }
        else
        {
            icon.append(current_icon);

            std::string feature_name;
            GetName(feature_name);

            DLT_LOG_STRINGF(dlt_libfscfg,
                            DLT_LOG_INFO,
                            "FeatureProxy(%s)::GetIcon: %s",
                            feature_name.c_str(), current_icon);
        }
    }

    return rc;
}

fscfg_ReturnCode FeatureProxy::GetTags(std::vector<std::string>& tags)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!feature_dbus_obj_)
    {
        rc = fscfg_kRcBadState;
    }
    else
    {
        const char* const* current_tags = nullptr;

        current_tags = config_feature_get_tags(feature_dbus_obj_);

        while (current_tags && *current_tags)
        {
            tags.push_back(*current_tags);
            ++current_tags;
        }
    }

    return rc;
}

fscfg_ReturnCode FeatureProxy::ConnectSignals()
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!feature_dbus_obj_)
    {
        rc = fscfg_kRcBadState;
    }
    else
    {
        if (!g_signal_connect(feature_dbus_obj_,
                              "g-properties-changed",
                              G_CALLBACK(FeatureProxy::OnPropertiesChanged),
                              this))
        {
            DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_ERROR,
                           "Failed to register FeatureProxy OnPropertiesChanged callback on Dbus.");
            rc = fscfg_kRcError;
        }
    }

    return rc;
}

void FeatureProxy::OnPropertiesChanged(GDBusProxy* proxy,
                                       GVariant* changed,
                                       GStrv invalidated,
                                       gpointer feature_instance)
{
    DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_INFO, "Received FeatureProxy OnPropertiesChanged callback");

    fscfg_ReturnCode rc;

    if (!feature_instance)
    {
        DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_ERROR,
                       "Failed to execute FeatureProxy OnPropertiesChanged, invalid input.");
    }
    else
    {
        FeatureProxy* feature_proxy = reinterpret_cast<FeatureProxy*>(feature_instance);

        if (feature_proxy)
        {
            if (g_variant_n_children(changed) > 0)
            {
                // Extract property names.
                GVariantIter* iter = nullptr;
                const gchar* key = nullptr;
                GVariant* value = nullptr;

                g_variant_get(changed, "a{sv}", &iter);

                // Check which property has changed and call the bindings.
                while (g_variant_iter_loop(iter, "{&sv}", &key, &value))
                {
                    std::string key_str(key);

                    if (key_str == "State")
                    {
                        State state;

                        rc = feature_proxy->GetState(state);

                        if (rc != fscfg_kRcSuccess)
                        {
                            DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_ERROR, "Failed to get feature proxy state");
                        }
                        else
                        {
                            feature_proxy->OnStateChanged(state);
                        }
                    }
                    else if (key_str == "Description")
                    {
                        std::string description;

                        rc = feature_proxy->GetDescription(description);

                        if (rc != fscfg_kRcSuccess)
                        {
                            DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_ERROR, "Failed to get feature proxy description");
                        }
                        else
                        {
                            feature_proxy->OnDescriptionChanged(description);
                        }
                    }
                    else if (key_str == "Uri")
                    {
                        std::string uri;

                        rc = feature_proxy->GetUri(uri);

                        if (rc != fscfg_kRcSuccess)
                        {
                            DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_ERROR, "Failed to get feature proxy uri");
                        }
                        else
                        {
                            feature_proxy->OnUriChanged(uri);
                        }
                    }
                    else if (key_str == "Icon")
                    {
                        std::string icon;

                        rc = feature_proxy->GetIcon(icon);

                        if (rc != fscfg_kRcSuccess)
                        {
                            DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_ERROR, "Failed to get feature proxy icon");
                        }
                        else
                        {
                            feature_proxy->OnIconChanged(icon);
                        }
                    }
                    else if (key_str == "Tags")
                    {
                        std::vector<std::string> tags;

                        rc = feature_proxy->GetTags(tags);

                        if (rc != fscfg_kRcSuccess)
                        {
                            DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_ERROR, "Failed to get feature proxy tags");
                        }
                        else
                        {
                            feature_proxy->OnTagsChanged(tags);
                        }
                    }
                    else
                    {
                        DLT_LOG_STRINGF(dlt_libfscfg, DLT_LOG_WARN,
                                        "FeatureProxy::OnPropertiesChanged invalid property received: %s",
                                        key_str.c_str());
                    }
                }

                g_variant_iter_free(iter);
            }
        }
    }
}

}

/** \}    end of addtogroup */
