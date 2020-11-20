/**
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
 *  \file     unittest_voc_frames.cc
 *  \brief    Volvo On Call, ccm signals, Test Cases
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup TestVolvoOnCall
 *  \{
 */

#include "test_classes.h"

#include <gtest/gtest.h>

#include <openssl/evp.h>

#include "signals/aes_ble_da_key_voc_frame.h"
#include "signals/aes_cert_actor_voc_frame.h"
#include "signals/aes_cert_nearfield_voc_frame.h"
#include "signals/aes_ca_voc_frame.h"
#include "signals/aes_car_actor_cert_voc_frame.h"
#include "signals/aes_csr_actor_voc_frame.h"
#include "signals/aes_csr_nearfield_voc_frame.h"
#include "signals/ecdhe_approval_voc_frame.h"
#include "signals/ecdhe_request_voc_frame.h"
#include "signals/voc_frame_codec.h"

namespace voc_voc_frame_test
{

using namespace volvo_on_call;
using namespace voc_test_classes;

static void DumpFrameAndKeys(std::string name,
                             std::vector<unsigned char> frame,
                             EVP_PKEY* tcam_key,
                             EVP_PKEY* peer_key,
                             std::vector<unsigned char> secret)
{
    DumpMessageFile(name + ".bin", frame);
    DumpPubKey(name + "_tcam_key.pem", tcam_key);
    DumpPubKey(name + "_peer_key.pem", tcam_key);
    DumpMessageFile(name + ".shared_secret", secret);
}

/**
 * Compares the public key part of two EVP_PKEY for equality
 */
bool ArePublicKeysEqual(EVP_PKEY* a, EVP_PKEY* b)
{
    if (!a || !b)
    {
        return false;
    }

    if (a == b)
    {
        return true;
    }

    // extract public key element from both EVP_PKEYS and compare
    BIO* a_bio = BIO_new(BIO_s_mem());
    BIO* b_bio = BIO_new(BIO_s_mem());

    if (!PEM_write_bio_PUBKEY(a_bio, a))
    {
        return false;
    }
    if (!PEM_write_bio_PUBKEY(b_bio, b))
    {
        return false;
    }

    char* a_buf = nullptr;
    char* b_buf = nullptr;

    long a_buf_size = BIO_get_mem_data(a_bio, &a_buf);
    long b_buf_size = BIO_get_mem_data(b_bio, &b_buf);

    if (a_buf_size != b_buf_size)
    {
        return false;
    }
    if (memcmp(a_buf, b_buf, a_buf_size) == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * \brief Test encryption / decryption of AesBleDaKeyVocFrame and very payload
 *        is intact after decrypt.
 */
TEST (VocFrameTest, AesBleDaKeyVocFrame)
{
    std::vector<unsigned char> data;

    // setup the frame
    AesBleDaKeyVocFrame::BleDaKey key =
        {'b', 'l', 'e', 'd', 'a', 't', 'a', 'a', 'c', 'c', 'e', 's', 's', 'k', 'e', 'y'};
    AesBleDaKeyVocFrame frame(key);

    // setup the codec
    VocFrameCodec codec;

    // frame type is encrypted, set up keys
    EVP_PKEY* tcam_key = nullptr;
    EVP_PKEY* peer_key = nullptr;
    std::vector<unsigned char> secret;
    ASSERT_TRUE(MakeKeysAndSecret(&tcam_key, &peer_key, secret));
    ASSERT_TRUE(codec.SetEncryptionKeys(tcam_key, peer_key, secret));

    // encode frame, expect encode to pass
    ASSERT_TRUE(codec.Encode(data, frame));

    // dump files to disk
    DumpFrameAndKeys("aes_ble_da_key_voc_frame",
                     data,
                     tcam_key,
                     peer_key,
                     secret);

    // try to decode the frame, expect to get a valid pointer back
    std::shared_ptr<fsm::Signal> decoded_frame = codec.Decode(data);

    if (!decoded_frame)
    {
        FAIL();
    }

    // verify that returned Signal is of correct type
    ASSERT_EQ(typeid(*decoded_frame).hash_code(), typeid(AesBleDaKeyVocFrame).hash_code());
    ASSERT_EQ(decoded_frame->GetSignalType(), VocSignalTypes::kAesBleDaKeyVocFrame);

    std::shared_ptr<AesBleDaKeyVocFrame> typed_decoded_frame =
        std::static_pointer_cast<AesBleDaKeyVocFrame>(decoded_frame);

    // verify that BDAK passed through encode/decode intact
    AesBleDaKeyVocFrame::BleDaKey decoded_key = typed_decoded_frame->GetKey();
    EXPECT_TRUE(decoded_key == key);

    EVP_PKEY_free(tcam_key);
    EVP_PKEY_free(peer_key);
}

/**
 * \brief Test various invalid encode and decode operations and verify that they fail.
 */
TEST (VocFrameTest, VocFrameCodecTest)
{
    std::vector<unsigned char> data;
    std::vector<unsigned char> empty_data;
    std::vector<unsigned char> bad_identifier = {0XAB, 0X00, 0X00, 0X01}; //not a VOC Frame
    std::vector<unsigned char> bad_header = {0XFE, 0X00, 0X00}; //to small to hold header
    std::vector<unsigned char> bad_frame_type = {0xFE, 0x00, 0x00, 0xCC}; //not a known frame type
    std::vector<unsigned char> bad_payload_length =
        {0xFE, 0x00, 0x30, 0x0A, 0x01, 0x02, 0x03, 0x04}; //less data then header claims

    AesBleDaKeyVocFrame::BleDaKey key =
        {'b', 'l', 'e', 'd', 'a', 't', 'a', 'a', 'c', 'c', 'e', 's', 's', 'k', 'e', 'y'};

    VocFrameCodec codec;
    AesBleDaKeyVocFrame frame(key);

    EVP_PKEY* tcam_key = nullptr;
    EVP_PKEY* peer_key = nullptr;

    std::vector<unsigned char> secret;
    std::vector<unsigned char> empty_secret;

    ASSERT_TRUE(MakeKeysAndSecret(&tcam_key, &peer_key, secret));

    // Try to set various combinations of bad keys and encode an encrypted Frame, expected to fail.
    EXPECT_FALSE(codec.Encode(data, frame));
    data.clear(); //buffer may be filled with garbage

    ASSERT_FALSE(codec.SetEncryptionKeys(nullptr, nullptr, empty_secret));
    EXPECT_FALSE(codec.Encode(data, frame));
    data.clear(); //buffer may be filled with garbage

    ASSERT_FALSE(codec.SetEncryptionKeys(nullptr, peer_key, secret));
    EXPECT_FALSE(codec.Encode(data, frame));
    data.clear(); //buffer may be filled with garbage

    ASSERT_FALSE(codec.SetEncryptionKeys(tcam_key, nullptr, secret));
    EXPECT_FALSE(codec.Encode(data, frame));
    data.clear(); //buffer may be filled with garbage

    ASSERT_FALSE(codec.SetEncryptionKeys(tcam_key, peer_key, empty_secret));
    EXPECT_FALSE(codec.Encode(data, frame));
    data.clear(); //buffer may be filled with garbage

    // Attempt to decode bad buffers, expected to return nullptr
    std::shared_ptr<fsm::Signal> decoded_frame;

    decoded_frame = codec.Decode(bad_identifier);
    EXPECT_EQ(decoded_frame, nullptr);

    decoded_frame = codec.Decode(bad_header);
    EXPECT_EQ(decoded_frame, nullptr);

    decoded_frame = codec.Decode(bad_identifier);
    EXPECT_EQ(decoded_frame, nullptr);

    decoded_frame = codec.Decode(bad_frame_type);
    EXPECT_EQ(decoded_frame, nullptr);

    decoded_frame = codec.Decode(bad_payload_length);
    EXPECT_EQ(decoded_frame, nullptr);

    // Attempt to decode a valid encrypted voc frame with a VocFrameCodec which does
    // not have a valid set of keys, expecetd to fail.
    ASSERT_TRUE(codec.SetEncryptionKeys(tcam_key, peer_key, secret));
    ASSERT_TRUE(codec.Encode(data, frame));

    VocFrameCodec bad_codec;

    decoded_frame = bad_codec.Decode(data);
    EXPECT_EQ(decoded_frame, nullptr);

    // Set a incorrect set of keys in the codec and attempt to decode

    EVP_PKEY_free(tcam_key);
    EVP_PKEY_free(peer_key);
    tcam_key = nullptr;
    peer_key = nullptr;
    secret.clear();

    ASSERT_TRUE(MakeKeysAndSecret(&tcam_key, &peer_key, secret));
    ASSERT_TRUE(bad_codec.SetEncryptionKeys(tcam_key, peer_key, secret));

    decoded_frame = bad_codec.Decode(data);
    EXPECT_EQ(decoded_frame, nullptr);

    EVP_PKEY_free(tcam_key);
    EVP_PKEY_free(peer_key);
}

/**
 * Test encode / decode cycle for EcdheApprovalVocFrame.
 * As there is no paylaod we can not test it passes through
 * intact.
 */
TEST (VocFrameTest, EcdheApprovalVocFrame)
{
    std::vector<unsigned char> data;

    // setup codec and frame
    VocFrameCodec codec;
    EcdheApprovalVocFrame frame;

    // try encode, expect it to pass
    ASSERT_TRUE(codec.Encode(data, frame));

    // dump frame to disk
    DumpMessageFile("ecdhe_approval_voc_frame.bin", data);

    // try decode, expect a valid pointer back
    std::shared_ptr<fsm::Signal> decoded_frame = codec.Decode(data);

    if (!decoded_frame)
    {
        FAIL();
    }

    // verify returned Singal is of correct type
    ASSERT_EQ(typeid(*decoded_frame).hash_code(), typeid(EcdheApprovalVocFrame).hash_code());
    ASSERT_EQ(decoded_frame->GetSignalType(), VocSignalTypes::kEcdheApprovalVocFrame);
}

/**
 * Test encode / decode cycle for AesCsrActorVocFrame.
 */
TEST (VocFrameTest, AesCsrActorVocFrame)
{
    std::vector<unsigned char> data;

    // setup frames
    X509_REQ* request = fsm::ut_GetCsrFromPEM(fsm::CLIENT_TWO_CSR_PEM.c_str());

    AesCsrActorVocFrame frame(request);
    AesCsrActorVocFrame bad_frame(nullptr);

    // setup codec
    VocFrameCodec codec;

    EVP_PKEY* tcam_key = nullptr;
    EVP_PKEY* peer_key = nullptr;

    std::vector<unsigned char> secret;

    ASSERT_TRUE(MakeKeysAndSecret(&tcam_key, &peer_key, secret));
    ASSERT_TRUE(codec.SetEncryptionKeys(tcam_key, peer_key, secret));

    // try encode of bad frame, expected to fail
    ASSERT_FALSE(codec.Encode(data, bad_frame));
    data.clear();

    // try encode of good frame, expect it to pass
    ASSERT_TRUE(codec.Encode(data, frame));

    // dump files to disk
    DumpFrameAndKeys("aes_csr_actor_voc_frame",
                     data,
                     tcam_key,
                     peer_key,
                     secret);

    // try decode, expect a valid pointer back
    std::shared_ptr<fsm::Signal> decoded_frame = codec.Decode(data);

    if (!decoded_frame)
    {
        FAIL();
    }

    // verify returned Singal is of correct type
    ASSERT_EQ(typeid(*decoded_frame).hash_code(), typeid(AesCsrActorVocFrame).hash_code());
    ASSERT_EQ(decoded_frame->GetSignalType(), VocSignalTypes::kAesCsrActorVocFrame);

    std::shared_ptr<AesCsrActorVocFrame> typed_decoded_frame =
        std::static_pointer_cast<AesCsrActorVocFrame>(decoded_frame);

    X509_REQ* decodec_request = typed_decoded_frame->GetRequest();

    if (!decodec_request)
    {
        FAIL();
    }

    EXPECT_TRUE(fsm::X509_REQ_eq(decodec_request, request));
}

/**
 * Test encode / decode cycle for AesCsrNearfieldVocFrame.
 */
TEST (VocFrameTest, AesCsrNearfieldVocFrame)
{
    std::vector<unsigned char> data;

    // setup frames
    X509_REQ* request = fsm::ut_GetCsrFromPEM(fsm::CLIENT_TWO_CSR_PEM.c_str());

    AesCsrNearfieldVocFrame frame(request);
    AesCsrNearfieldVocFrame bad_frame(nullptr);

    // setup codec
    VocFrameCodec codec;

    EVP_PKEY* tcam_key = nullptr;
    EVP_PKEY* peer_key = nullptr;

    std::vector<unsigned char> secret;

    ASSERT_TRUE(MakeKeysAndSecret(&tcam_key, &peer_key, secret));
    ASSERT_TRUE(codec.SetEncryptionKeys(tcam_key, peer_key, secret));

    // try encode of bad frame, expected to fail
    ASSERT_FALSE(codec.Encode(data, bad_frame));
    data.clear();

    // try encode of good frame, expect it to pass
    ASSERT_TRUE(codec.Encode(data, frame));

    // dump files to disk
    DumpFrameAndKeys("aes_csr_nearfield_voc_frame",
                     data,
                     tcam_key,
                     peer_key,
                     secret);

    // try decode, expect a valid pointer back
    std::shared_ptr<fsm::Signal> decoded_frame = codec.Decode(data);

    if (!decoded_frame)
    {
        FAIL();
    }

    // verify returned Singal is of correct type
    ASSERT_EQ(typeid(*decoded_frame).hash_code(), typeid(AesCsrNearfieldVocFrame).hash_code());
    ASSERT_EQ(decoded_frame->GetSignalType(), VocSignalTypes::kAesCsrNearfieldVocFrame);

    std::shared_ptr<AesCsrNearfieldVocFrame> typed_decoded_frame =
        std::static_pointer_cast<AesCsrNearfieldVocFrame>(decoded_frame);

    X509_REQ* decodec_request = typed_decoded_frame->GetRequest();

    if (!decodec_request)
    {
        FAIL();
    }

    EXPECT_TRUE(fsm::X509_REQ_eq(decodec_request, request));
}

/**
 * Test encode / decode cycle for AesCertActorVocFrame.
 */
TEST (VocFrameTest, AesCertActorVocFrame)
{
    std::vector<unsigned char> data;

    // setup frames
    X509* certificate = fsm::ut_GetCertFromPEM(fsm::CLIENT_TWO_CERT_PEM.c_str());

    AesCertActorVocFrame frame(certificate);
    AesCertActorVocFrame bad_frame(nullptr);

    // setup codec
    VocFrameCodec codec;

    EVP_PKEY* tcam_key = nullptr;
    EVP_PKEY* peer_key = nullptr;

    std::vector<unsigned char> secret;

    ASSERT_TRUE(MakeKeysAndSecret(&tcam_key, &peer_key, secret));
    ASSERT_TRUE(codec.SetEncryptionKeys(tcam_key, peer_key, secret));

    // try encode of bad frame, expected to fail
    ASSERT_FALSE(codec.Encode(data, bad_frame));
    data.clear();

    // try encode of good frame, expect it to pass
    ASSERT_TRUE(codec.Encode(data, frame));

    // dump files to disk
    DumpFrameAndKeys("aes_cert_actor_voc_frame",
                     data,
                     tcam_key,
                     peer_key,
                     secret);

    // try decode, expect a valid pointer back
    std::shared_ptr<fsm::Signal> decoded_frame = codec.Decode(data);

    if (!decoded_frame)
    {
        FAIL();
    }

    // verify returned Singal is of correct type
    ASSERT_EQ(typeid(*decoded_frame).hash_code(), typeid(AesCertActorVocFrame).hash_code());
    ASSERT_EQ(decoded_frame->GetSignalType(), VocSignalTypes::kAesCertActorVocFrame);

    std::shared_ptr<AesCertActorVocFrame> typed_decoded_frame =
        std::static_pointer_cast<AesCertActorVocFrame>(decoded_frame);

    X509* decodec_certificate = typed_decoded_frame->GetCertificate();

    if (!decodec_certificate)
    {
        FAIL();
    }

    EXPECT_EQ(X509_cmp(decodec_certificate, certificate), 0);
}

/**
 * Test encode / decode cycle for AesCertNearfieldVocFrame.
 */
TEST (VocFrameTest, AesCertNearfieldVocFrame)
{
    std::vector<unsigned char> data;

    // setup frames
    X509* certificate = fsm::ut_GetCertFromPEM(fsm::CLIENT_TWO_CERT_PEM.c_str());

    AesCertNearfieldVocFrame frame(certificate);
    AesCertNearfieldVocFrame bad_frame(nullptr);

    // setup codec
    VocFrameCodec codec;

    EVP_PKEY* tcam_key = nullptr;
    EVP_PKEY* peer_key = nullptr;

    std::vector<unsigned char> secret;

    ASSERT_TRUE(MakeKeysAndSecret(&tcam_key, &peer_key, secret));
    ASSERT_TRUE(codec.SetEncryptionKeys(tcam_key, peer_key, secret));

    // try encode of bad frame, expected to fail
    ASSERT_FALSE(codec.Encode(data, bad_frame));
    data.clear();

    // try encode of good frame, expect it to pass
    ASSERT_TRUE(codec.Encode(data, frame));

    // dump files to disk
    DumpFrameAndKeys("aes_cert_nearfield_voc_frame",
                     data,
                     tcam_key,
                     peer_key,
                     secret);

    // try decode, expect a valid pointer back
    std::shared_ptr<fsm::Signal> decoded_frame = codec.Decode(data);

    if (!decoded_frame)
    {
        FAIL();
    }

    // verify returned Singal is of correct type
    ASSERT_EQ(typeid(*decoded_frame).hash_code(), typeid(AesCertNearfieldVocFrame).hash_code());
    ASSERT_EQ(decoded_frame->GetSignalType(), VocSignalTypes::kAesCertNearfieldVocFrame);

    std::shared_ptr<AesCertNearfieldVocFrame> typed_decoded_frame =
        std::static_pointer_cast<AesCertNearfieldVocFrame>(decoded_frame);

    X509* decodec_certificate = typed_decoded_frame->GetCertificate();

    if (!decodec_certificate)
    {
        FAIL();
    }

    EXPECT_EQ(X509_cmp(decodec_certificate, certificate), 0);
}

/**
 * Test encode / decode cycle for AesCaVocFrame.
 */
TEST (VocFrameTest, AesCaVocFrame)
{
    std::vector<unsigned char> data;

    // setup frames
    X509* certificate = fsm::ut_GetCertFromPEM(fsm::CLIENT_TWO_CERT_PEM.c_str());

    AesCaVocFrame frame(certificate);
    AesCaVocFrame bad_frame(nullptr);

    // setup codec
    VocFrameCodec codec;

    EVP_PKEY* tcam_key = nullptr;
    EVP_PKEY* peer_key = nullptr;

    std::vector<unsigned char> secret;

    ASSERT_TRUE(MakeKeysAndSecret(&tcam_key, &peer_key, secret));
    ASSERT_TRUE(codec.SetEncryptionKeys(tcam_key, peer_key, secret));

    // try encode of bad frame, expected to fail
    ASSERT_FALSE(codec.Encode(data, bad_frame));
    data.clear();

    // try encode of good frame, expect it to pass
    ASSERT_TRUE(codec.Encode(data, frame));

    // dump files to disk
    DumpFrameAndKeys("aes_ca_voc_frame",
                     data,
                     tcam_key,
                     peer_key,
                     secret);

    // try decode, expect a valid pointer back
    std::shared_ptr<fsm::Signal> decoded_frame = codec.Decode(data);

    if (!decoded_frame)
    {
        FAIL();
    }

    // verify returned Singal is of correct type
    ASSERT_EQ(typeid(*decoded_frame).hash_code(), typeid(AesCaVocFrame).hash_code());
    ASSERT_EQ(decoded_frame->GetSignalType(), VocSignalTypes::kAesCaVocFrame);

    std::shared_ptr<AesCaVocFrame> typed_decoded_frame =
        std::static_pointer_cast<AesCaVocFrame>(decoded_frame);

    X509* decodec_certificate = typed_decoded_frame->GetCertificate();

    if (!decodec_certificate)
    {
        FAIL();
    }

    EXPECT_EQ(X509_cmp(decodec_certificate, certificate), 0);
}

/**
 * Test encode / decode cycle for AesCarActorCertVocFrame.
 */
TEST (VocFrameTest, AesCarActorCertVocFrame)
{
    std::vector<unsigned char> data;

    // setup frames
    X509* certificate = fsm::ut_GetCertFromPEM(fsm::CLIENT_TWO_CERT_PEM.c_str());

    AesCarActorCertVocFrame frame(certificate);
    AesCarActorCertVocFrame bad_frame(nullptr);

    // setup codec
    VocFrameCodec codec;

    EVP_PKEY* tcam_key = nullptr;
    EVP_PKEY* peer_key = nullptr;

    std::vector<unsigned char> secret;

    ASSERT_TRUE(MakeKeysAndSecret(&tcam_key, &peer_key, secret));
    ASSERT_TRUE(codec.SetEncryptionKeys(tcam_key, peer_key, secret));

    // try encode of bad frame, expected to fail
    ASSERT_FALSE(codec.Encode(data, bad_frame));
    data.clear();

    // try encode of good frame, expect it to pass
    ASSERT_TRUE(codec.Encode(data, frame));

    // dump files to disk
    DumpFrameAndKeys("aes_car_actor_cert_voc_frame",
                     data,
                     tcam_key,
                     peer_key,
                     secret);

    // try decode, expect a valid pointer back
    std::shared_ptr<fsm::Signal> decoded_frame = codec.Decode(data);

    if (!decoded_frame)
    {
        FAIL();
    }

    // verify returned Singal is of correct type
    ASSERT_EQ(typeid(*decoded_frame).hash_code(), typeid(AesCarActorCertVocFrame).hash_code());
    ASSERT_EQ(decoded_frame->GetSignalType(), VocSignalTypes::kAesCarActorCertVocFrame);

    std::shared_ptr<AesCarActorCertVocFrame> typed_decoded_frame =
        std::static_pointer_cast<AesCarActorCertVocFrame>(decoded_frame);

    X509* decodec_certificate = typed_decoded_frame->GetCertificate();

    if (!decodec_certificate)
    {
        FAIL();
    }

    EXPECT_EQ(X509_cmp(decodec_certificate, certificate), 0);
}

/**
 * \brief Test encryption / decryption of EcdheRequestVocFrame.
 */
TEST (VocFrameTest, EcdheRequestVocFrame)
{
    {
        // TEST the case when the key is null
        std::vector<unsigned char> data;
        EcdheRequestVocFrame frame(nullptr);
        VocFrameCodec codec;

        // expect Get to give nullptr
        ASSERT_EQ(nullptr, frame.GetKey());

        // encode frame, expect encode to fail
        ASSERT_FALSE(codec.Encode(data, frame));
    }

    {
        std::vector<unsigned char> data;

        // setup the codec
        VocFrameCodec codec;

        // set up keys
        EVP_PKEY* tcam_key = nullptr;
        EVP_PKEY* peer_key = nullptr;
        std::vector<unsigned char> secret;
        ASSERT_TRUE(MakeKeysAndSecret(&tcam_key, &peer_key, secret));

        EcdheRequestVocFrame frame_test(tcam_key);

        // verify GetKey returns correct key
        EVP_PKEY* tmp_key = frame_test.GetKey();
        ASSERT_TRUE(ArePublicKeysEqual(tcam_key, tmp_key));

        // encode frame, expect encode to pass
        ASSERT_TRUE(codec.Encode(data, frame_test));

        // dump files to disk
        DumpMessageFile("ecdhe_request_voc_frame.bin", data);

        // try to decode the frame, expect to get a valid pointer back
        std::shared_ptr<fsm::Signal> decoded_frame = codec.Decode(data);

        if (!decoded_frame)
        {
            FAIL();
        }

        // verify that returned Signal is of correct type
        ASSERT_EQ(typeid(*decoded_frame).hash_code(), typeid(EcdheRequestVocFrame).hash_code());
        ASSERT_EQ(decoded_frame->GetSignalType(), VocSignalTypes::kEcdheRequestVocFrame);

        std::shared_ptr<EcdheRequestVocFrame> typed_decoded_frame =
            std::static_pointer_cast<EcdheRequestVocFrame>(decoded_frame);

        // verify that Key passed through encode/decode intact
        EVP_PKEY* decoded_key = typed_decoded_frame->GetKey();
        ASSERT_TRUE(ArePublicKeysEqual(tcam_key, decoded_key));

        // and we are done, clean up
        EVP_PKEY_free(decoded_key);
        EVP_PKEY_free(tcam_key);
        EVP_PKEY_free(peer_key);
    }
}

} // namespace

/** \}    end of addtogroup */
