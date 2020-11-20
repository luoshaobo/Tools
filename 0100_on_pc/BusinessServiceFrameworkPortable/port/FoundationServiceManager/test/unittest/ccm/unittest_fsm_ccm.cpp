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
 *  \file     unittest_fsm_ccm.cpp
 *  \brief    Foundation Services Connectivity Compact Message, Test Cases
 *  \author   Iulian Sirghi
 *
 *  \addtogroup unittest
 *  \{
 */

/******************************************************************************
 * INCLUDE FILES
 ******************************************************************************/

#include <stack>
#include <string>
#include <vector>

#include <openssl/evp.h>

#include <gtest/gtest.h>

#include <INTEGER.h>
#include <CCM-Header.h>
#include "fsm_ccm.h"

#include "certUtils.h"
#include "keystore_stub.h"
#include "usermanager_interface.h"
#include "utilities.h"

#include <malloc.h>

/******************************************************************************
 * GLOBALS
 ******************************************************************************/
namespace
{
    const char* g_pDummyOid = "1.3.6.1.4.1.37916.6.1.1.1.1.1"; // A dummy OID (with CCM namespace)
}

/******************************************************************************
 * TESTS
 ******************************************************************************/

/**
 * Assert error when encoding and OID is NULL
 *
 * TEST PASSES:
 *      ccm_Encode fails because OID is NULL
 *
 * TEST FAILS:
 *      ccm_Encode reports success even though the message OID is NULL
 */
TEST(CcmTest, CCMEncodingNullOid)
{
    std::shared_ptr<std::vector<unsigned char>> content =
        std::make_shared<std::vector<unsigned char>>(4, 'a');

    ccm_Message message = ccm_Message();
    fs_Data encodedData;

    memset(&encodedData, 0, sizeof(encodedData));

    std::vector<fsm::UserId> no_recipients;

    // Add content so that we make sure the failure reason is the NULL OID
    message.payload.content.content = content;

    // Encoding is to fail because message OID was not supplied
    EXPECT_FALSE(ccm_Encode(&encodedData, &message, no_recipients, 0));

    // Encoding when supplying flag CCM_ENCODE_AS_PAYLOAD should also fail
    EXPECT_FALSE(ccm_Encode(&encodedData, &message, no_recipients, CCM_ENCODE_AS_PAYLOAD));
}

/**
 * ccm_Encode/ccm_Decode on emtpy-string Content
 *
 * TEST PASSES:
 *      ccm_Encode/ccm_Decode is successful on empty-string Content
 *      if encoded as payload
 *
 * TEST FAILS:
 *      ccm_Encode/ccm_Decode is not successful on empty-string Content
 *      encoded as payload
 */
TEST(CcmTest, CCMCodingEncryptEmptyStringPayload)
{
    std::shared_ptr<std::vector<unsigned char>> content =
        std::make_shared<std::vector<unsigned char>>();

    std::shared_ptr<fsm::UsermanagerInterface> um_if = fsm::UsermanagerInterface::Create();

    ccm_Message message = ccm_Message();
    ccm_Message* pDecodedMessage = 0;
    fs_Data encodedData;
    fs_Data decodedData;

    //Generate certificates
    fsm::GenerateCerts();

    memset(&encodedData, 0, sizeof(encodedData));
    memset(&decodedData, 0, sizeof(decodedData));

    // Form recipient stack
    std::vector<fsm::UserId> recipients;
    um_if->GetUsers(recipients);

    // Setup OID
    strcpy(message.payload.content.oid.oid, g_pDummyOid);

    message.payload.content.content = content;

    EXPECT_FALSE(ccm_Encode(&encodedData, &message, recipients, 0));
    ASSERT_TRUE(ccm_Encode(&encodedData, &message, recipients, CCM_ENCODE_AS_PAYLOAD));

    fsm::UserId sender;

    ASSERT_TRUE(ccm_Decode((const_fs_Data*)&encodedData, &pDecodedMessage, sender, nullptr, 0));


    EXPECT_EQ(*content, *(pDecodedMessage->payload.content.content));

    fsm::FreeCerts();

    delete pDecodedMessage;
    free(encodedData.data);
}

/**
 * ccm_Encode is consistent for zero-length data and unalters input/output data on failure
 *
 * TEST PASSES:
 *      ccm_Encode is consistent on NULL Content
 *
 * TEST FAILS:
 *      ccm_Encode is not consistent on NULL Content
 */
TEST(CcmTest, CCMCodingEncryptNullContent)
{
    ccm_Message message = ccm_Message();
    ccm_Message* pDecodedMessage = 0;
    fs_Data encodedData;
    fs_Data decodedData;

    memset(&encodedData, 0, sizeof(encodedData));
    memset(&decodedData, 0, sizeof(decodedData));

    std::vector<fsm::UserId> no_recipients;

    // Setup OID
    strcpy(message.payload.content.oid.oid, g_pDummyOid);

    // Just to be verbose, Payload is specifically set to 0.
    message.payload.content.content = nullptr;

    // Expected to be failed due to NULL contents.
    EXPECT_FALSE(ccm_Encode(&encodedData, &message, no_recipients, 0));

    // Encoded data ptr and length should be zero.
    EXPECT_EQ(0, encodedData.data);
    EXPECT_EQ(0, encodedData.num_bytes);
}

/**
 * No error should be generated if recipients are empty.
 * Empty recipient(s) is to be interpreted as "Signed-Only" Content
 *
 * TEST PASSES:
 *      ccm_Encode/ccm_Decode is successful on "Signed-Only" Content
 *
 * TEST FAILS:
 *      ccm_Encode/ccm_Decode is not successful on "Signed-Only" Content
 */
TEST(CcmTest, CCMCodingSignedNoEncrypt)
{
    std::shared_ptr<std::vector<unsigned char>> content =
        std::make_shared<std::vector<unsigned char>>(4, 'a');

    ccm_Message message = ccm_Message();
    ccm_Message* pDecodedMessage = 0;
    fs_Data encodedData;
    fs_Data decodedData;

    //Generate certificates.
    fsm::GenerateCerts();

    memset(&encodedData, 0, sizeof(encodedData));
    memset(&decodedData, 0, sizeof(decodedData));

    // Setup OID
    strcpy(message.payload.content.oid.oid, g_pDummyOid);

    // Form recipients empty list
    std::vector<fsm::UserId> no_recipients;
    no_recipients.empty();

    message.payload.content.content = content;

    // Test encoding.
    EXPECT_TRUE(ccm_Encode(&encodedData, &message, no_recipients, 0));

    fsm::UserId sender;

    // Check that the CCM decoding issues no error code
    ASSERT_TRUE(ccm_Decode((const_fs_Data*)&encodedData, &pDecodedMessage, sender, nullptr, 0));

    // Check that both the 'decoded' payload length and payload data match the 'original' data.
    ASSERT_EQ(message.payload.content.content->size(),
              pDecodedMessage->payload.content.content->size());

    EXPECT_EQ(*(message.payload.content.content),
              *(pDecodedMessage->payload.content.content));

    fsm::FreeCerts();
    delete pDecodedMessage;
    free(encodedData.data);
}

/**
 * Assert invalid recipitent encoding fail
 *
 * TEST PASSES:
 *      ccm_Encode returns error and Encode data and message remains the same.
 *
 * TEST FAILS:
 *      ccm_Encode returns successfully and/or Encode data and message not the same.
 */
TEST(CcmTest, CCMCodingInvalidRecipients)
{
    std::shared_ptr<std::vector<unsigned char>> content =
        std::make_shared<std::vector<unsigned char>>(4, 'a');

    ccm_Message message = ccm_Message();
    fs_Data encodedData;

    memset(&encodedData, 0, sizeof(encodedData));

    // Form a invalid recipient
    std::vector<fsm::UserId> recipients;
    recipients.push_back(static_cast<fsm::UserId>(fsm::kUndefinedUserId));

    // Setup OID
    strcpy(message.payload.content.oid.oid, g_pDummyOid);

    // Generate certificates
    fsm::GenerateCerts();

    // Add content so that we make sure the failure reason is the invalid user
    message.payload.content.content = content;

    // Create a copy as we will assert the input parameter remains unaltered on failure
    ccm_Message origMessage = message;

    // Encoding is to fail because no certificate.
    EXPECT_FALSE(ccm_Encode(&encodedData, &message, recipients, 0));

    // encoded structure should be un-altered
    fs_Data nullData;
    memset(&nullData, 0, sizeof(nullData));
    EXPECT_EQ(0, memcmp(&nullData, &encodedData, sizeof(nullData)));

    // message structure should be un-altered
    EXPECT_EQ(origMessage, message);

    fsm::FreeCerts();
}

/**
 * CCM Encoding fail due to keystore return error.
 *
 * TEST PASSES:
 *      ccm_Encode returns error and Encode data and message remains the same.
 *
 * TEST FAILS:
 *      ccm_Encode returns successfully and/or Encode data and message not the same.
 */
TEST(CcmTest, CCMCodingKeystoreErrorCase)
{
    std::shared_ptr<std::vector<unsigned char>> content =
        std::make_shared<std::vector<unsigned char>>(4, 'a');

    ccm_Message message = ccm_Message();
    fs_Data encodedData;

    memset(&encodedData, 0, sizeof(encodedData));

    std::vector<fsm::UserId> no_recipients;

    // Setup OID
    strcpy(message.payload.content.oid.oid, g_pDummyOid);

    // Free the certificates to simulate keystore error.
    fsm::FreeCerts();

    // Add content so that we make sure the failure reason is the get certificate failure
    message.payload.content.content = content;

    // Create a copy as we will assert the input parameter remains unaltered on failure
    ccm_Message origMessage = message;

    // Encoding is to fail because no certificate.
    EXPECT_FALSE(ccm_Encode(&encodedData, &message, no_recipients, 0));

    // encoded structure should be un-altered
    fs_Data nullData;
    memset(&nullData, 0, sizeof(nullData));
    EXPECT_EQ(0, memcmp(&nullData, &encodedData, sizeof(nullData)));

    // message structure should be un-altered
    EXPECT_EQ(origMessage, message);
}

/**
 * CCM decoding fail due to keystore return error.
 *
 * TEST PASSES:
 *      ccm_decode returns error and message remains the same.
 *
 * TEST FAILS:
 *      ccm_decode returns successfully and/or Encode data and message not the same.
 */
TEST(CcmTest, CCMDecodingKeystoreErrorCase)
{
    std::shared_ptr<std::vector<unsigned char>> content =
        std::make_shared<std::vector<unsigned char>>(4, 'a');

    std::shared_ptr<fsm::UsermanagerInterface> um_if = fsm::UsermanagerInterface::Create();

    ccm_Message message = ccm_Message();
    ccm_Message* pDecodedMessage = nullptr;
    fs_Data encodedData;
    fs_Data decodedData;

    // Generate certificates
    fsm::GenerateCerts();

    memset(&encodedData, 0, sizeof(encodedData));
    memset(&decodedData, 0, sizeof(decodedData));

    // Form recipient stack
    std::vector<fsm::UserId> recipients;
    um_if->GetUsers(recipients);

    // Setup OID
    strcpy(message.payload.content.oid.oid, g_pDummyOid);

    message.payload.content.content = content;

    ASSERT_TRUE(ccm_Encode(&encodedData, &message, recipients, 0));

    // Free the certificates to simulate keystore error.
    fsm::FreeCerts();

    fsm::UserId sender;

    EXPECT_FALSE(ccm_Decode((const_fs_Data*)&encodedData, &pDecodedMessage, sender, nullptr, 0));

    // message structure should be un-altered
    EXPECT_EQ(nullptr, pDecodedMessage);

    delete pDecodedMessage;
    free(encodedData.data);
}

/**
 * Test ccm_encode and ccm_decode with CCM_DO_NOT_ENCRYPT flag
 *
 * TEST PASSES:
 *      ccm_Encode/ccm_Decode is successful on "Signed-Only" Content
 *
 * TEST FAILS:
 *      ccm_Encode/ccm_Decode is not successful on "Signed-Only" Content
 */
TEST(CcmTest, CCMSubjectKeyIdentifierTest)
{
    std::shared_ptr<std::vector<unsigned char>> content =
        std::make_shared<std::vector<unsigned char>>(4, 'a');

    std::shared_ptr<fsm::UsermanagerInterface> um_if = fsm::UsermanagerInterface::Create();

    ccm_Message message = ccm_Message();
    ccm_Message* pDecodedMessage = 0;
    fs_Data encodedData;
    fs_Data decodedData;

    //Generate certificates.
    fsm::GenerateCerts();

    memset(&encodedData, 0, sizeof(encodedData));
    memset(&decodedData, 0, sizeof(decodedData));

    // Setup OID
    strcpy(message.payload.content.oid.oid, g_pDummyOid);

    // Form recipient stack
    std::vector<fsm::UserId> recipients;
    um_if->GetUsers(recipients);

    message.payload.content.content = content;

    // Test encoding with CCM_DO_NOT_ENCRYPT flag.
    EXPECT_TRUE(ccm_Encode(&encodedData, &message, recipients, CCM_DO_NOT_ENCRYPT));

    fsm::UserId sender;

    // Check that the CCM decoding issues no error code
    ASSERT_TRUE(ccm_Decode((const_fs_Data*)&encodedData, &pDecodedMessage, sender, nullptr, 0));

    // Check that both the 'decoded' payload length and payload data match the 'original' data.
    ASSERT_EQ(message.payload.content.content->size(),
              pDecodedMessage->payload.content.content->size());

    EXPECT_EQ(*(message.payload.content.content),
              *(pDecodedMessage->payload.content.content));

    // Expecting return sender got from stub.
    EXPECT_EQ(sender, fsm::client_user_id);

    fsm::FreeCerts();
    delete pDecodedMessage;
    free(encodedData.data);
}

/**
 * Assert success of Encoding/Decoding an ASN1 DER-encoded INTEGER buffer with CCM
 *
 * TEST PASSES:
 *      ccm_Encode/ccm_Decode is successful on "Signed-and-Encrypted" Content
 *
 * TEST FAILS:
 *      ccm_Encode/ccm_Decode is not successful on "Signed-and-Encrypted" Content
 */
TEST (CcmTest, CCMCodingEncryptAsn1Integer)
{
    const long MAGIC_NUMBER = 1337;
    ccm_Message message = ccm_Message();
    ccm_Message* pDecodedMessage = 0;
    fs_Data encodedData;
    fs_Data decodedData;
    INTEGER_t asnInteger;

    static char derIntegerBuff[1024];

    std::shared_ptr<fsm::UsermanagerInterface> um_if = fsm::UsermanagerInterface::Create();

    memset(&encodedData, 0, sizeof(encodedData));
    memset(&decodedData, 0, sizeof(decodedData));
    memset(derIntegerBuff, 0, sizeof(derIntegerBuff));
    memset(&asnInteger, 0, sizeof(asnInteger));
    asn_ulong2INTEGER(&asnInteger, MAGIC_NUMBER);

    asn_enc_rval_t asnEnc = der_encode_to_buffer(&asn_DEF_INTEGER,
                                                 &asnInteger,
                                                 reinterpret_cast<void*>(derIntegerBuff),
                                                 sizeof(derIntegerBuff));

    // Check DER encoding on ASN1 INTEGER.
    ASSERT_TRUE(asnEnc.encoded != 0);

    //Generate certificates.
    fsm::GenerateCerts();

    // Setup OID
    strcpy(message.payload.content.oid.oid, g_pDummyOid);

    std::shared_ptr<std::vector<unsigned char>> content =
        std::make_shared<std::vector<unsigned char>>();
    message.payload.content.content = content;
    content->assign(derIntegerBuff, derIntegerBuff + asnEnc.encoded);

    // Form recipient stack
    std::vector<fsm::UserId> recipients;
    um_if->GetUsers(recipients);

    EXPECT_TRUE(ccm_Encode(&encodedData, &message, recipients, 0));

    // Form sender
    fsm::UserId sender;

    ASSERT_TRUE(ccm_Decode((const_fs_Data*)&encodedData, &pDecodedMessage, sender, nullptr, 0));

    // number of data bytes must match!
    size_t origByteCount = message.payload.content.content->size();
    size_t decodedByteCount = pDecodedMessage->payload.content.content->size();

    ASSERT_TRUE(origByteCount == decodedByteCount);

    // content must match!
    const void* origRaw = message.payload.content.content->data();
    const void* decodedRaw = pDecodedMessage->payload.content.content->data();

    EXPECT_TRUE(memcmp(origRaw, decodedRaw, origByteCount) == 0);

    INTEGER_t* pAsnInteger = 0;
    long cinteger = 0;

    // Decode the magic number from ASN1 INTEGER
    asn_DEF_INTEGER.ber_decoder(0,
                                &asn_DEF_INTEGER,
                                reinterpret_cast<void**>(&pAsnInteger),
                                decodedRaw,
                                decodedByteCount,
                                0);

    asn_INTEGER2long(pAsnInteger, &cinteger);

    EXPECT_EQ(cinteger, MAGIC_NUMBER);

    asn_DEF_INTEGER.free_struct(&asn_DEF_INTEGER, pAsnInteger, 0);
    pAsnInteger = 0;

    fsm::FreeCerts();
    delete pDecodedMessage;
    free(encodedData.data);
}

/**
 * Verify attached certificates are extracted as expected.
 *
 * TEST PASSES:
 *      ccm_Decode returns attached certificates.
 *
 * TEST FAILS:
 *      ccm_Decode does not return expeted certificates, or returns unexpected certificates.
 */
TEST(CcmTest, CCMCodingAttachedCertificates)
{
    std::shared_ptr<std::vector<unsigned char>> content =
        std::make_shared<std::vector<unsigned char>>(4, 'a');

    ccm_Message message = ccm_Message();
    ccm_Message* pDecodedMessage = nullptr;
    fs_Data encodedData;
    fs_Data decodedData;

    //Generate certificates.
    fsm::GenerateCerts();

    memset(&encodedData, 0, sizeof(encodedData));
    memset(&decodedData, 0, sizeof(decodedData));

    // Setup OID
    strcpy(message.payload.content.oid.oid, g_pDummyOid);

    // Form recipients empty list
    std::vector<fsm::UserId> no_recipients;
    no_recipients.empty();

    message.payload.content.content = content;

    // encode
    ASSERT_TRUE(ccm_Encode(&encodedData, &message, no_recipients, 0));

    fsm::UserId sender;

    // Decode without adding certificates to the CMS structure, expect nothing to be returned.
    STACK_OF(X509)* certificates = nullptr;
    EXPECT_TRUE(ccm_Decode((const_fs_Data*)&encodedData, &pDecodedMessage, sender, &certificates, 0));
    EXPECT_EQ(certificates, nullptr);

    if (certificates)
    {
        sk_X509_pop_free(certificates, X509_free);
        certificates = nullptr;
    }

    delete pDecodedMessage;

    pDecodedMessage = nullptr;


    // Now try to add certs to the CMS before decode and then verify we get them returned as expected.

    // First add certs to CMS

    BIO* data_bio = BIO_new_mem_buf(encodedData.data, encodedData.num_bytes);

    ASSERT_NE(data_bio, nullptr);

    CMS_ContentInfo* signed_data = d2i_CMS_bio(data_bio, nullptr);

    ASSERT_NE(signed_data, nullptr);

    std::stack<std::pair<X509*, EVP_PKEY*>> certs;

    int kNumCerts = 10;

    for (int i = 0; i < kNumCerts; i++)
    {
        X509* cert = nullptr;
        EVP_PKEY* key = nullptr;
        GeneratePrivateKey(&key, fsm::kMinRSAKeySize);
        ASSERT_NE(key, nullptr);
        GenerateCertificate(&cert, key);
        ASSERT_NE(cert, nullptr);

        certs.push(std::make_pair(cert, key));

        ASSERT_EQ(CMS_add1_cert(signed_data, cert), 1);
    }

    fs_Data encoded_data_with_certs = {0};
    BIO* signed_bio = BIO_new(BIO_s_mem());

    ASSERT_GT(i2d_CMS_bio(signed_bio, signed_data), 0);
    BIO_set_close(signed_bio, BIO_NOCLOSE);
    encoded_data_with_certs.num_bytes = BIO_get_mem_data(signed_bio, &(encoded_data_with_certs.data));
    ASSERT_GT(encoded_data_with_certs.num_bytes, 0);
    ASSERT_NE(encoded_data_with_certs.data, nullptr);

    CMS_ContentInfo_free(signed_data);
    BIO_free(data_bio);
    BIO_free(signed_bio);

    // finally, now decode
    EXPECT_TRUE(ccm_Decode((const_fs_Data*)&encoded_data_with_certs, &pDecodedMessage, sender, &certificates, 0));
    ASSERT_NE(certificates, nullptr);

    int num_certs = sk_X509_num(certificates);

    EXPECT_EQ(num_certs, kNumCerts);

    for (int i = 0; i < kNumCerts; i++)
    {
        std::pair<X509*, EVP_PKEY*> cert_key_pair = certs.top();

        bool found = false;

        for (int k = 0; k < num_certs; k++)
        {
            found = X509_cmp(cert_key_pair.first, sk_X509_value(certificates, k)) == 0;

            if (found)
            {
                break;
            }
        }

        EXPECT_TRUE(found);

        X509_free(cert_key_pair.first);
        EVP_PKEY_free(cert_key_pair.second);

        certs.pop();
    }

    sk_X509_pop_free(certificates, X509_free);
    certificates = nullptr;

    fsm::FreeCerts();
    delete pDecodedMessage;
    free(encodedData.data);
    free(encoded_data_with_certs.data);
}

/**
 * \brief Test that allocated memory for address URI is freed
 */
TEST(CcmTest, CCMFreeAddressURI)
{
    const char uri[9] = "test/uri";
    ccm_Message *message = new ccm_Message();
    message->header.sender.type = ADDRESS_TYPE_URI;
    message->header.sender.address.uri = new char[9];
    memcpy(message->header.sender.address.uri, uri, 9);
    ASSERT_STREQ(message->header.sender.address.uri, uri);
    for (int i = 0; i < CCM_MAX_NUM_RECIPIENTS; i++)
    {
        message->header.recipients[i].type = ADDRESS_TYPE_URI;
        message->header.recipients[i].address.uri = new char[9];
        memcpy(message->header.recipients[i].address.uri, uri, 9);
        ASSERT_STREQ(message->header.recipients[i].address.uri, uri);
    }

    delete message;

    EXPECT_EQ(malloc_usable_size(message->header.sender.address.uri), 0);
    EXPECT_EQ(message->header.sender.address.uri, nullptr);
    for (int i = 0; i < CCM_MAX_NUM_RECIPIENTS; i++)
    {
        EXPECT_EQ(malloc_usable_size(message->header.recipients[i].address.uri), 0);
        EXPECT_EQ(message->header.recipients[i].address.uri, nullptr);
    }
}

/** \}    end of addtogroup */
