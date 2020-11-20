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
 *  \file     usermanager.cpp
 *  \brief    FSM UserManager
 *  \author   Imran Siddique
 *
 *  \addtogroup fsm_usermanager
 *  \{
*/

#include "usermanager.h"
#include "dlt/dlt.h"

#include <cstring>
#include <sstream>

DLT_DECLARE_CONTEXT(dlt_libfsm_usermanager);

namespace user_manager
{

#define USER_TABLE_NAME "users"
#define ID_COLUMN_NAME "user_id"
#define ROLE_COLUMN_NAME "role"
#define ACTOR_CERT_ID_COLUMN_NAME "actor_cert_id"
#define SHORTRANGE_CERT_ID_COLUMN_NAME "shortrange_cert_id"
#define COMMON_NAME_COLUMN_NAME "common_name"
#define VALID_COLUMN_NAME "valid"

Usermanager::Usermanager(std::shared_ptr<Keystore> keystore) : keystore_(keystore)
{
    DLT_REGISTER_CONTEXT(dlt_libfsm_usermanager, "USERMGR","libfsm_usermanager logs");

    if (!keystore_->database_)
    {
        DLT_LOG_STRINGF(dlt_libfsm_usermanager, DLT_LOG_ERROR,
                        "database is not opened for UserManager, error: %s",
                        sqlite3_errmsg(keystore_->database_));
    }
    else
    {
        // Create the usermanager table if it does not exist in the DB.
        sqlite3_stmt* setup_table = nullptr;

        if (sqlite3_prepare_v2(keystore_->database_,
                               "CREATE TABLE IF NOT EXISTS " USER_TABLE_NAME "(" \
                                 ID_COLUMN_NAME " INTEGER PRIMARY KEY AUTOINCREMENT, " \
                                 ROLE_COLUMN_NAME " INTEGER, "                   \
                                 ACTOR_CERT_ID_COLUMN_NAME " INTEGER, "          \
                                 SHORTRANGE_CERT_ID_COLUMN_NAME " INTEGER, "     \
                                 COMMON_NAME_COLUMN_NAME " BLOB UNIQUE, "        \
                                 VALID_COLUMN_NAME " INTEGER)",
                               -1, // Read above statement to null terminator
                               &setup_table,
                               nullptr) == SQLITE_OK)
        {
            if (sqlite3_step(setup_table) != SQLITE_DONE)
            {
                DLT_LOG_STRINGF(dlt_libfsm_usermanager, DLT_LOG_ERROR,
                                "Failed to execute open_table statement, error: %s",
                                sqlite3_errmsg(keystore_->database_));
            }

            sqlite3_finalize(setup_table);
            setup_table = nullptr;
        }
        else
        {
            DLT_LOG_STRINGF(dlt_libfsm_usermanager, DLT_LOG_ERROR,
                            "Failed to prepare open_table statement, error: %s",
                            sqlite3_errmsg(keystore_->database_));
        }
    }
}

Usermanager::~Usermanager()
{
    //TODO more to clean ?
    DLT_UNREGISTER_CONTEXT(dlt_libfsm_usermanager);
}

ReturnCode Usermanager::GetUsers(std::vector<UserId>& ids)
{
    ReturnCode rc = ReturnCode::kSuccess;

    sqlite3_stmt* prepared_statement = nullptr;

    std::string statement = "SELECT " ID_COLUMN_NAME ", " ROLE_COLUMN_NAME ", "\
                            ACTOR_CERT_ID_COLUMN_NAME ", " SHORTRANGE_CERT_ID_COLUMN_NAME " " \
                            "FROM " USER_TABLE_NAME " " \
                            "WHERE " VALID_COLUMN_NAME " = 1;";

    rc = keystore_->GetStatement(&prepared_statement, statement);

    if (rc == ReturnCode::kSuccess)
    {
        rc = GetUsers(ids, prepared_statement);
    }

    keystore_->ReturnStatement(prepared_statement, statement);

    return rc;
}

ReturnCode Usermanager::GetUsers(std::vector<UserId>& ids, const UserRole role)
{
    ReturnCode rc =  ReturnCode::kSuccess;

    sqlite3_stmt* prepared_statement = nullptr;

    std::string statement = "SELECT " ID_COLUMN_NAME ", " ROLE_COLUMN_NAME ", "\
                            ACTOR_CERT_ID_COLUMN_NAME ", " SHORTRANGE_CERT_ID_COLUMN_NAME " " \
                            "FROM " USER_TABLE_NAME " " \
                            "WHERE " ROLE_COLUMN_NAME " = ?1 AND " VALID_COLUMN_NAME " = 1;";

    if (!IsUserRole(role))
    {
        rc = ReturnCode::kInvalidArgument;
    }

    if (rc == ReturnCode::kSuccess)
    {
        rc = keystore_->GetStatement(&prepared_statement, statement);
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
        rc = GetUsers(ids, prepared_statement);
    }

    keystore_->ReturnStatement(prepared_statement, statement);

    return rc;
}

ReturnCode Usermanager::GetUser(UserId& id, const X509* certificate)
{
    ReturnCode rc =  ReturnCode::kSuccess;
    sqlite3_stmt* prepared_statement = nullptr;
    std::string statement;

    CertificateId cert_id = 0;

    // we expect a valid pointer.
    if (!certificate)
    {
        rc = ReturnCode::kInvalidArgument;
    }

    if (rc == ReturnCode::kSuccess)
    {
        rc = keystore_->GetCertificateId(cert_id, certificate);
    }

    if (rc == ReturnCode::kSuccess)
    {
        statement = "SELECT " ID_COLUMN_NAME ", " ROLE_COLUMN_NAME ", "\
                    ACTOR_CERT_ID_COLUMN_NAME ", " SHORTRANGE_CERT_ID_COLUMN_NAME " " \
                    "FROM " USER_TABLE_NAME " " \
                    "WHERE ("
                    ACTOR_CERT_ID_COLUMN_NAME " = ?1" \
                    " OR " \
                    SHORTRANGE_CERT_ID_COLUMN_NAME " = ?2)" \
                    " AND " VALID_COLUMN_NAME " = 1;";

        if (rc == ReturnCode::kSuccess)
        {
            rc = keystore_->GetStatement(&prepared_statement, statement);
        }

        if (rc == ReturnCode::kSuccess)
        {
            // bind the unique cert_id could be either actor or short range certificate.
            if ((sqlite3_bind_int(prepared_statement, 1, static_cast<int>(cert_id)) ||
                 sqlite3_bind_int(prepared_statement, 2, static_cast<int>(cert_id))) != SQLITE_OK)
            {
                rc =  ReturnCode::kError;
            }
        }
    } // if (rc == ReturnCode::kSuccess)

    if (rc == ReturnCode::kSuccess)
    {
        rc = GetUser(id, prepared_statement);
    }

    keystore_->ReturnStatement(prepared_statement, statement);

    return rc;
}

ReturnCode Usermanager::GetUser(UserId& id, const std::vector<unsigned char>& common_name)
{
    ReturnCode rc =  ReturnCode::kSuccess;

    sqlite3_stmt* prepared_statement = nullptr;

    std::string statement = "SELECT " ID_COLUMN_NAME ", " ROLE_COLUMN_NAME ", "\
                            ACTOR_CERT_ID_COLUMN_NAME ", " SHORTRANGE_CERT_ID_COLUMN_NAME " " \
                            "FROM " USER_TABLE_NAME " " \
                            "WHERE " COMMON_NAME_COLUMN_NAME " = ?1 AND " VALID_COLUMN_NAME " = 1;";

    if (rc == ReturnCode::kSuccess)
    {

        rc = keystore_->GetStatement(&prepared_statement, statement);
    }

    // WORKAROUND
    // BLEManager expects commonnames to be 16 byte UUIDS, however, the Certificates
    // currently available to the mobile application are not UUIDS.
    // To handle this we pad all stored common names with 0, or cap if needed, to kEnforcedCommonNameSize bytes.

    unsigned char size_fixed_common_name[kEnforcedCommonNameSize];
    std::memset(size_fixed_common_name, 0, kEnforcedCommonNameSize);
    std::memcpy(size_fixed_common_name, common_name.data(),
                common_name.size() < kEnforcedCommonNameSize ? common_name.size() : kEnforcedCommonNameSize);

    if (rc == ReturnCode::kSuccess && (sqlite3_bind_blob(prepared_statement,
                                                         1,
                                                         size_fixed_common_name,
                                                         kEnforcedCommonNameSize,
                                                         SQLITE_TRANSIENT)
                                       != SQLITE_OK))
    // Correct non WORKAROUND version:
    //if (rc == ReturnCode::kSuccess && (sqlite3_bind_blob(prepared_statement,
    //                                                     1,
    //                                                     &(common_name[0]),
    //                                                     common_name.size(),
    //                                                     SQLITE_TRANSIENT)
    //                                   != SQLITE_OK))
    {
        std::stringstream common_name_as_hex_stream;

        for (auto uc : common_name)
        {
            common_name_as_hex_stream << std::hex << (int) uc;
        }

        DLT_LOG_STRINGF(dlt_libfsm_usermanager, DLT_LOG_ERROR,
                        "Failed to bind common name: %s, error: %s",
                        common_name_as_hex_stream.str().c_str(),
                        sqlite3_errmsg(keystore_->database_));

        rc = ReturnCode::kError;
    }

    if (rc == ReturnCode::kSuccess)
    {
        rc = GetUser(id, prepared_statement);
    }

    keystore_->ReturnStatement(prepared_statement, statement);

    return rc;
}

ReturnCode Usermanager::GetUserRole(UserRole& role, const UserId id)
{
    ReturnCode rc = ReturnCode::kSuccess;

    sqlite3_stmt* prepared_statement = nullptr;

    std::string statement = "SELECT " ID_COLUMN_NAME ", " ROLE_COLUMN_NAME ", "\
                            ACTOR_CERT_ID_COLUMN_NAME ", " SHORTRANGE_CERT_ID_COLUMN_NAME " " \
                            "FROM " USER_TABLE_NAME " " \
                            "WHERE " ID_COLUMN_NAME " = ?1 AND " VALID_COLUMN_NAME " = 1;";

    rc = keystore_->GetStatement(&prepared_statement, statement);

    if (rc == ReturnCode::kSuccess)
    {
        if (sqlite3_bind_int(prepared_statement, 1, id) != SQLITE_OK)
        {
            rc =  ReturnCode::kError;
        }
    }

    if (rc == ReturnCode::kSuccess)
    {
        rc = GetUserRole(role, prepared_statement);
    }

    keystore_->ReturnStatement(prepared_statement, statement);

    return rc;
}

ReturnCode Usermanager::GetActorCertificate(X509** certificate, UserId id)
{
    ReturnCode rc = ReturnCode::kSuccess;

    sqlite3_stmt* prepared_statement = nullptr;
    CertificateId actor_cert_id = 0;

    std::string statement = "SELECT " ID_COLUMN_NAME ", " ROLE_COLUMN_NAME ", "\
                            ACTOR_CERT_ID_COLUMN_NAME ", " SHORTRANGE_CERT_ID_COLUMN_NAME " " \
                            "FROM " USER_TABLE_NAME " " \
                            "WHERE " ID_COLUMN_NAME " = ?1 AND " VALID_COLUMN_NAME " = 1;";

    // we expect a valid pointer.
    if (!certificate)
    {
        rc = ReturnCode::kInvalidArgument;
    }

    if (rc == ReturnCode::kSuccess)
    {
        rc = keystore_->GetStatement(&prepared_statement, statement);
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
        rc = GetActorCertificateId(actor_cert_id, prepared_statement);
    }

    if (rc == ReturnCode::kSuccess)
    {
        rc = keystore_->GetCertificate(certificate, actor_cert_id);
    }

    keystore_->ReturnStatement(prepared_statement, statement);

    return rc;
}

ReturnCode Usermanager::GetActorCertificates(STACK_OF(X509)* certificates, const std::vector<UserId> ids)
{
    ReturnCode rc = ReturnCode::kSuccess;

    std::vector<X509*> local_certificates;

    sqlite3_stmt* prepared_statement = nullptr;

    std::string statement = "SELECT " ID_COLUMN_NAME ", " ROLE_COLUMN_NAME ", "\
                            ACTOR_CERT_ID_COLUMN_NAME ", " SHORTRANGE_CERT_ID_COLUMN_NAME " " \
                            "FROM " USER_TABLE_NAME " " \
                            "WHERE " ID_COLUMN_NAME " = ?1 AND " VALID_COLUMN_NAME " = 1;";

    // we expect a valid pointer.
    if (!certificates)
    {
        rc = ReturnCode::kInvalidArgument;
    }

    if (rc == ReturnCode::kSuccess)
    {
        rc = keystore_->GetStatement(&prepared_statement, statement);
    }

    if (rc == ReturnCode::kSuccess)
    {
        for (UserId local_ids : ids)
        {
            CertificateId actor_cert_id = 0;
            X509* certificate = nullptr;

            if (rc == ReturnCode::kSuccess)
            {
                if (sqlite3_reset(prepared_statement) != SQLITE_OK
                    || sqlite3_clear_bindings(prepared_statement) != SQLITE_OK
                    || sqlite3_bind_int(prepared_statement, 1, static_cast<int>(local_ids)) != SQLITE_OK)
                {
                    rc =  ReturnCode::kError;
                }
            }

            if (rc == ReturnCode::kSuccess)
            {
                rc = GetActorCertificateId(actor_cert_id, prepared_statement);
            }

            if (rc == ReturnCode::kSuccess)
            {
                rc = keystore_->GetCertificate(&certificate, actor_cert_id);
            }

            if (rc == ReturnCode::kSuccess && certificate)
            {
                // Push the certificate to local vector.
                // if given set of users found all corresponding certificates
                // Vector will be pushed to STACK_OF certificates.
                local_certificates.push_back(certificate);
            }

            if (rc == ReturnCode::kNotFound || rc == ReturnCode::kError)
            {
                local_certificates.clear();
                break;
            }
        } // for ( ... )
    } // if (rc == ReturnCode::kSuccess)

    if (rc == ReturnCode::kSuccess)
    {
        for (X509* certs : local_certificates)
        {
            sk_X509_push(certificates, certs);
        }
    }

    keystore_->ReturnStatement(prepared_statement, statement);

    return rc;
}

ReturnCode Usermanager::GetShortRangeTlsClientCertificate(X509** certificate, const UserId id)
{
    ReturnCode rc = ReturnCode::kSuccess;

    sqlite3_stmt* prepared_statement = nullptr;
    CertificateId short_range_cert_id = 0;

    std::string statement = "SELECT " ID_COLUMN_NAME ", " ROLE_COLUMN_NAME ", "\
                            ACTOR_CERT_ID_COLUMN_NAME ", " SHORTRANGE_CERT_ID_COLUMN_NAME " " \
                            "FROM " USER_TABLE_NAME " " \
                            "WHERE " ID_COLUMN_NAME " = ?1 AND " VALID_COLUMN_NAME " = 1;";

    // we expect a valid pointer.
    if (!certificate)
    {
        rc = ReturnCode::kInvalidArgument;
    }

    if (rc == ReturnCode::kSuccess)
    {
        rc = keystore_->GetStatement(&prepared_statement, statement);
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
        rc = GetShortRangeTlsClientCertificateId(short_range_cert_id, prepared_statement);
    }

    if (rc == ReturnCode::kSuccess)
    {
        rc = keystore_->GetCertificate(certificate, short_range_cert_id);
    }

    keystore_->ReturnStatement(prepared_statement, statement);

    return rc;
}

ReturnCode Usermanager::GetShortRangeTlsClientCertificates(STACK_OF(X509)* certificates, const std::vector<UserId> ids)
{
    ReturnCode rc = ReturnCode::kSuccess;

    std::vector<X509*> local_certificates;

    sqlite3_stmt* prepared_statement = nullptr;

    std::string statement = "SELECT " ID_COLUMN_NAME ", " ROLE_COLUMN_NAME ", "\
                            ACTOR_CERT_ID_COLUMN_NAME ", " SHORTRANGE_CERT_ID_COLUMN_NAME " " \
                            "FROM " USER_TABLE_NAME " " \
                            "WHERE " ID_COLUMN_NAME " = ?1 AND " VALID_COLUMN_NAME " = 1;";

    // we expect a valid pointer.
    if (!certificates)
    {
        rc = ReturnCode::kInvalidArgument;
    }

    if (rc == ReturnCode::kSuccess)
    {
        rc = keystore_->GetStatement(&prepared_statement, statement);
    }

    if (rc == ReturnCode::kSuccess)
    {
        for (UserId local_ids : ids)
        {
            CertificateId shortrange_cert_id = 0;
            X509* certificate = nullptr;

            if (rc == ReturnCode::kSuccess)
            {
                if (sqlite3_reset(prepared_statement) != SQLITE_OK
                    || sqlite3_clear_bindings(prepared_statement) != SQLITE_OK
                    || sqlite3_bind_int(prepared_statement, 1, static_cast<int>(local_ids)) != SQLITE_OK)
                {
                    rc =  ReturnCode::kError;
                }
            }

            if (rc == ReturnCode::kSuccess)
            {
                rc = GetShortRangeTlsClientCertificateId(shortrange_cert_id, prepared_statement);
            }

            if (rc == ReturnCode::kSuccess)
            {
                rc = keystore_->GetCertificate(&certificate, shortrange_cert_id);
            }

            if (rc == ReturnCode::kSuccess && certificates)
            {
                // Push the certificate to local vector.
                // if given set of users found all corresponding certificates
                // Vector will be push to STACK_OF certificates.
                local_certificates.push_back(certificate);
            }

            if (rc == ReturnCode::kNotFound || rc == ReturnCode::kError)
            {
                local_certificates.clear();
                break;
            }
        } // for ( ... )
    } // if (rc == ReturnCode::kSuccess)

    if (rc == ReturnCode::kSuccess)
    {
        for (X509* certs : local_certificates)
        {
            sk_X509_push(certificates, certs);
        }
    }

    keystore_->ReturnStatement(prepared_statement, statement);

    return rc;
}

ReturnCode Usermanager::GetCommonName(std::vector<unsigned char>& common_name, const UserId id)
{
    ReturnCode rc = ReturnCode::kSuccess;

    sqlite3_stmt* prepared_statement = nullptr;

    std::string statement = "SELECT " ID_COLUMN_NAME ", " ROLE_COLUMN_NAME ", "\
                            ACTOR_CERT_ID_COLUMN_NAME ", " SHORTRANGE_CERT_ID_COLUMN_NAME ", " \
                            COMMON_NAME_COLUMN_NAME " " \
                            "FROM " USER_TABLE_NAME " " \
                            "WHERE " ID_COLUMN_NAME " = ?1 AND " VALID_COLUMN_NAME " = 1;";

    if (rc == ReturnCode::kSuccess)
    {
        rc = keystore_->GetStatement(&prepared_statement, statement);
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
        UserId local_id;
        UserRole local_role;
        CertificateId local_actor_cert_id;
        CertificateId local_shortrange_cert_id;

        rc = GetRow(local_id,
                    local_role,
                    local_actor_cert_id,
                    local_shortrange_cert_id,
                    prepared_statement,
                    &common_name);
    }

    keystore_->ReturnStatement(prepared_statement, statement);

    return rc;
}

ReturnCode Usermanager::AddUser(UserId& id,
                                UserRole role,
                                X509* actor_cert,
                                X509* shortrange_tls_client_cert)
{
    ReturnCode rc = ReturnCode::kSuccess;

    CertificateId actor_cert_id = kUndefinedCertificateId;
    CertificateId shortrange_cert_id = kUndefinedCertificateId;

    // Expecting valid arguments
    if (!actor_cert || !IsUserRole(role))
    {
        rc = ReturnCode::kInvalidArgument;
    }

    if (rc == ReturnCode::kSuccess)
    {
        rc = keystore_->AddCertificate(actor_cert_id, actor_cert, Keystore::CertificateRole::kUserActor);
    }

    if ((rc == ReturnCode::kSuccess) && shortrange_tls_client_cert)
    {
        rc = keystore_->AddCertificate(shortrange_cert_id,
                                       shortrange_tls_client_cert,
                                       Keystore::CertificateRole::kUserShortRangeTlsClient);
    }

    unsigned char* common_name = nullptr;
    unsigned int common_name_length = 0;

    if (rc == ReturnCode::kSuccess)
    {
        rc = GetCommonName(&common_name, common_name_length, actor_cert);
    }

    if (rc == ReturnCode::kSuccess)
    {
        // WORKAROUND
        // BLEManager expects commonnames to be 16 byte UUIDS, however, the Certificates
        // currently available to the mobile application are not UUIDS.
        // To handle this we pad all stored common names with 0, or cap if needed, to kEnforcedCommonNameSize bytes.
        unsigned char size_fixed_common_name[kEnforcedCommonNameSize];
        std::memset(size_fixed_common_name, 0, kEnforcedCommonNameSize);
        std::memcpy(size_fixed_common_name, common_name,
                    common_name_length < kEnforcedCommonNameSize ? common_name_length : kEnforcedCommonNameSize);

        rc = InsertUser(role, size_fixed_common_name, kEnforcedCommonNameSize, actor_cert_id, shortrange_cert_id);

        // Correct non WORKAROUND version:
        //rc = InsertUser(role, common_name, common_name_length, actor_cert_id, shortrange_cert_id);
    }

    // To avoid returning the wrong id if the same
    // user has previously been added for another role
    // we use sqlite3_last_insert_rowid .
    // There may be some risk of concurrency issues...
    if (rc == ReturnCode::kSuccess)
    {
        id = sqlite3_last_insert_rowid(keystore_->database_);

        if (id == 0)
        {
            rc = ReturnCode::kError;
        }
    }

    return rc;
}

ReturnCode Usermanager::DeleteUser(const UserId id)
{
    ReturnCode rc = ReturnCode::kSuccess;

    sqlite3_stmt* prepared_statement = nullptr;

    std::string statement = "UPDATE " USER_TABLE_NAME " "     \
                            "SET " VALID_COLUMN_NAME " = 0 "  \
                            "WHERE " ID_COLUMN_NAME " = ?1;";

    if (!keystore_->database_)
    {
        rc = ReturnCode::kError;
    }

    // Pre-check that the user-id actually exists prior to executing our update statement.
    fsm::UserRole dummy_role;
    rc = GetUserRole(dummy_role, id);

    if (rc == ReturnCode::kSuccess)
    {
        rc = keystore_->GetStatement(&prepared_statement, statement);
    }

    if (rc == ReturnCode::kSuccess)
    {
       if (sqlite3_bind_int(prepared_statement, 1, static_cast<int>(id)) !=
           SQLITE_OK)
       {
           rc =  ReturnCode::kError;
       }
    }

    if (rc == ReturnCode::kSuccess)
    {
        if (sqlite3_step(prepared_statement) != SQLITE_DONE)
        {
            DLT_LOG_STRINGF(dlt_libfsm_usermanager, DLT_LOG_ERROR,
                            "Failed to invalidate existing entry, error: %s",
                            sqlite3_errmsg(keystore_->database_));

            rc = ReturnCode::kError;
        }
    }

    keystore_->ReturnStatement(prepared_statement, statement);

    return rc;
}

ReturnCode Usermanager::GetUser(UserId& id, sqlite3_stmt* statement)
{
    ReturnCode rc = ReturnCode::kSuccess;

    UserId local_id = 0;
    UserRole local_role = UserRole::kUndefined;
    CertificateId local_actor_cert_id = 0;
    CertificateId local_shortrange_cert_id = 0;

    rc = GetRow(local_id, local_role, local_actor_cert_id, local_shortrange_cert_id, statement);

    if (rc == ReturnCode::kSuccess)
    {
        id = local_id;
    }

    return rc;
}

ReturnCode Usermanager::GetUserRole(UserRole& role, sqlite3_stmt* statement)
{
    ReturnCode rc = ReturnCode::kSuccess;

    UserId local_id = 0;
    UserRole local_role = UserRole::kUndefined;
    CertificateId local_actor_cert_id = 0;
    CertificateId local_shortrange_cert_id = 0;

    rc = GetRow(local_id, local_role, local_actor_cert_id, local_shortrange_cert_id, statement);

    if (rc == ReturnCode::kSuccess)
    {
        role = local_role;
    }

    return rc;
}

ReturnCode Usermanager::GetUsers(std::vector<UserId>& ids, sqlite3_stmt* statement)
{
    ReturnCode rc = ReturnCode::kSuccess;

    UserId local_id = 0;
    UserRole local_role = UserRole::kUndefined;
    CertificateId local_actor_cert_id = 0;
    CertificateId local_shortrange_cert_id = 0;

    while (rc == ReturnCode::kSuccess)
    {
        rc = GetRow(local_id, local_role, local_actor_cert_id, local_shortrange_cert_id, statement);

        if (rc == ReturnCode::kSuccess)
        {
            ids.push_back(local_id);
        }
    }

    if (ids.size() != 0 && rc == ReturnCode::kNotFound)
    {
        // If the number of found ids is not zero, than rc cannot stay kNotFound as we have results.
        // In this scope, ReturnCode::kNotFound means just that SQL is done retrieving results.
        rc = ReturnCode::kSuccess;
    }

    return rc;
}

ReturnCode Usermanager::GetActorCertificateId(CertificateId& actor_cert_id, sqlite3_stmt* statement)
{
    ReturnCode rc = ReturnCode::kSuccess;

    UserId local_id = 0;
    UserRole local_role = UserRole::kUndefined;
    CertificateId local_actor_cert_id = 0;
    CertificateId local_shortrange_cert_id = 0;

    rc = GetRow(local_id, local_role, local_actor_cert_id, local_shortrange_cert_id, statement);

    if (rc == ReturnCode::kSuccess)
    {
        actor_cert_id = local_actor_cert_id;
    }

    return rc;
}

ReturnCode Usermanager::GetShortRangeTlsClientCertificateId(CertificateId& shortrange_cert_id,
                                                            sqlite3_stmt* statement)
{
    ReturnCode rc = ReturnCode::kSuccess;

    UserId local_id = 0;
    UserRole local_role = UserRole::kUndefined;
    CertificateId local_actor_cert_id = 0;
    CertificateId local_shortrange_cert_id = 0;

    rc = GetRow(local_id, local_role, local_actor_cert_id, local_shortrange_cert_id, statement);

    if (rc == ReturnCode::kSuccess)
    {
        shortrange_cert_id = local_shortrange_cert_id;
    }

    return rc;
}

ReturnCode Usermanager::InsertUser(UserRole role,
                                   const unsigned char* common_name,
                                   int common_name_length,
                                   CertificateId actor_cert_id,
                                   CertificateId shortrange_cert_id)
{
    ReturnCode rc = ReturnCode::kSuccess;

    sqlite3_stmt* prepared_statement = nullptr;
    std::string statement = "INSERT INTO " USER_TABLE_NAME "(" \
                            ROLE_COLUMN_NAME ", " ACTOR_CERT_ID_COLUMN_NAME ", " \
                            SHORTRANGE_CERT_ID_COLUMN_NAME ", " COMMON_NAME_COLUMN_NAME ", " \
                            VALID_COLUMN_NAME ") VALUES (?1, ?2, ?3, ?4, 1);";

    if (!keystore_->database_)
    {
        rc = ReturnCode::kError;
    }

    if (rc == ReturnCode::kSuccess)
    {
        rc = keystore_->GetStatement(&prepared_statement, statement);
    }

    if (rc == ReturnCode::kSuccess && (sqlite3_bind_int(prepared_statement,
                                                        1,
                                                        static_cast<int>(role))
                                       != SQLITE_OK))
    {
        rc =  ReturnCode::kError;
    }
    if (rc == ReturnCode::kSuccess && (sqlite3_bind_int(prepared_statement,
                                                        2,
                                                        static_cast<int>(actor_cert_id))
                                       != SQLITE_OK))
    {
        rc =  ReturnCode::kError;
    }
    if (rc == ReturnCode::kSuccess && (sqlite3_bind_int(prepared_statement,
                                                        3,
                                                        static_cast<int>(shortrange_cert_id))
                                       != SQLITE_OK))
    {
        rc =  ReturnCode::kError;
    }
    if (rc == ReturnCode::kSuccess && (sqlite3_bind_blob(prepared_statement,
                                                         4,
                                                         common_name,
                                                         common_name_length,
                                                         SQLITE_TRANSIENT)
                                       != SQLITE_OK))
    {
        rc =  ReturnCode::kError;
    }

    if (rc == ReturnCode::kSuccess)
    {
        if (sqlite3_step(prepared_statement) != SQLITE_DONE)
        {
            DLT_LOG_STRINGF(dlt_libfsm_usermanager, DLT_LOG_ERROR,
                            "Failed to insert entry, error: %s",
                            sqlite3_errmsg(keystore_->database_));

            rc = ReturnCode::kError;
        }
    }

    keystore_->ReturnStatement(prepared_statement, statement);

    return rc;
}

ReturnCode Usermanager::GetRow(UserId& id,
                               UserRole& role,
                               CertificateId& actor_cert_id,
                               CertificateId& shortrange_cert_id,
                               sqlite3_stmt* statement,
                               std::vector<unsigned char>* common_name)
{
    ReturnCode rc = ReturnCode::kSuccess;
    bool done = false;

    if (!keystore_->database_)
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
            id = static_cast<UserId>(sqlite3_column_int(statement, 0));

            role = static_cast<UserRole>(sqlite3_column_int(statement, 1));
            actor_cert_id = static_cast<CertificateId>(sqlite3_column_int(statement, 2));
            shortrange_cert_id = static_cast<CertificateId>(sqlite3_column_int(statement, 3));

            if (common_name)
            {
                const unsigned char* local_common_name =
                    static_cast<const unsigned char*>(sqlite3_column_blob(statement, 4)); //do not free!
                int length = sqlite3_column_bytes(statement, 4);
                if (local_common_name && length > 0)
                {
                    common_name->assign(local_common_name, local_common_name + length);
                }
            }

            done = true;
            rc = ReturnCode::kSuccess;

            break;
        }
        case SQLITE_BUSY :
        {
            // We use a BUSY timout handler, so this means it timed out.
            id = 0;
            role = UserRole::kUndefined;
            actor_cert_id = 0;
            shortrange_cert_id = 0;

            done = true;
            rc = ReturnCode::kError;

            DLT_LOG_STRING(dlt_libfsm_usermanager, DLT_LOG_WARN, "Usermanager db busy timedout.");

            break;
        }
        case SQLITE_DONE :
        {
            id = 0;
            role = UserRole::kUndefined;
            actor_cert_id = 0;
            shortrange_cert_id = 0;

            done = true;
            rc = ReturnCode::kNotFound;

            break;
        }
        default:
        {
            // some sort of error occured
            id = 0;
            role = UserRole::kUndefined;
            actor_cert_id = 0;
            shortrange_cert_id = 0;

            done = true;
            rc = ReturnCode::kError;

            DLT_LOG_STRINGF(dlt_libfsm_usermanager, DLT_LOG_ERROR,
                            "Error occured while trying to find reference, error: %s.",
                            sqlite3_errmsg(keystore_->database_));
        }
        } // switch (status)
    }  // while (!done && rc == ReturnCode::kSuccess)

    return rc;
}

bool Usermanager::IsUserRole(const UserRole role)
{
    return role == UserRole::kAdmin ||
           role == UserRole::kUser ||
           role == UserRole::kDelegate ||
           role == UserRole::kCarAccess ||
           role == UserRole::kAssistanceCall;
}

bool Usermanager::IsValidCertRole(const fsm::Keystore::CertificateRole role)
{
    return role == fsm::Keystore::CertificateRole::kUserActor ||
           role == fsm::Keystore::CertificateRole::kUserShortRangeTlsClient;
}

ReturnCode Usermanager::GetCommonName(unsigned char** common_name,
                                      unsigned int& common_name_length,
                                      X509* certificate)
{
    ReturnCode rc = ReturnCode::kSuccess;

    if (!common_name || *common_name)
    {
        rc = ReturnCode::kInvalidArgument;
    }
    else if (!certificate)
    {
        rc = ReturnCode::kInvalidArgument;
    }

    if (rc == ReturnCode::kSuccess)
    {
        X509_NAME* name = nullptr;
        X509_NAME_ENTRY* name_entry = nullptr;
        int last_index = -1;

        name = X509_get_subject_name(certificate);

        if (name)
        {
            last_index = X509_NAME_get_index_by_NID(name, NID_commonName, last_index);

            if (last_index == -1)
            {
                DLT_LOG_STRING(dlt_libfsm_usermanager, DLT_LOG_ERROR,
                               "Failed to get common name subject name entry.");

                rc = ReturnCode::kError;
            }
            else
            {
                name_entry = X509_NAME_get_entry(name, last_index);

                if (name_entry)
                {
                    ASN1_STRING* common_name_data =  X509_NAME_ENTRY_get_data(name_entry);
                    if (common_name_data)
                    {
                        // we get the raw data rather than treating it as text
                        // as we expect UUIDs
//the complete handling of "common_name" needs to be checked and adapted to new OpenSSL version
// ASN1_STRING_data is deprecated and should be replaced by ASN1_STRING_get0_data, but without having a cast !
                        *common_name = (unsigned char*)ASN1_STRING_get0_data(common_name_data);
                        common_name_length = ASN1_STRING_length(common_name_data);
                    }
                    else
                    {
                        DLT_LOG_STRING(dlt_libfsm_usermanager, DLT_LOG_ERROR,
                                       "Failed to get common name subject name entry data.");

                        rc = ReturnCode::kError;
                    }
                }
                else
                {
                    DLT_LOG_STRING(dlt_libfsm_usermanager, DLT_LOG_ERROR,
                                   "Got bad common name subject name entry.");

                    rc = ReturnCode::kError;
                }

            }

            last_index = X509_NAME_get_index_by_NID(name, NID_commonName, last_index);

            if (last_index != -1)
            {
                DLT_LOG_STRING(dlt_libfsm_usermanager, DLT_LOG_WARN,
                               "User Actor certificate has more than one common name.");
            }
        }
        else
        {
            DLT_LOG_STRING(dlt_libfsm_usermanager, DLT_LOG_ERROR,
                           "Failed to get subject name from actor cert.");

            rc = ReturnCode::kError;
        }
    }
    return rc;
}

} // namespace user_manager

/** \}    end of addtogroup */
