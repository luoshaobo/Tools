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
 *  \file     keystore.h
 *  \brief    FSM KeyStore
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup fsm_keystore
 *  \{
*/

#ifndef FSM_KEYSTORE_H
#define FSM_KEYSTORE_H

#include "usermanager_types.h"

#include <mutex>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

#include <openssl/evp.h>
#include <openssl/x509.h>
#include <openssl/x509_vfy.h>
#include <sqlite3.h>

namespace user_manager
{

typedef int CertificateId;
typedef int KeyId;

const CertificateId kUndefinedCertificateId = -1;
const KeyId kUndefinedKeyId = -1;

const int kMinRegularECKeySize = 256;
// TODO (Dariusz): change to correct root key size after we start using correct EC curve
// for the Vehicle CA cert/key (VTV-3013)
const int kMinRootECKeySize = kMinRegularECKeySize;
const int kMinRSAKeySize = 2048;

/**
 * \brief Foundation Services Keystore, provides access to keys and certificates
 *        used with Fondation Services.
 *        The general usage pattern clients are expected to follow is to request
 *        the key or certificate when its needed, immediately use it and then free
 *        it. Caching the certificates and/or keys may cause them to become obsolete.
 *
 *        For openssl types the are handled as c style pointers, clients are expected
 *        to free any acquired pointers using the relevant openssl free call.
 */
class Keystore
{

 friend class Usermanager;

 public:

    /*
     * \brief Enumerates the list of roles (functions) for which the Keystore stores keys.
     *        All KeyRoles map to only one valid key.
     */
    enum class KeyRole : int
    {
        kUndefined = 0, ///< Indicates a unknown or undefined key role.
        // Below keys all correspond to a certificate in the Vehicle security domain ("red keys").
        kVehicleCa = 1, ///< The private key of the vehicle CA
        kVehicleActor = 2, ///< The private key of the vehicle actor.
        kVehicleShortRangeTlsServer = 3, ///< The private key of the vehicle BLE sever.
        // Below keys all correspond to a certificate in the Connected Car security domain (cloud, "green keys").
        kCloudTlsClient = 4,     ///< The private key of the VSDP tls client certificate.
        kDoIpTlsServer = 5, ///< The private key of the DoIp tls server certificate.
        kCloudSignEncrypt = 6, ///< The private key of the backend sign/encrypt certificate.
        // Extend as needed, Note: "id values" are used in the database, must not be changed.
        kEndMarker = 7 ///<Marks the maximum valid value + 1
    };

    /*
     * \brief Enumerates the list of roles (functions) for which the Keystore stores certificates.
     *        Most CertificateRoles will map to only one valid certificate, but some can map to
     *        several, these are: kUserActor, kUserShortRangeTlsClient, kCloudCa, kVehicleCa.
     *        This will affect the behavior of some functions.
     */
    enum class CertificateRole : int
    {
        // Values chosen to ensure we do not overlap with key roles.
        kUndefined = 1000, ///< Indicates a unknown or undefined certificate role.
        // Below certificates are in the Vehicle security domain ("red certificates")-
        kVehicleCa = 1001, ///< The certificate of the vehicle CA.
        kVehicleActor = 1002, ///< The vehicles actor certificate.
        kVehicleShortRangeTlsServer = 1003, ///< The vehicles BLE server certificate.
        kUserActor = 1004, ///< A paired users actor certificate.
        kUserShortRangeTlsClient = 1005, ///< A paired users BLE client certificate.
        // Below certificates are in the Connected Car security domain ("green certificates").
        kCloudCa = 1006, ///< The certificate of the cloud CA.
        kCloudTlsClient = 1007, ///< The vehicles tls client cert to be used when connecting to the cloud.
        kDoIpTlsServer = 1008, ///< The vehicles DoIp server certificate.
        kCloudSignEncrypt = 1009, ///< The sign/encrypt key to be used when sending messages to the cloud.
        // Extend as needed, Note: "id values" are used in the database, must not be changed.
        kEndMarker = 1010 ///<Marks the maximum valid value + 1
    };

    /**
     * \brief Creates a new Keystore instance. As some caching may be performed
     *        inside the keystore clients are recomended to not create a new instance
     *        for every operation.
     *        User must ensure to call tpPCL_init before trying to create the keystore.
     */
    Keystore();

    /**
     * \brief Destroys the Keystore instance freeing any associated resources.
     */
    ~Keystore();

    /**
     * \brief Get a certificate with specific id.
     *        Returned certificate should not be cached by client.
     * \param[out] certificate Will be set to point to certificate if found.
     *                         Caller must free the provided certificate using
     *                         openssl X509_free.
     *                         If nullptr is passed the function will only return proper
     *                         ReturnCode (useful for checking for certificate presence).
     * \param[in] id The id of the requested certificate.
     * \return ReturnCode::kNotFound if the certificate was not found,
     *         or was found but was invalid.
     *         ReturnCode::kError on error and ReturnCode::kSuccess
     *         on success.
     */
    ReturnCode GetCertificate(X509** certificate, const CertificateId id);

    /**
     * \brief Get the currently valid certificate for a role.
     *        Returned certificate should not be cached by client.
     * \param[out] certificate Will be set to point to certificate if found.
     *                         Caller must free the provided certificate using
     *                         openssl X509_free.
     *                         If nullptr is passed the function will only return proper
     *                         ReturnCode (useful for checking for certificate presence).
     * \param[in] role The role the certificate is requested for.
     *                 Only one-to-one mappings are allowed, thus
     *                 kUserActor, kUserShortRangeTlsClient,
     *                 kVehicleCa and kCloudCa are invalid input.
     * \return ReturnCode::kNotFound if the certificate was not found,
     *         ReturnCode::kError on error, ReturnCode::kInvalidArgument
     *         if bad role supplied and ReturnCode::kSuccess on success.
     */
    ReturnCode GetCertificate(X509** certificate, const CertificateRole role);

    /**
     * \brief Get the currently valid certificate for a role as a file.
     *        The "get as file functions are primarily intended for clients
     *        which neecd to feed keys and cerrtificates to third party libraries
     *        as files, typically for TLS, thus they do not exist in all flavours.
     *        Returned certificate should not be cached by client, get a new one for
     *        each session.
     * \param[out] certificate Will be set to point to a file containing the certificate if found.
     *                         Caller owns this file, it will typically be in tmpfs
     *                         but if it should be proactively deleted caller must do so.
     * \param[in] role The role the certificate is requested for.
     *                 Only one-to-one mappings are allowed, thus
     *                 kUserActor, kUserShortRangeTlsClient,
     *                 kVehicleCa and kCloudCa are invalid input.
     * \return ReturnCode::kNotFound if the certificate was not found,
     *         ReturnCode::kError on error, ReturnCode::kInvalidArgument
     *         if bad role supplied and ReturnCode::kSuccess on success.
     */
    ReturnCode GetCertificate(std::string& certificate, const CertificateRole role);

    /**
     * \brief Get the currently valid certificates for a role.
     *        Returned certificates should not be cached by client.
     * \param[out] certificates Will be filled with found certificates.
     *                          Caller must free the provided certificate using
     *                          appropriate openssl calls like sk_X509_pop_free.
     *                          If nullptr is passed the function will only return proper
     *                          ReturnCode (useful for checking for certificate presence).
     * \param[in] role The role the certificates are requested for.
     *                 Only one-to-many mappings are allowed, thus
     *                 kUserActor, kUserShortRangeTlsClient,
     *                 kVehicleCa and kCloudCa are the only valid input.
     * \return ReturnCode::kNotFound if no certificates was found,
     *         ReturnCode::kError on error, ReturnCode::kInvalidArgument
     *         if bad role supplied and ReturnCode::kSuccess on success.
     */
    ReturnCode GetCertificates(STACK_OF(X509)* certificates, const CertificateRole role);

    /**
     * \brief Get the currently valid certificates for a role as a file.
     *        The "get as file functions are primarily intended for clients
     *        which neecd to feed keys and cerrtificates to third party libraries
     *        as files, typically for TLS, thus they do not exist in all flavours.
     *        Returned certificate should not be cached by client, get a new one for
     *        each session.
     * \param[out] certificates Will be set to point to a file containing the certificates if found.
     *                          Caller owns this file, it will typically be in tmpfs
     *                          but if it should be proactively deleted caller must do so.
     * \param[in] role The role the certificates are requested for.
     *                 Only one-to-many mappings are allowed, thus
     *                 kUserActor, kUserShortRangeTlsClient,
     *                 kVehicleCa and kCloudCa are the only valid input.
     * \return ReturnCode::kNotFound if no certificates was found,
     *         ReturnCode::kError on error, ReturnCode::kInvalidArgument
     *         if bad role supplied and ReturnCode::kSuccess on success.
     */
    ReturnCode GetCertificates(std::string& certificates, const CertificateRole role);

    /**
     * \brief Get the currently valid key for a role.
     *        Returned key should not be cached by client.
     * \param[out] key Will be set to point to key if found.
     *                 Caller must free the provided key using openssl
     *                 EVP_PKEY_free.
     *                 If nullptr is passed the function will only return proper
     *                 ReturnCode (useful for checking for key presence).
     * \param[in] role The role the key is requested for.
     * \return ReturnCode::kNotFound if the key was not found,
     *         ReturnCode::kError on error and ReturnCode::kSuccess on success.
     */
    ReturnCode GetKey(EVP_PKEY** key, KeyRole role);

    /**
     * \brief Get the currently valid key for a role as a file.
     *        The "get as file functions are primarily intended for clients
     *        which neecd to feed keys and cerrtificates to third party libraries
     *        as files, typically for TLS, thus they do not exist in all flavours.
     *        Returned key should not be cached by client, get a new one for
     *        each session.
     * \param[out] key Will be set to point to a file containing the key if found.
     *                 Caller owns this file, it will typically be in tmpfs
     *                 but if it should be proactively deleted caller must do so.
     * \param[in] role The role the key is requested for.
     * \return ReturnCode::kNotFound if the key was not found,
     *         ReturnCode::kError on error and ReturnCode::kSuccess on success.
     */
    ReturnCode GetKey(std::string& key, KeyRole role);

    /**
     * \brief Get all known certificates that could be potentially have
     *        been used to sign a message which the vehicle has received.
     *        Typically this would be all the user actor certificates.
     * \param[in/out] certificates Found certificates will be added in
     *                             in this stack.
     *                             Caller must free the provided certificates
     *                             using openssl, typically via sk_X509_pop_free.
     *                             If nullptr is passed the function will only return proper
     *                             ReturnCode (useful for checking for certificate presence).
     * \return ReturnCode::kNotFound if no signer certificate was found,
     *         ReturnCode::kError on error and ReturnCode::kSuccess on success.
     */
    ReturnCode GetKnownSignerCertificates(STACK_OF(X509)* certificates);

    /**
     * \brief Get all known CAs.
     *        Typically this would be the Vehicle and Connected Car CAs
     *        and can be use to chain verify an incomming signature.
     * \param[out] ca_store Will be set to point to a certificate store
     *                      containing the CAs if fund.
     *                      Caller must free the provided store using openssl
     *                      X509_STORE_free.
     *                      If nullptr is passed the function will only return proper
     *                      ReturnCode (useful for checking for certificate presence).
     * \return ReturnCode::kNotFound if no CA certificates was found,
     *         ReturnCode::kError on error and ReturnCode::kSuccess on success.
     */
    ReturnCode GetCertificateAuthorities(X509_STORE** ca_store);

    /**
     * \brief Get a CA by role.
     *        Typically this would be either the Vehicle or Connected Car CA
     *        and can be use to chain verify an incomming signature.
     * \param[out] ca_store Will be set to point to a certificate store
     *                      containing the CA if fund.
     *                      Caller must free the provided store using openssl
     *                      X509_STORE_free.
                            If nullptr is passed the function will only return proper
     *                      ReturnCode (useful for checking for certificate presence).
     * \param[in] role The role to get CA for.
     *                 Only kVehicleCa, kCloudCa are valid input.
     * \return ReturnCode::kNotFound if no CA was found,
     *         ReturnCode::kError on error, ReturnCode::kInvalidArgument
     *         if bad role supplied and ReturnCode::kSuccess on success.
     */
    ReturnCode GetCertificateAuthority(X509_STORE** ca_store, const CertificateRole role);

    /**
     * \brief Set the certificate for a role, replaces the currently used
     *        certificate if any.
     * \param[out] id Will be set to point to the id of the certificate if
     *             successful.
     * \param[out] old_certificate If not null and operation successful
     *                             and present, will be set to point to the
     *                             old certificate.
     * \param[in] certificate Certificate to set.
     * \param[in] role Role the certificate is being set for.
     *                 Only one-to-one mappings are allowed, thus
     *                 kUserActor, kUserShortRangeTlsClient,
     *                 kVehicleCa and kCloudCa are invalid input.
     * \return ReturnCode::kError on error, ReturnCode::kInvalidArgument
     *         if bad role supplied and ReturnCode::kSuccess on success.
     */
    ReturnCode SetCertificate(CertificateId& id,
                              X509** old_certificate,
                              X509* certificate,
                              const CertificateRole role);

    /**
     * \brief Adds a user certificate.
     * \param[out] id Will be set to point to the id of the certificate if
     *             successful.
     * \param[in] certificate certificate to set.
     * \param[in] role Role the certificate is being set for. Only one to many
     *                 mapped roles are allowed thus only kUserActor,
     *                 kUserShortRangeTlsClient, kVehicleCa and kCloudCa are
     *                 valid input.
     * \return ReturnCode::kError on error, ReturnCode::kInvalidArgument
     *         if bad role supplied and ReturnCode::kSuccess on success.
     */
    ReturnCode AddCertificate(CertificateId& id,
                              X509* certificate,
                              const CertificateRole role);

    /**
     * \brief Updates a certificate.
     * \param[out] old_certificate  Will be set to point to the old certificate if
     *             successful and not nullptr.
     * \param[out] id Will be set to point to the id of the updated certificate
     *                if successful.
     * \param[in] certificate New certificate to set.
     * \param[in] id Id of the certificate to update.
     * \return ReturnCode::kError on error, ReturnCode::kNotFound
     *         if certificate matching id was not found, or was found
     *         but was invalid. ReturnCode::kSuccess on success.
     */
    ReturnCode UpdateCertificate(X509** old_certificate,
                                 CertificateId& new_id,
                                 X509* certificate,
                                 const CertificateId id);

    /**
     * \brief Set the key for a role, replaces the currently used
     *        key if any.
     * \param[out] id Will be set to point to the id of the key if
     *             successful.
     * \param[out] old_key If not null and operation successful and present,
     *                     will be set to point to the old key.


     * \param[in] key Key to set.
     * \param[in] role Role the key is being set for.
     * \return ReturnCode::kError on error and ReturnCode::kSuccess on success.
     */
    ReturnCode SetKey(KeyId& id, EVP_PKEY** old_key, EVP_PKEY* key, const KeyRole role);

    /**
     * \brief Delete a specific certificate.
     * \param[out] old_certificate If not null and operation successful,
     *                             will be set to point to the old certificate.
     * \param[in] id Id of the certificate to delete.
     * \return ReturnCode::kNotFound if certificate was not found,
     *         ReturnCode::kError on error and ReturnCode::kSuccess on success.
     */
    ReturnCode DeleteCertificate(X509** old_certificate, const CertificateId id);

    /**
     * \brief Delete the certificate for a specific one to one mapped role.
     *
     * \param[out] old_certificate If not null and operation successful,
     *                             will be set to point to the old certificate.
     * \param[in] role The Role for which to delete the certificate.
     *                 Only one-to-one mappings are allowed, thus
     *                 kUserActor, kUserShortRangeTlsClient,
     *                 kVehicleCa and kCloudCa are invalid input.
     * \return ReturnCode::kNotFound if certificate was not found,
     *         ReturnCode::kInvalidArgument if bad role supplied,
     *         ReturnCode::kError on error and ReturnCode::kSuccess on success.
     */
    ReturnCode DeleteCertificate(X509** old_certificate, const CertificateRole role);

    /**
     * \brief Delete the certificate for a specific one to many mapped role.
     *
     * \param[out] old_certificates If not null and operation successful,
     *                              will be set to point to the old certificate.
     * \param[in] role The Role for which to delete the certificate.
     *                 Only one-to-many mappings are allowed, thus only kUserActor,
     *                 kUserShortRangeTlsClient, kVehicleCa and kCloudCa are
     *                 valid input.
     * \return ReturnCode::kNotFound if certificate was not found,
     *         ReturnCode::kInvalidArgument if bad role supplied,
     *         ReturnCode::kError on error and ReturnCode::kSuccess on success.
     */
    ReturnCode DeleteCertificates(STACK_OF(X509)* old_certificates, const CertificateRole role);

    /**
     * \brief Delete the key for a specific id.
     * \param[out] old_key If not null and operation successful,
     *                     will be set to point to the old key.
     * \param[in] id Id of the key to delete.
     * \return ReturnCode::kNotFound if key was not found,
     *         ReturnCode::kError on error and ReturnCode::kSuccess on success.
     */
    ReturnCode DeleteKey(EVP_PKEY** old_key, const KeyId id);

    /**
     * \brief Delete the key for a specific role.
     * \param[out] old_key If not null and operation successful,
     *                     will be set to point to the old key.
     * \param[in] role The Role for which to delete the key.
     * \return ReturnCode::kNotFound if key was not found,
     *         ReturnCode::kError on error and ReturnCode::kSuccess on success.
     */
    ReturnCode DeleteKey(EVP_PKEY** old_key, const KeyRole role);

    //TODO: Missing ways to clean out all old invalidated certificates/keys
    //      or clear the keystore completely. So for now clients must track
    //      ids if the want to do that.

    /**
     * \brief Check if Certificate role has a one to many mapping.
     *
     * \param[in] role Role to check.
     *
     * \return True if one to many mapping, otherwise false-
     */
    bool IsOneToMany(const Keystore::CertificateRole role);

 private:

    /**
     * \brief Name of the database file.
     */
    const std::string kDatabaseName_ = "keystore.sqlite";

    /**
     * \brief Time sqlite should "retry" databaseoperations before returning SQLIT_BUSy.
     */
    const int kDatabaseTimeout_ = 1000; // timout before giving up on a sql statement in ms

    /**
     * \brief Enumerates possible formats to store a key or certificate reference in the
     *        databases ref column.
     */
    enum class StorageType : int
    {
        kUnknown,         ///< A unknown reference type.
        kInTable = 1,     ///< A PEM formatted certificate or key blob stored in the db.
        kFile = 2,        ///< A PEM fromatted file stored on fs with its path in the db.
        //kPersistance = 3, ///< A PEM formatted file resource in "OPT Persistency", resource identifier in the db.
        //kEncryptedPkcs12
        //kHwKeystore
    };

#if __GNUC__ < 6
    // Encountered compiler which did not have "built in" hash() for enums when
    // using enum as key in map
    struct CertificateRoleHash
    {
        std::size_t operator()(CertificateRole certificate_role) const
        {
            return static_cast<std::size_t>(certificate_role);
        }
    };

    struct KeyRoleHash
    {
        std::size_t operator()(KeyRole key_role) const
        {
            return static_cast<std::size_t>(key_role);
        }
    };

    /**
     * \brief A map of StorageType per CertificateRole.
     */
    typedef std::unordered_map<CertificateRole, std::vector<StorageType>, CertificateRoleHash> CertificateRoleStorageTypeMap;

    /**
     * \brief A map of StorageType per KeyRole.
     */
    typedef std::unordered_map<KeyRole, std::vector<StorageType>, KeyRoleHash> KeyRoleStorageTypeMap;

    /**
     * \brief A map of string per CertificateRole.
     */
    typedef std::unordered_map<CertificateRole, std::string, CertificateRoleHash> CertificateRoleStringMap;

    /**
     * \brief A map of string per KeyRole.
     */
    typedef std::unordered_map<KeyRole, std::string, KeyRoleHash> KeyRoleStringMap;
#else
    /**
     * \brief A map of StorageType per CertificateRole.
     */
    typedef std::unordered_map<CertificateRole, std::vector<StorageType>> CertificateRoleStorageTypeMap;

    /**
     * \brief A map of StorageType per KeyRole.
     */
    typedef std::unordered_map<KeyRole, std::vector<StorageType>> KeyRoleStorageTypeMap;

    /**
     * \brief A map of string per CertificateRole.
     */
    typedef std::unordered_map<CertificateRole, std::string> CertificateRoleStringMap;

    /**
     * \brief A map of string per KeyRole.
     */
    typedef std::unordered_map<KeyRole, std::string> KeyRoleStringMap;
#endif


    /**
     * \brief Lists which StorageTypes are allowed for each certificate role.
     */
    //TODO: For now all is allowed, prune later.
    const CertificateRoleStorageTypeMap kAllowedStorageForCertificateRole_ =
        {{CertificateRole::kVehicleCa, {StorageType::kInTable,
                                        StorageType::kFile}},
         {CertificateRole::kVehicleActor, {StorageType::kInTable,
                                           StorageType::kFile}},
         {CertificateRole::kVehicleShortRangeTlsServer, {StorageType::kInTable,
                                                         StorageType::kFile}},
         {CertificateRole::kUserActor, {StorageType::kInTable,
                                        StorageType::kFile}},
         {CertificateRole::kUserShortRangeTlsClient, {StorageType::kInTable,
                                                      StorageType::kFile}},
         {CertificateRole::kCloudCa, {StorageType::kFile}},
         {CertificateRole::kCloudTlsClient, {StorageType::kFile}},
         {CertificateRole::kDoIpTlsServer, {StorageType::kFile}},
         {CertificateRole::kCloudSignEncrypt, {StorageType::kFile}}};

    /**
     * \brief Lists which StorageTypes are allowed for each key role.
     */
    const KeyRoleStorageTypeMap kAllowedStorageForKeyRole_ =
        {{KeyRole::kVehicleCa, {StorageType::kInTable,
                                StorageType::kFile}},
         {KeyRole::kVehicleActor, {StorageType::kInTable,
                                   StorageType::kFile}},
         {KeyRole::kVehicleShortRangeTlsServer, {StorageType::kInTable,
                                                 StorageType::kFile}},
         {KeyRole::kCloudTlsClient, {StorageType::kFile}},
         {KeyRole::kDoIpTlsServer, {StorageType::kFile}},
         {KeyRole::kCloudSignEncrypt, {StorageType::kFile}}};

#define kCertificatesStoragePath "/certificates"
#define kKeysStoragePath "/keys"
#define kVehicleDomainStoragePath "/vehicle_domain"
#define kConnectedCarDomainStoragePath "/connected_car_domain"
#define kVehicleCertificateStoragePath_ kCertificatesStoragePath kVehicleDomainStoragePath
#define kConnectedCarCertificateStoragePath_ kCertificatesStoragePath kConnectedCarDomainStoragePath
#define kVehicleKeyStoragePath_  kKeysStoragePath kVehicleDomainStoragePath
#define kConnectedCarKeyStoragePath_ kKeysStoragePath kConnectedCarDomainStoragePath

    const CertificateRoleStringMap kStoragePathForCertificateRole_ =
        {{CertificateRole::kVehicleCa, kVehicleCertificateStoragePath_ "/VehicleCa"},
         {CertificateRole::kVehicleActor, kVehicleCertificateStoragePath_ "/VehicleActor"},
         {CertificateRole::kVehicleShortRangeTlsServer, kVehicleCertificateStoragePath_ "/VehicleShortRangeTlsServer"},
         {CertificateRole::kUserActor, kVehicleCertificateStoragePath_ "/UserActor"},
         {CertificateRole::kUserShortRangeTlsClient, kVehicleCertificateStoragePath_ "/UserShortRangeTlsClient"},
         {CertificateRole::kCloudCa, kConnectedCarCertificateStoragePath_ "/CloudCa"},
         {CertificateRole::kCloudTlsClient, kConnectedCarCertificateStoragePath_ "/CloudTlsClient"},
         {CertificateRole::kDoIpTlsServer, kConnectedCarCertificateStoragePath_ "/DoIpTlsServer"},
         {CertificateRole::kCloudSignEncrypt, kConnectedCarCertificateStoragePath_ "/CloudSignEncrypt"}};

    const KeyRoleStringMap kStoragePathForKeyRole_ =
        {{KeyRole::kVehicleCa, kVehicleKeyStoragePath_ "/VehicleCa"},
         {KeyRole::kVehicleActor, kVehicleKeyStoragePath_ "/VehicleActor"},
         {KeyRole::kVehicleShortRangeTlsServer, kVehicleKeyStoragePath_ "/VehicleShortRangeTlsServer"},
         {KeyRole::kCloudTlsClient, kConnectedCarKeyStoragePath_ "/CloudTlsClient"},
         {KeyRole::kDoIpTlsServer, kConnectedCarKeyStoragePath_ "/DoIpTlsServer"},
         {KeyRole::kCloudSignEncrypt, kConnectedCarKeyStoragePath_ "/CloudSignEncrypt"}};

    /**
     * \brief Pointer to the sqlite database, if nullptr we failed to open the database.
     */
    sqlite3* database_;

    /**
     * \brief Cache of previously loaded certificates, indexed by id.
     */
    std::unordered_map<CertificateId, X509*> certificate_cache_;

    /**
     * \brief Mutes governing access to certificate cache.
     */
    std::mutex certificate_cache_mutex_;

    /**
     * \brief Cache of previously loaded certificates, indexed by id.
     */
    std::unordered_map<KeyId, EVP_PKEY*> key_cache_;

    /**
     * \brief Mutes governing access to key cache.
     */
    std::mutex key_cache_mutex_;

    /**
     * \brief Cache of previously prepared statement, indexed by parameterized sql string.
     */
    std::unordered_map<std::string, std::stack<sqlite3_stmt*>> statement_cache_;

    /**
     * \brief Mutes governing access to statement cache.
     */
    std::mutex statement_cache_mutex_;

    /**
     * \brief Get the id of a certificate.
     *        If the same certificate is added for several roles the first match
     *        will be returned.
     * \param[out] id         Will be set to the id of the certificate if found.
     * \param[in] certificate The certificate to return the id for.
     * \return ReturnCode::kNotFound if the id was not found,
     *         ReturnCode::kError on error and ReturnCode::kSuccess on success.
     */
    ReturnCode GetCertificateId(CertificateId& id,
                                const X509* certificate);

    /**
     * \brief Get a prepared statement.
     *
     * \param[out] prepared_statment Will be set to a prepared sqlite3_stmt.
     *                               Caller is expected to return the statement
     *                               using ReturnStatement once finished with
     *                               the operation.
     * \param[in] statement Textual representation of the wanted statement,
     *
     * \return ReturnCode::kError on error and ReturnCode::kSuccess on success.
     */
    ReturnCode GetStatement(sqlite3_stmt** prepared_statement, const std::string& statement);

    /**
     * \brief Return a prepared statement. Using a statement after
     *        it has been returnd might cause bad things to happen.
     *
     * \param[in] statment           The statment to return, nullptr is quietly ignored.
     * \param[in] prepared_statement String used to create the statement.
     *
     * \return None.
     */
    void ReturnStatement(sqlite3_stmt* statement, const std::string& prepared_statement);

    /**
     * \brief Execute statement until a valid row is found
     *        and return the first valid hit.
     *
     * \param[out] reference Will be set to point to the reference if operation successful.
     *                       Pointer is valid until statment gets reset, finalized or stepped,
     *                       do not free! Interpret data based on type.
     * \param[out] num_bytes Returns number of bytes of reference data.
     * \param[out] type      Returns type of reference.
     * \param[out] id        Returns the id of the found reference.
     * \param[in]  statement Statemnt to execute, must be such that the
     *                       columns in the result set are, in order:
     *                       id, type, reference_type, reference, valid.
     *
     * \return ReturnCode:kNotFound if valid reference not found,
     *         ReturnCode::kError on error and ReturnCode::kSuccess on success.
     */
    ReturnCode GetRow(const void** reference,
                      int& num_bytes,
                      StorageType& reference_type,
                      int& type,
                      CertificateId& id,
                      sqlite3_stmt* statement);

    /**
     * \brief Load a certificate.
     *
     * \param[out] certificate Will be set to point to the laoded certificate
     *                         if operation successful.
     * \param[in]  type      Describes how the certificate is stored.
     * \param[in]  reference Reference to the certificate, should be interpreted
     *                       based on type.
     * \param[in]  num_bytes Size of teh reference buffer.
     * \param[in]  id        Id of the certificate beeing loaded.
     *
     * \return ReturnCode::kError on error and ReturnCode::kSuccess on success.
     */
    ReturnCode LoadCertificate(X509** certificate,
                               StorageType type,
                               const void* reference,
                               int num_bytes,
                               CertificateId id);

    /**
     * \brief Load a key.
     *
     * \param[out] key Will be set to point to the laoded certificate
     *                 if operation successful.
     * \param[in]  type      Describes how the key is stored.
     * \param[in]  reference Reference to the key, should be interpreted
     *                       based on type.
     * \param[in]  num_bytes Size of the reference buffer.
     * \param[in]  id        Id of the key beeing loaded.
     *
     * \return ReturnCode::kError on error and ReturnCode::kSuccess on success.
     */
    ReturnCode LoadKey(EVP_PKEY** key,
                       StorageType type,
                       const void* reference,
                       int num_bytes,
                       KeyId id);

    /**
     * \brief Get a certificate.
     *
     * \param[out] certificate Will be set to point to the certificate
     *                         if operation successful.
     * \param[in]  statement SQL statement to retrieve the certificate.
     *             First valid match will be returned.
     * \param[out] id          If not nullptr, will be set to the id of the certificate,
     *                         if found.
     * \param[out] role        If not nullptr, will be set to the role of the certificate,
     *                         if found.
     *
     * \return ReturnCode::kNotFound if not found, ReturnCode::kError on error
     *         and ReturnCode::kSuccess on success.
     */
    ReturnCode GetCertificate(X509** certificate,
                              sqlite3_stmt* statement,
                              CertificateId* id = nullptr,
                              CertificateRole* role = nullptr);

    /**
     * \brief Get a list of certificates.
     *
     * \param[out] certificates Will be populated with certificates
     *                          if operation successful.
     * \param[in]  statement SQL statement to retrieve the certificates.
     *                       All valid matches will be returned.
     *
     * \return ReturnCode::kNotFound if none found, ReturnCode::kError on error
     *         and ReturnCode::kSuccess on success.
     */
    ReturnCode GetCertificates(STACK_OF(X509)* certificates, sqlite3_stmt* statement);

    /**
     * \brief Get a key.
     *
     * \param[out] key Will be set to point to the key
     *                 if operation successful.
     * \param[in]  statement SQL statement to retrieve the key.
     *             First valid match will be returned.
     * \param[out] id  If not nullptr will be set to the id of the key, if found.
     *
     * \return ReturnCode::kNotFound if not found, ReturnCode::kError on error
     *         and ReturnCode::kSuccess on success.
     */
    ReturnCode GetKey(EVP_PKEY** key, sqlite3_stmt* statement, KeyId* id = nullptr);

    /**
     * \brief Stores a list of certificates in a store.
     *
     * \param[out] store Will be set to point to the store
     *                   if operation successful.
     * \param[in]  certificates The certificates to store.
     *
     * \return ReturnCode::kError on error and ReturnCode::kSuccess on success.
     */
    ReturnCode StoreCertificatesInStore(X509_STORE** store, STACK_OF(X509)* certificates);

    /**
     * \brief Invalidates a row.
     *
     * \param[in] row_id The id of the row to invalidate.
     *
     * \return ReturnCode::kError on error and ReturnCode::kSuccess on success.
     */
    ReturnCode InvalidateRow(int row_id);

    /**
     * \brief Deletes a row.
     *
     * \param[in] row_id The id of the row to invalidate.
     *
     * \return ReturnCode::kError on error and ReturnCode::kSuccess on success.
     */
    ReturnCode DeleteRow(int row_id);

    /**
     * \brief Create a temporary file.
     *
     * \param[out] path Will be populated with path to created file.
     *
     * \return pointer to file, or nullptr on failure.
     */
    FILE* CreateTemporaryFile(std::string& path);

    /**
     * \brief Create a permanent file in the keystore file storage.
     *
     * \param[out] path         Absolute path to the created file.
     * \param[in] relative_path Storage location realtive to the keystore root storage.
     * \param[in] filename      Name of the file to create.
     *
     * \return pointer to file, or nullptr on failure.
     */
    FILE* CreateFile(std::string& path, std::string relative_path, std::string filename);

    /**
     * \brief Creates a file in peristance and stores a certificate in it.
     *
     * \param[out] path        Path to the created file.
     * \param[in]  certificate The certificate to store.
     * \param[in]  role        Certificate role.
     */
    ReturnCode StoreCertificateInFile(std::string& path, X509* certificate,  CertificateRole role);

    /**
     * \brief Creates a file in peristance and stores a key in it.
     *
     * \param[out] path Path to the created file.
     * \param[in]  key  The key to store.
     * \param[in]  role Key role.
     */
    ReturnCode StoreKeyInFile(std::string& path, EVP_PKEY* key,  KeyRole role);

    /**
     * \brief Stores a key or certificate as a blob inside the table.
     *
     * \param[in] blob_data      Data buffer to insert.
     * \param[in] blob_data_size Size of the buffer in bytes.
     * \param[in] type           Certificate/Key role.
     * \param[in] storage_type   Data type inside the blob, defaults to "in table".
     *
     * \return ReturnCode::kError on error and ReturnCode::kSuccess on success.
     */
    ReturnCode InsertBlobEntry(const void* blob_data,
                               int blob_data_size,
                               int type,
                               StorageType storage_type = StorageType::kInTable);

    /**
     * \brief Stores a Certificate in a file and stores its path inside the table.
     *
     * \param[in] certificate The certificate to insert.
     * \param[in] role        Certificate role.
     *
     * \return ReturnCode::kError on error and ReturnCode::kSuccess on success.
     */
    ReturnCode InsertCertificateFile(X509* certificate, CertificateRole role);

    /**
     * \brief Insert a certificate into the table.
     *
     * \param[in] certificate The certificate to insert.
     * \param[in] role        The role the certificate should have.
     *
     * \return ReturnCode::kError on error and ReturnCode::kSuccess on success.
     */
    ReturnCode InsertCertificate(X509* certificate,
                                 const CertificateRole role);

    /**
     * \brief Stores a Key in a file and stores its path inside the table.
     *
     * \param[in] key  The key to insert.
     * \param[in] role Key role.
     *
     * \return ReturnCode::kError on error and ReturnCode::kSuccess on success.
     */
    ReturnCode InsertKeyFile(EVP_PKEY* key, KeyRole role);

    /**
     * \brief Insert a key into the table.
     *
     * \param[in] key  The key to insert.
     * \param[in] role The role the key should have.
     *
     * \return ReturnCode::kError on error and ReturnCode::kSuccess on success.
     */
    ReturnCode InsertKey(EVP_PKEY* key, const KeyRole role);

    /**
     * \brief check if type retrieved from DB is a certificate type.
     *
     * \param[in] type Type to check.
     *
     * \return True if certificate type, otherwise false.
     */
    bool IsCertificateType(int type);

    /**
     * \brief check if type retrieved from DB is a key type.
     *
     * \param[in] type Type to check.
     *
     * \return True if key type, otherwise false.
     */
    bool IsKeyType(int type);

    void RemoveFromCertificateCache(const CertificateId id);

    void AddToCertificateCache(const CertificateId id, X509* certificate);

    bool GetFromCertificateCache(const CertificateId id, X509** certificate);

    void RemoveFromKeyCache(const KeyId id);

    void AddToKeyCache(const KeyId id, EVP_PKEY* key);

    bool GetFromKeyCache(const KeyId id, EVP_PKEY** key);

    void AddToStatementCache(const std::string& statement, sqlite3_stmt* prepared_statement);

    bool GetFromStatementCache(const std::string& statement, sqlite3_stmt** prepared_statement);

    ReturnCode CheckKeySize(EVP_PKEY *key, const KeyRole role);

    ReturnCode CheckCertificateKeySize(X509 *certificate, const CertificateRole role);

    bool IsStorageTypeAllowedForCertificateRole(StorageType type, CertificateRole role);

    bool IsStorageTypeAllowedForKeyRole(StorageType type, KeyRole role);
};

} // namespace user_manager

#endif // FSM_KEYSTORE_H

/** \}    end of addtogroup */
