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

/** @file vehicle_comm.cpp
 * This file handles the core-functions of VehicleComm.
 *
 * @project     VCC
 * @subsystem   VehicleComm
 * @version     00.01
 * @author      Ola Lilja & Johan Bohlin
 * @date        07-Dec-2016
 ***************************************************************************/

#include <fcntl.h>
#include <execinfo.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "persistence_client_library.h"

#ifdef VC_TEST
#include "vehicle_comm_test.hpp"
#endif

#include "vehicle_comm.hpp"
#include "vc_utility.hpp"

namespace vc {

// Private

long MessageBase::unique_id_current = 0;
long VehicleComm::session_id_next_ = 0;

void VehicleComm::SignalHandler(int sig)
{
    UNUSED(sig);
    LOG(LOG_DEBUG, "%s.", __FUNCTION__);
    VehicleComm::GetInstance().Stop();
}

void VehicleComm::StackTraceSignalHandler(int sig)
{
    int j, nptrs;
    void *buffer[TRACE_SIZE];
    char **syms;

    nptrs = backtrace(buffer, TRACE_SIZE);
    syms = backtrace_symbols(buffer, nptrs);

    if (syms != NULL && nptrs != 0) {
        LOG(LOG_ERR, "*** Backtrace begin ***");
        for (j = 0; j < nptrs; j++) LOG(LOG_ERR, "%s", syms[j]);
        LOG(LOG_ERR, "*** Backtrace end ***");
    }
    else {
        LOG(LOG_ERR, "No trace available (nptrs=%d)", nptrs);
    }

    /*Default handling for this signal*/
    signal(sig,SIG_DFL);
    raise(sig);
}

void VehicleComm::ServiceAvailable(const gchar* service_name)
{
    LOG(LOG_INFO, "VehicleComm: %s: %s.", __FUNCTION__, service_name);
    if (strncmp(LIFECYCLEMGR_SERVICE_NAME, service_name, sizeof(LIFECYCLEMGR_SERVICE_NAME)) == 0) {
        /*ReturnValue ret = VehicleComm::GetInstance().lcm_.Init([](VCLCMSignal sig){return VehicleComm::GetInstance().LCMHandler(sig);});
        if (ret != RET_OK) {
            LOG(LOG_WARN, "VehicleComm: %s Failed to initialize LCM (ret = %s)!", __FUNCTION__, ReturnValueStr[ret]);
            return;
        }*/
    }
}

long VehicleComm::GetSessionID() {
    return session_id_next_++;
};

#ifdef VC_PRINTMEM
void VehicleComm::PrintMem()
{
    pid_t pid = getpid();

    std::stringstream ss;
    std::string str;
    ss << "/proc/" << pid << "/statm";

    std::fstream fs(ss.str());
    getline(fs, str);

    LOG(LOG_INFO, "VehicleComm: %s MEMORY: %s!", __FUNCTION__, str.c_str());
}
#endif

/**
    @brief Route messages to their destinations.

    This method will go through the messages in the message-queue mq,
    and send them to their destinations. This is done by calling the
    SendMessage-method of the endpoint-class associated with the destination
    endpoint of a message.

    @param[in]  mq     The message queue for all messages in VC
    @return     A VC return value
*/
ReturnValue VehicleComm::HandleMessage(MessageQueue &mq)
{
    int i = 0;
    size_t n = mq.Size();
    while (mq.Size() > 0) {
        MessageBase *m = mq.GetMessage();
        ReturnValue ret;

        if (m == NULL) {
            LOG(LOG_WARN, "VehicleComm: %s: message is NULL!", __FUNCTION__);
            return RET_ERR_INTERNAL;
        }

        i+=1;
        LOG(LOG_INFO, "VehicleComm: %s: Handling message %s (unique_id = %d, client_id = %d) [%d/%d].",
                __FUNCTION__, MessageStr(m), m->unique_id_, m->client_id_, i, n);
        //PrintMessage(m);

        LOG(LOG_DEBUG, "VehicleComm: %s: Message %s going to %s.", __FUNCTION__, MessageStr(m), EndpointStr[m->dst_]);

        if (m->dst_ == ENDPOINT_UNKNOWN)
            LOG(LOG_DEBUG, "VehicleComm: %s: Discarding message.", __FUNCTION__);
        else {
            // TODO: Better error-message handling. Could lead to error-message of an error-message.
            switch (m->dst_) {
            case ENDPOINT_VUC: {
                ret = vuc_.SendMessage(m);
                break;
            }
            case ENDPOINT_LAN: {
                ret = lan_.SendMessage(m);
                break;
            }
            case ENDPOINT_VGM: {
                ret = vgm_.SendMessage(m);
                break;
            }
            case ENDPOINT_IHU: {
                ret = ihu_.SendMessage(m);
                break;
            }
            case ENDPOINT_TS: {
                ret = ts_.SendMessage(m);
                break;
            }
            case ENDPOINT_DS: {
                ret = ds_.SendMessage(m);
                break;
            }
            case ENDPOINT_MP: {
                ret = mp_.SendMessage(m);
                break;
            }
            default:
                LOG(LOG_INFO, "VehicleComm: %s: Unknown message!", __FUNCTION__);
                break;
            }
            if (ret != RET_OK)
                LOG(LOG_WARN, "VehicleComm: %s: SendMessage failed error: %s", __FUNCTION__, ReturnValueStr[ret]);
            else if (HasGenericResponse(m)) {
                LOG(LOG_INFO, "VehicleComm: %s: Send RES_REQUESTSENT-response to request %s.", __FUNCTION__, VCRequestStr[m->id_]);
                MessageBase *m_res = InitMessage((MessageID)RES_REQUESTSENT, MESSAGE_RESPONSE,  m->client_id_,
                                                m->session_id_, m->dst_, m->src_, 0);
                if ((m_res != NULL) && (mq_.AddMessage(m_res) != RET_OK)) {
                    LOG(LOG_WARN, "VuCComm: %s Failed to send RES_REQUESTSENT-response!", __FUNCTION__);
                    delete(m_res);
                    SendErrorMessage(&mq_, m, ret);
                }
            }
        }
        mq.RemoveMessage();
        delete(m);
#ifdef VC_PRINTMEM
        PrintMem();
#endif
    }
    return RET_OK;
}

ReturnValue VehicleComm::Suspend()
{
    LOG(LOG_INFO, "VehicleComm: %s: Suspend called.", __FUNCTION__);
    return RET_OK;
}

ReturnValue VehicleComm::Resume()
{
    LOG(LOG_INFO, "VehicleComm: %s: Resume called.", __FUNCTION__);
    return RET_OK;
}

ReturnValue VehicleComm::LCMHandler(VCLCMSignal sig)
{
    LOG(LOG_DEBUG, "VehicleComm: %s.", __FUNCTION__);
    switch(sig) {
    case VCLCM_RESUME:
        return Resume();
    case VCLCM_SUSPEND:
        return Suspend();
    case VCLCM_SHUTDOWN:
        return Stop();
    default:
        LOG(LOG_ERR, "VehicleComm: %s: LCM-signal unknown!", __FUNCTION__);
        return RET_ERR_INTERNAL;
    }
}



// Public

VehicleComm::VehicleComm()
    : mq_(&cv_), ds_(&mq_), vuc_(&mq_, &ds_), lan_(&mq_), vgm_(&mq_, &lan_), ihu_(&mq_, &lan_), ts_(&mq_), mp_(&mq_, &ds_),
        wd_(Watchdog::GetInstance()), lcm_(LCM::GetInstance()), exit_(false)
{
}

VehicleComm::~VehicleComm()
{
    LOG(LOG_DEBUG, "VehicleComm: %s.", __FUNCTION__);
    #ifdef VC_TEST
    VehicleCommTest::StopTestThread();
    #endif
    tpSYS_deinitIPC();
    DLT_UNREGISTER_APP();
}

/**
    @brief Init the VehicleComm instance.

    This method takes care of the initialization work needed
    for VC.
    @return     A VC return value
*/
ReturnValue VehicleComm::Init()
{
    ReturnValue ret;
    signal(SIGINT,  SignalHandler);
    signal(SIGTERM, SignalHandler);
    signal(SIGHUP,SIG_IGN); /* Avoid restart daemon after use of serial port
            /dev/ttyHSL0 and restart debug-console.service*/
    signal(SIGSEGV, StackTraceSignalHandler);
    signal(SIGABRT, StackTraceSignalHandler);

    // Init ipc that is used for both VuCComm and LCM
    tpSYS_ReturnCode_t tpSYS_ret;
    tpSYS_ret = tpSYS_initIPC(E_SYS_IPC_DBUS, NULL, NULL);
    if (tpSYS_ret == E_SYS_IPC_RET_FAILURE) {
        LOG(LOG_ERR, "VehicleComm: %s: Failed to init tpSYS_initIPC!", __FUNCTION__);
        return RET_ERR_EXTERNAL;
    } else
        LOG(LOG_INFO, "VehicleComm: %s: tpSYS_initIPC initialized.", __FUNCTION__);

    // Load DataStorage
    LOG(LOG_INFO, "VehicleComm: %s: Starting DataStorage.", __FUNCTION__);
    ret = ds_.Init();
    if(ret != RET_OK) {
        LOG(LOG_ERR, "Failed to initialize DataStorage (ret = %s)!", ReturnValueStr[ret]);
        return ret;
    }

    // Start VuCComm
    LOG(LOG_INFO, "VehicleComm: %s: Starting VuCComm.", __FUNCTION__);
    ret = vuc_.Init();
    if(ret != RET_OK) {
        LOG(LOG_ERR, "Failed to initialize VuCComm (ret = %s)!", ReturnValueStr[ret]);
        return ret;
    }
    ret = vuc_.Start();
    if(ret != RET_OK) {
        LOG(LOG_ERR, "Failed to start VUCComm (ret = %s)!", ReturnValueStr[ret]);
        return ret;
    }

    // Start LANComm
    LOG(LOG_INFO, "VehicleComm: %s: Starting LANComm.", __FUNCTION__);
    ret = lan_.Init();
    if(ret != RET_OK) {
        LOG(LOG_WARN, "VehicleComm: %s: Failed to initialize LANComm (ret = %s)!", __FUNCTION__, ReturnValueStr[ret]);
        LOG(LOG_WARN, "VehicleComm: %s: No communication with SomeIP is possible!", __FUNCTION__);
    } else {
        ret = lan_.Start();
        if(ret != RET_OK) {
            LOG(LOG_ERR, "Failed to start LANComm (ret = %s)!", ReturnValueStr[ret]);
            return ret;
        }
    }

    // Start VGMComm
    LOG(LOG_INFO, "VehicleComm: %s: Starting VGMComm.", __FUNCTION__);
    ret = vgm_.Init();
    if(ret != RET_OK) {
        LOG(LOG_ERR, "Failed to initialize VGMComm (ret = %s)!", ReturnValueStr[ret]);
        return ret;
    }
    ret = vgm_.Start();
    if(ret != RET_OK) {
        LOG(LOG_ERR, "Failed to start VGMComm (ret = %s)!", ReturnValueStr[ret]);
        return ret;
    }

    // Start IHUComm
    LOG(LOG_INFO, "VehicleComm: %s: Starting IHUComm.", __FUNCTION__);
    ret = ihu_.Init();
    if(ret != RET_OK) {
        LOG(LOG_ERR, "Failed to initialize IHUComm (ret = %s)!", ReturnValueStr[ret]);
        return ret;
    }
    ret = ihu_.Start();
    if(ret != RET_OK) {
        LOG(LOG_ERR, "Failed to start IHUComm (ret = %s)!", ReturnValueStr[ret]);
        return ret;
    }

    // Start TSComm
    LOG(LOG_INFO, "VehicleComm: %s: Starting TSComm.", __FUNCTION__);
    ret = ts_.Init();
    if(ret != RET_OK) {
        LOG(LOG_ERR, "Failed to initialize TSComm (ret = %s)!", ReturnValueStr[ret]);
        return ret;
    }
    ret = ts_.Start();
    if(ret != RET_OK) {
        LOG(LOG_ERR, "Failed to start TSComm (ret = %s)!", ReturnValueStr[ret]);
        return ret;
    }

    // Start MessageProcessor
    ret = mp_.Init();
    if (ret != RET_OK) {
        LOG(LOG_ERR, "Failed to initialize MessageProcessor (ret = %s)!", ReturnValueStr[ret]);
        return ret;
    }
    ret = mp_.Start();
    if(ret != RET_OK) {
        LOG(LOG_ERR, "Failed to start MessageProcessor (ret = %s)!", ReturnValueStr[ret]);
        return ret;
    }

    // Start Life cycle manager - temporary removed due to platform issues
    tpSYS_ret = tpSYS_notifyServiceReady(LIFECYCLEMGR_SERVICE_NAME, false, false, VehicleComm::ServiceAvailable);
    if (tpSYS_ret == E_SYS_IPC_RET_FAILURE) {
        LOG(LOG_ERR, "VehicleComm: %s: Failed to register service notification for LCM!", __FUNCTION__);
        return RET_ERR_EXTERNAL;
    } else
        LOG(LOG_INFO, "VehicleComm: %s: registered LCM service notifier", __FUNCTION__);

    #ifdef VC_TEST
    // Start test-thread
    LOG(LOG_DEBUG, "VehicleComm: %s: Starting test-thread.", __FUNCTION__);
    VehicleCommTest::StartTestThread(&mq_);
    #endif

    return ret;
}

/**
    @brief Start VehicleComm.

    This method starts the execution of message-handling.
    @return     A VC return value
*/
ReturnValue VehicleComm::Start()
{
    LOG(LOG_INFO, "VehicleComm: %s: Entering message-loop...", __FUNCTION__);
    while (!exit_) {
        std::unique_lock<std::mutex> lck(mtx_);
        while(!mq_.NewMessages() && !exit_) {
            wd_.SetState(WD_THREAD_VEHICLECOMM, WD_THREAD_STATE_SLEEPING);
            cv_.wait(lck);
        }
        wd_.SetState(WD_THREAD_VEHICLECOMM, WD_THREAD_STATE_RUNNING);
        if(!exit_) {
            LOG(LOG_INFO, "VehicleComm: %s: %d new messages.", __FUNCTION__, mq_.Size());
            HandleMessage(mq_);
            mq_.SetNewMessages(false);
        }
    }

    LOG(LOG_INFO, "VehicleComm: %s: Exiting message-loop...", __FUNCTION__);
    return RET_OK;
}

/**
    @brief Stop VehicleComm.

    This method stops the execution of message-handling.
    @return     A VC return value
*/
ReturnValue VehicleComm::Stop()
{
    LOG(LOG_INFO, "VehicleComm: %s: Stop called.", __FUNCTION__);
    mp_.Stop();
    ihu_.Stop();
    vgm_.Stop();
    vuc_.Stop();
    exit_ = true;
    cv_.notify_all();
    return RET_OK;
}

VehicleComm& VehicleComm::GetInstance() {
    LOG(LOG_DEBUG, "LCM (%s):", __FUNCTION__);
    static VehicleComm vc_;
    return vc_;
}

} // namespace vc

/**
    @brief Main entry-point for the VehicleComm module.
    @return     value for the status during exit.
*/
int main(int argc, char **argv)
{
    int fd = -1;
    vc::ReturnValue ret;

    UNUSED(argc);
    UNUSED(argv);

    if (tpPCL_init("VehicleComm", nullptr, FALSE) != E_PCL_ERROR_NONE) {
        vc::LOG(vc::LOG_ERR, "VehicleComm: %s: Failed to init tpPCL_init needed for logging!", __FUNCTION__);
        return vc::RET_ERR_EXTERNAL;
    }

    DLT_REGISTER_APP("VCOM", "VehicleComm");
    vc::LOG_INIT("VCOM", "VehicleComm");
    /* Make sure VC is not already running */
    fd = open(VC_LOCK_FILE, O_RDWR | O_CREAT | O_EXCL);
    if(fd >= 0) {
        close(fd);
    }

    if (fd < 0) {
        vc::LOG(vc::LOG_ERR, "VehicleComm: %s: VehicleComm is already running, please shutdown and remove %s to rerun!",
                __FUNCTION__, VC_LOCK_FILE);
        DLT_UNREGISTER_APP();
        return vc::RET_ERR_INTERNAL;
    }

    vc::VehicleComm& vcomm = vc::VehicleComm::GetInstance();
    ret = vcomm.Init();
    if(ret != vc::RET_OK) {
        vc::LOG(vc::LOG_ERR, "VehicleComm Init failed: %s", vc::ReturnValueStr[ret]);
        vcomm.Stop();
        unlink(VC_LOCK_FILE);
        vc::LOG_DEINIT();
        return ret;
    }

    ret = vcomm.Start();
    if(ret != vc::RET_OK) {
        vc::LOG(vc::LOG_ERR, "VehicleComm Start failed: %s", vc::ReturnValueStr[ret]);
        vcomm.Stop();
        unlink(VC_LOCK_FILE);
        vc::LOG_DEINIT();
        return ret;
    }

    unlink(VC_LOCK_FILE);
    vc::LOG_DEINIT();

    return ret;
}
