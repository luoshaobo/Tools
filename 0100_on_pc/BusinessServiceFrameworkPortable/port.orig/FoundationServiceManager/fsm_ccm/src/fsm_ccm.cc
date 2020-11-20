/**
 * Copyright (C) 2016-2018 Continental Automotive GmbH
 *
 * Licensed under the "Supplier unrestricted white box" category, as
 * described in the 20160322-1_SW-amendment TCAM.pdf agreement
 * between Volvo Cars and Continental Automotive GmbH.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *  \file     fsm_ccm.cc
 *  \brief    Foundation Services Connectivity Compact Messages.
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup foundationservicemanager
 *  \{
 */


/******************************************************************************
 * INCLUDE FILES
 ******************************************************************************/

#include "fsm_ccm.h"

//ASN1C
#include "CCM-Header.h"
#include "CCM-Payload.h"

//CPP headers
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <random>

//POSIX headers
#include <unistd.h>

//OpenSSL headers
#include <openssl/bio.h>
#include <openssl/cms.h>
#include <openssl/err.h>
#include <openssl/objects.h>
#include <openssl/pem.h>
#include <openssl/rand.h>

//DLT header
#include "dlt/dlt.h"

//Custom headers
#include "keystore.h"
#include "usermanager_interface.h"


DLT_IMPORT_CONTEXT(dlt_libfsm);

/******************************************************************************
 * DEFINES
 ******************************************************************************/

static const int kSignFlags = CMS_BINARY | CMS_PARTIAL | CMS_NOSMIMECAP | CMS_NOATTR | CMS_NOCERTS | CMS_USE_KEYID;
static const int kEncryptionFlags = CMS_BINARY | CMS_USE_KEYID | CMS_PARTIAL;

static const unsigned int kFsOidMaxNodes = 13;

static const int kEnvelopedDataContentInfoSize = 19;
static const int kEnvelopedDataOidAndEcontentSize = 15; // kEnvelopedDataContentInfoSize - Sequnce "header"

/*** opensssl structs start ***/
// this is a hack
// openssl does not provide function to manipulate
// unprotected attributes, so we define copies of
// openssl internal structures here and access them
// directly
typedef struct CMS_EnvelopedData_st CMS_EnvelopedData;
typedef struct CMS_OriginatorInfo_st CMS_OriginatorInfo;
typedef struct CMS_EncryptedContentInfo_st CMS_EncryptedContentInfo;


struct CMS_ContentInfo_st {
    ASN1_OBJECT *contentType;
    union {
        CMS_EnvelopedData *envelopedData;
    } d;
};

struct CMS_EnvelopedData_st {
    long version;
    CMS_OriginatorInfo *originatorInfo;
    STACK_OF(CMS_RecipientInfo) *recipientInfos;
    CMS_EncryptedContentInfo *encryptedContentInfo;
    STACK_OF(X509_ATTRIBUTE) *unprotectedAttrs;
};
/*** opensssl structs end ***/

// ASN1C inline structs
typedef struct CCM_Payload::metadata metadata_t;

// keystore shorthands
typedef fsm::ReturnCode KeyStoreReturnCode;
typedef fsm::Keystore::CertificateRole CertificateRole;
typedef fsm::Keystore::KeyRole KeyRole;

//supported types of OIDs, the list is extensible but always leave the last two intact (see comments)
//this list indicates preference order in encoding where most compact encoding is prefered
typedef enum
{
    CCM_BASIC_CAR_STATUS = 0,
    CCM_VCC,   //keep next last, add new feature specific oid domains before this one
    CCM_FULL   //keep last
} CcmOidChoices;

/******************************************************************************
 * GLOBAL VARIABLES AND CONSTANTS
 ******************************************************************************/

static const char* kCCMHeaderOID = "1.3.6.1.4.1.37916.6.1.1";
static const char* kCCMPayloadOID = "1.3.6.1.4.1.37916.6.1.2";

//matching between oid choices and corresponsing partial OIDs (OID prefixes)
//the order must correspond to the order in CcmOidChoices enum
static const char* kOidMatch[CCM_FULL] = {kBasicCarStatusOidDomain, kVolvoOidDomain};


/******************************************************************************
 * STATIC FUNCTION PROTOTYPES
 ******************************************************************************/

static int write_encoded_bytes(const void *buffer, size_t size, void *clientdata);

static bool CompareOctetSrings(const OCTET_STRING_t *str, const OCTET_STRING_t *other_str);

//funtions to convert between asn1c and ccm.h types
static bool CCMEncoding2ASN1C (fs_Encoding ccm_encoding, Encoding_t* asn1c_encoding);
static void ASN1C2CCMEncoding (Encoding_t* asn1c_encoding, fs_Encoding* ccm_encoding);

static void CCMVersionInfo2ASN1C (const fs_VersionInfo* ccm_version, VersionInfo_t* asn1c_version);
static void ASN1C2CCMVersionInfo (VersionInfo_t* asn1c_version, fs_VersionInfo* ccm_version);

static bool CCMStoreAndForwardOptions2ASN1C (ccm_storeAndForwardOptions ccm_option,
                                             StoreAndForwardOptions_t* asn1c_option);
static void ASN1C2CCMStoreAndForwardOptions (StoreAndForwardOptions_t* asn1c_option,
                                             ccm_storeAndForwardOptions* ccm_option);

static bool CCMSmsDeliveryOptions2ASN1C (ccm_smsDeliveryOptions ccm_option,
                                         SmsDeliveryOptions_t* asn1c_option);
static void ASN1C2CCMSmsDeliveryOptions (SmsDeliveryOptions_t* asn1c_option,
                                         ccm_smsDeliveryOptions* ccm_option);

static void CCMDeliveryOptions2ASN1C (const ccm_DeliveryOptions* ccm_options,
                                      DeliveryOptions_t* asn1c_options);
static void ASN1C2CCMDeliveryOptions (DeliveryOptions_t* asn1c_options,
                                      ccm_DeliveryOptions* ccm_options);

static void CCMReceiptOptions2ASN1C (const ccm_ReceiptOptions* ccm_options,
                                     ReceiptOptions_t* asn1c_options);
static void ASN1C2CCMReceiptOptions (ReceiptOptions_t* asn1c_options,
                                     ccm_ReceiptOptions* ccm_options);

static bool ASN1C2CCMAddress (AddressCCM_t* asn1c_address, ccm_Address* ccm_address);

static bool CCMHeader2ASN1C (const ccm_Header* ccm_header, CCM_Header_t* asn1c_header);

static bool ASN1C2CCMHeader (CCM_Header_t* asn1c_header, ccm_Header* ccm_header);

static bool CCMContentType2ASN1C (const fs_Oid* ccm_oid, ContentType_t* asn1c_content_type);

static bool ASN1C2CCMContentType (ContentType_t* asn1c_content_type, fs_Oid* ccm_oid);

static bool CCMEnvelope2ASN1C (const ccm_Envelope* ccm_envelope, Envelope_t* asn1c_envelope);

static bool ASN1C2CCMEnvelope (Envelope_t* asn1c_envelope, ccm_Envelope* ccm_envelope);

static bool CCMPayload2ASN1C (const ccm_Payload* payload, CCM_Payload_t* asn1c_payload);

static bool ASN1C2CCMPayload (CCM_Payload_t* asn1c_payload, ccm_Payload* payload);


// functions to create, encode/decode ccm messages
/** \brief encodes a CCM_Payload_t structure
 *  \param[in] asn1c_payload - The payload to encode
 *  \param[out] payload_data - A buffer to hold the encoded payload will be allocated and returned.
 *                             Caller must free!
 *  \return false if unsuccessful
 */
static bool EncodePayload (CCM_Payload_t* asn1c_payload, fs_Data* payload_data);

/** \brief encodes a CCM_Header_t structure
 *  \param[in] asn1c_header - The header to encode
 *  \param[out] header_data - A buffer to hold the encoded header will be allocated and returned.
 *                           Caller must free!
 *  \return false if unsuccessful
 */
static bool EncodeHeader (CCM_Header_t* asn1c_header, fs_Data* header_data);

/** \brief decodes a buffer into a CCM_Header_t structure
 *  \param[in/out] asn1c_header - The header to populate
 *  \param[in] buf             - A buffer to decode
 *  \param[in] len             - Length of buffer
 *  \return false if unsuccessful
 */
static bool DecodeHeader (CCM_Header_t** asn1c_header, fs_Data* header_data);

/** \brief adds a encoded ccm_Header into the signedAttributes of a signedData
 *  \param[in/] signed_data - The signedData in which to add the header
 *  \param[in] buf         - encoded header
 *  \param[in] len         - Length of header buffer
 *  \return false if unsuccessful
 */
static bool SetHeaderSigned (CMS_ContentInfo* signed_data, fs_Data* header);

/** \brief adds a encoded ccm_Header into the unprotectedAttributes of a envelopedData
 *  \param[in] envelopedData - The envelopedData in which to add the header
 *  \param[in] header           - encoded header
 *  \return false if unsuccessful
 */
static bool SetHeaderEncrypted (CMS_ContentInfo* enveloped_data, fs_Data* header);

/** \brief adds each recipients and respective subject key identifier to the ASN1C header.
 *  \param[in] users - List of recipients
 *  \param[in/out] header - ASN1C CCM header to add recipients and SKIs.
 *  \return false if unsuccessful
 */
static bool AddRecipientsToASN1CHeader(const std::vector<fsm::UserId>& users, CCM_Header_t* header);

/**
 * \brief Populates the sender field in the CCM Header with the SKI of the vehicle credentials
 *        used to sign the message.
 * \param[in] counterparts The recipients of the message, needed to establish which credentials to use.
 * \param[in] header       The header to populate.
 * \return false if unsuccessful
 */
static bool AddSenderToASN1CHeader(const std::vector<fsm::UserId>& counterparts, CCM_Header_t* header);

/** \brief Extracts subject key identifier from the given certificate.
 *  \param[in] certificate - The given certificate
 *  \param[out] subject_key_identifier - SKI extracted from the given certificate
 *  \return false if unsuccessful
 */
static bool GetSubjectKeyIdentifier(X509* certificate, OCTET_STRING_t* subject_key_identifier);

/** \brief retrieves a encoded ccm_Header from the signedAttributes of a signedData
 *  \param[in]     signed_data - The signedData to extract the header from
 *  \param[in/out] header     - encoded header will be stored in this container
 *                              caller must free.
 *  \return false if unsuccessful
 */
static bool GetHeaderSigned (CMS_ContentInfo* signed_data, fs_Data* header);

/** \brief retrieves a encoded ccm_Header from the unprotectedAttributes of a envelopedData
 *  \param[in]     enveloped_data - The envelopedData to extract the header from
 *  \param[in/out] header     - encoded header will be stored in this container
 *                              caller must free.
 *  \return false if unsuccessful
 */
static bool GetHeaderEncrypted (CMS_ContentInfo* enveloped_data, fs_Data* header);

/** \brief extracts contents from a signedData
 * \param[in]     signed_data the signedData to extract contents from
 * \param[in/out] data        will be filled with signeData contents
 * \return false id unsuccessful
 */
static bool GetSignedData (CMS_ContentInfo* signed_data, fs_Data* data);

/** \brief Creates a envelopedData and encrypts the content,
 *         adds encoded header to unprotectedAttributes and
 *         sets the encryptedContentInfo oid
 *  \param[out] encrypted - A bio with the encoded and encrypted envelopedData
 *                          will be created and returned.
 *                          Caller must free.
 *  \param[in] data       - data to encrypt
 *  \param[in] header     - header to add
 *  \param[in] oid        - oid to set
 *  \param[in] recipients - recipients for which to encrypt
 *  \return false if unsuccessful
 */
static bool EncryptData (BIO** encrypted,
                         fs_Data* data,
                         fs_Data* header,
                         ASN1_OBJECT* oid,
                         const std::vector<fsm::UserId>& recipients);

/** \brief Decrypts the contents of a envelopedData.
 *  \param[in] data           - will be filled with the decrypted data
 *                              Caller must free the buffer.
 *  \param[in] enveloped_data - A CMS_ContentInfo representing the envelopedData to decrypt.
 *  \param[in] sender - sender of the message to be decrypted
 *  \return false if unsuccessful
 */
static bool DecryptData (fs_Data* data,
                         CMS_ContentInfo* enveloped_data,
                         fsm::UserId sender);

/** \brief Creates a signedData and signs the content. Also optionally
 *         adds encoded header to signerInfos if we are signing a unencrypted
 *         message.
 *  \param[out] signedBio - A bio with the signed signedData will be created and returned.
 *                          Caller must free.
 *  \param[in] data       - data to sign
 *  \param[in] header     - header to add, if not NULL
 *  \param[in] oid        - oid to set
 *  \param[in] recipients - recipients of the message
 *  \return false if unsuccessful
 */
static bool SignData (BIO** signed_bio,
                      BIO* data,
                      fs_Data* header,
                      ASN1_OBJECT* oid,
                      const std::vector<fsm::UserId>& recipients);

/** \brief Decodes data into a signedData and verifies the signature
 *  \param[out] signed_data       - A CMS_ContentInfo with the verified data will be
                                   created and returned.
 *                                 Caller must free.
 *  \param[out] contentBio       - bio to write contents of verified data to
 *  \param[in]  data             - data to decode and verify
 *  \return false if unsuccessful
 */
static bool VerifyData (CMS_ContentInfo** signed_data,
                        BIO* content_bio,
                        const_fs_Data* data);

/** \brief Verify that the sender's SKI from decoded header matches to SKI from the sender's certificate.
 *  \param[in]  header_ski - A SKI from decoded header
 *  \param[in]  sender - A userId to extract certificate and SKI.
 *  \return false if unsuccessful
 */
static bool VerifySenderSKI(const OCTET_STRING_t* header_ski, const fsm::UserId& sender);

/** \brief Create a ccm_Message and fill it with content from data.
 *         Contents will be copied. Caller must free.
 *  \param[in]     data - The contents
 *  \param[in/out] oid  - ContentType of the contents
 *  \return The created message, or NULL on failure
 */
static ccm_Message* CreateMessage (fs_Data* data, const ASN1_OBJECT* oid);


// functions to interact with keystore and usermanager


/** \brief Get the user manager instance used by fsm_ccm
 *         we keep it around between invocations to benefit from caching
 *         inside it.
 *  \return shared_ptr to Usermanager if successful, or shared_ptr to null.
 */
static std::shared_ptr<fsm::UsermanagerInterface> GetUsermanager();

/** \brief Get the keystore instance used by fsm_ccm
 *         we keep it around between invocations to benefit from caching
 *         inside it.
 *  \return shared_ptr to Keystore if successful, or shared_ptr to null.
 */
static std::shared_ptr<fsm::Keystore> GetKeystore();

/** \brief Extracts sender from CCM.
 *  \param[out] sender      - sender, or kUndefinedUserId if not found
 *  \param[in]  signed_data - CMS_ContentInfo with the verified data
 *  \return false if unsuccessful
 */
static bool GetSender(fsm::UserId& sender, CMS_ContentInfo* signed_data);

/** \brief Retrieves the list of known signers from keystore.
 *  \param[in/out] known_signers - populated with all known signers.
 *  \return false if unsuccessful
 */
static bool GetKnownSigners(STACK_OF(X509)* known_signers);

/** \brief Retrieves the vehicle credentials to use when encoding for
 *         a given set of counterparts.
 *         Depending on which domain the recipinets are in, vehicel or
 *         connected car we should use corresponding credentials.
 *         A mixed set of counterparts in both domains is invalid input.
 *  \param[out] cert         - certificate to use during encode, caller must free
 *  \param[out] key          - key to use during encode, caller nust free
 *  \param[in]  counterparts - list of counterparts
 *  \return false if unsuccessful
 */
static bool GetVehicleCredentials(X509** cert,
                                  EVP_PKEY** key,
                                  const std::vector<fsm::UserId>& counterparts);

/** \brief Retrieves known CAs to validate a CCM against.
 *  \param[out] ca_store - populated with CAs, caller must free.
 *  \return false if unsuccessful
 */
static bool GetCas(X509_STORE** ca_store);

/** \brief Retrieves the actor certificates for a list of users.
 *  \param[in/out] certificates - populated with actor certificates, caller must free
 *  \param[in]     users        - users to retrieve certificates for
 *  \return false if unsuccessful
 */
static bool GetActorCerts(STACK_OF(X509)* certificates, const std::vector<fsm::UserId>& users);


/******************************************************************************
 * GLOBAL FUNCTIONS
 ******************************************************************************/

bool ccm_IsValidOid(const char* oid_string)
{
    if (!oid_string)
    {
        return false;
    }

    size_t oid_length = strlen(oid_string);

    if (oid_length < FS_OID_MIN_LEN || oid_length > FS_OID_MAX_LEN)
    {
        return false;
    }

    return strncmp(oid_string, kVolvoOidDomain, FS_OID_VOLVO_DOMAIN_LEN) == 0;
}

bool ccm_SetOid(fs_Oid* oid, const char* oid_string)
{
    if (oid && ccm_IsValidOid(oid_string))
    {
        strncpy(oid->oid, oid_string, FS_OID_MAX_LEN);
        return true;
    }
    return false;
}

bool ccm_IsValidVersionInfo(const fs_VersionInfo* version_info)
{
    if (!version_info)
        return false;

    return version_info->schema_version > -1;
}

bool ccm_CompareVersionInfo(const fs_VersionInfo* version_info_one,
                            const fs_VersionInfo* version_info_two)
{
    if (!version_info_one || !version_info_two)
    {
        return false; //two null pointers is bad input, not equality
    }

    bool equal = true;

    equal = equal && version_info_one->schema_version == version_info_two->schema_version;

    // preliminary version and signal flow version is optional only compare if set in both
    if (version_info_one->signal_flow_version != -1 && version_info_two->signal_flow_version != -1)
    {
        equal = equal && version_info_one->signal_flow_version == version_info_two->signal_flow_version;
    }
    if (version_info_one->preliminary_version != -1 && version_info_two->preliminary_version != -1)
    {
        equal = equal && version_info_one->preliminary_version == version_info_two->preliminary_version;
    }

    return equal;
}

bool ccm_Decode (const_fs_Data* data,
                 ccm_Message** message,
                 fsm::UserId& sender,
                 STACK_OF(X509)** certificates,
                 unsigned int flags)
{
    bool return_value = true;
    ccm_Message* out_message = NULL;


    BIO* verified_bio = BIO_new(BIO_s_mem());

    CMS_ContentInfo* signed_data = NULL;
    CMS_ContentInfo* verified_data = NULL;

    CCM_Header_t* header = NULL;
    fs_Data header_data = {0};
    fs_Data message_data = {0};
    // in case of encryption we will take ownership of decrypted buffer from openssl
    // and must free the returned buffers
    bool should_free_message_data = false;

    const ASN1_OBJECT* content_type;
    const ASN1_OBJECT* content_type_to_set;

    DLT_LOG_STRING(dlt_libfsm, DLT_LOG_INFO, "ccm_Decode start");

    // verify input is sane
    if (!data || !(data->data) || data->num_bytes == 0)
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "invalid input, data is null or points to non null or is empty.");
        return_value = false;
    }
    else if (!message)
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "invalid input, message is null.");
        return_value = false;
    }
    else if (certificates && *certificates)
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "invalid input, certificates does not point to null");
        return_value = false;
    }

    //TODO: We may have to check for gzip and decompress here
    //      but spec is currently unclear. Ignored until we are
    //      aware of a client which does compress.

    // decode and verify input data
    if (return_value)
    {
        return_value = VerifyData(&signed_data,
                                  verified_bio,
                                  data);
        if (return_value)
        {
            DLT_LOG_STRING(dlt_libfsm, DLT_LOG_VERBOSE, "signature verified");
        }
        else
        {
            DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "signature verify fail");
        }

        if (return_value)
        {
            // Workaround, we should store return_value but dont as verify might have failed
            //            return_value = GetSender(sender, signed_data);
            GetSender(sender, signed_data);
        }

        if (return_value && certificates)
        {
            *certificates = CMS_get1_certs(signed_data);
        }
    }

    // Determine whether the message is encrypted or not.
    // According to the CCM spec the content of encapContentInfo will be
    // a EnvelopedData if it is encrypted. If it is not encrypted the
    // content will be either the actual message or a CCM-Payload, and
    // the encapsulated content type will be set accordingly.
    // Read out header and message contents as appropriate for each case
    // and store in header_data, message_data.
    if (return_value)
    {
        // Handle CCM - OPENSSL ContentInfo handling incompatability
        // CCM expects The signedData.encapContentInfo.eContent to directly
        // contain a EnvelopedData.
        // Openssl only has support for der encoding/decoding a EnvelopedData
        // as part of a ContentInfo via d2i/i2d_CMS, i.e. the expect/produce
        // something wrapped in a ContentInfo.
        //
        // Ta handle this we strip the ContentInfo wrapper after:
        // cms_encrypt()
        // i2d_CMS_bio()
        // before sending.
        //
        // When received we need to prepend the wrapper after:
        // cms_verify but before:
        // d2i_CMS_bio()
        //
        // The wrapper is a 19 bytes:
        // 0x30, 0x82, a, b "ContentInfo" sequence tag and length of contained data encoded in a & b
        // 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x07, 0x03 EnvelopedData OID
        // 0xa0, 0x82, c, d eContent tag with length of contained data encoded in c & d

        //TODO: push for change to CCM spec so we can get rid of this

        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_WARN, "Inserting EnvelopedData ContentInfo wrapper.\n");

        BIO *tmp = BIO_new(BIO_s_mem());

        unsigned char buf[] = {0x30, 0x82, 0x00, 0x00, //sequence
                               0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x07, 0x03, //EnvelopedData oid
                               0xa0, 0x82, 0x00, 0x00}; //eContent

        char* verified_bio_buf = NULL;

        long verified_bio_size = BIO_get_mem_data(verified_bio, &verified_bio_buf);

        buf[2] = ((verified_bio_size + kEnvelopedDataOidAndEcontentSize) >> 8) & 0xff;
        buf[3] = (verified_bio_size + kEnvelopedDataOidAndEcontentSize) & 0xff;

        buf[17] = (verified_bio_size >> 8) & 0xff;
        buf[18] = verified_bio_size & 0xff;

        BIO_write(tmp, buf, kEnvelopedDataContentInfoSize);
        BIO_write(tmp, verified_bio_buf, verified_bio_size);

        BIO_free(verified_bio);
        verified_bio = tmp;
        tmp = NULL;

        content_type = CMS_get0_eContentType(signed_data);
    }

    if (return_value && (OBJ_obj2nid(content_type) == NID_pkcs7_enveloped))
    {
        // contents are encrypted

        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_VERBOSE, "found encrypted content");

        if (return_value)
        {
            verified_data = d2i_CMS_bio(verified_bio, &verified_data);
        }
        if (verified_data)
        {
            return_value = DecryptData(&message_data, verified_data, sender);
            if (return_value)
            {
                should_free_message_data = true;
                content_type_to_set = CMS_get0_eContentType(verified_data);
                return_value = GetHeaderEncrypted(verified_data, &header_data);
                if (return_value)
                {
                    DLT_LOG_STRING(dlt_libfsm, DLT_LOG_VERBOSE, "header encrypted");
                }
                else
                {
                    DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "header encrypt fail");
                }
            }
            else
            {
                DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "enveloped data not decrypt");
            }
        }
        else
        {
            return_value = false;
            DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "header not encrypted");
        }
    }
    else if (return_value)
    {
        // contents are a plain message
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_VERBOSE, "found signed content");

        // OID is the already known contentType from encapContentInfo
        content_type_to_set = content_type;

        // verify received oid is valid
        fs_Oid oid;
        OBJ_obj2txt(oid.oid, FS_OID_MAX_LEN, content_type_to_set, 1);
        if (!ccm_IsValidOid(oid.oid))
        {
            DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "received invalid message oid");
            return_value = false;
        }

        if (return_value)
        {
            return_value = GetSignedData(signed_data, &message_data);
            if (return_value)
            {
                DLT_LOG_STRING(dlt_libfsm, DLT_LOG_VERBOSE, "get message signed");

                return_value = GetHeaderSigned(signed_data, &header_data);
                if (return_value)
                {
                    DLT_LOG_STRING(dlt_libfsm, DLT_LOG_VERBOSE, "get header signed");
                }
                else
                {
                    DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "get header signed fail");
                }
            }
            else
            {
                DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "get message signed fail ");
            }
        }
    }

    // parse the CCM header
    if (return_value)
    {
        return_value = DecodeHeader(&header, &header_data);
        if (return_value)
        {
            DLT_LOG_STRING(dlt_libfsm, DLT_LOG_VERBOSE, "parse the CCM header");
        }
        else
        {
            DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "parse the CCM header fail");
        }
    }

    // Verify Sender SKI, if set
    if (return_value
        && (sender != fsm::kUndefinedUserId)
        && (header->sender)
        && (header->sender->present == AddressCCM_PR_subjectKeyIdentifier))
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_VERBOSE, "Verifying sender SKI");
        return_value = VerifySenderSKI(&(header->sender->choice.subjectKeyIdentifier), sender);
        if (return_value)
        {
            DLT_LOG_STRING(dlt_libfsm, DLT_LOG_VERBOSE, "sender's SKI match");
        }
        else
        {
            DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "sender's SKI not match");
        }
    }

    // allocate message structure and fill it with contents from message_data
    if (return_value)
    {
        out_message = CreateMessage(&message_data, content_type_to_set);
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_VERBOSE, "Creating ccm message");
    }
    if (!out_message)
    {
        return_value = false;
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "not allocate message structure");
    }

    // write header into message
    if (return_value)
    {
        return_value = ASN1C2CCMHeader(header, &(out_message->header));
        if (return_value)
        {
            DLT_LOG_STRING(dlt_libfsm, DLT_LOG_VERBOSE, "write header into message");
        }
        else
        {
            DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "write header into message fail");
        }
    }

    // Populate out_message->payload.content->content_encoding and content_version
    // with header values if they were left with defaults by CreateMessage.
    // (should mean a non CCM-Payload)
    if (return_value &&
        !ccm_IsValidVersionInfo(&(out_message->payload.content.content_version)) &&
        out_message->payload.content.content_encoding == FS_ENCODING_UNDEFINED)
    {
        out_message->payload.content.content_version = out_message->header.content_version;
        out_message->payload.content.content_encoding = out_message->header.content_encoding;
    }

    // message is now fully decoded, yay!

    if (header)
    {
        asn_DEF_CCM_Header.free_struct(&asn_DEF_CCM_Header, header, 0);
    }

    if (out_message && !return_value)
    {
        delete out_message;
        out_message = NULL;
    }

    if (should_free_message_data)
    {
        OPENSSL_free(message_data.data);
    }

    //all of these should be NULL safe
    CMS_ContentInfo_free(signed_data);
    CMS_ContentInfo_free(verified_data);
    BIO_free(verified_bio);

    if (return_value)
    {
        *message = out_message;
    }
    else
    {
        if (message)
        {
            *message = nullptr;
        }

        if (certificates)
        {
            if (*certificates)
            {
                sk_X509_pop_free(*certificates, X509_free);
            }
            *certificates = nullptr;
        }
    }

    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "ccm_Decode %s.",
                    return_value ? "succeded" : "failed");

    return return_value;
}

bool ccm_Encode (fs_Data* data,
                 const ccm_Message* message,
                 const std::vector<fsm::UserId>& recipients,
                 unsigned int flags)
{
    bool return_value = true;

    BIO* dataBio = NULL;
    BIO* outBio = NULL;

    fs_Data header_data = {0};
    fs_Data message_data = {0};

    ASN1_OBJECT* message_content_type = NULL;

    CCM_Header_t header = {0};

    DLT_LOG_STRING(dlt_libfsm, DLT_LOG_INFO, "ccm_Encode start");

    // verify input is sane
    if (!data
        || !message
        || message->payload.content.content == nullptr
        || !ccm_IsValidOid(message->payload.content.oid.oid))
    {
        return_value = false;
    }

    if (!return_value)
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "invalid input");
    }
    else
    {
        // setup header
        return_value = CCMHeader2ASN1C(&(message->header), &header);
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_VERBOSE, "setup header");

        if (return_value)
        {
            // In order for AddSenderToASN1CHeader to be able to figure out which
            // vehicle credentials to use as sender it needs the list of recipients
            // (as what credentials we should use depends on whether we are sending
            // to vehicle or connected car domain).
            // TODO: It would perhaps be nicer to estabölish this once and pass the
            //       credentials down, Currently we call GetVehicleCredentials
            //       atleast twice during encode.
            return_value = AddSenderToASN1CHeader(recipients, &header);
            if (return_value)
            {
                DLT_LOG_STRING(dlt_libfsm, DLT_LOG_VERBOSE, "add sender to asn1 header");
            }
            else
            {
                DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "add sender to asn1 header fail");
            }
        }
        else
        {
            DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "header not setup");
        }

        if (return_value &&
            (flags & CCM_DO_NOT_ENCRYPT) &&
            !(recipients.empty()))
        {
            // Add recipients and SKI to asn1 header if not provided by client.
            return_value = AddRecipientsToASN1CHeader(recipients, &header);
            if (return_value)
            {
                DLT_LOG_STRING(dlt_libfsm, DLT_LOG_VERBOSE, "recipients and SKI added to asn1 header");
            }
            else
            {
                DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "add recipients and SKI to asn1 header fail");
            }
        }

        if (return_value)
        {
            return_value = EncodeHeader(&header, &header_data);
            if (return_value)
            {
                DLT_LOG_STRING(dlt_libfsm, DLT_LOG_VERBOSE, "header encoded");
            }
            else
            {
                DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "header encoded fail");
            }
        }
        else
        {
            DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "header not encoded");
        }
    }

    asn_DEF_CCM_Header.free_struct(&asn_DEF_CCM_Header, &header, 1);

    // examine input parameters and decide what to do:
    // - if recipients is NULL we should not encrypt
    // - if numMetadata != 0 or force falg is set we
    //   should encode a CCM-Payload

    if (return_value && ((message->payload.num_metadata > 0) || (flags & CCM_ENCODE_AS_PAYLOAD)))
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_INFO, "encoding as payload");
        // encode a CCM-Payload
        message_content_type = OBJ_txt2obj(kCCMPayloadOID, 1);

        CCM_Payload_t payload;
        memset(&payload, 0, sizeof(CCM_Payload_t));

        return_value = CCMPayload2ASN1C(&(message->payload), &payload);
        if (return_value)
        {
            DLT_LOG_STRING(dlt_libfsm, DLT_LOG_VERBOSE, "converts to ccm_Payload structure");

            return_value = EncodePayload(&payload, &message_data);
            if (return_value)
            {
                DLT_LOG_STRING(dlt_libfsm, DLT_LOG_VERBOSE, "encode payload");
            }
            else
            {
                DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "encode payload fail");
            }
        }
        else
        {
            DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "converts to ccm_Payload structure fail");
        }

        asn_DEF_CCM_Payload.free_struct(&asn_DEF_CCM_Payload, &payload, 1);
    }
    else if (return_value)
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_INFO, "encoding as plain");

        // "plain" message

        if (message->payload.content.content->size())
        {
            message_content_type = OBJ_txt2obj(message->payload.content.oid.oid, 1);
            message_data.data = message->payload.content.content->data();
            message_data.num_bytes = message->payload.content.content->size();
        }
        else
        {
            // a palin empty message makes no sence as there is nothing to
            // sign nor encrypt

            DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "can not create plain empty CCM");

            return_value = false;
        }
    }

    if (return_value && (recipients.empty() || (flags & CCM_DO_NOT_ENCRYPT)))
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_INFO, "not encrypting");
        // no encryption

        dataBio = BIO_new_mem_buf(message_data.data,
                                  message_data.num_bytes);

        if (!dataBio)
        {
            return_value = false;
        }

        // sign our data and write the header
        if (return_value)
        {
            return_value = SignData(&outBio, dataBio, &header_data, message_content_type, recipients);
            if (return_value)
            {
                DLT_LOG_STRING(dlt_libfsm, DLT_LOG_VERBOSE, "sign data and write the header under no encryption");
            }
            else
            {
                DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "signedData not create under no encryption");
            }
        }
    }
    else if (return_value)
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_INFO, "encrypting");
        // encrypted message

        if (return_value)
        {
            ASN1_OBJECT* enveloped_content_type = OBJ_nid2obj(NID_pkcs7_enveloped);

            // this will set up a envelopedData and:
            // - add header in unprotectedAttributes
            // - add payload content and encrypt it
            // - set the oid
            return_value = EncryptData(&dataBio,
                                       &message_data,
                                       &header_data,
                                       message_content_type,
                                       recipients);

            if (return_value)
            {
                return_value = SignData(&outBio, dataBio, NULL, enveloped_content_type, recipients);
                if (return_value)
                {
                    DLT_LOG_STRING(dlt_libfsm, DLT_LOG_VERBOSE, "sign data and write the header with encrypted message");
                }
                else
                {
                    DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "signedData not create with encrypted message");
                }
            }
            else
            {
                DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "fail to encrypt data");
            }

            ASN1_OBJECT_free(enveloped_content_type);
        }
    }


    //TODO: we may have to compress with gzip here but spec is
    //      currently unclear. Ignored until we are
    //      aware of a client which does compress.

    // copy signed data into "out" data
    if (return_value)
    {
        char* signedContent;
        long num_bytes = BIO_get_mem_data(outBio, &signedContent);
        if (num_bytes <= 0)
        {
            DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "outBio is empty");
            return_value = false;
        }
        else
        {
            data->data = std::malloc(num_bytes);

            if (data->data)
            {
                std::memcpy(data->data, signedContent, num_bytes);
                data->num_bytes = num_bytes;
            }
            else
            {
                data->num_bytes = 0;
                return_value = false;
                DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "outBio is empty");
            }
        }
    }

    // and we are done

    if (header_data.data)
    {
        std::free(header_data.data);
    }

    // NULL safe
    ASN1_OBJECT_free(message_content_type);
    BIO_free(dataBio);
    BIO_free(outBio);

    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "ccm_Encode %s.",
                    return_value ? "succeded" : "failed");

    return return_value;
}

bool FSOid2ASN1C (const fs_Oid* fs_oid, OBJECT_IDENTIFIER_t* asn1c_oid, int offset)
{
    //make sure offset has a valid value
    if (offset >= FS_OID_MAX_LEN || offset < 0)
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "FSOid2ASN1C, offset outside bounds");
        return false;
    }

    long nodes[kFsOidMaxNodes];
    unsigned int nodes_type_size = sizeof(nodes[0]);

    if ((fs_oid == NULL) || (asn1c_oid == NULL))
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "Failed to encode (parse) oid, incorrect input");
        return false;
    }

    int num_nodes = OBJECT_IDENTIFIER_parse_arcs(&(fs_oid->oid[offset]),
                                                 -1,
                                                 nodes,
                                                 kFsOidMaxNodes,
                                                 NULL);
    if (num_nodes <= 0)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "Failed to encode (parse) oid, result: %d",
                        num_nodes);
        return false;
    }

    int result = -1; //defaulting to error

    //check if full or relative oid is requested
    //for relative use methods designed for relative OIDs, methods for full OIDs do not work on
    //relative OIDs even though the relative and the absolute oid types share common base type
    if (offset == 0)
    {
        //no offset, full oid
        result = OBJECT_IDENTIFIER_set_arcs(asn1c_oid,
                                            nodes,
                                            nodes_type_size,
                                            num_nodes);
    }
    else
    {
        //relative OID
        result = RELATIVE_OID_set_arcs(asn1c_oid,
                                       nodes,
                                       nodes_type_size,
                                       num_nodes);
    }

    if (result < 0)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "Failed to encode oid, result: %d", result);
        return false;
    }
    return true;
}

bool ASN1C2FSOid (OBJECT_IDENTIFIER_t* asn1c_oid, fs_Oid* fs_oid, int offset)
{
    //make sure offset has a valid value
    if (offset >= FS_OID_MAX_LEN || offset < 0)
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "ASN1C2FSOid, offset outside bounds");
        return false;
    }

    unsigned long nodes[kFsOidMaxNodes];
    unsigned int nodes_type_size = sizeof(nodes[0]);

    if ((fs_oid == NULL) || (asn1c_oid == NULL))
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "Failed to decode (parse) oid, incorrect input");
        return false;
    }

    int num_nodes = -1;

    //check if full or relative oid is requested
    //for relative use methods designed for relative OIDs, methods for full OIDs do not work on
    //relative OIDs even though the relative and the absolute oid types share common base type
    if (offset == 0)
    {
        //no offset, full OID
        num_nodes = OBJECT_IDENTIFIER_get_arcs(asn1c_oid,
                                               (void*) nodes,
                                               nodes_type_size,
                                               kFsOidMaxNodes);
    }
    else
    {
        ///offset - relative OID
        num_nodes = RELATIVE_OID_get_arcs(asn1c_oid,
                                          (void*) nodes,
                                          nodes_type_size,
                                          kFsOidMaxNodes);

    }

    if (num_nodes <= 0)
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "Failed to parse OID");
        return false;
    }

    //parse asn1c OBJECT_IDENTIFIER_t into fs_Oid
    char* position = &(fs_oid->oid[offset]);
    int num_written = 0;

    for (int i = 0; i < num_nodes; i++)
    {
        char buffer[20]={0}; //should fit SLONG_MAX on 64bit which is the max according to asn1c
        sprintf(buffer, "%lu", nodes[i]);

        int num_digits = strlen(buffer);

        // copy the node if it fits
        if (num_written + num_digits < FS_OID_MAX_LEN)
        {
            strncpy(position, buffer, num_digits);
            position += num_digits;
            num_written += num_digits;
        }
        else
        {
            DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "Failed to fit OID");
            return false;
        }

        // add '.' if this is not the last node
        if (i+1 < num_nodes)
        {
            *position = '.';
            position += 1;
            num_written += 1;
        }
    }

    if (FS_OID_MAX_LEN >= (num_written + offset))
    {
        fs_oid->oid[offset + num_written] = '\0';
    }
    else
    {
        fs_oid->oid[FS_OID_MAX_LEN] = '\0';
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "fs_oid->oid has been truncated to FS_OID_MAX_LEN chars");
    }

    return true;
}


/** \brief Copies a ASN1C octet string of a certain expected length
 *         into a "ccm" char buffer.
 *  \param[in]     asn1c_octet_string - octet string to copy
 *  \param[in/out] ccm_buffer         - buffer to copy to (buffer must be null terminated by the caller)
 *  \param[in]     buffer_length      - The expected length
 *  \return false if unsuccessful
 */
bool ASN1C2OctetString (OCTET_STRING_t* asn1c_octet_string,
                        char* ccm_buffer,
                        size_t buffer_length)
{

    if ((asn1c_octet_string == NULL) || (ccm_buffer == NULL))
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "Failed to copy octet string, incorrect input");
        return false;
    }

    if ((size_t)asn1c_octet_string->size == buffer_length && asn1c_octet_string->buf)
    {
        std::memcpy(ccm_buffer, asn1c_octet_string->buf, buffer_length);
        return true;
    }
    return false;
}

/**
 * \brief Convert asn1c UTF8 string to std::string type.
 * \param[in] asn1_str the asn1c string for input
 * \param[in/out] to_str the std::string for output
 */
void ASN1String2StdString(const OCTET_STRING_t &asn1_str, std::string &to_str)
{

    char* data = (char*)asn1_str.buf;
    int length = asn1_str.size;
    to_str.assign(data, length);

}

std::string FsmGenerateId(const int length)
{
    unsigned char *generated_id;

    generated_id = new unsigned char [length];

    if (RAND_bytes(generated_id, length) != 1)
    {
        std::default_random_engine generator;

        //skipping 0 not to have end of line character
        std::uniform_int_distribution<char> distribution(1, 255);

        generator.seed(std::time(0));

        for (int i = 0; i < length; i++)
        {
            generated_id[i] = static_cast<char>(distribution(generator));
        }
    }

    std::string result;

    result.assign(generated_id, generated_id + length);

    delete[] generated_id;

    return result;
}

/******************************************************************************
 * STATIC FUNCTIONS
 ******************************************************************************/

static int write_encoded_bytes(const void *buffer, size_t size, void *clientdata)
{
    if (!clientdata)
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "bad client data");
        return -1;
    }

    fs_Data* encode_buffer = *((fs_Data**)clientdata);

    if (!encode_buffer)
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "bad buffer");
        return -1;
    }

    if (size == 0)
    {
        // do nothing
        // modify klocwork warning,yangjun add 20181107
        if(encode_buffer->data)
        {
            free(encode_buffer->data);
        }        
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_WARN, "asked to consume 0 encoded bytes");
        return 1;
    }

    //TODO: consider a more intelligent realloc strategy
    void* result = realloc(encode_buffer->data, (encode_buffer->num_bytes) + size);

    if (!result)
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "failed to reallocate buffer");
        return -1;
    }

    encode_buffer->data = result;

    char* start = ((char*) encode_buffer->data) + encode_buffer->num_bytes;
    std::memcpy(start, buffer, size);

    encode_buffer->num_bytes = encode_buffer->num_bytes + size;

    return 1;
}

/** \brief Compare two ASN1C octet strings
 *  \param[in]     str - A OCTET_STRING to compare
 *  \param[in]     other_str - A OCTET_STRING to compare
 *  \return If matched, return TRUE otherwise FALSE.
 */
static bool CompareOctetSrings(const OCTET_STRING_t* str, const OCTET_STRING_t* other_str)
{
    bool return_value = true;

    DLT_LOG_STRING(dlt_libfsm, DLT_LOG_VERBOSE, "Comparing two ASN1C octet strings");

    if (!str || !other_str)
    {
        return_value = false;
    }

    if (return_value)
    {
        if ((str->size != other_str->size) || (str->size <= 0))
        {
            return_value = false;
        }
    }

    if (return_value)
    {
        if (std::memcmp(str->buf, other_str->buf, str->size) != 0)
        {
            return_value = false;
        }
    }

    return return_value;
}

/** \brief Converts a fs_Encoding into a Encoding_t
 *  \param[in]     ccm_encoding   - The encoding to convert
 *  \param[in/out] asn1c_encoding - The encoding to convert to
 *  \return false if unsuccessful
 */
static bool CCMEncoding2ASN1C(fs_Encoding ccm_encoding, Encoding_t* asn1c_encoding)
{
    switch (ccm_encoding)
    {
    case FS_ENCODING_BER :
        *asn1c_encoding = Encoding_ber;
        break;
    case FS_ENCODING_CER :
        *asn1c_encoding = Encoding_cer;
        break;
    case FS_ENCODING_DER :
        *asn1c_encoding = Encoding_der;
        break;
    case FS_ENCODING_OER :
        *asn1c_encoding = Encoding_oer;
        break;
    case FS_ENCODING_PER :
        *asn1c_encoding = Encoding_per;
        break;
    case FS_ENCODING_UPER :
        *asn1c_encoding = Encoding_uper;
        break;
    case FS_ENCODING_XER :
        *asn1c_encoding = Encoding_xer;
        break;
    case FS_ENCODING_XML :
        *asn1c_encoding = Encoding_xml;
        break;
    default :
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_WARN, "got unknown encoding.");
        return false;
    }

    return true;
}

/** \brief Converts a Encoding_t into a fs_Encoding
 *  \param[in]     asn1c_encoding   - The encoding to convert
 *  \param[in/out] ccm_encoding - The encoding to convert to
 *  \return false if unsuccessful
 */
static void ASN1C2CCMEncoding(Encoding_t* asn1c_encoding, fs_Encoding* ccm_encoding)
{

    if (!asn1c_encoding)
    {
        *ccm_encoding = FS_ENCODING_UNDEFINED;
        return;
    }

    switch (*asn1c_encoding)
    {
    case Encoding_ber :
        *ccm_encoding = FS_ENCODING_BER;
        break;
    case Encoding_cer :
        *ccm_encoding = FS_ENCODING_CER;
        break;
    case Encoding_der :
        *ccm_encoding = FS_ENCODING_DER;
        break;
    case Encoding_oer :
        *ccm_encoding = FS_ENCODING_OER;
        break;
    case Encoding_per :
        *ccm_encoding = FS_ENCODING_PER;
        break;
    case Encoding_uper :
        *ccm_encoding = FS_ENCODING_UPER;
        break;
    case Encoding_xer :
        *ccm_encoding = FS_ENCODING_XER;
        break;
    case Encoding_xml :
        *ccm_encoding = FS_ENCODING_XML;
        break;
    default :
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_WARN, "got unknown encoding.");
        *ccm_encoding = FS_ENCODING_UNDEFINED;
    }
}

/** \brief Converts a fs_VersionInfo to a VersionInfo_t
 *  \param[in]     ccm_version   - The version to convert
 *  \param[in/out] asn1c_version - The version to fill in
 *  \return None
 */
static void CCMVersionInfo2ASN1C(const fs_VersionInfo* ccm_version, VersionInfo_t* asn1c_version)
{
    if(ccm_version->signal_flow_version == -1)
    {
        asn1c_version->signalFlowVersion = NULL;
    }
    else
    {
        asn1c_version->signalFlowVersion = static_cast<INT32_t*>(calloc(1, sizeof(long)));

        if (asn1c_version->signalFlowVersion)
        {
            *(asn1c_version->signalFlowVersion) = ccm_version->signal_flow_version;
        }
    }
    asn1c_version->schemaVersion = ccm_version->schema_version;
    if(ccm_version->preliminary_version == -1)
    {
        asn1c_version->preliminaryVersion = NULL;
    }
    else
    {
        asn1c_version->preliminaryVersion = static_cast<INT32_t*>(calloc(1, sizeof(long)));

        if (asn1c_version->preliminaryVersion)
        {
            *(asn1c_version->preliminaryVersion) = ccm_version->preliminary_version;
        }
    }
}

/** \brief Converts a VersionInfo_t to a fs_VersionInfo
 *  \param[in]     asn1c_version   - The version to convert
 *  \param[in/out] ccm_version - The version to fill in
 *  \return None
 */
static void ASN1C2CCMVersionInfo(VersionInfo_t* asn1c_version, fs_VersionInfo* ccm_version)
{
    if (asn1c_version->signalFlowVersion)
    {
        ccm_version->signal_flow_version = *(asn1c_version->signalFlowVersion);
    }
    else
    {
        ccm_version->signal_flow_version = -1;
    }
    ccm_version->schema_version = asn1c_version->schemaVersion;
    if (asn1c_version->preliminaryVersion)
    {
        ccm_version->preliminary_version = *(asn1c_version->preliminaryVersion);
    }
    else
    {
        ccm_version->preliminary_version = -1;
    }

}

static bool CCMStoreAndForwardOptions2ASN1C (ccm_storeAndForwardOptions ccm_option,
                                            StoreAndForwardOptions_t* asn1c_option)
{
    switch (ccm_option)
    {
    case CCM_STORE_AND_FORWARD_UNDEFINED :
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_WARN, "got unknown storeAndForwardOption.");
        return false;
    case CCM_STORE_AND_FORWARD_ALWAYS :
        *asn1c_option = StoreAndForwardOptions_storeAlways;
        break;
    case CCM_STORE_AND_FORWARD_LATEST :
        *asn1c_option = StoreAndForwardOptions_storeLatest;
        break;
    case CCM_STORE_AND_FORWARD_NEVER :
        *asn1c_option = StoreAndForwardOptions_dontStore;
        break;
    default:
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_WARN, "got unknown storeAndForwardOption.");
        return false;
    }
    return true;
}

static void ASN1C2CCMStoreAndForwardOptions (StoreAndForwardOptions_t* asn1c_option,
                                             ccm_storeAndForwardOptions* ccm_option)
{
    if (!asn1c_option)
    {
        *ccm_option = CCM_STORE_AND_FORWARD_UNDEFINED;
        return;
    }

    switch (*asn1c_option)
    {
    case StoreAndForwardOptions_storeAlways :
        *ccm_option = CCM_STORE_AND_FORWARD_ALWAYS;
        break;
    case StoreAndForwardOptions_storeLatest :
        *ccm_option = CCM_STORE_AND_FORWARD_LATEST;
        break;
    case StoreAndForwardOptions_dontStore :
        *ccm_option = CCM_STORE_AND_FORWARD_NEVER;
        break;
    default:
        *ccm_option = CCM_STORE_AND_FORWARD_UNDEFINED;
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_WARN, "got unknown storeAndForwardOption.");
    }
}

static bool CCMSmsDeliveryOptions2ASN1C (ccm_smsDeliveryOptions ccm_option,
                                         SmsDeliveryOptions_t* asn1c_option)
{
    switch (ccm_option)
    {
        case CCM_SMS_DELIVERY_UNDEFINED :
            DLT_LOG_STRING(dlt_libfsm, DLT_LOG_WARN, "got unknown smsDeliveryOptions.");
            return false;
        case CCM_SMS_DELIVERY_SEND_SMS_AS_BACKUP :
            *asn1c_option = SmsDeliveryOptions_sendSmsAsBackup;
            break;
        case CCM_SMS_DELIVERY_SEND_SMS_AS_BACKUP_AND_RETAIN :
            *asn1c_option = SmsDeliveryOptions_sendSmsAsBackupAndRetain;
            break;
        default:
            DLT_LOG_STRING(dlt_libfsm, DLT_LOG_WARN, "got unknown smsDeliveryOptions.");
            return false;
    }
    return true;
}

static void ASN1C2CCMSmsDeliveryOptions (SmsDeliveryOptions_t* asn1c_option,
                                         ccm_smsDeliveryOptions* ccm_option)
{
    if (!asn1c_option)
    {
        *ccm_option = CCM_SMS_DELIVERY_UNDEFINED;
        return;
    }

    switch (*asn1c_option)
    {
        case SmsDeliveryOptions_sendSmsAsBackup :
            *ccm_option = CCM_SMS_DELIVERY_SEND_SMS_AS_BACKUP;
            break;
        case SmsDeliveryOptions_sendSmsAsBackupAndRetain :
            *ccm_option = CCM_SMS_DELIVERY_SEND_SMS_AS_BACKUP_AND_RETAIN;
            break;
        default:
            *ccm_option = CCM_SMS_DELIVERY_UNDEFINED;
            DLT_LOG_STRING(dlt_libfsm, DLT_LOG_WARN, "got unknown smsDeliveryOptions.");
    }
}

static void CCMDeliveryOptions2ASN1C (const ccm_DeliveryOptions* ccm_options,
                                      DeliveryOptions_t* asn1c_options)
{
    StoreAndForwardOptions_t asn1c_option;
    if (CCMStoreAndForwardOptions2ASN1C (ccm_options->store_and_forward, &asn1c_option))
    {
        asn1c_options->storeAndForward =
            static_cast<StoreAndForwardOptions_t*>(calloc(1, sizeof(StoreAndForwardOptions_t)));

        if (asn1c_options->storeAndForward)
        {
            *(asn1c_options->storeAndForward) = asn1c_option;
        }
    }
    else
    {
        asn1c_options->storeAndForward = NULL;
    }

    if (ccm_options->qos_level == -1)
    {
        asn1c_options->qosLevel = NULL;
    }
    else
    {
        asn1c_options->qosLevel = static_cast<INT32_t*>(calloc(1, sizeof(long)));

        if (asn1c_options->qosLevel)
        {
            *(asn1c_options->qosLevel) = ccm_options->qos_level;
        }
    }

    if (ccm_options->send_push_notification == -1)
    {
        asn1c_options->sendPushNotification = NULL;
    }
    else
    {
        asn1c_options->sendPushNotification = static_cast<BOOLEAN_t*>(calloc(1, sizeof(BOOLEAN_t)));

        if (asn1c_options->sendPushNotification)
        {
            *(asn1c_options->sendPushNotification) = ccm_options->send_push_notification;
        }
    }

    SmsDeliveryOptions_t asn1c_sms_delivery_option;
    if (CCMSmsDeliveryOptions2ASN1C (ccm_options->sms_delivery_options, &asn1c_sms_delivery_option))
    {
        asn1c_options->smsDeliveryOptions = static_cast<SmsDeliveryOptions_t*>(calloc(1, sizeof(SmsDeliveryOptions_t)));

        if (asn1c_options->smsDeliveryOptions)
        {
            *(asn1c_options->smsDeliveryOptions) = asn1c_sms_delivery_option;
        }
    }
    else
    {
        asn1c_options->smsDeliveryOptions = NULL;
    }
}

static void ASN1C2CCMDeliveryOptions (DeliveryOptions_t* asn1c_options,
                                      ccm_DeliveryOptions* ccm_options)
{
    ASN1C2CCMStoreAndForwardOptions(asn1c_options->storeAndForward, &(ccm_options->store_and_forward));

    if (asn1c_options->qosLevel)
    {
        ccm_options->qos_level = *(asn1c_options->qosLevel);
    }
    else
    {
        ccm_options->qos_level = -1;
    }

    if (asn1c_options->sendPushNotification)
    {
        ccm_options->send_push_notification = *(asn1c_options->sendPushNotification);
    }
    else
    {
        ccm_options->send_push_notification = -1;
    }

    ASN1C2CCMSmsDeliveryOptions(asn1c_options->smsDeliveryOptions, &(ccm_options->sms_delivery_options));

}

static void CCMReceiptOptions2ASN1C(const ccm_ReceiptOptions* ccm_options,
                                     ReceiptOptions_t* asn1c_options)
{
    if (ccm_options->msg_delivered_ack == -1)
    {
        asn1c_options->msgDeliveredAck = NULL;
    }
    else
    {
        asn1c_options->msgDeliveredAck = static_cast<BOOLEAN_t*>(calloc(1, sizeof(BOOLEAN_t)));

        if (asn1c_options->msgDeliveredAck)
        {
            *(asn1c_options->msgDeliveredAck) = ccm_options->msg_delivered_ack;
        }
    }

    if (ccm_options->msg_delivered_sms_ack == -1)
    {
        asn1c_options->msgDeliveredSmsAck = NULL;
    }
    else
    {
        asn1c_options->msgDeliveredSmsAck = static_cast<BOOLEAN_t*>(calloc(1, sizeof(BOOLEAN_t)));

        if (asn1c_options->msgDeliveredSmsAck)
        {
            *(asn1c_options->msgDeliveredSmsAck) = ccm_options->msg_delivered_sms_ack;
        }
    }

    if (ccm_options->msg_delivered_sms_noack == -1)
    {
        asn1c_options->msgDeliveredSmsNoack = NULL;
    }
    else
    {
        asn1c_options->msgDeliveredSmsNoack = static_cast<BOOLEAN_t*>(calloc(1, sizeof(BOOLEAN_t)));

        if (asn1c_options->msgDeliveredSmsNoack)
        {
            *(asn1c_options->msgDeliveredSmsNoack) = ccm_options->msg_delivered_sms_noack;
        }
    }

    if (ccm_options->recipient_unconnected == -1)
    {
        asn1c_options->recipientUnconnected = NULL;
    }
    else
    {
        asn1c_options->recipientUnconnected = static_cast<BOOLEAN_t*>(calloc(1, sizeof(BOOLEAN_t)));

        if (asn1c_options->recipientUnconnected)
        {
            *(asn1c_options->recipientUnconnected) = ccm_options->recipient_unconnected;
        }
    }

    if (ccm_options->msg_undelivered_stored == -1)
    {
        asn1c_options->msgUndeliveredStored = NULL;
    }
    else
    {
        asn1c_options->msgUndeliveredStored = static_cast<BOOLEAN_t*>(calloc(1, sizeof(BOOLEAN_t)));

        if (asn1c_options->msgUndeliveredStored)
        {
            *(asn1c_options->msgUndeliveredStored) = ccm_options->msg_undelivered_stored;
        }
    }

    if (ccm_options->msg_undelivered_ttl_expired == -1)
    {
        asn1c_options->msgUndeliveredTtlExpired = NULL;
    }
    else
    {
        asn1c_options->msgUndeliveredTtlExpired = static_cast<BOOLEAN_t*>(calloc(1, sizeof(BOOLEAN_t)));

        if (asn1c_options->msgUndeliveredTtlExpired)
        {
            *(asn1c_options->msgUndeliveredTtlExpired) = ccm_options->msg_undelivered_ttl_expired;
        }
    }

    if (ccm_options->end_to_end_ack == -1)
    {
        asn1c_options->endToEndAck = NULL;
    }
    else
    {
        asn1c_options->endToEndAck = static_cast<BOOLEAN_t*>(calloc(1, sizeof(BOOLEAN_t)));

        if (asn1c_options->endToEndAck)
        {
            *(asn1c_options->endToEndAck) = ccm_options->end_to_end_ack;
        }
    }

    if (ccm_options->tracert_ack == -1)
    {
        asn1c_options->tracertAck = NULL;
    }
    else
    {
        asn1c_options->tracertAck = static_cast<BOOLEAN_t*>(calloc(1, sizeof(BOOLEAN_t)));

        if (asn1c_options->tracertAck)
        {
            *(asn1c_options->tracertAck) = ccm_options->tracert_ack;
        }
    }

    if (ccm_options->push_notification_sent == -1)
    {
        asn1c_options->pushNotificationSent = NULL;
    }
    else
    {
        asn1c_options->pushNotificationSent = static_cast<BOOLEAN_t*>(calloc(1, sizeof(BOOLEAN_t)));

        if (asn1c_options->pushNotificationSent)
        {
            *(asn1c_options->pushNotificationSent) = ccm_options->push_notification_sent;
        }
    }
}

static void ASN1C2CCMReceiptOptions(ReceiptOptions_t* asn1c_options,
                                     ccm_ReceiptOptions* ccm_options)
{
    ccm_options->msg_delivered_ack = asn1c_options->msgDeliveredAck == NULL ?
        -1 : *(asn1c_options->msgDeliveredAck);

    ccm_options->msg_delivered_sms_ack = asn1c_options->msgDeliveredSmsAck == NULL ?
        -1 : *(asn1c_options->msgDeliveredSmsAck);

    ccm_options->msg_delivered_sms_noack = asn1c_options->msgDeliveredSmsNoack == NULL ?
        -1 : *(asn1c_options->msgDeliveredSmsNoack);

    ccm_options->recipient_unconnected = asn1c_options->recipientUnconnected == NULL ?
        -1 : *(asn1c_options->recipientUnconnected);

    ccm_options->msg_undelivered_stored = asn1c_options->msgUndeliveredStored == NULL ?
        -1 : *(asn1c_options->msgUndeliveredStored);

    ccm_options->msg_undelivered_ttl_expired = asn1c_options->msgUndeliveredTtlExpired == NULL ?
        -1 : *(asn1c_options->msgUndeliveredTtlExpired);

    ccm_options->end_to_end_ack = asn1c_options->endToEndAck == NULL ?
        -1 : *(asn1c_options->endToEndAck);

    ccm_options->tracert_ack = asn1c_options->tracertAck == NULL ?
        -1 : *(asn1c_options->tracertAck);

    ccm_options->push_notification_sent = asn1c_options->pushNotificationSent == NULL ?
        -1 : *(asn1c_options->pushNotificationSent);
}

/** \brief Converts a asn1c address structure into a ccm_Address structure.
 *
 *  \param[in]     asn1c_address - structure to convert
 *                                 NULL is treated as a UNDIFINED address
 *  \param[in/out] ccm_address   - structure to convert into
 *  \return false if unsuccessful (malformed address)
 */
static bool ASN1C2CCMAddress (AddressCCM_t* asn1c_address, ccm_Address* ccm_address)
{

    int uri_length = 0;

    ccm_address->type = ADDRESS_TYPE_UNDEFINED;

    if (!asn1c_address)
    {
        return true;
    }

    switch (asn1c_address->present)
    {
    case AddressCCM_PR_NOTHING :
        ccm_address->type = ADDRESS_TYPE_UNDEFINED;
        break;
    case AddressCCM_PR_vehicle :
        if (!ASN1C2OctetString(&(asn1c_address->choice.vehicle),
                               ccm_address->address.vehicle,
                               FS_VIN_LEN))
        {
            DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "invalid vin");
            return false;
        }
        ccm_address->type = ADDRESS_TYPE_VEHICLE;
        break;
    case AddressCCM_PR_userId :
        if (!ASN1C2OctetString(&(asn1c_address->choice.userId),
                               ccm_address->address.user_id,
                               FS_UUID_LEN))
        {
            DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "invalid user id");
            return false;
        }
        ccm_address->type = ADDRESS_TYPE_USER_ID;
        break;
    case AddressCCM_PR_deviceId :
        if (!ASN1C2OctetString(&(asn1c_address->choice.deviceId),
                               ccm_address->address.device_id,
                               FS_UUID_LEN))
        {
            DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "invalid device id");
            return false;
        }
        ccm_address->type = ADDRESS_TYPE_DEVICE_ID;
        break;
    case AddressCCM_PR_subjectKeyIdentifier :
        if (!ASN1C2OctetString(&(asn1c_address->choice.subjectKeyIdentifier),
                               (char*)ccm_address->address.subject_key_identifier,
                               CCM_SUBJECT_KEY_IDENTIFIER_LEN))
        {
            DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "invalid subject key identifier");
            return false;
        }
        ccm_address->type = ADDRESS_TYPE_SUBJECT_KEY_IDENTIFIER;
        break;
    case AddressCCM_PR_uri :
        uri_length = asn1c_address->choice.uri.size;
        if (uri_length > 0 && asn1c_address->choice.uri.buf)
        {
            ccm_address->address.uri = new char[uri_length + 1];
            std::memcpy(ccm_address->address.uri, asn1c_address->choice.uri.buf, uri_length);
            ccm_address->address.uri[uri_length] = '\0';
        }
        else
        {
            DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "invalid uri");
            ccm_address->address.uri = NULL;
            return false;
        }
        ccm_address->type = ADDRESS_TYPE_URI;
        break;
    case AddressCCM_PR_instanceId :
        if (!ASN1C2OctetString(&(asn1c_address->choice.instanceId),
                               ccm_address->address.instance_id,
                               FS_UUID_LEN))
        {
            DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "invalid instance id");
            return false;
        }
        ccm_address->type = ADDRESS_TYPE_INSTANCE_ID;
        break;
    case AddressCCM_PR_objectId :
        if (!ASN1C2FSOid(&(asn1c_address->choice.objectId), &(ccm_address->address.object_id)))
        {
            DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "Failed to parse oid.");
            return false;
        }
        ccm_address->type = ADDRESS_TYPE_OBJECT_ID;
        break;
    default :
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_WARN, "got unknown Address type.");
    }

    return true;
}

/** \brief Converts a ccm_Header structure to a CCM_Header_t structure
 *  \param[in]     ccmHeader   - The header to convert
 *  \param[in/out] asn1cHeader - The header to fill in
 *  \return None
 */
static bool CCMHeader2ASN1C (const ccm_Header* ccmHeader, CCM_Header_t* asn1cHeader)
{
    // version
    fs_VersionInfo ccm_version;

    // header.version is the version of the ccm implementation
    // we populate it here as only we know the right values
    // ignoring whatever the client has set in message.

    ccm_version.signal_flow_version = kImplementedSignalFlowVersion;
    ccm_version.schema_version = kImplementedSchemaVersion;
    ccm_version.preliminary_version = kImplementedPreliminaryVersion;

    CCMVersionInfo2ASN1C(&ccm_version, &(asn1cHeader->version));

    // timestamp. optional, skip if tv_nsec set to -1
    if (ccmHeader->timestamp.tv_nsec != -1)
    {
        asn1cHeader->timestamp = static_cast<struct Timestamp*>(calloc(1, sizeof(struct Timestamp)));
        if (asn1cHeader->timestamp == nullptr)
        {
            return false;
        }
        asn1cHeader->timestamp->tvSec = static_cast<long> (ccmHeader->timestamp.tv_sec);
        //only encode tv_nsec if  > 0
        if (ccmHeader->timestamp.tv_nsec > 0)
        {
            asn1cHeader->timestamp->tvNsec = static_cast<NanoSeconds_t*>(calloc(1, sizeof(NanoSeconds_t)));

            if (asn1cHeader->timestamp->tvNsec)
            {
                *(asn1cHeader->timestamp->tvNsec) = ccmHeader->timestamp.tv_nsec;
            }
            else
            {
                return false;
            }
        }
        else
        {
            asn1cHeader->timestamp->tvNsec = NULL;
        }
    }


    // time to live
    if(ccmHeader->time_to_live)
    {
        asn1cHeader->timeToLive = static_cast<TimeToLive_t*>(calloc(1, sizeof(TimeToLive_t)));

        if (asn1cHeader->timeToLive)
        {
            if(ccmHeader->time_to_live < CCM_MAX_NUM_TIME_TO_LIVE_MILLISECONDS)
            {
                asn1cHeader->timeToLive->present = TimeToLive_PR_milliSeconds;
                asn1cHeader->timeToLive->choice.milliSeconds = ccmHeader->time_to_live;
            }
            else
            {
                asn1cHeader->timeToLive->present = TimeToLive_PR_seconds;
                asn1cHeader->timeToLive->choice.milliSeconds = ccmHeader->time_to_live / 1000;
            }
        }
        else
        {
            return false;
        }
    }

    // session id, (16 bytes of 0 is handled as undefined)
    for (int i = 0; i < CCM_SESSION_ID_LEN; i++)
    {

        if(ccmHeader->session_id[i] != 0)
        {
            asn1cHeader->sessionId = static_cast<SessionId_t*>(calloc(1, sizeof(SessionId_t)));
            OCTET_STRING_fromBuf(asn1cHeader->sessionId,
                                 ccmHeader->session_id,
                                 CCM_SESSION_ID_LEN);
            break;
        }
    }

    // transaction id, only 0 means undefined
    for (int i = 0; i < CCM_TRANSACTION_ID_LEN; i++)
    {
        if(ccmHeader->transaction_id[i] != 0)
        {
            asn1cHeader->transactionId = static_cast<TransactionId_t*>(calloc(1, sizeof(TransactionId_t)));
            OCTET_STRING_fromBuf(asn1cHeader->transactionId,
                                 ccmHeader->transaction_id,
                                 CCM_TRANSACTION_ID_LEN);
            break;
        }
    }

    // message id, CCM_MAX_UINT_16 means undefined
    if (ccmHeader->message_id != CCM_MAX_UINT_16)
    {
        asn1cHeader->messageId = static_cast<UINT16_t*>(calloc(1, sizeof(UINT16_t)));
        if (asn1cHeader->messageId  != nullptr)
        {
            *asn1cHeader->messageId = (UINT16_t) ccmHeader->message_id;
        }
        else
        {
            return false;
        }
    }


    // resend counter
    if(ccmHeader->resend_counter)
    {
        asn1cHeader->resendCounter = static_cast<UINT8_t*>(calloc(1, sizeof(UINT8_t)));

        if (asn1cHeader->resendCounter)
        {
            *(asn1cHeader->resendCounter) = ccmHeader->resend_counter;
        }
        else
        {
            return false;
        }
    }
    else
    {
        asn1cHeader->resendCounter = nullptr;
    }


    // sender and recipients are populated from user and vehicle credentials
    asn1cHeader->sender = NULL;
    asn1cHeader->recipients = NULL;

    // delivery options
    if (ccmHeader->delivery_options.store_and_forward == CCM_STORE_AND_FORWARD_UNDEFINED &&
        ccmHeader->delivery_options.qos_level == -1 &&
        ccmHeader->delivery_options.send_push_notification == -1)
    {
        asn1cHeader->deliveryOptions = NULL;
    }
    else
    {
        asn1cHeader->deliveryOptions = static_cast<DeliveryOptions_t*>(calloc(1, sizeof(DeliveryOptions_t)));

        if (asn1cHeader->deliveryOptions)
        {
            CCMDeliveryOptions2ASN1C (&(ccmHeader->delivery_options), asn1cHeader->deliveryOptions);
        }
        else
        {
            return false;
        }
    }

    // receipt options
    if (ccmHeader->receipt_options.msg_delivered_ack == -1 &&
        ccmHeader->receipt_options.msg_delivered_sms_ack == -1 &&
        ccmHeader->receipt_options.msg_delivered_sms_noack == -1 &&
        ccmHeader->receipt_options.recipient_unconnected == -1 &&
        ccmHeader->receipt_options.msg_undelivered_stored == -1 &&
        ccmHeader->receipt_options.msg_undelivered_ttl_expired == -1 &&
        ccmHeader->receipt_options.end_to_end_ack == -1 &&
        ccmHeader->receipt_options.tracert_ack == -1 &&
        ccmHeader->receipt_options.push_notification_sent == -1)
    {
        asn1cHeader->receiptOptions = NULL;
    }
    else
    {
        asn1cHeader->receiptOptions = static_cast<ReceiptOptions_t*>(calloc(1, sizeof(ReceiptOptions_t)));

        if (asn1cHeader->receiptOptions)
        {
            CCMReceiptOptions2ASN1C(&(ccmHeader->receipt_options), asn1cHeader->receiptOptions);
        }
        else
        {
            return false;
        }
    }

    // content encoding
    Encoding_t asn1c_encoding;
    if (CCMEncoding2ASN1C(ccmHeader->content_encoding, &asn1c_encoding))
    {
        //did not fail -> encoding was set
        asn1cHeader->contentEncoding = static_cast<Encoding_t*>(calloc(1, sizeof(Encoding_t)));

        if (asn1cHeader->contentEncoding)
        {
            *(asn1cHeader->contentEncoding) = asn1c_encoding;
        }
        else
        {
            return false;
        }
    }
    else
        asn1cHeader->contentEncoding = NULL;

    // content version
    asn1cHeader->contentVersion = NULL;

    if (!ccm_IsValidVersionInfo(&(ccmHeader->content_version)))
    {
        asn1cHeader->contentVersion = NULL;
    }
    else
    {
        asn1cHeader->contentVersion = static_cast<VersionInfo_t*>(calloc(1, sizeof(VersionInfo_t)));

        if (asn1cHeader->contentVersion)
        {
            CCMVersionInfo2ASN1C(&(ccmHeader->content_version), asn1cHeader->contentVersion);
        }
        else
        {
            return false;
        }
    }

    return true;
}

/** \brief Converts a CCM_Header_t structure to a  ccm_Header structure
 *  \param[in]     ccm_header   - The header to convert
 *  \param[in/out] asn1c_header - The header to fill in
 *  \return false if unsuccessful
 */
static bool ASN1C2CCMHeader (CCM_Header_t* asn1c_header, ccm_Header* ccm_header)
{
    // version
    ASN1C2CCMVersionInfo(&(asn1c_header->version), &(ccm_header->version));

    // timestamp
    if (asn1c_header->timestamp)
    {
        ccm_header->timestamp.tv_sec = (time_t) asn1c_header->timestamp->tvSec;
        if (asn1c_header->timestamp->tvNsec)
        {
            ccm_header->timestamp.tv_nsec = *(asn1c_header->timestamp->tvNsec);
        }
        else
        {
            ccm_header->timestamp.tv_nsec = 0;
        }
    }
    else
    {
        ccm_header->timestamp.tv_nsec = -1;
    }

    // TimeToLive
    // There's a time to live and a time to die
    // When it's time to meet the maker
    // There's time to live but isn't it strange
    // That as soon as you're born you're dying.
    if(asn1c_header->timeToLive)
    {
        if(asn1c_header->timeToLive->present == TimeToLive_PR_milliSeconds)
        {
            ccm_header->time_to_live = asn1c_header->timeToLive->choice.milliSeconds;
        }
        else if(asn1c_header->timeToLive->present == TimeToLive_PR_seconds)
        {
            ccm_header->time_to_live = asn1c_header->timeToLive->choice.seconds * 1000;
        }
    }

    // session id
    if(asn1c_header->sessionId)
    {
        if (!ASN1C2OctetString(asn1c_header->sessionId,
            ccm_header->session_id,
            CCM_SESSION_ID_LEN))
        {
            DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "invalid session id");
            return false;
        }
    }
    else
    {
        memset(ccm_header->session_id, 0, CCM_SESSION_ID_LEN);
    }

    // transaction id
    if (asn1c_header->transactionId)
    {
        if (!ASN1C2OctetString(asn1c_header->transactionId,
                               ccm_header->transaction_id,
                               CCM_TRANSACTION_ID_LEN))
        {
            DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "invalid transaction id");
            return false;
        }
    }
    else
    {
        memset(ccm_header->transaction_id, 0, CCM_TRANSACTION_ID_LEN);
    }

    // message id
    if (asn1c_header->messageId)
    {
        ccm_header->message_id = (uint16_t) (*(asn1c_header->messageId));
    }
    else
    {
        //if not specified, use CCM_MAX_UINT_16
        ccm_header->message_id = CCM_MAX_UINT_16;
    }

    // resend counter
    if (asn1c_header->resendCounter)
    {
        ccm_header->resend_counter = *asn1c_header->resendCounter;
    }

    // sender
    if (!ASN1C2CCMAddress(asn1c_header->sender, &(ccm_header->sender)))
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "invalid sender");
        return false;
    }

    // recipients
    if (asn1c_header->recipients)
    {
        int num_recipients = asn1c_header->recipients->list.count;

        if (num_recipients > CCM_MAX_NUM_RECIPIENTS)
        {
            num_recipients = CCM_MAX_NUM_RECIPIENTS;
            DLT_LOG_STRING(dlt_libfsm, DLT_LOG_WARN, "to many recipients");
        }

        for (int i = 0; i < num_recipients; i++)
        {
            if (!ASN1C2CCMAddress(asn1c_header->recipients->list.array[i],
                                  &(ccm_header->recipients[i])))
            {
                DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "invalid recipient");
                //TODO: quick fix to be able to parse message from mclient
                //      if failed to parse treat as not there and continue
                //      maybe this would actaully be good for all optional
                //      fields to be more robust?

                ccm_header->recipients[0].type = ADDRESS_TYPE_UNDEFINED;
                break;

                //return false;
            }
        }
    }
    else
    {
        ccm_header->recipients[0].type = ADDRESS_TYPE_UNDEFINED;
    }

    // delivery options
    if (asn1c_header->deliveryOptions)
    {
        ASN1C2CCMDeliveryOptions(asn1c_header->deliveryOptions,
                                 &(ccm_header->delivery_options));
    }
    else
    {
        ccm_header->delivery_options.store_and_forward = CCM_STORE_AND_FORWARD_UNDEFINED;
        ccm_header->delivery_options.qos_level = -1;
        ccm_header->delivery_options.send_push_notification = -1;
    }

    // receipt options
    if (asn1c_header->receiptOptions)
    {
        ASN1C2CCMReceiptOptions(asn1c_header->receiptOptions,
                                 &(ccm_header->receipt_options));
    }
    else
    {
        ccm_header->receipt_options.msg_delivered_ack = -1;
        ccm_header->receipt_options.msg_delivered_sms_ack = -1;
        ccm_header->receipt_options.msg_delivered_sms_noack = -1;
        ccm_header->receipt_options.recipient_unconnected = -1;
        ccm_header->receipt_options.msg_undelivered_stored = -1;
        ccm_header->receipt_options.msg_undelivered_ttl_expired = -1;
        ccm_header->receipt_options.end_to_end_ack = -1;
        ccm_header->receipt_options.tracert_ack = -1;
        ccm_header->receipt_options.push_notification_sent = -1;
    }

    // content encoding
    ASN1C2CCMEncoding(asn1c_header->contentEncoding, &(ccm_header->content_encoding));

    // content version
    if (asn1c_header->contentVersion)
    {
        ASN1C2CCMVersionInfo(asn1c_header->contentVersion, &(ccm_header->content_version));
    }
    else
    {
        ccm_header->content_version.signal_flow_version = -1;
        ccm_header->content_version.schema_version = -1;
        ccm_header->content_version.preliminary_version = -1;
    }

    return true;
}

static bool EncodeHeader (CCM_Header_t* asn1c_header, fs_Data* header_data)
{
    fs_Data** encode_buffer = static_cast<fs_Data**>(std::malloc(sizeof(fs_Data*)));

    if (encode_buffer)
    {
        *encode_buffer = header_data;

        asn_enc_rval_t result = der_encode(&asn_DEF_CCM_Header,
                                           asn1c_header,
                                           &write_encoded_bytes,
                                           encode_buffer);
        if (result.encoded <= 0)
        {
            DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "Failed to encode header");
            std::free(encode_buffer);
            std::free(header_data->data);
            header_data->data = NULL;
            header_data->num_bytes = 0;
            encode_buffer = NULL;
            return false;
        }

        header_data->num_bytes = result.encoded;

        std::free(encode_buffer);
    }
    else
    {
        return false;
    }

    return true;
}

static bool DecodeHeader (CCM_Header_t** asn1c_header, fs_Data* header_data)
{
    if (!header_data->data)
    {
        return false;
    }

    asn_dec_rval_t result = ber_decode(0,
                                       &asn_DEF_CCM_Header,
                                       (void**)asn1c_header,
                                       header_data->data,
                                       header_data->num_bytes);
    if (result.code != RC_OK)
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "Failed to decode header");
        return false;
    }

    // TODO: we could consider verifying that header.version corresponds to what we implement
    //       but if it did not likely parsing would ahve failed, and if it did not fail we are
    //       likely good?
    return true;
}

static bool SetHeaderSigned (CMS_ContentInfo* signed_data, fs_Data* header)
{
    ASN1_OBJECT* ccm_header_oid = OBJ_txt2obj(kCCMHeaderOID, 1);

    STACK_OF(CMS_SignerInfo)* signerInfos = CMS_get0_SignerInfos(signed_data);
    if (!signerInfos)
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "Failed to get SignerInfos");
        ASN1_OBJECT_free(ccm_header_oid);
        return false;
    }
    else if (sk_CMS_SignerInfo_num(signerInfos) != 1)
    {
        // CCM spec specifies there should be exactly on signerInfo, so this is an erro
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "Found %d singerInfos which is != 1",
                                                  sk_CMS_SignerInfo_num(signerInfos));
        ASN1_OBJECT_free(ccm_header_oid);
        return false;
    }

    CMS_SignerInfo* signerInfo = sk_CMS_SignerInfo_value(signerInfos, 0);

    if (!CMS_signed_add1_attr_by_OBJ(signerInfo,
                                     ccm_header_oid,
                                     V_ASN1_SEQUENCE,
                                     header->data,
                                     header->num_bytes))
    {
        char* error_buffer = ERR_error_string(ERR_get_error(), NULL);
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "Add header to SignedData failed, error: %s",
                                                  error_buffer);
        ASN1_OBJECT_free(ccm_header_oid);
        return false;
    }

    ASN1_OBJECT_free(ccm_header_oid);

    return true;
}

static bool SetHeaderEncrypted (CMS_ContentInfo* envelopedData, fs_Data* header)
{
    ASN1_OBJECT* ccm_header_oid = OBJ_txt2obj(kCCMHeaderOID, 1);

    // this is done in a hacky way as there
    // are no openssl function to manipulate
    // them.

    struct CMS_ContentInfo_st* tmp = (struct CMS_ContentInfo_st*) envelopedData;

    //possibly unnecessery sanity check
    if (OBJ_obj2nid(tmp->contentType) != NID_pkcs7_enveloped)
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "Failed to add header to enveloped data");
        ASN1_OBJECT_free(ccm_header_oid);
        return false;
    }

    STACK_OF(X509_ATTRIBUTE)* headerAttributes = sk_X509_ATTRIBUTE_new_null();

    X509_ATTRIBUTE* headerAttribute = X509_ATTRIBUTE_create_by_OBJ(NULL,
                                                                   ccm_header_oid,
                                                                   V_ASN1_SEQUENCE,
                                                                   header->data,
                                                                   header->num_bytes);
    if (!headerAttribute)
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "Failed to create header attribute");
        sk_X509_ATTRIBUTE_free(headerAttributes);
        ASN1_OBJECT_free(ccm_header_oid);
        return false;
    }

    // possible memory leak? this assumes openssl will free our attributes
    // when freeing the CMS_ContentInfo
    sk_X509_ATTRIBUTE_push(headerAttributes, headerAttribute);
    tmp->d.envelopedData->unprotectedAttrs = headerAttributes;

    ASN1_OBJECT_free(ccm_header_oid);

    return true;
}

static bool AddRecipientsToASN1CHeader(const std::vector<fsm::UserId>& users, CCM_Header_t* header)
{
    bool return_value = true;

    AddressCCM_t* local_address = NULL;
    STACK_OF(X509*) recipient_certificates = NULL;

    header->recipients = static_cast<Recipients_t*>(calloc(1, sizeof(Recipients_t)));
    if (header->recipients)
    {
        header->recipients->list.array = static_cast<AddressCCM_t**>(calloc(1, sizeof(AddressCCM_t*)));
    }

    if (!header->recipients || !(header->recipients->list.array))
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "Failed to allocate header recipients or it's list array.");
        return_value = false;
    }

    if (return_value)
    {
        // Get stack of recipients certificates
        recipient_certificates = sk_X509_new_null();
        if (recipient_certificates)
        {
            return_value = GetActorCerts(recipient_certificates, users);
        }
        else
        {
            DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "Failed to allocate certificate stack.");
            return_value = false;
        }
    }

    if (return_value)
    {
        int num_certificates = sk_X509_num(recipient_certificates);

        for (int i = 0; i < num_certificates; i++)
        {
            local_address = static_cast<AddressCCM_t*>(calloc(1, sizeof(AddressCCM_t)));
            if (local_address)
            {
                // Extract SKI from the recipient's certificate
                return_value = GetSubjectKeyIdentifier(sk_X509_pop(recipient_certificates),
                                                       &(local_address->choice.subjectKeyIdentifier));
                if (return_value)
                {
                    // Set the present field
                    local_address->present = AddressCCM_PR_subjectKeyIdentifier;

                    if (ASN_SEQUENCE_ADD(&(header->recipients->list), local_address) != 0)
                    {
                        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "Fail to add SKI to recipients.");
                        return_value = false;

                        break;
                    }
                }
            } // if (local_address)
        } // for()
    } // if (return_value)

    if (!return_value && local_address)
    {
        asn_DEF_AddressCCM.free_struct(&asn_DEF_AddressCCM, &local_address, 0);
    }

    if (recipient_certificates)
    {
        sk_X509_free(recipient_certificates);
    }

    return return_value;
}

static bool AddSenderToASN1CHeader(const std::vector<fsm::UserId>& counterparts, CCM_Header_t* header)
{
    bool return_value = true;

    X509* sender_cert = NULL;

    // Will be freed by top level recursive free of header
    header->sender = static_cast<AddressCCM_t*>(calloc(1, sizeof(AddressCCM_t)));

    if (!header->sender)
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "Failed to allocate header sender.");
        return_value = false;
    }

    if (return_value)
    {
        return_value = GetVehicleCredentials(&sender_cert, nullptr, counterparts);
    }

    if (return_value)
    {
        // Extract SKI from the recipient's certificate
        return_value = GetSubjectKeyIdentifier(sender_cert,
                                               &(header->sender->choice.subjectKeyIdentifier));
    }
    else
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "Failed to select vehicle credentials.");
    }

    if (return_value)
    {
        // Set the present field
        header->sender->present = AddressCCM_PR_subjectKeyIdentifier;
    }
    else
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "Failed to get SKI.");
    }

    X509_free(sender_cert);

    return return_value;
}

static bool GetSubjectKeyIdentifier(X509* certificate, OCTET_STRING_t* subject_key_identifier)
{
    bool return_value = true;
    ASN1_OCTET_STRING *data = reinterpret_cast<ASN1_OCTET_STRING*>(X509_get_ext_d2i(certificate, NID_subject_key_identifier, 0, 0));

    if (data)
    {
        const unsigned char *bytes = ASN1_STRING_data(data);

        if (0 != OCTET_STRING_fromBuf(subject_key_identifier,
                                      reinterpret_cast<const char *>(bytes),
                                      CCM_SUBJECT_KEY_IDENTIFIER_LEN))
        {
            DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "Failed to copy buf to octet string.");
            return_value = false;
        }

        ASN1_OCTET_STRING_free(data);
    }

    return return_value;
}

static bool GetHeaderSigned (CMS_ContentInfo* signed_data, fs_Data* header)
{
    bool return_value = true;
    ASN1_STRING* rawHeader = NULL;

    ASN1_OBJECT* ccm_header_oid = OBJ_txt2obj(kCCMHeaderOID, 1);

    STACK_OF(CMS_SignerInfo)* signerInfos = CMS_get0_SignerInfos(signed_data);
    if (!signerInfos)
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "Failed to get SignerInfos");
        return_value = false;
    }
    else if (sk_CMS_SignerInfo_num(signerInfos) != 1)
    {
        //CCM spec specifies there should be exactly on signerInfo, so this is an error
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "Found %d singerInfos which is != 1",
                                                  sk_CMS_SignerInfo_num(signerInfos));
        return_value = false;
    }

    if (return_value)
    {
        CMS_SignerInfo* signerInfo = sk_CMS_SignerInfo_value(signerInfos, 0);

        rawHeader = static_cast<ASN1_STRING*>(CMS_signed_get0_data_by_OBJ(signerInfo,
                                                                          ccm_header_oid,
                                                                          0,
                                                                          V_ASN1_SEQUENCE));
        if (!rawHeader)
        {
            DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "Failed to get CCM header");
            return_value = false;
        }
        if (ASN1_STRING_length(rawHeader) <= 0)
        {
            //empty header?
            DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "Found empty CCM header");
            return_value = false;
        }
    }

    if (return_value)
    {
        header->data = ASN1_STRING_data(rawHeader);
        header->num_bytes = ASN1_STRING_length(rawHeader);
    }

    ASN1_OBJECT_free(ccm_header_oid);

    return return_value;
}

static bool GetHeaderEncrypted (CMS_ContentInfo* enveloped_data, fs_Data* header)
{
    int return_value = true;
    ASN1_STRING* rawHeader = NULL;
    STACK_OF(X509_ATTRIBUTE)* headerAttributes = NULL;

    ASN1_OBJECT* ccm_header_oid = OBJ_txt2obj(kCCMHeaderOID, 1);

    // this is done in a hacky way as there
    // are no openssl function to manipulate
    // them.

    struct CMS_ContentInfo_st* tmp = (struct CMS_ContentInfo_st*) enveloped_data;

    //probably unnecessery sanity check, but if it is not so we would likely crash below
    if (OBJ_obj2nid(tmp->contentType) != NID_pkcs7_enveloped)
    {
        //not enveloped?
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_INFO,
                       "GetHeaderEncrypted called with non enveloped content");
        return_value = false;
    }

    if (return_value)
    {
        headerAttributes = tmp->d.envelopedData->unprotectedAttrs;
        if (!headerAttributes)
        {
            DLT_LOG_STRING(dlt_libfsm, DLT_LOG_INFO, "did not find unprotected attributes");
            return_value = false;
        }
    }

    if (return_value)
    {
        rawHeader = static_cast<ASN1_STRING*>(X509at_get0_data_by_OBJ(headerAttributes,
                                                                      ccm_header_oid,
                                                                      -1,
                                                                      V_ASN1_SEQUENCE));
        if (!rawHeader)
        {
            char* error_buffer = ERR_error_string(ERR_get_error(), NULL);
            DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "Failed to get CCM header, error: %s",
                            error_buffer);
            return_value = false;
        }
        if (ASN1_STRING_length(rawHeader) <= 0)
        {
            //empty header?
            DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "Found empty CCM header");
            return_value = false;
        }
    }

    if (return_value)
    {
        header->data = ASN1_STRING_data(rawHeader);
        header->num_bytes = ASN1_STRING_length(rawHeader);
    }

    ASN1_OBJECT_free(ccm_header_oid);

    return return_value;
}

static bool GetSignedData(CMS_ContentInfo* signed_data, fs_Data* data)
{
    int return_value = true;

    ASN1_OCTET_STRING** encap_content = CMS_get0_content(signed_data);
    if(!encap_content || !(*encap_content))
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "Failed to get signed content");
        return_value = false;
    }

    if (return_value && (!ASN1_STRING_data(*encap_content)))
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "Failed to get signed content data");
        return_value = false;
    }
    else if (return_value && ((*encap_content)->length <= 0))
    {
        //empty content?
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "Found empty signed content");
        return_value = false;
    }

    if (return_value)
    {
        data->data = ASN1_STRING_data(*encap_content);
        data->num_bytes = (*encap_content)->length;
    }

    return return_value;
}

static bool EncryptData (BIO** encrypted, fs_Data* data, fs_Data* header, ASN1_OBJECT* oid,
                         const std::vector<fsm::UserId>& recipients)
{
    int return_value = true;

    CMS_ContentInfo* encrypted_data = NULL;
    STACK_OF(X509*) recipient_certificates = NULL;

    // encrypt contents
    BIO* tmp_bio = nullptr;

    if (data->num_bytes > 0)
    {
        tmp_bio = BIO_new_mem_buf(data->data,
                        data->num_bytes);
    }

    if (!tmp_bio)
    {
        return_value = false;
    }

    // get recipient certs
    if (return_value)
    {
        recipient_certificates = sk_X509_new_null();
        if (recipient_certificates)
        {
            return_value = GetActorCerts(recipient_certificates, recipients);
        }
        else
        {
            DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "Failed to allocate certificate stack.");

            return_value = false;
        }
    }

    if (return_value)
    {
        //TODO: which encryption should be used, cant find it in the specs
        encrypted_data = CMS_encrypt(recipient_certificates,
                                     tmp_bio,
                                     EVP_aes_128_ofb(),
                                     kEncryptionFlags);
        if (!encrypted_data)
        {
            char* error_buffer = ERR_error_string(ERR_get_error(), NULL);
            DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "Failed to encrypt data, error: %s",
                            error_buffer);
            return_value = false;
        }
    }

    //set the encryptedContentInfo oid
    if (return_value && (!CMS_set1_eContentType(encrypted_data, oid)))
    {
        char* error_buffer = ERR_error_string(ERR_get_error(), NULL);
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "Failed to set oid in encrypted payload, "
                                                  "error: %s", error_buffer);
        return_value = false;
     }

    // add header to unprotected attributes
    if (return_value && (!SetHeaderEncrypted (encrypted_data, header)))
    {
        return_value = false;
    }

    // finalize encryption
    if (return_value && (!CMS_final(encrypted_data, tmp_bio, NULL, kEncryptionFlags)))
    {
        char* error_buffer = ERR_error_string(ERR_get_error(), NULL);
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "Failed to encrypt data, error: %s",
                                                  error_buffer);
        return_value = false;
    }

    // encode CMS
    if (return_value)
    {
        *encrypted = BIO_new(BIO_s_mem());
        if (!i2d_CMS_bio(*encrypted, encrypted_data))
        {
            char* error_buffer = ERR_error_string(ERR_get_error(), NULL);
            DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "Failed to encode encrypted data, error: %s",
                            error_buffer);
            return_value = false;
        }
    }

    if (return_value)
    {
        // Handle CCM - OPENSSL ContentInfo handling incompatability
        // CCM expects The signedData.encapContentInfo.eContent to directly
        // contain a EnvelopedData.
        // Openssl only has support for der encoding/decoding a EnvelopedData
        // as part of a ContentInfo via d2i/i2d_CMS, i.e. the expect/produce
        // something wrapped in a ContentInfo.
        //
        // Ta handle this we strip the ContentInfo wrapper after:
        // cms_encrypt()
        // i2d_CMS_bio()
        // before sending.
        //
        // When received we need to prepend the wrapper after:
        // cms_verify but before:
        // d2i_CMS_bio()
        //
        // The wrapper is a 19 bytes:
        // 0x30, 0x82, a, b "ContentInfo" sequence tag and length of contained data encoded in a & b
        // 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x07, 0x03 EnvelopedData OID
        // 0xa0, 0x82, c, d eContent tag with length of contained data encoded in c & d

        //TODO: push for change to CCM spec so we can get rid of this

        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_WARN, "Removing EnvelopedData ContentInfo wrapper.\n");

        char buf[kEnvelopedDataContentInfoSize];
        if (kEnvelopedDataContentInfoSize != BIO_read(*encrypted, buf, kEnvelopedDataContentInfoSize))
        {
            DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR,
                           "Failed to remove leading kEnvelopedDataContentInfoSize bytes from EnvelopedData ContentInfo.");

            return_value = false;
        }
        else
        {
            DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR,
                           "Removed leading kEnvelopedDataContentInfoSize bytes from EnvelopedData ContentInfo.");
        }
    }


    CMS_ContentInfo_free(encrypted_data);
    BIO_free(tmp_bio);

    return return_value;
}

static bool DecryptData (fs_Data* data,
                         CMS_ContentInfo* enveloped_data,
                         fsm::UserId sender)
{
    int return_value = true;

    X509* recipient_cert = nullptr;
    EVP_PKEY* recipient_key = nullptr;

    std::vector<fsm::UserId> sender_as_vector;
    if (sender != fsm::kUndefinedUserId)
    {
        //TODO: remove when removing cloud resource workaround
        sender_as_vector.push_back(sender);
    }

    return_value = GetVehicleCredentials(&recipient_cert, &recipient_key, sender_as_vector);

    BIO* decrypted_bio = NULL;

    if (return_value)
    {
        decrypted_bio = BIO_new(BIO_s_mem());
        if (!decrypted_bio)
        {
            return_value = false;
        }
    }

    // decrypt enveloped data
    if (return_value && (!CMS_decrypt(enveloped_data,
                                      recipient_key,
                                      recipient_cert,
                                      NULL,
                                      decrypted_bio,
                                      0)))
    {
        char* error_buffer = ERR_error_string(ERR_get_error(), NULL);
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "failed to decrypt contents, error: %s",
                                                 error_buffer);
        return_value = false;
    }

    // get message contents
    if (return_value)
    {
        BIO_set_close(decrypted_bio, BIO_NOCLOSE);
        data->num_bytes = BIO_get_mem_data(decrypted_bio, &(data->data));
        if (!data->data)
        {
            DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "Failed to decode decrypted content");
            return_value = false;
        }
        else if (data->num_bytes <= 0)
        {
            //empty content?
            DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "Found empty decrypted content");
        return_value = false;
        }
    }

    BIO_free(decrypted_bio);
    EVP_PKEY_free(recipient_key);
    X509_free(recipient_cert);

    return return_value;
}

static bool SignData (BIO** signedBio,
                      BIO* data,
                      fs_Data* header,
                      ASN1_OBJECT* oid,
                      const std::vector<fsm::UserId>& recipients)
{
    int return_value = true;

    X509* sender_cert = NULL;
    EVP_PKEY* sender_key = NULL;

    return_value = GetVehicleCredentials(&sender_cert, &sender_key, recipients);

    CMS_ContentInfo* signed_data = NULL;

    if (return_value)
    {
        signed_data = CMS_sign(sender_cert,
                               sender_key,
                               NULL,        //chain certs to include
                               data,
                               kSignFlags);
    }
    if (!signed_data)
    {
        char* error_buffer = ERR_error_string(ERR_get_error(), NULL);
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "Failed to sign data, error: %s", error_buffer);
        return_value = false;
    }

    if (return_value && (!CMS_set1_eContentType(signed_data, oid)))
    {
        char* error_buffer = ERR_error_string(ERR_get_error(), NULL);
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "Failed to set oid, error: %s", error_buffer);
        return_value = false;
    }

    // add header if specified
    if (return_value && header)
    {
        if (!SetHeaderSigned(signed_data, header))
        {
            return_value = false;
        }
    }

    if (return_value && (!CMS_final(signed_data, data, NULL, kSignFlags)))
    {
        char* error_buffer = ERR_error_string(ERR_get_error(), NULL);
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "Failed to finalize signing of data, error: %s",
                                                  error_buffer);
        return_value = false;
    }

    if (return_value)
    {
        *signedBio = BIO_new(BIO_s_mem());
        if (!i2d_CMS_bio(*signedBio, signed_data))
        {
            char* error_buffer = ERR_error_string(ERR_get_error(), NULL);
            DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "Failed to encode signedData, error: %s",
                            error_buffer);
            return_value = false;
        }
    }

    CMS_ContentInfo_free(signed_data);

    return return_value;
}

static bool VerifyData (CMS_ContentInfo** signed_data,
                        BIO* contentBio,
                        const_fs_Data* data)
{
    int return_value = true;

    // convert data buffer from der to openssl internal format
    BIO* dataBio = BIO_new_mem_buf(data->data, data->num_bytes);
    if (!dataBio)
    {
        char* error_buffer = ERR_error_string(ERR_get_error(), NULL);
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "Failed to allocate dataBio, error: %s",
                                                  error_buffer);
        return_value = false;
    }
    if (return_value)
    {
        *signed_data = d2i_CMS_bio(dataBio, signed_data);
        if (!(*signed_data))
        {
            char* error_buffer = ERR_error_string(ERR_get_error(), NULL);
            DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "Failed to read signedData, error: %s",
                            error_buffer);
            return_value = false;
        }
    }

    // get candidate signer certificates (essentially all known users)
    // and cas
    STACK_OF(X509)* known_signers = nullptr;
    X509_STORE* ca_store = nullptr;

    if (return_value)
    {
        known_signers = sk_X509_new_null();

        if (known_signers)
        {
            return_value = GetKnownSigners(known_signers);
        }
        else
        {
            DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "Failed to allocate certificate stack.");

            return_value = false;
        }
    }
    if (return_value)
    {
        return_value = GetCas(&ca_store);
    }

    // verify the data
    if (return_value && (!CMS_verify(*signed_data,
                                     known_signers,
                                     ca_store,
                                     NULL,
                                     contentBio,
                                     CMS_NOINTERN)))
    {
        char* error_buffer = ERR_error_string(ERR_get_error(), NULL);

        //            DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
        //                  "Failed to verify signedData, error: %s",
        //                  error_buffer);
        //  return_value = false;

        //TODO: Woraround we continue even if verify fails as we donthave backend certs
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_WARN,
                        "Failed to verify signedData, "
                        "ignoring due to cloud resource workaround, "
                        "error: %s",
                        error_buffer);

        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_WARN,
                       "if this is an encrypted message, expect issues");
     }

    if (known_signers)
    {
        sk_X509_pop_free(known_signers, X509_free);
    }
    if (ca_store)
    {
        X509_STORE_free(ca_store);
    }

    BIO_free(dataBio);

    return return_value;
}

static bool VerifySenderSKI(const OCTET_STRING_t* header_ski, const fsm::UserId& sender)
{
    bool return_value = true;

    X509* sender_cert = nullptr;
    FSSubjectKeyIdentifier_t* sender_ski =
        static_cast<FSSubjectKeyIdentifier_t*>(calloc(1, sizeof(FSSubjectKeyIdentifier_t)));

    if (!sender_ski)
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "Failed to allocate SubjectKeyIdentifier.");
        return_value = false;
    }

    if (return_value)
    {
        // get sender's certificate
        std::shared_ptr<fsm::UsermanagerInterface> user_manager = GetUsermanager();

        if (user_manager->GetActorCertificate(&sender_cert, sender) !=
            fsm::ReturnCode::kSuccess)
        {
            return_value = false;
        }
    }

    if (return_value)
    {
        // Get SKI from the certificate
        return_value = GetSubjectKeyIdentifier(sender_cert, sender_ski);

        // Compare SKIs
        return_value = return_value ? CompareOctetSrings(header_ski, sender_ski) : return_value;
    }

    if (sender_cert)
    {
        X509_free(sender_cert);
    }
    if (sender_ski)
    {
        asn_DEF_FSSubjectKeyIdentifier.free_struct(&asn_DEF_FSSubjectKeyIdentifier, sender_ski, 0);
    }

    return return_value;
}

static bool CCMContentType2ASN1C (const fs_Oid* ccm_oid, ContentType_t* asn1c_content_type)
{
    //pick most compact encoding
    CcmOidChoices  selected_choice = CCM_FULL; //find
    int offset = 0; //keeps track of how many initial characters to remove

   selected_choice = CCM_FULL; //TODO: hardcoded to FULL, remove when other parties support partial
   offset=0;

    switch (selected_choice)
    {
    case CCM_BASIC_CAR_STATUS:
        asn1c_content_type->present = ContentType_PR_vccBasicCarStatusOid;
        if (!FSOid2ASN1C(ccm_oid, &(asn1c_content_type->choice.vccBasicCarStatusOid), offset))
        {
            DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "Failed to copy BasicCarStatusoid.");
            return false;
        }
        break;
    case CCM_VCC:
        asn1c_content_type->present = ContentType_PR_vccOid;
        if (!FSOid2ASN1C(ccm_oid, &(asn1c_content_type->choice.vccOid), offset))
        {
            DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "Failed to copy vccOid.");
            return false;
        }
        break;
    case CCM_FULL:
        asn1c_content_type->present = ContentType_PR_oid;
        if (!FSOid2ASN1C(ccm_oid, &(asn1c_content_type->choice.oid), offset))
        {
            DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "Failed to copy oid.");
            return false;
        }
        break;
    default:
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "Found unhandled oid domain: %d, exiting.", selected_choice);
        return false;
    }

    return true;
}

static bool ASN1C2CCMContentType (ContentType_t* asn1c_content_type, fs_Oid* ccm_oid)
{
    const char* prefix = NULL;
    OBJECT_IDENTIFIER_t* asn1c_oid = NULL;
    //construct full oid from the input
    switch (asn1c_content_type->present)
    {
    case ContentType_PR_vccBasicCarStatusOid:
        prefix = kOidMatch[CCM_BASIC_CAR_STATUS];
        asn1c_oid = &asn1c_content_type->choice.vccBasicCarStatusOid;
        break;
    case ContentType_PR_vccOid:
        prefix = kOidMatch[CCM_VCC];
        asn1c_oid = &asn1c_content_type->choice.vccOid;
        break;
    case ContentType_PR_oid:
        prefix = "";
        asn1c_oid = &asn1c_content_type->choice.oid;
        break;
    default:
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "Received unsupported oid domain: %d.", asn1c_content_type->present);
        return false;
    }

    int offset = strlen(prefix); //end of line character should be ignored

    //if there is a valid prefix, add it
    if (offset != 0)
    {
        memcpy(&(ccm_oid->oid[0]), prefix, offset);
    }

    // content type
    if (asn1c_oid && !ASN1C2FSOid(asn1c_oid, ccm_oid, offset))
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "Failed to parse message oid.");
        return false;
    }

    return true;
}


static bool CCMEnvelope2ASN1C (const ccm_Envelope* ccm_envelope, Envelope_t* asn1c_envelope)
{

    if (!CCMContentType2ASN1C(&(ccm_envelope->oid), &(asn1c_envelope->contentType)))
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "Failed to encode content type.");
        return false;
    }

    //content encoding  asn1c_envelope->contentEncoding
    Encoding_t asn1c_encoding;
    if (CCMEncoding2ASN1C(ccm_envelope->content_encoding,
                          &asn1c_encoding))
    {
        //there is something to encode
        asn1c_envelope->contentEncoding = static_cast<Encoding_t*>(calloc(1, sizeof(Encoding_t)));

        if (asn1c_envelope->contentEncoding)
        {
            *(asn1c_envelope->contentEncoding) = asn1c_encoding;
        }
        else
        {
            return false;
        }
    }
    else
    {
        //nothing to encode, optional parameter
        asn1c_envelope->contentEncoding = NULL;
    }


    //content version
    asn1c_envelope->contentVersion = NULL;
    if (!ccm_IsValidVersionInfo(&(ccm_envelope->content_version)))
    {
        asn1c_envelope->contentVersion = NULL;
    }
    else
    {
        asn1c_envelope->contentVersion = static_cast<VersionInfo_t*>(calloc(1, sizeof(VersionInfo_t)));

        if (asn1c_envelope->contentVersion)
        {
            CCMVersionInfo2ASN1C(&(ccm_envelope->content_version), asn1c_envelope->contentVersion);
        }
        else
        {
            return false;
        }
    }

    //content
    if (OCTET_STRING_fromBuf(&(asn1c_envelope->content),
                             reinterpret_cast<const char*>(ccm_envelope->content->data()),
                             ccm_envelope->content->size()) != 0)
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "Failed to copy contents.");
        return false;
    }

    return true;
}

static bool ASN1C2CCMEnvelope (Envelope_t* asn1c_envelope, ccm_Envelope* ccm_envelope)
{
    if (!ASN1C2CCMContentType(&(asn1c_envelope->contentType), &(ccm_envelope->oid)))
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "Failed to parse content type.");
        return false;
    }

    // content encoding
    ASN1C2CCMEncoding(asn1c_envelope->contentEncoding,
                      &(ccm_envelope->content_encoding));

    // content version
    if (asn1c_envelope->contentVersion)
    {
        ASN1C2CCMVersionInfo(asn1c_envelope->contentVersion,
                             &(ccm_envelope->content_version));
    }
    else
    {
        ccm_envelope->content_version.signal_flow_version = -1;
        ccm_envelope->content_version.schema_version = -1;
        ccm_envelope->content_version.preliminary_version = -1;
    }

    // content
    ccm_envelope->content = std::make_shared<std::vector<unsigned char>>();

    if (ccm_envelope->content)
    {
        ccm_envelope->content->assign(asn1c_envelope->content.buf,
                                      asn1c_envelope->content.buf + asn1c_envelope->content.size);
    }
    else
    {
        return false;
    }

    return true;
}

/** \brief Converts a CCM_Payload_t structure to a  ccm_Payload structure
 *  \param[in]     payload - The payload to convert
 *  \param[in/out] asn1c_payload       - The payload to fill in
 *  \return false on failure
 */
static bool CCMPayload2ASN1C (const ccm_Payload* ccm_payload, CCM_Payload_t* asn1c_payload)
{
    if (!CCMEnvelope2ASN1C(&(ccm_payload->content), &(asn1c_payload->message)))
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "Failed to encode message envelope.");
        return false;
    }

    size_t num_metadata = ccm_payload->num_metadata;
    if (num_metadata == 0)
    {
        asn1c_payload->metadata = NULL;
    }
    else
    {
        int point_of_failure = -1;

        if (num_metadata > CCM_MAX_NUM_METADATA)
        {
            num_metadata = CCM_MAX_NUM_METADATA;
            DLT_LOG_STRING(dlt_libfsm, DLT_LOG_WARN, "Too many metadata.");
        }

        asn1c_payload->metadata = static_cast<metadata_t*>(calloc(1, sizeof(metadata_t)));

        if (asn1c_payload->metadata)
        {
            asn1c_payload->metadata->list.array = static_cast<Envelope_t**>(calloc(1, sizeof(Envelope_t*)));
        }

        if (!asn1c_payload->metadata || !asn1c_payload->metadata->list.array)
        {
            return false;
        }

        Envelope_t* current_metadata = 0;

        for (size_t i = 0; i < num_metadata; i++)
        {
            current_metadata = static_cast<Envelope_t*>(calloc(1, sizeof(Envelope_t)));

            if(!current_metadata)
            {
                DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "Failed to allocate metadata envelop");
                point_of_failure = i;
                break;
            }

            if (!CCMEnvelope2ASN1C(&(ccm_payload->metadata[i]), current_metadata))
            {
                DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "Failed to encode metadata.");
                point_of_failure = i;
                break;
            }

            if (ASN_SEQUENCE_ADD(&(asn1c_payload->metadata->list), current_metadata) != 0)
            {
                DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "Failed to add metadata.");
                point_of_failure = i;
                break;
            }
        }

        if (point_of_failure >= 0)
        {
            if (current_metadata)
            {
                asn_DEF_Envelope.free_struct(&asn_DEF_Envelope, current_metadata, 0);
            }

            return false;
        }
    }

    return true;
}

/** \brief Converts a CCM_Payload_t structure to a  ccm_Payload structure
 *  \param[in]     asn1c_payload - The payload to convert
 *  \param[in/out] payload       - The payload to fill in
 *  \return false on failure
 */
static bool ASN1C2CCMPayload(CCM_Payload_t* asn1c_payload, ccm_Payload* payload)
{

    if (!ASN1C2CCMEnvelope(&(asn1c_payload->message), &(payload->content)))
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "Failed to decode message envelope.");
        return false;
    }

    if (asn1c_payload->metadata)
    {
        int num_metadata = asn1c_payload->metadata->list.count;

        if (num_metadata > CCM_MAX_NUM_METADATA)
        {
            DLT_LOG_STRING(dlt_libfsm, DLT_LOG_WARN, "Too many metadata.");
            num_metadata = CCM_MAX_NUM_METADATA;
        }

        for (int i = 0; i < num_metadata; i++)
        {
            if (!ASN1C2CCMEnvelope(asn1c_payload->metadata->list.array[i], &(payload->metadata[i])))
            {
                DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "Failed to decode metadata envelope.");
                payload->num_metadata = 0;
                return false;
            }
        }

        payload->num_metadata = num_metadata;
    }
    else
    {
        payload->num_metadata = 0;
    }

    return true;
}

static bool EncodePayload (CCM_Payload_t* asn1c_payload, fs_Data* payload_data)
{
    fs_Data** encode_buffer = static_cast<fs_Data**>(std::malloc(sizeof(fs_Data*)));

    if (encode_buffer)
    {
        *encode_buffer = payload_data;

        asn_enc_rval_t result = der_encode(&asn_DEF_CCM_Payload,
                                           asn1c_payload,
                                           &write_encoded_bytes,
                                           encode_buffer);
        if (result.encoded <= 0)
        {
            DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "Failed to encode payload");
            std::free(encode_buffer);
            if (payload_data->data)
            {
                std::free(payload_data->data);
            }
            payload_data->data = NULL;
            payload_data->num_bytes = 0;
            encode_buffer = NULL;
            return false;
        }

        payload_data->num_bytes = result.encoded;

        std::free(encode_buffer);
    }
    else
    {
        return false;
    }

    return true;
}

static ccm_Message* CreateMessage (fs_Data* data, const ASN1_OBJECT* oid)
{
    bool result = true;

    ccm_Message* message = NULL;
    CCM_Payload_t* payload = NULL;

    ASN1_OBJECT* ccm_payload_oid = OBJ_txt2obj(kCCMPayloadOID, 1);

    if (OBJ_cmp(ccm_payload_oid, oid) == 0)
    {
        // parse the CCM-Payload
        asn_dec_rval_t rval = ber_decode(0,
                                         &asn_DEF_CCM_Payload,
                                         (void**)&payload,
                                         data->data,
                                         data->num_bytes);
        if (rval.code != RC_OK)
        {
            DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "Failed to decode payload");
            result = false;
        }
    }

    if (result)
    {
        message = new ccm_Message();

        if (payload)
        {
            if (message && !ASN1C2CCMPayload(payload, &(message->payload)))
            {
                delete message;

                message = NULL;
                result = false;
            }
        }
        else if (message)
        {
            message->payload.content.content = std::make_shared<std::vector<unsigned char>>();
            if (message->payload.content.content != nullptr)
            {
                message->payload.content.content->assign(static_cast<char*>(data->data),
                                                         static_cast<char*>(data->data)+data->num_bytes);
                // copy the oid
                OBJ_obj2txt(message->payload.content.oid.oid, FS_OID_MAX_LEN, oid, 1);

                // These are invalid for plain messages (non CCM-Payload), for safety set
                // deafult values here.
                // It would be nice, however, if we could fill this with the valid header
                // values to make life easier for client. But header is not yet populated here
                // and ccm_Decode does not know if it was a CCM-Payload.

                message->payload.content.content_encoding = FS_ENCODING_UNDEFINED;
                message->payload.content.content_version.signal_flow_version = -1;
                message->payload.content.content_version.schema_version = -1;
                message->payload.content.content_version.preliminary_version = -1;

                // initialize metadatas to 0
                message->payload.num_metadata = 0;
            }
            else
            {
                delete message;

                message = NULL;
                result = false;
            }
        }
    }

    if (payload)
    {
        asn_DEF_CCM_Payload.free_struct(&asn_DEF_CCM_Payload, payload, 0);
    }

    ASN1_OBJECT_free(ccm_payload_oid);

    return message;
}

static std::shared_ptr<fsm::UsermanagerInterface> GetUsermanager()
{
    static std::shared_ptr<fsm::UsermanagerInterface> user_manager =
        fsm::UsermanagerInterface::Create(GetKeystore());

    return user_manager;
}

static std::shared_ptr<fsm::Keystore> GetKeystore()
{
    std::shared_ptr<fsm::Keystore> keystore = std::make_shared<fsm::Keystore>();
    return keystore;
}

static bool GetKnownSigners(STACK_OF(X509)* known_signers)
{
    bool return_value = true;

   std::shared_ptr<fsm::Keystore> keystore = GetKeystore();

   const KeyStoreReturnCode ks_rc = keystore->GetKnownSignerCertificates(known_signers);

    if (!(ks_rc == KeyStoreReturnCode::kSuccess
          || ks_rc == KeyStoreReturnCode::kNotFound))
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "fsm_ccm.cc Keystore GetKnownSignerCertificates failed");
        return_value = false;
    }

    return return_value;
}

static bool GetSender(fsm::UserId& sender, CMS_ContentInfo* signed_data)
{
    bool return_value = true;

    std::shared_ptr<fsm::UsermanagerInterface> user_manager = GetUsermanager();

    X509* signer = NULL;  // we will not free this as openssl will retain ownership
    STACK_OF(X509)* signers = CMS_get0_signers(signed_data);

    if (signers)
    {
        int num_signers = sk_X509_num(signers);
        if (num_signers == 0  || num_signers > 1)
        {
            // we are expectign to never see multiple signers
            //TODO: is this valid?
            sender = fsm::kUndefinedUserId;
            return_value = false;
        }
        else
        {
            signer = sk_X509_value(signers, 0);
        }
    }
    else
    {
        char* error_buffer = ERR_error_string(ERR_get_error(), NULL);
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "Failed to find signer certificates, error: %s",
                                                  error_buffer);

        sender = fsm::kUndefinedUserId;
        return_value = false;
    }

    if (return_value)
    {
        if (user_manager->GetUser(sender, signer) != fsm::ReturnCode::kSuccess)
        {
            sender = fsm::kUndefinedUserId;
            return_value = false;
        }
    }

    return return_value;
}

static bool GetVehicleCredentials(X509** cert,
                                  EVP_PKEY** key,
                                  const std::vector<fsm::UserId>& counterparts)
{
    bool return_value = true;

    std::shared_ptr<fsm::Keystore> keystore = GetKeystore();
    std::shared_ptr<fsm::UsermanagerInterface> user_manager = GetUsermanager();

    // first establish which domain the counterparts are in

    bool vehicle_domain = false;
    bool connected_car_domain = false;

    if (counterparts.empty())
    {
        // we assume the message is to/from the backend if there is no counterpart
        connected_car_domain = true;
    }

    for (fsm::UserId user : counterparts)
    {
        fsm::UserRole role = fsm::UserRole::kUndefined;

        if (user_manager->GetUserRole(role, user) == fsm::ReturnCode::kSuccess)
        {
            if (role == fsm::UserRole::kAdmin ||
                role == fsm::UserRole::kUser ||
                role == fsm::UserRole::kDelegate)
            {
                vehicle_domain = true;
            }
            else
            {
                connected_car_domain = true;
            }
        }
    }
    if (vehicle_domain == connected_car_domain)
    {
        // either we found counterparts in both domains
        // or we were not able to establish any domain
        return_value = false;
    }

    if (vehicle_domain)
    {
        if (keystore->GetCertificate(cert, CertificateRole::kVehicleActor) !=
            KeyStoreReturnCode::kSuccess)
        {
            return_value = false;
        }
        else if (keystore->GetKey(key, KeyRole::kVehicleActor) !=
                 KeyStoreReturnCode::kSuccess)
        {
            return_value = false;
        }
    }
    else
    {
        if (keystore->GetCertificate(cert, CertificateRole::kCloudSignEncrypt) !=
            KeyStoreReturnCode::kSuccess)
        {
            return_value = false;
        }
        else if (keystore->GetKey(key, KeyRole::kCloudSignEncrypt) !=
                 KeyStoreReturnCode::kSuccess)
        {
            return_value = false;
        }
    }

    return return_value;
}

static bool GetCas(X509_STORE** ca_store)
{
    bool return_value = true;

    std::shared_ptr<fsm::Keystore> keystore = GetKeystore();

    if (keystore->GetCertificateAuthorities(ca_store) !=
        KeyStoreReturnCode::kSuccess)
    {
        return_value = false;
    }

    return return_value;
}

static bool GetActorCerts(STACK_OF(X509)* certificates, const std::vector<fsm::UserId>& users)
{
    bool return_value = true;

    std::shared_ptr<fsm::UsermanagerInterface> user_manager = GetUsermanager();

    if (user_manager->GetActorCertificates(certificates, users) !=
        fsm::ReturnCode::kSuccess)
    {
        return_value = false;
    }

    return return_value;
}
