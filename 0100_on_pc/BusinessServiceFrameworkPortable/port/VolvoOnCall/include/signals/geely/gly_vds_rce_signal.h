///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental AG and subsidiaries
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  @file gly_vds_rce_signal.h
//  This file handles the communication with the VuC, using the OTP VCS-interface
 
// @project     GLY_TCAM
// @subsystem   VolvoOnCall
// @author      uia93888
// @Init date   22-Jan-2019
///////////////////////////////////////////////////////////////////
#ifndef GLY_VDS_REMOTE_START_SIGNAL_H_
#define GLY_VDS_REMOTE_START_SIGNAL_H_

#include "signals/signal_types.h"
#include "voc_framework/signals/signal.h"
#include "voc_framework/signals/signal_pack.h"
#include "voc_framework/signals/vdmsg_signal.h"
#include "voc_framework/signals/vdmsg_signal_types.h"
#include "voc_framework/signal_sources/vehicle_comm_signal_source.h"
#include "remote_start/remote_start_common.h"

#include "asn.h"

#include <memory>

namespace volvo_on_call
{
    static const std::string GLY_VDS_PARAM_EMOTE_CLIMATE_NAME = "rce.temp";
    static const std::string GLY_VDS_PARAM_LEVEL = "rce.level";
    static const std::string GLY_VDS_PARAM_REMOTE_HEAT_NAME = "rce.heat";

    static const int GLY_VDS_PARAM_CLIAMTE_RUNNG_TIME_DEFAULT = 17*60;  // <* default running time, unit: second
    static const int GLY_VDS_PARAM_CLIAMTE_RUNNG_TIME_MIN = 0;          // <* min running time, unit: second
    static const int GLY_VDS_PARAM_CLIAMTE_RUNNG_TIME_MAX = 59*60;      // <* max running time, unit: second

    static const int GLY_VDS_PARAM_CLIAMTE_TEMP_DEFAULT = 15.5;  // <* default running time, unit: second
    static const int GLY_VDS_PARAM_CLIAMTE_TEMP_MIN = 15.5;          // <* min running time, unit: second
    static const int GLY_VDS_PARAM_CLIAMTE_TEMP_MAX = 28.5;      // <* max running time, unit: second

const int GEELY_STEERING_WHEEL_HEAT_TIME = 8 * 60; //unit: seconds

    class GlyVdsRceSignal: public fsm::VdmSignal, public fsm::SignalPack
    {
        public:
            ////////////////////////////////////////////////////////////
            // @brief :Constructs signal.
            // @param[in]  VdServiceTransactionId, vds transactionid.
            // @param[in]  vdsService, vds msg.
            // @return     
            // @author     uia93888, 22-Jan-2019
            ////////////////////////////////////////////////////////////

            GlyVdsRceSignal(fsm::VdServiceTransactionId& transaction_id, void* vdsService);
            ////////////////////////////////////////////////////////////
            // @brief :create res signal.
            // @param[in]  VdServiceTransactionId, vds transactionid.
            // @param[in]  vdsService, vds msg.
            // @return     
            // @author     uia93888, 22-Jan-2019
            ////////////////////////////////////////////////////////////
            static std::shared_ptr<fsm::Signal> CreateGlyVdsRceSignal (fsm::VdServiceTransactionId& transaction_id, void* vdsService);

            ////////////////////////////////////////////////////////////
            // @brief :destruct signal.
            // @return     
            // @author     uia93888, 22-Jan-2019
            ////////////////////////////////////////////////////////////
            ~GlyVdsRceSignal();

            std::string ToString(){ return "GlyVdsRceSignal"; }

            ////////////////////////////////////////////////////////////
            // @brief :GetSignal.
            // @return     fsm::Signal*
            // @author     uia93888, 22-Jan-2019
            ////////////////////////////////////////////////////////////
            fsm::Signal* GetSignal()
            {
                return this;
            }

            ////////////////////////////////////////////////////////////
            // @brief :PackGeelyAsn
            // @return     fsm::Signal *
            // @author     uia93888, 5-Mar-2018
            ////////////////////////////////////////////////////////////
            virtual int PackGeelyAsn(void *vdServiceRequest);

            ////////////////////////////////////////////////////////////
            // @brief :return response VDS message.
            // @return     fsm::Signal*
            // @author     uia93888, 22-Jan-2019
            ////////////////////////////////////////////////////////////
            void *GetPackPayload()
            {
                return (void *)NULL;
            }

            ////////////////////////////////////////////////////////////
            // @brief :get unpacked ASN1C structure.
            // @return     unpacked structure
            // @author     uia93888, 3-Sep-2018
            ////////////////////////////////////////////////////////////
            std::shared_ptr<RMT_ClimatizationRequest_t> GetRequestPayload();

            ////////////////////////////////////////////////////////////
            // @brief :pack RequestHeader.
            // @return     True if successfully set, false otherwise
            // @author     uia93888, 22-Jan-2019
            ////////////////////////////////////////////////////////////
            bool SetHeadPayload();

            ////////////////////////////////////////////////////////////
            // @brief :pack RequestHeader.
            //
            // @param[in]  RMT_ClimatizationResponse_t
            // @return     bool
            // @author     uia93888, Jan-26-2019
            ////////////////////////////////////////////////////////////
            bool SetResultPayload(RMT_ClimatizationResponse_t res);

            ////////////////////////////////////////////////////////////
            // @brief :pack RequestHeader.
            // @return     True if successfully set, false otherwise
            // @author     uia93888, 22-Jan-2019
            ////////////////////////////////////////////////////////////
            ServiceType GetServiceType(){ return m_climateReq->m_serviceType; }

            ////////////////////////////////////////////////////////////
            // @brief :pack RequestHeader.
            // @return     True if successfully set, false otherwise
            // @author     uia93888, 22-Jan-2019
            ////////////////////////////////////////////////////////////
            OperateCommands GetOperateCommand(){ return m_climateReq->m_operateCmd; }

        private:
            ////////////////////////////////////////////////////////////
            // @brief :unpack ASN1C structure.
            // @return     True if successfully set, false otherwise
            // @author     uia93888, 3-Sep-2018
            ////////////////////////////////////////////////////////////
            bool UnpackPayload(void* vdsServiceRequest);

        private:
            /*VDServiceRequest* m_pVDSReq;
            VDServiceRequest* m_pVDSRes;*/
            std::shared_ptr<fsm::RMT_CliamtizationRequest_t> m_pRMT_ClimatizationReq;
            std::shared_ptr<fsm::RMT_CliamteAutoStopResult_t> m_pRMT_ClimateAutoStopRes;
            std::shared_ptr<fsm::RMT_ClimateSrtStopResult_t> m_pRMT_ClimateStrtStopRes;
            std::shared_ptr<fsm::RMT_RemoteStartCommErrorResult_t> m_pRMT_RemoteStrtErrRes;
            std::shared_ptr<RMT_ClimatizationRequest_t> m_climateReq;
    };

}


#endif //GLY_VDS_REMOTE_START_SIGNAL_H_