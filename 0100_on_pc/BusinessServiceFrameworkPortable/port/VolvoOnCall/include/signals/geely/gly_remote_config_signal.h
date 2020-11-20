///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//	@file gly_remote_config_signal.h
//	geely remote_config signal.

// @project		GLY_TCAM
// @subsystem	Application
// @author		zhouyou
// @Init date	31-NEV-2018
///////////////////////////////////////////////////////////////////

#ifndef VOC_SIGNALS_GLY_REMOTE_CONFIG_SIGNAL_H_
#define VOC_SIGNALS_GLY_REMOTE_CONFIG_SIGNAL_H_

#include "signals/signal_types.h"
#include "voc_framework/signals/signal.h"
#include "voc_framework/signals/signal_pack.h"
#include "voc_framework/signals/vdmsg_signal.h"
#include "voc_framework/signals/vdmsg_signal_types.h"

#include "asn.h"

#include <memory>
#include <string>

namespace volvo_on_call
{
enum 
{
    UNKNOW = -1,        /********config Type not int or not string **************/
    TYPEINT = 1,        /********config Type is integer **************/
    TYPESTR = 2         /********config Type is string **************/
};


typedef struct 
{
    std::string itemName;      
    int valueType;
    std::string key; 
    std::string strVal;
    int intVal; 
} ConfigUpdate_t;




class GlyRemoteConfigSignal: public fsm::VdmSignal, public fsm::SignalPack
{
     public:
    /**
     * \brief Constructs empty signal
     */ 
    GlyRemoteConfigSignal (fsm::VdServiceTransactionId& transaction_id,VDServiceRequest* vdsService);

    static std::shared_ptr<fsm::Signal> CreateGlyRemoteConfigSignal(fsm::VdServiceTransactionId& transaction_id, VDServiceRequest* vdsService);

    ~GlyRemoteConfigSignal ();

    void *GetPackPayload()
    {
        return (void *)m_vdsResponse;
    }

    fsm::Signal *GetSignal()
    {
	    return this;
    }

    std::string ToString() {return "GlyRemoteConfigSignal"; }

    std::vector<ConfigUpdate_t> GetConfigUpdate(void);

    bool SetVdsPayload();
    bool UnpackPayload();

private:    
    bool UnpackLoadItems(asnListElement** asnEltCfg);
    
    VDServiceRequest* m_vdsService;
    VDServiceRequest* m_vdsResponse;
    std::vector<ConfigUpdate_t> m_configData;
     
};

   

}


#endif





