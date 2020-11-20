#include <stdio.h>
#include "timestamp.h"
#include "dlt/dlt.h"
#include "signals/signal_types.h"
#include "signals/geely/gly_vds_rvdc_signal.h"
#include "voc_framework/signals/vdmsg_signal_types.h"
#include "voc_framework/signal_adapt/rvdc_signal_adapt.h"

namespace volvo_on_call
{

/************************************************************/
// @brief :create rvc signal.
// @param[in]  VdServiceTransactionId, vds transactionid.
// @param[in]  vdsService, vds msg.
// @return
// @author     nieyujin, 27-Feb-2019
/************************************************************/
std::shared_ptr<fsm::Signal> GlyVdsRvdcSignal::CreateGlyVdsRvdcSignal(fsm::VdServiceTransactionId& transaction_id, void * vdServiceRequest)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRvdcSignal::%s", __FUNCTION__);
    GlyVdsRvdcSignal* signal = new GlyVdsRvdcSignal(transaction_id, vdServiceRequest);
    
    return std::shared_ptr<GlyVdsRvdcSignal>(signal);
}

/************************************************************/
// @brief :Constructs function.
// @param[in]  VdServiceTransactionId, vds transactionid.
// @param[in]  vdsService, vds msg.
// @return
// @author     nieyujin, 27-Feb-2019
/************************************************************/
GlyVdsRvdcSignal::GlyVdsRvdcSignal (fsm::VdServiceTransactionId& transaction_id, void *vdServiceRequest)
    :fsm::VdmSignal(transaction_id, fsm::kVDServiceRvc)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRvdcSignal::%s", __FUNCTION__);
    bool result = UnpackPayload(vdServiceRequest);
    if(result)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s,rvdc request payload success.", __FUNCTION__);
    }
    else
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s,rvdc request payload fail.", __FUNCTION__);
    }
}

/************************************************************/
// @brief :destruct function.
// @param[in]  none
// @return
// @author     nieyujin, 27-Feb-2019
/************************************************************/
GlyVdsRvdcSignal::~GlyVdsRvdcSignal ()
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRvdcSignal::%s", __FUNCTION__);

}

/************************************************************/
// @brief :pack RVDC_Data into ASN1C structure. upload body msg.
// @param[in]  response, RVDC_Data structure .
// @return     True if successfully set, false otherwise
// @author     nieyujin, 27-Feb-2019
/************************************************************/
bool GlyVdsRvdcSignal::SetResultPayload(fsm::RVDC_Data payload)
{
    return true;
}

/************************************************************/
// @brief :pack RVDC_Data into ASN1C structure. upload body msg.
// @param[in]  response, RVDC_Data structure .
// @return     0 if successfully set, -1 otherwise
// @author     nieyujin, 27-Feb-2019
/************************************************************/
int GlyVdsRvdcSignal::PackGeelyAsn(void *vdServiceRequest)
{
    return 0;
}

/************************************************************/
// @brief :unpack ASN1C structure.
// @param[in]  ASN1C structur
// @return     True if successfully set, false otherwise
// @author     nieyujin, 27-Feb-2019
/************************************************************/
bool GlyVdsRvdcSignal::UnpackPayload(void * vdServiceRequest)
{
    return true;
}

} // namespace volvo_on_call