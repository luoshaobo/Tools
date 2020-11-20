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
 *  \file     voc_inject_signal.cc
 *  \brief    VOC test inject signal
 *  \author   Niklas Robertsson
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

// ** INCLUDES *****************************************************************
#include "voc_inject_signal.h"

#include "voc_framework/signals/signal.h"
#include "voc_framework/signals/signal_factory.h"
#include "voc_framework/signal_sources/signal_source.h"
#include "voc_framework/signal_sources/car_access_signal_source.h"
#include "voc_framework/signal_sources/ip_command_broker_signal_source.h"
#include "voc_framework/signal_sources/vehicle_comm_signal_source.h"
#include "voc.h"

#include "signals/signal_types.h"
#include "signals/temp_signal.h"
#include "signals/bcc_cl_002_signal.h"
#include "signals/bcc_cl_020_signal.h"
#include "transactions/privacy_notification_transaction.h"

#include <fstream>
#include <getopt.h>
#include <iostream>
#include <string.h>
#include <unistd.h>

#include "dlt/dlt.h"

DLT_IMPORT_CONTEXT(dlt_voc);

namespace volvo_on_call
{

bool VolvoOnCallTestInjectSignal::InjectSignals(char *line)
{
    const int kMaxNumTokens = 5;  // inject_signal --type type --signal_file signal_file
    char* tokens[kMaxNumTokens];  // will hold tokenized command line
    const char* kDelimiter = " "; // token delimiter

    const char* kExitCommand = "exit";
    const char* kInjectSignalCommand = "inject_signal";
    const char* kCatalogueCommand = "catalogue";
    const char* kMqttCommand = "mqtt";
    const char* kCloudResourceCommand = "cloud_resource";
    const char* kGetPrivacySettingsCommand = "print_privacy_settings";
    const char* kListOfPairedDevicesRequest = "ListOfPairedDevicesRequest";
    const char* kRoleSelectedNotify = "RoleSelectedNotify";
    const char* kCaDel002Signal = "CaDel002Signal";
    const char* kCaDel010Signal = "CaDel010Signal";
    const char* kNumberOfKeysInsideVehicleRequest = "NumberOfKeysInsideVehicleRequest";
    const char* kTimeout = "Timeout";
    const char* kKeysFound = "KeysFound";
    const char* kDevicePairingConfirmationByAdminRequest = "DevicePairingConfirmationByAdminRequest";
    const char* kDevicePairingConfirmationByAdminDeviceResponse = "DevicePairingConfirmationByAdminDeviceResponse";
    const char* kPairingVisibilityRequest = "PairingVisibilityRequest";
    const char* kCodeConfirmationByUserNotify = "CodeConfirmationByUserNotify";
    const char* kBccCl020Command = "BccCl020";

    static struct option options[] = {
        {"signal-type", required_argument, 0, 't' },
        {"signal-file", required_argument, 0, 'f' },
        {"transaction-id", required_argument, 0, 'i' },
        {"signal-data", required_argument, 0, 'd' },
        {"help", no_argument, 0, 'h' },
        {0, 0, 0, 0}
    };

    std::string signal_type;
    std::string signal_file;
    std::string transaction_id;
    std::string signal_serialized;

    DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "Starting signal injection receiver.");

    // Tokenize the input so we can run getopt
    int num_tokens = 0;
    char* token = strtok(line, kDelimiter);
    while (token != NULL && num_tokens < kMaxNumTokens)
    {
        //trim trailing whitespace
        int token_length = strlen(token);
        char* end = token + token_length - 1;

        while (end > token && isspace(*end))
            end--;

        *(end + 1) = 0;

        //store the trimmed token
        tokens[num_tokens] = token;
        token = strtok(NULL, kDelimiter);
        num_tokens++;
    }

    // check command
    if (num_tokens == 0)
    {
        return false;
    }
    if ((tokens[0] != NULL) && (strncmp(tokens[0], kExitCommand, strlen(kExitCommand))) == 0)
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO,
                       "Stopping signal injection receiver due to user input.");
        return false;
    }
    else if ((tokens[0] != NULL) && (strncmp(tokens[0], kCatalogueCommand, strlen(kCatalogueCommand))) == 0)
    {
        //Trigger an upload of certificates to the catalogue
        //TODO: remove, when the full flow through IHU available
        DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN, "Catalogue upload.");
        //in reality not a CCM signal, but this is not relevant in test code
        fsm::CCMTransactionId transaction_id;
        std::shared_ptr<TempSignal> upload_signal = std::make_shared<TempSignal>(transaction_id,
                                                                                 VocSignalTypes::kCatalogueSignal);
        ca_->HandleSignal(upload_signal);
    }
    else if ((tokens[0] != NULL) && (strncmp(tokens[0],
                                             kListOfPairedDevicesRequest,
                                             strlen(kListOfPairedDevicesRequest))) == 0)
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN, "ListOfPairedDevicesRequest");

        //TODO: remove when actual signal available
        fsm::CCMTransactionId transaction_id;
        std::shared_ptr<TempSignal> signal = std::make_shared<TempSignal>(transaction_id,
                                                                          fsm::Signal::BasicSignalTypes::kListPairedDevicesRequest);
        ca_->HandleSignal(signal);
    }
    else if ((tokens[0] != NULL) && (strncmp(tokens[0], kRoleSelectedNotify, strlen(kRoleSelectedNotify))) == 0)
    {
        fsm::ResRoleSelectedNotify res_role_selected_notify;  //ToDo: replace with proper type of VehicleComm (when available)
        if (tokens[1] == NULL)
        {
            res_role_selected_notify.role_admin = true;
        }
        else
        {
            res_role_selected_notify.role_admin = (strcmp(tokens[1], "admin") == 0);  // anything else is "user"
        }
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_WARN, "RoleSelectedNotify (%s)",\
                        res_role_selected_notify.role_admin?"admin":"user");
        fsm::VehicleCommTransactionId transaction_id;  // will create automatically an ID
        std::shared_ptr<fsm::Signal> signal = std::make_shared<fsm::RoleSelectedSignal>(
                    fsm::Signal::BasicSignalTypes::kRoleSelectedNotify, transaction_id, res_role_selected_notify, vc::RET_OK);
        ca_->HandleSignal(signal);
    } else if ((tokens[0] != NULL) && (strncmp(tokens[0], kCloudResourceCommand, strlen(kCloudResourceCommand))) == 0)
        {
            if (tokens[1] == NULL)
            {
                DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "No cloud resource name entered");
            }
            else
            {
                fsm::CloudResource<void> cloud_resource(tokens[1]);
                std::vector<std::uint8_t> raw_payload = cloud_resource.GetRawPayload();
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_WARN, "Payload size for %s is %d",\
                               tokens[1], raw_payload.size());

            }
        }
    else if ((tokens[0] != NULL) && (strncmp(tokens[0], kCaDel002Signal, strlen(kCaDel002Signal))) == 0)
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN, "CaDel002Signal");

        //TODO: remove when actual signal available
        fsm::CCMTransactionId transaction_id;
        std::shared_ptr<TempSignal> signal = std::make_shared<TempSignal>(transaction_id,
                                                                          VocSignalTypes::kCaDel002Signal);
        ca_->HandleSignal(signal);
    }
    else if ((tokens[0] != NULL) && (strncmp(tokens[0], kCaDel010Signal, strlen(kCaDel010Signal))) == 0)
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN, "CaDel010Signal");

        //TODO: remove when actual signal available
        fsm::CCMTransactionId transaction_id;
        std::shared_ptr<TempSignal> signal = std::make_shared<TempSignal>(transaction_id,
                                                                          VocSignalTypes::kCaDel010Signal);
        ca_->HandleSignal(signal);
    }
    else if ((tokens[0] != NULL) && (strncmp(tokens[0], kNumberOfKeysInsideVehicleRequest, strlen(kNumberOfKeysInsideVehicleRequest))) == 0)
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN, "NumberOfKeysInsideVehicleRequest");

        //TODO: remove when actual signal available
        fsm::CCMTransactionId transaction_id;
        std::shared_ptr<TempSignal> signal = std::make_shared<TempSignal>(transaction_id,
                                                                          fsm::Signal::BasicSignalTypes::kNumberOfKeysInsideVehicleRequest);
        ca_->HandleSignal(signal);
    }
    else if ((tokens[0] != NULL) && (strncmp(tokens[0], kTimeout, strlen(kTimeout))) == 0)
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN, "Timeout");

        //TODO: remove when actual signal available
        fsm::CCMTransactionId transaction_id;
        std::shared_ptr<TempSignal> signal = std::make_shared<TempSignal>(transaction_id,
                                                                          fsm::Signal::BasicSignalTypes::kTimeout);
        ca_->HandleSignal(signal);
    }
    else if ((tokens[0] != NULL) && (strncmp(tokens[0], kKeysFound, strlen(kKeysFound))) == 0)
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN, "KeysFound");

        //TODO: remove when actual signal available
        fsm::CCMTransactionId transaction_id;
        std::shared_ptr<TempSignal> signal = std::make_shared<TempSignal>(transaction_id,
                                                                          fsm::Signal::BasicSignalTypes::kKeysFound);
        ca_->HandleSignal(signal);
    }
    else if ((tokens[0] != NULL) && (strncmp(tokens[0],
                                             kDevicePairingConfirmationByAdminRequest,
                                             strlen(kDevicePairingConfirmationByAdminRequest))) == 0)
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN, "DevicePairingConfirmationByAdminRequest");

        //TODO: remove when actual signal available
        fsm::CCMTransactionId transaction_id;
        std::shared_ptr<TempSignal> signal =
                std::make_shared<TempSignal>(transaction_id,
                                             fsm::Signal::BasicSignalTypes::kDevicePairingConfirmationByAdminRequest);
        ca_->HandleSignal(signal);
    }
    else if ((tokens[0] != NULL) && (strncmp(tokens[0],
                                             kDevicePairingConfirmationByAdminDeviceResponse,
                                             strlen(kDevicePairingConfirmationByAdminDeviceResponse))) == 0)
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN, "DevicePairingConfirmationByAdminDeviceResponse");

        //TODO: remove when actual signal available
        fsm::CCMTransactionId transaction_id;
        std::shared_ptr<TempSignal> signal =
                std::make_shared<TempSignal>(transaction_id,
                                             fsm::Signal::BasicSignalTypes::kDevicePairingConfirmationByAdminDeviceResponse);
        ca_->HandleSignal(signal);
    }
    else if ((tokens[0] != NULL) && (strncmp(tokens[0],
                                             kPairingVisibilityRequest,
                                             strlen(kPairingVisibilityRequest))) == 0)
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN, "PairingVisibilityRequest");
        fsm::VehicleCommTransactionId transaction_id;  // will create automatically an ID
        vc::Empty empty;
        std::shared_ptr<fsm::Signal> signal = std::make_shared<fsm::PairingVisibilityRequestSignal>(
                    fsm::Signal::BasicSignalTypes::kPairingVisibilityRequest, transaction_id, empty, vc::RET_OK);
        ca_->HandleSignal(signal);
    }
    else if ((tokens[0] != NULL) && (strncmp(tokens[0],
                                             kCodeConfirmationByUserNotify,
                                             strlen(kCodeConfirmationByUserNotify))) == 0)
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN, "CodeConfirmationByUserNotify");
        fsm::ResCodeConfirmationByUserNotify code_confirmation_by_user;  //ToDo: replace with proper type of VehicleComm (when available)
        code_confirmation_by_user.confirmation = true;
        std::shared_ptr<fsm::Signal> signal = std::make_shared<fsm::CodeConfirmationByUserNotifySignal>(
                    fsm::Signal::kCodeConfirmationByUserNotify, default_ihu_transaction_id, code_confirmation_by_user, vc::RET_OK);
        ca_->HandleSignal(signal);
    }
    else if ((tokens[0] != NULL) && (strncmp(tokens[0], kMqttCommand, strlen(kMqttCommand))) == 0)
    {
        //The code below is to test if we can successfully publish an MQTT message
        //It uses the BCC CL 002 signal as a test signal. Used primarily to verify the testing setup.
        //TODO: remove, when related functionality becomes stable
        DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN, "MQTT injection.");
        fsm::CCMTransactionId transaction_id;

        std::shared_ptr<BccCl002Signal> test_signal = BccCl002Signal::CreateBccCl002Signal(transaction_id, 1);

        test_signal->SetSuccess();
        fsm::CarAccessSignalSource& car_access_signal_source = fsm::CarAccessSignalSource::GetInstance();
        if (car_access_signal_source.PublishMessageOnTopic(test_signal, "vocmo_test_topic_out"))
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "MQTT publishing OK.");
        }
        else
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "MQTT publishing NOK.");
        }
    }
    else if ((tokens[0] != NULL) && (strncmp(tokens[0], kBccCl020Command, strlen(kBccCl020Command))) == 0)
    {
        //The code below is to test if we can successfully publish an MQTT message
        //It uses the BCC CL 002 signal as a test signal. Used primarily to verify the testing setup.
        //TODO: remove, when related functionality becomes stable
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "Sending BCC-CL-020.");
        fsm::CCMTransactionId transaction_id;

        std::shared_ptr<BccCl020Signal> bcc_cl_020 = BccCl020Signal::CreateBccCl020Signal(transaction_id, 1);

        bcc_cl_020->SetPositionCanBeTrusted(true);
        bcc_cl_020->SetLatitude(45.0);
        bcc_cl_020->SetLongitude(145.0);
        bcc_cl_020->SetHeading(13);

        fsm::CarAccessSignalSource& car_access_signal_source = fsm::CarAccessSignalSource::GetInstance();

        std::vector<fsm::UserId> users;
        car_access_signal_source.GetAllPairedUsers(users);
        bcc_cl_020->SetRecipients(users);

        if (car_access_signal_source.PublishMessage(bcc_cl_020))
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "Successfully published BCC-CL_020.");
        }
        else
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "Failed to publish BCC-CL-020.");
        }
    }
    else if ((tokens[0] != NULL) && (strncmp(tokens[0],
                                             kGetPrivacySettingsCommand,
                                             strlen(kGetPrivacySettingsCommand))) == 0)
    {
        bool car_stats_upload_allowed = false;
        bool location_services_allowed = false;

        bool success = PrivacyNotificationTransaction::GetPrivacySettings(car_stats_upload_allowed,
                                                                          location_services_allowed);

        if (success)
        {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Location services allowed?: %s "
                                                   "Car stat upload allowed?: %s",
                            location_services_allowed ? "true" : "false",
                            car_stats_upload_allowed ? "true" : "false");
        }
        else
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN, "Failed to get privacy settings");
        }
    }
    else if ((tokens[0] != NULL) && (strncmp(tokens[0],
                                             kInjectSignalCommand,
                                             strlen(kInjectSignalCommand))) != 0)
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN,
                       "fsm::Signal injection unknown command.");
        return false;
    }

    // use getopt to parse input

    optind = 1; //reset getopt internal index
    int option = -1;

    while (true)
    {
        option = getopt_long(num_tokens, tokens, "", options, NULL);

        if (option == -1)
            break;

        switch (option)
        {
        case 't':
            signal_type = std::string(optarg);
            break;
        case 'f':
            signal_file = std::string(optarg);
            break;
        case 'i':
            transaction_id = std::string(optarg);
            break;
        case 'd':
            signal_serialized = std::string(optarg);
            break;
        case 'h':
            return false;
            break;
        default:
            DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN, "getopt returned unexpected option.");
        }
    }

    if (signal_type.empty() || (signal_file.empty() && signal_serialized.empty()))
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "No signal data provided. Specify --signal-file or --signal-data");
        return false;
    }

    // read the signal data file

    char* signal_data = NULL;
    int signal_size = 0;

    std::ifstream file (signal_file.c_str(), std::ios::in|std::ios::binary|std::ios::ate);

    if (file.is_open())
    {
        signal_size = file.tellg();
        signal_data = new char [signal_size];
        file.seekg (0, std::ios::beg);
        file.read (signal_data, signal_size);
        file.close();
    }
    else if (!signal_file.empty()) // Log only if the parameter was given from command-line.
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "Failed to open signal file: %s.",
                        signal_file.c_str());
        return false;
    }

    // transform the signal serialized representation to signal_data if
    // signal_data was not given from file.
    if (!signal_serialized.empty() && !signal_data)
    {
        signal_size = signal_serialized.size();
        signal_data = new char[signal_size + 1]; // include null term.
        strcpy(signal_data, signal_serialized.c_str());
    }

    if (signal_data == NULL || signal_size == 0)
    {
        delete[] signal_data;
        DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "Failed to read signal data.");
        return false;
    }

    if (signal_type.compare("CCM") == 0)
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "Injecting CCM signal.");
        std::shared_ptr<fsm::Signal> signal =
                fsm::SignalFactory::DecodeCcm(reinterpret_cast<unsigned char*>(signal_data),
                                              signal_size);
        if (signal)
        {
            fsm::SignalSource& signal_source = fsm::CarAccessSignalSource::GetInstance();
            signal_source.BroadcastSignal(signal);
        }
        else
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "Failed to create injected CCM signal.");
        }
    }
    else if (signal_type.compare("IpCommandBroker") == 0)
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "Injecting IPCommandBroker signal.");
        std::shared_ptr<fsm::Signal> signal = fsm::SignalFactory::CreateSignalFromIpcb(signal_data,
                                                                                       signal_size,
                                                                                       transaction_id);
        fsm::SignalSource& signal_source = fsm::IpCommandBrokerSignalSource::GetInstance();
        signal_source.BroadcastSignal(signal);
    }
    else if (signal_type.compare("VehicleComm") == 0)
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "Injecting VehicleComm signal.");

        std::shared_ptr<fsm::Signal> signal = fsm::SignalFactory::CreateSignalFromVehicleCommSerializedString(signal_data,
                                                                                                              signal_size,
                                                                                                              transaction_id);
        if (signal)
        {
            fsm::SignalSource& signal_source = fsm::VehicleCommSignalSource::GetInstance();
            signal_source.BroadcastSignal(signal);
        }
        else
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "Failed to instantiate VehicleComm signal.");
        }
    }
    else
    {
        delete[] signal_data;
        return false;
    }

    delete[] signal_data;
    signal_data = NULL;
    return true;
}

void VolvoOnCallTestInjectSignal::SetCarAccessFeature(CarAccessFeature *ca)
{
     ca_ = ca;
}

VolvoOnCallTestInjectSignal& VolvoOnCallTestInjectSignal::GetInstance()
{
    static VolvoOnCallTestInjectSignal instance;
    return instance;
}

} // namespace volvo_on_call

/** \}    end of addtogroup */
