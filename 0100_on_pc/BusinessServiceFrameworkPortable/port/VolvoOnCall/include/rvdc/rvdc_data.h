///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//	@file rvdc_data.h
//	This file handles the RVDC statemachine for application.

// @project 	GLY_TCAM
// @subsystem	Application
// @author		Nie Yujin
// @Init date	28-Jan-2019
///////////////////////////////////////////////////////////////////
#ifndef RVDC_DATA_H__
#define RVDC_DATA_H__

#include <vector>
#include <algorithm>

namespace volvo_on_call
{
static uint32_t currentMDPIndex;
static uint32_t currentSignalIndex;

enum SignalStatus
{
    eSigStsInit = 0,
    eSigStsWaitRun,
    eSigStsWaitAck,
    eSigStsDone
};

enum MDPUploadStatus
{
    eMDPUpStsInit = 0,
    eMDPUpStsWaitUpload,
    eMDPUpStsWaitAck,
    eMDPUpStsDel
};

typedef struct
{
    std::shared_ptr<fsm::Signal> signal;
    SignalStatus status;
} RvdcSignal_t;

typedef struct {
    OpRVDCMeasurementDataPackage_Request_Data data;
    MDPUploadStatus status;
} RvdcMDP_t;

class RvdcSignal
{
public:
    RvdcSignal()
        : m_signal(NULL)
        , m_status(eSigStsInit)
        , m_index(0)
    {
    }
	
    RvdcSignal(RvdcSignal_t data)
        : m_signal(data.signal)
        , m_status(data.status)
    {
        ++currentSignalIndex;
        m_index = currentSignalIndex;
    }

    uint32_t getIndex() const { return m_index; }
    
    void setStatus(SignalStatus status) { m_status = status; }
    SignalStatus getStatus() const { return m_status; }

    std::shared_ptr<fsm::Signal> getSignal() const { return m_signal; }

protected:
    
    std::shared_ptr<fsm::Signal> m_signal;
    SignalStatus m_status;
    uint32_t m_index;
    
private:
    
};

class RvdcMDP
{
public:
    RvdcMDP()
        : m_mdp()
        , m_status(eMDPUpStsInit)
        , m_index(0)

    {
    }
    
    RvdcMDP(OpRVDCMeasurementDataPackage_Request_Data data, MDPUploadStatus status)
        : m_mdp(data)
        , m_status(status)
    {
        ++currentMDPIndex;
        m_index = currentMDPIndex;
    }
    uint32_t getIndex() const { return m_index; }
    MDPUploadStatus getStatus() const { return m_status; }
    OpRVDCMeasurementDataPackage_Request_Data getMdp() const { return m_mdp; }
    uint32_t getPriority() const { return m_mdp.telematicsparameters.uploadpriority; }
    void setStatus(MDPUploadStatus status) { m_status = status; }

protected:
    OpRVDCMeasurementDataPackage_Request_Data m_mdp;
    MDPUploadStatus m_status;
    uint32_t m_index;
};
}
#endif // RVDC_DATA_H__