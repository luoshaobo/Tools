#include "ip_command_broker_signal_source.h"
#include "ipcb_IOTAService.hpp"
#include "fake_ipcb_service.h"

namespace fsm
{
IpCommandBrokerSignalSource::IpCommandBrokerSignalSource(){}
IpCommandBrokerSignalSource::~IpCommandBrokerSignalSource(){}
	
IpCommandBrokerSignalSource& IpCommandBrokerSignalSource::GetInstance()
{
    static IpCommandBrokerSignalSource instance;
    return instance;
}

IpcbIOTAService& IpCommandBrokerSignalSource::GetIpcbIOTAServiceObject()
{
    static FakeIpcbService mFake;
    return mFake;
}
IpcbIRVDCService& IpCommandBrokerSignalSource::GetIpcbRvdcServiceObject()
{
    static FakeIpcbRvdcService mFakeRvdc;
    return mFakeRvdc;
}
} //namespace

