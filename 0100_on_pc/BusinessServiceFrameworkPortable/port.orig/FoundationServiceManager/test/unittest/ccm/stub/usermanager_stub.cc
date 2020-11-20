/**
 * Copyright (C) 2017 Continental Automotive GmbH
 *
 * Licensed under the "Supplier unrestricted white box" category, as
 * described in the 20160322-1_SW-amendment TCAM.pdf agreement
 * between Volvo Cars and Continental Automotive GmbH.
 *
 * Unless required by applicable law or agreed to in writing, software
% * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *  \file     usermanager_stub.cc
 *  \brief    usermanager stub implemenation
 *  \author   Imran Siddique
 *
 *  \addtogroup unittest
 *  \{
 */

#include <iostream>

#include "keystore_stub.h"
#include "usermanager_interface.h"

namespace user_manager
{

class UsermanagerInterfaceTestImpl : public UsermanagerInterface
{

    ReturnCode GetUsers(std::vector<UserId>& ids, UserRole role)
    {
        std::cerr << "GetUsers() not implemented!" << std::endl;
        return ReturnCode::kNotImplemented;
    }

    ReturnCode GetShortRangeTlsClientCertificate(X509** certificate, const UserId id)
    {
        std::cerr << "GetShortRangeTlsClientCertificate() not implemented!" << std::endl;
        return ReturnCode::kNotImplemented;
    }

    ReturnCode GetShortRangeTlsClientCertificates(STACK_OF(X509)* certificates, const std::vector<UserId> ids)
    {
        std::cerr << "GetShortRangeTlsClientCertificates() not implemented!" << std::endl;
        return ReturnCode::kNotImplemented;
    }

    ReturnCode AddUser(UserId& id,
                       UserRole role,
                       X509* actor_cert,
                       X509* shortrange_tls_client_cert = nullptr)
    {
        std::cerr << "AddUser() not implemented!" << std::endl;
        return ReturnCode::kNotImplemented;
    }

    ReturnCode DeleteUser(const UserId id)
    {
        std::cerr << "DeleteUser() not implemented!" << std::endl;
        return ReturnCode::kNotImplemented;
    }

    ReturnCode GetUsers(std::vector<UserId>& ids)
    {
        ids.assign(fsm::users.begin(), fsm::users.end());
        return ReturnCode::kSuccess;
    }

    ReturnCode GetUser(UserId& id, const X509* certificate)
    {
        id = fsm::client_user_id;
        return ReturnCode::kSuccess;
    }

    ReturnCode GetUser(UserId& id, const std::vector<unsigned char>& common_name)
    {
        id = fsm::client_user_id;
        return ReturnCode::kSuccess;
    }

    ReturnCode GetUserRole(UserRole& role, const UserId user)
    {
        ReturnCode ret_code = ReturnCode::kSuccess;

        if (fsm::kUndefinedUserId == user)
        {
            role = UserRole::kUndefined;
            ret_code = ReturnCode::kNotFound;
        }
        else
        {
            role = UserRole::kUser;
        }

        return ret_code;
    }

    ReturnCode GetActorCertificateId(UserId id, CertificateId& actor_cert_id)
    {
        ReturnCode ret_code = ReturnCode::kSuccess;

        auto element = fsm::users_certs_pair.find(id);
        if (element != fsm::users_certs_pair.end())
        {
            actor_cert_id = element->second;
        }
        else
        {
            ret_code = ReturnCode::kNotFound;
        }

        return ret_code;
    }

    ReturnCode GetActorCertificate(X509** certificate, UserId id)
    {
        ReturnCode rc = ReturnCode::kSuccess;

        fsm::Keystore keystore;
        fsm::CertificateId cert_id;

        if (id != fsm::client_user_id)
        {
            rc = ReturnCode::kNotFound;
        }

        if (rc == ReturnCode::kSuccess)
        {
            rc = GetActorCertificateId(id, cert_id);
        }

        if (rc == ReturnCode::kSuccess)
        {
            keystore.GetCertificate(certificate, cert_id);
        }

        return rc;
    }

    ReturnCode GetActorCertificates(STACK_OF(X509)* certificates, const std::vector<UserId> ids)
    {
        fsm::Keystore keystore;

        for (UserId local_ids : ids)
        {
            if (fsm::kUndefinedUserId == local_ids)
            {
                return ReturnCode::kNotFound;
            }
        }

        return keystore.GetKnownSignerCertificates(certificates) ==
            fsm::ReturnCode::kSuccess ?
            ReturnCode::kSuccess : ReturnCode::kError;
    }

    ReturnCode GetCommonName(std::vector<unsigned char>& common_name, const UserId id)
    {
        return ReturnCode::kNotImplemented;
    }
};

std::shared_ptr<UsermanagerInterface> UsermanagerInterface::Create(std::shared_ptr<Keystore> keystore)
{
    return std::make_shared<UsermanagerInterfaceTestImpl>();
}

UsermanagerInterface::~UsermanagerInterface()
{
}

} // namespace fsm
