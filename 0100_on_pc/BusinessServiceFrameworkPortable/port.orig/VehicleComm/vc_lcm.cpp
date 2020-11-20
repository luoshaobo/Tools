/**
* Copyright (C) 2016 Continental Automotive GmbH
*
* Licensed under the "Supplier unrestricted white box" category, as
* described in the 20160322-1_SW-amendment TCAM.pdf agreement
* between Volvo Cars and Continental Automotive GmbH.
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*/

/** @file vc_lcm.cpp
 * This file handles the lifecycle manager functionality for VehicleComm.
 *
 * @project     VCC
 * @subsystem   VehicleComm
 * @version     00.01
 * @author      Johan Bohlin & Ola Lilja
 * @date        28-Dec-2016
 ***************************************************************************/

/***************************************************************************
 * HEADER FILE INCLUDES
 **************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include "vc_lcm.hpp"
#include "vc_utility.hpp"

namespace vc {

std::function<ReturnValue(VCLCMSignal)> LCM::lcmhandler_;

/**
    @brief Callback for LCM framework.

    This callback is registered into OTPs LCM framework. It will be called by the
    LCM framework to notify suspend and shutdown.
    @param[in]  shutdown_type      Shutdown type (suspend/resume)
    @return     Error return code to LCM framework
*/
tpLCM_ReturnCode_t LCM::ShutdownNotification(tpLCM_Shutdowntype_t shutdown_type)
{
    ReturnValue ret;
    tpLCM_AppInfo_t appinfo;
    tpLCM_ReturnCode_t retval = E_LCM_RET_FAILURE;

    shutdown_type == E_LCM_SHUTDOWN ? LOG(LOG_DEBUG, "LCM (%s): shutdown triggered : %d",__FUNCTION__, shutdown_type)
        : LOG(LOG_DEBUG, "LCM (%s): suspend triggered : %d",__FUNCTION__, shutdown_type);

    if(lcmhandler_ != NULL) {
        if(shutdown_type == E_LCM_SUSPEND) {
            ret = lcmhandler_(VCLCM_SUSPEND);
        } else {
            ret = lcmhandler_(VCLCM_SHUTDOWN);
        }
        if (ret != RET_OK)
            LOG(LOG_ERR, "LCM (%s): callbacks returned an error: %s",__FUNCTION__, ReturnValueStr[ret]);
    }
    else {
        LOG(LOG_ERR, "LCM (%s): no callbacks register to handle resume/suspend & shutdown",__FUNCTION__);
    }


    STRNCPY(appinfo.app_name, SERVICE_NAME, LCM_MAX_APPNAME_LENGTH);
    appinfo.app_user_id = getuid();
    appinfo.app_proc_id = getpid();
    appinfo.app_thread_id = 0;
    retval = tpLCM_shutdownReady(&appinfo);
    if(retval != E_LCM_RET_SUCCESS) {
        LOG(LOG_WARN, "LCM (%s): tpLCM_shutdownReady error: %d", __FUNCTION__, retval);
    }

    return E_LCM_RET_SUCCESS;
}


/**
    @brief Callback for LCM framework.

    This callback is registered into OTPs LCM framework. It will be called by the
    LCM framework to notify when to resume.
    @return     Error return code to LCM framework
*/
tpLCM_ReturnCode_t LCM::ResumeNotification(const tpLCM_WakeupReason_t wakeup_reason)
{
    LOG(LOG_DEBUG, "LCM (%s): resume triggered", __FUNCTION__);
    UNUSED(wakeup_reason);
    lcmhandler_(VCLCM_RESUME);
    return E_LCM_RET_SUCCESS;
}


/**
    @brief Constructor for the LCM-class.
*/
LCM::LCM()
{
    LOG(LOG_DEBUG, "LCM (%s):", __FUNCTION__);
}


/**
    @brief Init LCM framework for VehicleComm.

    This method takes care of the initialization work needed
    for VC to communicate with LCM framework. It takes a callback
    function as input in order to notify VehicleComm about different
    states.
    @param[in]  lcmhandler      function provided that LCM calls upon events.
    @return     A VC return value
*/
ReturnValue LCM::Init(std::function<ReturnValue(VCLCMSignal)> lcmhandler)
{
    tpLCM_AppInfo_t appinfo;
    tpLCM_ReturnCode_t ret_val;

    lcmhandler_ = NULL;

    LOG(LOG_DEBUG, "LCM (%s):", __FUNCTION__);

    ret_val = tpLCM_init();
    if (ret_val != E_LCM_RET_SUCCESS) {
        LOG(LOG_ERR, "LCM (%s): tpLCM_init failed! ret value : %d", __FUNCTION__, ret_val);
        return RET_ERR_EXTERNAL;
    }

    STRNCPY(appinfo.app_name, SERVICE_NAME, LCM_MAX_APPNAME_LENGTH);
    appinfo.app_user_id = getuid();
    appinfo.app_proc_id = getpid();
    appinfo.app_thread_id = 0;
    LOG(LOG_DEBUG,"LCM (%s): app_name:%s\t app_user_id:%d\t app_proc_id:%d\t app_thread_id : %d ", __FUNCTION__,
            appinfo.app_name, appinfo.app_user_id,appinfo.app_proc_id,appinfo.app_thread_id);
 
    ret_val = tpLCM_register(&appinfo);
    if (ret_val != E_LCM_RET_SUCCESS) {
        LOG(LOG_ERR,"LCM (%s): tpLCM_register failed! ret value : %d",__FUNCTION__, ret_val);
        return RET_ERR_EXTERNAL;
    }

    ret_val = tpLCM_registerShutdownSignal(ShutdownNotification);
    if (ret_val != E_LCM_RET_SUCCESS) {
        LOG(LOG_ERR,"LCM (%s): tpLCM_registerShutdownSignal failed! ret value : %d",__FUNCTION__, ret_val);
        return RET_ERR_EXTERNAL;
    }

    ret_val = tpLCM_registerResumeSignal(ResumeNotification);
    if (ret_val != E_LCM_RET_SUCCESS) {
        LOG(LOG_ERR,"LCM (%s): tpLCM_registerResumeSignal failed! ret value : %d",__FUNCTION__, ret_val);
        return RET_ERR_EXTERNAL;
    }

    lcmhandler_ = lcmhandler;

    return RET_OK;
}


/**
    @brief Destructor for the LCM-class.
*/
LCM::~LCM()
{
    LOG(LOG_DEBUG, "LCM (%s):", __FUNCTION__);
    tpLCM_ReturnCode_t ret_val;
    ret_val = tpLCM_deinit();
    if (ret_val != E_LCM_RET_SUCCESS) {
        LOG(LOG_ERR,"LCM (%s): tpLCM_deinit failed! ret value : %d", __FUNCTION__, ret_val);
    }
}

/**
    @brief Get the singleton instance of LCM
*/
LCM& LCM::GetInstance()
{
    LOG(LOG_DEBUG, "LCM (%s):", __FUNCTION__);
    static LCM inst;
    return inst;
}

} //namespace vc
