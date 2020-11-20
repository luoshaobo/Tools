
#ifndef VOC_FRAMEWORK_SIGNALS_VDMSG_SIGNAL_H_
#define VOC_FRAMEWORK_SIGNALS_VDMSG_SIGNAL_H_

#include "voc_framework/transactions/transaction_id.h"

#include <memory>
#include <string>

#include "usermanager_types.h"
#include "voc_framework/signals/signal.h"

// #include "asn.h"
// #include "VDSasn.h"
//#include "voc_framework/signal_adapt/app_data.h"

#include <stdlib.h>
#include <time.h>

#define random(x) (rand()%x)

#define FSM_REQUEST_ID_MIN_VALUE 0
#define FSM_REQUEST_ID_MAX_VALUE 255

namespace fsm
{
    class VdmSignal:public Signal
    {
        public:
     
            VdmSignal (VdServiceTransactionId& vdsId, const long signal_type);
           ~VdmSignal();
           
            std::string ToString()  { return "VdmSignal";}

            // virtual void SetHeadPayload(VDServiceRequest* request);
            // virtual void SetHeadPayload(asn_wrapper::VDServiceRequest* request);

            int getRequestId()
            {
                srand((int)time(0));
                m_requestId = random(FSM_REQUEST_ID_MAX_VALUE);
               
                return m_requestId;
            }
            //int SetHeadPayload(RequestHeader_Data &header);

         private:
            
            int m_requestId = FSM_REQUEST_ID_MIN_VALUE;
    };

}

#endif

























