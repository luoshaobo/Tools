/*
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
 *  \file     keystore.cc
 *  \brief    FSM KeyStore
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup fsm_keystore
 *  \{
*/

// ** INCLUDES *****************************************************************

#include "keystore.h"

#include <algorithm>
#include <cstring>
#include <sstream>

#include <fcntl.h>
#include <linux/limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "dlt/dlt.h"
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/rand.h>

#include "persistence_client_library.h"

DLT_DECLARE_CONTEXT(dlt_libfsm_keystore);

#include <iostream>

namespace user_manager
{

#define KEYSTORE_TABLE_NAME "keystore"
#define ID_COLUMN_NAME "id"
#define TYPE_COLUMN_NAME "type"
#define REFERENCE_TYPE_COLUMN_NAME "reference_type"
#define REFERENCE_COLUMN_NAME "reference"
#define VALID_COLUMN_NAME "valid"

Keystore::Keystore() : database_(nullptr)
{

    DLT_REGISTER_CONTEXT(dlt_libfsm_keystore, "LKEY","libfsm_keystore logs");

    char* persistance_path = nullptr;
    int path_size = 0;

    // ensure PRNG is seeded, we will use it for
    // key file names
    //TODO: is this good? or should we settle for
    //      seed at program start?
    const int kRndGenSeedSize = 32;
    if (RAND_load_file("/dev/urandom", kRndGenSeedSize) != kRndGenSeedSize)
    {
        DLT_LOG_STRING(dlt_libfsm_keystore, DLT_LOG_WARN,
                       "failed to seed PRNG.");
    }

    tpPCL_Error_t pcl_return = tpPCL_getWriteablePath(E_PCL_LDBID_PUBLIC,
                                                    &persistance_path,
                                                    &path_size);

    if (pcl_return != E_PCL_ERROR_NONE || !persistance_path || path_size == 0)
    {
        DLT_LOG_STRING(dlt_libfsm_keystore, DLT_LOG_ERROR, "failed to retrieve persistance path");
    }
    else
    {
        bool database_ok = true;

        // Append the filename of the database to the persistence PUBLIC area and then
        // try to open the database.
        std::string database_path(persistance_path);
        database_path.append(kDatabaseName_);

        if (SQLITE_OK != sqlite3_open_v2(database_path.c_str(),
                                         &database_,
                                         SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
                                         nullptr))
        {
            database_ok = false;
            DLT_LOG_STRINGF(dlt_libfsm_keystore, DLT_LOG_ERROR,
                            "Failed to open keystore db, path %s, error: %s",
                            database_path.c_str(),
                            database_ ? sqlite3_errmsg(database_) : "Failed to allocate memory");
        }
        else
        {
            // Ensure sqlite will sleep and retry for some time if db is busy
            // when trying to execute some statement (default behaviour is to
            // fail immediately with SQLITE_BUSY).
            if (sqlite3_busy_timeout(database_, kDatabaseTimeout_) != SQLITE_OK)
            {
                DLT_LOG_STRINGF(dlt_libfsm_keystore, DLT_LOG_WARN,
                                "Failed to set busy_timeout, error: %s",
                                sqlite3_errmsg(database_));
            }

            // Create the keystore table if it does not exist in the DB.
            sqlite3_stmt* setup_table = nullptr;

            if (sqlite3_prepare_v2(database_,
                                   "CREATE TABLE IF NOT EXISTS " KEYSTORE_TABLE_NAME "(" \
                                     ID_COLUMN_NAME " INTEGER PRIMARY KEY AUTOINCREMENT, " \
                                     TYPE_COLUMN_NAME " INTEGER, "                     \
                                     REFERENCE_TYPE_COLUMN_NAME " INTEGER, "           \
                                     REFERENCE_COLUMN_NAME " BLOB, "                   \
                                     VALID_COLUMN_NAME " INTEGER)",
                                   -1, // Read above statement to null terminator
                                   &setup_table,
                                   nullptr) == SQLITE_OK)
            {
                if (sqlite3_step(setup_table) != SQLITE_DONE)
                {
                    database_ok = false;
                    DLT_LOG_STRINGF(dlt_libfsm_keystore, DLT_LOG_ERROR,
                                    "Failed to execute open_table statement, error: %s",
                                    sqlite3_errmsg(database_));
                }

                sqlite3_finalize(setup_table);
                setup_table = nullptr;
            }
            else
            {
                database_ok = false;
                DLT_LOG_STRINGF(dlt_libfsm_keystore, DLT_LOG_ERROR,
                                "Failed to prepare open_table statement, error: %s",
                                sqlite3_errmsg(database_));
            }
        }

        if (!database_ok && database_)
        {
            sqlite3_close(database_);
            database_ = nullptr;
        }
    }
	
    if(persistance_path)
        g_free(persistance_path);

}

Keystore::~Keystore()
{
    // TODO:
    // clean up cached keys
    // clean up prepared statements

    // clean up cached certificates
    for(auto entry : certificate_cache_)
    {
        X509_free(entry.second);
    }

    for(auto entry : key_cache_)
    {
        EVP_PKEY_free(entry.second);
    }

    for(auto entry : statement_cache_)
    {
        while (!entry.second.empty())
        {
            sqlite3_stmt* prepared_statement = entry.second.top();
            entry.second.pop();

            sqlite3_finalize(prepared_statement);
        }
    }

    sqlite3_close(database_); // nullptr safe

    DLT_UNREGISTER_CONTEXT(dlt_libfsm_keystore);
}

ReturnCode Keystore::GetCertificate(X509** certificate, const CertificateId id)
{
    ReturnCode rc =  ReturnCode::kSuccess;

    sqlite3_stmt* prepared_statement = nullptr;

    std::string statement = "SELECT " ID_COLUMN_NAME ", " TYPE_COLUMN_NAME ", " \
                            REFERENCE_TYPE_COLUMN_NAME ", " REFERENCE_COLUMN_NAME ", " \
                            VALID_COLUMN_NAME " FROM " KEYSTORE_TABLE_NAME " WHERE " \
                            ID_COLUMN_NAME " = ?1 AND " VALID_COLUMN_NAME " = 1;";

    // we expect a nullptr or a valid pointer to nullptr
    if (certificate && *certificate)
    {
        rc = ReturnCode::kInvalidArgument;
    }

    if (rc == ReturnCode::kSuccess)
    {
        rc = GetStatement(&prepared_statement, statement);
    }

    if (rc == ReturnCode::kSuccess)
    {
        if (sqlite3_bind_int(prepared_statement, 1, id) != SQLITE_OK)
        {
            rc =  ReturnCode::kError;
        }
    }

    if (rc == ReturnCode::kSuccess)
    {
        rc = GetCertificate(certificate, prepared_statement);
    }

    ReturnStatement(prepared_statement, statement);

    return rc;
}

ReturnCode Keystore::GetCertificate(X509** certificate,
                                    const Keystore::CertificateRole role)
{
    ReturnCode rc =  ReturnCode::kSuccess;

    sqlite3_stmt* prepared_statement = nullptr;

    std::string statement = "SELECT " ID_COLUMN_NAME ", " TYPE_COLUMN_NAME ", " \
                            REFERENCE_TYPE_COLUMN_NAME ", " REFERENCE_COLUMN_NAME ", " \
                            VALID_COLUMN_NAME " FROM " KEYSTORE_TABLE_NAME " "\
                            "WHERE " TYPE_COLUMN_NAME " = ?1 AND " VALID_COLUMN_NAME " = 1;";

    // we expect a nullptr or a valid pointer to nullptr.
    if (certificate && *certificate)
    {
        rc = ReturnCode::kInvalidArgument;
    }
    if (IsOneToMany(role))
    {
        rc = ReturnCode::kInvalidArgument;
    }

    if (rc == ReturnCode::kSuccess)
    {
        rc = GetStatement(&prepared_statement, statement);
    }

    if (rc == ReturnCode::kSuccess)
    {
        if (sqlite3_bind_int(prepared_statement, 1, static_cast<int>(role)) != SQLITE_OK)
        {
            rc =  ReturnCode::kError;
        }
    }

    if (rc == ReturnCode::kSuccess)
    {
        rc = GetCertificate(certificate, prepared_statement);
    }

    ReturnStatement(prepared_statement, statement);

    return rc;
}

ReturnCode Keystore::GetCertificate(std::string& certificate, const CertificateRole role)
{
    // In order to hide the internals of the Keystore we will
    // make a tmp file, store the certificate in it and return that
    // to the client.
    // Possibly things could be speed up by simply copying the certificate
    // if it is already stored as file, or even returning path into storage
    // but these should typically only be called when setting up a tls session.

    // TODO: is this secure enough? we have more control over permissions on keystore files

    X509* local_certificate = nullptr;
    std::string local_certificate_file_path;

    ReturnCode rc = GetCertificate(&local_certificate, role);

    if (rc == ReturnCode::kSuccess)
    {
        FILE* file = CreateTemporaryFile(local_certificate_file_path);

        if (file)
        {
            if (PEM_write_X509(file, local_certificate) == 1)
            {
                certificate.assign(local_certificate_file_path);
            }
            else
            {
                DLT_LOG_STRINGF(dlt_libfsm_keystore, DLT_LOG_ERROR,
                                "Failed to populate temporary file: %s.",
                                local_certificate_file_path.c_str());

                rc = ReturnCode::kError;
            }

            fclose(file);
        }
        else
        {
            DLT_LOG_STRINGF(dlt_libfsm_keystore, DLT_LOG_ERROR,
                            "Failed to open temporary file: %s.",
                            local_certificate_file_path.c_str());

            rc = ReturnCode::kError;
        }
    }

    return rc;
}

ReturnCode Keystore::GetCertificates(STACK_OF(X509)* certificates,
                                     const Keystore::CertificateRole role)
{
    ReturnCode rc =  ReturnCode::kSuccess;

    sqlite3_stmt* prepared_statement = nullptr;

    std::string statement = "SELECT " ID_COLUMN_NAME ", " TYPE_COLUMN_NAME ", " \
                            REFERENCE_TYPE_COLUMN_NAME ", " REFERENCE_COLUMN_NAME ", " \
                            VALID_COLUMN_NAME " FROM " KEYSTORE_TABLE_NAME " "\
                            "WHERE " TYPE_COLUMN_NAME " = ?1 AND " VALID_COLUMN_NAME " = 1;";

    if (!IsOneToMany(role))
    {
        rc = ReturnCode::kInvalidArgument;
    }

    if (rc == ReturnCode::kSuccess)
    {
        rc = GetStatement(&prepared_statement, statement);
    }

    if (rc == ReturnCode::kSuccess)
    {
        if (sqlite3_bind_int(prepared_statement, 1, static_cast<int>(role)) != SQLITE_OK)
        {
            rc =  ReturnCode::kError;
        }
    }

    if (rc == ReturnCode::kSuccess)
    {
        rc = GetCertificates(certificates, prepared_statement);
    }

    ReturnStatement(prepared_statement, statement);

    return rc;
}

ReturnCode Keystore::GetCertificates(std::string& certificates, const CertificateRole role)
{
    // In order to hide the internals of the Keystore we will
    // make a tmp file, store the certificates in it and return that
    // to the client. We also never store chain files.
    // Possibly things could be speed up by simply copying the certificate
    // if it is already stored as file, or even returning path into storage
    // but these should typically only be called when setting up a tls session.

    // TODO: is this secure enough? we have more control over permissions on keystore files

    ReturnCode rc = ReturnCode::kSuccess;

    std::string local_certificates_file_path;
    STACK_OF(X509)* local_certificates = sk_X509_new_null();

    if (!local_certificates)
    {
        rc = ReturnCode::kError;
    }

    if (rc == ReturnCode::kSuccess)
    {
        rc = GetCertificates(local_certificates, role);
    }

    if (rc == ReturnCode::kSuccess)
    {
        FILE* file = CreateTemporaryFile(local_certificates_file_path);

        if (file)
        {
            int num_certificates = sk_X509_num(local_certificates);

            for (int i = 0; i < num_certificates; i++)
            {
                X509* local_certificate = sk_X509_value(local_certificates, i);

                if (PEM_write_X509(file, local_certificate) != 1)
                {
                    DLT_LOG_STRINGF(dlt_libfsm_keystore, DLT_LOG_ERROR,
                                    "Failed to populate temporary file: %s.",
                                    local_certificates_file_path.c_str());

                    rc = ReturnCode::kError;
                }
                X509_free(local_certificate);
            }

            fclose(file);

            sk_X509_free(local_certificates);

            if (rc == ReturnCode::kSuccess)
            {
                certificates.assign(local_certificates_file_path);
            }
        }
        else
        {
            DLT_LOG_STRINGF(dlt_libfsm_keystore, DLT_LOG_ERROR,
                            "Failed to open temporary file: %s.",
                            local_certificates_file_path.c_str());

            rc = ReturnCode::kError;
        }
    }

    return rc;
}

ReturnCode Keystore::GetKey(EVP_PKEY** key, const Keystore::KeyRole role)
{
    ReturnCode rc =  ReturnCode::kSuccess;

    sqlite3_stmt* prepared_statement = nullptr;

    std::string statement = "SELECT " ID_COLUMN_NAME ", " TYPE_COLUMN_NAME ", " \
                            REFERENCE_TYPE_COLUMN_NAME ", " REFERENCE_COLUMN_NAME ", " \
                            VALID_COLUMN_NAME " " \
                            "FROM " KEYSTORE_TABLE_NAME " "\
                            "WHERE " TYPE_COLUMN_NAME " = ?1 AND " VALID_COLUMN_NAME " = 1;";

    // we expect a nullptr or a valid pointer to nullptr.
    if (key && *key)
    {
        rc = ReturnCode::kInvalidArgument;
    }
    if (!IsKeyType(static_cast<int>(role)))
    {
        rc = ReturnCode::kInvalidArgument;
    }

    if (rc == ReturnCode::kSuccess)
    {
        rc = GetStatement(&prepared_statement, statement);
    }

    if (rc == ReturnCode::kSuccess)
    {
        if (sqlite3_bind_int(prepared_statement, 1, static_cast<int>(role)) != SQLITE_OK)
        {
            rc =  ReturnCode::kError;
        }
    }

    if (rc == ReturnCode::kSuccess)
    {
        rc = GetKey(key, prepared_statement);
    }

    ReturnStatement(prepared_statement, statement);

    return rc;
}

ReturnCode Keystore::GetKey(std::string& key, const KeyRole role)
{
    // In order to hide the internals of the Keystore we will
    // make a tmp file, store the key in it and return that
    // to the client.
    // Possibly things could be speed up by simply copying the key
    // if it is already stored as file, or even returning path into storage
    // but these should typically only be called when setting up a tls session.

    // TODO: this is not secure enough for private keys, temporary until clarified how to handle

    EVP_PKEY* local_key = nullptr;
    std::string local_key_file_path;
    ReturnCode rc = GetKey(&local_key, role);

    if (rc == ReturnCode::kSuccess)
    {
        FILE* file = CreateTemporaryFile(local_key_file_path);

        if (file)
        {
            if (PEM_write_PKCS8PrivateKey(file, local_key, nullptr, nullptr, 0, 0, nullptr) == 1)
            {
                key.assign(local_key_file_path);
            }
            else
            {
                DLT_LOG_STRINGF(dlt_libfsm_keystore, DLT_LOG_ERROR,
                                "Failed to prepare temporary file: %s.",
                                local_key_file_path.c_str());

                rc = ReturnCode::kError;
            }

            fclose(file);
        }
        else
        {
            DLT_LOG_STRINGF(dlt_libfsm_keystore, DLT_LOG_ERROR,
                            "Failed to open temporary file: %s.",
                            local_key_file_path.c_str());

            rc = ReturnCode::kError;
        }
    }

    return rc;
}

ReturnCode Keystore::GetKnownSignerCertificates(STACK_OF(X509)* certificates)
{
    ReturnCode rc =  ReturnCode::kSuccess;

    sqlite3_stmt* prepared_statement = nullptr;

    std::string statement = "SELECT " ID_COLUMN_NAME ", " TYPE_COLUMN_NAME ", " \
                            REFERENCE_TYPE_COLUMN_NAME ", " REFERENCE_COLUMN_NAME ", " \
                            VALID_COLUMN_NAME " FROM " KEYSTORE_TABLE_NAME " "\
                            "WHERE " VALID_COLUMN_NAME " = 1 AND " TYPE_COLUMN_NAME " = ?1;";

    if (rc == ReturnCode::kSuccess)
    {
        rc = GetStatement(&prepared_statement, statement);
    }

    if (rc == ReturnCode::kSuccess)
    {
        if (sqlite3_bind_int(prepared_statement,
                             1,
                             static_cast<int>(CertificateRole::kUserActor)) != SQLITE_OK)
        {
            rc =  ReturnCode::kError;
        }
    }

    if (rc == ReturnCode::kSuccess)
    {
        rc = GetCertificates(certificates, prepared_statement);
    }

    ReturnStatement(prepared_statement, statement);

    return rc;
}

ReturnCode Keystore::GetCertificateAuthorities(X509_STORE** ca_store)
{
    ReturnCode rc =  ReturnCode::kSuccess;

    sqlite3_stmt* prepared_statement = nullptr;

    STACK_OF(X509)* certificates = sk_X509_new_null();

    std::string statement = "SELECT " ID_COLUMN_NAME ", " TYPE_COLUMN_NAME ", " \
                            REFERENCE_TYPE_COLUMN_NAME ", " REFERENCE_COLUMN_NAME ", " \
                            VALID_COLUMN_NAME " FROM " KEYSTORE_TABLE_NAME " "\
                            "WHERE " TYPE_COLUMN_NAME " = ?1 " \
                            "OR " TYPE_COLUMN_NAME " = ?2 AND " VALID_COLUMN_NAME " = 1;";


    // we expect a nullptr or a valid pointer to nullptr.
    if (ca_store && *ca_store)
    {
        rc = ReturnCode::kInvalidArgument;
    }

    if (rc == ReturnCode::kSuccess)
    {
        rc = GetStatement(&prepared_statement, statement);
    }

    if (rc == ReturnCode::kSuccess)
    {
        if (sqlite3_bind_int(prepared_statement,
                             1,
                             static_cast<int>(CertificateRole::kVehicleCa)) != SQLITE_OK)
        {
            rc =  ReturnCode::kError;
        }
        if (sqlite3_bind_int(prepared_statement,
                             2,
                             static_cast<int>(CertificateRole::kCloudCa)) != SQLITE_OK)
        {
            rc =  ReturnCode::kError;
        }
    }

    if (rc == ReturnCode::kSuccess)
    {
        rc = GetCertificates(certificates, prepared_statement);
    }

    ReturnStatement(prepared_statement, statement);

    if (rc == ReturnCode::kSuccess)
    {
        rc = StoreCertificatesInStore(ca_store, certificates);
    }

    sk_X509_pop_free(certificates, X509_free);

    return rc;
}

ReturnCode Keystore::GetCertificateAuthority(X509_STORE** ca_store,
                                             const Keystore::CertificateRole role)
{
    ReturnCode rc =  ReturnCode::kSuccess;

    sqlite3_stmt* prepared_statement = nullptr;

    STACK_OF(X509)* certificates = sk_X509_new_null();

    std::string statement = "SELECT " ID_COLUMN_NAME ", " TYPE_COLUMN_NAME ", " \
                            REFERENCE_TYPE_COLUMN_NAME ", " REFERENCE_COLUMN_NAME ", " \
                            VALID_COLUMN_NAME " "                                       \
                            "FROM " KEYSTORE_TABLE_NAME " "\
                            "WHERE " TYPE_COLUMN_NAME " = ?1 AND " VALID_COLUMN_NAME " = 1;";

    // we expect a nullptr or a valid pointer to nullptr.
    if (ca_store && *ca_store)
    {
        rc = ReturnCode::kInvalidArgument;
    }

    if (rc == ReturnCode::kSuccess)
    {
        rc = GetStatement(&prepared_statement, statement);
    }

    if (rc == ReturnCode::kSuccess)
    {
        if (sqlite3_bind_int(prepared_statement, 1, static_cast<int>(role)) != SQLITE_OK)
        {
            rc =  ReturnCode::kError;
        }
    }

    if (rc == ReturnCode::kSuccess)
    {
        rc = GetCertificates(certificates, prepared_statement);
    }

    ReturnStatement(prepared_statement, statement);

    if (rc == ReturnCode::kSuccess)
    {
        rc = StoreCertificatesInStore(ca_store, certificates);
    }

    sk_X509_pop_free(certificates, X509_free);

    return rc;
}

ReturnCode Keystore::SetCertificate(CertificateId& id,
                                    X509** old_certificate,
                                    X509* certificate,
                                    const CertificateRole role)
{
    ReturnCode rc = ReturnCode::kSuccess;

    sqlite3_stmt* prepared_statement = nullptr;
    std::string statement = "SELECT " ID_COLUMN_NAME ", " TYPE_COLUMN_NAME ", " \
                            REFERENCE_TYPE_COLUMN_NAME ", " REFERENCE_COLUMN_NAME ", " \
                            VALID_COLUMN_NAME " FROM " KEYSTORE_TABLE_NAME " "\
                            "WHERE " TYPE_COLUMN_NAME " = ?1 AND " VALID_COLUMN_NAME " = 1;";

    CertificateId old_certificate_id = -1;

    // we expect a valid pointer to nullptr, or nullptr pointer.
    if (old_certificate && *old_certificate)
    {
        rc = ReturnCode::kInvalidArgument;
    }
    if (!certificate)
    {
        rc = ReturnCode::kInvalidArgument;
    }
    if (IsOneToMany(role))
    {
        rc =  ReturnCode::kInvalidArgument;
    }

    // First try to find exisiting certificate
    if (rc == ReturnCode::kSuccess)
    {
        rc = GetStatement(&prepared_statement, statement);
    }

    if (rc == ReturnCode::kSuccess)
    {
        if (sqlite3_bind_int(prepared_statement, 1, static_cast<int>(role)) != SQLITE_OK)
        {
            rc =  ReturnCode::kError;
        }
    }

    if (rc == ReturnCode::kSuccess)
    {
        rc = GetCertificate(old_certificate, prepared_statement, &old_certificate_id);
    }

    ReturnStatement(prepared_statement, statement);

    if (rc == ReturnCode::kNotFound)
    {
        // This is ok, simply means we are adding, reset code
        rc = ReturnCode::kSuccess;
    }

    // Add the certificate
    if (rc == ReturnCode::kSuccess)
    {
        rc = InsertCertificate(certificate, role);
    }

    if (rc == ReturnCode::kSuccess && old_certificate_id > -1)
    {
        //Invalidate old certificate
        rc = InvalidateRow(old_certificate_id);
    }

    // To avoid returning the wrong id if the same
    // cert has previously been added for another role
    // we use sqlite3_last_insert_rowid .
    // There may be some risk of concurrency issues...
    if (rc == ReturnCode::kSuccess)
    {
        id = sqlite3_last_insert_rowid(database_);
        if (id == 0)
        {
            rc = ReturnCode::kError;
        }
    }

    return rc;
}

ReturnCode Keystore::AddCertificate(CertificateId& id,
                                    X509* certificate,
                                    const Keystore::CertificateRole role)
{
    ReturnCode rc = ReturnCode::kSuccess;

    if (!certificate)
    {
        rc = ReturnCode::kInvalidArgument;
    }
    if (!IsOneToMany(role))
    {
        rc = ReturnCode::kInvalidArgument;
    }

    if (rc == ReturnCode::kSuccess)
    {
        rc = InsertCertificate(certificate, role);
    }

    // To avoid returning the wrong id if the same
    // cert has previously been added for another role
    // we use sqlite3_last_insert_rowid .
    // There may be some risk of concurrency issues...
    if (rc == ReturnCode::kSuccess)
    {
        id = sqlite3_last_insert_rowid(database_);
        if (id == 0)
        {
            rc = ReturnCode::kError;
        }
    }

    return rc;
}

ReturnCode Keystore::SetKey(KeyId& id,
                            EVP_PKEY** old_key,
                            EVP_PKEY* key,
                            const Keystore::KeyRole role)
{
    ReturnCode rc = ReturnCode::kSuccess;

    // First try to find exisiting key
    sqlite3_stmt* prepared_statement = nullptr;
    std::string statement = "SELECT " ID_COLUMN_NAME ", " TYPE_COLUMN_NAME ", " \
                            REFERENCE_TYPE_COLUMN_NAME ", " REFERENCE_COLUMN_NAME ", " \
                            VALID_COLUMN_NAME " FROM " KEYSTORE_TABLE_NAME " "\
                            "WHERE " TYPE_COLUMN_NAME " = ?1 AND " VALID_COLUMN_NAME " = 1;";

    KeyId old_key_id = -1;

    // we expect a valid pointer to nullptr, or nullptr pointer.
    if (old_key && *old_key)
    {
        rc = ReturnCode::kInvalidArgument;
    }
    if (!key)
    {
        rc = ReturnCode::kInvalidArgument;
    }
    if (!IsKeyType(static_cast<int>(role)))
    {
        rc = ReturnCode::kInvalidArgument;
    }

    if (rc == ReturnCode::kSuccess)
    {
        rc = GetStatement(&prepared_statement, statement);
    }

    if (rc == ReturnCode::kSuccess)
    {
        if (sqlite3_bind_int(prepared_statement, 1, static_cast<int>(role)) != SQLITE_OK)
        {
            rc =  ReturnCode::kError;
        }
    }

    if (rc == ReturnCode::kSuccess)
    {
        rc = GetKey(old_key, prepared_statement, &old_key_id);
    }

    ReturnStatement(prepared_statement, statement);

    if (rc == ReturnCode::kNotFound)
    {
        // This is ok, simply means we are adding, reset code
        rc = ReturnCode::kSuccess;
    }

    // Add the key
    if (rc == ReturnCode::kSuccess)
    {
        rc = InsertKey(key, role);
    }

    if (rc == ReturnCode::kSuccess && old_key_id > -1)
    {
        //Invalidate old key
        rc = InvalidateRow(old_key_id);
    }

    // As its unclear how to compare two private keys
    // we use sqlite3_last_insert_rowid .
    // There may be some risk of concurrency issues...
    if (rc == ReturnCode::kSuccess)
    {
        id = sqlite3_last_insert_rowid(database_);
        if (id == 0)
        {
            rc = ReturnCode::kError;
        }
    }

    return rc;
}

ReturnCode Keystore::UpdateCertificate(X509** old_certificate,
                                       CertificateId& new_id,
                                       X509* certificate,
                                       const CertificateId id)
{
    ReturnCode rc = ReturnCode::kSuccess;

    // First try to find exisiting certificate

    sqlite3_stmt* prepared_statement = nullptr;
    std::string statement = "SELECT " ID_COLUMN_NAME ", " TYPE_COLUMN_NAME ", " \
                            REFERENCE_TYPE_COLUMN_NAME ", " REFERENCE_COLUMN_NAME ", " \
                            VALID_COLUMN_NAME " FROM " KEYSTORE_TABLE_NAME " "\
                            "WHERE " ID_COLUMN_NAME " = ?1 AND " VALID_COLUMN_NAME " = 1;";

    CertificateId old_certificate_id = 0;
    CertificateRole old_certificate_type = CertificateRole::kUndefined;

    // we expect a valid pointer to nullptr, or nullptr pointer.
    if (old_certificate && *old_certificate)
    {
        rc = ReturnCode::kInvalidArgument;
    }
    if (!certificate)
    {
        rc = ReturnCode::kInvalidArgument;
    }

    if (rc == ReturnCode::kSuccess)
    {
        rc = GetStatement(&prepared_statement, statement);
    }

    if (rc == ReturnCode::kSuccess)
    {
        if (sqlite3_bind_int(prepared_statement, 1, static_cast<int>(id)) != SQLITE_OK)
        {
            rc =  ReturnCode::kError;
        }
    }

    if (rc == ReturnCode::kSuccess)
    {
        rc = GetCertificate(old_certificate,
                            prepared_statement,
                            &old_certificate_id,
                            &old_certificate_type);
    }

    ReturnStatement(prepared_statement, statement);

    // Add the certificate
    if (rc == ReturnCode::kSuccess)
    {
        rc = InsertCertificate(certificate, old_certificate_type);
    }

    // Invalidate old certificate
    if (rc == ReturnCode::kSuccess)
    {
        rc = InvalidateRow(old_certificate_id);
    }

    // Finally get the id of the new certificate
    // This could probably be done much faster with
    // sqlite3_last_insert_rowid but this seems safer
    if (rc == ReturnCode::kSuccess)
    {
        rc = GetCertificateId(new_id, certificate);
    }

    return rc;
}

ReturnCode Keystore::DeleteCertificate(X509** old_certificate, const CertificateId id)
{
    ReturnCode rc = ReturnCode::kSuccess;

    // First try to find exisiting certificate

    sqlite3_stmt* prepared_statement = nullptr;
    std::string statement = "SELECT " ID_COLUMN_NAME ", " TYPE_COLUMN_NAME ", " \
                            REFERENCE_TYPE_COLUMN_NAME ", " REFERENCE_COLUMN_NAME ", " \
                            VALID_COLUMN_NAME " FROM " KEYSTORE_TABLE_NAME " "\
                            "WHERE " ID_COLUMN_NAME " = ?1;";

    CertificateId old_certificate_id = 0;

    // we expect a valid pointer to nullptr, or nullptr pointer.
    if (old_certificate && *old_certificate)
    {
        rc = ReturnCode::kInvalidArgument;
    }

    if (rc == ReturnCode::kSuccess)
    {
        rc = GetStatement(&prepared_statement, statement);
    }

    if (rc == ReturnCode::kSuccess)
    {
        if (sqlite3_bind_int(prepared_statement, 1, static_cast<int>(id)) != SQLITE_OK)
        {
            rc =  ReturnCode::kError;
        }
    }

    if (rc == ReturnCode::kSuccess)
    {
        rc = GetCertificate(old_certificate,
                            prepared_statement,
                            &old_certificate_id);
    }

    ReturnStatement(prepared_statement, statement);

    // Delete old certificate
    if (rc == ReturnCode::kSuccess)
    {
        rc = DeleteRow(old_certificate_id);
        RemoveFromCertificateCache(old_certificate_id);
    }

    return rc;
}

ReturnCode Keystore::DeleteCertificate(X509** old_certificate, const CertificateRole role)
{
    ReturnCode rc = ReturnCode::kSuccess;

    // First try to find exisiting certificate

    sqlite3_stmt* prepared_statement = nullptr;
    std::string statement = "SELECT " ID_COLUMN_NAME ", " TYPE_COLUMN_NAME ", " \
                            REFERENCE_TYPE_COLUMN_NAME ", " REFERENCE_COLUMN_NAME ", " \
                            VALID_COLUMN_NAME " FROM " KEYSTORE_TABLE_NAME " "\
                            "WHERE " TYPE_COLUMN_NAME " = ?1 AND " VALID_COLUMN_NAME " = 1;";

    CertificateId old_certificate_id = 0;

    // we expect a valid pointer to nullptr, or nullptr pointer.
    if (old_certificate && *old_certificate)
    {
        rc = ReturnCode::kInvalidArgument;
    }
    if (IsOneToMany(role))
    {
        rc = ReturnCode::kInvalidArgument;
    }

    if (rc == ReturnCode::kSuccess)
    {
        rc = GetStatement(&prepared_statement, statement);
    }

    if (rc == ReturnCode::kSuccess)
    {
        if (sqlite3_bind_int(prepared_statement, 1, static_cast<int>(role)) != SQLITE_OK)
        {
            rc =  ReturnCode::kError;
        }
    }

    if (rc == ReturnCode::kSuccess)
    {
        rc = GetCertificate(old_certificate,
                            prepared_statement,
                            &old_certificate_id);
    }

    ReturnStatement(prepared_statement, statement);

    // Delete old certificate
    if (rc == ReturnCode::kSuccess)
    {
        rc = DeleteRow(old_certificate_id);
        RemoveFromCertificateCache(old_certificate_id);
    }

    return rc;
}

ReturnCode Keystore::DeleteCertificates(STACK_OF(X509)* old_certificates, const CertificateRole role)
{
    ReturnCode rc = ReturnCode::kSuccess;

    bool deleted_atleast_one = false;

    sqlite3_stmt* prepared_statement = nullptr;
    std::string statement = "SELECT " ID_COLUMN_NAME ", " TYPE_COLUMN_NAME ", " \
                            REFERENCE_TYPE_COLUMN_NAME ", " REFERENCE_COLUMN_NAME ", " \
                            VALID_COLUMN_NAME " FROM " KEYSTORE_TABLE_NAME " "\
                            "WHERE " TYPE_COLUMN_NAME " = ?1 AND " VALID_COLUMN_NAME " = 1;";

    if (!IsOneToMany(role))
    {
        rc =  ReturnCode::kInvalidArgument;
    }

    if (rc == ReturnCode::kSuccess)
    {
        rc = GetStatement(&prepared_statement, statement);
    }

    if (rc == ReturnCode::kSuccess)
    {
        if (sqlite3_bind_int(prepared_statement, 1, static_cast<int>(role)) != SQLITE_OK)
        {
            rc =  ReturnCode::kError;
        }
    }

    while (rc == ReturnCode::kSuccess)
    {
        CertificateId old_certificate_id = 0;
        X509* old_certificate = nullptr;

        rc = GetCertificate(&old_certificate,
                            prepared_statement,
                            &old_certificate_id);

        if (rc == ReturnCode::kSuccess)
        {
            rc = DeleteRow(old_certificate_id);
            RemoveFromCertificateCache(old_certificate_id);
        }

        if (rc == ReturnCode::kSuccess)
        {
            deleted_atleast_one = true;
            if (old_certificates)
            {
                sk_X509_push(old_certificates, old_certificate);
            }
            else if (old_certificate)
            {
                X509_free(old_certificate);
            }
        }
    }

    if (rc == ReturnCode::kNotFound && deleted_atleast_one)
    {
        rc = ReturnCode::kSuccess;
    }

    ReturnStatement(prepared_statement, statement);

    return rc;
}

ReturnCode Keystore::DeleteKey(EVP_PKEY** old_key, const KeyId id)
{
    ReturnCode rc = ReturnCode::kSuccess;

    // First try to find exisiting certificate

    sqlite3_stmt* prepared_statement = nullptr;
    std::string statement = "SELECT " ID_COLUMN_NAME ", " TYPE_COLUMN_NAME ", " \
                            REFERENCE_TYPE_COLUMN_NAME ", " REFERENCE_COLUMN_NAME ", " \
                            VALID_COLUMN_NAME " FROM " KEYSTORE_TABLE_NAME " "\
                            "WHERE " ID_COLUMN_NAME " = ?1;";

    KeyId old_key_id = 0;

    // we expect a valid pointer to nullptr, or nullptr pointer.
    if (old_key && *old_key)
    {
        rc = ReturnCode::kInvalidArgument;
    }

    if (rc == ReturnCode::kSuccess)
    {
        rc = GetStatement(&prepared_statement, statement);
    }

    if (rc == ReturnCode::kSuccess)
    {
        if (sqlite3_bind_int(prepared_statement, 1, static_cast<int>(id)) != SQLITE_OK)
        {
            rc =  ReturnCode::kError;
        }
    }

    if (rc == ReturnCode::kSuccess)
    {
        rc = GetKey(old_key,
                    prepared_statement,
                    &old_key_id);
    }

    ReturnStatement(prepared_statement, statement);

    // Delete old certificate
    if (rc == ReturnCode::kSuccess)
    {
        rc = DeleteRow(old_key_id);
        RemoveFromKeyCache(old_key_id);
    }

    return rc;
}

ReturnCode Keystore::DeleteKey(EVP_PKEY** old_key, const Keystore::KeyRole role)
{
    ReturnCode rc = ReturnCode::kSuccess;

    // First try to find exisiting key
    sqlite3_stmt* prepared_statement = nullptr;
    std::string statement = "SELECT " ID_COLUMN_NAME ", " TYPE_COLUMN_NAME ", " \
                            REFERENCE_TYPE_COLUMN_NAME ", " REFERENCE_COLUMN_NAME ", " \
                            VALID_COLUMN_NAME " FROM " KEYSTORE_TABLE_NAME " "\
                            "WHERE " TYPE_COLUMN_NAME " = ?1 AND " VALID_COLUMN_NAME " = 1;";

    KeyId old_key_id = 0;

    // we expect a valid pointer to nullptr, or nullptr pointer.
    if (old_key && *old_key)
    {
        rc = ReturnCode::kInvalidArgument;
    }
    if (!IsKeyType(static_cast<int>(role)))
    {
        rc = ReturnCode::kInvalidArgument;
    }

    if (rc == ReturnCode::kSuccess)
    {
        rc = GetStatement(&prepared_statement, statement);
    }

    if (rc == ReturnCode::kSuccess)
    {
        if (sqlite3_bind_int(prepared_statement, 1, static_cast<int>(role)) != SQLITE_OK)
        {
            rc =  ReturnCode::kError;
        }
    }

    if (rc == ReturnCode::kSuccess)
    {
        rc = GetKey(old_key,
                    prepared_statement,
                    &old_key_id);
    }

    ReturnStatement(prepared_statement, statement);

    // Delete old certificate
    if (rc == ReturnCode::kSuccess)
    {
        rc = DeleteRow(old_key_id);
        RemoveFromKeyCache(old_key_id);
    }

    return rc;
}

ReturnCode Keystore::GetCertificateId(CertificateId& id, const X509* certificate)
{
    ReturnCode rc =  ReturnCode::kSuccess;

    sqlite3_stmt* prepared_statement = nullptr;

    std::string statement = "SELECT " ID_COLUMN_NAME ", " TYPE_COLUMN_NAME ", " \
                            REFERENCE_TYPE_COLUMN_NAME ", " REFERENCE_COLUMN_NAME ", " \
                            VALID_COLUMN_NAME " FROM " KEYSTORE_TABLE_NAME " " \
                            "WHERE " VALID_COLUMN_NAME " = 1 AND " TYPE_COLUMN_NAME " BETWEEN ?1 AND ?2;";

    // we expect a valid pointer.
    if (!certificate)
    {
        rc = ReturnCode::kInvalidArgument;
    }

    if (rc == ReturnCode::kSuccess)
    {
        rc = GetStatement(&prepared_statement, statement);
    }

    if (rc == ReturnCode::kSuccess)
    {
        if (sqlite3_bind_int(prepared_statement, 1, static_cast<int>(CertificateRole::kUndefined))
            != SQLITE_OK)
        {
            rc =  ReturnCode::kError;
        }
    }
    if (rc == ReturnCode::kSuccess)
    {
        if (sqlite3_bind_int(prepared_statement, 2, static_cast<int>(CertificateRole::kEndMarker))
            != SQLITE_OK)
        {
            rc =  ReturnCode::kError;
        }
    }

    if (rc == ReturnCode::kSuccess)
    {
        bool found = false;

        while (rc == ReturnCode::kSuccess)
        {
            CertificateId candidate_id = 0;
            X509* candidate = nullptr;

            rc = GetCertificate(&candidate, prepared_statement, &candidate_id);

            if (rc == ReturnCode::kSuccess)
            {
                if (X509_cmp(candidate, certificate) == 0)
                {
                    found = true;
                    id = candidate_id;
                    break;
                }
                X509_free(candidate);
            }
        }

        if (!found)
        {
            rc = ReturnCode::kNotFound;
        }
    }

    ReturnStatement(prepared_statement, statement);

    return rc;
}

ReturnCode Keystore::GetStatement(sqlite3_stmt** prepared_statement,
                                  const std::string& statement)
{

    ReturnCode rc = ReturnCode::kSuccess;

    if (!GetFromStatementCache(statement, prepared_statement))
    {
        if (!database_ || (sqlite3_prepare_v2(database_,
                                              statement.c_str(),
                                              -1, // Read above statement to null terminator
                                              prepared_statement,
                                              nullptr) != SQLITE_OK))
        {
            DLT_LOG_STRINGF(dlt_libfsm_keystore, DLT_LOG_ERROR,
                            "Failed to prepare statement, error: %s",
                            sqlite3_errmsg(database_));

            *prepared_statement = nullptr;
            rc = ReturnCode::kError;
        }
    }
    return rc;
}

void Keystore::ReturnStatement(sqlite3_stmt* prepared_statement, const std::string& statement)
{
    // For convinience we quitely ignore nullptr here
    // better to call it once to often...
    if (prepared_statement)
    {
        sqlite3_clear_bindings(prepared_statement);
        sqlite3_reset(prepared_statement);
        AddToStatementCache(statement, prepared_statement);
    }
}

ReturnCode Keystore::GetRow(const void** reference,
                            int& num_bytes,
                            StorageType& reference_type,
                            int& type,
                            CertificateId& id,
                            sqlite3_stmt* statement)
{
    ReturnCode rc = ReturnCode::kSuccess;
    bool done = false;

    if (!database_)
    {
        rc = ReturnCode::kError;
    }

    while (!done && rc == ReturnCode::kSuccess)
    {
        int status = sqlite3_step(statement);

        switch (status)
        {
        case SQLITE_ROW :
        {
            id = sqlite3_column_int(statement, 0);
            type = sqlite3_column_int(statement, 1);
            reference_type = static_cast<StorageType>(sqlite3_column_int(statement, 2));
            *reference = sqlite3_column_blob(statement, 3);
            num_bytes = sqlite3_column_bytes(statement, 3);

            done = true;
            rc = ReturnCode::kSuccess;

            break;
        }
        case SQLITE_BUSY :
        {
            // We use a BUSY timout handler, so this means it timed out.
            *reference = nullptr;
            num_bytes = 0;
            reference_type = StorageType::kUnknown;
            type = 0;
            id = 0;

            done = true;
            rc = ReturnCode::kError;

            DLT_LOG_STRING(dlt_libfsm_keystore, DLT_LOG_ERROR, "Keystore db busy timedout.");

            break;
        }
        case SQLITE_DONE :
        {
            *reference = nullptr;
            num_bytes = 0;
            reference_type = StorageType::kUnknown;
            type = 0;
            id = 0;

            done = true;
            rc = ReturnCode::kNotFound;

            DLT_LOG_STRING(dlt_libfsm_keystore, DLT_LOG_WARN, "Failed to find reference.");

            break;
        }
        default:
        {
            // some sort of error occured
            *reference = nullptr;
            num_bytes = 0;
            reference_type = StorageType::kUnknown;
            type = 0;
            id = 0;

            done = true;
            rc = ReturnCode::kError;

            DLT_LOG_STRINGF(dlt_libfsm_keystore, DLT_LOG_ERROR,
                            "Error occured while trying to find reference, error: %s.",
                            sqlite3_errmsg(database_));
        }
        }
    }

    return rc;
}

ReturnCode Keystore::LoadCertificate(X509** certificate,
                                     StorageType type,
                                     const void* reference,
                                     int num_bytes,
                                     CertificateId id)
{
    ReturnCode rc = ReturnCode::kSuccess;

    if (!GetFromCertificateCache(id, certificate))
    {
        X509* local_certificate = nullptr;

        switch (type)
        {
        case StorageType::kInTable :
        {
            BIO* reference_bio = BIO_new_mem_buf(reference, num_bytes);
            BIO_set_close(reference_bio, BIO_NOCLOSE); // buffer is owned by database

            if (!PEM_read_bio_X509(reference_bio, &local_certificate, 0, nullptr))
            {
                rc = ReturnCode::kError;

                char* error_buffer = ERR_error_string(ERR_get_error(), nullptr);
                DLT_LOG_STRINGF(dlt_libfsm_keystore, DLT_LOG_ERROR,
                                "Error occured while loading certificate: %s.", error_buffer);
            }

            BIO_free(reference_bio);
            break;
        }
        case StorageType::kFile :
        {
            FILE* file = fopen(static_cast<const char*>(reference), "r");

            if (file)
            {
                if (!PEM_read_X509(file, &local_certificate, 0, nullptr))
                {
                    rc = ReturnCode::kError;

                    char* error_buffer = ERR_error_string(ERR_get_error(), nullptr);
                    DLT_LOG_STRINGF(dlt_libfsm_keystore, DLT_LOG_ERROR,
                                    "Error occured while loading certificate: %s.", error_buffer);
                }

                fclose(file);
            }
            else
            {
                rc = ReturnCode::kError;

                DLT_LOG_STRINGF(dlt_libfsm_keystore, DLT_LOG_ERROR,
                                "Error occured while loading certificate file: %s, error: %s.",
                                static_cast<const char*>(reference), strerror(errno));
            }

            break;
        }
        default :
            rc = ReturnCode::kError;

            DLT_LOG_STRING(dlt_libfsm_keystore, DLT_LOG_ERROR,
                           "LoadCertificate: Received unknown reference type.");
        }

        if (certificate && rc == ReturnCode::kSuccess)
        {
            AddToCertificateCache(id, local_certificate);
            *certificate = local_certificate;
        }
        else
        {
            X509_free(local_certificate);
        }

        if (certificate && rc == ReturnCode::kError)
        {
            *certificate = nullptr;
        }
    }

    return rc;
}

ReturnCode Keystore::LoadKey(EVP_PKEY** key,
                             StorageType type,
                             const void* reference,
                             int num_bytes,
                             KeyId id)
{
    ReturnCode rc = ReturnCode::kSuccess;


    if (!GetFromKeyCache(id, key))
    {
        EVP_PKEY* local_key = nullptr;

        switch (type)
        {
        case StorageType::kInTable :
        {
            BIO* reference_bio = BIO_new_mem_buf(reference, num_bytes);
            BIO_set_close(reference_bio, BIO_NOCLOSE); // buffer is owned by database

            if (!PEM_read_bio_PrivateKey(reference_bio, &local_key, 0, nullptr))
            {
                rc = ReturnCode::kError;

                char* error_buffer = ERR_error_string(ERR_get_error(), nullptr);
                DLT_LOG_STRINGF(dlt_libfsm_keystore, DLT_LOG_ERROR,
                                "Error occured while loading key: %s.", error_buffer);
            }

            BIO_free(reference_bio);
            break;
        }
        case StorageType::kFile :
        {
            FILE* file = fopen(static_cast<const char*>(reference), "r");

            if (file)
            {
                if (!PEM_read_PrivateKey(file, &local_key, 0, nullptr))
                {
                    rc = ReturnCode::kError;

                    char* error_buffer = ERR_error_string(ERR_get_error(), nullptr);
                    DLT_LOG_STRINGF(dlt_libfsm_keystore, DLT_LOG_ERROR,
                                    "Error occured while loading key %s.", error_buffer);
                }

                fclose(file);
            }
            else
            {
                rc = ReturnCode::kError;

                DLT_LOG_STRINGF(dlt_libfsm_keystore, DLT_LOG_ERROR,
                                "Error occured while loading key file: %s, error: %s.",
                                static_cast<const char*>(reference), strerror(errno));
            }

            break;
        }
        default :
            rc = ReturnCode::kError;

            DLT_LOG_STRING(dlt_libfsm_keystore, DLT_LOG_ERROR,
                           "LoadKey: Received unknown reference type.");
        }

        if (key && rc == ReturnCode::kSuccess)
        {
            AddToKeyCache(id, local_key);
            *key = local_key;
        }
        else
        {
            EVP_PKEY_free(local_key);
        }

        if (key && rc == ReturnCode::kError)
        {
            *key = nullptr;
        }
    }

    return rc;
}

ReturnCode Keystore::GetCertificate(X509** certificate,
                                    sqlite3_stmt* statement,
                                    CertificateId* id,
                                    CertificateRole* role)
{
    ReturnCode rc = ReturnCode::kSuccess;

    const void* reference;
    int num_bytes = 0;
    StorageType reference_type = StorageType::kUnknown;
    int local_type = 0;
    CertificateId local_id = 0;

    rc = GetRow(&reference, num_bytes, reference_type, local_type, local_id, statement);

    if (rc == ReturnCode::kSuccess)
    {
        if (!IsCertificateType(local_type))
        {
            rc = ReturnCode::kError;

            DLT_LOG_STRING(dlt_libfsm_keystore, DLT_LOG_WARN,
                           "GetCertificate: Received non-certificate reference type.");
        }
    }

    if (rc == ReturnCode::kSuccess)
    {
        if (id)
        {
            *id = local_id;
        }
        if (role)
        {
            *role = static_cast<CertificateRole>(local_type);
        }
        rc = LoadCertificate(certificate, reference_type, reference, num_bytes, local_id);
    }

    return rc;
}

ReturnCode Keystore::GetCertificates(STACK_OF(X509)* certificates,
                                     sqlite3_stmt* statement)
{
    ReturnCode rc = ReturnCode::kSuccess;

    bool found_atleast_one = false;

    while (rc == ReturnCode::kSuccess)
    {
        const void* reference;
        int num_bytes = 0;
        StorageType reference_type = StorageType::kUnknown;
        int type = 0;
        CertificateId id = 0;
        X509* certificate = nullptr;

        rc = GetRow(&reference, num_bytes, reference_type, type, id, statement);

        if (rc == ReturnCode::kSuccess)
        {
            if (!IsCertificateType(type))
            {
                rc = ReturnCode::kError;

                DLT_LOG_STRING(dlt_libfsm_keystore, DLT_LOG_WARN,
                               "GetCertificates: Received non-certificate reference type.");
            }
        }

        if (rc == ReturnCode::kSuccess)
        {
            rc = LoadCertificate(&certificate, reference_type, reference, num_bytes, id);
        }

        if (rc == ReturnCode::kSuccess)
        {
            if (certificates)
            {
                sk_X509_push(certificates, certificate);
            }
            found_atleast_one = true;
        }
    }

    if (rc == ReturnCode::kNotFound && found_atleast_one)
    {
        rc = ReturnCode::kSuccess;
    }

    return rc;
}

ReturnCode Keystore::GetKey(EVP_PKEY** key, sqlite3_stmt* statement, KeyId* id)
{
    ReturnCode rc = ReturnCode::kSuccess;

    const void* reference;
    int num_bytes = 0;
    StorageType reference_type = StorageType::kUnknown;
    int type = 0;
    KeyId local_id = 0;

    rc = GetRow(&reference, num_bytes, reference_type, type, local_id, statement);

    if (rc == ReturnCode::kSuccess)
    {
        if (!IsKeyType(type))
        {
            rc = ReturnCode::kError;

            DLT_LOG_STRING(dlt_libfsm_keystore, DLT_LOG_WARN,
                           "GetKey: Received non-key reference type.");
        }
    }

    if (rc == ReturnCode::kSuccess)
    {
        if (id)
        {
            *id = local_id;
        }
        rc = LoadKey(key, reference_type, reference, num_bytes, local_id);
    }

    return rc;
}

ReturnCode Keystore::StoreCertificatesInStore(X509_STORE** store,
                                              STACK_OF(X509)* certificates)
{
    ReturnCode rc = ReturnCode::kSuccess;

    if (store)
    {
        X509_STORE* local_store = X509_STORE_new();

        int num_certificates = sk_X509_num(certificates);

        for (int i = 0; i < num_certificates; i++)
        {
            X509* certificate = sk_X509_pop(certificates);
            if (!X509_STORE_add_cert(local_store, certificate))
            {
                rc = ReturnCode::kError;

                X509_STORE_free(local_store);
                local_store = nullptr;

                break;
            }
        }

        if (rc == ReturnCode::kSuccess)
        {
            *store = local_store;
        }
    }

    return rc;
}

ReturnCode Keystore::InvalidateRow(int row_id)
{
    ReturnCode rc = ReturnCode::kSuccess;

    sqlite3_stmt* prepared_statement = nullptr;
    std::string statement = "UPDATE " KEYSTORE_TABLE_NAME " "     \
                            "SET " VALID_COLUMN_NAME " = 0 " \
                            "WHERE " ID_COLUMN_NAME " = ?1;";

    if (!database_)
    {
        rc = ReturnCode::kError;
    }

    if (rc == ReturnCode::kSuccess)
    {
        rc = GetStatement(&prepared_statement, statement);
    }

    if (rc == ReturnCode::kSuccess)
    {
        if (sqlite3_bind_int(prepared_statement, 1, static_cast<int>(row_id)) !=
            SQLITE_OK)
        {
            rc =  ReturnCode::kError;
        }
    }

    if (rc == ReturnCode::kSuccess)
    {
        if (sqlite3_step(prepared_statement) != SQLITE_DONE)
        {
            DLT_LOG_STRINGF(dlt_libfsm_keystore, DLT_LOG_ERROR,
                            "Failed to invalidate existing entry, error: %s",
                            sqlite3_errmsg(database_));

            rc = ReturnCode::kError;
        }
    }

    ReturnStatement(prepared_statement, statement);

    return rc;
}

ReturnCode Keystore::DeleteRow(int row_id)
{
    ReturnCode rc = ReturnCode::kSuccess;

    if (!database_)
    {
        rc = ReturnCode::kError;
    }

    // First delete file if row credential stored as file

    sqlite3_stmt* prepared_statement = nullptr;
    std::string statement = "SELECT " ID_COLUMN_NAME ", " TYPE_COLUMN_NAME ", " \
                            REFERENCE_TYPE_COLUMN_NAME ", " REFERENCE_COLUMN_NAME ", " \
                            VALID_COLUMN_NAME " FROM " KEYSTORE_TABLE_NAME " WHERE " \
                            ID_COLUMN_NAME " = ?1 AND " VALID_COLUMN_NAME " = 1;";

    if (rc == ReturnCode::kSuccess)
    {
        rc = GetStatement(&prepared_statement, statement);
    }

    if (rc == ReturnCode::kSuccess)
    {
        if (sqlite3_bind_int(prepared_statement, 1, static_cast<int>(row_id)) !=
            SQLITE_OK)
        {
             rc =  ReturnCode::kError;
        }
    }

    const void* reference;
    int num_bytes = 0;
    StorageType reference_type = StorageType::kUnknown;
    int local_type = 0;
    CertificateId local_id = 0;

    if (rc == ReturnCode::kSuccess)
    {
        rc = GetRow(&reference, num_bytes, reference_type, local_type, local_id, prepared_statement);
    }

    if (rc == ReturnCode::kSuccess)
    {
        if (reference_type == StorageType::kFile)
        {
            if (unlink(static_cast<const char*>(reference)) == -1 &&
                errno != ENOENT)
            {
                DLT_LOG_STRINGF(dlt_libfsm_keystore, DLT_LOG_ERROR,
                                "Failed to remove file %s, error: %d",
                                static_cast<const char*>(reference), errno);

                rc =  ReturnCode::kError;
            }
        }
    }

    ReturnStatement(prepared_statement, statement);

    prepared_statement = nullptr;
    statement = "DELETE FROM " KEYSTORE_TABLE_NAME " " \
                "WHERE " ID_COLUMN_NAME " = ?1;";

    if (rc == ReturnCode::kSuccess)
    {
        rc = GetStatement(&prepared_statement, statement);
    }

    if (rc == ReturnCode::kSuccess)
    {
        if (sqlite3_bind_int(prepared_statement, 1, static_cast<int>(row_id)) !=
            SQLITE_OK)
        {
             rc =  ReturnCode::kError;
        }
    }

    if (rc == ReturnCode::kSuccess)
    {
        if (sqlite3_step(prepared_statement) != SQLITE_DONE)
        {
            DLT_LOG_STRINGF(dlt_libfsm_keystore, DLT_LOG_ERROR,
                            "Failed to delete existing entry, error: %s",
                            sqlite3_errmsg(database_));

            rc = ReturnCode::kError;
        }
    }

    ReturnStatement(prepared_statement, statement);

    return rc;
}

ReturnCode Keystore::InsertCertificate(X509* certificate,
                                       const CertificateRole role)
{
    ReturnCode rc = ReturnCode::kSuccess;

    // Don't insert into keystore if key size is too short
    rc = CheckCertificateKeySize(certificate, role);

    if (rc == ReturnCode::kSuccess)
    {
        if (IsStorageTypeAllowedForCertificateRole(StorageType::kInTable, role))
        {
            // First create PEM buffer of certificate
            BIO* reference_bio = BIO_new(BIO_s_mem());
            int num_reference_bytes = 0;
            void* reference_data = nullptr;

            if (!PEM_write_bio_X509(reference_bio, certificate))
            {
                char* error_buffer = ERR_error_string(ERR_get_error(), nullptr);
                DLT_LOG_STRINGF(dlt_libfsm_keystore, DLT_LOG_ERROR,
                                "Error occured while writing cert: %s.", error_buffer);

                rc = ReturnCode::kError;
            }

            if (rc == ReturnCode::kSuccess)
            {
                num_reference_bytes = BIO_get_mem_data(reference_bio, &reference_data);
            }

            if (rc == ReturnCode::kSuccess)
            {
                rc = InsertBlobEntry(reference_data, num_reference_bytes, static_cast<int>(role));
            }

            BIO_free(reference_bio);
        }
        else if (IsStorageTypeAllowedForCertificateRole(StorageType::kFile, role))
        {
            rc = InsertCertificateFile(certificate, role);
        }
        else
        {
            DLT_LOG_STRINGF(dlt_libfsm_keystore, DLT_LOG_ERROR,
                            "Failed to insert Certificate, "
                            "no allowed storage type applicable for role: %d:",
                            role);

            rc = ReturnCode::kError;
        }
    }
    else
    {
        DLT_LOG_STRING(dlt_libfsm_keystore, DLT_LOG_ERROR,
                       "Failed to insert Certificate, "
                       "key has unallowed size.");
    }

    return rc;
}

ReturnCode Keystore::InsertKey(EVP_PKEY* key,
                               const KeyRole role)
{
    ReturnCode rc = ReturnCode::kSuccess;

    // Don't insert into keystore if key size is too short
    rc = CheckKeySize(key, role);

    if (rc == ReturnCode::kSuccess)
    {
        if (IsStorageTypeAllowedForKeyRole(StorageType::kInTable, role))
        {
            // First create PEM buffer of certificate
            BIO* reference_bio = BIO_new(BIO_s_mem());
            int num_reference_bytes = 0;
            void* reference_data = nullptr;

            if (!PEM_write_bio_PKCS8PrivateKey(reference_bio, key, nullptr, nullptr, 0, 0, nullptr))
            {
                char* error_buffer = ERR_error_string(ERR_get_error(), nullptr);
                DLT_LOG_STRINGF(dlt_libfsm_keystore, DLT_LOG_ERROR,
                                "Error occured while writing key: %s.", error_buffer);

                rc = ReturnCode::kError;
            }

            if (rc == ReturnCode::kSuccess)
            {
                num_reference_bytes = BIO_get_mem_data(reference_bio, &reference_data);
            }

            if (rc == ReturnCode::kSuccess)
            {
                rc = InsertBlobEntry(reference_data, num_reference_bytes, static_cast<int>(role));
            }

            BIO_free(reference_bio);
        }
        else if (IsStorageTypeAllowedForKeyRole(StorageType::kFile, role))
        {
            rc = InsertKeyFile(key, role);
        }
        else
        {
            DLT_LOG_STRINGF(dlt_libfsm_keystore, DLT_LOG_ERROR,
                            "Failed to insert key, "
                            "no allowed storage type applicable to role: %d.",
                            role);

            rc = ReturnCode::kError;
        }
    }

    return rc;
}

FILE* Keystore::CreateTemporaryFile(std::string& path)
{

    FILE* file = nullptr;

    int path_length = 11;
    char file_path[path_length + 1] = "/tmp/XXXXXX";

    int fd = mkstemp(file_path); // will be closed when caller closes file

    if (fd != -1)
    {
        path.assign(file_path, path_length);
        file = fdopen(fd, "w");
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm_keystore, DLT_LOG_ERROR,
                        "Failed to prepare temporary file: %s.",
                        path.c_str());
    }

    return file;
}

FILE* Keystore::CreateFile(std::string& path, std::string relative_path, std::string filename)
{
    bool success = true;
    FILE* return_value = nullptr;

    char* persistance_path = nullptr;
    int path_size = 0;

    tpPCL_Error_t pcl_return = tpPCL_getWriteablePath(E_PCL_LDBID_PUBLIC,
                                                    &persistance_path,
                                                    &path_size);

    if (pcl_return != E_PCL_ERROR_NONE || !persistance_path || path_size == 0)
    {
        DLT_LOG_STRING(dlt_libfsm_keystore, DLT_LOG_ERROR, "failed to retrieve persistance path");
    }
    else
    {
        // Create the directories if they do not exist
        std::stringstream path_stream;
        path_stream << persistance_path;

        std::string::size_type pos = 0;
        while (pos != std::string::npos && success)
        {
            pos = relative_path.find('/');
            std::string token = relative_path.substr(0, pos);
            relative_path.erase(0, pos + 1);
            path_stream << token << '/';

            struct stat st = {0};

            if (stat(path_stream.str().c_str(), &st) == -1)
            {
                if (errno == ENOENT)
                {
                    if (mkdir(path_stream.str().c_str(),
                              S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP) == -1 && errno != EEXIST)
                    {
                        DLT_LOG_STRINGF(dlt_libfsm_keystore, DLT_LOG_ERROR,
                                        "failed to create subpath: %s.",
                                        path_stream.str().c_str());

                        success = false;
                    }
                    else
                    {
                        if (chmod(path_stream.str().c_str(),
                                  S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP) == -1)
                        {
                            DLT_LOG_STRINGF(dlt_libfsm_keystore, DLT_LOG_ERROR,
                                            "failed to chmod subpath: %s.",
                                            path_stream.str().c_str());

                            success = false;
                        }
                    }
                } // if (errno == ENOENT)
                else
                {
                    DLT_LOG_STRINGF(dlt_libfsm_keystore, DLT_LOG_ERROR,
                                    "failed to access subpath: %s.",
                                    path_stream.str().c_str());

                    success = false;
                }
            } // if (stat(path_stream.str().c_str(), &st) == -1)
        } // while (pos != std::string::npos && rc == ReturnCode::kSuccess)

        // Create the file
        if (success)
        {
            path_stream << "/" << filename;

            int fd = open(path_stream.str().c_str(),
                      O_CREAT | O_EXCL | O_WRONLY,
                      S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);

            if (fd != -1)
            {
                if (fchmod(fd, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP) == -1)
                {
                    DLT_LOG_STRINGF(dlt_libfsm_keystore, DLT_LOG_ERROR,
                                    "failed to chmod file: %s.",
                                    path_stream.str().c_str());
                }
                else
                {
                    return_value = fdopen(fd, "w");

                    path = path_stream.str();
                }
            }
            else
            {
                DLT_LOG_STRINGF(dlt_libfsm_keystore, DLT_LOG_ERROR,
                                "failed to open file: %s.",
                                path_stream.str().c_str());
            }
        }

    }

    return return_value;
}

ReturnCode Keystore::StoreCertificateInFile(std::string& path,
                                            X509* certificate,
                                            CertificateRole role)
{
    ReturnCode rc = ReturnCode::kSuccess;

    std::stringstream filename_stream;

    // We will use a hex representation of the SKI as the filename
    ASN1_OCTET_STRING *ski =
        reinterpret_cast<ASN1_OCTET_STRING*>(X509_get_ext_d2i(certificate,
                                                              NID_subject_key_identifier,
                                                              0,
                                                              0));

    if (ski)
    {
        const unsigned char *bytes = ASN1_STRING_data(ski);
        int num_bytes = ASN1_STRING_length(ski);

        if (bytes)
        {
            filename_stream << std::hex;
            for (int i = 0; i < num_bytes; i++)
            {
                filename_stream << static_cast<int>(bytes[i]);
            }
        }
        else
        {
            DLT_LOG_STRING(dlt_libfsm_keystore, DLT_LOG_ERROR, "failed to get SKI as bytes");

            rc = ReturnCode::kError;
        }

        ASN1_OCTET_STRING_free(ski);
    }
    else
    {
        DLT_LOG_STRING(dlt_libfsm_keystore, DLT_LOG_ERROR, "failed to get SKI from certificate");

        rc = ReturnCode::kError;
    }

    if (rc == ReturnCode::kSuccess)
    {
        FILE* file = CreateFile(path,
                                kStoragePathForCertificateRole_.at(role),
                                filename_stream.str());

        if (file)
        {
            if (0 == PEM_write_X509(file, certificate))
            {
                DLT_LOG_STRING(dlt_libfsm_keystore, DLT_LOG_ERROR,
                               "failed to write certificate PEM to file");

                rc = ReturnCode::kError;
            }

            fclose(file);
        }
        else
        {
            DLT_LOG_STRING(dlt_libfsm_keystore, DLT_LOG_ERROR,
                           "failed to create file for certificate");

            rc = ReturnCode::kError;
        }
    }

    return rc;
}

ReturnCode Keystore::StoreKeyInFile(std::string& path,
                                    EVP_PKEY* key,
                                    KeyRole role)
{
    ReturnCode rc = ReturnCode::kSuccess;

    std::stringstream filename_stream;

    // We will use some random bytes for key filenames

    const int kNumBytes = 16;
    unsigned char bytes[kNumBytes];
    if (!RAND_bytes(bytes, kNumBytes))
    {
        char* error_buffer = ERR_error_string(ERR_get_error(), NULL);
        DLT_LOG_STRINGF(dlt_libfsm_keystore, DLT_LOG_ERROR, "Failed to generate key filename, error: %s",
                        error_buffer);

        rc = ReturnCode::kError;
    }
    else
    {
        filename_stream << std::hex;
        for (int i = 0; i < kNumBytes; i++)
        {
            filename_stream << static_cast<int>(bytes[i]);
        }
    }

    if (rc == ReturnCode::kSuccess)
    {
        FILE* file = CreateFile(path,
                                kStoragePathForKeyRole_.at(role),
                                filename_stream.str());

        if (file)
        {

            if (0 == PEM_write_PKCS8PrivateKey(file, key, nullptr, nullptr, 0, 0, nullptr))
            {
                DLT_LOG_STRING(dlt_libfsm_keystore, DLT_LOG_ERROR,
                               "failed to write key PEM to file");

                rc = ReturnCode::kError;
            }

            fclose(file);
        }
        else
        {
            DLT_LOG_STRING(dlt_libfsm_keystore, DLT_LOG_ERROR,
                           "failed to create file for key");

            rc = ReturnCode::kError;
        }
    }

    return rc;
}

ReturnCode Keystore::InsertBlobEntry(const void* blob_data,
                                     int blob_data_size,
                                     int type,
                                     StorageType storage_type)
{
    ReturnCode rc = ReturnCode::kSuccess;

    sqlite3_stmt* prepared_statement = nullptr;
    std::string statement = "INSERT INTO " KEYSTORE_TABLE_NAME "(" \
                            TYPE_COLUMN_NAME ", " REFERENCE_TYPE_COLUMN_NAME ", " \
                            REFERENCE_COLUMN_NAME ", " VALID_COLUMN_NAME ")" \
                            "VALUES (?1, ?2, ?3, 1);";

    if (!database_)
    {
        rc = ReturnCode::kError;
    }

    if (rc == ReturnCode::kSuccess)
    {
        rc = GetStatement(&prepared_statement, statement);
    }

    if (rc == ReturnCode::kSuccess && (sqlite3_bind_int(prepared_statement,
                                                        1,
                                                        type)
                                       != SQLITE_OK))
    {
        rc =  ReturnCode::kError;
    }
    if (rc == ReturnCode::kSuccess && (sqlite3_bind_int(prepared_statement,
                                                        2,
                                                        static_cast<int>(storage_type))
                                       != SQLITE_OK))
    {
        rc =  ReturnCode::kError;
    }
    if (rc == ReturnCode::kSuccess && (sqlite3_bind_blob(prepared_statement,
                                                         3,
                                                         blob_data,
                                                         blob_data_size,
                                                         SQLITE_TRANSIENT) //safe, could maybe be avoided
                                       != SQLITE_OK))
    {
        rc =  ReturnCode::kError;
    }

    if (rc == ReturnCode::kSuccess)
    {
        if (sqlite3_step(prepared_statement) != SQLITE_DONE)
        {
            DLT_LOG_STRINGF(dlt_libfsm_keystore, DLT_LOG_ERROR,
                            "Failed to insert entry, error: %s",
                            sqlite3_errmsg(database_));

            rc = ReturnCode::kError;
        }
    }

    ReturnStatement(prepared_statement, statement);

    return rc;
}

ReturnCode Keystore::InsertCertificateFile(X509* certificate, CertificateRole role)
{
    ReturnCode rc = ReturnCode::kSuccess;

    std::string path;

    rc = StoreCertificateInFile(path, certificate, role);

    if (rc == ReturnCode::kSuccess)
    {
        const void* blob_data = static_cast<const void*>(path.c_str());
        int blob_data_size = path.length() + 1;

        rc = InsertBlobEntry(blob_data, blob_data_size, static_cast<int>(role), StorageType::kFile);
    }

    return rc;
}

ReturnCode Keystore::InsertKeyFile(EVP_PKEY* key, KeyRole role)
{
    ReturnCode rc = ReturnCode::kSuccess;

    std::string path;

    rc = StoreKeyInFile(path, key, role);

    if (rc == ReturnCode::kSuccess)
    {
        const void* blob_data = static_cast<const void*>(path.c_str());
        int blob_data_size = path.length() + 1;

        rc = InsertBlobEntry(blob_data, blob_data_size, static_cast<int>(role), StorageType::kFile);
    }

    return rc;
}

bool Keystore::IsCertificateType(int type)
{
    return (type > static_cast<int>(CertificateRole::kUndefined)) &&
        (type < static_cast<int>(CertificateRole::kEndMarker));
}

bool Keystore::IsKeyType(int type)
{
    return (type > static_cast<int>(KeyRole::kUndefined)) &&
        (type < static_cast<int>(KeyRole::kEndMarker));
}

bool Keystore::IsOneToMany(const CertificateRole role)
{
    return role == CertificateRole::kUserActor ||
           role == CertificateRole::kUserShortRangeTlsClient ||
           role == CertificateRole::kVehicleCa ||
           role == CertificateRole::kCloudCa;
}

void Keystore::RemoveFromCertificateCache(const CertificateId id)
{
    std::lock_guard<std::mutex> lock(certificate_cache_mutex_);

    auto element = certificate_cache_.find(id);
    if(element != certificate_cache_.end())
    {
        X509_free(element->second);
    }

    certificate_cache_.erase(id);
}

void Keystore::AddToCertificateCache(const CertificateId id, X509* certificate)
{
    std::lock_guard<std::mutex> lock(certificate_cache_mutex_);

    // We have one ref for cache and one for each client we give the certificate to.
    // Here we add the ref for the cache.
    X509_up_ref(certificate);
    certificate_cache_[id] = certificate;
}

bool Keystore::GetFromCertificateCache(const CertificateId id, X509** certificate)
{
    bool return_value = false;

    // We may get a null pointer going all teh way into here
    // if client was for instance deleting and not interested
    // in the old entry, quitely ignore.
    if (certificate)
    {
        std::lock_guard<std::mutex> lock(certificate_cache_mutex_);

        auto element = certificate_cache_.find(id);
        if(element != certificate_cache_.end())
        {
             // We have one ref for cache and one for each time we give it to the client.
            // Here we add the ref for the client.
            *certificate = element->second;
            X509_up_ref(*certificate);

            return_value = true;
        }
    }

    return return_value;
}

void Keystore::RemoveFromKeyCache(const KeyId id)
{
    std::lock_guard<std::mutex> lock(key_cache_mutex_);

    auto element = key_cache_.find(id);
    if(element != key_cache_.end())
    {
        EVP_PKEY_free(element->second);
    }

    key_cache_.erase(id);
}

void Keystore::AddToKeyCache(const KeyId id, EVP_PKEY* key)
{
    std::lock_guard<std::mutex> lock(key_cache_mutex_);

    // We have one ref for cache and one for each client we give the certificate to.
    // Here we add the ref for the cache.
    EVP_PKEY_up_ref(key);
    key_cache_[id] = key;
}

bool Keystore::GetFromKeyCache(const KeyId id, EVP_PKEY** key)
{
    bool return_value = false;

    // We may get a null pointer going all teh way into here
    // if client was for instance deleting and not interested
    // in the old entry, quitely ignore.
    if (key)
    {
        std::lock_guard<std::mutex> lock(key_cache_mutex_);

        auto element = key_cache_.find(id);
        if(element != key_cache_.end())
        {
            // We have one ref for cache and one for each time we give it to the client.
            // Here we add the ref for the client.
            *key = element->second;
            EVP_PKEY_up_ref(*key);

            return_value = true;
        }
    }

    return return_value;
}

void Keystore::AddToStatementCache(const std::string& statement, sqlite3_stmt* prepared_statement)
{
    std::lock_guard<std::mutex> lock(statement_cache_mutex_);

    std::stack<sqlite3_stmt*>& statement_stack = statement_cache_[statement];
    statement_stack.push(prepared_statement);
}

bool Keystore::GetFromStatementCache(const std::string& statement, sqlite3_stmt** prepared_statement)
{
    bool return_value = false;

    std::lock_guard<std::mutex> lock(statement_cache_mutex_);

    auto element = statement_cache_.find(statement);
    if(element != statement_cache_.end() && !element->second.empty())
    {
        *prepared_statement = element->second.top();
        element->second.pop();

        return_value = true;
    }

    return return_value;
}

ReturnCode Keystore::CheckKeySize(EVP_PKEY *key, const KeyRole role)
{
    ReturnCode rc = ReturnCode::kInvalidArgument;

    int key_size = EVP_PKEY_bits(key);

    int min_key_size = 0;

    switch (role)
    {
        case KeyRole::kVehicleActor:
        case KeyRole::kVehicleShortRangeTlsServer:
        case KeyRole::kCloudTlsClient:
        case KeyRole::kDoIpTlsServer:
        case KeyRole::kCloudSignEncrypt:
            if (EVP_PKEY_EC == EVP_PKEY_id(key))
            {
                min_key_size = kMinRegularECKeySize;
            }
            else if (EVP_PKEY_RSA == EVP_PKEY_id(key))
            {
                min_key_size = kMinRSAKeySize;
            }
            break;
        case KeyRole::kVehicleCa:
            if (EVP_PKEY_EC == EVP_PKEY_id(key))
            {
                min_key_size = kMinRootECKeySize;
            }
            else if (EVP_PKEY_RSA == EVP_PKEY_id(key))
            {
                min_key_size = kMinRSAKeySize;
            }
            break;
        default:
            {
                DLT_LOG_STRINGF(dlt_libfsm_keystore, DLT_LOG_WARN,
                        "Invalid key role %d", role);
            }
            break;
    }


    if (min_key_size && key_size >= min_key_size)
    {
        rc = ReturnCode::kSuccess;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm_keystore, DLT_LOG_ERROR,
                "Not storing in Keystore bacause of too short key size: %d,"
                " should be minimum %d", key_size, min_key_size);
    }

    return rc;
}

ReturnCode Keystore::CheckCertificateKeySize(X509 *certificate, const CertificateRole role)
{
    ReturnCode rc = ReturnCode::kInvalidArgument;

    EVP_PKEY *key = X509_get_pubkey(certificate);

    int key_size = EVP_PKEY_bits(key);

    int min_key_size = 0;

    switch (role)
    {
        case CertificateRole::kVehicleActor:
        case CertificateRole::kVehicleShortRangeTlsServer:
        case CertificateRole::kUserActor:
        case CertificateRole::kUserShortRangeTlsClient:
        case CertificateRole::kCloudTlsClient:
        case CertificateRole::kDoIpTlsServer:
        case CertificateRole::kCloudSignEncrypt:
            if (EVP_PKEY_EC == EVP_PKEY_id(key))
            {
                min_key_size = kMinRegularECKeySize;
            }
            else if (EVP_PKEY_RSA == EVP_PKEY_id(key))
            {
                min_key_size = kMinRSAKeySize;
            }
            break;
        case CertificateRole::kVehicleCa:
        case CertificateRole::kCloudCa:
            if (EVP_PKEY_EC == EVP_PKEY_id(key))
            {
                min_key_size = kMinRootECKeySize;
            }
            else if (EVP_PKEY_RSA == EVP_PKEY_id(key))
            {
                min_key_size = kMinRSAKeySize;
            }
            break;
        default:
            {
                DLT_LOG_STRINGF(dlt_libfsm_keystore, DLT_LOG_WARN,
                        "Invalid certificate role %d", role);
            }
            break;
    }

    if (min_key_size && key_size >= min_key_size)
    {
        rc = ReturnCode::kSuccess;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm_keystore, DLT_LOG_ERROR,
                "Not storing in Keystore bacause of too short key size: %d,"
                " should be minimum %d", key_size, min_key_size);
    }

    return rc;
}

bool Keystore::IsStorageTypeAllowedForCertificateRole(StorageType type, CertificateRole role)
{
    const std::vector<StorageType>& allowed_types = kAllowedStorageForCertificateRole_.at(role);

    return std::find(allowed_types.begin(), allowed_types.end(), type) != allowed_types.end();
}

bool Keystore::IsStorageTypeAllowedForKeyRole(StorageType type, KeyRole role)
{
    const std::vector<StorageType>& allowed_types = kAllowedStorageForKeyRole_.at(role);

    return std::find(allowed_types.begin(), allowed_types.end(), type) != allowed_types.end();
}

} // namespace user_manager

/** \}    end of addtogroup */
