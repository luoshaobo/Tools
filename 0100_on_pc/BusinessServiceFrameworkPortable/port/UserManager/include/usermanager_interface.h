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
 *  \file     usermanager_interface.h
 *  \brief    FSM UserManager Interface
 *  \author   Imran Siddique
 *
 *  \addtogroup fsm_usermanagerinterface
 *  \{
*/

#ifndef FSM_USERMANAGERINTERFACE_H
#define FSM_USERMANAGERINTERFACE_H

#include "usermanager_types.h"
#include "keystore.h"

#include <string>
#include <memory>
#include <vector>

#include <openssl/x509.h>

namespace user_manager
{

/**
 * \brief Foundation Services usermanagerinterface, provides the interface to access the
 *        services of user manager.
 */

class UsermanagerInterface
{

 public:

    /**
    * \brief Destroys the Keystore instance freeing any associated resources.
    */
    virtual ~UsermanagerInterface();

    /**
    * \brief creates base class instance.
    * \param[in] keystore Takes Keystore as a in parameter. Keystore need to be instantiated
    *                     before calling for usermanager.
    * \return instance of Usermanager class
    */
    static std::shared_ptr<UsermanagerInterface> Create(std::shared_ptr<Keystore> keystore = std::shared_ptr<Keystore>(nullptr));

    /**
    * \brief Get all users
    * \param[out] ids List of all stored users in DB. If the given vector is not empty, the result will append to it.
    * \return ReturnCode::kNotFound, if no user stored.
    *         ReturnCode::kError, on error
    *         ReturnCode::kSuccess, on success.
    */
    virtual ReturnCode GetUsers(std::vector<UserId>& ids) = 0;

    /**
    * \brief Get all users matching a given role
    * \param[out] ids  gets the list of users having specified role. If the given vector is not empty, the result
    *                  will append to it.
    * \param[in]  role User role could be
    *                  kAdmin, kUser, kDelegate etc.
    * \return ReturnCode::kNotFound, if not user found with specified role.
    *         ReturnCode::kError, on error
    *         ReturnCode::kUndefined, if given role doesn't exists.
    *         ReturnCode::kSuccess on success.
    */
    virtual ReturnCode GetUsers(std::vector<UserId>& ids, UserRole role) = 0;

    /**
    * \brief Find a user matching a certificate, can match either actor or short range tls
    * \param[out] id          Get a user id with given certificate.
    * \param[in]  certificate The certificate to return the user id for.
    * \return ReturnCode::kNotFound, if no user found
    *         ReturnCode::kError, on error
    *         ReturnCode::kInvalidArgument, if invalid certificate provided.
    *         ReturnCode::kSuccess on success.
    */
    virtual ReturnCode GetUser(UserId& id, const X509* certificate) = 0;

    /**
     * \brief Find user by Common Name.
     * \param[out] id          Id of user if found.
     * \param[in]  common_name common_name in the users actor certificate.
     * \return ReturnCode::kNotFound, if no user found
     *         ReturnCode::kError, on error
     *         ReturnCode::kInvalidArgument, if empty common_name provided.
     *         ReturnCode::kSuccess on success.
     */
    virtual ReturnCode GetUser(UserId& id, const std::vector<unsigned char>& common_name) = 0;

    /**
    * \brief Get user role.
    * \param[out] role UserRole for requested user.
    * \param[in]  id   UserId for requested role
    * \return ReturnCode::kNotFound, if not user found with specified role.
    *         ReturnCode::kError, on error
    *         ReturnCode::kUndefined, if given role doesn't exists.
    *         ReturnCode::kSuccess on success.
    */
    virtual ReturnCode GetUserRole(UserRole& role, const UserId id) = 0;

    /**
    * \brief Get the actor certificate for a given of user.
    * \param[out] certificate Certificate for the user, if found.
    * \param[in]  ids         List of users of the requested certificates
    * \return ReturnCode::kNotFound, if no certificate found
    *         ReturnCode::kError, on error
    *         ReturnCode::kSuccess on success.
    */
    virtual ReturnCode GetActorCertificate(X509** certificate, UserId id) = 0;

    /**
    * \brief Get the actor certificates for a list of users.
    *        This is a utility function not strictly necessary.
    * \param[out] certificates List of certificates of all given users.
    * \param[in]  ids          List of users of the requested certificates
    * \return ReturnCode::kNotFound, if no certificate found, or if a certificate was not found for one of the users,
    *                                in that case certificates will be empty
    *         ReturnCode::kError, on error
    *         ReturnCode::kSuccess, on success.
    */
    virtual ReturnCode GetActorCertificates(STACK_OF(X509)* certificates, const std::vector<UserId> ids) = 0;

    /**
    * \brief Get the short range tls client certificate for a user.
    * \param[out] certificate Short range TLS client certificate of a given user, if found.
    * \param[in]  id          UserId for requested short range TLS client certificate.
    * \return ReturnCode::kNotFound, if no short range tls certificate found for a given user.
    *         ReturnCode::kError, on error
    *         ReturnCode::kSuccess, on success.
    */
    virtual ReturnCode GetShortRangeTlsClientCertificate(X509** certificate, const UserId id) = 0;

    /**
    * \brief Get the short range tls client certificates for a list of users.
    *        This is a utility function not strictly necessary.
    * \param[out] certificates Short range TLS client certificates of list of given users.
    * \param[in]  ids          List of users for short range TLS client certificates.
    * \return ReturnCode::kNotFound, if no short range tls certificate found for given users, or if a certificate was not found
    *                                for one of the users, in that case certificates will be empty
    *         ReturnCode::kError, on error
    *         ReturnCode::kSuccess, on success.
    */
    virtual ReturnCode GetShortRangeTlsClientCertificates(STACK_OF(X509)* certificates, const std::vector<UserId> ids) = 0;

    /**
     * \brief Get the Common Name of a user.
     *
     * \param[out] common_name Common Name will be written to this vector on success.
     * \param[in]  id          User to look up Common Name for.
     *
     * \return ReturnCode::kNotFound, if user not found, ReturnCode::kError on error
     *          and ReturnCode::kSuccess on success.
     */
    virtual ReturnCode GetCommonName(std::vector<unsigned char>& common_name, const UserId id) = 0;

    /**
    * \brief Add a user to the list of known users
    * \param[out] id                            new userId
    * \param[in] role                           role associated with new user
    * \param[in] actor_cert                     actor certificate to be stored
    * \param[in] shortrange_cls_client_cert     if not nullprt, Short range TLS certificate
    *                                           to be store for new user
    * \return ReturnCode::kError
    *         ReturnCode::kSuccess, on success.
    */
    virtual ReturnCode AddUser(UserId& id,
                               UserRole role,
                               X509* actor_cert,
                               X509* shortrange_tls_client_cert = nullptr) = 0;

    /**
    * \brief Remove a existing user
    * \param[in] id A userId to be removed.
    * \return ReturnCode::kNotFound, if user doesn't exists.
    *         ReturnCode::kError, on error
    *         ReturnCode::kSuccess, on success.
    */
    virtual ReturnCode DeleteUser(const UserId id) = 0;

}; // class UsermanagerInterface

} // namespace user_manager
#endif // FSM_USERMANAGERINTERFACE_h

/** \}    end of addtogroup */
