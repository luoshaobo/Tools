///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive AG and subsidiaries
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// @file gly_vds_jl_signal.h
// geely journal signal.

// @project         GLY_TCAM
// @subsystem   Application
// @author        Hu Tingting
// @Init date     26-Sep-2018
///////////////////////////////////////////////////////////////////

#ifndef VOC_SIGNALS_GLY_VDS_JL_SIGNAL_H_
#define VOC_SIGNALS_GLY_VDS_JL_SIGNAL_H_

#include "asn.h"
#include "signals/signal_types.h"
#include "voc_framework/signals/signal.h"
#include "voc_framework/signals/signal_pack.h"
#include "voc_framework/signals/vdmsg_signal.h"
#include "voc_framework/signals/vdmsg_signal_types.h"
#include "voc_framework/signal_adapt/journeylog_signal_adapt.h"
#include <list>
#include <memory>

namespace volvo_on_call
{

typedef struct TimeStampJL {
    int seconds;
    int milliseconds; /* OPTIONAL */
} TimeStamp_t;

typedef struct PositionJL {
    long   latitude; /* OPTIONAL */
    long   longitude; /* OPTIONAL */
    long   altitude; /* OPTIONAL */
    bool   posCanBeTrusted; /* OPTIONAL */
    bool   carLocatorStatUploadEn; /* OPTIONAL */
bool   marsCoordinates; /* OPTIONAL */
} PositionJL_t;


typedef struct TrackPointJL {
    TimeStamp_t   systemTime;
    PositionJL_t   position;
    long   speed;
} TrackPointJL_t;

typedef struct JournalData {
long   tripId;
TimeStamp_t   startTime;
long   startOdometer;/* OPTIONAL */
long   fuelConsumption;/* OPTIONAL */
long   traveledDistance;
std::list<PositionJL_t>   waypoints;
long   avgSpeed;
TimeStamp_t   endTime;
long   endOdometer;/* OPTIONAL */
long   electricConsumption;/* OPTIONAL */
long   electricRegeneration;/* OPTIONAL */
} JournalData_t;


typedef struct {
    int    service_id;
    int    ack_flag;
} JournalAck_t;

class GlyVdsJLSignal: public fsm::VdmSignal, public fsm::SignalPack
{

 public:

    /************************************************************/
    // @brief :Constructs signal.
    // @param[in]  VdServiceTransactionId, vds transactionid.
    // @param[in]  vdsService, vds msg.
    // @return
    // @author     Hu Tingting, 26-Sep-2018
    /************************************************************/
    GlyVdsJLSignal (fsm::VdServiceTransactionId& transaction_id,void* vdsService);


    /************************************************************/
    // @brief :create journal signal.
    // @param[in]  VdServiceTransactionId, vds transactionid.
    // @param[in]  vdsService, vds msg.
    // @return
    // @author     Hu Tingting, 26-Sep-2018
    /************************************************************/
    static std::shared_ptr<fsm::Signal> CreateGlyVdsJLSignal (fsm::VdServiceTransactionId& transaction_id,void* vdsService);


    /************************************************************/
    // @brief :destruct signal.
    // @param[in]  none
    // @return
    // @author     Hu Tingting, 26-Sep-2018
    /************************************************************/
    ~GlyVdsJLSignal ();

    std::string ToString() {return "GlyVdsJLSignal"; }


    /************************************************************/
    // @brief :pack JournalData_t into ASN1C structure. upload body msg.
    // @param[in]  response, JournalData_t structure .
    // @return     True if successfully set, false otherwise
    // @author     Hu Tingting, 26-Sep-2018
    /************************************************************/
    bool SetJLPayload(JournalData_t response);


    /************************************************************/
    // @brief :get packed ASN1C structure. Default encode upload func.
    // @return     pointer to asn1c struct containing payload, caller must free
    // @author     Hu Tingting, 26-Sep-2018
    /************************************************************/
    void *GetPackPayload()
    {
        return nullptr;
    }

    virtual int PackGeelyAsn(void *vdServiceRequest) override;

    fsm::Signal *GetSignal()
    {
        return this;
    }

    /************************************************************/
    // @brief :get unpacked ASN1C structure.used to return ack msg with own struct.
    // @return     unpacked structure
    // @author     Hu Tingting, 26-Sep-2018
    /************************************************************/
    std::shared_ptr<JournalAck_t> GetRequestPayload()
    {
        return m_JLAck;
    }


 private:
    /************************************************************/
    // @brief :unpack ASN1C structure.Currently used to receive ack msg.
    // @return     True if successfully set, false otherwise
    // @author     Hu Tingting, 26-Sep-2018
    /************************************************************/
    bool UnpackPayload();

 private:
    //fsm asn1 signal VDServiceRequest
    asn_wrapper::VDServiceRequest* m_vdsService;
    //upload signal,default
    JournalData_t m_response;
   //change VDServiceRequest to own struct
    std::shared_ptr<JournalAck_t> m_JLAck;
};

} // namespace volvo_on_call

#endif //VOC_SIGNALS_BCC_CL_001_SIGNAL_H_

