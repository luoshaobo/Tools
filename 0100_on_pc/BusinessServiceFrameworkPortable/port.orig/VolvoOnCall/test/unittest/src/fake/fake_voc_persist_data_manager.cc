#include "voc_persist_data_mgr.h"
#include "fake_control.h"

namespace vocpersistdatamgr
{

VocPersistDataMgr &VocPersistDataMgr::GetInstance()
{
    static VocPersistDataMgr handler;
    return handler;
}

VocPersistDataMgr::VocPersistDataMgr(){}

VocPersistDataMgr::~VocPersistDataMgr() {}

bool VocPersistDataMgr::GetData(const VOC_PERSISTENT_ATTRIBUTE resource, uint32_t &outData)
{
    outData = FakeVocPersistDataControl::Get_MTA_Uploaded_Flag();
    return true;
}

bool VocPersistDataMgr::GetData(const VOC_PERSISTENT_ATTRIBUTE resource, std::string &outData)
{
    outData = "TEST";
    return true;
}

bool VocPersistDataMgr::SetData(vocpersistdatamgr::VOC_PERSISTENT_ATTRIBUTE, unsigned int flag)
{
    FakeVocPersistDataControl::Set_MTA_Uploaded_Flag(flag);
    return true;
}

}
