/**
 * Copyright (C) 2017-2018 Continental Automotive GmbH
 *
 * Licensed under the "Supplier unrestricted white box" category, as
 * described in the 20160322-1_SW-amendment TCAM.pdf agreement
 * between Volvo Cars and Continental Automotive GmbH.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *  \file     fsm_persist_data_mgr.cpp
 *  \brief    Foundation Services Manager library FsmPersistDataMgr (libfsm_persist_data_mgr) implementation.
 *  \author   Maksym Mozok
 *
 *  \addtogroup libfsm_persist_data_mgr
 *  \{
 */

// ** INCLUDES *****************************************************************

#include <cstdint>
#include <string>

#include <assert.h>

#include "dlt/dlt.h"
#include "persistence_client_library.h"
#include "fsm_persist_data_mgr.h"

DLT_DECLARE_CONTEXT(dlt_libfpdm);

namespace fsm
{

namespace fsmpersistdatamgr
{

void FsmPersistDataMgr::PersistDataInitSelf()
{
    DLT_REGISTER_CONTEXT(dlt_libfpdm, "LFPDM", "libfpdm logs");
    return;
}

FsmPersistDataMgr::FsmPersistDataMgr()
{
    PersistDataInitSelf();
    DLT_LOG_STRING(dlt_libfpdm, DLT_LOG_INFO, "Creating libfpdm");
    resource_db_file_ = kDefaultSSMPersistPathFilename;
    str_signal_service_uri_keyname_ = kDefaultSignalServiceURIKeyname;
}

FsmPersistDataMgr::FsmPersistDataMgr(const std::string &str_resource_path_file)
{
    PersistDataInitSelf();
    DLT_LOG_STRINGF(dlt_libfpdm, DLT_LOG_INFO, "Creating libfpdm for:%s", str_resource_path_file.c_str());
    if(0 != str_resource_path_file.size())
    {
        DLT_LOG_STRINGF(dlt_libfpdm, DLT_LOG_INFO, "Creating libfpdm for: %s", str_resource_path_file.c_str());
        resource_db_file_.assign(str_resource_path_file.c_str());
    }
    else
    {
        DLT_LOG_STRING(dlt_libfpdm, DLT_LOG_ERROR, "Creating libfpdm for default: /data/fsm/ssm.db");
        resource_db_file_ = kDefaultSSMPersistPathFilename;
    }
    str_signal_service_uri_keyname_ = kDefaultSignalServiceURIKeyname;
}

FsmPersistDataMgr::~FsmPersistDataMgr()
{
    tpPCL_deinit();
    DLT_LOG_STRING(dlt_libfpdm, DLT_LOG_INFO, "Terminating libfpdm");
    DLT_UNREGISTER_CONTEXT(dlt_libfpdm);
    return;
}

bool FsmPersistDataMgr::FsmPersistDataInit()
{
    int sizebuf = 0;
    char* storage_path = nullptr;
    bool boolianrc = true;
    tpPCL_Error_t rc = E_PCL_ERROR_NONE;

    DLT_LOG_STRING(dlt_libfpdm, DLT_LOG_INFO, "Entering FsmPersistDataInit libfpdm");
    rc = tpPCL_init("fsm_persist_data_mgr", nullptr, FALSE);

    if(E_PCL_ERROR_NONE != rc)
    {
        DLT_LOG_STRINGF(dlt_libfpdm, DLT_LOG_ERROR, "tpPCL_init rc=%x", rc);
        boolianrc = false; //critical error
    }
    else
    {
        rc = tpPCL_keyGetSize(E_PCL_LDBID_PUBLIC,
                              reinterpret_cast<const unsigned char*>(str_signal_service_uri_keyname_.c_str()),
                              &sizebuf);

        if(E_PCL_ERROR_NONE == rc && (0 != sizebuf))
        {
            char *key_value = new char[static_cast<ulong>(sizebuf+1)]();
            tpPCL_keyReadData(E_PCL_LDBID_PUBLIC,
                              reinterpret_cast<const unsigned char*>(str_signal_service_uri_keyname_.c_str()),
                              sizebuf,
                              key_value);
        str_url_ssm_.assign(key_value,static_cast<std::string::size_type>(sizebuf));
        delete[] key_value;
        }
        else
        {
            DLT_LOG_STRINGF(dlt_libfpdm, DLT_LOG_ERROR,
                            "Can't read str_signal_service_uri_keyname_: %s",
                            str_signal_service_uri_keyname_.c_str());
            boolianrc = false;
        }
        //The sizebuf return HAS /0 ENDING AS PART OF VALUABLE DATA
        //Temporary fix until OTP provides proper fix for datasize
        //ONLY tpPCL_getWriteablePath related!
        rc = tpPCL_getWriteablePath(E_PCL_LDBID_PUBLIC, &storage_path, &sizebuf);
        if(E_PCL_ERROR_NONE == rc)
        {
            public_storage_path_.assign(storage_path);
            DLT_LOG_STRINGF(dlt_libfpdm, DLT_LOG_INFO,
                            "Init public_storage_path_: %s",
                            public_storage_path_.c_str());
        }
        else
        {
            DLT_LOG_STRINGF(dlt_libfpdm, DLT_LOG_ERROR,
                            "Can't init public_storage_path_: %s",
                            public_storage_path_.c_str());
            boolianrc = false;
        }

        //The sizebuf return HAS /0 ENDING AS PART OF VALUABLE DATA
        //Temporary fix until OTP provides proper fix for datasize
        //ONLY tpPCL_getWriteablePath related!
        local_storage_path_ = "undefined";
        /*rc = tpPCL_getWriteablePath(E_PCL_LDBID_LOCAL, &storage_path, &sizebuf);
        if(E_PCL_ERROR_NONE == rc)
        {
            local_storage_path_.assign(storage_path);
            DLT_LOG_STRINGF(dlt_libfpdm, DLT_LOG_INFO,
                            "Init local_storage_path_: %s",
                            local_storage_path_.c_str());
        }
        else
        {
            DLT_LOG_STRINGF(dlt_libfpdm, DLT_LOG_ERROR,
                            "Can't init local_storage_path_: %s",
                            local_storage_path_.c_str());
        }*/
    }

    //modify klocwork warning,yangjun add 20181107
    if(storage_path)
    {
        free(storage_path);
        storage_path = nullptr;
    }
    return boolianrc;
}

bool FsmPersistDataMgr::FsmPersistDataSync()
{
    //TODO: resource sync routine implementation here
    return true;
}

void FsmPersistDataMgr::FsmPersistDataPurge()
{

    FsmPersistDataSet(str_signal_service_uri_keyname_, kDefaultPurgeString);
    return;
}

std::string FsmPersistDataMgr::FsmPersistDataGet(const std::string &key_name)
{
    int sizebuf = 0;
    std::string string_to_return;
    tpPCL_Error_t rc = E_PCL_ERROR_NONE;

    if(0 == key_name.size())
    {
        return string_to_return.erase();
    }

    rc = tpPCL_keyGetSize(E_PCL_LDBID_PUBLIC, reinterpret_cast<const unsigned char*>(key_name.c_str()), &sizebuf);

    if(E_PCL_ERROR_NONE == rc && (0 != sizebuf))
    {
        char *key_value = new char[static_cast<ulong>(sizebuf+1)]();
        tpPCL_keyReadData(E_PCL_LDBID_PUBLIC,
                          reinterpret_cast<const unsigned char*>(key_name.c_str()),
                          sizebuf,
                          key_value);

        string_to_return.assign(key_value,static_cast<std::string::size_type>(sizebuf)).shrink_to_fit();

        DLT_LOG_STRINGF(dlt_libfpdm, DLT_LOG_INFO, "FsmPersistDataGet key_name: %s value: %s",
                                                   key_name.c_str(), key_value);

        delete[] key_value;
    }

    return string_to_return;

}

bool FsmPersistDataMgr::FsmPersistDataSet(const std::string &key_name, const std::string &str_value)
{
    int32_t rc = 0;

    if(0 == key_name.size())
    {
        DLT_LOG_STRING(dlt_libfpdm, DLT_LOG_ERROR, "FsmPersistDataSet key_name unknown");
        return false;
    }

    rc = tpPCL_keyWriteData(E_PCL_LDBID_PUBLIC,
                            reinterpret_cast<const unsigned char*>(key_name.c_str()),
                            E_PCL_DATA_TYPE_STRING,
                            str_value.size(),
                            const_cast<void*>(reinterpret_cast<const void*>(str_value.c_str())));

    if(E_PCL_ERROR_NONE != rc)
    {
        DLT_LOG_STRINGF(dlt_libfpdm, DLT_LOG_ERROR, "FsmPersistDataSet tpPCL_keyWriteData rc = %x", rc);
        return false;
    }
    return true;
}

std::string FsmPersistDataMgr::FsmPersistDataPathGet(const tpPCL_LogicalDatabaseId_t path_locator)
{
    std::string str_return;
    switch(path_locator)
    {
        case E_PCL_LDBID_LOCAL:
            str_return = local_storage_path_;
            DLT_LOG_STRING(dlt_libfpdm, DLT_LOG_ERROR, "FsmPersistDataSet E_PCL_LDBID_LOCAL workaround, exiting");
            assert(false && "WORKAROUND: can't get path for E_PCL_LDBID_LOCAL, exiting");
            break;
        case E_PCL_LDBID_PUBLIC:
            str_return = public_storage_path_;
            break;
        default:
            str_return.clear();
    }
    return str_return;
}
} //fsmpersistdatamgr

} //fsm
/** \}    end of addtogroup */
