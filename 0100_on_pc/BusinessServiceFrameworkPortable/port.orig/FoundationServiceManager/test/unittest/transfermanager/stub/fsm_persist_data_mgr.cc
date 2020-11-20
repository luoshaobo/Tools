/**
 * Copyright (C) 2017 Continental Automotive GmbH
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
 *  \brief    Stub for Foundation Services Manager library FsmPersistDataMgr (libfsm_persist_data_mgr) implementation.
 *  \author   Florian Schindler
 *
  *  \addtogroup unittest
 *  \{
 */


#include "fsm_persist_data_mgr.h"
#include "unittest_http_cache_db.h"

namespace fsm
{

namespace fsmpersistdatamgr
{


FsmPersistDataMgr::FsmPersistDataMgr()
{
}

FsmPersistDataMgr::~FsmPersistDataMgr()
{
}

bool FsmPersistDataMgr::FsmPersistDataInit()
{
    return true;
}

std::string FsmPersistDataMgr::FsmPersistDataGet(const std::string &key_name)
{
    return SQLLITE_PERSISTENCY_TEST_DB_NAME;
}

std::string FsmPersistDataMgr::FsmPersistDataPathGet(const tpPCL_LogicalDatabaseId_t path_locator)
{
    std::string str_return;
    switch(path_locator)
    {
        case E_PCL_LDBID_LOCAL:
            str_return = SQLLITE_PERSISTENCY_LOCAL_TEST_DB_DIR;
            break;
        case E_PCL_LDBID_PUBLIC:
            str_return = SQLLITE_PERSISTENCY_PUBLIC_TEST_DB_DIR;
            break;
        default:
            str_return.clear();
    }
    return str_return;
}

} //fsmpersistdatamgr

} //fsm
/** \}    end of addtogroup */