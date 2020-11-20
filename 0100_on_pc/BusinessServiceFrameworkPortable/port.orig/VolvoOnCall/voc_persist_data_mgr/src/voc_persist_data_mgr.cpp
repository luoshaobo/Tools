///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//	@file voc_persist_data_mgr.h
//	Application library VocPersistDataMgr (libvoc_persist_data_mgr) common functions .

// @project		GLY_TCAM
// @subsystem	Application
// @author		linhaixia
// @Init date	01-Aug-2018
///////////////////////////////////////////////////////////////////

#include <memory>
#include <string>
#include <glib.h>
#include "dlt/dlt.h"
#include "persistence_client_library.h"
#include "voc_persist_data_mgr.h"


DLT_DECLARE_CONTEXT(dlt_voc);

namespace vocpersistdatamgr
{
VocPersistDataMgr &VocPersistDataMgr::GetInstance()
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s.\n", __FUNCTION__);
    static VocPersistDataMgr handler;
    return handler;
}

VocPersistDataMgr::VocPersistDataMgr() :
                m_bPCLInitialized(false)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s.\n", __FUNCTION__);
}

VocPersistDataMgr::~VocPersistDataMgr()
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s.\n", __FUNCTION__);
    tpPCL_deinit();
}

bool VocPersistDataMgr::InitPersistenceData()
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s.\n", __FUNCTION__);

    tpPCL_Error_t resultCode = E_PCL_ERROR_NONE;

    if (!m_bPCLInitialized)
    {
        resultCode = tpPCL_init("VolvoOnCall", nullptr, FALSE);

        if (E_PCL_ERROR_NONE != resultCode)
        {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "tpPCL_init failure resultCode=[%x]\n", resultCode);
            m_bPCLInitialized = false;
        }
        else
        {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "tpPCL_init success resultCode=[%x]\n", resultCode);
            m_bPCLInitialized = true;
        }
    }
    return m_bPCLInitialized;
}

bool VocPersistDataMgr::GetData(const VOC_PERSISTENT_ATTRIBUTE resource, uint32_t &outData)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s.\n", __FUNCTION__);

    bool retVal = false;
    
    std::string key_name = vocPersistentAttributeKeyStrings[static_cast<int>(resource)];
    auto it = integerAttributeMap_.find(key_name);
    if (it != integerAttributeMap_.end())
    {
        outData = it->second;
        retVal = true;
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Get Data from cache: key=[%s], value=[%d]\n", key_name.c_str(), outData);
    }
    else
    {
        retVal = GetPersistenceData(key_name, outData);
        integerAttributeMap_[key_name] = outData;
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Get Data from persistence: key=[%s], value=[%d]\n", key_name.c_str(), outData);
    }
    return retVal;
}

bool VocPersistDataMgr::GetData(const VOC_PERSISTENT_ATTRIBUTE resource, std::string &outData)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s.\n", __FUNCTION__);

    bool retVal = false;
    ;
    std::string key_name = vocPersistentAttributeKeyStrings[static_cast<int>(resource)];
    auto it = stringAttributeMap_.find(key_name);
    if (it != stringAttributeMap_.end())
    {
        outData = it->second;
        retVal = true;
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Get Data from cache: key=[%s], value=[%s]\n", key_name.c_str(), outData.c_str());
    }
    else
    {
        retVal = GetPersistenceData(key_name, outData);
        stringAttributeMap_[key_name] = outData;
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Get Data from persistence: key=[%s], value=[%s]\n", key_name.c_str(), outData.c_str());
    }

    return retVal;
}

bool VocPersistDataMgr::SetData(const VOC_PERSISTENT_ATTRIBUTE resource, uint32_t data)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s.\n", __FUNCTION__);

    bool retVal = false;
    std::string key_name = vocPersistentAttributeKeyStrings[static_cast<int>(resource)];
    retVal = SetPersistenceData(key_name, data);
    if (retVal)
    {
        integerAttributeMap_[key_name] = data;
    }
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Set Data in persistence: key=[%s], value=[%d]\n", key_name.c_str(), data);

    return retVal;
}

bool VocPersistDataMgr::SetData(const VOC_PERSISTENT_ATTRIBUTE resource, std::string &data)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s.\n", __FUNCTION__);

    bool retVal = false;
    std::string key_name = vocPersistentAttributeKeyStrings[static_cast<int>(resource)];

    retVal = SetPersistenceData(key_name, data);
    if (retVal)
    {
        stringAttributeMap_[key_name] = data;
    }
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Set Data in persistence: key=[%s], value=[%s]\n", key_name.c_str(), data.c_str());

    return retVal;
}

bool VocPersistDataMgr::GetPersistenceData(const std::string &key_name, uint32_t &outData)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s. key: %s\n", __FUNCTION__, key_name.c_str());

    bool retVal = false;
    tpPCL_Error_t resCode = E_PCL_ERROR_NONE;

    if (InitPersistenceData())
    {
        resCode = tpPCL_keyReadData(E_PCL_LDBID_PUBLIC, reinterpret_cast<const unsigned char *>(key_name.c_str()),
                        sizeof(outData), &outData);

        if (resCode != E_PCL_ERROR_NONE)
        {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "GetPersistenceData failure: key=[%s], resCode=[%d]\n", key_name.c_str(), resCode);
            retVal = false;
        }
        else
        {
            retVal = true;
        }
    }

    return retVal;
}

bool VocPersistDataMgr::GetPersistenceData(const std::string &key_name, std::string &outData)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s. key: %s\n", __FUNCTION__, key_name.c_str());

    bool retVal = false;
    gint sizebuf = 0;
    tpPCL_Error_t resCode = E_PCL_ERROR_NONE;

    if (InitPersistenceData())
    {
        resCode = tpPCL_keyGetSize(E_PCL_LDBID_PUBLIC, reinterpret_cast<const unsigned char*>(key_name.c_str()),
                        &sizebuf);

        if (E_PCL_ERROR_NONE == resCode && (0 != sizebuf))
        {
            std::unique_ptr<char[]> keyValue(new char[sizebuf + 1]());
            resCode = tpPCL_keyReadData(E_PCL_LDBID_PUBLIC, reinterpret_cast<const unsigned char*>(key_name.c_str()),
                            sizebuf, keyValue.get());

            if (E_PCL_ERROR_NONE != resCode)
            {
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "GetPersistenceData failure: key=[%s], resCode=[%d]\n", key_name.c_str(), resCode);
                retVal = false;
            }
            else
            {
                outData.assign(keyValue.get());
                retVal = true;
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GetPersistenceData sizeBuf  %d\n",sizebuf);     //fix klocwork
            }
        }
    }

    return retVal;
}

bool VocPersistDataMgr::SetPersistenceData(const std::string &key_name, uint32_t data)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s. key: %s, set value: %d\n", __FUNCTION__, key_name.c_str(), data);

    bool retVal = true;
    tpPCL_Error_t resCode = E_PCL_ERROR_NONE;

    if (InitPersistenceData())
    {
        resCode = tpPCL_keyWriteData(E_PCL_LDBID_PUBLIC, reinterpret_cast<const unsigned char*>(key_name.c_str()),
                        E_PCL_DATA_TYPE_INTEGER, sizeof(data), &data);

        if (E_PCL_ERROR_NONE != resCode)
        {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "SetPersistenceData failure: key=[%s], data=[%d], resCode=[%x]\n", key_name.c_str(),
                            data, resCode);
            retVal = false;
        }
    }
    else
    {
        retVal = false;
    }
    return retVal;
}

bool VocPersistDataMgr::SetPersistenceData(const std::string &key_name, std::string &data)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s. key: %s, set value: %s\n", __FUNCTION__, key_name.c_str(), data.c_str());

    bool retVal = true;
    tpPCL_Error_t resCode = E_PCL_ERROR_NONE;

    if (InitPersistenceData())
    {
        resCode = tpPCL_keyWriteData(E_PCL_LDBID_PUBLIC, reinterpret_cast<const unsigned char*>(key_name.c_str()),
                        E_PCL_DATA_TYPE_STRING, data.size(),
                        const_cast<void*>(reinterpret_cast<const void*>(data.c_str())));

        if (E_PCL_ERROR_NONE != resCode)
        {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "SetPersistenceData failure: key=[%s], data=[%s], resCode=[%x]\n", key_name.c_str(),
                            data.c_str(), resCode);
            retVal = false;
        }
    }
    else
    {
        retVal = false;
    }
    return retVal;
}


} //VocPersistdatamgr
