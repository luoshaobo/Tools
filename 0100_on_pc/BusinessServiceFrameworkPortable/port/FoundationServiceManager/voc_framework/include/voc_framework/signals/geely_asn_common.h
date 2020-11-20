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
 *  \file     signal_factory.h
 *  \brief    VOC Service signal factory
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_FRAMEWORK_SIGNALS_GEELY_ASN_COMMON_H_
#define VOC_FRAMEWORK_SIGNALS_GEELY_ASN_COMMON_H_

#include <memory>
#include <string>
#include <cstring>
#include <map>

namespace fsm
{

struct t_geely_data
{
    void* data; /**< raw data, must be freed when no longer needed */
    size_t num_bytes;  /**< number of bytes of data */

    t_geely_data()
    {
        data = NULL;
        num_bytes = 0;
    }
    bool operator==(const t_geely_data & other) const
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

    ~t_geely_data()
    {
        if (data)
        {
            free(data);
            data = NULL;
        }
        num_bytes = 0;
    }
};

class t_auto_free_vdrequest
{
public:
    t_auto_free_vdrequest(void **p);
    virtual ~t_auto_free_vdrequest();
private:
    void **m_p;
};

int geely_write_encoded_bytes(const void *buffer, size_t size, void *clientdata);

int serialize_gly_asn_2_buf(void * in_vds_sr, std::shared_ptr<t_geely_data> out_data);

int encode_buf_to_xer(unsigned char * binary_data, size_t num_bytes, std::string & xer_str);

} // namespace fsm

#endif //VOC_FRAMEWORK_SIGNALS_GEELY_ASN_COMMON_H_

/** \}    end of addtogroup */
