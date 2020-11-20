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
 *  \file     unittest_transfermanager.cpp
 *  \brief    Foundation Services Connectivity Compact Message, Test Cases
 *  \author   Iulian Sirghi
 *
 *  \addtogroup unittest
 *  \{
 */

/******************************************************************************
 * INCLUDE FILES
 ******************************************************************************/
#include "transfer_response.h"

#include <fsm/transfermanager/transfer_manager.h>
#include <fsm/transfermanager/request.h>

#include<dlt/dlt.h>

#include <gtest/gtest.h>

#include <map>
#include <fstream>
#include <sstream>

#include <cstdlib>
#include <ctime>

DLT_DECLARE_CONTEXT(dlt_fssdd);

namespace fsm
{

namespace transfermanager
{


/******************************************************************************
 * DUMMY CLASSES
 ******************************************************************************/

class DummyMessage : public Message
{
public:
    DummyMessage()
        : Message()
    {

    }

    ErrorCode GetBody(std::string& message_body) const override
    {
        return ErrorCode::kNotImplemented;
    }
};

/******************************************************************************
 * TESTS
 ******************************************************************************/

TEST(Message, Constructor)
{
    DummyMessage message;

    // all header fields should be initially empty.
    {
        std::string header;
        std::string empty;

        // success is expected.
        EXPECT_EQ(ErrorCode::kNone, message.GetHeader(header));

        // header should remain un-modified.
        EXPECT_EQ(empty, header);
    }

    // getting header field values for keys that were not added must report kBadKey.
    {
        std::vector<std::string> userAgentValues;
        std::vector<std::string> empty;

        // expect kBadKey as "User-Agent" was never added.
        ASSERT_EQ(ErrorCode::kBadKey, message.GetHeaderFieldValues(Message::FieldKey::kUserAgent, userAgentValues));

        // userAgentValues is still empty.
        EXPECT_EQ(empty, userAgentValues);
    }

    // removing header fields that were never added must report kBadKey.
    {
        // expect key does not already exist.
        ASSERT_EQ(ErrorCode::kBadKey, message.RemoveHeaderField(Message::FieldKey::kUserAgent));
    }
}

TEST(Message, HeaderFieldNames)
{
    // Make sure our identifiers are correctly tranformed to the string representation.
    // Create the expected mapping.
    std::map<Message::FieldKey, std::string> expected_name_mapping = {
        std::make_pair(Message::FieldKey::kHost, "Host"),
        std::make_pair(Message::FieldKey::kAccept, "Accept"),
        std::make_pair(Message::FieldKey::kAcceptEncoding, "Accept-Encoding"),
        std::make_pair(Message::FieldKey::kAcceptLanguage, "Accept-Language"),
        std::make_pair(Message::FieldKey::kUserAgent, "User-Agent"),
        std::make_pair(Message::FieldKey::kContentType, "Content-Type"),
        std::make_pair(Message::FieldKey::kContentLength, "Content-Length"),
        std::make_pair(Message::FieldKey::kContentEncoding, "Content-Encoding"),
        std::make_pair(Message::FieldKey::kXCcSource, "X-Cc-Source"),
        std::make_pair(Message::FieldKey::kID, "ID"),
        std::make_pair(Message::FieldKey::kETag, "ETag"),
        std::make_pair(Message::FieldKey::kDate, "Date"),
        std::make_pair(Message::FieldKey::kStrictTransportSecurity, "Strict-Transport-Security"),
        std::make_pair(Message::FieldKey::kLocation, "Location"),
        std::make_pair(Message::FieldKey::kCacheControl, "Cache-Control"),
    };

    // iterate through all identifiers check the mapping.
    std::uint32_t key_id = static_cast<std::uint32_t>(Message::FieldKey::kUndefined);
    const std::uint32_t kKeyIdMax = static_cast<std::uint32_t>(Message::FieldKey::kMax);

    for (++key_id; key_id < kKeyIdMax; ++key_id)
    {
        DummyMessage message;

        std::string name_representation; // key as string.

        ASSERT_EQ(ErrorCode::kNone, message.AddHeaderField(Message::FieldKey(key_id), ""));
        ASSERT_EQ(ErrorCode::kNone, message.GetHeader(name_representation));

        // Remove column.
        const std::string::size_type column_pos = name_representation.find_first_of(':');
        name_representation = name_representation.substr(0, column_pos);

        // compare against expected.
        const std::string& expected_name = expected_name_mapping[static_cast<Message::FieldKey>(key_id)];

        ASSERT_EQ(expected_name, name_representation);
    }
}

TEST(Message, HeaderFieldOperations)
{
    // Single value per header-key test case.
    {
        DummyMessage message;
        std::vector<std::string> reported_values;
        const std::string kHeaderValue = "gzip";
        const std::string kExpectedHeader = "Content-Encoding: " + kHeaderValue + "\r\n";

        // Add a "Content-Encoding" field-key
        ASSERT_EQ(ErrorCode::kNone, message.AddHeaderField(Message::FieldKey::kContentEncoding, kHeaderValue));

        // now retrieve it back.
        ASSERT_EQ(ErrorCode::kNone, message.GetHeaderFieldValues(Message::FieldKey::kContentEncoding, reported_values));

        // We've only added one value.
        ASSERT_EQ(1, reported_values.size());

        // Retrieve that value and check is it indeed the one we added.
        ASSERT_EQ(kHeaderValue, reported_values[0]);

        // will also check if the header is computed as expected.
        std::string actual_header;

        ASSERT_EQ(ErrorCode::kNone, message.GetHeader(actual_header));
        ASSERT_EQ(actual_header, kExpectedHeader);

        // will remove the header-key and check if the full header is empty.
        std::string empty_header;

        ASSERT_EQ(ErrorCode::kNone, message.RemoveHeaderField(Message::FieldKey::kContentEncoding));
        ASSERT_EQ(ErrorCode::kNone, message.GetHeader(empty_header));

        ASSERT_EQ(empty_header, std::string());
    }

    // Multiple values per header-key test case.
    {
        static const std::string kExpectedHeader = "Accept-Encoding: gzip,deflate\r\n";
        DummyMessage message;

        // add gzip.
        ASSERT_EQ(ErrorCode::kNone, message.AddHeaderField(Message::FieldKey::kAcceptEncoding, "gzip"));
        // add deflate
        ASSERT_EQ(ErrorCode::kNone, message.AddHeaderField(Message::FieldKey::kAcceptEncoding, "deflate"));

        std::vector<std::string> reported_values;
        ASSERT_EQ(ErrorCode::kNone, message.GetHeaderFieldValues(Message::FieldKey::kAcceptEncoding, reported_values));
        ASSERT_EQ(2, reported_values.size());

        // will also check if the header is computed as expected.
        std::string actual_header;

        ASSERT_EQ(ErrorCode::kNone, message.GetHeader(actual_header));
        ASSERT_EQ(actual_header, kExpectedHeader);

        // will remove the header-key and check if the full header is empty.
        std::string empty_header;

        ASSERT_EQ(ErrorCode::kNone, message.RemoveHeaderField(Message::FieldKey::kAcceptEncoding));
        ASSERT_EQ(ErrorCode::kNone, message.GetHeader(empty_header));

        ASSERT_EQ(empty_header, std::string());
    }
}

TEST(Response, ResponseStatusCode)
{
    TransferResponse response;

    std::array<std::string, 3> parts = {"HTTP/1.1 ", "200 ", "OK\r\n"};
    std::array<std::string, 3>::const_iterator it = parts.begin();


    response.SetState(Response::State::kRequestQueued);

    for (; it != parts.end(); ++it)
    {
        std::stringstream stream(*it);
        ASSERT_EQ(ErrorCode::kNone, response.Update(stream));
    }

    Response::StatusCode status_code;

    ASSERT_EQ(ErrorCode::kNone, response.GetStatusCode(status_code));
    ASSERT_EQ(Response::StatusCode::kOk, status_code);
}

TEST(Response, ResponseParsing)
{
    TransferResponse response;

    const std::string kFullResponse = "HTTP/1.1 200 OK\r\n"
                                    "Server: Apache\r\n"
                                    "Content-Length: 3\r\n"
                                    "Content-Type: text/plain\r\n"
                                    "\r\n"
                                    "HI!";

    response.SetState(Response::State::kRequestQueued);

    std::srand(std::time(NULL));
    const std::uint32_t kMinSize = 1;
    const std::uint32_t kMaxSize = 5;

    std::stringstream full_response(kFullResponse);
    char buffer[kMaxSize];

    std::streamsize nr_read = 0;

    do
    {
        std::stringstream stream;

        nr_read = full_response.readsome(buffer, rand() % kMaxSize + kMinSize);
        stream.write(buffer, nr_read);

        ASSERT_EQ(ErrorCode::kNone, response.Update(stream));

        stream.str(std::string());
    } while(nr_read);

    Response::StatusCode status_code;
    std::vector<std::string> content_length;
    std::vector<std::string> content_type;

    ASSERT_EQ(ErrorCode::kNone, response.GetStatusCode(status_code));
    ASSERT_EQ(status_code, Response::StatusCode::kOk);

    ASSERT_EQ(ErrorCode::kNone, response.GetHeaderFieldValues(Message::FieldKey::kContentLength, content_length));
    ASSERT_EQ(ErrorCode::kNone, response.GetHeaderFieldValues(Message::FieldKey::kContentType, content_type));

    ASSERT_EQ("3", content_length[0]);
    ASSERT_EQ("text/plain", content_type[0]);

    std::stringstream stream;
    std::size_t actual_bytes;

    ASSERT_EQ(ErrorCode::kNone, response.Read(stream, actual_bytes));
    ASSERT_EQ(3, actual_bytes);
    ASSERT_EQ("HI!", stream.str());
}

/**
 * Test is based on a running local web-server. Use the existing
 * SoapUI project (.xml) supplied in the unit-test file-system tree.
 */
TEST(TransferManager, RequestHelloWorld)
{
    TransferManager tm("localhost");

    // test request with absolute path.
    {
        Request request("localhost/CN/HelloWorld");
        std::shared_ptr<Response> response;

        ASSERT_EQ(ErrorCode::kNone, tm.Read(request, response));

        std::stringstream stream;
        std::size_t actual_bytes;

        ASSERT_EQ(ErrorCode::kNone, response->Read(stream, actual_bytes));
        ASSERT_TRUE(actual_bytes != 0);
        ASSERT_EQ("Hello World!", stream.str());
    }

    // test request with relative path.
    {
        Request request("/CN/HelloWorld");
        std::shared_ptr<Response> response;

        ASSERT_EQ(ErrorCode::kNone, tm.Read(request, response));

        std::stringstream stream;
        std::size_t actual_bytes;

        ASSERT_EQ(ErrorCode::kNone, response->Read(stream, actual_bytes));
        ASSERT_TRUE(actual_bytes != 0);
        ASSERT_EQ("Hello World!", stream.str());
    };
}

TEST(TransferManager, BadHostname)
{
    TransferManager tm("my_little_hostname");
    Request request("/Data");

    std::shared_ptr<Response> response;
    std::stringstream response_stream;
    std::size_t actual_bytes;

    ASSERT_EQ(ErrorCode::kNone, tm.Read(request, response));
    ASSERT_EQ(ErrorCode::kNone, response->Read(response_stream, actual_bytes));

    Response::StatusCode status_code;

    ASSERT_EQ(ErrorCode::kNone, response->GetStatusCode(status_code));
    ASSERT_TRUE(status_code != Response::StatusCode::kOk);
}

bool fileExists(const std::string& path)
{
    bool exists = true;

    FILE* pFile = fopen(path.c_str(), "rb");

    if (!pFile)
    {
        exists = false;
    }
    else
    {
        fclose(pFile);
    }

    return exists;
}

TEST(TransferManager, HttpsVolvoCloud)
{
    HttpsCredentials credentials;

    credentials.bUseSSLVerifyHost = true;
    credentials.bUseSSLVerifyPeer = true;
    credentials.eCertificateType = Certificate_Type_PEM;
    credentials.eSslKeyType = SslKey_Type_PEM;

    ASSERT_TRUE(fileExists("transfermanager/keyz/CAinfo.txt"));
    ASSERT_TRUE(fileExists("transfermanager/keyz/ccCert.cer"));
    ASSERT_TRUE(fileExists("transfermanager/keyz/ccCert.key"));

    credentials.sCAInfo = "transfermanager/keyz/CAinfo.txt";
    credentials.sCertificate = "transfermanager/keyz/ccCert.cer";
    credentials.sCertificateKey = "transfermanager/keyz/ccCert.key";
    TransferManager tm("", &credentials);
    Request request("https://cnep.test.volvocars.com/?clientid=TCAMCONTISPASE005");
    std::shared_ptr<Response> response;

    request.AddHeaderField(transfermanager::Message::FieldKey::kAcceptLanguage, "se");
    request.AddHeaderField(transfermanager::Message::FieldKey::kAcceptLanguage, "en-gb;q=0.8");
    request.AddHeaderField(transfermanager::Message::FieldKey::kAcceptLanguage, "en;q=0.7");
    request.AddHeaderField(::fsm::transfermanager::Message::FieldKey::kAccept,"application/volvo.cloud.EntryPoint+xml");

    fsm::transfermanager::ErrorCode err_code = tm.Read(request, response);

    ASSERT_TRUE(err_code == fsm::transfermanager::ErrorCode::kNone);
}

} // namespace transfermanager
} // namespace fsm

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
/** \}    end of addtogroup */
