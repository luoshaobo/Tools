///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental AG and subsidiaries
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  @file remote_start_seatHeat.h
//  This file handles the communication with the VuC, using the OTP VCS-interface

// @project     GLY_TCAM
// @subsystem   VolvoOnCall
// @author      uia93888
// @Init date   26-Jan-2018
///////////////////////////////////////////////////////////////////
#ifndef TCAM_APPLICATION_REMOTE_SEAT_HEAT_H_
#define TCAM_APPLICATION_REMOTE_SEAT_HEAT_H_

#include "remote_start_basic.h"
#include "remote_start_common.h"

namespace volvo_on_call
{

class RMT_SeatHeat: public volvo_on_call::RMTStartBasic
{
public:
    RMT_SeatHeat();
    ~RMT_SeatHeat();

private:
    ////////////////////////////////////////////////////////////
    // @brief : handle response signal time signal
    //
    // @param[in[  Signal
    // @return     bool
    // @author     uia93888, Nov 12, 2018
    ///////////////////////////////////////////////////////////
    bool Handle_ResponseSignal(std::shared_ptr<fsm::Signal> signal) override; 

    ////////////////////////////////////////////////////////////
    // @brief : send response to TSP
    //
    // @param[in]  RMT_ClimatizationResponse_t
    // @author     uia93888, Jan 24, 2019
    ///////////////////////////////////////////////////////////
    bool SendResponseToTSP(RMT_ClimatizationResponse_t response) override;

    ////////////////////////////////////////////////////////////
    // @brief : send request to Vuc to start/stop remote start applications
    //          such as: climatization, steer wheel heat, seat heat, seat 
    //          ventilation
    //
    // @param[in]  RMT_ClimatizationRequest_t
    // @author     uia93888, Jan 24, 2019
    ///////////////////////////////////////////////////////////
    bool SendRequest(std::shared_ptr<RMT_ClimatizationRequest_t> request) override;

    ////////////////////////////////////////////////////////////
    // @brief : set seat heat level: 0: close, 1: Lv1, 2: Lv2, 3:Lv3
    //
    // @param[in]  Req_OperateRMTSeatHeat
    // @param[in]  seatHeat
    // @param[in]  level
    // @author     uia93888, Jan 24, 2019
    ///////////////////////////////////////////////////////////
    bool SetSeat(vc::Req_OperateRMTSeatHeat& req, int32_t seatHeat, int level);
};

}


#endif //TCAM_APPLICATION_REMOTE_SEAT_HEAT_H_