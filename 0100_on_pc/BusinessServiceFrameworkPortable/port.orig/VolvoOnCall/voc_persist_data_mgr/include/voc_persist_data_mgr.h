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

#ifndef VOC_PERSIST_DATA_MGR_H
#define VOC_PERSIST_DATA_MGR_H

#include <map>
#include <string>

namespace vocpersistdatamgr
{

enum VOC_PERSISTENT_ATTRIBUTE : uint8_t
{
    SVT_START_TRACKING_TIME_SECONDS = 0,
    SVT_START_TRACKING_TIME_MILLISECONDS = 1,
    SVT_TRACKING_INTERVAL,
    SVT_STOP_TRACKING_TIME_SECONDS,
    SVT_STOP_TRACKING_TIME_MILLISECONDS,
    SVT_MIN_TRACKING_INTERVAL,
    CAR_MODE_STATE,
    CAR_CONFIG_SVT_PARAM,
    END_SEND_STANDBY_TIME,
    SVT_WAIT_FOR_GPS_TIME,
    SVT_WAIT_FOR_GPS_RETRY_TIME,
    // remote start engine: begin
    /**
     * \brief Timeout time for the VFC used to activate Engine Remote Start
     * range: 0 ~ 127 seconds, default: 3 seconds
     */
    ERS_VFC_TIMEOUT,
    /**
     * \brief Describes how long a ERS service is expected to run before it is considered to be 
     * time out
     * range: 0 ~ 127 minutes, default: 17 minutes
     */
    ERS_TIMOUT_TIME,
    /**
     * \brief Describes the timeout time for receiving a response from the CEM.
     * range: 0 ~ 255 seconds, default: 15 seconds.
     */
    ERS_RESPONSE_TIME,
    /**
     * \brief Describes how long a response that shall be forward to TSP is allowed to be stored in TCAM.
     * range: 0 ~ 4096 seconds, default: 1200 seconds.
     */
    ERS_RESPONSE_VALID_TIME,
    /**
     *\brief Describes the timeout time for expecting a update value of RS_APP_STATUS from the CEM.
     * range: 0 ~ 127 seconds, default: 2 seconds.
    */
    ERS_STATUS_UPDATE_TIME,
    /**
     *\brief Describes the timeout time of wakeup ers-vfc timer;
     * range: 0 ~ 127 seconds, default: 3 seconds.
    */
    ERS_VFC_WAKEUP_TIME,
    RCE_TSP_RESPONSE_TIME,
    RCE_CLIMA_REQ_TIMEOUT,
    RCE_RPC_VFC_TIMEOUT,
    // remote start end: end
    LOCK_STATUS_UPDATE_TIME,
    TR_UNLOCK_TIMEOUT,
    RDU_VFC_TIMEOUT,
    CLHAL_VFC_TIMEOUT,
    TC_VFC_TIMEOUT,
    WINDOW_VFC_TIMEOUT,
    LOCK_VALID_STATUS_TIME,
    UNLOCK_CHECK_TIME,
    WINDOW_STATUS_UPDATE_TIME,
    FLASH_STATUS_UPDATE_TIME,
    PM25_STATUS_UPDATE_TIME,
    WAIT_CAN_WAKEUP_TIME,
    MTA_UPLOADED_FLAG,
    DLA_CONFIGURATION_STATUS,
    DLA_DOWNLOAD_CONSENT,

    PERSISTENCE_ATTRIBUTE_MAX //must be last item, do not use
};

/**
* @brief Configuration key strings in persistence config file
*
* The keys are accessed using the PersistentAttribute enum.
* The strings defined here must match the keys used in persistence configuration file.
*/
const std::string vocPersistentAttributeKeyStrings[] =
{
    "svt_start_tracking_time_seconds",
    "svt_start_tracking_time_milliseconds",
    "svt_tracking_interval",
    "svt_stop_tracking_time_seconds",
    "svt_stop_tracking_time_milliseconds",
    "svt_min_tracking_interval",
    "car_mode_state",
    "car_config_svt_param",
    "end_send_standby_time",
    "svt_wait_for_gps_time",
    "svt_wait_for_gps_retry_time",
	// remote start engine: begin
    "ers_vfc_timeout",
    "ers_timeout_time",
    "ers_response_time",
    "ers_response_valid_time",
    "ers_status_update_time",
    "ers_vfc_wakeup_time",
    "rce_tsp_response_time",
    "rce_clima_req_timeout",
    "rce_rpc_vfc_timeout",
    // remote start engine: end
    "lock_status_update_time",
    "tr_unlock_timeout",
    "rdu_vfc_timeout",
    "clhal_vfc_timeout",
    "tc_vfc_timeout",
    "window_vfc_timeout",
    "lock_valid_status_time",
    "unlock_check_time",
    "window_status_update_time",
    "flash_status_update_time",
    "pm25_status_update_time",
    "wait_can_wakeup_time",
    "mta_uploaded_flag",
    "dla_configuration_status",
    "dla_download_consent",

    "persistence_attribute_max" //must be last item, do not use
};

class VocPersistDataMgr
{
public:
    static VocPersistDataMgr &GetInstance();

    // getter/setter for use with enum name (convenient in business logic code)

    /**
     * @brief Get integer persistence data
     * Get integer persistence data identified by key_name
     */
    bool GetData(const VOC_PERSISTENT_ATTRIBUTE attribute, uint32_t &outData);

    /**
     * @brief Get string persistence data
     * Get string persistence data identified by key_name
     */
    bool GetData(const VOC_PERSISTENT_ATTRIBUTE attribute, std::string &outData);

    /**
     * @brief Set integer persistence data
     * Set integer persistence data identified by key_name
     */
    bool SetData(const VOC_PERSISTENT_ATTRIBUTE attribute, uint32_t data);

    /**
     * @brief Set string persistence data
     * Set string persistence data identified by key_name
     */
    bool SetData(const VOC_PERSISTENT_ATTRIBUTE attribute, std::string &data);

    // getter/setter for use with string key name (used in config file)

    /**
     * @brief InitPersistenceData
     * Initialize PCL when needed.
     *
     * @note Take care to init IPC before!
     */
    bool InitPersistenceData();

    /**
     * @brief GetPersistenceData (int)
     * Get integer data from persistence
     */
    bool GetPersistenceData(const std::string &key_name, uint32_t &outData);

    /**
     * @brief GetPersistenceData (string)
     * Get string data from persistence
     */
    bool GetPersistenceData(const std::string &key_name, std::string &outData);

    /**
     * @brief SetPersistenceData (int)
     * Set integer data from persistence
     */
    bool SetPersistenceData(const std::string &key_name, uint32_t data);

    /**
     * @brief SetPersistenceData (string)
     * Set string data from persistence
     */
    bool SetPersistenceData(const std::string &key_name, std::string &data);

    ~VocPersistDataMgr();

private:

   VocPersistDataMgr();

    /**
     * @brief VocPersistDataMgr copy constructor
     * Delete the copy constructor
     */
    VocPersistDataMgr(const VocPersistDataMgr&) = delete;

    /**
     * @brief VocPersistDataMgr move constructor
     * Delete the move constructor
     */
    VocPersistDataMgr(const VocPersistDataMgr&&) = delete;

    /**
     * @brief Assignment operator
     * Delete the assignment operator
     */
    VocPersistDataMgr& operator=(const VocPersistDataMgr&) = delete;

    /**
     * @brief integerAttributeMap_
     * Chached integer values
     */
    std::map<std::string, uint32_t> integerAttributeMap_;

    /**
     * @brief integerAttributeMap_
     * Chached string values
     */
    std::map<std::string, std::string> stringAttributeMap_;

    /**
     * @brief m_bPCLInitialized
     * Indicates if PCL is initialized
     */
    bool m_bPCLInitialized;
};
} //vocpersistdatamgr

#endif // VOC_PERSIST_DATA_MGR_H
