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
 *  \file     usermanager_tool.h
 *  \brief    User Manager CLI tool functions
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm_usermanager
 *  \{
*/

#ifndef USERMANAGER_TOOL_H_
#define USERMANAGER_TOOL_H_

#include "usermanager_tool_types.h"

#include <usermanager_types.h>

#include <openssl/evp.h>
#include <openssl/x509.h>

namespace fsm
{

namespace usermanager_tool
{

void CreateMappings();

ReturnCode ParseArgs(int argc, char* argv[], CommandLineArguments& arguments);

ReturnCode ValidateArgs(const CommandLineArguments& arguments);

ReturnCode PerformOperations(const CommandLineArguments& arguments);

void PrintUsage();

namespace utils
{
    X509* LoadCertificateFromFile(const std::string& file_path);
    STACK_OF(X509)* LoadCertificatesFromFile(const std::string& file_path);
    EVP_PKEY* LoadKeyFromFile(const std::string& file_path);
    void FreeCertificate(X509* cert);
    void FreeCertificates(STACK_OF(X509)* certs);
    void FreeKey(EVP_PKEY* key);
}

} // namespace usermanager_tool

} // namespace fsm

#endif // USERMANAGER_TOOL_H_

/** \}    end of addtogroup */
