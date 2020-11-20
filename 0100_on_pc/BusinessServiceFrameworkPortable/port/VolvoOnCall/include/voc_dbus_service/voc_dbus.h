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
 *  \file     voc_dbus.h
 *  \brief    VOC D-Bus service
 *  \author   Niklas Robertsson
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_DBUS_H
#define VOC_DBUS_H

// ** INCLUDES *****************************************************************
#include "voc_dbus_generated.h"
#include <glib.h>

namespace volvo_on_call
{

const char* const kVOCDBusService = "com.contiautomotive.tcam.VolvoOnCall";
const char* const kVOCDBusObject = "/com/contiautomotive/tcam/VolvoOnCall/Test";

/**
 * \brief This class setup the VOC D-Bus server using gdbus and implements the InjectSignal method.
 * \note Should only be used for testing purposes.
 */
class VolvoOnCallDBus
{
public:
    /**
     * \brief Returns reference of VolvoOnCallDBus, implemented as a singelton
     * \return Reference to the instance of VolvoOnCallDBus
     */
    static VolvoOnCallDBus& GetInstance();
private:
    guint32 owner_id_;
    VolvoOnCallDBus();
    ~VolvoOnCallDBus() { }
    static void OnBusAcquired(GDBusConnection *connection, const gchar *name, gpointer user_data);
    static void OnNameAcquired(GDBusConnection *connection, const gchar *name, gpointer user_data);
    static void OnNameLost(GDBusConnection *connection, const gchar  *name, gpointer user_data);
    static gboolean OnHandleInjectSignal(VolvoOnCallTest *interface, GDBusMethodInvocation *invocation, const gchar *arg_command);
};

} // namespace volvo_on_call

#endif // VOC_DBUS_H

/** \}    end of addtogroup */
