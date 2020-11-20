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
 *  \file     ssm_signal_source.cc
 *  \brief    VOC Service SSM signal source.
 *  \author   Piotr Tomaszewski
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#include "voc_framework/signal_sources/ssm_signal_source.h"

#include <sstream>
#include <algorithm>

#include "dlt/dlt.h"
#include "voc_framework/signals/signal_factory.h"


DLT_IMPORT_CONTEXT(dlt_libfsm);

namespace fsm
{

SsmSignalSource::SsmSignalSource()
{
    //SSM signal source specific setup, may involve
    //setting upp connection to SSM, assigning callbacks etc
#ifndef VOC_TESTS
    DLT_LOG_STRING(dlt_libfsm, DLT_LOG_INFO, "SsmSignalSource: Signing up to SSM\n");
    ssm_.set_sms_callback(std::bind(&SsmSignalSource::ssm_sms_callback, this, _1));
    if (ssm_.SignalServiceManagerInit(std::bind(&SsmSignalSource::SSMCallBack, this, _1)) != signalservicemanager::ErrorCode::kNone)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "%s(): Failed to init SSM", __FUNCTION__);
        raise(SIGTERM);
    }
    ssm_.SignalServiceManagerConnect();

    m_sms_certificate.set_cert_name("/data/persistency/public/file/sms_server.cert.pem");
    m_sms_certificate.init();
#endif

    t_send_s_ack::instance().test();

}

SsmSignalSource::~SsmSignalSource()
{
#ifndef VOC_TESTS
    ssm_.SignalServiceManagerDisconnect();
    ssm_.SignalServiceManagerDone();
#endif
}


bool SsmSignalSource::SubscribeToTopic(std::string topic)
{
#ifndef VOC_TESTS
    //subscribe to mqtt topic through SSM
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "SsmSignalSource: subscribing to topic: %s\n", topic.c_str());
    signalservicemanager::ErrorCode result = ssm_.SignalServiceManagerSubscribe(topic, 1);
#endif
    return signalservicemanager::ErrorCode::kNone == result;
}

bool SsmSignalSource::UnSubscribeFromTopic(std::string topic)
{
#ifndef VOC_TESTS
    //unsubscribe from mqtt topic through SSM
    ssm_.SignalServiceManagerUnSubscribe(topic);
#endif

    return true;
}

bool SsmSignalSource::PublishMessageOnTopic(std::shared_ptr<PayloadInterface> signal,
                                            std::string topic)
{
    bool successful_publish = false;

    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO,
                    "Publishing signal %d on topic %s\n ",
                    signal ? signal->GetIdentifier().signal_type : 0,
                    topic.c_str());

    if (topic.size() > 0)
    {
        std::shared_ptr<std::vector<unsigned char>> vector_encoded = SignalFactory::Encode(signal);
        unsigned char* buffer = vector_encoded->data();
        size_t num_bytes = vector_encoded->size();

        if (0 != vector_encoded->size())
        {
#ifndef VOC_TESTS
            const_fs_Data encoded_data = {buffer, num_bytes};
            //TODO: refactor SSM to use std::vector insteed of structs
            successful_publish = (fsm::signalservicemanager::ErrorCode::kNone ==
                                  ssm_.SignalServiceManagerPublish(topic, encoded_data));
#else
            successful_publish = true;
#endif
        }
        else
        {
            DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "Failed to encode signal.");
        }
    }
    else
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "Cannot publish message, no topic defined");
    }

    return successful_publish;
}

#ifndef VOC_TESTS

void SsmSignalSource::SSMCallBack(fsm::signalservicemanager::MsgPkt_t &msg)
{
     DLT_LOG_STRING(dlt_libfsm, DLT_LOG_INFO, "SSMCallBack  zhouyou");
    //based on the input data create a typed signal using signal factory

    SignalFactory::t_factory_return ret = SignalFactory::DecodeServiceV2(
                                reinterpret_cast<const unsigned char*>(msg.payload_.c_str()),
                                msg.payload_.size());

    if (ret.error_code != 0)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "%s(): failed to decode a signal, ignoring it",
                        __FUNCTION__);
        return;
    }
    if (ret.ret_signal == nullptr)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "%s(): return signal is nullptr, ignoring it",
                        __FUNCTION__);
        return;
    }
    
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "%s(): received a signal %s\n",
                        __FUNCTION__,
                        ret.ret_signal->ToString().c_str());
    t_send_struct send_struct;
    send_struct.vd_service = ret.copyed_vd_service;
    int ret2 = t_send_s_ack::instance().send_s_ack(send_struct);
    if(ret2 != 0)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG, "%s(): failed send s_ack",
                        __FUNCTION__);
        //return;
    }
    HandleSignal(ret.ret_signal, msg.topic_);
    return;
}
#endif

//lijing-test

void lijing_test(NADIF_SMS_DATA *out_sms_data)
{
  unsigned char c_sms[]="56444652484CFCF7E05B87AFE50048E05B87AFE50048800C780015400001801872686C01030081405B61F4DDBAB277BBB1A8574E8ABAB001DB4FEE74202E135B2CDDD98EF887BF59942F0CC0D6230256BB79557B730DF5DA6C7BED6658A2B5BE54A08BA53732FD37";
  out_sms_data->data_len =asc_str_2_hex_str(c_sms, (unsigned char *)out_sms_data->data);
  if(out_sms_data->data_len <= 0)

{
      DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "%s(), asc_str_2_hex_str error \n", __FUNCTION__);
      return;
  }
  return;
}


void SsmSignalSource::ssm_sms_callback(const NADIF_SMS_DATA *sms_data)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "%s()  zhouyou", __FUNCTION__);

    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG,
                    "%s() , begin",
                    __FUNCTION__);

    int signature_len = 66;
    const int sms_header_len = 6;

    //lijing-test
    //NADIF_SMS_DATA temp_sms;
    //lijing_test(&temp_sms);
    //const NADIF_SMS_DATA *pSms_data = &temp_sms;

    const NADIF_SMS_DATA *pSms_data = sms_data;

    if(pSms_data == NULL)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "%s() , error!!!!!! pSms_data is null",
                     __FUNCTION__);
        return;
    }

    std::string sms_hex_str;
    unsigned int sms_len = pSms_data->data_len;
    hex_to_hexstr((const unsigned char *)pSms_data->data, sms_len, sms_hex_str);

    if (pSms_data->data_len <= (sms_header_len + signature_len) ||
        pSms_data->data_len > 500)
    {
        //lijing-test. not verify sms signature in E3
        //DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "%s() , error; sms data length = %d, drop sms:%s",
        //             __FUNCTION__, pSms_data->data_len, sms_hex_str.c_str());
        //return;
    }

    int pos = 0, tmp_pos = 0;
    //PROTOCOL ID
    char protocol_id[4];
    tmp_pos=0;
    protocol_id[tmp_pos++] = pSms_data->data[pos++];
    protocol_id[tmp_pos++] = pSms_data->data[pos++];
    protocol_id[tmp_pos++] = pSms_data->data[pos++];
    protocol_id[tmp_pos++] = 0;
    if (strcmp(protocol_id, "VDI") != 0)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "%s() , error, protocol id is not VDA, %s",
                        __FUNCTION__, protocol_id);
    }
    //service id
    char service_id[4];
    tmp_pos=0;
    service_id[tmp_pos++] = pSms_data->data[pos++];
    service_id[tmp_pos++] = pSms_data->data[pos++];
    service_id[tmp_pos++] = pSms_data->data[pos++];
    service_id[tmp_pos++] = 0;
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG, "%s() , protocol_id=%s, service_id=%s",
                    __FUNCTION__, protocol_id, service_id);
    //asn payload
    //lijing-test. not verify sms signature in E3
    //size_t asn_payload_len = pSms_data->data_len - sms_header_len - signature_len;
    size_t asn_payload_len = pSms_data->data_len - sms_header_len;
    if (asn_payload_len <= 0)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "%s() , ERROR!!!!!!!!! asn_payload_len=%d",
                        __FUNCTION__, asn_payload_len);
        return;
    }
    std::shared_ptr<std::vector<char> > asn_payload = std::make_shared<std::vector<char> >(asn_payload_len);
    if(asn_payload.get() == NULL)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "%s() , ERROR!!!!!!!!! (asn_payload.get() == NULL)",
                        __FUNCTION__);
        return;
    }
    memcpy((*asn_payload).data(), pSms_data->data + sms_header_len, (*asn_payload).size());
//    std::shared_ptr<Signal> signal = SignalFactory::DecodeServiceV1(
//                                reinterpret_cast<const unsigned char*>((*asn_payload).data()),
//                             (*asn_payload).size());
//    if (signal.get() == NULL)
//    {
//        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "%s() , ERROR!!!!!!!!! (signal.get() == NULL)",
//                        __FUNCTION__);
//        return;
//    }
    SignalFactory::t_factory_return ret = SignalFactory::DecodeServiceV2(
                                reinterpret_cast<const unsigned char*>((*asn_payload).data())
                                ,(*asn_payload).size());
    if (ret.error_code != 0)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "%s(): failed to decode a signal, ignoring it",
                        __FUNCTION__);
        return;
    }
    if (ret.ret_signal == nullptr)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "%s(): return signal is nullptr, ignoring it",
                        __FUNCTION__);
        return;
    }

    //lijing-test. not verify sms signature in E3
#if 0
    if (sms_len > sizeof(pSms_data->data))
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
                        "%s() , ERROR!!!!!!!!! sms data length is error: %d",
                        __FUNCTION__, pSms_data->data_len);
        sms_len = sizeof(pSms_data->data);
    }

    //verify sms signature
    if(m_sms_certificate.is_available() == false)
    {
      m_sms_certificate.init();
    }
    if(m_sms_certificate.is_available() == false)
    {
      DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
            "%s() , ERROR!!! sms certificate is not available, drop sms:%s",
                      __FUNCTION__, sms_hex_str.c_str());
      return;
    }
    //obtain payload
    unsigned char payload[200];
    std::memset(payload, 0, sizeof(payload));
    int payload_len = sms_len - 66;
    if(payload_len <=0 || payload_len >200)
    {
      DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
            "%s() , ERROR!!! sms payload is invalid :%d, drop sms:%s",
                      __FUNCTION__, payload_len ,sms_hex_str.c_str());
      return;
    }
    std::memcpy(payload, pSms_data->data, payload_len);
    //obtain signature
    unsigned char signature[66];
    std::memset(signature, 0, sizeof(signature));
    std::memcpy(signature, pSms_data->data +payload_len, signature_len);
    //verify
    int verify_result = m_sms_certificate.verify(payload, payload_len, signature, signature_len);
    if(verify_result != 1)
    {
      DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
            "%s() , ERROR!!! verify sms failed, drop sms:%s",
                      __FUNCTION__, sms_hex_str.c_str());
      return;
    }
#endif
    t_send_struct send_struct;
    send_struct.vd_service = ret.copyed_vd_service;
    send_struct.is_sms_retry = true;
    int ret2 = t_send_s_ack::instance().send_s_ack(send_struct);
    if(ret2 != 0)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG, "%s(): failed send s_ack",
                        __FUNCTION__);
        //return;
    }
    
    HandleSignal(ret.ret_signal, "");

    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG,
                    "%s() , end",
                    __FUNCTION__);

}

void SsmSignalSource::HandleSignal(std::shared_ptr<Signal> signal, const std::string topic)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "SsmSignalSource: broadcasting signal %s\n", signal->ToString().c_str());
    BroadcastSignal(signal);
}



} // namespace fsm
/** \}    end of addtogroup */
