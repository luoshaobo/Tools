/**
 * Copyright (C) 2017-2018 Continental Automotive GmbH
 *
 * Licensed under the "Supplier unrestricted white box" category, as
 * described in the 20160322-1_SW-amendment TCAM.pdf agreement
 * between Volvo Cars and Continental Automotive GmbH.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *  \file     unittest_keystore.cpp
 *  \brief    Foundation Services Connectivity Keystore, Test Cases
 *  \author   Iulian Sirghi
 *
 *  \addtogroup unittest
 *  \{
 */

/******************************************************************************
 * INCLUDE FILES
 ******************************************************************************/
#include "stubs/persistence/persistence_stub.h"
#include "utilities.h"

#include <gtest/gtest.h>
#include <keystore.h>

#include <future>
#include <tuple>
#include <thread>
#include <mutex>

/******************************************************************************
 * TESTS
 ******************************************************************************/

/*
 * Test Constructor with different Persistence API return codes.
 * Keystore is to perform proper error-handling on tpPCL error cases.
 */
TEST(Keystore, ConstructorWithPCLSuccess)
{
    // set the call-count to zero. We are planning to test if the Persistence API gets called at all.
    tpPCL_stub_setCallCount(0);
    tpPCL_stub_setErrorCode(E_PCL_ERROR_NONE);

    {
        gint dummy;
        fsm::Keystore keystore;

        // We expect tpPCL was actually called.
        ASSERT_NE(tpPCL_stub_getCallCount(), 0);

        // the keystore database should be available in the filesystem.
        char* database_folder;
        std::string database_path;

        // get the path the database was created in.
        tpPCL_getWriteablePath(0, &database_folder, &dummy);
        database_path.assign(database_folder);

        // append the internal database name.
        database_path.append("keystore.sqlite");

        ASSERT_TRUE(FileExists(database_path.c_str()));

        // Remove the database file.
        tpPCL_stub_clean();
    }
}

TEST(Keystore, ConstructorWithPCLError)
{
    // set the call-count to zero. We are planning to test if the Persistence API gets called at all.
    tpPCL_stub_setCallCount(0);
    // Test in tpPCL error-case scenario.
    tpPCL_stub_setErrorCode(E_PCL_ERROR_NO_MEM);

    {
        gint dummy;
        fsm::Keystore keystore;

        // We expect tpPCL was actually called.
        ASSERT_NE(tpPCL_stub_getCallCount(), 0);

        char* database_folder;
        std::string database_path;

        // get the path the database was created in.
        tpPCL_stub_setErrorCode(E_PCL_ERROR_NONE);
        tpPCL_getWriteablePath(0, &database_folder, &dummy);
        database_path.assign(database_folder);

        // append the internal database name.
        database_path.append("keystore.sqlite");

        // AS tpPCL failed, the database should not exist.
        ASSERT_FALSE(FileExists(database_path.c_str()));

        // Let us attempt to access the db, even if it should not be created.
        // error-code should be reported as db can't really be accessed.
        X509_STORE* cert_store = nullptr;

        fsm::ReturnCode rc;
        rc = keystore.GetCertificateAuthority(&cert_store, fsm::Keystore::CertificateRole::kVehicleCa);

        EXPECT_NE(rc, fsm::ReturnCode::kSuccess);
    }
}

/*
 * Ensure that a newly created database is empty.
 */
TEST(Keystore, InitialEmptyDb)
{
    tpPCL_stub_setErrorCode(E_PCL_ERROR_NONE);

    {
        gint dummy;
        const int kMaxIds = 10000;
        char* database_folder;
        std::string database_path;

        fsm::Keystore keystore;

        // get the path the database was created in.
        tpPCL_getWriteablePath(0, &database_folder, &dummy);
        database_path.assign(database_folder);

        // append the internal database name.
        database_path.append("keystore.sqlite");

        // Make sure the database exists.
        ASSERT_TRUE(FileExists(database_path.c_str()));


        // iterate to kMaxIds and check that no actual certificates are returned,
        // taking into account that the database is empty!
        for (int cert_id = 0; cert_id < kMaxIds; ++cert_id)
        {
            fsm::ReturnCode rc;
            X509* cert = nullptr;

            rc = keystore.GetCertificate(&cert, cert_id);

            EXPECT_EQ(rc, fsm::ReturnCode::kNotFound);
            EXPECT_EQ(cert, nullptr);

            // also check that this API works with nullptr cert
            rc = keystore.GetCertificate(nullptr, cert_id);
            EXPECT_EQ(rc, fsm::ReturnCode::kNotFound);
        }

        // We apply the same technique but check against the certificate role.
        int cert_role = static_cast<int>(fsm::Keystore::CertificateRole::kUndefined);
        ++cert_role; // skip undefined.

        for (; cert_role < static_cast<int>(fsm::Keystore::CertificateRole::kEndMarker); ++cert_role)
        {
            fsm::ReturnCode rc;
            X509* cert = nullptr;

            rc = keystore.GetCertificate(&cert, static_cast<fsm::Keystore::CertificateRole>(cert_role));

            EXPECT_NE(rc, fsm::ReturnCode::kSuccess);
            EXPECT_EQ(cert, nullptr);

            // also check that this API works with nullptr cert
            rc = keystore.GetCertificate(nullptr, static_cast<fsm::Keystore::CertificateRole>(cert_role));
            EXPECT_NE(rc, fsm::ReturnCode::kSuccess);
        }

        // Iterate through all key-roles and check that no actual keys are returned.
        int key_role = static_cast<int>(fsm::Keystore::KeyRole::kUndefined);
        ++key_role; // skip undefined

        for (; key_role < static_cast<int>(fsm::Keystore::KeyRole::kEndMarker); ++key_role)
        {
            fsm::ReturnCode rc;
            EVP_PKEY* key = nullptr;

            rc = keystore.GetKey(&key, static_cast<fsm::Keystore::KeyRole>(key_role));

            EXPECT_EQ(rc, fsm::ReturnCode::kNotFound);
            EXPECT_EQ(key, nullptr);

            // also check that this API works with nullptr key
            rc = keystore.GetKey(nullptr, static_cast<fsm::Keystore::KeyRole>(key_role));
            EXPECT_EQ(rc, fsm::ReturnCode::kNotFound);
        }

        fsm::ReturnCode rc;

        // Attempt to retrieve all certificate authorities on an empty db should fail.
        X509_STORE* store = nullptr;
        rc = keystore.GetCertificateAuthorities(&store);

        EXPECT_EQ(rc, fsm::ReturnCode::kNotFound);
        EXPECT_EQ(store, nullptr);

        // also check that this API works with nullptr
        rc = keystore.GetCertificateAuthorities(nullptr);
        EXPECT_EQ(rc, fsm::ReturnCode::kNotFound);

        // We expect attempt to retrieve stack of signers to fail.
        STACK_OF(X509)* cert_stack = sk_X509_new_null();

        rc = keystore.GetKnownSignerCertificates(cert_stack);
        int certs_in_stack = sk_X509_num(cert_stack);
        sk_X509_pop_free(cert_stack, X509_free);

        EXPECT_EQ(certs_in_stack, 0);
        EXPECT_EQ(rc, fsm::ReturnCode::kNotFound);

        // also check that this API works with nullptr
        rc = keystore.GetKnownSignerCertificates(nullptr);
        EXPECT_EQ(rc, fsm::ReturnCode::kNotFound);

        // Expect zero certificates in the cert-store.
        X509_STORE* cert_store = nullptr;

        rc = keystore.GetCertificateAuthorities(&cert_store);

        EXPECT_EQ(cert_store, nullptr);
        EXPECT_NE(rc, fsm::ReturnCode::kSuccess);

        // Expect proper error propagation when attempting to delete
        // keys that do not exist

        // Delete by id.
        {
            fsm::ReturnCode rc;

            for (int key_id = 0; key_id < kMaxIds; ++key_id)
            {
                EVP_PKEY* key = nullptr;

                rc = keystore.DeleteKey(&key, key_id);

                EXPECT_EQ(key, nullptr);
                EXPECT_NE(rc, fsm::ReturnCode::kSuccess);
            }
        }

        // Delete by role.
        {
            int key_role = static_cast<int>(fsm::Keystore::KeyRole::kUndefined);
            ++key_role; // skip undefined

            for (; key_role < static_cast<int>(fsm::Keystore::KeyRole::kEndMarker); ++key_role)
            {
                fsm::ReturnCode rc;
                EVP_PKEY* key = nullptr;

                rc = keystore.DeleteKey(&key, static_cast<fsm::Keystore::KeyRole>(key_role));

                EXPECT_EQ(key, nullptr);
                EXPECT_NE(rc, fsm::ReturnCode::kSuccess);
            }
        }

        // delete our Db
        tpPCL_stub_clean();
    }
}
typedef std::pair<X509*, EVP_PKEY*> CertKeyPair;

std::vector<fsm::Keystore::CertificateRole> one_to_one_cert_roles = {
    fsm::Keystore::CertificateRole::kVehicleActor,
    fsm::Keystore::CertificateRole::kVehicleShortRangeTlsServer,
    fsm::Keystore::CertificateRole::kCloudTlsClient,
    fsm::Keystore::CertificateRole::kDoIpTlsServer,
    fsm::Keystore::CertificateRole::kCloudSignEncrypt
};

std::vector<fsm::Keystore::CertificateRole> one_to_many_cert_roles = {
    fsm::Keystore::CertificateRole::kVehicleCa,
    fsm::Keystore::CertificateRole::kUserActor,
    fsm::Keystore::CertificateRole::kUserShortRangeTlsClient,
    fsm::Keystore::CertificateRole::kCloudCa,
};


static const fsm::CertificateId kInvalidCertId = -1;

/*
 * Check operations on the one-to-one certificate set.
 */
TEST(Keystore, OneToOneCertificates)
{
    tpPCL_stub_setErrorCode(E_PCL_ERROR_NONE);
    std::map<fsm::CertificateId, CertKeyPair> cert_key_pairs;
    std::map<int, CertKeyPair> cert_key_pairs_by_role;
    char* database_folder;
    std::string database_path;
    gint dummy;

    // get the path the database was created in.
    tpPCL_getWriteablePath(0, &database_folder, &dummy);
    database_path.assign(database_folder);

    // append the internal database name.
    database_path.append("keystore.sqlite");

    // Make sure the database doesn't exist.
    ASSERT_FALSE(FileExists(database_path.c_str()));

    // Populate the database.
    {
        fsm::Keystore keystore;

        // Make sure the database exists.
        ASSERT_TRUE(FileExists(database_path.c_str()));

        // Populate fresh keystore for one-to-one certificates.
        for (auto cert_role : one_to_one_cert_roles)
        {
            fsm::CertificateId cert_id;
            fsm::ReturnCode rc;

            EVP_PKEY* pkey = nullptr;
            X509* cert = nullptr;
            X509* old_cert = nullptr;

            GeneratePrivateKey(&pkey, fsm::kMinRSAKeySize);
            GenerateCertificate(&cert, pkey);

            rc = keystore.SetCertificate(cert_id, &old_cert, cert, cert_role);

            EXPECT_EQ(rc, fsm::ReturnCode::kSuccess);
            // As the db is initially empty, old_cert should be null.
            EXPECT_EQ(old_cert, nullptr);

            cert_key_pairs[cert_id] = CertKeyPair(cert, pkey);
            cert_key_pairs_by_role[static_cast<int>(cert_role)] = CertKeyPair(cert, pkey);
        }

        // Attempt to replace the one-to-ones with null certs. This should not be allowed.
        for (auto cert_role : one_to_one_cert_roles)
        {
            fsm::CertificateId cert_id;
            fsm::ReturnCode rc;
            X509* cert = nullptr;
            X509* old_cert = nullptr;

            rc = keystore.SetCertificate(cert_id, &old_cert, cert, cert_role);

            EXPECT_NE(rc, fsm::ReturnCode::kSuccess);
            EXPECT_EQ(old_cert, nullptr);
        }

        // Attempt to set certificates for one-to-many roles. This should not be allowed.
        for (auto cert_role : one_to_many_cert_roles)
        {
            fsm::CertificateId cert_id = kInvalidCertId;
            fsm::ReturnCode rc;
            EVP_PKEY* pkey = nullptr;
            X509* cert = nullptr;
            X509* old_cert = nullptr;

            GeneratePrivateKey(&pkey, fsm::kMinRSAKeySize);
            GenerateCertificate(&cert, pkey);

            rc = keystore.SetCertificate(cert_id, &old_cert, cert, cert_role);

            EXPECT_NE(rc, fsm::ReturnCode::kSuccess);
            EXPECT_EQ(old_cert, nullptr);
            EXPECT_EQ(cert_id, kInvalidCertId);

            EVP_PKEY_free(pkey);
            X509_free(cert);
        }
    }

    // Re-open the database. The certs should be persistent.
    // convert both cert_key_pairs and the ones retrives from keystore to PEMs and compare
    // in order to guarantee consistency. (what we add is what we retrieve later).
    {
        fsm::Keystore keystore;

        // Make sure the database exists.
        ASSERT_TRUE(FileExists(database_path.c_str()));


        std::map<fsm::CertificateId, CertKeyPair>::const_iterator it = cert_key_pairs.begin();

        // iterate over all known ids and check that we can retrieve them.
        for (; it != cert_key_pairs.end(); ++it)
        {
            X509* db_cert = nullptr;

            fsm::ReturnCode rc;

            rc = keystore.GetCertificate(&db_cert, it->first);

            EXPECT_EQ(rc, fsm::ReturnCode::kSuccess);

            X509* mem_cert = it->second.first;

            std::string pem_mem_cert = CertToPem(mem_cert);
            std::string pem_db_cert = CertToPem(db_cert);

            EXPECT_EQ(pem_mem_cert, pem_db_cert);

            X509_free(db_cert);

            // also check that this API works with nullptr cert
            rc = keystore.GetCertificate(nullptr, it->first);
            EXPECT_EQ(rc, fsm::ReturnCode::kSuccess);
        }

        std::map<int, CertKeyPair>::const_iterator it2 = cert_key_pairs_by_role.begin();
        // iterate over all add certs by role and check that we can retrieve them as files.
        for (; it2 != cert_key_pairs_by_role.end(); ++it2)
        {
            std::string db_cert_path;

            fsm::ReturnCode rc;

            rc = keystore.GetCertificate(db_cert_path, static_cast<fsm::Keystore::CertificateRole>(it2->first));

            EXPECT_EQ(rc, fsm::ReturnCode::kSuccess);

            X509* mem_cert = it2->second.first;

            std::string pem_mem_cert = CertToPem(mem_cert);
            std::string pem_db_cert = FileToPem(db_cert_path);

            EXPECT_EQ(pem_mem_cert, pem_db_cert);
        }
    }

    // Re-open the database and attempt to delete the added certificates.
    // Interogate the database and check that the deleted ones are not reported anymore.
    // Additionally, in order to check that the delete operation is persistent, re-open the database and iterogate
    // once more.
    {
        // Open DB, delete one-to-one-mapepd certs - check certs are deleted.
        {
            fsm::Keystore keystore;

            // Make sure the database exists.
            ASSERT_TRUE(FileExists(database_path.c_str()));

            // let's delete by role.
            for (auto cert_role : one_to_one_cert_roles)
            {
                X509* deleted_cert = nullptr;
                fsm::ReturnCode rc;

                rc = keystore.DeleteCertificate(&deleted_cert, cert_role);

                EXPECT_EQ(rc, fsm::ReturnCode::kSuccess);
                EXPECT_NE(deleted_cert, nullptr);

                if (rc == fsm::ReturnCode::kSuccess
                    && deleted_cert != nullptr)
                {
                    // search for the reported deleted cert in the set we already know we added (We compare PEM).
                    bool cert_found = false;
                    for (auto cert_pair : cert_key_pairs)
                    {
                        X509* mem_cert = cert_pair.second.first;

                        std::string pem_deleted_cert = CertToPem(deleted_cert);
                        std::string pem_mem_cert = CertToPem(mem_cert);

                        // if the PEMs are equal, it's a cert we've added.
                        cert_found = pem_deleted_cert == pem_mem_cert;

                        if (cert_found)
                            break;
                    }

                    EXPECT_TRUE(cert_found);
                }
            }

            // As the certificates are deleted, we should not be able to retrieve them via their original id.
            for (auto cert_pair : cert_key_pairs)
            {
                X509* cert = nullptr;
                fsm::ReturnCode rc;

                rc = keystore.GetCertificate(&cert, cert_pair.first);

                // We expect the certificate to not be found.
                EXPECT_EQ(rc, fsm::ReturnCode::kNotFound);
                EXPECT_EQ(cert, nullptr);

                // Also check that this API works with nullptr cert
                rc = keystore.GetCertificate(nullptr, cert_pair.first);
                EXPECT_EQ(rc, fsm::ReturnCode::kNotFound);
            }
        }

        // Check that the deletion operation was persistent by opening the keystore again and search via original id.
        {
            fsm::Keystore keystore;

            // Make sure the database exists.
            ASSERT_TRUE(FileExists(database_path.c_str()));

            // As the certificates are deleted, we should not be able to retrieve them via their original id.
            for (auto cert_pair : cert_key_pairs)
            {
                X509* cert = 0;
                fsm::ReturnCode rc;

                rc = keystore.GetCertificate(&cert, cert_pair.first);

                // We expect the certificate to not be found.
                EXPECT_NE(rc, fsm::ReturnCode::kSuccess);
                EXPECT_EQ(cert, nullptr);
            }
        }

        // Attempt to populate fresh keystore for one-to-one certificates
        // with too short (less than 2048) RSA key size, it should fail.
        {
            fsm::Keystore keystore;

            for (auto cert_role : one_to_one_cert_roles)
            {
                fsm::CertificateId cert_id;
                fsm::ReturnCode rc;

                EVP_PKEY* pkey = nullptr;
                X509* cert = nullptr;
                X509* old_cert = nullptr;

                GeneratePrivateKey(&pkey, fsm::kMinRSAKeySize/2);
                GenerateCertificate(&cert, pkey);

                rc = keystore.SetCertificate(cert_id, &old_cert, cert, cert_role);

                EXPECT_EQ(rc, fsm::ReturnCode::kInvalidArgument);
            }
        }

    }

    // Delete the database.
    tpPCL_stub_clean();

    // clean-up the generated cert_key_pairs
    for (auto pair : cert_key_pairs)
    {
        CertKeyPair& cert_n_key = pair.second;

        X509_free(cert_n_key.first);
        EVP_PKEY_free(cert_n_key.second);
    }

    cert_key_pairs.clear();
}


/*
 * Check operations on the one-to-many certificate set.
 */
TEST(Keystore, OneToManyCertificates)
{
    tpPCL_stub_setErrorCode(E_PCL_ERROR_NONE);

    typedef std::pair<X509*, EVP_PKEY*> CertKeyPair;
    std::map<fsm::CertificateId, CertKeyPair> cert_key_pairs;
    std::map<int, std::vector<CertKeyPair>> cert_key_pairs_by_role;
    char* database_folder;
    std::string database_path;
    gint dummy;

    // get the path the database was created in.
    tpPCL_getWriteablePath(0, &database_folder, &dummy);
    database_path.assign(database_folder);

    // append the internal database name.
    database_path.append("keystore.sqlite");

    // Make sure the database doesn't exist.
    ASSERT_FALSE(FileExists(database_path.c_str()));

    const int kCertsPerRole = 2; // max certs per role.

    // Populate the database.
    {
        fsm::Keystore keystore;

        // Make sure the database exists.
        ASSERT_TRUE(FileExists(database_path.c_str()));

        // Populate fresh keystore for one-to-many certificates.
        for (auto cert_role : one_to_many_cert_roles)
        {
            for (int cert_index = 0; cert_index < kCertsPerRole; ++cert_index)
            {
                fsm::CertificateId cert_id;
                fsm::ReturnCode rc;

                EVP_PKEY* pkey = nullptr;
                X509* cert = nullptr;

                GeneratePrivateKey(&pkey, fsm::kMinRSAKeySize);
                GenerateCertificate(&cert, pkey);

                rc = keystore.AddCertificate(cert_id, cert, cert_role);

                EXPECT_EQ(rc, fsm::ReturnCode::kSuccess);
                cert_key_pairs[cert_id] = CertKeyPair(cert, pkey);
                cert_key_pairs_by_role[static_cast<int>(cert_role)].push_back(CertKeyPair(cert, pkey));
            }

        }

        // Attempt to update the one-to-many with null certs. This should not be allowed.
        for (auto cert_pair : cert_key_pairs)
        {
            fsm::ReturnCode rc;
            fsm::CertificateId new_id = 0;

            X509* new_cert = nullptr;
            X509* old_cert = nullptr;

            rc = keystore.UpdateCertificate(&old_cert, new_id, new_cert, cert_pair.first);

            EXPECT_NE(rc, fsm::ReturnCode::kSuccess);
        }

        // Attempt to add certificates for one-to-one roles. This should not be allowed.
        for (auto cert_role : one_to_one_cert_roles)
        {
            fsm::CertificateId cert_id;
            fsm::ReturnCode rc;
            EVP_PKEY* pkey = nullptr;
            X509* cert = nullptr;

            GeneratePrivateKey(&pkey, fsm::kMinRSAKeySize);
            GenerateCertificate(&cert, pkey);


            rc = keystore.AddCertificate(cert_id, cert, cert_role);

            EXPECT_NE(rc, fsm::ReturnCode::kSuccess);

            EVP_PKEY_free(pkey);
            X509_free(cert);
        }
    }

    // Re-open the database. The certs should be persistent.
    // Convert certs to PEM and compare. (we compare the db one with the in-memory one).
    {
        fsm::Keystore keystore;

        // Make sure the database exists.
        ASSERT_TRUE(FileExists(database_path.c_str()));

        std::map<fsm::CertificateId, CertKeyPair>::const_iterator it = cert_key_pairs.begin();

        // iterate over all known ids and check that we can retrieve them.
        for (; it != cert_key_pairs.end(); ++it)
        {
            X509* db_cert = nullptr;
            fsm::ReturnCode rc;

            rc = keystore.GetCertificate(&db_cert, it->first);

            EXPECT_EQ(rc, fsm::ReturnCode::kSuccess);
            X509* mem_cert = it->second.first;

            std::string pem_mem_cert = CertToPem(mem_cert);
            std::string pem_db_cert = CertToPem(db_cert);

            EXPECT_EQ(pem_mem_cert, pem_db_cert);

            X509_free(db_cert);
        }
    }

    // Re-open the database and retrieve all certs on a one-to-many role.
    {
        fsm::Keystore keystore;

        // Make sure the database exists.
        ASSERT_TRUE(FileExists(database_path.c_str()));

        for (auto role : one_to_many_cert_roles)
        {
            fsm::ReturnCode rc;

            STACK_OF(X509)* cert_stack = sk_X509_new_null();
            rc = keystore.GetCertificates(cert_stack, role);
            std::uint32_t certs_per_role = sk_X509_num(cert_stack);

            EXPECT_EQ(rc, fsm::ReturnCode::kSuccess);
            EXPECT_EQ(certs_per_role, kCertsPerRole);

            sk_X509_pop_free(cert_stack, X509_free);
        }

        // test that we can get certs as file
        // the file has the certs in teh same order they were added
        // and that it contains all
        for (auto role : one_to_many_cert_roles)
        {
            fsm::ReturnCode rc;

            std::string certificates_file;
            rc = keystore.GetCertificates(certificates_file, role);

            EXPECT_EQ(rc, fsm::ReturnCode::kSuccess);

            std::string certificates_pem;
            for (auto cert_key_pair : cert_key_pairs_by_role[static_cast<int>(role)])
            {
                certificates_pem.append(CertToPem(cert_key_pair.first));
            }

            std::string certificates_file_pem = FileToPem(certificates_file);

            EXPECT_EQ(certificates_pem, certificates_file_pem);
        }

        // Verifiy that if nullptr stack is passed kSuccess is returned
        for (auto role : one_to_many_cert_roles)
        {
            fsm::ReturnCode rc;

            rc = keystore.GetCertificates(nullptr, role);

            EXPECT_EQ(rc, fsm::ReturnCode::kSuccess);
        }

        // Try to retrieve certs for one-to-many with one-to-one roles.
        // Operation should fail as certs for one-to-one roles are to be retrieved via 'GetCertificate'.
        for (auto role : one_to_one_cert_roles)
        {
            fsm::ReturnCode rc;
            STACK_OF(X509)* cert_stack = sk_X509_new_null();

            rc = keystore.GetCertificates(cert_stack, role);
            std::uint32_t certs_per_role = sk_X509_num(cert_stack);

            EXPECT_NE(rc, fsm::ReturnCode::kSuccess);
            EXPECT_EQ(certs_per_role, 0); // no certs should be reported as call failed.

            sk_X509_pop_free(cert_stack, X509_free);
        }
    }

    // Re-open the database and delete the one-to-many's by id and check the persistency.
    {
        // First, delete by id and check getter by id on the same instance.
        {
            fsm::Keystore keystore;
            fsm::ReturnCode rc;
            X509* db_cert = nullptr;

            for (auto cert_pair : cert_key_pairs)
            {
                rc = keystore.DeleteCertificate(&db_cert, cert_pair.first);

                // Check that the reported deleted certificate is the actual one
                // we know we added.
                EXPECT_EQ(rc, fsm::ReturnCode::kSuccess);
                X509* mem_cert = cert_pair.second.first;

                std::string pem_mem_cert = CertToPem(mem_cert);
                std::string pem_db_cert = CertToPem(db_cert);

                EXPECT_EQ(pem_mem_cert, pem_db_cert);

                X509_free(db_cert);
                db_cert = nullptr;
            }

            // Check getter by id. The keystore should fail finding the certificate.
            for (auto cert_pair : cert_key_pairs)
            {
                db_cert = nullptr;
                rc = keystore.GetCertificate(&db_cert, cert_pair.first);

                EXPECT_NE(rc, fsm::ReturnCode::kSuccess);
                EXPECT_EQ(db_cert, nullptr);
            }

            // Verifiy that if nullptr stack is passed kNotFound is returned
            for (auto role : one_to_many_cert_roles)
            {
                fsm::ReturnCode rc;

                rc = keystore.GetCertificates(nullptr, role);

                EXPECT_EQ(rc, fsm::ReturnCode::kNotFound);
            }
       }

        // Secondly, by using another instance and calling getter by id ensures that
        // the delete operation is persistent.
        {
            fsm::Keystore keystore;
            fsm::ReturnCode rc;
            X509* db_cert = nullptr;

            // Check getter by id. The keystore should fail finding the certificate.
            for (auto cert_pair : cert_key_pairs)
            {
                db_cert = nullptr;
                rc = keystore.GetCertificate(&db_cert, cert_pair.first);

                EXPECT_EQ(rc, fsm::ReturnCode::kNotFound);
                EXPECT_EQ(db_cert, nullptr);
            }
        }

        // Attempt to populate fresh keystore for one-to-many certificates
        // with too short (less than 2048) RSA key size, it should fail.
        {
            fsm::Keystore keystore;

            for (auto cert_role : one_to_many_cert_roles)
            {
                fsm::CertificateId cert_id;
                fsm::ReturnCode rc;

                EVP_PKEY* pkey = nullptr;
                X509* cert = nullptr;
                X509* old_cert = nullptr;

                GeneratePrivateKey(&pkey, fsm::kMinRSAKeySize/2);
                GenerateCertificate(&cert, pkey);

                rc = keystore.SetCertificate(cert_id, &old_cert, cert, cert_role);

                EXPECT_EQ(rc, fsm::ReturnCode::kInvalidArgument);
            }
        }

    }

    // Delete the database.
    tpPCL_stub_clean();

    // clean-up the generated cert_key_pairs
    for (auto pair : cert_key_pairs)
    {
        CertKeyPair& cert_n_key = pair.second;

        X509_free(cert_n_key.first);
        EVP_PKEY_free(cert_n_key.second);
    }

    cert_key_pairs.clear();
}

std::vector<fsm::Keystore::KeyRole> key_roles = {
    fsm::Keystore::KeyRole::kVehicleCa,
    fsm::Keystore::KeyRole::kVehicleActor,
    fsm::Keystore::KeyRole::kVehicleShortRangeTlsServer,
    fsm::Keystore::KeyRole::kCloudTlsClient,
    fsm::Keystore::KeyRole::kDoIpTlsServer,
    fsm::Keystore::KeyRole::kCloudSignEncrypt
};

typedef std::pair<EVP_PKEY*, fsm::Keystore::KeyRole> KeyRolePair;
std::map<fsm::KeyId, KeyRolePair> keys;

/*
 * Check operations on keys.
 */
TEST(Keystore, Keys)
{
    tpPCL_stub_setErrorCode(E_PCL_ERROR_NONE);
    char* database_folder;
    std::string database_path;
    gint dummy;

    // get the path the database will be created in.
    tpPCL_getWriteablePath(0, &database_folder, &dummy);
    database_path.assign(database_folder);

    // append the internal database name.
    database_path.append("keystore.sqlite");

   // Check we can populate the Db with new keys.
    {
        fsm::Keystore keystore;

        // Make sure the database exists.
        ASSERT_TRUE(FileExists(database_path.c_str()));

        for (auto key_role : key_roles)
        {
            EVP_PKEY* old_key = nullptr;
            EVP_PKEY* fresh_key = nullptr;
            fsm::KeyId key_id;
            fsm::ReturnCode rc;

            GeneratePrivateKey(&fresh_key, fsm::kMinRSAKeySize);

            rc = keystore.SetKey(key_id, &old_key, fresh_key, key_role);
            EXPECT_EQ(rc, fsm::ReturnCode::kSuccess);

            // there should be no old key (database actually expected to be empty).
            EXPECT_EQ(old_key, nullptr);

            // make sure key is unique.
            EXPECT_EQ(keys.count(key_id), 0);

            keys[key_id] = KeyRolePair(fresh_key, key_role);
        }

        // Test we can retrieve the keys we just added via the Keystore interface.
        for (auto key_pair : keys)
        {
            fsm::ReturnCode rc;
            EVP_PKEY* db_key = nullptr;

            rc = keystore.GetKey(&db_key, key_pair.second.second);
            EXPECT_EQ(rc, fsm::ReturnCode::kSuccess);


            // Check PEMs match.
            std::string pem_mem_key = KeyToPem(key_pair.second.first);
            std::string pem_db_key = KeyToPem(db_key);

            EXPECT_EQ(pem_mem_key, pem_db_key);

            // Release key.
            EVP_PKEY_free(db_key);

            // Verify that this API works with nullptr key
            rc = keystore.GetKey(nullptr, key_pair.second.second);
            EXPECT_EQ(rc, fsm::ReturnCode::kSuccess);
        }
    }

    // Instantiate a new keystore and check that previously added keys are persistent in db.
    {
        fsm::Keystore keystore;

        for (auto key_pair : keys)
        {
            fsm::ReturnCode rc;
            EVP_PKEY* db_key = nullptr;

            rc = keystore.GetKey(&db_key, key_pair.second.second);

            EXPECT_EQ(rc, fsm::ReturnCode::kSuccess);


            // Check PEMs match.
            std::string pem_mem_key = KeyToPem(key_pair.second.first);
            std::string pem_db_key = KeyToPem(db_key);

            EXPECT_EQ(pem_mem_key, pem_db_key);

            // Release key.
            EVP_PKEY_free(db_key);
        }

        for (auto key_pair : keys)
        {
            fsm::ReturnCode rc;
            std::string db_key_path;

            rc = keystore.GetKey(db_key_path, key_pair.second.second);

            EXPECT_EQ(rc, fsm::ReturnCode::kSuccess);


            // Check PEMs match.
            std::string pem_mem_key = KeyToPem(key_pair.second.first);
            std::string pem_db_key = FileToPem(db_key_path);

            EXPECT_EQ(pem_mem_key, pem_db_key);
        }
    }

    // Check key deletion operation.
    {
        fsm::Keystore keystore;

        fsm::ReturnCode rc;
        EVP_PKEY* db_key = nullptr;

        for (auto key_pair : keys)
        {
            rc = keystore.DeleteKey(&db_key, key_pair.first);

            EXPECT_EQ(rc, fsm::ReturnCode::kSuccess);

            // The returned key PEM must match the one we know we added.
            std::string pem_mem_key = KeyToPem(key_pair.second.first);
            std::string pem_db_key = KeyToPem(db_key);

            EXPECT_EQ(pem_mem_key, pem_db_key);

            // Release key.
            EVP_PKEY_free(db_key);

            db_key = nullptr;

            // Attempting to delete what is actually the same key (identified by KeyRole, this time) should fail.
            rc = keystore.DeleteKey(&db_key, key_pair.second.second);

            EXPECT_NE(rc, fsm::ReturnCode::kSuccess);

            // The output parameter should not be affected in error-cases.
            EXPECT_EQ(db_key, nullptr);

        }

        // Re-iterate and check that getter no longer returns the deleted keys.
        for (auto key_pair : keys)
        {
            db_key = nullptr;
            rc = keystore.GetKey(&db_key, key_pair.second.second);

            // Failure should be reported as key should no longer exist.
            EXPECT_EQ(rc, fsm::ReturnCode::kNotFound);

            // The output parameter should not be affected in error-cases.
            EXPECT_EQ(db_key, nullptr);

            // Verify that this API works with nullptr as key
            rc = keystore.GetKey(nullptr, key_pair.second.second);
            EXPECT_EQ(rc, fsm::ReturnCode::kNotFound);
        }
    }

    // Check key deletion operation is persistent by instantiating a new keystore.
    {
        fsm::Keystore keystore;

        fsm::ReturnCode rc;
        EVP_PKEY* db_key = nullptr;

        // Re-iterate and check that getter no longer returns the deleted keys.
        for (auto key_pair : keys)
        {
            db_key = nullptr;
            rc = keystore.GetKey(&db_key, key_pair.second.second);

            // Failure should be reported as key should no longer exist.
            EXPECT_NE(rc, fsm::ReturnCode::kSuccess);

            // The output parameter should not be affected in error-cases.
            EXPECT_EQ(db_key, nullptr);
        }
    }

    // Attempt to add keys with too short (less than 2048) key sizes, it should fail.
    {
        fsm::Keystore keystore;

        for (auto key_role : key_roles)
        {
            EVP_PKEY* old_key = nullptr;
            EVP_PKEY* fresh_key = nullptr;
            fsm::KeyId key_id;
            fsm::ReturnCode rc;

            GeneratePrivateKey(&fresh_key, fsm::kMinRSAKeySize/2);

            rc = keystore.SetKey(key_id, &old_key, fresh_key, key_role);
            EXPECT_EQ(rc, fsm::ReturnCode::kInvalidArgument);
        }
    }

    // Delete the database.
    tpPCL_stub_clean();

    // clean-up the generated keys.
    for (auto key_pair : keys)
    {
        EVP_PKEY* key = key_pair.second.first;

        EVP_PKEY_free(key);
    }
    keys.clear();
}

std::vector<fsm::Keystore::CertificateRole> ca_cert_roles = {
    fsm::Keystore::CertificateRole::kVehicleCa,
    fsm::Keystore::CertificateRole::kCloudCa
};

std::map<fsm::CertificateId, CertKeyPair> ca_cert_key_pairs;
std::map<fsm::CertificateId, fsm::Keystore::CertificateRole> ca_id_role_pairs;

/*
 * Check operations on Certificate Authorities.
 */
TEST(Keystore, CAs)
{
    tpPCL_stub_setErrorCode(E_PCL_ERROR_NONE);
    char* database_folder;
    std::string database_path;
    gint dummy;

    // get the path the database will be created in.
    tpPCL_getWriteablePath(0, &database_folder, &dummy);
    database_path.assign(database_folder);

    // append the internal database name.
    database_path.append("keystore.sqlite");

    const std::uint32_t kCertsPerCaRole = 10;
    fsm::Keystore keystore;

    {
        // Make sure the database exists.
        ASSERT_TRUE(FileExists(database_path.c_str()));
        fsm::ReturnCode rc;

        // populate keystore with kCertPerCaRole CA certificates.
        for (auto role : ca_cert_roles)
        {
            for (std::uint32_t cert_index = 0; cert_index < kCertsPerCaRole; ++cert_index)
            {
                fsm::CertificateId cert_id;
                EVP_PKEY* key = nullptr;
                X509* cert = nullptr;

                GeneratePrivateKey(&key, fsm::kMinRSAKeySize);
                GenerateCertificate(&cert, key);

                rc = keystore.AddCertificate(cert_id, cert, role);
                ca_cert_key_pairs[cert_id] = CertKeyPair(cert, key);
                ca_id_role_pairs[cert_id] = role;

                EXPECT_EQ(rc, fsm::ReturnCode::kSuccess);
            }
        }

        // Check reported full store.
        {
            X509_STORE* store = nullptr;
            rc = keystore.GetCertificateAuthorities(&store);

            EXPECT_EQ(rc, fsm::ReturnCode::kSuccess);
            EXPECT_NE(store, nullptr);

            std::uint32_t ca_cert_count = sk_X509_OBJECT_num(store->objs);

            EXPECT_EQ(ca_cert_count, kCertsPerCaRole * ca_cert_roles.size());

            // Verify that this API works with nullptr as store
            rc = keystore.GetCertificateAuthorities(nullptr);
            EXPECT_EQ(rc, fsm::ReturnCode::kSuccess);

            // linear search for each cert from memory structure in the retrived store.
            // This ensures that the store certificates are actually the ones we know we added.

            // Transform the store objects stack into a vector for easy searching.
            std::vector<X509*> store_certs;

            for (std::uint32_t ca_cert_index = 0; ca_cert_index < ca_cert_count; ++ca_cert_index)
            {
                X509_OBJECT* cert_object = sk_X509_OBJECT_pop(store->objs);
                store_certs.push_back(cert_object->data.x509);
            }

            // Perform the actual linear search.
            for (auto cert_pair : ca_cert_key_pairs)
            {
                X509* mem_cert = cert_pair.second.first;

                // each mem_cert must be found in the store_certs to guarantee that store contains all previously
                // added certs.
                bool cert_found = false;

                for (auto store_cert : store_certs)
                {
                    if (X509_cmp(store_cert, mem_cert) == 0)
                    {
                        cert_found = true;
                        break;
                    }
                }

                // expectation is each cert in memory is found in the store.
                EXPECT_TRUE(cert_found);
            }

            X509_STORE_free(store);
        }

        // Check reported store for each CA role.
        {
            // Create a vector of key_pairs so what we can remove the found certs.
            // At the end of the test, this vector must be empty - meaning all certs were found.

            for (auto role : ca_cert_roles)
            {
                X509_STORE* store = nullptr;
                rc = keystore.GetCertificateAuthority(&store, role);

                EXPECT_EQ(rc, fsm::ReturnCode::kSuccess);
                EXPECT_NE(store, nullptr);

                std::uint32_t ca_cert_count = sk_X509_OBJECT_num(store->objs);

                EXPECT_EQ(ca_cert_count, kCertsPerCaRole);

                // Verify that this API works with nullptr as store
                rc = keystore.GetCertificateAuthority(nullptr, role);
                EXPECT_EQ(rc, fsm::ReturnCode::kSuccess);

                // Transform the store objects stack into a vector for easy searching.
                std::vector<X509*> store_certs;

                for (std::uint32_t ca_cert_index = 0; ca_cert_index < ca_cert_count; ++ca_cert_index)
                {
                    X509_OBJECT* cert_object = sk_X509_OBJECT_pop(store->objs);
                    store_certs.push_back(cert_object->data.x509);
                }

                // linear-search in memory in order to find all certs that match the current role.
                std::vector<X509*> certs_for_role;

                for (auto id_role_pair : ca_id_role_pairs)
                {
                    if (id_role_pair.second == role)
                    {
                        const fsm::CertificateId cert_id = id_role_pair.first;
                        X509* cert = ca_cert_key_pairs[cert_id].first;

                        certs_for_role.push_back(cert);
                    }
                }


                // As we gathered all certs for current role in certs_for_role vector, the size of the vector must
                // match the size of the store.
                EXPECT_EQ(ca_cert_count, certs_for_role.size());

                // Perform the actual linear search.
                for (auto mem_cert : certs_for_role)
                {
                    // each mem_cert must be found in the store_certs to guarantee that store contains all previously
                    // added certs.
                    bool cert_found = false;

                    for (auto store_cert : store_certs)
                    {
                        if (X509_cmp(store_cert, mem_cert) == 0)
                        {
                            cert_found = true;
                            break;
                        }
                    }

                    // expectation is each cert in memory is found in the store.
                    EXPECT_TRUE(cert_found);
                }

                X509_STORE_free(store);
            }
        }

        // Attempt to retrieve store for non-CA roles. Operation should fail.
        {
            // The set of non-CA roles is the union between one-to-one and one-to-many
            // minus the known CA roles.
            std::vector<fsm::Keystore::CertificateRole> non_ca_roles;

            non_ca_roles.insert(non_ca_roles.end(), one_to_one_cert_roles.begin(), one_to_one_cert_roles.end());
            non_ca_roles.insert(non_ca_roles.end(), one_to_many_cert_roles.begin(), one_to_many_cert_roles.end());

            // Remove the CA roles to from the non-CA roles.
            for (auto ca_role : ca_cert_roles)
            {
                auto it = std::find(non_ca_roles.begin(), non_ca_roles.end(), ca_role);

                if (it != non_ca_roles.end())
                {
                    non_ca_roles.erase(it);
                }
            }

            // for all non-CA roles, try to retrive store.
            for (auto non_ca_role : non_ca_roles)
            {
                X509_STORE* store = nullptr;
                rc = keystore.GetCertificateAuthority(&store, non_ca_role);

                EXPECT_NE(rc, fsm::ReturnCode::kSuccess);
                EXPECT_EQ(store, nullptr);

                X509_STORE_free(store);
            }
        }

        // Delete all ca certificates.
        // Operation should report a stack of size kCertsPerCaRole for each role.
        for (auto ca_role : ca_cert_roles)
        {
            STACK_OF(X509)* cert_stack = sk_X509_new_null();
            keystore.DeleteCertificates(cert_stack, ca_role);
            std::uint32_t cert_count = sk_X509_num(cert_stack);

            EXPECT_EQ(cert_count, kCertsPerCaRole);

            sk_X509_pop_free(cert_stack, X509_free);
        }

        // Verify that there are no ca certificates left
        for (auto ca_role : ca_cert_roles)
        {
            X509_STORE* store = nullptr;
            rc = keystore.GetCertificateAuthority(&store, ca_role);

            EXPECT_EQ(rc, fsm::ReturnCode::kNotFound);
            EXPECT_EQ(store, nullptr);

            X509_STORE_free(store);

            // Verify that this API works with nullptr as store
            rc = keystore.GetCertificateAuthority(nullptr, ca_role);
            EXPECT_EQ(rc, fsm::ReturnCode::kNotFound);
        }

    }

    // Delete the database.
    tpPCL_stub_clean();
}


std::mutex global_sync;
typedef std::pair<X509*, fsm::Keystore::CertificateRole> CertRolePair;
typedef std::set<CertRolePair> WorkerDataSet;
std::vector<WorkerDataSet> global_data_set;

const std::uint32_t kWriterCount = 2;
const std::uint32_t kWorkerRoleSetSize = 10;
const std::uint32_t kReaderCount = 1;
std::vector<fsm::CertificateId> keystore_ids;


bool WriterWorker(fsm::Keystore* keystore,
                  WorkerDataSet worker_data_set)
{
    bool success = true;

    for (auto cert_pair : worker_data_set)
    {
        fsm::ReturnCode rc;
        fsm::CertificateId cert_id, kInvalidCertId;

        rc = keystore->AddCertificate(cert_id, cert_pair.first, cert_pair.second);

        EXPECT_EQ(rc, fsm::ReturnCode::kSuccess);
        EXPECT_NE(cert_id, kInvalidCertId);

        success = success ? rc == fsm::ReturnCode::kSuccess : success;
        success = success ? cert_id != kInvalidCertId : success;

        global_sync.lock();
        keystore_ids.push_back(cert_id);
        global_sync.unlock();
    }

    return success;
}

bool ReaderWorker(fsm::Keystore* keystore)
{
    bool success = true;
    const uint32_t kTotalAddedCerts = kWriterCount * kWorkerRoleSetSize * one_to_many_cert_roles.size();
    uint32_t total_read_certs = 0;
    do
    {
        fsm::CertificateId cert_id;

        global_sync.lock();
        const uint32_t certs_added = keystore_ids.size();
        global_sync.unlock();

        if (total_read_certs < certs_added)
        {
            global_sync.lock();
            cert_id = keystore_ids.at(total_read_certs);
            global_sync.unlock();

            fsm::ReturnCode rc;
            X509* cert = nullptr;

            rc = keystore->GetCertificate(&cert, cert_id);

            EXPECT_EQ(rc, fsm::ReturnCode::kSuccess);
            EXPECT_NE(cert, nullptr);

            success = success ? rc == fsm::ReturnCode::kSuccess : success;
            success = success ? cert != nullptr : success;

            ++total_read_certs;
        }

    } while (total_read_certs < kTotalAddedCerts);

    return success;
}

/*
 * Check concurrent read-write access on keystore.
 */
TEST(Keystore, ConcurrentAccess)
{
    tpPCL_stub_setErrorCode(E_PCL_ERROR_NONE);
    char* database_folder;
    std::string database_path;
    gint dummy;

    // get the path the database will be created in.
    tpPCL_getWriteablePath(0, &database_folder, &dummy);
    database_path.assign(database_folder);

    // append the internal database name.
    database_path.append("keystore.sqlite");

    // Create the per-worker data-set for each write-worker.
    for (uint32_t write_worker_idx = 0; write_worker_idx < kWriterCount; ++write_worker_idx)
    {
        WorkerDataSet worker_data_set;

        // for each one-to-many role, generate kWorkerRoleSetSize certificates.
        for (auto role : one_to_many_cert_roles)
        {
            for (std::uint32_t cert_idx = 0; cert_idx < kWorkerRoleSetSize; ++cert_idx)
            {
                EVP_PKEY* key = 0;
                X509* cert = 0;

                GeneratePrivateKey(&key, fsm::kMinRSAKeySize);
                GenerateCertificate(&cert, key);

                worker_data_set.insert(CertRolePair(cert, role));
            }
        }

        global_data_set.push_back(worker_data_set);
    }

    // Concurrent read-write.
    // Instantiate kWriterCount writer threads that will populate the database while
    // kReaderCount are continuosly reading.
    {
        typedef std::packaged_task<bool()> Task;
        typedef std::pair<std::thread, std::future<bool>> TaskResultPair;
        fsm::Keystore keystore;
        std::vector<TaskResultPair> tasks;

        // Writers.
        for (uint32_t write_worker_idx = 0; write_worker_idx < kWriterCount; ++write_worker_idx)
        {
            // launch each write task.
            Task write_task(std::bind(WriterWorker, &keystore, global_data_set[write_worker_idx]));

            std::future<bool> future_result = write_task.get_future();

            std::thread worker_thread(std::move(write_task)); // launch on different thread.
            tasks.push_back(TaskResultPair(std::move(worker_thread), std::move(future_result)));
        }

        // Readers.
        for (uint32_t read_worker_idx = 0; read_worker_idx < kReaderCount; ++read_worker_idx)
        {
            // launch each read task.
            Task read_task(std::bind(ReaderWorker, &keystore));

            std::future<bool> future_result = read_task.get_future();

            std::thread worker_thread(std::move(read_task)); // launch on different thread.
            tasks.push_back(TaskResultPair(std::move(worker_thread), std::move(future_result)));
        }

        // grab results and check them.
        for (auto& task : tasks)
        {
            EXPECT_TRUE(task.second.get());
            task.first.join();
        }

        // Delete all certificates by using the one-to-many role.
        // Operation should report a stack of size kWriterCount * kWorkerRoleSetSize for each role.
        for (auto role : one_to_many_cert_roles)
        {

            STACK_OF(X509)* cert_stack = sk_X509_new_null();
            keystore.DeleteCertificates(cert_stack, role);
            std::uint32_t cert_count = sk_X509_num(cert_stack);

            EXPECT_EQ(cert_count, kWriterCount * kWorkerRoleSetSize);

            sk_X509_pop_free(cert_stack, X509_free);
        }
    }

    // Delete the database.
    tpPCL_stub_clean();


    // Release data-sets.
    for (auto local_data_set : global_data_set)
    {
        for (auto cert_pair : local_data_set)
        {
            X509_free(cert_pair.first);
        }
    }
}

/** \}    end of addtogroup */
