///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental AG and subsidiaries
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  @file parking_climate.h
//  This file handles the communication with the VuC, using the OTP VCS-interface

// @project     GLY_TCAM
// @subsystem   VolvoOnCall
// @author      uia93888
// @Init date   Mar-22-2019
///////////////////////////////////////////////////////////////////
#ifndef GLY_APPLICATION_PARKING_CLIMATE_H_
#define GLY_APPLICATION_PARKING_CLIMATE_H_

#include <iostream>
#include <time>

#include "voc_framework/transactions/smart_transaction.h" 
#include "../voc_persist_data_mgr/include/voc_persist_data_mgr.h"
#include "signals/signal_types.h"
#include "signals/geely/gly_vds_rcc_signal.h"
#include "remote_start_common.h"

#include "voc_framework/signal_sources/vocmo_signal_source.h"
#include "voc_framework/signals/vdmsg_signal.h"
#include "voc_framework/signals/vdmsg_signal_types.h"
#include "voc_framework/signal_sources/vehicle_comm_signal_source.h"

#include "usermanager_types.h"

namespace volvo_on_call
{

typedef struct {
	bool ParClima_CarMode_Flag;
	bool ParClima_UsageMode_Flag;
	bool ParClima_EngineLevel_Flag;
} ParkClima_PreCondi_Flags_S;

const int PARKING_CLIMATE_VFC_ID = 3;
static const uint8_t PARKING_CLIMATE_VFC_SUCCESS = 0;   // operate VFC success
const char PARKING_CLIMATE_STRING_SPLIT_CHAR = ';';
/**
 * @brief this class is used for parking climate app,
 *   the service id is 19(app_rcc), the main function
 *   is handle the TSP request, and timing to send request
 *   to CCM, to operate climate, seat heat and steer wheel
 *   heat. And then send response to TSP
 **/
class Parking_Climate: public fsm::SmartTransaction
{
public:
	typedef enum{
		kParkClima_New,
		kParkClima_CheckPrecondition,
		kParkClima_VFCOper,
		kParkClima_OperateParkClima,
		kParkClima_Done
	} ParkingClimaStateMachine_E;
	
public:
	////////////////////////////////////////////////////////////
	// @brief : construct
	//
	// @author     uia93888, Mar 22, 2019
	///////////////////////////////////////////////////////////
	Parking_Climate();

	////////////////////////////////////////////////////////////
	// @brief : destruct
	//
	// @author     uia93888, Mar 22, 2019
	///////////////////////////////////////////////////////////
	virtual ~Parking_Climate();

	////////////////////////////////////////////////////////////
	// @brief : initialization the class
	//
	// @author     uia93888, Mar 22, 2019
	///////////////////////////////////////////////////////////
	bool Init();

	////////////////////////////////////////////////////////////
	// @brief : create a signal
	//
	// @param[in]  signal
	// @return     std::shared_ptr<GlyVdsRccSignal>
	// @author     uia93888, Mar 22, 2019
	///////////////////////////////////////////////////////////
	std::shared_ptr<GlyVdsRccSignal> CreateGlyVdsRccSignal(std::shared_ptr<fsm::Signal> signal);

	////////////////////////////////////////////////////////////
	// @brief : add transaction id to map
	//
	// @param[in]  signal
	// @return     std::shared_ptr<GlyVdsRccSignal>
	// @author     uia93888, Mar 22, 2019
	///////////////////////////////////////////////////////////
	void AddTransactionIdToMap(std::shared_ptr<fsm::Signal> signal)
	{
		MapTransactionId(signal->GetTransactionId().GetSharedCopy());
	}

private:
	////////////////////////////////////////////////////////////
	// @brief : to request vehicle status
	//
	// @param[in]  signal
	// @return     bool
	// @author     uia93888, Mar 25, 2019
	///////////////////////////////////////////////////////////
	bool Request_PreCondition();

	////////////////////////////////////////////////////////////
	// @brief : to request active/inactive VFC
	//
	// @param[in]  bool true: active, false: inactive
	// @return     bool
	// @author     uia93888, Mar 25, 2019
	///////////////////////////////////////////////////////////
	bool Request_VFCOperate(bool keep_wake);

	////////////////////////////////////////////////////////////
	// @brief : to request parking climate start/stop
	//
	// @param[in]  signal
	// @return     bool
	// @author     uia93888, Mar 22, 2019
	///////////////////////////////////////////////////////////
	bool Request_ParkingClimateOper();

	////////////////////////////////////////////////////////////
	// @brief : handle the request signal which from TSP server
	//
	// @param[in]  signal
	// @return     bool
	// @author     uia93888, Mar 22, 2019
	///////////////////////////////////////////////////////////
	bool Handle_GlyRequest(std::<fsm::Signal> signal);

	////////////////////////////////////////////////////////////
	// @brief : handle car mode signal
	//
	// @param[in]  signal
	// @return     bool
	// @author     uia93888, Mar 22, 2019
	///////////////////////////////////////////////////////////
	bool Handle_CarModeSignal(std::<fsm::Signal> signal);

	////////////////////////////////////////////////////////////
	// @brief : handle usage mode signal
	//
	// @param[in]  signal
	// @return     bool
	// @author     uia93888, Mar 22, 2019
	///////////////////////////////////////////////////////////
	bool Handle_UsageModeSignal(std::<fsm::Signal> signal);

	////////////////////////////////////////////////////////////
	// @brief : handle engine level signal
	//
	// @param[in]  signal
	// @return     bool
	// @author     uia93888, Mar 22, 2019
	///////////////////////////////////////////////////////////
	bool Handle_EngineLevelSignal(std::<fsm::Signal> signal);
	
	
	/**@brief TODO, not have this signal to handle, bu in the 
	 *        future, can handle these signals.
	 * bool Handle_TcamModeSignal(std::<fsm::Signal> signal);
	 * bool Handle_CarConfig180Signa(std::<fsm::Signal> signal);
	 * bool Handle_CarConfig182Signa(std::<fsm::Signal> signal);
	 * bool Handle_CarConfig186Signa(std::<fsm::Signal> signal);
	 **/

	////////////////////////////////////////////////////////////
	// @brief : handle vfc response signal
	//
	// @param[in]  signal
	// @return     bool
	// @author     uia93888, Mar 22, 2019
	///////////////////////////////////////////////////////////
	bool Handle_VFCSignal(std::<fsm::Signal> signal);

	////////////////////////////////////////////////////////////
	// @brief : handle start/stop parking climate request' response
	//
	// @param[in]  signal
	// @return     bool
	// @author     uia93888, Mar 22, 2019
	///////////////////////////////////////////////////////////
	bool Handle_ParkingClimateOperSignal(std::<fsm::Signal> signal);

	////////////////////////////////////////////////////////////
	// @brief : handle timeout
	//
	// @param[in]  signal
	// @return     bool
	// @author     uia93888, Mar 22, 2019
	///////////////////////////////////////////////////////////
	bool Handle_Timeout(std::<fsm::Signal> signal);

	////////////////////////////////////////////////////////////
	// @brief : handle error
	//
	// @param[in]  signal
	// @return     bool
	// @author     uia93888, Mar 22, 2019
	///////////////////////////////////////////////////////////
	bool Handle_Error(Parking_ClimateResult& result);

	////////////////////////////////////////////////////////////
	// @brief : send response to TSP server
	//
	// @param[in]  signal
	// @return     bool
	// @author     uia93888, Mar 22, 2019
	///////////////////////////////////////////////////////////
	bool SendResponse(Parking_ClimateResult& result);
	
	////////////////////////////////////////////////////////////
	// @brief : GET local time
	//
	// @param[in]  signal
	// @author     uia93888, Mar 22, 2019
	///////////////////////////////////////////////////////////
	void GetUTCTimer();

	////////////////////////////////////////////////////////////
	// @brief : struct tm to struct time_t
	//
	// @param[in]  signal
	// @return     std::shared_ptr<GlyVdsRccSignal>
	// @author     uia93888, Mar 22, 2019
	///////////////////////////////////////////////////////////
	time_t MkTime(struct tm* pTimer);

	////////////////////////////////////////////////////////////
	// @brief : get timer ids from sql
	//
	// @param[in]  signal
	// @return     std::string
	// @author     uia93888, Mar 22, 2019
	///////////////////////////////////////////////////////////
	std::string GetTimerIdFromSQL();

	////////////////////////////////////////////////////////////
	// @brief : insert timer id to SQL
	//
	// @param[in]  signal
	// @return     bool
	// @author     uia93888, Mar 22, 2019
	///////////////////////////////////////////////////////////	
	bool InsertTimerIdToSQL();

	////////////////////////////////////////////////////////////
	// @brief : update timer id
	//
	// @return     bool
	// @author     uia93888, Mar 22, 2019
	///////////////////////////////////////////////////////////	
	bool UpdateTimerIdToSQL();

	////////////////////////////////////////////////////////////
	// @brief : split string by delimiter
	//
	// @param[in]  std::string src
	// @param[in]  char delimiter
	// @return     std::Vector<std::string>
	// @author     uia93888, Mar 22, 2019
	///////////////////////////////////////////////////////////	
	std::vector<std::string> SplitString(std::string src, char delimiter);

	////////////////////////////////////////////////////////////
	// @brief : change std::vector<>  to request struct
	//
	// @param[in]  std::vector<std::string>
	// @return     Parking_ClimateRequest
	// @author     uia93888, Mar 22, 2019
	///////////////////////////////////////////////////////////	
	Parking_ClimateRequest StrVectorToRequestStruct(std::vector<std::string> strVec);
	
private:
    vc::IGeneral& m_igen;
    vocpersistdatamgr::VocPersistDataMgr& m_vocpersist;
    unsigned int m_VFCTimeout;
    unsigned int m_TSPTimeout;
    unsigned int m_RequestTimeout;
	ParkClima_PreCondi_Flags_S m_ParkClima_Flags;
	bool m_VFC_Flag;
	
	fsm::TimeoutTransactionId m_timeoutTransactionId;
    fsm::TimeoutTransactionId m_TSPTimeoutId;

	std::shared_ptr<volvo_on_call::GlyVdsRccSignal> m_pGlyVdsRccSignal;
	struct tm* m_pCurrentTimer_tm;
	time_t m_timep;
};

}


#endif //GLY_APPLICATION_PARKING_CLIMATE_H_