#ifndef VOC_SIGNALS_CVC_VDS_RVS_SIGNAL_H_
#define VOC_SIGNALS_CVC_VDS_RVS_SIGNAL_H_

#include "signals/signal_types.h"
#include "voc_framework/signals/vdmsg_signal_types.h"
#include "voc_framework/signals/vdmsg_signal.h"
#include "voc_framework/signals/signal.h"
#include "voc_framework/signals/signal_pack.h"
#include "voc_framework/signal_adapt/rvs_signal_adapt.h"

#include "timestamp.h"

namespace volvo_on_call
{


class GlyVdsRvsSignal: public fsm::VdmSignal, public fsm::SignalPack
{

 public:
    /**
     * \brief Constructs empty signal
     */ 
    GlyVdsRvsSignal (fsm::VdServiceTransactionId& transaction_id,void* vdsService);

    ~GlyVdsRvsSignal ();

    bool checkFieldsInAsnScope_long(long inputValue, long minValue, long maxValue);

    bool checkFieldsInAsnScope_int(int inputValue, int minValue, int maxValue);

    static std::shared_ptr<fsm::Signal> CreateGlyVdsRvsSignal(fsm::VdServiceTransactionId& transaction_id,void* vdsService);
     
    /**
     * \brief String identifier for printouts
     */
    std::string ToString() {return "GlyVdsRvsSignal"; }

    virtual void *GetPackPayload();

    std::shared_ptr<RvsRequest_t> GetRequestPayload()
    {
        return m_rvsRequest;
    }
	
	fsm::Signal *GetSignal()
    {
	return this;
    }

    bool GetInternalPayload();

    bool UnpackPayload(void *vdServiceRequest);

    bool SetBodyPayload();

    bool SetBsPayload(Rvs_BS_t bs);//basicVehicleStatus

    bool SetAsPayload(Rvs_AS_t as);//additionalVehicleStatus
    
    bool SetTsPayload(Rvs_TS_t ts);//temStatus

    bool SetAsCiPayload(Rvs_AS_CI_t as_ci);//additionalVehicleStatus.confAndId

    bool SetAsDsPayload(Rvs_AS_DS_t as_ds);//additionalVehicleStatus.drivingSafetyStatus

    bool SetAsMsPayload(Rvs_AS_MS_t as_ms);//additionalVehicleStatus.maintenanceStatus

    bool SetAsRsPayload(Rvs_AS_RS_t as_rs);//additionalVehicleStatus.runningStatus

    bool SetAsCsPayload(Rvs_AS_CS_t as_cs);//additionalVehicleStatus.climateStatus

    bool SetAsDbPayload(Rvs_AS_DB_t as_db);//additionalVehicleStatus.drivingBehaviourStatus

    bool SetAsEsPayload(Rvs_AS_ES_t as_es);//additionalVehicleStatus.electricVehicleStatus

    bool SetAsPsPayload(Rvs_AS_PS_t as_ps);//additionalVehicleStatus.pollutionStatus

    bool SetAsEwPayload(Rvs_AS_EW_t as_ew);//additionalVehicleStatus.ecuWarningMessages

    bool SetAsCcPayload(Rvs_AS_CC_t as_cc);//additionalVehicleStatus.carConfig

    bool SetAsSignalsKeyPayload(Rvs_AS_SIGNALS_KEY_t as_signals_key);//additionalVehicleStatus.signals.<key>

    bool UnpackServiceParameters();

    bool UnpackEventRule();

    bool SetHeadPayload();

    virtual void * PackGeelyAsn();
    bool SetLockStatusPayload(const fsm::LockStatus_Data &payload);
    bool SetMaintenanceStatusPayload(const fsm::MaintenanceStatus_Data &payload);
    bool SetRunningStatusPayload(const fsm::RunningStatus_Data &payload);
    bool SetClimateStatusPayload(const fsm::ClimateStatus_Data &payload);
    bool SetPollutionStatusPayload(const fsm::PollutionStatus_Data &payload);
    bool SetElectricStatusPayload(const fsm::ElectricStatus_Data &payload);
    bool SetBasicVehicleStatusPayload(const fsm::BasicVehicleStatus_Data &payload);
    bool SetAdditionalStatusPayload(const fsm::AdditionalStatus_Data &payload);


    
private:    
    std::shared_ptr<RvsRequest_t> m_rvsRequest;
    RvsHead_t m_rvsHeader;

    std::shared_ptr<asn_wrapper::VDServiceRequest> m_vdMsg;
    std::shared_ptr<fsm::RvsSignalAdapt> m_EncodeAdapt;
};

}

#endif
