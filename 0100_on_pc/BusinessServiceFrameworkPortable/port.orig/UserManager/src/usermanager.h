/*
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
 *  \file     usermanager.h
 *  \brief    FSM UserManager
 *  \author   Imran Siddique
 *
 *  \addtogroup fsm_usermanager
 *  \{
*/

#ifndef FSM_USERMANAGER_H
#define FSM_USERMANAGER_H

#include <mutex>
#include <stack>
#include <unordered_map>

#include <openssl/evp.h>
#include <openssl/x509.h>
#include <openssl/x509_vfy.h>
#include <sqlite3.h>

#include "usermanager_interface.h"

namespace user_manager
{

/**
 * \brief Foundation Services usermanager, provides the functionality to add, delete, update
 *        and define role for foundation services user. Gets the user related data and
 *        accosciated certificated upon request.
 */
class Usermanager : public UsermanagerInterface
{

 public:

    /**
     * \brief Creates a new Usermanger instance.
     */
    Usermanager(std::shared_ptr<Keystore> keystore);

    /**
     * \brief Destroys the Usermanager instance freeing any associated resources.
     */
    ~Usermanager();

    ReturnCode GetUsers(std::vector<UserId>& ids);

    ReturnCode GetUsers(std::vector<UserId>& ids, const UserRole role);

    ReturnCode GetUser(UserId& id, const X509* certificate);

    ReturnCode GetUser(UserId& id, const std::vector<unsigned char>& common_name);

    ReturnCode GetUserRole(UserRole& role, const UserId id);

    ReturnCode GetActorCertificate(X509** certificate, UserId id);

    ReturnCode GetActorCertificates(STACK_OF(X509)* certificates, const std::vector<UserId> ids);

    ReturnCode GetShortRangeTlsClientCertificate(X509** certificate, const UserId id);

    ReturnCode GetShortRangeTlsClientCertificates(STACK_OF(X509)* certificates, const std::vector<UserId> ids);

    ReturnCode GetCommonName(std::vector<unsigned char>& common_name, const UserId id);

    ReturnCode AddUser(UserId& id,
                       UserRole role,
                       X509* actor_cert,
                       X509* shortrange_tls_client_cert = nullptr);

    ReturnCode DeleteUser(const UserId id);

 private:

    /**
     * \brief Size in bytes which all returned common names shall be
     *        padded/capped to.
     */
    const unsigned int kEnforcedCommonNameSize = 16;

    // Pointer to the keystore.
    std::shared_ptr<Keystore> keystore_;

    /**
     * \brief Gets a UserId from database.
     * \param[in]  statement  Statemnt to execute, must be such that the
     *                        columns in the result set are, in order:
     *                        id, role, actor_cert_it, shortrange_cert_id.
     * \param[out] id         Get the user id.
     * \return ReturnCode::kNotFound if valid reference not found,
     *         ReturnCode::kError on error
     *         ReturnCode::kSuccess on success.
     */
    ReturnCode GetUser(UserId& id, sqlite3_stmt* statement);

    /**
     * \brief Gets a user role from database.
     * \param[in]  statement  Statemnt to execute, must be such that the
     *                        columns in the result set are, in order:
     *                        id, role, actor_cert_it, shortrange_cert_id.
     * \param[out] role       Get the user role.
     * \return ReturnCode::kNotFound if valid reference not found,
     *         ReturnCode::kError on error
     *         ReturnCode::kSuccess on success.
     */
    ReturnCode GetUserRole(UserRole& role, sqlite3_stmt* statement);

    /**
     * \brief Get all users matching the statement
     * \param[out] ids        gets the list of users.
     * \param[in]  statement  Statemnt to execute, must be such that the
     *                        columns in the result set are, in order:
     *                        id, role, actor_cert_it, shortrange_cert_id.
     * \return ReturnCode::kNotFound if valid reference not found,
     *         ReturnCode::kError on error
     *         ReturnCode::kSuccess on success.
     */
    ReturnCode GetUsers(std::vector<UserId>& ids, sqlite3_stmt* statement);

    /**
     * \brief Get a actor certificate id
     * \param[out] actor_cert_id  Returns the actor certificate id, if found
     * \param[in]  statement      Statemnt to execute, must be such that the
     *                            columns in the result set are, in order:
     *                            id, role, actor_cert_it, shortrange_cert_id.
     * \return ReturnCode::kNotFound if valid reference not found,
     *         ReturnCode::kError on error
     *         ReturnCode::kSuccess on success.
     */
    ReturnCode GetActorCertificateId(CertificateId& actor_cert_id, sqlite3_stmt* statement);

    /**
     * \brief Get a short range tls certificate id
     * \param[out] shortrange_cert_id Returns the short range tls certificate id. if found.
     * \param[in]  statement      Statemnt to execute, must be such that the
     *                            columns in the result set are, in order:
     *                            id, role, actor_cert_it, shortrange_cert_id.
     * \return ReturnCode::kNotFound if valid reference not found,
     *         ReturnCode::kError on error
     *         ReturnCode::kSuccess on success.
     */
    ReturnCode GetShortRangeTlsClientCertificateId(CertificateId& shortrange_cert_id,
                                                   sqlite3_stmt* statement);

    /**
     * \brief Insert an entry to the user manager table.
     * \param[in]  role              User role could be
     *                               kAdmin, kUser, kDelegate etc.
     * \param[in] common_name        Common Name of the user, binary data, typically UUID.
     * \param[in] common_name_length Size of the common name buffer.
     * \param[in] actor_cert_id      Actor certificate id to be store
     * \param[in] shortrange_cert_id Short Range TLS certificate id to be store.
     * \return ReturnCode::kError, on error
     *         ReturnCode::kSuccess on success.
     */
    ReturnCode InsertUser(UserRole role,
                          const unsigned char* common_name,
                          int common_name_length,
                          CertificateId actor_cert_id,
                          CertificateId shortrange_cert_id);

    /**
     * \brief Execute statement until a valid row is found
     *        and return the first valid hit.
     *
     * \param[out] id                 Returns the user id
     * \param[out] role               Returns the user role.
     * \param[out] actor_cert_id      Returns the actor certificate id.
     * \param[out] shortrange_cert_id Returns the short range tls certificate id.
     * \param[in]  statement          Statemnt to execute, must be such that the
     *                                columns in the result set are, in order:
     *                                id, role, actor_cert_it, shortrange_cert_id
     *                                and optionally common_name.
     * \param[in] common_name         Optional, if not nullptr will be populated with
     *                                common name.
     *
     * \return ReturnCode::kNotFound if valid reference not found,
     *         ReturnCode::kError on error
     *         ReturnCode::kSuccess on success.
     */
     ReturnCode GetRow(UserId& id,
                       UserRole& role,
                       CertificateId& actor_cert_id,
                       CertificateId& shortrange_cert_id,
                       sqlite3_stmt* statement,
                       std::vector<unsigned char>* common_name = nullptr);
    /**
     * \brief check if the give role is user role type.
     *
     * \param[in] role User role to be checked.
     *
     * \return True if it is user role, otherwise false.
     */
    bool IsUserRole(const UserRole role);

    /**
     * \brief check if the give role is user certificate role type.
     *
     * \param[in] role certificate role to be checked.
     *
     * \return True if certificate role, otherwise false.
     */
    bool IsValidCertRole(const fsm::Keystore::CertificateRole role);

    /**
     * \brief Gets the first common name from a X509.
     *
     * \param[out] common_name        Will be allocated and filled with common_name on success,
     *                                comon_name will be binary data, typically an UUID.
     *                                Should not be freed by caller, data belongs to certificate.
     * \param[out] common_name_length Will be set to length of allocated buffer on success.
     * \param[in]   certificate        Certificate to extract common name from.
     *
     * \return ReturnCode::kNotFound if no CN found,
     *         ReturnCode::kInvalidArgument if certificate is not valid or bad pointers.
     *         ReturnCode::kError on error
     *         ReturnCode::kSuccess on success.
     */
    ReturnCode GetCommonName(unsigned char** common_name,
                             unsigned int& common_name_length,
                             X509* certificate);
}; // class Usermanager

} // namespace user_manager
#endif // FSM_USERMANAGER_h

/** \}    end of addtogroup */
