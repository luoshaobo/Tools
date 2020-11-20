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
 *  \file     fsm_persist_data_mgr.h
 *  \brief    Foundation Services Manager library FsmPersistDataMgr (libfsm_persist_data_mgr) common functions.
 *  \author   Maksym Mozok
 *
 *  \addtogroup libfsm_persist_data_mgr
 *  \{
 */

#ifndef FSM_PERSIST_DATA_MGR_H
#define FSM_PERSIST_DATA_MGR_H

#include <string>
#include "persistence_client_library.h"
/**
         * \brief string Default persistant absolut path to db file .
         */

const std::string kDefaultSSMPersistPathFilename ("/fsm/ssm.db");
        /**
         * \brief string Default persistant MQTT signal service uri keyname .
         */
const std::string kDefaultSignalServiceURIKeyname ("signal_service_uri");
        /**
         * \brief string Default purge string value.
         */
const std::string kDefaultPurgeString("");

namespace fsm
{
namespace fsmpersistdatamgr
{
    class FsmPersistDataMgr
    {
        std::string str_url_ssm_;
        std::string str_signal_service_uri_keyname_;
        std::string resource_db_file_;
        std::string local_storage_path_;
        std::string public_storage_path_;

        void UpdateUrlNotifyCb(...); //TODO: resource sync routine implementation here
        void PersistDataInitSelf();

    public:
        /**
         * \brief Default constructor. Creates FsmPersistDataMgr instance with default
         *        data values.
         */
        FsmPersistDataMgr();
        /**
         * \brief Constructor. Creates the FsmPersistDataMgr instance.
         * \param[in] str_resource_path_file String absolute path to persist file.
         *            Default value is kDefaultSSMPersistPathFilename
         */
        FsmPersistDataMgr(const std::string &str_resource_path_file);
        /**
         * \brief Destructor.
         */
        ~FsmPersistDataMgr();
        /**
         * \brief Inits the FsmPersistDataMgr instance.
         *        default value is kDefaultSSMPersistPathFilename
         * \return true on success
         */
        bool FsmPersistDataInit();
        /**
         * \brief TODO: Sync setup the FsmPersistDataMgr resource changes.
         * \return true on success
         */
        bool FsmPersistDataSync(); //TODO: resource sync routine implementation here
        /**
         * \brief Purge signal_service_uri from the FsmPersistDataMgr resource.
         */
        void FsmPersistDataPurge(); //TODO: purge any additional persist data from here.
        /**
         * \brief Get Key value from the FsmPersistDataMgr instance.
         * \param[in] key_name String, Key_name to get.
         * \param[out] String, returns key_value.
         */
        std::string FsmPersistDataGet(const std::string &key_name);
        /**
         * \brief Get Key value of the the FsmPersistDataMgr instance.
         * \param[in] key_name String key_name to set.
         * \param[in] str_value String str_value to set.
         * \return true on success.
         */
        bool FsmPersistDataSet( const std::string &key_name, const std::string &str_value );
        /**
         * \brief Get path to persistent storage.
         * \param[in] path_locator const tpPCL_LogicalDatabaseId_t. Can be E_PCL_LDBID_LOCAL or E_PCL_LDBID_PUBLIC
         * \return String, returns path string.
         */
         std::string FsmPersistDataPathGet(const tpPCL_LogicalDatabaseId_t path_locator);
    };

} //fsmpersistdatamgr

} //fsm
#endif // FSM_PERSIST_DATA_MGR_H
/** \}    end of addtogroup */
