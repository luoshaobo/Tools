#ifndef FAKE_IPCB_SERVICE
#define FAKE_IPCB_SERVICE

#include "ipcb_IOTAService.hpp"
#include "ipcb_IRVDCService.hpp"
#include "ipcb_data.hpp"

typedef void (*fgen)(void);

class FakeIpcbService : public IpcbIOTAService
{
public :
    bool registerOTAService(fgen) override;
    void deregisterOTAService() override;
    bool sendOTAServiceData(OperationId, OperationType, uint64_t, DataBase&) override;
};

class FakeIpcbRvdcService : public IpcbIRVDCService
{
public :
    bool registerRVDCService(fgen) override;
    void deregisterRVDCService() override;
    bool sendRVDCServiceData(OperationId, OperationType, uint64_t, DataBase&) override;
};

#endif
