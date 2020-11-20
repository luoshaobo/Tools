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
 *  \file     fsm_ccm.h
 *  \brief    Foundation Services Connectivity Compact Messages.
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup foundationservicemanager
 *  \{
 */


#ifndef FSM_CCM_CCM_H_
#define FSM_CCM_CCM_H_

/******************************************************************************
 * INCLUDE FILES
 ******************************************************************************/
#include "OBJECT_IDENTIFIER.h"
#include "OCTET_STRING.h"


#include <time.h>

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>
#include <string>
#include <vector>

#include <openssl/cms.h>
#include <openssl/err.h>
#include <stdint.h>


#include "usermanager_types.h"

/******************************************************************************
 * MACROS
 ******************************************************************************/


/******************************************************************************
 * DEFINES
 ******************************************************************************/

/** \brief version of the schema we are currently supporting */
const int kImplementedSchemaVersion = 1;

/** \brief version of the signaling flow that we are currently supporting */
const int kImplementedSignalFlowVersion = 1;

/** \brief preliminary version we are currently supporting */
const int kImplementedPreliminaryVersion = 9;

/** \brief minimum valid oid length 1.3.6.1.4.1.37916.2.X */
#define FS_OID_MIN_LEN 21

/** \brief length of the volvo domain oid, minimum valid oid length 1.3.6.1.4.1.37916 */
#define FS_OID_VOLVO_DOMAIN_LEN 18

/** \brief maximum valid oid length */
#define FS_OID_MAX_LEN 40

/** \brief Volvo's OID domain */
const char kVolvoOidDomain[] = "1.3.6.1.4.1.37916.";

/** \brief Basic Car Status' OID domain */
const char kBasicCarStatusOidDomain[] = "1.3.6.1.4.1.37916.3.7.";

/** \brief UUID length */
#define FS_UUID_LEN 16

/** \brief VIN length */
#define FS_VIN_LEN 17

/** \brief maximum number of metadata items */
#define CCM_MAX_NUM_METADATA 50

/** \brief maximum number of recipients */
#define CCM_MAX_NUM_RECIPIENTS 100

/** \brief maximum setting for the time to live parameter (in miliseconds) */
#define CCM_MAX_NUM_TIME_TO_LIVE_MILLISECONDS 3600000

/** \brief session Id length */
#define CCM_SESSION_ID_LEN 16

/** \brief transaction Id length */
#define CCM_TRANSACTION_ID_LEN 6

/** \brief subject key identifier length */
#define CCM_SUBJECT_KEY_IDENTIFIER_LEN 20

/** \brief max value of uint 16 */
#define CCM_MAX_UINT_16 UINT16_MAX

/** \brief flags to control behavior of EncodeCCM */
typedef enum
{
    CCM_ENCODE_AS_PAYLOAD = 0x01, /**< encode contents as CCM_Payload */
    CCM_DO_NOT_ENCRYPT = 0x02, /**< Only sign data, no encrption */
} ccm_EncodeFlags;

/** \brief flags to control behavior of DecodeCCM */
typedef enum
{
    //TODO: remove
    CCM_CLOUD_RESOURCE_WORKAROUND = 0x01, /**< disable signer verification and accept attached certificates */
} ccm_DecodeFlags;

/** \brief Enocoding */
typedef enum
{
    FS_ENCODING_UNDEFINED,
    FS_ENCODING_BER,
    FS_ENCODING_CER,
    FS_ENCODING_DER,
    FS_ENCODING_OER,
    FS_ENCODING_PER,
    FS_ENCODING_UPER,
    FS_ENCODING_XER,
    FS_ENCODING_XML
} fs_Encoding;

/** \brief Foundation Services Object Identifier, used to identify resources
 *         1.3.6.1.4.1.37916.x.y.z.a.b.c is Volvo Cars
 *         1.3.6.1.4.1.37916.6.y.z.a.b.c is Connectivity Compact Message
 *         y=feature, z=function and a,b,c together specify the specifiv message type
 *
 *         TODO: should maybe be in a higher level header
 *         TODO: what int type to use, not able to find range clearly defined
 *         TODO: write helper functions to work with oids
 *         TODO: add int[] and enumerated fields
 */
typedef struct fs_Oid
{
    char oid[FS_OID_MAX_LEN+1]; /**< Object Identifier */

    bool operator==(const fs_Oid& other) const
    {
        bool return_value = true;

        for (int i = 0; i < FS_OID_MAX_LEN+1 && return_value; i++)
        {
            return_value = return_value && oid[i] == other.oid[i];
        }

        return return_value;
    }
} fs_Oid;

/** \brief UUID type */
typedef uint8_t UUID[FS_UUID_LEN];


/** \brief Foundation Services version information */
typedef struct fs_VersionInfo
{
    long signal_flow_version;  /**< signal flow version, optional, -1 is undefined */ //Not valid for CCM schemas
    long schema_version;       /**< schema version */
    long preliminary_version;  /**< preliminary version, optional, -1 is undefined */

    bool operator==(const fs_VersionInfo& other) const
    {
        bool return_value = true;

        return_value = return_value && signal_flow_version == other.signal_flow_version;
        return_value = return_value && schema_version == other.schema_version;
        return_value = return_value && preliminary_version == other.preliminary_version;

        return return_value;
    }

    bool operator!=(const fs_VersionInfo& other) const
    {
        bool return_value = true;

        return_value = return_value && signal_flow_version != other.signal_flow_version;
        return_value = return_value && schema_version != other.schema_version;
        return_value = return_value && preliminary_version != other.preliminary_version;

        return return_value;
    }

} fs_VersionInfo;


/** \brief Data container */
typedef struct fs_Data
{
    void* data; /**< raw data, must be freed when no longer needed */
    size_t num_bytes;  /**< number of bytes of data */

    bool operator==(const fs_Data& other) const
    {
        bool return_value = true;

        return_value = return_value && num_bytes == other.num_bytes;
        if (return_value && data && other.data)
        {
            return_value = return_value && 0 == std::memcmp(data, other.data, num_bytes);
        }
        else if (!(!data && !other.data))
        {
            return_value = false;
        }

        return return_value;
    }

} fs_Data;

/** \brief Data container, const version */
typedef struct const_fs_Data
{
    const void* data; /**< raw data */
    const size_t num_bytes;  /**< number of bytes of data */

    bool operator==(const const_fs_Data& other) const
    {
        bool return_value = true;

        return_value = return_value && num_bytes == other.num_bytes;
        if (data && other.data)
        {
            return_value = return_value && 0 == std::memcmp(data, other.data, num_bytes);
        }
        else if (data && !other.data)
        {
            return_value = false;
        }
        else if (!data && other.data)
        {
            return_value = false;
        }

        return return_value;
    }

} const_fs_Data;

/** \brief Supported address types */
typedef enum
{
    ADDRESS_TYPE_UNDEFINED,
    ADDRESS_TYPE_VEHICLE,
    ADDRESS_TYPE_USER_ID,
    ADDRESS_TYPE_DEVICE_ID,
    ADDRESS_TYPE_SUBJECT_KEY_IDENTIFIER,
    ADDRESS_TYPE_URI,
    ADDRESS_TYPE_INSTANCE_ID,
    ADDRESS_TYPE_OBJECT_ID
} ccm_AddressType;


/** \brief A address to a recipient or sender of a CMM */
typedef struct ccm_Address
{
    ccm_AddressType type; /**< CCM address type */

    /** \brief Supported address types */
    union TypedAddress {
        char vehicle[FS_VIN_LEN];  /**< Vehicle's VIN number */
        char user_id[FS_UUID_LEN]; /**< User Id */
        char device_id[FS_UUID_LEN];  /**< Device Id */
        char subject_key_identifier[CCM_SUBJECT_KEY_IDENTIFIER_LEN];  /**< Subject key identifier*/
        char* uri;  /**< URI as null terminated utf-8 */
        char instance_id[FS_UUID_LEN];  /**< Instance Id */
        fs_Oid object_id; /**< Object Id */
    } address; /**< CCM Address*/

    bool operator==(const ccm_Address& other) const
    {
        bool return_value = true;

        return_value = return_value && type == other.type;

        switch (type)
        {
        case ADDRESS_TYPE_UNDEFINED :
            break;
        case ADDRESS_TYPE_VEHICLE :
            for (int i = 0; i < FS_VIN_LEN && return_value; i++)
            {
                return_value = return_value && address.vehicle[i] == other.address.vehicle[i];
            }
            break;
        case ADDRESS_TYPE_USER_ID :
            for (int i = 0; i < FS_UUID_LEN && return_value; i++)
            {
                return_value = return_value && address.user_id[i] == other.address.user_id[i];
            }
            break;
        case ADDRESS_TYPE_DEVICE_ID :
            for (int i = 0; i < FS_UUID_LEN && return_value; i++)
            {
                return_value = return_value && address.device_id[i] == other.address.device_id[i];
            }
            break;
        case ADDRESS_TYPE_SUBJECT_KEY_IDENTIFIER :
            for (int i = 0; i < CCM_SUBJECT_KEY_IDENTIFIER_LEN && return_value; i++)
            {
                return_value = return_value && address.subject_key_identifier[i] == other.address.subject_key_identifier[i];
            }
            break;
        case ADDRESS_TYPE_URI :
            //TODO: scary, can we bound this somehow?
            if (address.uri && other.address.uri)
            {
                return_value = return_value && 0 == std::strcmp(address.uri, other.address.uri);
            }
            else if (address.uri && !other.address.uri)
            {
                return_value = false;
            }
            else if (!address.uri && other.address.uri)
            {
                return_value = false;
            }
            break;
        case ADDRESS_TYPE_INSTANCE_ID :
            for (int i = 0; i < FS_UUID_LEN && return_value; i++)
            {
                return_value = return_value && address.instance_id[i] == other.address.instance_id[i];
            }
            break;
        case ADDRESS_TYPE_OBJECT_ID :
            return_value = return_value && address.object_id == other.address.object_id;
            break;
        default :
            return_value = false;
        }

        return return_value;
    }

} ccm_Address;


/** \brief options to signaling service controling how to deliver messages */
typedef enum
{
    CCM_STORE_AND_FORWARD_UNDEFINED, /**< used to denote that no options are defined, cloud will use default */
    CCM_STORE_AND_FORWARD_ALWAYS, /**< default used by cloud when options are undefined */
    CCM_STORE_AND_FORWARD_LATEST,
    CCM_STORE_AND_FORWARD_NEVER
} ccm_storeAndForwardOptions;

/** \brief options to signaling service controling how to deliver messages */
typedef enum
{
    CCM_SMS_DELIVERY_UNDEFINED,
    CCM_SMS_DELIVERY_SEND_SMS_AS_BACKUP, /**< default */
    CCM_SMS_DELIVERY_SEND_SMS_AS_BACKUP_AND_RETAIN
} ccm_smsDeliveryOptions;

/** \brief options to signaling service controling how to deliver messages */
typedef struct ccm_DeliveryOptions
{
    ccm_storeAndForwardOptions store_and_forward; /**< optional */
    long qos_level; /**< optional */
    int send_push_notification; /**< boolean value, -1 is unset 0 is false, optional */
    ccm_smsDeliveryOptions sms_delivery_options; /**< optional */

    bool operator==(const ccm_DeliveryOptions& other) const
    {
        bool return_value = true;

        return_value = return_value && store_and_forward == other.store_and_forward;
        return_value = return_value && qos_level == other.qos_level;
        return_value = return_value && send_push_notification == other.send_push_notification;
        return_value = return_value && sms_delivery_options == other.sms_delivery_options;

        return return_value;
    }

} ccm_DeliveryOptions;

/** \brief options to signaling service controling how to deliver messages*/
typedef struct ccm_ReceiptOptions
{
    int msg_delivered_ack; /**< optional */
    int msg_delivered_sms_ack; /**< optional */
    int msg_delivered_sms_noack; /**< optional */
    int recipient_unconnected; /**< optional */
    int msg_undelivered_stored; /**< optional */
    int msg_undelivered_ttl_expired; /**< optional */
    int end_to_end_ack; /**< optional */
    int tracert_ack; /**< optional */
    int push_notification_sent; /**< optional */

    bool operator==(const ccm_ReceiptOptions& other) const
    {
        bool return_value = true;

        return_value = return_value && msg_delivered_ack == other.msg_delivered_ack;
        return_value = return_value && msg_delivered_sms_ack == other.msg_delivered_sms_ack;
        return_value = return_value && msg_delivered_sms_noack == other.msg_delivered_sms_noack;
        return_value = return_value && recipient_unconnected == other.recipient_unconnected;
        return_value = return_value && msg_undelivered_stored == other.msg_undelivered_stored;
        return_value = return_value && msg_undelivered_ttl_expired == other.msg_undelivered_ttl_expired;
        return_value = return_value && end_to_end_ack == other.end_to_end_ack;
        return_value = return_value && tracert_ack == other.tracert_ack;
        return_value = return_value && push_notification_sent == other.push_notification_sent;

        return return_value;
    }

} ccm_ReceiptOptions;

/** \brief CMM header */
typedef struct ccm_Header
{
    fs_VersionInfo version;  \
        /**< header syntax version */
    struct timespec timestamp;  \
        /**< creation time of the message, zero tv_nsec will be omitted from encoded header
         *   as it is an optional parameter, -1 tv_nsec means that no timestamp value is specified*/
    uint64_t time_to_live; \
        /**< number of millis the message should live, max val = 5*10^10 */
    char session_id[CCM_SESSION_ID_LEN]; \
        /**< uniquely identifies which session the message belongs to (UUID), optional (16 bytes of 0 is handled as undefined) */
    char transaction_id[CCM_TRANSACTION_ID_LEN]; \
        /**< uniquely identifies which transaction the message belongs to, filled with zeros if not provided*/
    uint16_t message_id; \
        /**< counter for messages sent during a transaction, CCM_MAX_UINT_16 if no value set  */
    uint8_t resend_counter; \
        /**< resend counter for this unique message, optional */
    ccm_Address sender; \
        /**< identifies the sender of the message, populated during decode if provided but ignored during encode, optional */
    ccm_Address recipients[CCM_MAX_NUM_RECIPIENTS]; \
        /**< list of recipients, populated during decode if provided but ignored during encode, optional */
    ccm_DeliveryOptions delivery_options; \
        /**< message delivery options, optional */
    ccm_ReceiptOptions receipt_options; \
        /**< details which receipts sender wants, optional */
    fs_Encoding content_encoding; \
        /**< Encoding used for payload, unless specified in envelope */
    fs_VersionInfo content_version; \
        /**< Content syntax used for payload, unless specified in envelope */

    bool operator==(const ccm_Header& other) const
    {
        bool return_value = true;

        return_value = return_value && version == other.version;
        return_value = return_value && timestamp.tv_sec == other.timestamp.tv_sec;
        return_value = return_value && timestamp.tv_nsec == other.timestamp.tv_nsec;
        return_value = return_value && time_to_live == other.time_to_live;
        for (int i = 0; i < CCM_SESSION_ID_LEN && return_value; i++)
        {
            return_value = return_value && session_id[i] == other.session_id[i];
        }
        for (int i = 0; i < CCM_TRANSACTION_ID_LEN && return_value; i++)
        {
            return_value = return_value && transaction_id[i] == other.transaction_id[i];
        }
        return_value = return_value && message_id == other.message_id;
        return_value = return_value && resend_counter == other.resend_counter;
        return_value = return_value && sender == other.sender;
        for (int i = 0; i < CCM_MAX_NUM_RECIPIENTS && return_value; i++)
        {
            return_value = return_value && recipients[i] == other.recipients[i];
        }
        return_value = return_value && delivery_options == other.delivery_options;
        return_value = return_value && receipt_options == other.receipt_options;
        return_value = return_value && content_encoding == other.content_encoding;
        return_value = return_value && content_version == other.content_version;

        return return_value;
    }

} ccm_Header;


/** \brief CMM content Envelope */
typedef struct ccm_Envelope
{
    fs_Oid oid;                                          /**< Identifier of the message/metadata */
    fs_Encoding content_encoding;                        /**< Encoding used for payload */
    fs_VersionInfo content_version;                      /**< Version of the content */
    std::shared_ptr<std::vector<unsigned char>> content; /**< Payload */

    bool operator==(const ccm_Envelope& other) const
    {
        bool return_value = true;

        return_value = return_value && oid == other.oid;
        return_value = return_value && content_encoding == other.content_encoding;
        return_value = return_value && content_version == other.content_version;
        if (other.content && content)
        {
            return_value = return_value && *(content) == *(other.content);
        }
        else if (other.content && !content)
        {
            return_value = false;
        }
        else if (!other.content && content)
        {
            return_value = false;
        }

        return return_value;
    }

} ccm_Envelope;


/** \brief CMM payload */
typedef struct ccm_Payload
{
    ccm_Envelope content;  /**< actual message */
    ccm_Envelope metadata[CCM_MAX_NUM_METADATA];  /**< metadata, may be empty*/
    size_t num_metadata; /**< number of metadatas present, may be 0 */
    //TODO, maybe this is to wasteful?

    bool operator==(const ccm_Payload& other) const
    {
        bool return_value = true;

        return_value = return_value && content == other.content;
        return_value = return_value && num_metadata == other.num_metadata;

        if (return_value)
        {
            for (unsigned int i = 0; i < num_metadata && return_value; i++)
            {
                return_value = return_value && metadata[i] == other.metadata[i];
            }
        }

        return return_value;
    }

} ccm_Payload;


/** \brief decoded CMM message
  *
  * \note We treet message payload as a CCM-Payload even if it is
  *       a plain message OID with no metadata, in that case the
  *       payload member metadata will be empty.
  */
typedef struct ccm_Message
{
    ccm_Header header;      /**< Message header */
    ccm_Payload payload;    /**< Message payload */

    bool operator==(const ccm_Message& other) const
    {
        return header == other.header && payload == other.payload;
    }

    void operator delete(void* p)
    {
        ccm_Message *msg = static_cast<ccm_Message*>(p);
        if (msg->header.sender.type == ADDRESS_TYPE_URI)
        {
            delete [] msg->header.sender.address.uri;
            msg->header.sender.address.uri = nullptr;
        }
        for (int i = 0; i < CCM_MAX_NUM_RECIPIENTS; i++)
        {
            if (msg->header.recipients[i].type == ADDRESS_TYPE_URI)
            {
                delete [] msg->header.recipients[i].address.uri;
                msg->header.recipients[i].address.uri = nullptr;
            }
        }
        ::operator delete(msg);
    }

} ccm_Message;


/******************************************************************************
 * GLOBAL FUNCTIONS
 ******************************************************************************/


/*****************************************************************************/
/** \brief Verify if a oid is valid.
 *         Should be larger than 0 band no larger than FS_OID_MAX_LEN.
 *         Should be part of the Volvo OID domain:
 *         1.3.6.1.4.1.37916.*
 *
 *  \param[in]  oid_string - The string to validate.
 *
 *  \return false if unsuccessful, true if successful.
 *****************************************************************************/
bool ccm_IsValidOid(const char* oid_string);


/*****************************************************************************/
/** \brief Populates a fs_Oid from a character sequence.
 *  \param[out] oid        - The oid to populate.
 *  \param[in]  oid_string - The string to populate it with. Should be a valid OID.
 *  \return false if unsuccessful, true if successful.
 *****************************************************************************/
bool ccm_SetOid(fs_Oid* oid, const char* oid_string);


/*****************************************************************************/
/** \brief Checks if a fs_VersionInfo contains valid data.
 *  \param[in] version_info - The fs_VersionInfo to check.
 *  \return true if schema_version has been set, false otherwise
 *****************************************************************************/
bool ccm_IsValidVersionInfo(const fs_VersionInfo* version_info);


/*****************************************************************************/
/** \brief Checks if two fsVersionInfos are equal.
 *  \param[in] a - The first fs_VersionInfo to compare.
 *  \param[in] b - The second fs_VersionInfo to compare.
 *  \return true if all fields that are set in both version_info_one
 *          and version_info_two are equal, false otherwise.
 *****************************************************************************/
bool ccm_CompareVersionInfo(const fs_VersionInfo* version_info_one,
                            const fs_VersionInfo* version_info_two);


/*****************************************************************************/
/** \brief Decompress, decode and decrypt a Connectivity Compact Message.
 *  \param[in]  data         - binary message.
 *  \param[out] message      - decoded and decrypted message, or NULL if unsuccessful.
 *  \param[out] sender       - user which signed ccm, or kUndefinedUserId if unsuccessful.
 *  \param[out] certificates - will be set to point to a stack of all certificates contained in the CCM
 *                             CMS structure, or nullptr if non were found.
 *                             Caller must free the stack using something like sk_X509_pop_free.
 *  \param[in]  flags        - optional set of ccm_DecodeFlags.
 *                             TODO: this is ha hacky w/o:
 *                             CCM_CLOUD_RESOURCE_WORKAROUND will disable signer
 *                             verification of the CCM and enable accepting attached
 *                             certificates, used for cloud resources while they are
 *                             signed by self signed certificates.
 *  \return false if unsuccessful, true if successful.
 *****************************************************************************/
bool ccm_Decode (const_fs_Data* data,
                 ccm_Message** message,
                 fsm::UserId& sender,
                 STACK_OF(X509)** certificates,
                 unsigned int flags);


/*****************************************************************************/
/** \brief Compress, encode and encrypt a Connectivity Compact Message.
 *  \param[out] data         - buffer with binary encoded and encrypted message
 *                             will be created and data->data pointer will be set
 *                             to point to it if successful.
 *                             Caller must free!
 *  \param[in]  message      - message to be encoded and encrypted.
 *                             Note that contents of message will affect behaviour,
 *                             for instance if there are no metadata the contents will
 *                             not be encoded as a CCM-Payload.
 *  \param[in]  recipients   - list of recipients.
 *                             If recipinet list is empty the message will not be
 *                             encrypted and will be signed using Connected Car domain
 *                             credentials.
 *  \param[in]  flags        - optional set of ccm_EncodeFlags.
 *                             CCM_ENCODE_AS_PAYLOAD will cause contents to be
 *                             encoded as CCM-Paylod even if no metadata, enabling
 *                             encryption of message oid.
 *  \return false if unsuccessful, true if successful.
 *****************************************************************************/
bool ccm_Encode (fs_Data* data,
                 const ccm_Message* message,
                 const std::vector<fsm::UserId>& recipients,
                 unsigned int flags);


/*****************************************************************************/
/** \brief Encodes OID provided in fs_Oid structure into ASN1C
 *  \param[in]    fs_oid         - OID encoded as fs_Oid structure
 *  \param[out]   asn1c_oid      - ASN encoded OID
 *  \param[in]    offset         - optional, used to encode relative OIDs. Indicates
 *                                 the number of characters in OID to skip
 *                                 (from the begining of OID)
 *  \return false if unsuccessful, true otherwise
 *****************************************************************************/
bool FSOid2ASN1C (const fs_Oid* fs_oid, OBJECT_IDENTIFIER_t* asn1c_oid, int offset = 0);

/*****************************************************************************/
/** \brief Copies ASN1C encoded OID into fs_Oid structure
 *  \param[in]     asn1c_oid          - object identifier to copy
 *  \param[out]    fs_oid             - fs_Oid structure to copy to
 *  \param[in]     offset             - position in fsOid from which copying should begin,
 *                                      used for relative OIDs
 *  \return false if unsuccessful, true otherwise
 *****************************************************************************/
bool ASN1C2FSOid (OBJECT_IDENTIFIER_t* asn1c_oid, fs_Oid* fs_oid, int offset = 0);



/*****************************************************************************/
/** \brief Copies a ASN1C octet string of a certain expected length
 *         into a "ccm" char buffer.
 *  \param[in]      asn1c_octet_string - octet string to copy
 *  \param[in,out]  ccm_buffer         - buffer to copy to
 *  \param[in]      buffer_length      - the expected length
 *  \return false if unsuccessful
 *****************************************************************************/
bool ASN1C2OctetString (OCTET_STRING_t* asn1c_octet_string,
                        char* ccm_buffer,
                        size_t buffer_length);

/*****************************************************************************/
/**
 * \brief Convert asn1c UTF8 string to std::string type.
 * \param[in] asn1_str the asn1c string for input
 * \param[in/out] to_str the std::string for output
 *****************************************************************************/
void ASN1String2StdString(const OCTET_STRING_t &asn1_str, std::string &to_str);


/*****************************************************************************/
/** \brief generated random id of a given length
 *  \param[in] length - length of the id
 *  \return generated id
 *****************************************************************************/
std::string FsmGenerateId(const int length);

#endif /*FSM_CCM_CCM_H_ */
/** \}    end of addtogroup */
