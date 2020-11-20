#ifndef VOC_SIGNALS_VDMSG_TEST__SIGNAL_H_
#define VOC_SIGNALS_VDMSG_TEST_SIGNAL_H_

#include <memory>
#include <vector>

//voc_framework
#include "voc_framework/signals/signal.h"
#include "voc_framework/signals/signal_pack.h"

#include "asn.h"

class VDMsgTestSignal: public fsm::Signal, public fsm::SignalPack
{
    public:

        VDMsgTestSignal( fsm::VdServiceTransactionId& transaction_id,VDServiceRequest  *  vdmsg);
        virtual ~VDMsgTestSignal();

        virtual std::string ToString();
        static std::shared_ptr<fsm::Signal> CreateVdmsgSignal( fsm::VdServiceTransactionId& transaction_id,VDServiceRequest  *  vdmsg);

        virtual void *GetPackPayload();

        private:
            bool UnpackPayload(VDServiceRequest  *  vdmsg);
            long my_reqId;
            long my_signal_type;
            VDServiceRequest  tsVdmsg;
};


#endif















