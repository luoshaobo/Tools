/**
 * Copyright (C) 2016-2017 Continental Automotive GmbH
 *
 * Licensed under the "Supplier unrestricted white box" category, as
 * described in the 20160322-1_SW-amendment TCAM.pdf agreement
 * between Volvo Cars and Continental Automotive GmbH.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *  \file     ccm_test.cpp
 *  \brief    Foundation Services Test Code
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup foundationservicemanager
 *  \{
 */

#include "fsm_ccm.h"
#include "fsm.h"
#include "keystore.h"
#include "usermanager_interface.h"

#include "dlt/dlt.h"

#include <string>
#include <cstring>
#include <cstdio>

#define EXPECTED_MSG_ID 42383
#define EXPECTED_RESEND_COUNTER 42
#define EXPECTED_SCHEMA_VERSION 123
#define EXPECTED_PRELIMINARY_VERSION 456
#define EXPECTED_TIMESTAMP_SEC 789
#define EXPECTED_TIMESTAMP_NSEC 12
#define EXPECTED_TIME_TO_LIVE 49999999999
#define EXPECTED_CONTENT_ENCODING FS_ENCODING_UPER
#define EXPECTED_CONTENT_SCHEMA_VERSION 123
#define EXPECTED_CONTENT_PRELIMINARY_VERSION 456
#define EXPECTED_METADATA_CONTENT_ENCODING FS_ENCODING_PER
char const *EXPECTED_SESSION_ID = "thisisasessioni";
char const *EXPECTED_TRANSACTION_ID = "trans";
std::shared_ptr<std::vector<unsigned char>> EXPECTED_CONTENT =
                std::make_shared<std::vector<unsigned char>>(4, 'a');
char const *EXPECTED_OID = "1.3.6.1.4.1.37916.9.9.999";

std::shared_ptr<std::vector<unsigned char>> EXPECTED_METADATA_CONTENT =
                std::make_shared<std::vector<unsigned char>>(4, 'b');
char const *EXPECTED_METADATA_OID = "1.3.6.1.4.1.37916.9.9.789";

DLT_DECLARE_CONTEXT(dlt_fssdd);  // must be globally visible!

//check message against expected values
//return 0 if not ok
int checkMessage(ccm_Message* msg)
{
    int ok = 0;

    ok = ok && msg->header.version.schema_version == EXPECTED_SCHEMA_VERSION;
    ok = ok && msg->header.version.preliminary_version == EXPECTED_PRELIMINARY_VERSION;
    ok = ok && msg->header.timestamp.tv_sec == EXPECTED_TIMESTAMP_SEC;
    ok = ok && msg->header.timestamp.tv_nsec == EXPECTED_TIMESTAMP_NSEC;
    ok = ok && msg->header.time_to_live == EXPECTED_TIME_TO_LIVE;
    ok = ok && (strncmp(msg->header.session_id,
                        EXPECTED_SESSION_ID,
                        CCM_SESSION_ID_LEN) == 0);
    ok = ok && (strncmp(msg->header.transaction_id,
                        EXPECTED_TRANSACTION_ID,
                        CCM_TRANSACTION_ID_LEN) == 0);
    ok = msg->header.message_id == EXPECTED_MSG_ID;
    ok = msg->header.resend_counter == EXPECTED_RESEND_COUNTER;
    ok = msg->header.content_encoding == EXPECTED_CONTENT_ENCODING;
    ok = ok && msg->header.content_version.schema_version == EXPECTED_CONTENT_SCHEMA_VERSION;
    ok = ok && msg->header.content_version.preliminary_version == EXPECTED_CONTENT_PRELIMINARY_VERSION;

    ok = ok && *(msg->payload.content.content) == *EXPECTED_CONTENT;

    ok = ok && (strncmp((char*)(msg->payload.content.oid.oid),
                        EXPECTED_OID,
                        FS_OID_MAX_LEN) == 0);

    for (size_t i = 0; i < msg->payload.num_metadata; i++)
    {
        ok = msg->payload.metadata[i].content_encoding == EXPECTED_METADATA_CONTENT_ENCODING;
        ok = ok && *(msg->payload.metadata[i].content) == *EXPECTED_METADATA_CONTENT;
        ok = ok && (strncmp((char*)(msg->payload.metadata[i].oid.oid),
                            EXPECTED_METADATA_OID,
                            FS_OID_MAX_LEN) == 0);

    }

    printf("%s\n", ok ? "RESULT: PASS:" : "RESULT: FAIL:");

    if (!ok)
        return 0;

    else
        return 1;

}

void dumpBinaries(fs_Data* data, int testNum)
{
    /* uncomment to enable very verbose output
    printf("encoded content:\n");
    fwrite(data->data, 1, data->num_bytes, stdout);
    printf("\n");

    printf("as hex:\n");
    printf("%x ", ((char*)data->data)[0] & 0xff);  //because 0%8 is 0
    for (size_t i = 1; i < data->num_bytes; i++)
    {
        printf("%x ", ((char*)data->data)[i] & 0xff);
        if (i % 8 == 0)
            printf("\n");
    }
    */

    if (testNum > 0 && testNum < 100)
    {
        char fname[17];
        sprintf(fname, "/data/test%d.bin", testNum);
        remove(fname);
        FILE* fp = fopen(fname, "wb");
        fwrite(data->data, 1, data->num_bytes, fp);
        fclose(fp);
    }
    else
    {
        printf("WARNING: you are running to many tests!");
    }
}

void PrintCCM(ccm_Message* message)
{
    printf("received message:\n");
    printf("message_id %u\n", message->header.message_id);
    printf("OID: %s\n", message->payload.content.oid.oid);
    printf("content: %.*s\n", message->payload.content.content->size(),
                              (char*)message->payload.content.content->data());

}

int main(int argc, char *argv[])
{

    DLT_REGISTER_APP("CCMT", "FSAPI CCM Test");
    DLT_REGISTER_CONTEXT(dlt_fssdd, "CCMT","CCM Test");

    FsmInitialize();
    OpenSSL_add_all_algorithms();

    fs_Data encoded = {};

    ccm_Message sendMessage;

    sendMessage.header.version.schema_version = EXPECTED_SCHEMA_VERSION;
    sendMessage.header.version.preliminary_version = EXPECTED_PRELIMINARY_VERSION;
    sendMessage.header.timestamp.tv_sec = EXPECTED_TIMESTAMP_SEC;
    sendMessage.header.timestamp.tv_nsec = EXPECTED_TIMESTAMP_NSEC;
    sendMessage.header.time_to_live = EXPECTED_TIME_TO_LIVE;
    strncpy(sendMessage.header.session_id, EXPECTED_SESSION_ID, CCM_SESSION_ID_LEN);
    strncpy(sendMessage.header.transaction_id, EXPECTED_TRANSACTION_ID, CCM_TRANSACTION_ID_LEN);
    sendMessage.header.message_id=EXPECTED_MSG_ID;
    sendMessage.header.resend_counter=EXPECTED_RESEND_COUNTER;
    sendMessage.header.content_encoding = EXPECTED_CONTENT_ENCODING;
    sendMessage.header.content_version.schema_version = EXPECTED_CONTENT_SCHEMA_VERSION;
    sendMessage.header.content_version.preliminary_version = EXPECTED_CONTENT_PRELIMINARY_VERSION;

    sendMessage.payload.content.content = EXPECTED_CONTENT;
    strncpy(sendMessage.payload.content.oid.oid, EXPECTED_OID, FS_OID_MAX_LEN);
    sendMessage.payload.num_metadata = 0;

    ccm_Message* receiveMessage = NULL;

    //sender should never be resolved to a user, as the signer will be our vehicle credentials
    fsm::UserId sender;
    std::vector<fsm::UserId> no_recipients;
    std::vector<fsm::UserId> recipients;

    std::shared_ptr<fsm::UsermanagerInterface> user_manager =
        fsm::UsermanagerInterface::Create();

    printf("getting users\n");

    if (user_manager->GetUsers(recipients) != fsm::ReturnCode::kSuccess)
    {
        printf("RESULT: FAIL: did not find any users.\n");
        return -1;
    }

    if (argc < 2)
    {
        /*** TEST 1 ***/
        printf("\n");
        printf("testing encode, no encrypt, no CCM-Payload\n");

        if (!ccm_Encode(&encoded,
                        &sendMessage,
                        no_recipients,
                        0))
        {
            printf("RESULT: FAIL: encode failed.\n");
            return -1;
        }

        if (encoded.data == NULL)
        {
            printf("RESULT: FAIL: got null data.\n");
            return -1;
        }

        dumpBinaries(&encoded, 1);

        if(!ccm_Decode((const_fs_Data*)&encoded,
                       &receiveMessage,
                       sender,
                       nullptr,
                       0))
        {
            printf("RESULT: FAIL: decode failed.\n");
            return -1;
        }

        if (receiveMessage == NULL)
        {
            printf("RESULT: FAIL: got null message.\n");
            return -1;
        }

        checkMessage(receiveMessage);

        delete receiveMessage;

        free(encoded.data);
        encoded.num_bytes = 0;
        /*** END TEST 1 ***/


        /*** START TEST 2 ***/
        printf("\n");
        printf("testing encode, encrypted, no CCM-Payload\n");

        if (!ccm_Encode(&encoded,
                        &sendMessage,
                        recipients,
                        0))
        {
            printf("RESULT: FAIL: encode failed.\n");
            return -1;
        }

        if (encoded.data == NULL)
        {
            printf("RESULT: FAIL: got null data.\n");
            return -1;
        }

        dumpBinaries(&encoded, 2);

        if(!ccm_Decode((const_fs_Data*)&encoded,
                       &receiveMessage,
                       sender,
                       nullptr,
                       0))
        {
            printf("RESULT: FAIL: decode failed.\n");
            return -1;
        }

        if (receiveMessage == NULL)
        {
            printf("RESULT: FAIL: got null message.\n");
            return -1;
        }

        checkMessage(receiveMessage);

        delete receiveMessage;

        free(encoded.data);
        encoded.num_bytes = 0;
        /*** END TEST 2 ***/

        /*** TEST 3 ***/
        printf("\n");
        printf("testing encode, no encrypt forced payload\n");

        sendMessage.payload.content.content_encoding = FS_ENCODING_DER;

        if ( !ccm_Encode(&encoded,
                         &sendMessage,
                         no_recipients,
                         CCM_ENCODE_AS_PAYLOAD))
        {
            printf("RESULT: FAIL: encode failed.\n");
            return -1;
        }

        if (encoded.data == NULL)
        {
            printf("RESULT: FAIL: got null data.\n");
            return -1;
        }

        dumpBinaries(&encoded, 3);

        if(!ccm_Decode((const_fs_Data*)&encoded,
                       &receiveMessage,
                       sender,
                       nullptr,
                       0))
        {
            printf("RESULT: FAIL: decode failed.\n");
            return -1;
        }

        if (receiveMessage == NULL)
        {
            printf("RESULT: FAIL: got null message.\n");
            return -1;
        }

        checkMessage(receiveMessage);

        delete receiveMessage;

        free(encoded.data);
        encoded.num_bytes = 0;
        /*** END TEST 3 ***/


        /*** START TEST 4 ***/
        printf("\n");
        printf("testing encode, encrypted, forced payload\n");

        if (!ccm_Encode(&encoded,
                        &sendMessage,
                        recipients,
                        CCM_ENCODE_AS_PAYLOAD))
        {
            printf("RESULT: FAIL: encode failed.\n");
            return -1;
        }

        if (encoded.data == NULL)
        {
            printf("RESULT: FAIL: got null data.\n");
            return -1;
        }

        dumpBinaries(&encoded, 4);

        if(!ccm_Decode((const_fs_Data*)&encoded,
                       &receiveMessage,
                       sender,
                       nullptr,
                       0))
        {
            printf("RESULT: FAIL: decode failed.\n");
            return -1;
        }

        if (receiveMessage == NULL)
        {
            printf("RESULT: FAIL: got null message.\n");
            return -1;
        }

        checkMessage(receiveMessage);

        delete receiveMessage;

        free(encoded.data);
        encoded.num_bytes = 0;

        sendMessage.payload.content.content_encoding = FS_ENCODING_UNDEFINED;
        /*** END TEST 4 ***/

        /*** TEST 5 ***/
        sendMessage.payload.num_metadata = 1;
        sendMessage.payload.content.content_encoding = FS_ENCODING_DER;
        sendMessage.payload.metadata[0].content_encoding = EXPECTED_METADATA_CONTENT_ENCODING;
        sendMessage.payload.metadata[0].content = EXPECTED_METADATA_CONTENT;
        strncpy(sendMessage.payload.metadata[0].oid.oid, EXPECTED_METADATA_OID, FS_OID_MAX_LEN);

        printf("\n");
        printf("testing encode, no encrypt\n");

        if (!ccm_Encode(&encoded,
                        &sendMessage,
                        no_recipients,
                        0))
        {
            printf("RESULT: FAIL: encode failed.\n");
            return -1;
        }

        if (encoded.data == NULL)
        {
            printf("RESULT: FAIL: got null data.\n");
            return -1;
        }

        dumpBinaries(&encoded, 5);

        if(!ccm_Decode((const_fs_Data*)&encoded,
                       &receiveMessage,
                       sender,
                       nullptr,
                       0))
        {
            printf("RESULT: FAIL: decode failed.\n");
            return -1;
        }

        if (receiveMessage == NULL)
        {
            printf("RESULT: FAIL: got null message.\n");
            return -1;
        }

        checkMessage(receiveMessage);

        delete receiveMessage;

        free(encoded.data);
        encoded.num_bytes = 0;
        /*** END TEST 5 ***/


        /*** START TEST 6 ***/
        printf("\n");
        printf("testing encode, encrypted\n");

        if (!ccm_Encode(&encoded,
                        &sendMessage,
                        recipients,
                        0))
        {
            printf("RESULT: FAIL: encode failed.\n");
            return -1;
        }

        if (encoded.data == NULL)
        {
            printf("RESULT: FAIL: got null data.\n");
            return -1;
        }

        dumpBinaries(&encoded, 6);

        if(!ccm_Decode((const_fs_Data*)&encoded,
                       &receiveMessage,
                       sender,
                       nullptr,
                       0))
        {
            printf("RESULT: FAIL: decode failed.\n");
            return -1;
        }

        if (receiveMessage == NULL)
        {
            printf("RESULT: FAIL: got null message.\n");
            return -1;
        }

        checkMessage(receiveMessage);

        delete receiveMessage;

        free(encoded.data);
        encoded.num_bytes = 0;
        /*** END TEST 6 ***/
    }

    if (argc == 2)
    {
        printf("reading message file: %s\n", argv[1]);
        encoded.data = malloc(10000);  //crazy large?

        FILE* fp = fopen(argv[1], "rb");
        encoded.num_bytes = fread(encoded.data, 1, 10000, fp);
        fclose(fp);

        if (encoded.num_bytes == 0)
        {
            printf("RESULT: FAIL: failed to read message from fs\n");
            return -1;
        }

        printf("starting decode\n");

        if(!ccm_Decode((const_fs_Data*)&encoded,
                       &receiveMessage,
                       sender,
                       nullptr,
                       0))
        {
            printf("RESULT: FAIL: decode failed.\n");
            return -1;
        }

        printf("after decode\n");

        if (receiveMessage == NULL)
        {
            printf("RESULT: FAIL: got null message.\n");
            return -1;
        }

        PrintCCM(receiveMessage);

        delete receiveMessage;

        free(encoded.data);
        encoded.num_bytes = 0;
    }

    FsmTerminate();
    EVP_cleanup();

    DLT_UNREGISTER_CONTEXT(dlt_fssdd);
    DLT_UNREGISTER_APP();
}
