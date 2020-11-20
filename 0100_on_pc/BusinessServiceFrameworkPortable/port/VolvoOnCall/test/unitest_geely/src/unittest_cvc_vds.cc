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
 *  \file     unittest_signal_source.cc
 *  \brief    Volvo On Call, signal_source, Test Cases
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup TestVolvoOnCall
 *  \{
 */

/******************************************************************************
 * INCLUDE FILES
 ******************************************************************************/


#include "voc_framework/signals/geely_asn_common.h"

#include "ServiceData.h"
#include "VDServiceRequest.h"
#include "RequestHeader.h"

#include <gtest/gtest.h>
#include <vector>
#include <mutex>
#include <string.h>
#include <fstream>
#include <memory>
#include <cstring>


#define NOT_NULL(p) if(!p) { std::cout << "[error], " << #p << " is null" << std::endl; exit(0); }
#define NOT_NULL_S(p, s) if(!p) { std::cout << "[error], " << #p << " is null; " << s << std::endl; exit(0); }
#define NOT_EQ_0(v) if( v != 0 ) { std::cout << "[error], " << #v << " is not eq 0" << std::endl; exit(0); }
#define NOT_EQ_0_S(v, s) if( v != 0 ) { std::cout << "[error], " << v << " is not eq 0; " << s << std::endl; exit(0); }

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
namespace geely_asn_test
{

using namespace fsm;
/******************************************************************************
 * GLOBALS
 ******************************************************************************/


/******************************************************************************
 * TESTS
 ******************************************************************************/

TEST (HelloworldTest, TestEncodeVDServiceRequest)
{
#if 0
    std::cout << "TestEncodeVDServiceRequest" <<std::endl;

    VDServiceRequest_t* vdsServiceRequest = (VDServiceRequest_t*) calloc(1, sizeof(VDServiceRequest_t));
    NOT_NULL(vdsServiceRequest);

    //set header
    //requestid
    vdsServiceRequest->header.requestid = 222;
    //timeStamp
    vdsServiceRequest->header.timeStamp.seconds = 11;
    vdsServiceRequest->header.timeStamp.milliseconds = (long *)calloc(1, sizeof(long)); 
    NOT_NULL(vdsServiceRequest->header.timeStamp.milliseconds);
    *vdsServiceRequest->header.timeStamp.milliseconds = 11;

    int ret =0;
    //set body
    RequestBody_t *body = (RequestBody_t *)calloc(1, sizeof(RequestBody_t));
    NOT_NULL(body);
    vdsServiceRequest->body = body;

    //serviceId
    ServiceId_t *serviceId = (ServiceId_t *)calloc(1, sizeof(ServiceId_t));
    NOT_NULL(serviceId);

    *serviceId = 10000;
    body->serviceId = serviceId;

    //serviceData
    body->serviceData = (ServiceData_t *)calloc(1, sizeof(ServiceData_t));
    NOT_NULL(body->serviceData);

	//set service trigger
    body->serviceData->serviceTrigger = static_cast<ServiceTrigger *>(calloc(1, sizeof(ServiceTrigger)));
    NOT_NULL(body->serviceData->serviceTrigger);

    body->serviceData->serviceTrigger->scheduledTime = static_cast<TimeStamp_t *>(calloc(1, sizeof(TimeStamp_t)));
    NOT_NULL(body->serviceData->serviceTrigger->scheduledTime);
    body->serviceData->serviceTrigger->scheduledTime->seconds = 11;

	//set serviceParameters
	body->serviceData->serviceParameters = static_cast<struct ServiceData_t::ServiceData__serviceParameters *>(calloc(1, sizeof(struct ServiceData_t::ServiceData__serviceParameters)));
	NOT_NULL(body->serviceData->serviceParameters);
    body->serviceData->serviceParameters->list.array = static_cast<ServiceParameter_t**>(calloc(1, sizeof(ServiceParameter_t*)));
	NOT_NULL(body->serviceData->serviceParameters->list.array);

    ServiceParameter *tmp_serviceParameter = static_cast<ServiceParameter *>(calloc(1, sizeof(ServiceParameter)));
	NOT_NULL(tmp_serviceParameter);
    OCTET_STRING_fromString(&(tmp_serviceParameter->key), "12345678901234567");
    ret = ASN_SEQUENCE_ADD(&(body->serviceData->serviceParameters->list), tmp_serviceParameter);
    NOT_EQ_0_S(ret, "ASN_SEQUENCE_ADD");

    //ServiceCommand
    body->serviceData->serviceCommand = static_cast<ServiceCommand_t *>(calloc(1, sizeof(ServiceCommand_t)));
	NOT_NULL(body->serviceData->serviceCommand);
    * body->serviceData->serviceCommand = 2;

    //serviceResult
    body->serviceData->serviceResult = static_cast<ServiceResult *>(calloc(1, sizeof(ServiceResult)));
	NOT_NULL(body->serviceData->serviceResult);
    body->serviceData->serviceResult->operationSucceeded = 0;
    body->serviceData->serviceResult->error = (Error *)(calloc(1, sizeof(Error)));
	NOT_NULL(body->serviceData->serviceResult->error);
    body->serviceData->serviceResult->error->message = (PrintableString_t *)(calloc(1, sizeof(PrintableString_t)));
	NOT_NULL(body->serviceData->serviceResult->error->message);
    OCTET_STRING_fromString(body->serviceData->serviceResult->error->message, "22345678901234567");

    //GeneralMessage
    body->serviceData->message = static_cast<GeneralMessage *>(calloc(1, sizeof(GeneralMessage)));
    NOT_NULL(body->serviceData->message);
    body->serviceData->message->present = GeneralMessage_PR_ocetMessage;
    OCTET_STRING_fromString(& body->serviceData->message->choice.ocetMessage, "33345678901234567");

    //pois
    body->serviceData->pois = (ServiceData::ServiceData__pois *)(calloc(1, sizeof(ServiceData::ServiceData__pois)));
    body->serviceData->pois->list.array = (PointOfInterest**)(calloc(1, sizeof(PointOfInterest*)));
    PointOfInterest * point_of_interest = (PointOfInterest *)(calloc(1, sizeof(PointOfInterest)));
    OCTET_STRING_fromString(& point_of_interest->name, "point_of_interest 111");
    ret = ASN_SEQUENCE_ADD(& body->serviceData->pois->list, point_of_interest);

    //vehicleStatus
    body->serviceData->vehicleStatus = (VehicleStatus_t *)(calloc(1, sizeof(VehicleStatus_t)));
    body->serviceData->vehicleStatus->msd = (OEMMSD_t *)(calloc(1, sizeof(OEMMSD_t)));
    body->serviceData->vehicleStatus->msd->vin = (VIN_t *)(calloc(1, sizeof(VIN_t)));
    OCTET_STRING_fromString(body->serviceData->vehicleStatus->msd->vin, "66666678901234567");
    

 
    std::shared_ptr<t_geely_data> data = std::make_shared<t_geely_data>();
    ret = serialize_gly_asn_2_buf(vdsServiceRequest, data);
    if (ret != 0)
    {
        std::cout<<"%s___, call serialize_gly_asn_2_buf failed = %d"<<std::endl;
    }
    std::cout << "encode begin..." <<std::endl;

    std::cout << "dump to bin file, data->num_bytes = " << data->num_bytes <<std::endl;
    std::ofstream file("./geely_asn.bin", std::ios::out | std::ios::binary);
    if (file.is_open())
    {
        file.write((const char *)data->data, data->num_bytes);
        file.close();
    }
    else
    {
        std::cout << "[error] dump to bin file failed" << std::endl;
    }

    std::string xer_str;
    ret = encode_buf_to_xer((unsigned char *)data->data, data->num_bytes, xer_str);
    if (ret != 0)
    {
        std::cout << "encode_buf_to_xer error!!!" <<std::endl;
    }

    std::cout << "xer str = "<< xer_str <<std::endl;
    // asn_fprint(stdout, &asn_DEF_VDServiceRequest, vdsServiceRequest);
    
#endif

    ASSERT_TRUE(1);
    
}


TEST (HelloworldTest, TestDecodeVDServiceRequest)
{
#if 0 

    std::cout << "TestDecodeVDServiceRequest" <<std::endl;

    std::string file_name = "/mnt/hgfs/ubuntu/xusujuan/asn_bin/geely_asn_new.bin";
    
    std::ifstream file (file_name, std::ios::in|std::ios::binary|std::ios::ate);

    t_geely_data in_data;
    
    if (file.is_open())
    {
        in_data.num_bytes = file.tellg();
        in_data.data = (unsigned char *)malloc(in_data.num_bytes);
        file.seekg (0, std::ios::beg);
        file.read ((char *)in_data.data, in_data.num_bytes);
        file.close();
    }
    else
    {
        std::cout << "[error] _____________ file can not open " << std::endl;

    }

    std::cout << "data size = " << in_data.num_bytes << std::endl;

    VDServiceRequest_t *out_vds_sr = 0;

    asn_dec_rval_t  retVal = ber_decode(0, &asn_DEF_VDServiceRequest, (void **)&out_vds_sr, 
                                                                in_data.data, in_data.num_bytes);
  
    if(retVal.code != RC_OK)
    {
          std::cout<<"DecodeService  ber_decode  ERROR"<<std::endl;
    }

    std::cout << "file name:" << file_name << ", decode data is:" << std::endl;
    //asn_fprint(stdout, &asn_DEF_VDServiceRequest, out_vds_sr);

#endif    

    ASSERT_TRUE(1);
}



} // namesapce
