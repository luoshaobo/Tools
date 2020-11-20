///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental AG and subsidiaries
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  @file parking_climate.cc
//  This file handles the communication with the VuC, using the OTP VCS-interface

// @project     GLY_TCAM
// @subsystem   VolvoOnCall
// @author      uia93888
// @Init date   Mar-22-2019
///////////////////////////////////////////////////////////////////

#include "remote_start/parking_climate.h"

#include "dlt/dlt.h"
DLT_IMPORT_CONTEXT(dlt_voc)

namespace volvo_on_call{

Parking_Climate::Parking_Climate():
    fsm::SmartTransaction(kParkClima_New),
	m_igen(fsm::VehicleCommSignalSource::GetInstance().GetVehicleCommClientObject().GetGeneralInterface()),
	m_vocpersist(vocpersistdatamgr::VocPersistDataMgr::GetInstance()),
    m_VFCTimeout(-1), m_TSPTimeout(-1), m_RequestTimeout(-1),
	m_ParkClima_Flags(false, false, false),
	m_VFC_Flag(false),
	m_pCurrentTimer_tm(nullptr)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Parking_Climate::%s construct", __FUNCTION__);
}

Parking_Climate::~Parking_Climate()
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Parking_Climate::%s destruct", __FUNCTION__);
}

bool Parking_Climate::Init()
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Parking_Climate::%s.", __FUNCTION__);
    SignalFunction Handle_Reuqest_Signal =
            std::bind(&Parking_Climate::Handle_GlyRequest, this, std::placeholders::_1);
    SignalFunction Handle_Carmode_Signal =
            std::bind(&Parking_Climate::Handle_CarModeSignal, this, std::placeholders::_1);
    SignalFunction Handle_Usagemode_Signal =
            std::bind(&Parking_Climate::Handle_UsageModeSignal, this, std::placeholders::_1);
    SignalFunction Handle_EngineLevel_Signal =
            std::bind(&Parking_Climate::Handle_EngineLevelSignal, this, std::placeholders::_1);
    SignalFunction Handle_VFC_Signal =
            std::bind(&Parking_Climate::Handle_VFCSignal, this, std::placeholders::_1);
    SignalFunction Handle_ParkingClimateOper_Signal =
            std::bind(&Parking_Climate::Handle_ParkingClimateOperSignal, this, std::placeholders::_1);
    SignalFunction Handle_Timeout_Signal =
            std::bind(&Parking_Climate::Handle_Timeout, this, std::placeholders::_1);
			
	StateMap state_map = {
            {kParkClima_New,				{nullptr,
                                                {{fsm::kVDServiceRcc, Handle_Reuqest_Signal},
                                                {fsm::Signal::kTimeout, Handle_Timeout_Signal}},
                                                {kParkClima_CheckPrecondition, kParkClima_Done}}},
            {kParkClima_CheckPrecondition,  {nullptr,
                                                {{fsm::Signal::kCarMode, Handle_Carmode_Signal},
                                                {fsm::Signal::kCarUsageMode, Handle_Usagemode_Signal}
                                                {fsm::Signal::kElectEngLvlSignal, Handle_EngineLevel_Signal}
                                                {fsm::Signal::kTimeout, Handle_Timeout_Signal}},
                                                {kParkClima_VFCOper, kParkClima_Done}}},
            {kParkClima_VFCOper,            {nullptr,
                                                {{fsm::Signal::kVFCActivateSignal, Handle_VFC_Signal},
                                                {fsm::Signal::kTimeout, Handle_Timeout_Signal}},
                                                {kParkClima_OperateParkClima, kParkClima_Done}}},
            {kParkClima_OperateParkClima,   {nullptr,
                                                {{fsm::Signal::kParkingClimateOperSignal, Handle_ParkingClimateOper_Signal},
                                                {fsm::Signal::kTimeout, Handle_Timeout_Signal}},
                                                {kParkClima_Done}}},
            {kParkClima_Done,               {nullptr,
                                                {{fsm::Signal::kTimeout, Handle_Timeout_Signal}},
                                                {}}},
	};
    SetStateMap(state_map);
	MapSignalType(fsm::Signal::kCarMode);
	MapSignalType(fsm::Signal::kCarUsageMode);
	MapSignalType(fsm::Signal::kElectEngLvlSignal);
	MapSignalType(fsm::Signal::kVFCActivateSignal);
	MapSignalType(fsm::Signal::kParkingClimateOperSignal);
	MapSignalType(fsm::Signal::kTimeout);


    if (!m_vocpersist.GetData(vocpersistdatamgr::RCE_RPC_VFC_TIMEOUT, m_VFCTimeout)){
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "~RMTStartBasic::%s , failed, using default vfc-timeout time value: 3s", __FUNCTION__);
        m_VFCTimeout = GLY_CLIMA_RPC_VFC_TIMEOUT_DEFAULT;
    } else {
        if ((GLY_CLIMA_RPC_VFC_TIMEOUT_MIX > m_VFCTimeout) ||
            (GLY_CLIMA_RPC_VFC_TIMEOUT_MAX < m_VFCTimeout)){
                m_VFCTimeout = GLY_CLIMA_RPC_VFC_TIMEOUT_DEFAULT;
        }
    }
    if (!m_vocpersist.GetData(vocpersistdatamgr::RCE_CLIMA_REQ_TIMEOUT, m_RequestTimeout)){
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "~RMTStartBasic::%s , failed, using default vfc-timeout time value: 3s", __FUNCTION__);
        m_RequestTimeout = GLY_CLIMA_REQUEST_TIME_DEFAULT;
    } else {
        if ((GLY_CLIMA_REQUEST_TIME_MIN > m_RequestTimeout) ||
            (GLY_CLIMA_REQUEST_TIME_MAX < m_RequestTimeout)){
                m_RequestTimeout = GLY_CLIMA_REQUEST_TIME_DEFAULT;
        }
    }
    if (!m_vocpersist.GetData(vocpersistdatamgr::RCE_TSP_RESPONSE_TIME, m_TSPTimeout)){
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "~RMTStartBasic::%s , failed, using default vfc-timeout time value: 3s", __FUNCTION__);
        m_TSPTimeout = GLY_CT_TSP_SET_RESPONSE_TIME_DEFAULT;
    } else {
        if ((GLY_CT_TSP_SET_RESPONSE_TIME_MIN > m_TSPTimeout) ||
            (GLY_CT_TSP_SET_RESPONSE_TIME_MAX < m_TSPTimeout)){
                m_TSPTimeout = GLY_CT_TSP_SET_RESPONSE_TIME_DEFAULT;
        }
    }

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Parking_Climate::%s, m_VFCTimeout: %d, end.", __FUNCTION__, m_VFCTimeout);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Parking_Climate::%s, m_RequestTimeout: %d, end.", __FUNCTION__, m_RequestTimeout);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Parking_Climate::%s, m_TSPTimeout: %d, end.", __FUNCTION__, m_TSPTimeout);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Parking_Climate::%s, end.", __FUNCTION__);
	return true;
}

std::shared_ptr<GlyVdsRccSignal> Parking_Climate::CreateGlyVdsRccSignal(std::shared_ptr<fsm::Signal> signal)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Parking_Climate::%s.", __FUNCTION__);
	return std::dynamic_pointer_cast<volvo_on_call::GlyVdsRccSignal>(signal);
}

bool Parking_Climate::Request_PreCondition()
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Parking_Climate::%s.", __FUNCTION__);
	bool ret = false;

    std::shared_ptr<fsm::VehicleCommTransactionId> vehicle_comm_request_id =
        std::make_shared<fsm::VehicleCommTransactionId>();
    MapTransactionId(vehicle_comm_request_id);

	if ((m_igen.Request_CarMode( vehicle_comm_request_id->GetId()) == vc::RET_OK) &&
		(m_igen.Request_CarUsageMode( vehicle_comm_request_id->GetId()) == vc::RET_OK) &&
		(m_igen.Request_ElectEnergyLevel( vehicle_comm_request_id->GetId()) == vc::RET_OK)){
			ret = true;
			SetState(kParkClima_CheckPrecondition);
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO,
                            "Parking_Climate::%s, Success", __FUNCTION__, GetState());
	} else {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO,
                        "Parking_Climate::%s, Failure", __FUNCTION__, GetState());
		SetState(kParkClima_Done);
		Parking_ClimateResult res;
		res.operationSuccessed = false;
		res.errorCode = executionFailure;
		Handle_Error(res);
	}
	return ret;
}

bool Parking_Climate::Request_VFCOperate(bool keep_wake)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Parking_Climate::%s.", __FUNCTION__);
	if ((false == m_ParkClima_Flags.ParClima_CarMode_Flag) ||
		(false == m_ParkClima_Flags.ParClima_UsageMode_Flag) ||
		(false == m_ParkClima_Flags.ParClima_EngineLevel_Flag)){
		return false;
	}

	std::shared_ptr<fsm::VehicleCommTransactionId> vehicle_comm_request_id =
		std::make_shared<fsm::VehicleCommTransactionId>();
    MapTransactionId(vehicle_comm_request_id);
	vc::ReqVFCActivate request;
    request.id = PARKING_CLIMATE_VFC_ID;
    request.type = keep_wake;
	
	if (m_igen.Request_VFCActivate(&request, vehicle_comm_request_id->GetId()) == vc::RET_OK){
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Parking_Climate:: %s, m_VFCTimeout timer: %ds", __FUNCTION__, m_VFCTimeout);
            m_timeoutTransactionId = RequestTimeout(static_cast<std::chrono::seconds>(m_VFCTimeout), false);
            MapTransactionId(m_timeoutTransactionId.GetSharedCopy());
			if (keep_wake){// active VFC
				m_VFC_Flag = true;
				SetState(kParkClima_VFCOper);
			}
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO,
                "Parking_Climate: %s, success to requests for VFC %s.", __FUNCTION__, (keep_wake)?"Active":"Inactive");
	} else {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO,
                "Parking_Climate: %s, failed to requests for VFC %s.", __FUNCTION__, (active_vfc)?"Active":"Inactive");
		SetState(kParkClima_Done);
		Parking_ClimateResult res;
		res.operationSuccessed = false;
		res.errorCode = executionFailure;
		Handle_Error(res);
	}
	return true;
}

bool Parking_Climate::Request_ParkingClimateOper()
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Parking_Climate::%s.", __FUNCTION__);
	return true;
}

bool Parking_Climate::Handle_GlyRequest(std::<fsm::Signal> signal)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Parking_Climate::%s.", __FUNCTION__);
	return true;
}

bool Parking_Climate::Handle_CarModeSignal(std::<fsm::Signal> signal)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Parking_Climate::%s.", __FUNCTION__);
    m_ParkClima_Flags.ParClima_CarMode_Flag = false;
    std::shared_ptr<fsm::CarModeSignal> car_mode_response =  std::static_pointer_cast<fsm::CarModeSignal>(signal);
    vc::ReturnValue vc_return_value = car_mode_response->GetVcStatus();
    if (vc_return_value != vc::RET_OK)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_WARN, "Parking_Climate::%s, Failed vc_return_value != vc::RET_OK.", __FUNCTION__);
		SetState(kParkClima_Done);
		Parking_ClimateResult res;
		res.operationSuccessed = false;
		res.errorCode = executionFailure;
		Handle_Error(res);
        return false;
    }
    vc::CarModeState car_mode_return = car_mode_response->GetData()->carmode;
    if (car_mode_return == vc::CAR_NORMAL)
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "Parking_Climate::Handle_CarModeSignal, car mode OK");
        m_ParkClima_Flags.ParClima_CarMode_Flag = true;
        Request_VFCOperate(true);
    } else {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "Parking_Climate::Handle_CarModeSignal, car mode NOK");
		SetState(kParkClima_Done);
		Parking_ClimateResult res;
		res.operationSuccessed = false;
		res.errorCode = executionFailure;
		Handle_Error(res);
       return false;
    }
    return true;
}

bool Parking_Climate::Handle_UsageModeSignal(std::<fsm::Signal> signal)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Parking_Climate::%s.", __FUNCTION__);
    m_ParkClima_Flags.ParClima_UsageMode_Flag = false;
    std::shared_ptr<fsm::CarUsageModeSignal> car_usage_mode_response = std::static_pointer_cast<fsm::CarUsageModeSignal>(signal);
    vc::ReturnValue vc_return_value = car_usage_mode_response->GetVcStatus();
    if (vc::RET_OK != vc_return_value){
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_WARN, "Parking_Climate::%s, Failed vc_return_value != vc::RET_OK.", __FUNCTION__);
		SetState(kParkClima_Done);
		Parking_ClimateResult res;
		res.operationSuccessed = false;
		res.errorCode = executionFailure;
		Handle_Error(res);
        return false;
    }
    vc::CarUsageModeState car_usage_mode_return = car_usage_mode_response->GetData()->usagemode;
	if ((vc::CAR_INACTIVE == car_usage_mode_return) ||
		(vc::CAR_USGMODCNVINC == car_usage_mode_return) ||
		(vc::CAR_ACTIVE == car_usage_mode_return)){
            m_ParkClima_Flags.ParClima_UsageMode_Flag = false;
            Request_VFCOperate(true);
	} else {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "Parking_Climate::Handle_CarModeSignal, usage mode NOK");
		SetState(kParkClima_Done);
		Parking_ClimateResult res;
		res.operationSuccessed = false;
		res.errorCode = executionFailure;
		Handle_Error(res);
       return false;
	}
	return true;
}

bool Parking_Climate::Handle_EngineLevelSignal(std::<fsm::Signal> signal)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Parking_Climate::%s.", __FUNCTION__);
    m_ParkClima_Flags.ParClima_EngineLevel_Flag = false;
    std::shared_ptr<fsm::ElectEngLvlSignal> m_engine_level =  std::static_pointer_cast<fsm::ElectEngLvlSignal>(signal);
    vc::ReturnValue vc_return_value = m_engine_level->GetVcStatus();
    if (vc_return_value != vc::RET_OK)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR,
                "Parking_Climate::%s, Get engine Level Failed.", __FUNCTION__);
		SetState(kParkClima_Done);
		Parking_ClimateResult res;
		res.operationSuccessed = false;
		res.errorCode = executionFailure;
		Handle_Error(res);
        return false;
    }
    if (vc::ENERGY_LVL_NORMAL == m_engine_level->GetData()->level)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR,
                    "Parking_Climate::%s, Engine Level is vc::ENERGY_LVL_NORMAL.", __FUNCTION__);
        m_ParkClima_Flags.ParClima_EngineLevel_Flag = true;
        Request_VFCOperate(true);
    } else {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR,
                    "Parking_Climate::%s, Engine Level is not vc::ENERGY_LVL_NORMAL.", __FUNCTION__);
		SetState(kParkClima_Done);
		Parking_ClimateResult res;
		res.operationSuccessed = false;
		res.errorCode = executionFailure;
		Handle_Error(res);
        return false;
    }
	return true;
}

bool Parking_Climate::Handle_VFCSignal(std::<fsm::Signal> signal)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Parking_Climate::%s.", __FUNCTION__);
    RemoveTimeout(m_timeoutTransactionId);
    std::shared_ptr<fsm::VFCActivateSignal> _pVFCActivateSignal = std::static_pointer_cast<fsm::VFCActivateSignal>(signal);
    vc::ReturnValue vc_return_value = _pVFCActivateSignal->GetVcStatus();
    if(vc_return_value != vc::RET_OK){
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_WARN, "Parking_Climate:%s, vc_return_value != vc::RET_OK", __FUNCTION__);
		SetState(kParkClima_Done);
		Parking_ClimateResult res;
		res.operationSuccessed = false;
		res.errorCode = executionFailure;
		Handle_Error(res);
        return false;
    }
    std::shared_ptr<vc::ResVFCActivate> _resVFCActivate = _pVFCActivateSignal->GetData();
    if (PARKING_CLIMATE_VFC_ID == _resVFCActivate->id){
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_WARN, "Parking_Climate:%s, VFC operater is ers's vfc", __FUNCTION__);
        if(PARKING_CLIMATE_VFC_SUCCESS == _resVFCActivate->resule){//operate success
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_WARN, "Parking_Climate:%s, Success; current state: %s",
                                    __FUNCTION__, GetRMT_EngineState().c_str());
            if (kParkClima_VFCOper == GetState()){
                Request_ParkingClimateOper();
            } else { // inactive VFC
                SetState(kParkClima_Done);
            }
        } else {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_WARN, "Parking_Climate:%s, VFC operate failed", __FUNCTION__);
	    	SetState(kParkClima_Done);
	    	Parking_ClimateResult res;
	    	res.operationSuccessed = false;
	    	res.errorCode = executionFailure;
	    	Handle_Error(res);
            return false;
        }
    }
	return true;
}

bool Parking_Climate::Handle_ParkingClimateOperSignal(std::<fsm::Signal> signal)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Parking_Climate::%s.", __FUNCTION__);
	return true;
}

bool Parking_Climate::Handle_Timeout(std::<fsm::Signal> signal)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Parking_Climate::%s.", __FUNCTION__);
	Parking_ClimateResult res;
	res.operationSuccessed = false;
	res.errorCode = timeout;
	SetState(kParkClima_Done);
	if (m_TSPTimeoutId == signal->GetTransactionId()){//TSP-request timeout
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Parking_Climate::%s, TSP-request timeout.", __FUNCTION__);
        return true;
	}
	switch (GetState()){
		case kParkClima_VFCOper:{ //VFC timeout
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Parking_Climate::%s, kParkClima_VFCOper timeout.", __FUNCTION__);
			break;
		}
		case kParkClima_OperateParkClima:{ //request VUC timeout
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Parking_Climate::%s, kParkClima_OperateParkClima timeout.", __FUNCTION__);
			break;
		}
		case kParkClima_CheckPrecondition:
		case kParkClima_Done:
		case kParkClima_New:
		default:{
			break;
		}
	}
	Handle_Error(res);
	return true;
}

bool Parking_Climate::Handle_Error(Parking_ClimateResult& result)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Parking_Climate::%s.", __FUNCTION__);
	SendResponse(result);
	return true;
}

bool Parking_Climate::SendResponse(Parking_ClimateResult& result)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Parking_Climate::%s.", __FUNCTION__);
    RemoveTimeout(m_TSPTimeoutId);
    if (!m_pGlyVdsRccSignal->SetResultPayload(result)){
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Parking_Climate:: %s, set result message failed", __FUNCTION__);
        return false;
    }
    fsm::VocmoSignalSource vocmo_signal_source;
    if (vocmo_signal_source.GeelySendMessage(m_pGlyVdsRccSignal))
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, " Parking_Climate::%s, success", __FUNCTION__);
    } else {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "Parking_Climate::%s, failed", __FUNCTION__);
    }
    return true;
}

void Parking_Climate::GetUTCTimer()
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Parking_Climate::%s.", __FUNCTION__);
	time(&m_timep);  // get time_t struct time, UTC time
	m_pCurrentTimer_tm = gmtime(m_timep); // change struct time_t to struct tm, UTC time;
	DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Parking_Climate::%s, Current time: %d-%d-%d %d %d:%d%d.", __FUNCTION__,
						1+m_pCurrentTimer_tm->tm_mon, m_pCurrentTimer_tm->tm_mday, m_pCurrentTimer_tm->tm_year, m_pCurrentTimer_tm->tm_wday,
						m_pCurrentTimer_tm->tm_hour, m_pCurrentTimer_tm->tm_min, m_pCurrentTimer_tm->tm_sec);
}

time_t Parking_Climate::MkTime(struct tm* pTimer)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Parking_Climate::%s.", __FUNCTION__);
	return mktime(pTimer);
}

std::string Parking_Climate::GetTimerIdFromSQL()
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Parking_Climate::%s.", __FUNCTION__);
	return "simple";
}

bool Parking_Climate::InsertTimerIdToSQL()
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Parking_Climate::%s.", __FUNCTION__);
	return true;
}

bool Parking_Climate::UpdateTimerIdToSQL()
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Parking_Climate::%s.", __FUNCTION__);
	return true;
}

std::vector<std::string> Parking_Climate::SplitString(std::string src, char delimiter)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Parking_Climate::%s.", __FUNCTION__);
	std::vector<std::string> _strVec;
	std::string::size_type pos1, pos2;
	pos2 = src.find(delimiter);
	pos1 = 0;
	while (std::string::npos != pos2){
		_strVec.push_back(src.substr(pos1, pos2 - pos1));
		pos1 = pos2 + 1;
		pos2 = src.find(delimiter, pos1);
	}
	_strVec.push_back(src.substr(pos1));
	return _strVec;
}

Parking_ClimateRequest StrVectorToRequestStruct(std::vector<std::string> strVec)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Parking_Climate::%s.", __FUNCTION__);
	Parking_ClimateRequest req;
	for(std::string& str: strVec){
		
	}
}

}