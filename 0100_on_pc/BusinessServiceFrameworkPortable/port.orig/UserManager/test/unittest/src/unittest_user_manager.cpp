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
 *  \file     unittest_user_manager.cpp
 *  \brief    Foundation Services Connectivity User Manager, Test Cases
 *  \author   Iulian Sirghi
 *
 *  \addtogroup unittest
 *  \{
 */

/******************************************************************************
 * INCLUDE FILES
 ******************************************************************************/
#include "utilities.h"
#include "stubs/persistence/persistence_stub.h"
#include <gtest/gtest.h>

#include <usermanager_interface.h>

#include <tuple>
#include <algorithm>
#include <vector>
#include <map>
#include <memory>
#include <utility>

#include <cstdint>

/******************************************************************************
 * TEST Utilities
 ******************************************************************************/
/*
 * Clears the database of users.
 */
void DeleteDatabase();

/*
 * Split a full-set of users until two different sets, one that contains user ids with just
 * actor certs, and one with users that contain both actor cert and short-range tls client cert.
*/
void SplitUserCertificateSets(const std::map<fsm::UserId, std::tuple<fsm::UserRole, X509*, X509*>>& full_set,
                              std::vector<fsm::UserId>& users,
                              std::vector<fsm::UserId>& users_no_sr_cert);

/******************************************************************************
 * TESTS
 ******************************************************************************/

// TODO:Iulian:Prerequisite is that db is initially empty. We ought to delete the DB ahead of time.
// We could additionally introduce a fixture that deletes db before and after.

/* Check if factory method returns non-null pointer to interface */
TEST(UserManagerInterface, create)
{
    std::shared_ptr<fsm::UsermanagerInterface> um_if = fsm::UsermanagerInterface::Create();

    ASSERT_NE(um_if, nullptr);
}

/* Check Db initially empty */
TEST(UserManagerInterface, InitialEmptyDb)
{
    DeleteDatabase();

    std::shared_ptr<fsm::UsermanagerInterface> um_if = fsm::UsermanagerInterface::Create();
    ASSERT_NE(um_if, nullptr);

    // The entire set of user ids should be empty, as we had no prior operation to populate the db.
    {
        std::vector<fsm::UserId> users;
        fsm::ReturnCode rc = um_if->GetUsers(users);

       EXPECT_EQ(rc, fsm::ReturnCode::kNotFound);
       EXPECT_EQ(users.size(), 0);
    }

    // For each valid role, the set of user ids should be empty as no prior operation to populate the db was done.
    {
        std::uint32_t valid_role_index = static_cast<std::uint32_t>(fsm::UserRole::kUndefined);

        for (; valid_role_index < static_cast<std::uint32_t>(fsm::UserRole::kEndMarker); ++valid_role_index)
        {
            std::vector<fsm::UserId> users_for_role;
            fsm::UserRole user_role = static_cast<fsm::UserRole>(valid_role_index);

            fsm::ReturnCode rc = um_if->GetUsers(users_for_role, user_role);

            if (user_role != fsm::UserRole::kUndefined)
            {
                EXPECT_EQ(rc, fsm::ReturnCode::kNotFound);
            }
            else
            {
                EXPECT_EQ(rc, fsm::ReturnCode::kInvalidArgument);
            }

            EXPECT_EQ(users_for_role.size(), 0);
        }
    }

    // Blindly generate a set of certificates and attempt to retrieve a UserId. All operations should fail as Db is
    // empty.
    {
        const std::uint32_t kCertificateCount = 10;

        for (std::uint32_t cert_index = 0; cert_index < kCertificateCount; ++cert_index)
        {
            EVP_PKEY* key = nullptr;
            X509* cert = nullptr;
            fsm::UserId user_id = fsm::kUndefinedUserId;

            GeneratePrivateKey(&key, fsm::kMinRSAKeySize);
            GenerateCertificate(&cert, key);

            fsm::ReturnCode rc = um_if->GetUser(user_id, cert);

            EXPECT_EQ(rc, fsm::ReturnCode::kNotFound);
            EXPECT_EQ(user_id, fsm::kUndefinedUserId);

            EVP_PKEY_free(key);
            X509_free(cert);
        }
    }

    const std::uint32_t kUserCount = 65535;

    // Choose a set of user-id's and attempt to retrieve the role. All attempts should fail as no users are present.
    {
        fsm::UserRole user_role = fsm::UserRole::kUndefined;

        for (std::uint32_t user_index = 0; user_index < kUserCount; ++user_index)
        {
            fsm::ReturnCode rc = um_if->GetUserRole(user_role, static_cast<fsm::UserId>(user_index));

            EXPECT_EQ(rc, fsm::ReturnCode::kNotFound);
            EXPECT_EQ(user_role, fsm::UserRole::kUndefined);
        }
    }

    // Attempt to retrieve the actor certificate for kUserCount id's. All attempts should fail as no users are present.
    {
        X509* cert = nullptr;

        for (std::uint32_t user_index = 0; user_index < kUserCount; ++user_index)
        {
            fsm::ReturnCode rc = um_if->GetActorCertificate(&cert, static_cast<fsm::UserId>(user_index));

            EXPECT_EQ(rc, fsm::ReturnCode::kNotFound);
            EXPECT_EQ(cert, nullptr);
        }
    }

    // Try to get actor certificates for a set of users. At each iteration we halve the user count until it reaches
    // zero.
    {
        std::uint32_t kHighUserIdBound = kUserCount;

        while (kHighUserIdBound)
        {
            std::vector<fsm::UserId> users;

            // populate the user-id vector.
            for (std::uint32_t user_index = 0; user_index < kHighUserIdBound; ++user_index)
            {
                users.push_back(static_cast<fsm::UserId>(user_index));
            }

            STACK_OF(X509*) cert_stack = sk_X509_new_null();

            fsm::ReturnCode rc = um_if->GetActorCertificates(cert_stack, users);

            EXPECT_EQ(rc, fsm::ReturnCode::kNotFound);
            EXPECT_EQ(sk_X509_num(cert_stack), 0); // expect no certs reported.

            sk_X509_pop_free(cert_stack, X509_free);

            kHighUserIdBound /= 2;
        }
    }

    // Try to retrieve Short-Range TLS Client certificate for each user in [0, kUserCount) interval. As no users were
    // added, we expect failure.
    {
        for (std::uint32_t user_index = 0; user_index < kUserCount; ++user_index)
        {
            X509* cert = nullptr;

            fsm::ReturnCode rc = um_if->GetShortRangeTlsClientCertificate(&cert, static_cast<fsm::UserId>(user_index));

            EXPECT_EQ(rc, fsm::ReturnCode::kNotFound);
            EXPECT_EQ(cert, nullptr);
        }
    }

    // Try to get Short-Range TLS Client certificates for a set of users.
    // With each iteration we halve the user count until it reaches zero.
    {
        std::uint32_t kHighUserIdBound = kUserCount;

        while (kHighUserIdBound)
        {
            std::vector<fsm::UserId> users;

            // populate the user-id vector.
            for (std::uint32_t user_index = 0; user_index < kUserCount; ++user_index)
            {
                users.push_back(static_cast<fsm::UserId>(user_index));
            }

            STACK_OF(X509*) cert_stack = sk_X509_new_null();

            fsm::ReturnCode rc = um_if->GetShortRangeTlsClientCertificates(cert_stack, users);

            EXPECT_EQ(rc, fsm::ReturnCode::kNotFound);
            EXPECT_EQ(sk_X509_num(cert_stack), 0); // expect no certs reported.

            sk_X509_pop_free(cert_stack, X509_free);

            kHighUserIdBound /= 2;
        }
    }

    // Attempt to delete users that do not exist. Of course, all operations are expected to fail.
    {
        for (std::uint32_t user_index = 0; user_index < kUserCount; ++user_index)
        {
            fsm::ReturnCode rc = um_if->DeleteUser(static_cast<fsm::UserId>(user_index));

            EXPECT_EQ(rc, fsm::ReturnCode::kNotFound);
        }
    }
}

static std::map<fsm::UserId, std::pair<fsm::UserRole, X509*>> added_users;
const std::uint32_t kUsersPerRole = 10;

/*
 * Test case will add users on a per-role basis. The in-memory structure 'added_users' is the representation of what
 * should already be in the database. Retrieval attempts should check against this 'in-memory' representation.
 * For this test-case, we will only check that all for all known certificates, we get a valid user id reported.
 * Following test-cases will perform more complex operations.
 */
TEST(UserManagerInterface, AddUsers)
{
    // Pre-condition, we delete database file, if any.
    DeleteDatabase();

    // Add kUsersPerRoleCount users on a per-role basis and check operation success. Also populate the added_users
    // in-memory structure so that we can retrieve from UserManager and check against it.
    {
        std::shared_ptr<fsm::UsermanagerInterface> um_if = fsm::UsermanagerInterface::Create();
        ASSERT_NE(um_if, nullptr);

        // for each role, add kUsersPerRole users.
        std::uint32_t user_role_index = static_cast<std::uint32_t>(fsm::UserRole::kUndefined) + 1;
        const std::uint32_t kUserRoleMaxIndex = static_cast<std::uint32_t>(fsm::UserRole::kEndMarker);

        for (; user_role_index < kUserRoleMaxIndex; ++user_role_index)
        {
            const fsm::UserRole user_role = static_cast<fsm::UserRole>(user_role_index);

            for (std::uint32_t user_id_index = 0; user_id_index < kUsersPerRole; ++user_id_index)
            {
                fsm::UserId user_id = fsm::kUndefinedUserId;

                EVP_PKEY* key = nullptr;
                X509* cert = nullptr;

                GeneratePrivateKey(&key, fsm::kMinRSAKeySize);
                GenerateCertificate(&cert, key);

                fsm::ReturnCode rc = um_if->AddUser(user_id, user_role, cert);

                EXPECT_EQ(rc, fsm::ReturnCode::kSuccess);
                EXPECT_NE(user_id, fsm::kUndefinedUserId);

                added_users[user_id] = std::make_pair(user_role, cert);

                EVP_PKEY_free(key);
            }
        }

        // Just retrieve each user by it's associated cert. We don't really wish to check anything else. All tests
        // on retrieval will be performed on a new instance of UserManager in order to check persistency.
        // TODO:Iulian:Also check that a user-id can be returned for a short-range TLS Client cert.
        for (auto id_role_cert_pair : added_users)
        {
            X509* cert = id_role_cert_pair.second.second;

            fsm::UserId user_id = fsm::kUndefinedUserId;

            fsm::ReturnCode rc = um_if->GetUser(user_id, cert);

            EXPECT_EQ(rc, fsm::ReturnCode::kSuccess);
            EXPECT_EQ(user_id, id_role_cert_pair.first);
        }
    }
}


/* Get all user id's and make sure all of them are the ones we originally added */
TEST(UserManagerInterface, GetUsers)
{
    std::shared_ptr<fsm::UsermanagerInterface> um_if = fsm::UsermanagerInterface::Create();
    ASSERT_NE(um_if, nullptr);

    std::vector<fsm::UserId> user_ids;

    fsm::ReturnCode rc = um_if->GetUsers(user_ids);

    EXPECT_EQ(rc, fsm::ReturnCode::kSuccess);
    EXPECT_EQ(added_users.size(), user_ids.size());

    // all user ids must be found in added_users.
    for (auto id_role_cert_pair : added_users)
    {
        std::vector<fsm::UserId>::iterator it = std::find(user_ids.begin(), user_ids.end(), id_role_cert_pair.first);

        EXPECT_NE(it, user_ids.end());
    }
}

/* Get all UserId's on a per-role basis and match the in-memory UserRoles with the reported ones */
TEST(UserManagerInterface, GetUsersPerRole)
{
    std::shared_ptr<fsm::UsermanagerInterface> um_if = fsm::UsermanagerInterface::Create();
    ASSERT_NE(um_if, nullptr);

    // for each role, get all users.
    std::uint32_t user_role_index = static_cast<std::uint32_t>(fsm::UserRole::kUndefined) + 1;
    const std::uint32_t kUserRoleMaxIndex = static_cast<std::uint32_t>(fsm::UserRole::kEndMarker);

    for (; user_role_index < kUserRoleMaxIndex; ++user_role_index)
    {
        std::vector<fsm::UserId> role_user_ids;

        const fsm::UserRole user_role = static_cast<fsm::UserRole>(user_role_index);
        fsm::ReturnCode rc = um_if->GetUsers(role_user_ids, user_role);

        EXPECT_EQ(rc, fsm::ReturnCode::kSuccess);
        EXPECT_EQ(role_user_ids.size(), kUsersPerRole);


        // Match the role
        for (auto role_user_id : role_user_ids)
        {
            const fsm::UserRole memory_user_role = added_users[role_user_id].first;

            EXPECT_EQ(memory_user_role, user_role);
        }
    }
}

/* For all known user-ids, check that the reported role is the one we added */
TEST(UserManagerInterface, GetUserRole)
{
    std::shared_ptr<fsm::UsermanagerInterface> um_if = fsm::UsermanagerInterface::Create();
    ASSERT_NE(um_if, nullptr);

    // for all in-memory user-ids, retreive the role.
    for (auto id_role_cert_pair : added_users)
    {
        fsm::UserRole reported_user_role = fsm::UserRole::kUndefined;
        fsm::UserRole expected_user_role = id_role_cert_pair.second.first;
        fsm::UserId user_id = id_role_cert_pair.first;

        fsm::ReturnCode rc = um_if->GetUserRole(reported_user_role, user_id);

        EXPECT_EQ(rc, fsm::ReturnCode::kSuccess);

        EXPECT_EQ(reported_user_role, expected_user_role);
    }
}

/* Retrieve all known added users by their certificate while exercising persistency. */
TEST(UserManagerInterface, GetUserByCert)
{
    std::shared_ptr<fsm::UsermanagerInterface> um_if = fsm::UsermanagerInterface::Create();
    ASSERT_NE(um_if, nullptr);

    for (auto id_role_cert_pair : added_users)
    {
        X509* cert = id_role_cert_pair.second.second;

        fsm::UserId user_id = fsm::kUndefinedUserId;

        fsm::ReturnCode rc = um_if->GetUser(user_id, cert);

        EXPECT_EQ(rc, fsm::ReturnCode::kSuccess);
        EXPECT_EQ(user_id, id_role_cert_pair.first);
    }
}

/* Get all users by common name */
TEST(UserManagerInterface, GetUserByCN)
{
    std::shared_ptr<fsm::UsermanagerInterface> um_if = fsm::UsermanagerInterface::Create();
    ASSERT_NE(um_if, nullptr);

    for (auto id_role_cert_pair : added_users)
    {
        fsm::UserId id;
        std::vector<unsigned char> cn;

        GetCommonName(cn, id_role_cert_pair.second.second);

        fsm::ReturnCode rc = um_if->GetUser(id, cn);

        EXPECT_EQ(rc, fsm::ReturnCode::kSuccess);
        EXPECT_EQ(id, id_role_cert_pair.first);
    }
}

/* Get common name for all users */
TEST(UserManagerInterface, GetCN)
{
    std::shared_ptr<fsm::UsermanagerInterface> um_if = fsm::UsermanagerInterface::Create();
    ASSERT_NE(um_if, nullptr);

    for (auto id_role_cert_pair : added_users)
    {
        fsm::UserId id;
        std::vector<unsigned char> local_cn;
        std::vector<unsigned char> um_cn;

        GetCommonName(local_cn, id_role_cert_pair.second.second);

        fsm::ReturnCode rc = um_if->GetCommonName(um_cn, id_role_cert_pair.first);

        EXPECT_EQ(rc, fsm::ReturnCode::kSuccess);

        // WORKAROUND
        // BLEManager expects commonnames to be 16 byte UUIDS, however, the Certificates
        // currently available to the mobile application are not UUIDS.
        // To handle this we pad all stored common names with 0, or cap if needed, to kEnforcedCommonNameSize bytes.
        // This obviously breaks this test
        if (local_cn.size() >= 16)
        {
            EXPECT_EQ(local_cn, um_cn);
        }
        else
        {
            EXPECT_NE(local_cn, um_cn);
        }
    }
}

/* Test that CN smaller than 16 works as expected */
TEST(UserManagerInterface, CommonNameWorkaroundTest1)
{
    std::shared_ptr<fsm::UsermanagerInterface> um_if = fsm::UsermanagerInterface::Create();
    ASSERT_NE(um_if, nullptr);

    fsm::UserId user_id = fsm::kUndefinedUserId;
    std::string user_common_name = "short";

    EVP_PKEY* key = nullptr;
    X509* cert = nullptr;

    GeneratePrivateKey(&key, fsm::kMinRSAKeySize);
    GenerateCertificate(&cert, key, user_common_name);

    fsm::ReturnCode rc = um_if->AddUser(user_id, fsm::UserRole::kAdmin, cert);

    EXPECT_EQ(rc, fsm::ReturnCode::kSuccess);
    EXPECT_NE(user_id, fsm::kUndefinedUserId);

    EVP_PKEY_free(key);


    // Check that user can be retrieved by common name
    fsm::UserId id = fsm::kUndefinedUserId;
    std::vector<unsigned char> cn;

    GetCommonName(cn, cert);

    rc = um_if->GetUser(id, cn);

    EXPECT_EQ(rc, fsm::ReturnCode::kSuccess);
    EXPECT_EQ(id, user_id);


    // Check that user can be retrieved with padded common name
    cn.resize(16, 0);

    id = fsm::kUndefinedUserId;

    rc = um_if->GetUser(id, cn);

    EXPECT_EQ(rc, fsm::ReturnCode::kSuccess);
    EXPECT_EQ(id, user_id);


    // Check that padded common name matches the common name returned for user
    std::vector<unsigned char> um_cn;

    rc = um_if->GetCommonName(um_cn, user_id);

    EXPECT_EQ(rc, fsm::ReturnCode::kSuccess);

    EXPECT_EQ(cn, um_cn);

    rc = um_if->DeleteUser(user_id);
    ASSERT_EQ(rc, fsm::ReturnCode::kSuccess);

    X509_free(cert);
}

/* Test that CN larger than 16 works as expected */
TEST(UserManagerInterface, CommonNameWorkaroundTest2)
{
    std::shared_ptr<fsm::UsermanagerInterface> um_if = fsm::UsermanagerInterface::Create();
    ASSERT_NE(um_if, nullptr);

    fsm::UserId user_id = fsm::kUndefinedUserId;
    std::string user_common_name = "yournameisnotwhstyouthinkitisyournameiswhatisayitis";

    EVP_PKEY* key = nullptr;
    X509* cert = nullptr;

    GeneratePrivateKey(&key, fsm::kMinRSAKeySize);
    GenerateCertificate(&cert, key, user_common_name);

    fsm::ReturnCode rc = um_if->AddUser(user_id, fsm::UserRole::kAdmin, cert);

    EXPECT_EQ(rc, fsm::ReturnCode::kSuccess);
    EXPECT_NE(user_id, fsm::kUndefinedUserId);

    EVP_PKEY_free(key);


    // Check that user can be retrieved by common name
    fsm::UserId id = fsm::kUndefinedUserId;
    std::vector<unsigned char> cn;

    GetCommonName(cn, cert);

    rc = um_if->GetUser(id, cn);

    EXPECT_EQ(rc, fsm::ReturnCode::kSuccess);
    EXPECT_EQ(id, user_id);


    // Check that user can be retrieved with capped common name
    cn.resize(16);

    id = fsm::kUndefinedUserId;

    rc = um_if->GetUser(id, cn);

    EXPECT_EQ(rc, fsm::ReturnCode::kSuccess);
    EXPECT_EQ(id, user_id);


    // Check that capped common name matches the common name returned for user
    std::vector<unsigned char> um_cn;

    rc = um_if->GetCommonName(um_cn, user_id);

    EXPECT_EQ(rc, fsm::ReturnCode::kSuccess);

    EXPECT_EQ(cn, um_cn);

    rc = um_if->DeleteUser(user_id);
    ASSERT_EQ(rc, fsm::ReturnCode::kSuccess);

    X509_free(cert);
}

/* Test that CN of size 16 works as expected */
TEST(UserManagerInterface, CommonNameWorkaroundTest3)
{
    std::shared_ptr<fsm::UsermanagerInterface> um_if = fsm::UsermanagerInterface::Create();
    ASSERT_NE(um_if, nullptr);

    fsm::UserId user_id = fsm::kUndefinedUserId;
    std::string user_common_name = "1234123412341234";

    EVP_PKEY* key = nullptr;
    X509* cert = nullptr;

    GeneratePrivateKey(&key, fsm::kMinRSAKeySize);
    GenerateCertificate(&cert, key, user_common_name);

    fsm::ReturnCode rc = um_if->AddUser(user_id, fsm::UserRole::kAdmin, cert);

    EXPECT_EQ(rc, fsm::ReturnCode::kSuccess);
    EXPECT_NE(user_id, fsm::kUndefinedUserId);

    EVP_PKEY_free(key);


    // Check that user can be retrieved by common name
    fsm::UserId id = fsm::kUndefinedUserId;
    std::vector<unsigned char> cn;

    GetCommonName(cn, cert);

    rc = um_if->GetUser(id, cn);

    EXPECT_EQ(rc, fsm::ReturnCode::kSuccess);
    EXPECT_EQ(id, user_id);

    // Check that common name matches the common name returned for user
    std::vector<unsigned char> um_cn;

    rc = um_if->GetCommonName(um_cn, user_id);

    EXPECT_EQ(rc, fsm::ReturnCode::kSuccess);

    EXPECT_EQ(cn, um_cn);

    rc = um_if->DeleteUser(user_id);
    ASSERT_EQ(rc, fsm::ReturnCode::kSuccess);

    X509_free(cert);
}

/* Retrieve the actor certificate on a per-user basis and check against the in-memory structure */
TEST(UserManagerInterface, GetActorCertificate)
{
    std::shared_ptr<fsm::UsermanagerInterface> um_if = fsm::UsermanagerInterface::Create();
    ASSERT_NE(um_if, nullptr);

    for (auto id_role_cert_pair : added_users)
    {
        X509* db_cert = nullptr;
        X509* mem_cert = id_role_cert_pair.second.second;

        fsm::ReturnCode rc = um_if->GetActorCertificate(&db_cert, id_role_cert_pair.first);

        ASSERT_NE(db_cert, nullptr);


        EXPECT_EQ(rc, fsm::ReturnCode::kSuccess);
        EXPECT_EQ(X509_cmp(db_cert, mem_cert), 0); // The certificates must match.

        X509_free(db_cert);
    }
}

/* Retrieve the actor certificates for all users in one go and check against the in-memory structure */
TEST(UserManagerInterface, GetActorCertificates)
{
    std::shared_ptr<fsm::UsermanagerInterface> um_if = fsm::UsermanagerInterface::Create();
    ASSERT_NE(um_if, nullptr);

    std::vector<fsm::UserId> all_user_ids;

    fsm::ReturnCode rc = um_if->GetUsers(all_user_ids);

    EXPECT_EQ(rc, fsm::ReturnCode::kSuccess);
    EXPECT_EQ(all_user_ids.size(), added_users.size());

    // match all reported user ids against the in-memory structure.
    for (auto id_role_cert_pair : added_users)
    {
        const fsm::UserId mem_user_id = id_role_cert_pair.first;
        std::vector<fsm::UserId>::iterator it = std::find(all_user_ids.begin(), all_user_ids.end(), mem_user_id);

        EXPECT_NE(it, all_user_ids.end());
    }

    // Get the stack of all certificates for all users.
    // We expect that the first user id in 'ids' has the top-most certificate associated in the stack.*

    STACK_OF(X509)* cert_stack = sk_X509_new_null(); // I assume that the responsability for the lifecycle of the stack
                                                     // and it's elements belong to the caller.

    rc = um_if->GetActorCertificates(cert_stack, all_user_ids);

    EXPECT_EQ(rc, fsm::ReturnCode::kSuccess);
    EXPECT_EQ(all_user_ids.size(), sk_X509_num(cert_stack));

    std::vector<fsm::UserId>::reverse_iterator it = all_user_ids.rbegin();

    for (; it != all_user_ids.rend(); ++it)
    {
        X509* db_cert = sk_X509_pop(cert_stack);
        X509* mem_cert = added_users[(*it)].second;

        // Match the certificates.
        EXPECT_EQ(X509_cmp(db_cert, mem_cert), 0);

        X509_free(db_cert);
    }

    sk_X509_free(cert_stack);
}

/* For each user, attempt to get the TLS short-range certificate which was never set. */
TEST(UserManagerInterface, GetShortRangeTlsCertificateWithUnsetTlsClientCert)
{
    std::shared_ptr<fsm::UsermanagerInterface> um_if = fsm::UsermanagerInterface::Create();
    ASSERT_NE(um_if, nullptr);

    std::vector<fsm::UserId> user_ids;

    fsm::ReturnCode rc = um_if->GetUsers(user_ids);
    EXPECT_EQ(rc, fsm::ReturnCode::kSuccess);

    for (auto user_id : user_ids)
    {
        X509* tls_cert = nullptr;

        rc = um_if->GetShortRangeTlsClientCertificate(&tls_cert, user_id);

        EXPECT_NE(rc, fsm::ReturnCode::kSuccess);
        EXPECT_EQ(tls_cert, nullptr);
   }
}

/*
 * Try to retrieve all short-range TLS certificates in one go, for all users.
 * Operation should fail as no users has TLS certificate.
*/
TEST(UserManagerInterface, GetShortRangeTlsCertificatesWithUnsetTlsCLientCert)
{
    std::shared_ptr<fsm::UsermanagerInterface> um_if = fsm::UsermanagerInterface::Create();
    ASSERT_NE(um_if, nullptr);

    STACK_OF(X509)* cert_store = sk_X509_new_null();
    std::vector<fsm::UserId> user_ids;

    fsm::ReturnCode rc = um_if->GetUsers(user_ids);
    EXPECT_EQ(rc, fsm::ReturnCode::kSuccess);

    rc = um_if->GetShortRangeTlsClientCertificates(cert_store, user_ids);

    EXPECT_NE(rc, fsm::ReturnCode::kSuccess);
    EXPECT_EQ(sk_X509_num(cert_store), 0);

    sk_X509_pop_free(cert_store, X509_free);
}

/* Delete all known added users and check data intergrity and that the delete operation is persistent */
TEST(UserManagerInterface, DeleteUser)
{
    // Perform the delete operation on all known users and check that the user is not reported anymore
    // When we attempt to interogate for it's certificate.
    {
        std::shared_ptr<fsm::UsermanagerInterface> um_if = fsm::UsermanagerInterface::Create();
        ASSERT_NE(um_if, nullptr);

        std::map<fsm::UserId, std::pair<fsm::UserRole, X509*>> remaining_users(added_users);

        // For all remaining users, check their roles and certificates.
        // After we perform the check, remove a user and re-iterate until no users remain.
        while (remaining_users.size())
        {
            std::vector<fsm::UserId> all_user_ids;

            fsm::ReturnCode rc = um_if->GetUsers(all_user_ids);

            EXPECT_EQ(rc, fsm::ReturnCode::kSuccess);
            EXPECT_EQ(all_user_ids.size(), remaining_users.size());

            // Check the data integrity of all users.
            for (auto id_role_cert_pair : remaining_users)
            {
                const fsm::UserId user_id = id_role_cert_pair.first;
                const fsm::UserRole mem_user_role = id_role_cert_pair.second.first;
                X509* mem_cert = id_role_cert_pair.second.second;

                // Check that the in-memory user-id actually exists in the reported vector of user ids
                std::vector<fsm::UserId>::iterator it_found = std::find(all_user_ids.begin(),
                                                                        all_user_ids.end(),
                                                                        user_id);
                EXPECT_NE(it_found, all_user_ids.end());

                fsm::UserRole db_user_role = fsm::UserRole::kUndefined;
                X509* db_cert = nullptr;

                // Check match on UserRole.
                rc = um_if->GetUserRole(db_user_role, user_id);

                EXPECT_EQ(rc, fsm::ReturnCode::kSuccess);
                EXPECT_EQ(db_user_role, mem_user_role);

                // Check match on certificate.
                rc = um_if->GetActorCertificate(&db_cert, user_id);

                ASSERT_NE(db_cert, nullptr);

                EXPECT_EQ(rc, fsm::ReturnCode::kSuccess);
                EXPECT_EQ(X509_cmp(mem_cert, db_cert), 0);

                X509_free(db_cert);
            }

            // Remove one user and it's associated data.
            rc = um_if->DeleteUser(remaining_users.begin()->first);
            EXPECT_EQ(rc, fsm::ReturnCode::kSuccess);
            remaining_users.erase(remaining_users.begin());
        }
    }

    // Perform another iteration and check that no data is reported for the previously-deleted user ids.
    // On test success, it would mean that the deletion operation was persistent.
    {
        std::shared_ptr<fsm::UsermanagerInterface> um_if = fsm::UsermanagerInterface::Create();
        ASSERT_NE(um_if, nullptr);

        // Check no user ids are reported
        std::vector<fsm::UserId> all_user_ids;
        fsm::ReturnCode rc = um_if->GetUsers(all_user_ids);

        EXPECT_EQ(rc, fsm::ReturnCode::kNotFound);
        EXPECT_EQ(all_user_ids.size(), 0);

        // We still have the old set of users that we know we added.
        // Attempting to retrieve data for the previously-deleted users should fail.
        for (auto id_role_cert_pair : added_users)
        {
            fsm::UserId mem_user_id = id_role_cert_pair.first;
            X509* mem_cert = id_role_cert_pair.second.second;
            X509* db_cert = nullptr;
            fsm::UserId db_user_id = fsm::kUndefinedUserId;
            fsm::UserRole db_user_role = fsm::UserRole::kUndefined;


            // try to retrieve a user_id via it's old certificate.
            fsm::ReturnCode rc = um_if->GetUser(db_user_id, mem_cert);
            // Of course, operation should not return success as the user was deleted.
            EXPECT_NE(rc, fsm::ReturnCode::kSuccess);
            // Output param is to remain unchanged.
            EXPECT_EQ(db_user_id, fsm::kUndefinedUserId);


            // Try to retrieve an actor certificate for the old user-id.
            rc = um_if->GetActorCertificate(&db_cert, mem_user_id);
            // Operation is to fail as the user was previously deleted.
            EXPECT_NE(rc, fsm::ReturnCode::kSuccess);
            // Output param is to remain unchanged.
            EXPECT_EQ(db_cert, nullptr);


            // Try to retrieve the user role for the old user-id.
            rc = um_if->GetUserRole(db_user_role, mem_user_id);
            // Operation is to fail as the user was previously deleted.
            EXPECT_NE(rc, fsm::ReturnCode::kSuccess);
            // Output param is to remain unchanged.
            EXPECT_EQ(db_user_role, fsm::UserRole::kUndefined);
        }


        // Check no actor certificates are reported for the old user ids.
        std::vector<fsm::UserId> old_user_ids;

        // populate the 'old_user_ids' vector.
        for (auto id_role_cert_pair : added_users)
        {
            old_user_ids.push_back(id_role_cert_pair.first);
        }

        STACK_OF(X509*) cert_stack = sk_X509_new_null();

        rc = um_if->GetActorCertificates(cert_stack, old_user_ids);
        EXPECT_NE(rc, fsm::ReturnCode::kSuccess); // operation should not succeed as the Db has no users as they got deleted.
        EXPECT_EQ(sk_X509_num(cert_stack), 0); // no certificates should be reported as Db is empty.


        sk_X509_free(cert_stack);
    }

    // Let us free the added_users in-memory structure. Certs must be freed.
    for (auto id_role_cert_pair : added_users)
    {
        X509_free(id_role_cert_pair.second.second);
    }

    added_users.clear();
}

std::map<fsm::UserId, std::tuple<fsm::UserRole, X509*, X509*>> added_users_tls;

/*
 * Populate a set of users in which the existence of a Tls Client Certificate is altered.
 * This will allow us to form test-cases that are a bit more complex, regarding the TLS Client Certificates.
 * We just check the add operations, all other properties will be checked on a new instance in order to guarantee
 * persistency.
 */
TEST(UserManagerInterface, AddUsersWithTlsClientCert)
{
    // Pre-condition, we delete database file, if any.
    DeleteDatabase();

    // Iteration is similar to the 'AddUsers' test case, only that we alternate on the existence of the
    // client
    {
        std::shared_ptr<fsm::UsermanagerInterface> um_if = fsm::UsermanagerInterface::Create();
        ASSERT_NE(um_if, nullptr);

        // for each role, add kUsersPerRole users.
        std::uint32_t user_role_index = static_cast<std::uint32_t>(fsm::UserRole::kUndefined) + 1;
        const std::uint32_t kUserRoleMaxIndex = static_cast<std::uint32_t>(fsm::UserRole::kEndMarker);

        bool user_has_tls = false;

        for (; user_role_index < kUserRoleMaxIndex; ++user_role_index)
        {
            const fsm::UserRole user_role = static_cast<fsm::UserRole>(user_role_index);

            for (std::uint32_t user_id_index = 0; user_id_index < kUsersPerRole; ++user_id_index)
            {
                fsm::UserId user_id = fsm::kUndefinedUserId;

                EVP_PKEY* actor_key = nullptr;
                EVP_PKEY* client_key = nullptr;

                X509* actor_cert = nullptr;
                X509* client_cert = nullptr;

                GeneratePrivateKey(&actor_key, fsm::kMinRSAKeySize);
                GenerateCertificate(&actor_cert, actor_key);

                EVP_PKEY_free(actor_key);
                actor_key = nullptr;


                if (user_has_tls)
                {
                    GeneratePrivateKey(&client_key, fsm::kMinRSAKeySize);
                    GenerateCertificate(&client_cert, client_key);

                    EVP_PKEY_free(client_key);
                }

                fsm::ReturnCode rc = um_if->AddUser(user_id, user_role, actor_cert, client_cert);
                EXPECT_EQ(rc, fsm::ReturnCode::kSuccess);

                added_users_tls[user_id] = std::make_tuple(user_role, actor_cert, client_cert);

                user_has_tls = !user_has_tls;
            }
        }
    }
}

/*
 * Check if the fact the half of the users have a short-range TLS client certificate set affects the number of users
 * reported by 'GetUsers'.
 */
TEST(UserManagerInterface, GetUsersWhileTlsClientCertIsSet)
{
    std::shared_ptr<fsm::UsermanagerInterface> um_if = fsm::UsermanagerInterface::Create();
    ASSERT_NE(um_if, nullptr);

    std::vector<fsm::UserId> user_ids;

    fsm::ReturnCode rc = um_if->GetUsers(user_ids);
    EXPECT_EQ(rc, fsm::ReturnCode::kSuccess);

    // Of course, the number of user ids reported must match the number of users we know we've added.
    EXPECT_EQ(user_ids.size(), added_users_tls.size());
}

/*
 * Check that the Short-Range Tls Client Certificate is correctly reported for all known users.
 * From all known users, we will extract two sets, one with users that do not have client certificates and one with
 * users that do. We will iterate over these sets and check each individual user.
 */
TEST(UserManagerInterface, GetShortRangeTlsClientCertificate)
{
    std::shared_ptr<fsm::UsermanagerInterface> um_if = fsm::UsermanagerInterface::Create();
    ASSERT_NE(um_if, nullptr);

    std::vector<fsm::UserId> users; // users with both actor and short-range TLS client cert.
    std::vector<fsm::UserId> users_no_sr_cert; // users with actor-cert only.

    // Populate the two separate sets.
    SplitUserCertificateSets(added_users_tls, users, users_no_sr_cert);

    // Check the set of users that don't have any short-range client certificate.
    // Operation should fail, of course - no cert to report as one was never added.
    for (auto user_id : users_no_sr_cert)
    {
        X509* client_cert = nullptr;
        fsm::ReturnCode rc = um_if->GetShortRangeTlsClientCertificate(&client_cert, user_id);

        EXPECT_NE(rc, fsm::ReturnCode::kSuccess);
        EXPECT_EQ(client_cert, nullptr);
    }

    // Check the set of users that do have a short-range client certificate.
    // We expect operation to succeed and the actual certificates to match!
    for (auto user_id : users)
    {
        X509* db_client_cert = nullptr;
        X509* mem_client_cert = nullptr;

        mem_client_cert = std::get<2>(added_users_tls[user_id]);
        fsm::ReturnCode rc = um_if->GetShortRangeTlsClientCertificate(&db_client_cert, user_id);

        ASSERT_EQ(rc, fsm::ReturnCode::kSuccess);
        EXPECT_EQ(X509_cmp(mem_client_cert, db_client_cert), 0);

        X509_free(db_client_cert);
    }
}

/*
 * Check certificate stack when attempting to retrieve the short-range certificate for both users that have it set
 * and users that don't - conceptually same as 'GetShortRangeTlsClientCertificate', but use the utility functions.
 */
TEST(UserManagerInterface, GetShortRangeTlsClientCertificates)
{
    std::shared_ptr<fsm::UsermanagerInterface> um_if = fsm::UsermanagerInterface::Create();
    ASSERT_NE(um_if, nullptr);

    std::vector<fsm::UserId> users; // users with both actor and short-range TLS client cert.
    std::vector<fsm::UserId> users_no_sr_cert; // users with actor-cert only.

    // Populate the two separate sets.
    SplitUserCertificateSets(added_users_tls, users, users_no_sr_cert);

    STACK_OF(X509)* users_no_sr_cert_stack = sk_X509_new_null();
    STACK_OF(X509)* users_cert_stack = sk_X509_new_null();

    // Attempt to retrieve the stack of certs for users that do not have short-range TLS client certs.
    // Operation should fail and the cert-stack should remain intact (no certs addedd).
    fsm::ReturnCode rc = um_if->GetShortRangeTlsClientCertificates(users_no_sr_cert_stack, users_no_sr_cert);

    EXPECT_NE(rc, fsm::ReturnCode::kSuccess);
    EXPECT_EQ(sk_X509_num(users_no_sr_cert_stack), 0);
    sk_X509_free(users_no_sr_cert_stack);

    // Following, we will retrieve the stack of certs for the users that do have a short-range TLS client set.
    // Stack should contain the certificates for all the user ids that were provided and operation should succeed.
    rc = um_if->GetShortRangeTlsClientCertificates(users_cert_stack, users);

    EXPECT_EQ(rc, fsm::ReturnCode::kSuccess);
    ASSERT_EQ(sk_X509_num(users_cert_stack), users.size()); // Certs should be reported for all users.

    // Now, conceptually, the last user-id in the vector should get the certificate on top of the stack associated.
    // In order to check that the certificates match, we do a reverse iteration.
    std::vector<fsm::UserId>::reverse_iterator it = users.rbegin();

    for (; it != users.rend(); ++it)
    {
        X509* db_client_cert = sk_X509_pop(users_cert_stack);
        X509* mem_client_cert = std::get<2>(added_users_tls[(*it)]);

        EXPECT_EQ(X509_cmp(db_client_cert, mem_client_cert), 0);
    }

    sk_X509_pop_free(users_cert_stack, X509_free);
}

void DeleteDatabase()
{
    tpPCL_stub_clean();
}


void SplitUserCertificateSets(const std::map<fsm::UserId, std::tuple<fsm::UserRole, X509*, X509*>>& full_set,
                              std::vector<fsm::UserId>& users,
                              std::vector<fsm::UserId>& users_no_sr_cert)
{
    // Populate the two separate sets.
    for (const auto& id_tuple : full_set)
    {
        X509* client_cert = std::get<2>(id_tuple.second);

        // Deduce which set the users belongs to.
        std::vector<fsm::UserId>& user_set = client_cert == nullptr ? users_no_sr_cert : users;

        user_set.push_back(id_tuple.first);
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
