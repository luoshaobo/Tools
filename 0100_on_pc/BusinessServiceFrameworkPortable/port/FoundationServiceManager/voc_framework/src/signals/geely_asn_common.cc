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

#include <cstring>
#include <functional>
#include <map>
#include <sstream>
#include <string>
#include <sstream>

#include "dlt/dlt.h"

#include "voc_framework/signals/geely_asn_common.h"

#include "asn.h"
#include "VDSasn.h"

DLT_IMPORT_CONTEXT(dlt_libfsm);

namespace fsm
{
int serialize_gly_asn_2_buf(void * in_vds_sr, std::shared_ptr<t_geely_data> out_data)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG, "%s(), entered", __FUNCTION__);
    asn_wrapper::VDServiceRequest * vds_request = (asn_wrapper::VDServiceRequest *) in_vds_sr;
    if(vds_request == NULL)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "%s(), error!!! (vds_request == NULL)", __FUNCTION__);
        return -1;
    }
    if(out_data.get() == NULL)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "%s(), error!!! (out_data.get() == NULL)", __FUNCTION__);
        return -1;
    }

    //lijing-test, todo, should mutex?
    asnContext context;
    context.setAbstractSyntax(&AbstractSyntax);
    context.setEncodingTraceWhileEncoding(asnTRUE); //lijing-test
    context.setValueTraceWhileEncoding(asnTRUE);
    //context.setValueTraceWhileDecoding(asnTRUE);
    //context.setEncodingTraceWhileDecoding(asnTRUE);
    context.setIsHexaEncodingTrace(asnTRUE);
    context.setConstraintCheckDuringEncoding(asnTRUE);
    //context.setConstraintCheckDuringDecoding(asnTRUE);
    //context.setAutomaticDecoding(asnTRUE);
    context.setAsciiEncodingTrace(asnTRUE);	
    asnFileStream traceStream("/data/zhouyou_encode.txt",          // file name
                                                    asnFSTREAM_TXT_WRITE );// write text stream
    context.setTraceStream(&traceStream);
    asnMemoryStream encoding(asnFSTREAM_WRITE); // write binary stream

    asnMAXUINT len = 0;
    // encode myValue into the encoding stream
    try{
        len = vds_request->PERencode(&context, &encoding);

    }
    catch(asnException e)
    {
      char * location = context.getErrorLocation();
      DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, 
                    "%s(), error!!! catch asnException, Exception %s in %s \n",
                    __FUNCTION__, context.getErrorText(e.errorCode), location);
      asnfree(location);
      return -1;
    }
    catch(std::exception e)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "%s(), error!!! catch std::exception, e.what() = %s",
                    __FUNCTION__, e.what());
        return -1;
    }
    catch(...)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "%s(), error!!! catch unknow exception",
                    __FUNCTION__);
        return -1;
    }

    if(len <= 0)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "%s(), error!!! (len <= 0), len = %d",
                    __FUNCTION__, len);
        return -1;
    }
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG, "%s(), decode lenth is: %d",
            __FUNCTION__, len);

    // get the buffer from encoding stream
    asnbytep buffer = encoding.detach();
    t_auto_free_vdrequest auto_free((void **)&buffer);
    out_data->data = calloc(1, len);
    if (out_data->data == NULL)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "%s(), error!!! (out_data->data == NULL)",
                    __FUNCTION__);
        return -1;
    }
    out_data->num_bytes = len;
    std::memcpy((void *)out_data->data, buffer, out_data->num_bytes);
    return 0;
    
}

t_auto_free_vdrequest::t_auto_free_vdrequest(void **p) : m_p(p)
{
}

t_auto_free_vdrequest::~t_auto_free_vdrequest()
{
    asnbytep **tmp = (asnbytep **)m_p;
    if(*tmp)
    {
        asnfree(*tmp);
    }
}

#if 0

int geely_write_encoded_bytes(const void *buffer, size_t size, void *clientdata)
{
    if (!clientdata)
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "geely_write_encoded_bytes___, no client data");
        return -1;
    }

    t_geely_data* encode_buffer = (t_geely_data *) clientdata;
    if (!encode_buffer)
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "geely_write_encoded_bytes___, bad buffer");
        return -1;
    }

    if (size == 0)
    {
        // do nothing
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "geely_write_encoded_bytes___, asked to consume 0 encoded bytes");
        return 1;
    }

    void* result = 0;
    if (encode_buffer->num_bytes == 0)
    {
        encode_buffer->data = malloc((encode_buffer->num_bytes) + size);
        if (encode_buffer->data == 0)
        {
            DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "geely_write_encoded_bytes___, encode_buffer->data == 0");
            return -1;
        }
    }
    else
    {
        //TODO: consider a more intelligent realloc strategy
        result = realloc(encode_buffer->data, (encode_buffer->num_bytes) + size);
        if (result == 0)
        {
            DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "geely_write_encoded_bytes___, [error] realloc result == 0");
            if (encode_buffer->data)
            {
                free(encode_buffer->data);
            }
            encode_buffer->data = nullptr;
            encode_buffer->num_bytes = 0;
            return -1;
        }
        encode_buffer->data = result;
    }
    
    char* start = ((char*) encode_buffer->data) + encode_buffer->num_bytes;
    std::memcpy(start, buffer, size);

    encode_buffer->num_bytes = encode_buffer->num_bytes + size;

    return 1;
}



int serialize_gly_asn_2_xer_buf(void * in_vds_sr, std::shared_ptr<t_geely_data> out_data)
{
    asn_enc_rval_t ec = {};
    t_geely_data *data = out_data.get();
    data->data = 0;
    data->num_bytes = 0;
    ec = xer_encode(&asn_DEF_VDServiceRequest, (void *)in_vds_sr, XER_F_BASIC, &geely_write_encoded_bytes, data);
    if (ec.encoded == -1)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "%s(), [error], ec.encoded == -1", __FUNCTION__);
        free(data->data);
        data->data = NULL;
        data->num_bytes = 0;
        return -1;
    }

    return 0;
}

#endif

int encode_buf_to_xer(unsigned char * binary_data, size_t num_bytes, std::string & xer_str)
{
#if 0

    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG, "%s(), begin", __FUNCTION__);

    int ret =0;
    xer_str.clear();

    VDServiceRequest_t *out_vds_request = 0;
    t_auto_free_vdrequest auto_free((void **)&out_vds_request);
    asn_dec_rval_t  retVal = ber_decode(0, &asn_DEF_VDServiceRequest, (void **)&out_vds_request, 
                                                                binary_data, num_bytes);
  
    if(retVal.code != RC_OK)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "%s(), [error], ec.encoded == -1", __FUNCTION__);

        return -1;
    }

    std::shared_ptr<t_geely_data> data = std::make_shared<t_geely_data>();
    if (data == nullptr)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "%s(), data is nullptr", __FUNCTION__);
        return -1;
    }

    ret = serialize_gly_asn_2_xer_buf(out_vds_request, data);
    if (ret != 0)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "%s(), call serialize_gly_asn_2_buf failed = %d", __FUNCTION__, ret);
        return -1;
    }

    xer_str.resize(data->num_bytes + 1, 0);
    if(xer_str.length() < data->num_bytes)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "%s(), xer_str.length() < data->num_bytes, %d, %d", __FUNCTION__, xer_str.size(), data->num_bytes);
        return -1;
    }
    std::memcpy((void *)xer_str.data(), data->data, data->num_bytes);

    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG, "%s(), end", __FUNCTION__);
#endif
    return 0;
}


} // namespace fsm
/** \}    end of addtogroup */
