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
 *
 *  \file     voc_dbus.cc
 *  \brief    VOC D-Bus service
 *  \author   Niklas Robertsson
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

// ** INCLUDES *****************************************************************
#include "voc_dbus.h"
#include "voc_inject_signal.h"
#include "dlt/dlt.h"

DLT_IMPORT_CONTEXT(dlt_voc);

namespace volvo_on_call
{

// Callback function for the InjectSignal method
gboolean VolvoOnCallDBus::OnHandleInjectSignal(VolvoOnCallTest *interface, GDBusMethodInvocation *invocation, const gchar *arg_command)
{
    gboolean response = FALSE;
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "%s: %s", __FUNCTION__, arg_command);
    response = VolvoOnCallTestInjectSignal::GetInstance().InjectSignals(const_cast<char*>(arg_command));
    volvo_on_call_test_complete_inject_signal(interface, invocation, response);
    return TRUE;
}

// This callback is not used. Registration is done in OnNameAcquired
void VolvoOnCallDBus::OnBusAcquired(GDBusConnection *connection, const gchar *name, gpointer user_data)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "%s", __FUNCTION__);
}

// Callback function used for registration of server D-Bus object and method callbacks
void VolvoOnCallDBus::OnNameAcquired(GDBusConnection *connection, const gchar *name, gpointer user_data)
{
    VolvoOnCallTest *interface = nullptr;
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "%s", __FUNCTION__);
    interface = volvo_on_call_test_skeleton_new();
    if (!interface)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "Failed to create skeleton (%s)", kVOCDBusService);
    }
    else
    {
        g_signal_connect(interface, "handle-inject-signal", G_CALLBACK(VolvoOnCallDBus::OnHandleInjectSignal), NULL);
        gboolean iface_exported = g_dbus_interface_skeleton_export(G_DBUS_INTERFACE_SKELETON(interface), connection, kVOCDBusObject, NULL);

        if (!iface_exported)
        {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "Failed to export interface (%s)", kVOCDBusService);
        }
        else
        {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "Interface exported (%s)", kVOCDBusService);
        }
    }
}

// Callback function when the name is lost or connection has been closed
void VolvoOnCallDBus::OnNameLost(GDBusConnection *connection, const gchar  *name, gpointer user_data)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "%s", __FUNCTION__);
}

VolvoOnCallDBus::VolvoOnCallDBus()
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "Request for owning bus name %s...", kVOCDBusService);

    owner_id_ = g_bus_own_name(G_BUS_TYPE_SYSTEM,
                               kVOCDBusService,
                               G_BUS_NAME_OWNER_FLAGS_REPLACE,
                               OnBusAcquired,
                               OnNameAcquired,
                               OnNameLost,
                               NULL,
                               NULL);
    if (owner_id_ == 0)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "Failed to aqcuire bus name %s", kVOCDBusService);
    }
}

VolvoOnCallDBus& VolvoOnCallDBus::GetInstance()
{
    static VolvoOnCallDBus instance;
    return instance;
}

} // namespace volvo_on_call

/** \}    end of addtogroup */
