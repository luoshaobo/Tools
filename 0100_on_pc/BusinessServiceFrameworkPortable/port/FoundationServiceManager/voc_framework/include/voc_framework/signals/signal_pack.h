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

#ifndef VOC_FRAMEWORK_SIGNALS_SIGNAL_PACK_H_
#define VOC_FRAMEWORK_SIGNALS_SIGNAL_PACK_H_

#include "voc_framework/signals/signal.h"
#include <vector>
#include <memory>

namespace fsm
{

class SignalPack
{
 public:
    SignalPack() {}
    virtual ~SignalPack() {} //forces polymorphism

public:
    virtual void *GetPackPayload() = 0;
	virtual fsm::Signal *GetSignal() {return NULL;};
    virtual void *get_asn_payload()
    { return NULL; }
    virtual void *get_sms_payload()
    { return NULL; }
    virtual std::shared_ptr<std::vector<unsigned char> > get_sms_protocol_id()
    { return NULL; }
    virtual std::shared_ptr<std::vector<unsigned char> > get_sms_service_id()
    { return NULL; }
    virtual int get_service_id()
    { return -1; }
    virtual int PackGeelyAsn(void *vdServiceRequest)
    {
        vdServiceRequest = NULL;
        return -1;
    }
    virtual void * PackGeelyAsn()
    {
        return NULL;
    }
};

} // namespace fsm

#endif //VOC_FRAMEWORK_SIGNALS_SIGNAL_PACK_H_

/** \}    end of addtogroup */

