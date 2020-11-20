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
 *  \brief    VOC D-Bus client for inject signal functionality
 *  \author   Niklas Robertsson
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

// ** INCLUDES *****************************************************************
#include <iostream>
#include <cstring>
#include <glib.h>
#include "voc_dbus_generated.h"
#include "dlt/dlt.h"
#include "voc_dbus.h"

DLT_DECLARE_CONTEXT(dlt_vocc);

/**
 *  \brief Print signal injection usage information to stdout.
 *  \return Nothing
 */
static void usage()
{
    printf("\n"
           "Usage:\n"
           "inject_signal --signal-type type <--signal-file file> <--signal-data data>\n"
           "    --signal-type    the type of signal to inject, one of: CCM\n"
           "                                                           IpCommandBroker\n"
           "                                                           VehicleComm\n"
           "    --signal-file    path to a file with the signal data\n"
           "    --signal-data    quoated serialized signal representation\n"
           "catalogue   uploads certificates to the catalogue\n"           
           "mqtt   publishes test BCC_CL_002 signal on vocmo_test_topic_out topic\n"
           "cloud_resource resource_name prints out the size of given cloud resource\n"
           "BccCl020 publishes bcc_cl_020 with foo data to all paired clients\n"
           "print_privacy_settings\n"
           " -- below commands will trigger a \"TempSignal\" --\n"
           "RoleSelectedNotify [admin/user]\n"
           "CaDel002Signal\n"
           "CaDel010Signal\n"
           "NumberOfKeysInsideVehicleRequest\n"
           "Timeout\n"
           "KeysFound\n"
           "DevicePairingConfirmationByAdminRequest\n"
           "DevicePairingConfirmationByAdminDeviceResponse\n"
           "PairingVisibilityRequest\n"
           "CaDpa002\n"
           "CodeConfirmationByUserNotify\n"
           "\n");
}

int main(int argc, const char* argv[])
{
    VolvoOnCallTest *proxy = nullptr;
    GError *error = nullptr;
    gboolean response = false;
    std::string line = "";

    DLT_REGISTER_APP("VOCC", "Volvo On Call D-Bus client");
    DLT_REGISTER_CONTEXT(dlt_vocc, "VOCC", "Volvo On Call D-Bus client");

    DLT_LOG_STRING(dlt_vocc, DLT_LOG_DEBUG, "VoC D-Bus client starting...");

    proxy = volvo_on_call_test_proxy_new_for_bus_sync(G_BUS_TYPE_SYSTEM,
                                                      G_DBUS_PROXY_FLAGS_NONE,
                                                      volvo_on_call::kVOCDBusService,
                                                      volvo_on_call::kVOCDBusObject,
                                                      NULL,
                                                      &error);
    if (!proxy)
    {
        if (error)
        {
            DLT_LOG_STRINGF(dlt_vocc, DLT_LOG_ERROR, "Failed to create proxy D-Bus object, error: %s", error->message);
            fprintf(stderr, "Failed to create proxy D-Bus object, error: %s\n", error->message);
            g_clear_error(&error);
        }
        else
        {
            DLT_LOG_STRING(dlt_vocc, DLT_LOG_ERROR, "Failed to create proxy D-Bus object");
            fprintf(stderr, "Failed to create proxy D-Bus object\n");
        }
    }
    else
    {
        printf(">> ");
        while (std::getline(std::cin, line))
        {
            if ((line == "q") || (line == "quit"))
            {
                break;
            }
            else
            {
                volvo_on_call_test_call_inject_signal_sync(proxy, line.c_str(), &response, NULL, &error);
                if (error)
                {
                    DLT_LOG_STRINGF(dlt_vocc, DLT_LOG_ERROR, "D-Bus method error: %s", error->message);
                    fprintf(stderr, "D-Bus method error: %s\n", error->message);
                    g_clear_error(&error);
                }
                else if (!response)
                {
                    DLT_LOG_STRING(dlt_vocc, DLT_LOG_ERROR, "Failed to parse command line");
                    fprintf(stderr, "Failed to parse command line\n");
                    usage();
                }
            }
            printf(">> ");
        }
        g_object_unref(proxy);
    }

    DLT_UNREGISTER_CONTEXT(dlt_vocc);
    DLT_UNREGISTER_APP();

    return 0;
}

/** \}    end of addtogroup */
