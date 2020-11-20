/**
* Copyright (C) 2018 Continental Automotive GmbH
*
* Licensed under the "Supplier unrestricted white box" category, as
* described in the 20160322-1_SW-amendment TCAM.pdf agreement
* between Volvo Cars and Continental Automotive GmbH.
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*/

/**
 *
 * @file vpom_lcm.cpp
 * @brief This file handles the lifecycle manager functionality for VPOM
 *
 */

/***************************************************************************
 * HEADER FILE INCLUDES
 **************************************************************************/
#include <cstring>
#include "vpom_lcm.hpp"
#include <algorithm>

#define STRNCPY(dst,src,max_sz) std::strncpy(dst,src,max_sz-1); dst[max_sz-1]='\0'

DLT_IMPORT_CONTEXT(dlt_vpom);

/**
    @brief Callback for LCM framework.

    This callback is registered into OTPs LCM framework. It will be called by the
    LCM framework to notify suspend and shutdown.
    @param[in]  shutdown_type      Shutdown type (suspend/resume)
    @return     Error return code to LCM framework
*/
tpLCM_ReturnCode_t VpomLCM::ShutdownNotification(tpLCM_Shutdowntype_t shutdown_type)
{
    VpomLCM& VpomLCM_(VpomLCM::getInstance());

    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "%s: ", __FUNCTION__);

    //Do the work in a new thread so that we can return asap
    if (VpomLCM_.tmpThr_.joinable()) {
        VpomLCM_.tmpThr_.join();
    }
    VpomLCM_.tmpThr_ = std::thread(std::bind(&VpomLCM::ShutdownNotification_, &VpomLCM_, shutdown_type));
    VpomLCM_.tmpThr_.detach();

    return E_LCM_RET_SUCCESS;
}

tpLCM_ReturnCode_t VpomLCM::ShutdownNotification_(tpLCM_Shutdowntype_t shutdown_type)
{
    tpLCM_AppInfo_t appinfo;
    tpLCM_ReturnCode_t retval = E_LCM_RET_FAILURE;
    int ret;

    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "VpomLCM (%s): Suspend/Shutdown  triggered", __FUNCTION__);

    if (vpomLcmHandler_ != nullptr) {
        VpomLcmSignal sig;
        if (shutdown_type == E_LCM_SUSPEND) {
            sig = VPOM_LCM_SUSPEND;
        } else {
            sig = VPOM_LCM_SHUTDOWN;
        }
        ret = vpomLcmHandler_(sig);
        if (ret != 0) {
            DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_ERROR, "VpomLCM (%s): vpomLcmHandler_ returned an error: %d", __FUNCTION__, ret);
        }
    }
    else {
        DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_ERROR, "VpomLCM (%s): vpomLcmHandler_ missing", __FUNCTION__);
    }

    STRNCPY(appinfo.app_name, SERVICE_NAME, LCM_MAX_APPNAME_LENGTH);
    appinfo.app_user_id = getuid();
    appinfo.app_proc_id = getpid();
    appinfo.app_thread_id = 0;

    retval = tpLCM_shutdownReady(&appinfo);
    if (retval != E_LCM_RET_SUCCESS) {
        DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_WARN, "VpomLCM (%s): tpLCM_shutdownReady error: %d", __FUNCTION__, retval);
    }

    return E_LCM_RET_SUCCESS;
}

/**
    @brief Callback for LCM framework.

    This callback is registered into OTPs LCM framework. It will be called by the
    LCM framework to notify when to resume.
    @return     Error return code to LCM framework
*/
tpLCM_ReturnCode_t VpomLCM::ResumeNotification(const tpLCM_WakeupReason_t wakeup_reason)
{
    VpomLCM& VpomLCM_(VpomLCM::getInstance());

    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "%s: ", __FUNCTION__);

    //Do the work in a new thread so that we can return asap
    if (VpomLCM_.tmpThr_.joinable()) {
        VpomLCM_.tmpThr_.join();
    }
    VpomLCM_.tmpThr_ = std::thread(std::bind(&VpomLCM::ResumeNotification_, &VpomLCM_, wakeup_reason));
    VpomLCM_.tmpThr_.detach();

    return E_LCM_RET_SUCCESS;
}

tpLCM_ReturnCode_t VpomLCM::ResumeNotification_(const tpLCM_WakeupReason_t wakeup_reason)
{
    UNUSED_VAR(wakeup_reason);

    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "VpomLCM (%s): Resume triggered", __FUNCTION__);

    vpomLcmHandler_(VPOM_LCM_RESUME);

    return E_LCM_RET_SUCCESS;
}

/**
    @brief Constructor for the VpomLCM-class.
*/
VpomLCM::VpomLCM()
{
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "VpomLCM (%s)", __FUNCTION__);
}

/**
    @brief Init LCM framework for VolvoConnectionManager.

    This method takes care of the initialization work needed
    for VPOM to communicate with LCM framework. It takes a callback
    function as input in order to notify VPOM about different
    states.
    @return     A return value
*/
bool VpomLCM::Init(std::function<int(VpomLcmSignal)> vpomLcmHandler)
{
    VpomLCM& VpomLCM_(VpomLCM::getInstance());

    tpLCM_AppInfo_t appinfo;
    tpLCM_ReturnCode_t ret_val;

    VpomLCM_.vpomLcmHandler_ = nullptr;

    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "VpomLCM (%s)", __FUNCTION__);

    ret_val = tpLCM_init();
    if (ret_val != E_LCM_RET_SUCCESS) {
        DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_ERROR, "VpomLCM (%s): tpLCM_init failed! ret value : %d", __FUNCTION__, ret_val);
        return false;
    }

    STRNCPY(appinfo.app_name, SERVICE_NAME, LCM_MAX_APPNAME_LENGTH);
    appinfo.app_user_id = getuid();
    appinfo.app_proc_id = getpid();
    appinfo.app_thread_id = 0;
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "VpomLCM (%s): app_name:%s\t app_user_id:%d\t app_proc_id:%d\t app_thread_id : %d", __FUNCTION__,
                                              appinfo.app_name, appinfo.app_user_id,appinfo.app_proc_id,appinfo.app_thread_id);

    ret_val = tpLCM_register(&appinfo);
    if (ret_val != E_LCM_RET_SUCCESS) {
        DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_ERROR, "VpomLCM (%s): tpLCM_register failed! ret value : %d",__FUNCTION__, ret_val);
        return false;
    }

    ret_val = tpLCM_registerShutdownSignal(ShutdownNotification);
    if (ret_val != E_LCM_RET_SUCCESS) {
        DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_ERROR, "VpomLCM (%s): tpLCM_registerShutdownSignal failed! ret value : %d",__FUNCTION__, ret_val);
        return false;
    }

    ret_val = tpLCM_registerResumeSignal(ResumeNotification);
    if (ret_val != E_LCM_RET_SUCCESS) {
        DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_ERROR, "VpomLCM (%s): tpLCM_registerResumeSignal failed! ret value : %d",__FUNCTION__, ret_val);
        return false;
    }

    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "VpomLCM (%s): Done", __FUNCTION__);

    VpomLCM_.vpomLcmHandler_ = vpomLcmHandler;

    return true;
}


/**
    @brief Destructor for the VpomLCM-class.
*/
VpomLCM::~VpomLCM()
{
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "VpomLCM (%s)", __FUNCTION__);
    tpLCM_ReturnCode_t ret_val;
    ret_val = tpLCM_deinit();
    if (ret_val != E_LCM_RET_SUCCESS) {
        DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_ERROR, "VpomLCM (%s): tpLCM_deinit failed! ret value : %d", __FUNCTION__, ret_val);
    }
}

/**
    @brief Get the singleton instance of VpomLCM
*/
VpomLCM& VpomLCM::getInstance()
{
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "VpomLCM (%s)", __FUNCTION__);
    static VpomLCM inst;
    return inst;
}
