#include "fake_ipcb_service.h"

bool FakeIpcbService::registerOTAService(fgen){return true;}
void FakeIpcbService::deregisterOTAService(){}
bool FakeIpcbService::sendOTAServiceData(OperationId, OperationType, uint64_t, DataBase&){return true;}

bool FakeIpcbRvdcService::registerRVDCService(fgen){return true;}
void FakeIpcbRvdcService::deregisterRVDCService(){}
bool FakeIpcbRvdcService::sendRVDCServiceData(OperationId, OperationType, uint64_t, DataBase&){return true;}