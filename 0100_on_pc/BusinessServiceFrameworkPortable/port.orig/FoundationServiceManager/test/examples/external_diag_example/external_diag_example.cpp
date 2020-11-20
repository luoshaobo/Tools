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
 *  \file     external_diag_example.cpp
 *  \brief    voc_framework example usage for ExternalDiagnostics resource.
 *  \author   Iulian Sirghi
 *
 *  \addtogroup transfermanager
 *  \{
 */

#include <cstdlib>

#include <iostream>

#include <unistd.h>

#include <dlt/dlt.h>

// FsmInitialize, FsmUninitialize
#include <fsm.h>

// Resource name constants
#include <fscfg/bus_names.h>

#include <voc_framework/features/feature_config.h>
#include <voc_framework/signals/cloud_resource.h>
#include <voc_framework/signals/signal_factory.h>

DLT_DECLARE_CONTEXT(dlt_edex);


/**
 * \brief RemoteSession handler logic for situations when a new payload for RemoteSession becomes available.
 *
 * \param[in] new_payload pointer to the current payload of RemoteSession when notification was triggered.
 */
void PrintRemoteSession(std::shared_ptr<fsm::XmlPayload> new_payload);

int main(int argc, const char* argv[])
{
    // Perform application initialization. Register the DLT context, allocate and construct the data models related
    // to the ExternalDiagnostics feature and it's resource(s).

    DLT_REGISTER_APP("EDEX", "External Diagnostics Example");
    DLT_REGISTER_CONTEXT(dlt_edex, "EDEX", "External Diagnostics Example");

    // Initialize our telematics basis functions. (Part of FoundationServiceManager).
    FsmInitialize();

    // Activate the possibility to read from XML payloads. The rationale behind it is to enable voc_framework to perform
    // additional decoding or decryption on a XML payload prior to returning the XML to the clients.
    fsm::XmlPayload xml_payload;
    fsm::PayloadInterface::PayloadIdentifier xml_identifier = xml_payload.GetIdentifier().payload_identifier;
    fsm::SignalFactory::RegisterPayloadFactory<fsm::XmlPayload>(xml_identifier);

    // Instantiate the object we are interested in monitoring and/or processing. In this particular example, we will
    // care about the ExternalDiagnostics feature as well as RemoteSession  - we are interested just in the state of
    // the feature (is it enabled or disabled?) We might not wish to handle the details of the RemoteSession setup if
    // the ExternalDiagnostics feature is disabled - there is a link between the RemoteSession, ClientCapabilities
    // and ExternalDiagnostics; We see RemoteSession and ClientCapabilities as child resources to ExternalDiagnostics,
    // as such, it makes a lot of sense to add the additional sanity check and decline a request to handle the VPN
    // Setup details if the ExternalDiagnostics feature is disabled.
    fsm::FeatureConfig external_diagnostics_feature(fsm::fscfg_kExternalDiagnosticsResource);

    // Instantiate the object that represents the RemoteSession resource. If interested, one can also
    // instantiate the ClientCapabilities resource as fsm::fscfg_kExternalDiagnosticsClientCapabilitiesResource.
    fsm::CloudResource<fsm::XmlPayload> remote_session_resource(fsm::fscfg_kExternalDiagnosticsRemoteSessionResource);

    // So, our intention within this example is, if needed, perform an initial processing of the
    // current-available resource data, and register for later changes.
    if (external_diagnostics_feature.IsEnabled())
    {
        DLT_LOG_STRING(dlt_edex, DLT_LOG_INFO, "perform initial processing for RemoteSession");

        std::shared_ptr<fsm::XmlPayload> remote_session_payload = remote_session_resource.GetPayload();

        // Here's a little caveat that might not be immediately visible; You should avoid calling a change
        // notification handler directly, without proper synchronization as the change handler can be called
        // from a worker thread that listens to changes through IPC boundaries (using corresponding middle-ware,
        // such as D-Bus).
        PrintRemoteSession(remote_session_payload);
    }

    // Register a change notification handler that will print the payload of the RemoteSession resource. Situations
    // when a change notification is dispatched at the resource level include, cache entry replacement (even if the
    // trigger is another voc_framework application), resource availability or expiry notification.
    //
    // Here is a little bit of rationale and background information. What we need to understand is that
    // a resource described by VSDP (and not only; picture is a bit bigger but out of scope for this example)
    // can either expire via the signaling service (we can be instructed to re-fetch a resource on demand of
    // the VSDP Cloud via aspects that regard provisioning) or the HTTP Cache directives and mechanisms can
    // deduce that a resource's payload needs to be replaced with a newer one; in such situations we
    // (clients to VoC Framework) receive a notification at the resource level that we can register for in order
    // to re-process resources that we care about; The above explanation is just a small attempt to offer a
    // bit of background as to why we offer such functionality and to raise awareness if client should/want
    // to care about.
    remote_session_resource.RegisterChangeHandler(std::bind(PrintRemoteSession, std::placeholders::_1));

    // Wait until enter is pressed.
    bool enter_pressed = false;

    std::cout << "Press Enter to continue..." << std::endl;

    do
    {
        char read_char;

        read(STDIN_FILENO, &read_char, 1);

        enter_pressed = read_char == '\n' ? true : false;

    } while (!enter_pressed);

    // Tear down the FSM libs.
    FsmTerminate();

    // Unregister DLT context and application.
    DLT_UNREGISTER_CONTEXT(dlt_edex);
    DLT_UNREGISTER_APP();

    return EXIT_SUCCESS;
}

/**
 * RemoteSession handler logic for situations when a new payload for RemoteSession becomes available.
 * Concurrency-wise, callback is received on a worker thread started when first accessing the resource configuration
 * interfaces (these interfaces are part of libfscfg - voc_framework uses this layer underneath to request access
 * to resource state and data - the resources are tracked from within an active service (fscfgd daemon), which keeps the
 * resources in-sync with their data sources (For example, VSDP)).The change callback is called in two situations,
 * either there is an expiry notification received on a given resource, or the system discovers that the resource has
 * changed (e.g. another client has requested the resource, it was re-downloaded and the payload turned out to be
 * modified).
 *
 * \param[in] new_payload pointer to the current payload of RemoteSession when notification was triggered.
 */
void PrintRemoteSession(std::shared_ptr<fsm::XmlPayload> new_payload)
{
    if (!new_payload)
    {
        // RemoteSession resource is optional - it's existence is dynamic.
        DLT_LOG_STRING(dlt_edex, DLT_LOG_ERROR, "no payload for RemoteSession");
    }
    else
    {
        // Well, this would be a good time to somehow process the data (use your favorite XML parser). As that is
        // outside the scope of this example, we will just print the RemoteSession XML data. Kindly ask XmlPayload to
        // retrieve the raw XML.
        std::string remote_session_raw_xml = new_payload->GetXmlData();

        if (remote_session_raw_xml.size() == 0)
        {
            DLT_LOG_STRING(dlt_edex, DLT_LOG_ERROR, "failed to get RemoteSession XML data");
        }
        else
        {
            DLT_LOG_STRING(dlt_edex,
                           DLT_LOG_INFO,
                           "Successfully retrieved payload for RemoteSession");

            // Print the RemoteSessions's payload.
            std::cout << remote_session_raw_xml << std::endl;
        }
    }
}

/** \}    end of addtogroup */

