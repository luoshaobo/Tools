
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
 *  \file     usermanager_tool.cpp
 *  \brief    User Manager CLI tool
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm_user_manager
 *  \{
*/

#include "usermanager_tool.h"

#include <bits/stdtr1c++.h>
#include <iostream>

#include <cstdio>
#include <cstdlib>

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>

#include <getopt.h>

#include <openssl/pem.h>
#include <dlt/dlt.h>
#include <persistence_client_library.h>

#include "quark.h"
#include <usermanager_interface.h>
#include <keystore.h>

namespace
{
    std::map<std::string, fsm::usermanager_tool::Quark> g_quarks;
}

DLT_DECLARE_CONTEXT(dlt_usermanager_test);


static void CheckCert(fsm::Keystore& keystore,
                      fsm::Keystore::CertificateRole role,
                      const std::string& name)
{
    using namespace fsm;

    X509* cert = nullptr;
    ReturnCode rc = ReturnCode::kSuccess;

    rc = keystore.GetCertificate(&cert, role);
    if (rc == ReturnCode::kSuccess && cert)
    {
        std::cout << name << " installed\n";
    }
    else
    {
        std::cout << name << " not installed\n";
    }

    X509_free(cert);
}

static void CheckKey(fsm::Keystore& keystore,
                     fsm::Keystore::KeyRole role,
                     const std::string& name)
{
    using namespace fsm;

    EVP_PKEY* key = nullptr;
    ReturnCode rc = ReturnCode::kSuccess;


    rc = keystore.GetKey(&key, role);
    if (rc == ReturnCode::kSuccess && key)
    {
        std::cout << name << " installed\n";
    }
    else
    {
        std::cout << name << " not installed\n";
    }

    EVP_PKEY_free(key);
}


bool SetUserAndGroup()
{
    int result;
    int errno_saved;
    bool return_value = true;
    uid_t uid = 1008;
    gid_t gid = 1020;

    struct group  *group_result;
    group_result = getgrnam("pers-group");
    if (group_result != NULL)
    {
        gid = group_result->gr_gid;
    }
    else
    {
        std::cerr << "getgrnam failed!" << std::endl;
        DLT_LOG_STRING(dlt_usermanager_test, DLT_LOG_ERROR, "getgrnam failed!");
    }
    struct passwd *passwd_result;
    passwd_result = getpwnam("pers");
    if (passwd_result != NULL)
    {
        uid = passwd_result->pw_uid;
        // dont't do: gid = passwd_result->pw_gid;  // this returns wrongly congfigured passwd, which returns group "gpio" for this
    }
    else
    {
        std::cerr << "getpwnam failed!" << std::endl;
        DLT_LOG_STRING(dlt_usermanager_test, DLT_LOG_ERROR, "getpwnam failed!");
    }
    // caution: need to set gid first, otherwise setuid will fail due to missing rights!
    result = setgid(gid);
    errno_saved = errno;
    if (result != 0)
    {
        char error_string[100];
        snprintf(error_string, sizeof(error_string), "setgid(%i) failed: %i, errno=%i", static_cast<int>(gid), result, errno_saved);
        std::cerr << error_string << std::endl;
        DLT_LOG_STRING(dlt_usermanager_test, DLT_LOG_ERROR, error_string);
        return_value = false;
    }
    result = setuid(uid);
    errno_saved = errno;
    if (result != 0)
    {
        char error_string[100];
        snprintf(error_string, sizeof(error_string), "setuid(%i) failed: %i, errno=%i", static_cast<int>(uid), result, errno_saved);
        std::cerr << error_string << std::endl;
        DLT_LOG_STRING(dlt_usermanager_test, DLT_LOG_ERROR, error_string);
        return_value = false;
    }
    DLT_LOG_STRINGF(dlt_usermanager_test, DLT_LOG_INFO, "SetUserAndGroup returns %s, usr=%i, grp=%i",\
        return_value?"true":"false", static_cast<int>(getuid()), static_cast<int>(getgid()));
    return return_value;
}


int main(int argc, char *argv[])
{
    fsm::usermanager_tool::CommandLineArguments arguments;
    fsm::ReturnCode rc = fsm::ReturnCode::kUndefined;

    // Tear-up, register DLT, initialize tpPCL.
    DLT_REGISTER_APP("USRT", "FSAPI User Manger Test");
    DLT_REGISTER_CONTEXT(dlt_usermanager_test, "USRT", "FSAPI User Manager Test");
    SetUserAndGroup();  // need to set correct user and group to save the persistency with correct rights.
    rc = tpPCL_init("fsm_keystore", nullptr, FALSE) == E_PCL_ERROR_NONE
                                             ? fsm::ReturnCode::kSuccess : fsm::ReturnCode::kError;

    fsm::usermanager_tool::CreateMappings();

    // Parse command-line arguments.
    rc = rc == fsm::ReturnCode::kSuccess
         ? fsm::usermanager_tool::ParseArgs(argc, argv, arguments) : fsm::ReturnCode::kError;

    if (rc != fsm::ReturnCode::kSuccess)
    {
        std::cerr << "Failed to parse command-line arguments" << std::endl;
    }

    // Validate the command-line arguments.
    rc = rc == fsm::ReturnCode::kSuccess
               ? fsm::usermanager_tool::ValidateArgs(arguments) : fsm::ReturnCode::kError;

    if (rc != fsm::ReturnCode::kSuccess)
    {
        std::cerr << "Failed to validate command-line arguments" << std::endl;
    }

    // Execute the command-line operations.
    rc = rc == fsm::ReturnCode::kSuccess
               ? fsm::usermanager_tool::PerformOperations(arguments) : fsm::ReturnCode::kError;

    if (rc != fsm::ReturnCode::kSuccess)
    {
        std::cerr << "Failed to execute command-line arguments" << std::endl;
    }

    // Tear-down, unregister DLT, de-initialize tpPCL.
    tpPCL_deinit();

    DLT_UNREGISTER_CONTEXT(dlt_usermanager_test);
    DLT_UNREGISTER_APP();

    if (rc != fsm::ReturnCode::kSuccess)
    {
        fsm::usermanager_tool::PrintUsage();
        std::string return_code_literal;

        g_quarks["ReturnCode"].AtValue(static_cast<int>(rc), return_code_literal);

        std::cerr << "ReturnCode: " << return_code_literal << std::endl;
    }

    return rc == fsm::ReturnCode::kSuccess ? EXIT_SUCCESS : EXIT_FAILURE;
}

namespace fsm
{

namespace usermanager_tool
{

void CreateMappings()
{
    static std::map<std::string, int> key_role_map = {
        std::make_pair("VehicleCa", static_cast<int>(Keystore::KeyRole::kVehicleCa)),
        std::make_pair("VehicleActor",  static_cast<int>(Keystore::KeyRole::kVehicleActor)),
        std::make_pair("VehicleShortRangeTlsServer", static_cast<int>(Keystore::KeyRole::kVehicleShortRangeTlsServer)),
        std::make_pair("CloudTlsClient", static_cast<int>(Keystore::KeyRole::kCloudTlsClient)),
        std::make_pair("DoIpTlsServer", static_cast<int>(Keystore::KeyRole::kDoIpTlsServer)),
        std::make_pair("CloudSignEncrypt", static_cast<int>(Keystore::KeyRole::kCloudSignEncrypt))
    };

    static std::map<std::string, int> certificate_role_map = {
        std::make_pair("VehicleCa", static_cast<int>(Keystore::CertificateRole::kVehicleCa)),
        std::make_pair("VehicleActor", static_cast<int>(Keystore::CertificateRole::kVehicleActor)),
        std::make_pair("VehicleShortRangeTlsServer", static_cast<int>(Keystore::CertificateRole::kVehicleShortRangeTlsServer)),
        std::make_pair("UserShortRangeTlsClient", static_cast<int>(Keystore::CertificateRole::kUserShortRangeTlsClient)),
        std::make_pair("UserActor", static_cast<int>(Keystore::CertificateRole::kUserActor)),
        std::make_pair("CloudCa", static_cast<int>(Keystore::CertificateRole::kCloudCa)),
        std::make_pair("CloudTlsClient", static_cast<int>(Keystore::CertificateRole::kCloudTlsClient)),
        std::make_pair("DoIpTlsServer", static_cast<int>(Keystore::CertificateRole::kDoIpTlsServer)),
        std::make_pair("CloudSignEncrypt", static_cast<int>(Keystore::CertificateRole::kCloudSignEncrypt))
    };

    static std::map<std::string, int> user_role_map = {
        std::make_pair("Admin", static_cast<int>(UserRole::kAdmin)),
        std::make_pair("User", static_cast<int>(UserRole::kUser)),
        std::make_pair("Delegate", static_cast<int>(UserRole::kDelegate)),
        std::make_pair("CarAccess", static_cast<int>(UserRole::kCarAccess)),
        std::make_pair("AssistanceCall", static_cast<int>(UserRole::kAssistanceCall))
    };

    static std::map<std::string, int> return_code_map = {
        std::make_pair("Undefined", static_cast<int>(ReturnCode::kUndefined)),
        std::make_pair("Success", static_cast<int>(ReturnCode::kSuccess)),
        std::make_pair("Error", static_cast<int>(ReturnCode::kError)),
        std::make_pair("NotFound", static_cast<int>(ReturnCode::kNotFound)),
        std::make_pair("InvalidArgument", static_cast<int>(ReturnCode::kInvalidArgument)),
        std::make_pair("NotImplemented", static_cast<int>(ReturnCode::kNotImplemented))
    };

    static Quark key_role_quark(key_role_map);
    static Quark certificate_role_quark(certificate_role_map);
    static Quark user_role_quark(user_role_map);
    static Quark return_code_quark(return_code_map);

    g_quarks["KeyRole"] = key_role_quark;
    g_quarks["CertificateRole"] = certificate_role_quark;
    g_quarks["UserRole"] = user_role_quark;
    g_quarks["ReturnCode"] = return_code_quark;
}

ReturnCode ParseArgs(int argc, char* argv[], CommandLineArguments& arguments)
{
    int option;
    ReturnCode rc = ReturnCode::kSuccess;
    Operation& operation = std::get<0>(arguments);
    EntryType& entry_type = std::get<1>(arguments);
    ArgValPairs& arg_val_pairs = std::get<2>(arguments);

    operation = Operation::kUndefined;
    entry_type = EntryType::kUndefined;

    static struct option options[] = {
        {"add", required_argument, 0, 'a'},
        {"update", required_argument, 0, 'u'},
        {"remove", required_argument, 0, 'r'},
        {"list", required_argument, 0, 'l'},
        {"role", required_argument, 0, 'o'},
        {"file", required_argument, 0, 'f'},
        {"id", required_argument, 0, 'i'},
        {"help", required_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    // short option to Operation mapping
    std::map<int, Operation> operation_map = {
        std::make_pair('a', Operation::kAdd),
        std::make_pair('u', Operation::kUpdate),
        std::make_pair('r', Operation::kRemove),
        std::make_pair('l', Operation::kList)
    };

    // short option to ArgumentType mapping
    std::map<int, ArgumentType> argument_map = {
        std::make_pair('i', ArgumentType::kId),
        std::make_pair('o', ArgumentType::kRole),
        std::make_pair('f', ArgumentType::kFile)
    };

    // entry-type literal to enum value mapping
    std::map<std::string, EntryType> entry_type_map = {
        std::make_pair("key", EntryType::kKey),
        std::make_pair("certificate", EntryType::kCertificate),
        std::make_pair("user", EntryType::kUser)
    };

    while (rc == ReturnCode::kSuccess)
    {
        option = getopt_long(argc, argv, "", options, NULL);

        if (option == -1)
            break;

        switch (option)
        {
            case 'a':
            case 'u':
            case 'r':
            case 'l':
            {
                if (operation != Operation::kUndefined)
                {
                    // more than one operation provided in the command line.
                    rc = ReturnCode::kError;
                }
                else
                {
                    if (!entry_type_map.count(optarg))
                    {
                        rc = ReturnCode::kError; // bad argument.
                    }
                    else
                    {
                        operation = operation_map[option];
                        entry_type = entry_type_map[optarg];
                    }
                }

                break;
            }

            case 'i':
            case 'o':
            case 'f':
            {
                arg_val_pairs[argument_map[option]] = optarg;
                break;
            }
            case 'h':
            {
                PrintUsage();
                break;
            }

            default:
            {
                rc = ReturnCode::kError;
            }
        }
    }

    return rc;
}

ReturnCode ValidateArgs(const CommandLineArguments& arguments)
{
    ReturnCode rc = ReturnCode::kSuccess;

    const Operation& operation = std::get<0>(arguments);
    const EntryType& entry_type = std::get<1>(arguments);
    const ArgValPairs& arg_val_pairs = std::get<2>(arguments);

    // argument configuration.
    typedef std::vector<ArgumentType> NeededArguments;

    const NeededArguments kAddArguments = {ArgumentType::kFile, ArgumentType::kRole};
    const NeededArguments kRemoveArguments = {ArgumentType::kId};
    const NeededArguments kUpdateArguments = {ArgumentType::kFile, ArgumentType::kId};
    const NeededArguments kListArguments;

    std::map<Operation, NeededArguments> needed_args = {
        std::make_pair(Operation::kAdd, kAddArguments),
        std::make_pair(Operation::kRemove, kRemoveArguments),
        std::make_pair(Operation::kUpdate, kUpdateArguments),
        std::make_pair(Operation::kList, kListArguments)
    };

    // actual validation.
    // all operations need the entry type (is it a key, certificate or user?)
    if (entry_type == EntryType::kUndefined)
    {
        std::cerr << "bad entry-type" << std::endl; // TODO:Iulian:Maybe also specificy the bad string.
        rc = ReturnCode::kError;
    }
    else
    {
        // Check that for the selected operation has all it's required arguments.
        NeededArguments op_needed_args(needed_args[operation]);

        if (op_needed_args.size() != arg_val_pairs.size())
        {
            // The number of arguments doesn't match. (required vs given).
            rc = ReturnCode::kError;
        }
        else
        {
            ArgValPairs::const_iterator it = arg_val_pairs.begin();

            for (; it != arg_val_pairs.end() && rc == ReturnCode::kSuccess; ++it)
            {
                NeededArguments::const_iterator it_at = std::find(op_needed_args.begin(), op_needed_args.end(), it->first);

                // argument was not found
                if (it_at == op_needed_args.end())
                {
                    rc = ReturnCode::kError;
                }
                else
                {
                    // TODO:Iulian:I would love to have something like argument contraints and match them prior.
                    // remove it as it was matched.
                    op_needed_args.erase(it_at);
                }
            }
        }

        // Check that if a role was specified, it coresponds to the entry type.
        if (rc == ReturnCode::kSuccess)
        {
            // Choose the right mapping to look into based on the given entry-type.
            const static std::map<EntryType, const Quark*> entry_role_map = {
                std::make_pair(EntryType::kKey, &g_quarks.at("KeyRole")),
                std::make_pair(EntryType::kCertificate, &g_quarks.at("CertificateRole")),
                std::make_pair(EntryType::kUser, &g_quarks.at("UserRole"))
            };

            const std::map<std::string, int>& role_map = (entry_role_map.at(entry_type)->GetMap());

            // search the arguments for role.
            if (arg_val_pairs.count(ArgumentType::kRole))
            {
                if (!role_map.count(arg_val_pairs.at(ArgumentType::kRole)))
                {
                    // The argument for the given role was not found in the selected role map.
                    // This means that the given role for the entry type does not exist.

                    std::cerr << "bad role \'" << arg_val_pairs.at(ArgumentType::kRole) << "\'" << std::endl;
                    rc = ReturnCode::kError;
                }
            }
        }
    }

    return rc;
}

// forward declaration of all possible operations.
static ReturnCode PerformAdd(const EntryType& entry_type,
                             const ArgValPairs& arg_val_pairs,
                             std::shared_ptr<fsm::Keystore> keystore,
                             std::shared_ptr<fsm::UsermanagerInterface> um_if);

static ReturnCode PerformUpdate(const EntryType& entry_type,
                             const ArgValPairs& arg_val_pairs,
                             std::shared_ptr<fsm::Keystore> keystore,
                             std::shared_ptr<fsm::UsermanagerInterface> um_if);

static ReturnCode PerformRemove(const EntryType& entry_type,
                             const ArgValPairs& arg_val_pairs,
                             std::shared_ptr<fsm::Keystore> keystore,
                             std::shared_ptr<fsm::UsermanagerInterface> um_if);

static ReturnCode PerformList(const EntryType& entry_type,
                             const ArgValPairs& arg_val_pairs,
                             std::shared_ptr<fsm::Keystore> keystore,
                             std::shared_ptr<fsm::UsermanagerInterface> um_if);


ReturnCode PerformOperations(const CommandLineArguments& arguments)
{
    ReturnCode rc = ReturnCode::kSuccess;

    const Operation& operation = std::get<0>(arguments);
    const EntryType& entry_type = std::get<1>(arguments);
    const ArgValPairs& arg_val_pairs = std::get<2>(arguments);

    std::shared_ptr<fsm::Keystore> keystore = std::make_shared<fsm::Keystore>();
    std::shared_ptr<fsm::UsermanagerInterface> um_if = fsm::UsermanagerInterface::Create(keystore);

    switch (operation)
    {
        case Operation::kAdd:
        {
            rc = PerformAdd(entry_type, arg_val_pairs, keystore, um_if);
            break;
        }

        case Operation::kUpdate:
        {
            rc = PerformUpdate(entry_type, arg_val_pairs, keystore, um_if);
            break;
        }

        case Operation::kRemove:
        {
            rc = PerformRemove(entry_type, arg_val_pairs, keystore, um_if);
            break;
        }

        case Operation::kList:
        {
            rc = PerformList(entry_type, arg_val_pairs, keystore, um_if);
            break;
        }

        default:
        {
            std::cerr << "Unidentified operation" << std::endl;
            rc = ReturnCode::kError;
            break;
        }
    }

    return rc;
}

//! Operations implementation.
namespace
{
    const int kUndefinedUserRole = static_cast<int>(UserRole::kUndefined);
    const int kUndefinedCertificateRole = static_cast<int>(Keystore::CertificateRole::kUndefined);
}

// Add
static ReturnCode PerformAdd(const EntryType& entry_type,
                             const ArgValPairs& arg_val_pairs,
                             std::shared_ptr<fsm::Keystore> keystore,
                             std::shared_ptr<fsm::UsermanagerInterface> um_if)
{
    ReturnCode rc = ReturnCode::kSuccess;

    const std::string& file_path = arg_val_pairs.at(ArgumentType::kFile);

    X509* certificate = utils::LoadCertificateFromFile(file_path);
    STACK_OF(X509)* certificates = utils::LoadCertificatesFromFile(file_path);
    EVP_PKEY* key = utils::LoadKeyFromFile(file_path);

    switch (entry_type)
    {
        case EntryType::kUser:
        {
            UserId user_id = kUndefinedUserId;
            int user_role_value = kUndefinedUserRole;
            g_quarks["UserRole"].AtKey(arg_val_pairs.at(ArgumentType::kRole), user_role_value);

            UserRole user_role = user_role_value != kUndefinedUserRole ? static_cast<UserRole>(user_role_value)
                                                                       : UserRole::kUndefined;

            rc = um_if->AddUser(user_id, user_role, certificate);
            break;
        }

        case EntryType::kCertificate:
        {
            CertificateId certificate_id = kUndefinedCertificateId;
            int certificate_role_value = kUndefinedCertificateRole;
            g_quarks["CertificateRole"].AtKey(arg_val_pairs.at(ArgumentType::kRole), certificate_role_value);

            Keystore::CertificateRole certificate_role = certificate_role_value != kUndefinedCertificateRole
                                      ? static_cast<Keystore::CertificateRole>(certificate_role_value)
                                      : Keystore::CertificateRole::kUndefined;

            if (keystore->IsOneToMany(certificate_role))
            {
                int num_certs = sk_X509_num(certificates);

                for (int i = 0; i < num_certs && rc == ReturnCode::kSuccess; i++)
                {
                    X509* local_cert = sk_X509_value(certificates, i);
                    rc = keystore->AddCertificate(certificate_id, local_cert, certificate_role);
                }

                if (rc == ReturnCode::kError)
                {
                    std::cerr << "WARNING: PerformAdd: error whilst adding multiple certs, store may be in bad state." << std::endl;
                }
            }
            else
            {
                X509* old_cert = nullptr;
                rc = keystore->SetCertificate(certificate_id, &old_cert, certificate, certificate_role);
                utils::FreeCertificate(old_cert);
            }
            break;
        }

        case EntryType::kKey:
        {
            KeyId key_id = kUndefinedKeyId;
            int key_role_value = kUndefinedKeyId;
            g_quarks["KeyRole"].AtKey(arg_val_pairs.at(ArgumentType::kRole), key_role_value);

            Keystore::KeyRole key_role = key_role_value != kUndefinedKeyId
                                                        ? static_cast<Keystore::KeyRole>(key_role_value)
                                                        : Keystore::KeyRole::kUndefined;

            EVP_PKEY* old_key = nullptr;
            rc = keystore->SetKey(key_id, &old_key, key, key_role);
            utils::FreeKey(old_key);
            break;
        }

        default:
        {
            std::cerr << "PerformAdd: bad entry type" << std::endl;
            rc = ReturnCode::kError;
            break;
        }
    }

    utils::FreeCertificate(certificate);
    utils::FreeCertificates(certificates);
    utils::FreeKey(key);

    return rc;
}

// Update
static ReturnCode PerformUpdate(const EntryType& entry_type,
                             const ArgValPairs& arg_val_pairs,
                             std::shared_ptr<fsm::Keystore> keystore,
                             std::shared_ptr<fsm::UsermanagerInterface> um_if)
{
    ReturnCode rc = ReturnCode::kSuccess;

    const std::string& file_path = arg_val_pairs.at(ArgumentType::kFile);

    X509* certificate = utils::LoadCertificateFromFile(file_path);

    switch (entry_type)
    {
        case EntryType::kUser:
        {
            rc = ReturnCode::kNotImplemented;
            break;
        }

        case EntryType::kCertificate:
        {
            CertificateId new_certificate_id = kUndefinedCertificateId;
            int certificate_id = atoi(arg_val_pairs.at(ArgumentType::kId).c_str());

            X509* old_cert = nullptr;
            rc = keystore->UpdateCertificate(&old_cert, new_certificate_id, certificate, certificate_id);
            utils::FreeCertificate(old_cert);

            break;
        }

        case EntryType::kKey:
        {
            rc = ReturnCode::kNotImplemented;
            break;
        }

        default:
        {
            std::cerr << "PerformUpdate: bad entry type" << std::endl;
            rc = ReturnCode::kError;
            break;
        }
    }

    return rc;
}

// Remove
static ReturnCode PerformRemove(const EntryType& entry_type,
                                const ArgValPairs& arg_val_pairs,
                                std::shared_ptr<fsm::Keystore> keystore,
                                std::shared_ptr<fsm::UsermanagerInterface> um_if)
{
    ReturnCode rc = ReturnCode::kSuccess;
    int entry_id = atoi(arg_val_pairs.at(ArgumentType::kId).c_str());

    switch (entry_type)
    {
        case EntryType::kUser:
        {
            rc = um_if->DeleteUser(entry_id);
            break;
        }

        case EntryType::kCertificate:
        {
            X509* old_cert = nullptr;
            rc = keystore->DeleteCertificate(&old_cert, entry_id);
            utils::FreeCertificate(old_cert);
            break;
        }

        case EntryType::kKey:
        {
            EVP_PKEY* old_key = nullptr;
            rc = keystore->DeleteKey(&old_key, entry_id);
            utils::FreeKey(old_key);
            break;
        }

        default:
        {
            std::cerr << "PerformRemove: bad entry type" << std::endl;
            rc = ReturnCode::kError;
            break;
        }
    }

    return rc;
}

// List
static ReturnCode PerformList(const EntryType& entry_type,
                              const ArgValPairs& arg_val_pairs,
                              std::shared_ptr<fsm::Keystore> keystore,
                              std::shared_ptr<fsm::UsermanagerInterface> um_if)
{
    ReturnCode rc = ReturnCode::kSuccess;
    const std::string kLineSeparator("----------");

    switch (entry_type)
    {
        case EntryType::kUser:
        {
            std::vector<UserId> all_users;
            typedef std::tuple<UserId, int, bool, std::vector<unsigned char>> UserData;
            std::set<UserData> user_data;

            rc = um_if->GetUsers(all_users);

            if (rc == ReturnCode::kSuccess)
            {
                UserRole user_role = UserRole::kUndefined;

                std::vector<UserId>::iterator it = all_users.begin();

                for (; it != all_users.end(); ++it)
                {
                    X509* cert = nullptr;
                    um_if->GetShortRangeTlsClientCertificate(&cert, *it);
                    rc = um_if->GetUserRole(user_role, *it);

                    std::vector<unsigned char> common_name;
                    if (rc == ReturnCode::kSuccess)
                    {
                        rc = um_if->GetCommonName(common_name, *it);
                    }

                    if (rc == ReturnCode::kSuccess)
                    {
                        user_data.insert(std::make_tuple(*it,
                                                         static_cast<int>(user_role),
                                                         cert ? true : false,
                                                         common_name));
                    }

                    utils::FreeCertificate(cert);
                }
            }

            if (rc == ReturnCode::kSuccess)
            {
                std::set<UserData>::const_iterator it = user_data.begin();

                std::cout << kLineSeparator << std::endl;
                for (; it != user_data.end(); ++it)
                {
                    std::string user_role;

                    g_quarks["UserRole"].AtValue(std::get<1>(*it), user_role);

                    std::cout << "id=" << std::get<0>(*it) << " | ";
                    std::cout << "common name=";
                    for (auto uc : std::get<3>(*it))
                    {
                        std::cout << "0x" << std::hex << static_cast<int>(uc) << std::dec << ", ";
                    }
                    std::cout << std::dec << " | ";
                    std::cout << "role=" << user_role << " | ";
                    std::cout << "has short-range cert=" << std::get<2>(*it) << std::endl;
                }
                std::cout << kLineSeparator << std::endl;
            }

            break;
        }

        case EntryType::kCertificate:
        {
            static std::vector<Keystore::CertificateRole> one_to_one_roles = {
                Keystore::CertificateRole::kVehicleActor,
                Keystore::CertificateRole::kVehicleShortRangeTlsServer,
                Keystore::CertificateRole::kCloudTlsClient,
                Keystore::CertificateRole::kDoIpTlsServer,
                Keystore::CertificateRole::kCloudSignEncrypt
            };

            std::vector<Keystore::CertificateRole>::const_iterator it = one_to_one_roles.begin();

            std::cout << kLineSeparator << std::endl;
            for (; it != one_to_one_roles.end(); ++it)
            {
                std::string certificate_role;

                g_quarks["CertificateRole"].AtValue(static_cast<int>(*it), certificate_role);

                CheckCert(*keystore.get(), *it, certificate_role);
            }
            std::cout << kLineSeparator << std::endl;

            break;
        }

        case EntryType::kKey:
        {
            static std::vector<Keystore::KeyRole> one_to_one_roles = {
                Keystore::KeyRole::kVehicleActor,
                Keystore::KeyRole::kVehicleShortRangeTlsServer,
                Keystore::KeyRole::kCloudTlsClient,
                Keystore::KeyRole::kDoIpTlsServer,
                Keystore::KeyRole::kCloudSignEncrypt
            };

            std::vector<Keystore::KeyRole>::const_iterator it = one_to_one_roles.begin();

            std::cout << kLineSeparator << std::endl;
            for (; it != one_to_one_roles.end(); ++it)
            {
                std::string certificate_role;

                g_quarks["KeyRole"].AtValue(static_cast<int>(*it), certificate_role);

                CheckKey(*keystore.get(), *it, certificate_role);
            }
            std::cout << kLineSeparator << std::endl;

            break;
        }

        default:
        {
            std::cerr << "PerformList: bad entry type" << std::endl;
            rc = ReturnCode::kError;
            break;
        }
    }

    return rc;
}

void PrintUsage()
{
    std::cout << "usage: usermanager_tool [options]\n";
    std::cout << "--add [key/certificate/user]      Add/update entry by category.\n";
    std::cout << "--update [key/certificate/user]   Update entry.\n";
    std::cout << "---remove [key/certificate/user]  Remove a keystore entry.\n";
    std::cout << "--list [key/certificate/user]     List status of all keystore entr\n";
    std::cout << "--role                            Role of the entry based on entry-type\n";
    std::cout << "--file                            Path to PEM file containing the key/certificate to add/update.\n";
    std::cout << "--id                              Id of the update/remove to update/remove.\n";
    std::cout << "--help                            This help text.\n";
}

namespace utils
{

X509* LoadCertificateFromFile(const std::string& file_path)
{
    X509* cert = nullptr;
    FILE* fp = fopen(file_path.c_str(), "r");

    if (fp)
    {
        cert = PEM_read_X509(fp, NULL, 0, NULL);
        fclose(fp);
    }

    return cert;
}

STACK_OF(X509)* LoadCertificatesFromFile(const std::string& file_path)
{
    FILE* fp = fopen(file_path.c_str(), "r");

    STACK_OF(X509)* certificates = sk_X509_new_null();;

    if (fp)
    {
        X509* cert = nullptr;

        do
        {
            cert = PEM_read_X509(fp, NULL, 0, NULL);

            if (cert)
            {
                sk_X509_push(certificates, cert);
            }
        }
        while (cert);

        if (sk_X509_num(certificates) <= 0)
        {
            sk_X509_free(certificates);
            certificates = nullptr;
        }

        fclose(fp);
    }

    return certificates;
}

EVP_PKEY* LoadKeyFromFile(const std::string& file_path)
{
    EVP_PKEY* key = nullptr;
    FILE* fp = fopen(file_path.c_str(), "r");

    if (fp)
    {
        key = PEM_read_PrivateKey(fp, NULL, 0, NULL);
        fclose(fp);
    }

    return key;
}

void FreeCertificate(X509* cert)
{
    if (cert)
    {
        X509_free(cert);
    }
}

void FreeCertificates(STACK_OF(X509)* certs)
{
    if (certs)
    {
        sk_X509_pop_free(certs, X509_free);
    }
}

void FreeKey(EVP_PKEY* key)
{
    if (key)
    {
        EVP_PKEY_free(key);
    }
}

} // namespace utils

} // namespace usermanager_tool

} // namespace fsm

/** \}    end of addtogroup */
