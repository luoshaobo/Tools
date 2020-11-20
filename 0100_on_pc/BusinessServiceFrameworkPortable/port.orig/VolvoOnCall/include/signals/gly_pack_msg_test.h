#ifndef VOC_SIGNALS_GLY_PACK_MSG_TEST_H_
#define VOC_SIGNALS_GLY_PACK_MSG_TEST_H_

#include <memory>
#include <vector>

//asn1c
//#include "asn_application.h"
//marben
#include "asn.h"

//voc_framework
#include "voc_framework/transactions/transaction_id.h"
#include "voc_framework/signals/signal.h"
#include "voc_framework/signals/signal_pack.h"

class t_gly_pack_msg_test: public fsm::SignalPack , public fsm::Signal
{
    public:

        t_gly_pack_msg_test(const fsm::TransactionId& transaction_id);
        
        virtual ~t_gly_pack_msg_test();

        virtual void *GetPackPayload();

		virtual fsm::Signal *GetSignal();

        virtual std::string ToString(){return "t_gly_pack_msg_test";}

        virtual SignalType GetSignalType() {return 67 /*kVDServiceOta*/ ;};
        
     private:
         VDServiceRequest *m_vdmsg;

};



#endif















