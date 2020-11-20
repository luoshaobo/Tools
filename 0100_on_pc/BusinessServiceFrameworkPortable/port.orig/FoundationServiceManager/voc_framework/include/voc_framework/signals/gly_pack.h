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
 *  \file     signal.h
 *  \brief    VOC Service signal base class.
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_FRAMEWORK_SIGNALS_GLY_PACK_H_
#define VOC_FRAMEWORK_SIGNALS_GLY_PACK_H_


namespace fsm
{
class t_gly_pack_base
{
public:
    t_gly_pack_base(){}
    virtual ~t_gly_pack_base(){};
    virtual void *get_asn_payload() = 0;

    virtual void *get_sms_payload() = 0;
    virtual std::shared_ptr<std::vector<unsigned char> > get_sms_protocol_id() = 0;
    virtual std::shared_ptr<std::vector<unsigned char> > get_sms_service_id() = 0;
};


} // namespace fsm

#endif //VOC_FRAMEWORK_SIGNALS_GLY_PACK_H_

/** \}    end of addtogroup */

