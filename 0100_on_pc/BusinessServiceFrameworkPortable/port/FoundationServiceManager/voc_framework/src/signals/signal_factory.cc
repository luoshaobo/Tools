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
 *  \file     signal_factory.cc
 *  \brief    VOC Service signal factory
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#include "voc_framework/signals/ccm.h"
#include "voc_framework/transactions/transaction_id.h"
#include "voc_framework/signals/signal_factory.h"
#include "voc_framework/signals/geely_asn_common.h"

#include "keystore.h"

#include <cstring>
#include <functional>
#include <map>
#include <sstream>
#include <string>
#include <sstream>

#include "dlt/dlt.h"
#include "fsm_ccm.h"
#include "keystore.h"
#include "usermanager_interface.h"

#ifdef ENABLE_SIGNAL_INJECTION
#include "voc_framework/signals/vehicle_comm_signal.h"
#include "voc_framework/signals/ip_command_broker_signal.h"
#endif

#include "voc_framework/transactions/transaction_id.h"
#include "voc_framework/signals/vdmsg_signal_types.h"
#include "voc_framework/signals/signal.h"

#include "signal_adapt_base.h"

DLT_IMPORT_CONTEXT(dlt_libfsm);

namespace fsm
{

//define static class members
std::map<std::string, SignalFactory::SignalFactoryFunction> SignalFactory::oid_factory_map_;
std::mutex SignalFactory::oid_factory_map_mutex_;

std::map<long, SignalFactory::VDSSignalVdmsgFunction> SignalFactory::serviceId_factory_map_;
std::mutex SignalFactory::serviceId_factory_map_mutex_;

std::map<long, SignalFactory::CreateSignalFunction> SignalFactory::signal_factory_map_;
std::mutex SignalFactory::signal_factory_mutex_;

std::map<PayloadInterface::CodecType, std::shared_ptr<CodecInterface>> SignalFactory::codec_map_;
std::mutex SignalFactory::codec_map_mutex_;
std::map<PayloadInterface::PayloadIdentifier, SignalFactory::PayloadFactoryFunction> SignalFactory::payload_factory_map_;
std::mutex SignalFactory::payload_factory_map_mutex_;
std::shared_ptr<CcmCodec> SignalFactory::ccm_codec_ = std::make_shared<CcmCodec>();
std::shared_ptr<MarbenCodec> SignalFactory::marben_codec_ = std::make_shared<MarbenCodec>();
std::shared_ptr<XmlCodec> SignalFactory::xml_codec_ = std::make_shared<XmlCodec>();

//static
void SignalFactory::Register_CreateSignalFunction(long serviceType, CreateSignalFunction fun)
{
    std::lock_guard<std::mutex> lock(signal_factory_mutex_);
    if (serviceType>0 && fun != NULL)
    {
        signal_factory_map_[serviceType] = fun;
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "%s(), serviceType =%d" ,
            __FUNCTION__, serviceType);
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
            "%s(), Incorrect input to SignalVdmsgFunction",
            __FUNCTION__);
    }
}

void SignalFactory::RegisterVdmsgFactory(long serviceType, SignalVdmsgFunction factory_function)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,"%s(): error, should not running here \n",
                __FUNCTION__);

#if 0
    std::lock_guard<std::mutex> lock(serviceId_factory_map_mutex_);
    if (serviceType>0&&factory_function != NULL)
    {
        serviceId_factory_map_[serviceType] = factory_function;
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "serviceType =%d" , serviceType);
    }
    else
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "Incorrect input to SignalVdmsgFunction");
    }
#endif
}

void SignalFactory::RegisterVdmsgFactory(long serviceType, VDSSignalVdmsgFunction factory_function)
{
    std::lock_guard<std::mutex> lock(serviceId_factory_map_mutex_);
    if (serviceType>0&&factory_function != NULL)
    {
        serviceId_factory_map_[serviceType] = factory_function;
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "serviceType =%d" , serviceType);
    }
    else
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "Incorrect input to SignalVdmsgFunction");
    }
}

void SignalFactory::RegisterCcmFactory(std::string oid, SignalFactoryFunction factory_function)
{
    std::lock_guard<std::mutex> lock(oid_factory_map_mutex_);
    if (oid.size() > 0 && factory_function != NULL)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "Registering factory for oid: %s", oid.c_str());
        oid_factory_map_[oid] = factory_function;
    }
    else
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "Incorrect input to RegisterNewSignalType");
    }
}



void SignalFactory::RegisterPayloadFactory(PayloadInterface::PayloadIdentifier identifier,
                            PayloadFactoryFunction factory_function)
{
    std::lock_guard<std::mutex> lock(payload_factory_map_mutex_);
    if (identifier.oid.size() > 0 && factory_function != NULL)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "Registering factory for oid: %s",
                        identifier.oid.c_str());
        payload_factory_map_[identifier] = factory_function;
    }
    else
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "Incorrect input to RegisterPayloadFactory");
    }
}



void SignalFactory::RegisterPayloadFactory(std::string oid,
                            PayloadFactoryFunction factory_function)
{
    std::lock_guard<std::mutex> lock(payload_factory_map_mutex_);
    if (oid.size() > 0 && factory_function != NULL)
    {
        PayloadInterface::PayloadIdentifier identifier;
        identifier.oid = oid;
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "Registering factory for oid: %s",
                        oid.c_str());
        payload_factory_map_[identifier] = factory_function;
    }
    else
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "Incorrect input to RegisterPayloadFactory");
    }
}

SignalFactory::SignalFactoryFunction SignalFactory::GetLegacySignalFactoryFunction(std::string oid)
{
    SignalFactoryFunction return_function = nullptr;

    //TODO:not a very efficient solution, as we serialize read access. If it turns
    //     out to be a problem, consider some multiple read single write pattern
    //     in the absence of a shared mutex
    std::lock_guard<std::mutex> lock(oid_factory_map_mutex_);

    //find factory for the given oid
    auto iterator = oid_factory_map_.find(oid);
    if (iterator != oid_factory_map_.end())
    {
        return_function = iterator->second;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_WARN,
                        "Failed to find legacy factory for oid %s", oid.c_str());
    }

    return return_function;
}

SignalFactory::PayloadFactoryFunction SignalFactory::GetPayloadFactory(PayloadInterface::PayloadIdentifier identifier)
{
    PayloadFactoryFunction return_function = nullptr;

    std::lock_guard<std::mutex> lock(payload_factory_map_mutex_);

    //find factory for the given PayloadIdentifier
    auto iterator = payload_factory_map_.find(identifier);
    if (iterator != payload_factory_map_.end())
    {
        return_function = iterator->second;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_WARN,
                        "Failed to find payload factory for oid %s", identifier.oid.c_str());
    }

    return return_function;
}




SignalFactory::VDSSignalVdmsgFunction SignalFactory::GetVdmsgFactory(long serviceType)
{
     std::lock_guard<std::mutex> lock(serviceId_factory_map_mutex_);

    VDSSignalVdmsgFunction return_function = nullptr;

    auto iterator = serviceId_factory_map_.find(serviceType);
    if (iterator != serviceId_factory_map_.end())
    {
        return_function = iterator->second;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_WARN,
                        "Failed to find GetVdmsgFactory, serviceType= %d", serviceType);
    }

    return return_function;
}


std::shared_ptr<const CodecInterface> SignalFactory::GetCodec(PayloadInterface::CodecType codec_type)
{
    std::lock_guard<std::mutex> lock(codec_map_mutex_);

    std::shared_ptr<const CodecInterface> return_pointer = nullptr;

    // First try to provide a codec provided by client, if thsi fails
    // use SignalFactory defaults.
    auto iterator = codec_map_.find(codec_type);
    if (iterator != codec_map_.end())
    {
        return_pointer = iterator->second;
    }
    else
    {
        switch (codec_type)
        {
        case PayloadInterface::kCodecTypeLegacyCcm:
        case PayloadInterface::kCodecTypeCcm:
            return_pointer = ccm_codec_;
            break;
        case PayloadInterface::kCodecTypeMarben:
            return_pointer = marben_codec_;
            break;
        case PayloadInterface::kCodecTypeXml:
             return_pointer = xml_codec_;
             break;
        default:
            DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
                            "Failed to find codec for CodecType: %d", codec_type);
        }
    }

    return return_pointer;
}

std::shared_ptr<Signal> SignalFactory::DecodeCcm(const unsigned char* binary_data,
                                                 size_t num_bytes)
{
    std::shared_ptr<Signal> signal = nullptr;

    std::shared_ptr<PayloadInterface> payload = nullptr;

    if (ccm_codec_->Decode(payload, binary_data, num_bytes))
    {
        //legacy signal handling
        signal = std::dynamic_pointer_cast<Signal>(payload);

        if (signal == nullptr)
        {
            //new signal handling
        }
    }

    return signal;

}

std::shared_ptr<Signal> SignalFactory::DecodeService(const unsigned char* binary_data,
                                                 size_t num_bytes)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,"%s(): error, should not running here \n",
                __FUNCTION__);
#if 0
    DLT_LOG_STRING(dlt_libfsm, DLT_LOG_INFO,"DecodeService   entering~~~~~~~~~~~~:   %\n");

    asnContext context;

  // activate Trace Management and create a traceStream (optional)
    context.setEncodingTraceWhileEncoding(asnTRUE);
    context.setValueTraceWhileDecoding(asnTRUE);
    asnFileStream traceStream("/data/zhouyou.txt",          // file name
                                                    asnFSTREAM_TXT_WRITE );// write text stream
    context.setTraceStream(&traceStream);

    asnMemoryStream decoding(const_cast<unsigned char*>(binary_data),           // memory area
                                                num_bytes, asnFSTREAM_READ); // read binary stream

    asn_wrapper::VDServiceRequest  *decPtr = new asn_wrapper::VDServiceRequest();
    try{
        decPtr->PERdecode(&context, &decoding);
    }
    catch(asnException e)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "%s(), error!!! catch asnException, e.getLocation() = %s",
                    __FUNCTION__, e.getLocation());
        return nullptr;
    }
    catch(std::exception e)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "%s(), error!!! catch std::exception, e.what() = %s",
                    __FUNCTION__, e.what());
        return nullptr;
    }
    catch(...)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "%s(), error!!! catch unknow exception",
                    __FUNCTION__);
        return nullptr;
    }

    VdServiceTransactionId vdsTransId;
    std::shared_ptr<Signal> signal = nullptr;
    if(!decPtr->body)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
                                    "DecodeService   decPtr->body  nullptr, %s", __FUNCTION__);
        return nullptr;

    }
    VDSSignalVdmsgFunction return_function = GetVdmsgFactory(decPtr->body->serviceId);
    if (return_function == nullptr)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
                                    "DecodeService   return_function is nullptr, %s", __FUNCTION__);
        return nullptr;
    }

    signal = return_function(vdsTransId,decPtr);

    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO,
                                        "DecodeService   xmlBuff :   %ld ",decPtr->body->serviceId);

    if(signal)
    {
          DLT_LOG_STRING(dlt_libfsm, DLT_LOG_INFO,
                                  "DecodeService  signal is not null");
    }
    return signal;
#endif
    return nullptr;
}

std::shared_ptr<Signal> SignalFactory::DecodeServiceV1(const unsigned char* binary_data,
                                                 size_t num_bytes)
{

    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO,"%s()   entering~~~~~~~~~~~~:   %\n",
                __FUNCTION__);

    asnContext context;

  // activate Trace Management and create a traceStream (optional)
    context.setEncodingTraceWhileEncoding(asnTRUE);
    context.setValueTraceWhileDecoding(asnTRUE);
    asnFileStream traceStream("/data/zhouyou.txt",          // file name
                                                    asnFSTREAM_TXT_WRITE );// write text stream
    context.setTraceStream(&traceStream);

    asnMemoryStream decoding(const_cast<unsigned char*>(binary_data),           // memory area
                                                num_bytes, asnFSTREAM_READ); // read binary stream

    asn_wrapper::VDServiceRequest  *decPtr = new asn_wrapper::VDServiceRequest();
    try{
        decPtr->PERdecode(&context, &decoding);
    }
    catch(asnException e)
    {
        char * location = context.getErrorLocation();
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, 
                      "%s(), error!!! catch asnException, Exception %s in %s \n",
                      __FUNCTION__, context.getErrorText(e.errorCode), location);
        asnfree(location);
        return nullptr;
    }
    catch(std::exception e)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "%s(), error!!! catch std::exception, e.what() = %s",
                    __FUNCTION__, e.what());
        return nullptr;
    }
    catch(...)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "%s(), error!!! catch unknow exception",
                    __FUNCTION__);
        return nullptr;
    }

    VdServiceTransactionId vdsTransId;
    std::shared_ptr<Signal> signal = nullptr;
    if(!decPtr->body)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
                                    "DecodeService   decPtr->body  nullptr, %s", __FUNCTION__);
        return nullptr;

    }
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG, "%s() decode success:   %\n",
                __FUNCTION__);


    int serviceType =decPtr->body->serviceId;
    CreateSignalFunction create_signal_function = nullptr;
    {
      std::lock_guard<std::mutex> lock(signal_factory_mutex_);
      auto iterator = signal_factory_map_.find(serviceType);
      if (iterator == signal_factory_map_.end())
      {
           DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_WARN,
                          "Failed to find signal_factory_map_, serviceType= %d", serviceType);
           return nullptr;
      }
      create_signal_function = iterator->second;
    }

    std::shared_ptr<asn_wrapper::VDServiceRequest> auto_del_decPtr(decPtr);
    signal = create_signal_function(vdsTransId, (void *)auto_del_decPtr.get());

    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG,
                                        "%S()   xmlBuff :   %ld ",
                                        __FUNCTION__,
                                        decPtr->body->serviceId);

    if(signal == nullptr)
    {
      DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
                              "%s()  signal is null",
                              __FUNCTION__);
      return nullptr;
    }
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG,
                            "%s  signal is not null",
                            __FUNCTION__);
    return signal;

}

std::shared_ptr<asn_wrapper::VDServiceRequest> SignalFactory::DecodeVdService(const unsigned char* binary_data,
                                                 size_t num_bytes)
{

    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG, "%s()   entering~~~~~~~~~~~~:   %\n",
                __FUNCTION__);

    std::shared_ptr<asn_wrapper::VDServiceRequest> return_value;
    
    asnContext context;

  // activate Trace Management and create a traceStream (optional)
    context.setEncodingTraceWhileEncoding(asnTRUE);
    context.setValueTraceWhileDecoding(asnTRUE);
    asnFileStream traceStream("/data/zhouyou.txt",          // file name
                                                    asnFSTREAM_TXT_WRITE );// write text stream
    context.setTraceStream(&traceStream);

    asnMemoryStream decoding(const_cast<unsigned char*>(binary_data),           // memory area
                                                num_bytes, asnFSTREAM_READ); // read binary stream

    auto decPtr = std::make_shared<asn_wrapper::VDServiceRequest>();
    if (decPtr == nullptr)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, 
                      "%s(), new decPtr failed \n",
                      __FUNCTION__);
        return return_value;
    }
    try{
        decPtr->PERdecode(&context, &decoding);
    }
    catch(asnException e)
    {
        char * location = context.getErrorLocation();
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, 
                      "%s(), error!!! catch asnException, Exception %s in %s \n",
                      __FUNCTION__, context.getErrorText(e.errorCode), location);
        asnfree(location);
        return return_value;
    }
    catch(std::exception e)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "%s(), error!!! catch std::exception, e.what() = %s",
                    __FUNCTION__, e.what());
        return return_value;
    }
    catch(...)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "%s(), error!!! catch unknow exception",
                    __FUNCTION__);
        return return_value;
    }

    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG, "%s() decode success:   %\n",
                __FUNCTION__);

    return_value = decPtr;
    return return_value;

}

SignalFactory::t_factory_return SignalFactory::DecodeServiceV2(const unsigned char* binary_data,
                                                 size_t num_bytes)
{

    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO,"%s()   entering~~~~~~~~~~~~:   %\n",
                __FUNCTION__);

    SignalFactory::t_factory_return return_value;
    
    auto decPtr = SignalFactory::DecodeVdService(binary_data, num_bytes);
    if(decPtr == nullptr)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
                                    "%s()   decPtr is  nullptr", 
                                    __FUNCTION__);
        return return_value;
    }

    VdServiceTransactionId vdsTransId;
    std::shared_ptr<Signal> signal = nullptr;
    if( (decPtr->body == NULL) || 
        (decPtr->optional.getPresence(asn_wrapper::asn_body) == false) )
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
                                    "DecodeService   decPtr->body  nullptr, %s", __FUNCTION__);
        return return_value;

    }
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG, "%s() decode success:   %\n",
                __FUNCTION__);


    int serviceType =decPtr->body->serviceId;
    CreateSignalFunction create_signal_function = nullptr;
    {
      std::lock_guard<std::mutex> lock(signal_factory_mutex_);
      auto iterator = signal_factory_map_.find(serviceType);
      if (iterator == signal_factory_map_.end())
      {
           DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_WARN,
                          "Failed to find signal_factory_map_, serviceType= %d", serviceType);
           return return_value;
      }
      create_signal_function = iterator->second;
    }

    signal = create_signal_function(vdsTransId, (void *)decPtr.get());

    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG,
                                        "%S()   xmlBuff :   %ld ",
                                        __FUNCTION__,
                                        decPtr->body->serviceId);

    if(signal == nullptr)
    {
      DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
                              "%s()  signal is null",
                              __FUNCTION__);
      return return_value;
    }
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG,
                            "%s  signal is not null",
                            __FUNCTION__);

    auto copyed_vd_service = SignalFactory::DecodeVdService(binary_data, num_bytes);
    if(copyed_vd_service == nullptr)
    {
      DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
                              "%s()  copyed_vd_service is null",
                              __FUNCTION__);
      return return_value;
    }

    return_value.ret_signal = signal;
    return_value.copyed_vd_service = copyed_vd_service;
    return_value.service_id = serviceType;
    return_value.error_code = 0;
    return return_value;

}


std::shared_ptr<std::vector<unsigned char> > SignalFactory::Encode(std::shared_ptr<const PayloadInterface> payload,
                                                                  fs_Encoding* used_encoding,
                                                                  int* applied_encode_flags)
{
    std::shared_ptr<std::vector<unsigned char> > return_buffer = nullptr;
    std::shared_ptr<const CodecInterface> codec = nullptr;

    if (payload)
    {
        std::vector<PayloadInterface::CodecType> supported_codecs = payload->GetSupportedCodecs();

        if (supported_codecs.size() > 0)
        {
            codec = GetCodec(supported_codecs[0]);
        }
    }
    else
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR,
                       "SignalFactory asked to encode null payload");
    }

    if (!codec)
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR,
                       "SignalFactory failed to find codec for encode");
    }

    if (codec && payload)
    {
        if (!codec->Encode(return_buffer, used_encoding, applied_encode_flags, payload))
        {
            DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR,
                           "SignalFactory codec failed to encode payload");

            return_buffer = nullptr;
        }
    }

    return return_buffer;
}


std::shared_ptr<std::vector<unsigned char> > SignalFactory::GeelyEncode(std::shared_ptr<fsm::SignalPack> signal,
                                                                    bool is_encode_to_xml,
                                                                  fs_Encoding* used_encoding,
                                                                  int* applied_encode_flags)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,"%s(): error, should not running here \n",
                __FUNCTION__);
#if 0
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG,
                           "%s___, is_encode_to_xml = %d", __FUNCTION__, is_encode_to_xml);

    int ret=0;

    std::shared_ptr<std::vector<unsigned char> > return_buffer;

    std::shared_ptr<t_geely_data> data= std::make_shared<t_geely_data>();
    if (data.get() == NULL)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
                               "%s(), data.get() == nullptr", __FUNCTION__);
        return return_buffer;
    }

    asn_wrapper::VDServiceRequest *vdmsg = (asn_wrapper::VDServiceRequest *)signal->GetPackPayload();
    if (!vdmsg)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
                               "%s___, GetPackPayload return NULL ", __FUNCTION__);
        return return_buffer;
    }
    ret = serialize_gly_asn_2_buf(vdmsg, data);
    if (ret != 0)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
                               "%s___, call serialize_gly_asn_2_buf failed = %d", __FUNCTION__, ret);
        return return_buffer;
    }

    // asn_DEF_VDServiceRequest.free_struct(&asn_DEF_VDServiceRequest, vdmsg, 0);
    // vdmsg = NULL;
    // DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO,
    //                "%s(): freeed vdmsg", __FUNCTION__);

    return_buffer = std::make_shared<std::vector<unsigned char> >();
    if (return_buffer.get() == NULL)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
                               "%s(), alloc return_buffer failed", __FUNCTION__);
        return return_buffer;
    }

    return_buffer->resize(data->num_bytes);

    std::memcpy(return_buffer->data(), data->data, data->num_bytes);

    // std::string xer_str;
    // ret = encode_buf_to_xer((unsigned char *)return_buffer->data(), data->num_bytes, xer_str);
    // if (ret != 0)
    // {
    //     DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
    //                                 "%s(), encode_buf_to_xer error ", __FUNCTION__);
    // }
    // DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG,
    //                                 "%s(), encode_buf_to_xer = %s", __FUNCTION__, xer_str.c_str());

    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO,
                           "%s___, return buffer size is %d", __FUNCTION__, return_buffer->size());
    return return_buffer;
#endif
    return nullptr;
}

std::shared_ptr<std::vector<unsigned char> > SignalFactory::GeelyEncodeV1(std::shared_ptr<fsm::SignalPack> signal,
                                                                    bool is_encode_to_xml,
                                                                  fs_Encoding* used_encoding,
                                                                  int* applied_encode_flags)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG,
                           "%s___, is_encode_to_xml = %d", __FUNCTION__, is_encode_to_xml);

    int ret=0;

    std::shared_ptr<std::vector<unsigned char> > return_buffer;

    std::shared_ptr<t_geely_data> data= std::make_shared<t_geely_data>();
    if (data.get() == NULL)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
                               "%s(), data.get() == nullptr", __FUNCTION__);
        return return_buffer;
    }
    void *void_msg = NULL;
    auto vdmsg = std::make_shared<asn_wrapper::VDServiceRequest>();
    if (vdmsg == nullptr)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
                               "%s___, GetPackPayload return NULL ", __FUNCTION__);
        return return_buffer;
    }
    ret = signal->PackGeelyAsn(vdmsg.get());
    if (ret != 0)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
                               "%s(), signal->PackGeelyAsn step 1 failed ", __FUNCTION__);
        void *pMsg = signal->PackGeelyAsn();
        if(pMsg == NULL)
        {
            DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
                                   "%s(), signal->PackGeelyAsn step 2 failed ", __FUNCTION__);
            return return_buffer;
        }
        else
        {
            void_msg = pMsg;
            DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO,
                                   "%s(), signal->PackGeelyAsn step 2 success ", __FUNCTION__);
        }
    }
    else
    {
        void_msg = (void *)vdmsg.get();
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG,
                               "%s(), signal->PackGeelyAsn step 1 success ", __FUNCTION__);
    }
    //lijing-test
    SignalFactory::print_vd_service((asn_wrapper::VDServiceRequest *)void_msg);
    ret = serialize_gly_asn_2_buf((void *)void_msg, data);
    if (ret != 0)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
                               "%s___, call serialize_gly_asn_2_buf failed = %d", __FUNCTION__, ret);
        return return_buffer;
    }

    // asn_DEF_VDServiceRequest.free_struct(&asn_DEF_VDServiceRequest, vdmsg, 0);
    // vdmsg = NULL;
    // DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO,
    //                "%s(): freeed vdmsg", __FUNCTION__);

    return_buffer = std::make_shared<std::vector<unsigned char> >();
    if (return_buffer.get() == NULL)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
                               "%s(), alloc return_buffer failed", __FUNCTION__);
        return return_buffer;
    }

    return_buffer->resize(data->num_bytes);

    std::memcpy(return_buffer->data(), data->data, data->num_bytes);

    // std::string xer_str;
    // ret = encode_buf_to_xer((unsigned char *)return_buffer->data(), data->num_bytes, xer_str);
    // if (ret != 0)
    // {
    //     DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
    //                                 "%s(), encode_buf_to_xer error ", __FUNCTION__);
    // }
    // DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG,
    //                                 "%s(), encode_buf_to_xer = %s", __FUNCTION__, xer_str.c_str());

    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO,
                           "%s___, return buffer size is %d", __FUNCTION__, return_buffer->size());
    return return_buffer;
}

std::shared_ptr<std::vector<unsigned char> > SignalFactory::GlySmsEncode(std::shared_ptr<fsm::SignalPack> signal,
                                                                    fs_Encoding* used_encoding,
                                                                    int* applied_encode_flags)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,"%s(): error, should not running here \n",
                __FUNCTION__);

#if 0
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG,
                           "%s()___", __FUNCTION__);

    int ret=0;

    std::shared_ptr<std::vector<unsigned char> > return_buffer;
    std::shared_ptr<std::vector<unsigned char> > ret_error_buf;

    std::shared_ptr<t_geely_data> data= std::make_shared<t_geely_data>();
    if (data.get() == NULL)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
                               "%s(), data.get() == nullptr", __FUNCTION__);
        return return_buffer;
    }

    asn_wrapper::VDServiceRequest *vdmsg = (asn_wrapper::VDServiceRequest *)signal->GetPackPayload();
    if (!vdmsg)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
                               "%s___, GetPackPayload return NULL ", __FUNCTION__);
        return return_buffer;
    }
    ret = serialize_gly_asn_2_buf(vdmsg, data);
    if (ret != 0)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
                               "%s___, call serialize_gly_asn_2_buf failed = %d", __FUNCTION__, ret);
        return return_buffer;
    }

    size_t protocol_id_len = 3;
    size_t service_id_len = 3;
    size_t buffer_len = protocol_id_len + service_id_len + data->num_bytes;
    return_buffer = std::make_shared<std::vector<unsigned char> >(buffer_len);
    if (return_buffer.get() == NULL)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
                               "%s(), alloc return_buffer failed", __FUNCTION__);
        return ret_error_buf;
    }

    size_t off_set = 0;

    //protocol id "VDA"
    (*return_buffer)[off_set++] = 'V';
    (*return_buffer)[off_set++] = 'D';
    (*return_buffer)[off_set++] = 'A';

    //SERVICE ID, 3 letters
    if (vdmsg->body == NULL)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
                               "%s(), (vdmsg->body == NULL)", __FUNCTION__);
        return ret_error_buf;
    }
    std::string str_service_id = VdsSignalType::toString(vdmsg->body->serviceId);
    if(str_service_id.empty() || str_service_id.length() != 3)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "%s(), str_service_id is incorrect, %s, %d",
                                __FUNCTION__, str_service_id.c_str(), vdmsg->body->serviceId);
        return ret_error_buf;
    }
    (*return_buffer)[off_set++] = str_service_id[0];
    (*return_buffer)[off_set++] = str_service_id[1];
    (*return_buffer)[off_set++] = str_service_id[2];

    //PAYLOAD
    std::memcpy(return_buffer->data() + off_set, data->data, data->num_bytes);
    off_set += data->num_bytes;

    if (off_set != return_buffer->size())
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
                               "%s(), off_set != return_buffer.size()", __FUNCTION__);
        return ret_error_buf;
    }

    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO,
                           "%s___, return buffer size is %d", __FUNCTION__, return_buffer->size());
    return return_buffer;
#endif
    return nullptr;

}

std::shared_ptr<std::vector<unsigned char> > SignalFactory::GlySmsEncodeV1(std::shared_ptr<fsm::SignalPack> signal,
                                                                    fs_Encoding* used_encoding,
                                                                    int* applied_encode_flags)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG,
                           "%s()___", __FUNCTION__);

    int ret=0;

    std::shared_ptr<std::vector<unsigned char> > return_buffer;
    std::shared_ptr<std::vector<unsigned char> > ret_error_buf;

    std::shared_ptr<t_geely_data> data= std::make_shared<t_geely_data>();
    if (data.get() == NULL)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
                               "%s(), data.get() == nullptr", __FUNCTION__);
        return return_buffer;
    }

    asn_wrapper::VDServiceRequest *void_msg = NULL;
    auto vdmsg = std::make_shared<asn_wrapper::VDServiceRequest>();
    if (vdmsg == nullptr)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
                               "%s___, GetPackPayload return NULL ", __FUNCTION__);
        return return_buffer;
    }
    ret = signal->PackGeelyAsn(vdmsg.get());
    if (ret != 0)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
                               "%s(), signal->PackGeelyAsn step 1 failed ", __FUNCTION__);
        void *pMsg = signal->PackGeelyAsn();
        if(pMsg == NULL)
        {
            DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
                                   "%s(), signal->PackGeelyAsn step 2 failed ", __FUNCTION__);
            return return_buffer;
        }
        else
        {
            void_msg = (asn_wrapper::VDServiceRequest *)pMsg;
            DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO,
                                   "%s(), signal->PackGeelyAsn step 2 success ", __FUNCTION__);
        }
    }
    else
    {
        void_msg = vdmsg.get();
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG,
                               "%s(), signal->PackGeelyAsn step 1 success ", __FUNCTION__);
    }
    SignalFactory::print_vd_service((asn_wrapper::VDServiceRequest *)void_msg);
    ret = serialize_gly_asn_2_buf((void *)void_msg, data);
    if (ret != 0)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
                               "%s___, call serialize_gly_asn_2_buf failed = %d", __FUNCTION__, ret);
        return return_buffer;
    }

    size_t protocol_id_len = 3;
    size_t service_id_len = 3;
    size_t buffer_len = protocol_id_len + service_id_len + data->num_bytes;
    return_buffer = std::make_shared<std::vector<unsigned char> >(buffer_len);
    if (return_buffer.get() == NULL)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
                               "%s(), alloc return_buffer failed", __FUNCTION__);
        return ret_error_buf;
    }

    size_t off_set = 0;

    //protocol id "VDA"
    (*return_buffer)[off_set++] = 'V';
    (*return_buffer)[off_set++] = 'D';
    (*return_buffer)[off_set++] = 'I';

    //SERVICE ID, 3 letters
    int tmp_service_id = -1;
    bool is_body_invalid = false;
    if ( (void_msg->optional.getPresence(asn_body) == false)
        || (void_msg->body == NULL) )
    {
        is_body_invalid = false;
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG,
                               "%s(), error, body is invalid", __FUNCTION__);
    }
    else
    {
        is_body_invalid = true;
    }
    
    if (is_body_invalid == false)
    {
        tmp_service_id = signal->get_service_id();
        if (tmp_service_id == -1)
        {
            DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
                                   "%s(), get service_id failed", __FUNCTION__);
            return ret_error_buf;
        }
        else
        {
        }
    }
    else
    {
        tmp_service_id = void_msg->body->serviceId;
    }
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG,
                           "%s(), get service_id:%d", __FUNCTION__, tmp_service_id);
                           
    std::string str_service_id = VdsSignalType::toString(tmp_service_id);
    if(str_service_id.empty())
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "%s(), str_service_id is incorrect, %s, %d",
                                __FUNCTION__, str_service_id.c_str(), tmp_service_id);
        return ret_error_buf;
    }
    (*return_buffer)[off_set++] = str_service_id[0];
    (*return_buffer)[off_set++] = str_service_id[1];
    (*return_buffer)[off_set++] = str_service_id[2];

    //PAYLOAD
    std::memcpy(return_buffer->data() + off_set, data->data, data->num_bytes);
    off_set += data->num_bytes;

    if (off_set != return_buffer->size())
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
                               "%s(), off_set != return_buffer.size()", __FUNCTION__);
        return ret_error_buf;
    }

    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO,
                           "%s___, return buffer size is %d", __FUNCTION__, return_buffer->size());
    return return_buffer;
}

std::shared_ptr<PayloadInterface> SignalFactory::DecodePayload(const unsigned char* binary_data,
                                                               size_t num_bytes,
                                                               const PayloadInterface::PayloadIdentifier* identifier,
                                                               const fs_Encoding* encoding,
                                                               const fs_VersionInfo* version)
{
    std::shared_ptr<PayloadInterface> payload = nullptr;

    if (identifier)
    {
        PayloadFactoryFunction factory = GetPayloadFactory(*identifier);

        if (factory)
        {
            payload = factory();
        }

        if (payload)
        {
            std::shared_ptr<const CodecInterface> codec = nullptr;

            std::vector<PayloadInterface::CodecType> supported_codecs;
            supported_codecs = payload->GetSupportedCodecs();

            for (auto supported_codec : supported_codecs)
            {
                codec = GetCodec(supported_codec);

                DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO,
                                "SignalFactory attempting decode of payload of size %u "
                                "using codec for codec type: %d",
                                num_bytes, supported_codec);

                if(codec)//modify klocwork warning,yangjun add 20181107
                {
                    // attempt to decode using supported codecs until we success.
                    if (codec->Decode(payload, binary_data, num_bytes, encoding, version))
                    {
                        break;
                    }
                    else
                    {
                        payload = nullptr;
                        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_WARN,
                                        "Decode of payload usiong codec for codec type: %d failed",
                                        supported_codec);
                    }
                }
            }
        }
    }
    else
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_WARN,
                       "SignalFactory currently does not support deciding unidentified payloads");

        // TODO: try to deduce
        //       attempt to decode as ccm
        //       attempt to decode as xml
    }

    return payload;
}

void SignalFactory::print_vd_service(asn_wrapper::VDServiceRequest * vd_service)
{
  if (vd_service == NULL)
  {
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG
                  , "SignalFactory::%s(), lijing-test , vd_service is NULL"
                  , __FUNCTION__);
    return;
  }
  if (vd_service->header == NULL)
  {
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG
                  , "SignalFactory::%s(), lijing-test , header is NULL"
                  , __FUNCTION__);
    return;
  }
  DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG
                , "SignalFactory::%s(), lijing-test , requestid=%u"
                , __FUNCTION__, vd_service->header->requestid);

  DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG
                , "SignalFactory::%s(), lijing-test , timeStamp->seconds=%u"
                , __FUNCTION__, vd_service->header->timeStamp->seconds);
  DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG
                , "SignalFactory::%s(), lijing-test , timeStamp->milliseconds=%u"
                , __FUNCTION__, vd_service->header->timeStamp->milliseconds);

  if (vd_service->header->optional.getPresence(asn_wrapper::asn_eventId) == true)
  {
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG
                  , "SignalFactory::%s(), lijing-test , eventId->seconds=%u"
                  , __FUNCTION__, vd_service->header->eventId->seconds);
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG
                  , "SignalFactory::%s(), lijing-test , eventId->milliseconds=%u"
                  , __FUNCTION__, vd_service->header->eventId->milliseconds);
  }
  
  DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG
                , "SignalFactory::%s(), lijing-test , createId=%u"
                , __FUNCTION__, vd_service->header->creatorId);
  
  DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG
                , "SignalFactory::%s(), lijing-test , messageTTL=%u"
                , __FUNCTION__, vd_service->header->messageTTL);
                
  DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG
                , "SignalFactory::%s(), lijing-test , requestType=%u"
                , __FUNCTION__, vd_service->header->requestType);
                
  DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG
                , "SignalFactory::%s(), lijing-test , ackRequired=%u"
                , __FUNCTION__, vd_service->header->ackRequired);
  
  return;
}

#ifdef ENABLE_SIGNAL_INJECTION

bool DeserializeResHornNLight(std::stringstream& stream, HornNLightSignal* msg)
{
    bool success = false;

    if (msg)
    {

        std::map<std::string, vc::HNLReturnCode> hnl_return_code_map = {
                            std::make_pair("HL_OK", vc::HL_OK),
                            std::make_pair("HL_SEQUENCE_ONGOING", vc::HL_SEQUENCE_ONGOING),
                            std::make_pair("HL_WRONG_USAGE_MODE", vc::HL_WRONG_USAGE_MODE)
        };

        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_INFO, "Deserializing ResHornNLight");

        try
        {
            std::string hnl_return_code;

            std::getline(stream, hnl_return_code, ',');

            vc::ResHornNLight res_horn_n_light;

            res_horn_n_light.return_code = hnl_return_code_map[hnl_return_code];
            msg->SetData(res_horn_n_light);

            success = true;
        }
        catch (const std::exception& e)
        {
            DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "Exception for VehicleComm "
                            "ResHornNLight deserializer -> %s", e.what());
        }
    }

    return success;
}

std::shared_ptr<Signal> DeserializeVehicleCommSignal(std::stringstream& stream, const std::string& transaction_id_str)
{
    std::map<std::string, int> msg_id_map = {
                        std::make_pair("kHornNLight", Signal::kHornNLight)
    };

    std::map<std::string, vc::ReturnValue> vc_ret_val_map = {
                        std::make_pair("RET_OK", vc::RET_OK),
                        std::make_pair("RET_ERR_INVALID_ARG", vc::RET_ERR_INVALID_ARG),
                        std::make_pair("RET_ERR_EXTERNAL", vc::RET_ERR_EXTERNAL),
                        std::make_pair("RET_ERR_INTERNAL", vc::RET_ERR_INTERNAL),
                        std::make_pair("RET_ERR_OUT_OF_MEMORY", vc::RET_ERR_OUT_OF_MEMORY),
                        std::make_pair("RET_ERR_TIMEOUT", vc::RET_ERR_TIMEOUT),
                        std::make_pair("RET_ERR_BUSY", vc::RET_ERR_BUSY),
                        std::make_pair("RET_ERR_VALUE_NOT_AVAILABLE", vc::RET_ERR_VALUE_NOT_AVAILABLE),
                        std::make_pair("RET_ERR_INVALID_STATE", vc::RET_ERR_INVALID_STATE)
    };

    std::shared_ptr<Signal> signal = nullptr;

    long transaction_id;

    std::string msg_id_string;
    std::string vc_ret_val_string;

    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "Deserialize VehicleCom from %s", stream.str().c_str());

    try
    {
        std::stringstream transaction_id_stream(transaction_id_str);

        transaction_id_stream >> transaction_id;
        VehicleCommTransactionId vehicle_comm_transaction_id(transaction_id);

        std::getline(stream, msg_id_string, ',');
        std::getline(stream, vc_ret_val_string, ',');
        vc::ReturnValue vc_ret_val = vc_ret_val_map[vc_ret_val_string];

        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "Deserialize VehicleCom "
                                               "(transaction_id: %u"
                                               " id_: %s"
                                               " vc_ret_val: %s"
                                               ")",
                                               static_cast<unsigned>(transaction_id),
                                               msg_id_string.c_str(),
                                               vc_ret_val_string.c_str());


        bool success = false;

        if (msg_id_string == "kHornNLight")
        {
            vc::ResHornNLight res_horn_n_light = {vc::HL_COMMUNICATION_ERROR} ;  // create empty dummy structure, which will be filled in DeserializeResHornNLight
            HornNLightSignal* msg = new HornNLightSignal(Signal::kHornNLight, vehicle_comm_transaction_id,\
                res_horn_n_light, vc_ret_val);  // preliminary fill with no data, this is set in DeserializeResHornNLight
            success = DeserializeResHornNLight(stream, msg);
            signal = std::shared_ptr<Signal>(msg);
        }

        if (!success)
        {
            DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "Failed to deserialize for %s", msg_id_string.c_str());
            signal = nullptr;
        }
    }
    catch(const std::exception& e)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "Exception for VehicleComm Signal deserializer -> %s", e.what());
    }

    return signal;
}


std::shared_ptr<Signal> SignalFactory::CreateSignalFromVehicleCommSerializedString(const char* serialized_string,
                                                                                   const size_t num_bytes,
                                                                                   const std::string& transaction_id)
{
    std::shared_ptr<Signal> signal = nullptr;

    if (serialized_string
        && num_bytes)
    {
        std::stringstream serialized_stream;
        serialized_stream.rdbuf()->sputn(serialized_string, num_bytes);

        signal = DeserializeVehicleCommSignal(serialized_stream, transaction_id);

    }

    return signal;
}

std::shared_ptr<Signal> SignalFactory::CreateSignalFromIpcb(const char* file_contents,
                                                            const size_t num_bytes,
                                                            const std::string transaction_id_str)
{
    std::shared_ptr<Signal> return_signal;

    std::istringstream contents_iss(std::string(file_contents, num_bytes));

    int type;
    contents_iss >> type;
    Signal::SignalType signal_type = static_cast<Signal::SignalType>(type);

    switch (signal_type)
    {
    case Signal::kPrivacyNotificationSignal :
    {
        uint64_t request_id = IpCommandBrokerTransactionId::kUndefinedTransactionId;
        bool car_stat_upload = false;
        bool location_services = false;
        contents_iss >> request_id >> std::boolalpha >> car_stat_upload >> location_services;

        IpCommandBrokerTransactionId transaction_id(request_id);
        PrivacyNotificationType payload = {car_stat_upload, location_services};

        return_signal =
            PrivacyNotificationSignal::CreateIpCommandBrokerSignal(payload,
                                                                   transaction_id,
                                                                   signal_type);
        break;
    }
    case Signal::kDeadReckonedPositionSignal:
    {
        Signal::SignalType signal_type = Signal::kDeadReckonedPositionSignal;
        IpCommandBrokerTransactionId transaction_id(transaction_id_str);
        // ToDo: perhaps read out the following data from a file
        // quick and dirty way to populate payload (i.e. ignore file contents):
        DeadReckonedPositionResponse dr_resp;
        memset(&dr_resp, 0, sizeof(dr_resp));
        dr_resp.position.noValidData = 0;  // means valid
        // infotainmentIpBus.asn: Coordinates in degrees/360*2^32. Positive values are North and East
        dr_resp.position.drPosition.longlat.longitude = 3611111;  // static_cast<int32_t>( 13.0f / 360.0 * 100000000);
        dr_resp.position.drPosition.longlat.latitude = 25000000;  // static_cast<int32_t>( 90.0f / 360.0 * 100000000);
        dr_resp.position.drPosition.heading = 99;
        dr_resp.position.drPosition.speedKmph = 181;
        dr_resp.position.drPosition.hdopX10 = 12;
        dr_resp.position.drPosition.numSat = 5;
        dr_resp.position.drPosition.fixTime.year = 2017;
        dr_resp.position.drPosition.fixTime.month = 2;
        dr_resp.position.drPosition.fixTime.day = 29;
        dr_resp.position.drPosition.fixTime.hour = 12;
        dr_resp.position.drPosition.fixTime.minute = 34;
        dr_resp.position.drPosition.fixTime.second = 56;
        dr_resp.position.drPosition.fixType = ::fix3D;
        dr_resp.position.drPosition.drType = ::drMapMatched;
        dr_resp.position.drPosition.drDistance = 42;
        dr_resp.request_id = transaction_id.GetId();
        DeadReckonedPositionSignal::CreateIpCommandBrokerSignal(dr_resp.position,
                                                                transaction_id,
                                                                signal_type);
        break;
    }
    default:
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "Found unexpected ipcb signal type: %i", type);
    }

    return return_signal;
}


#endif

} // namespace fsm
/** \}    end of addtogroup */
