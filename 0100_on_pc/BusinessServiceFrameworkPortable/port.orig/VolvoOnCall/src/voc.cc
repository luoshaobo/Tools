/**
 * Copyright (C) 2016, 2018 Continental Automotive GmbH
 *
 * Licensed under the "Supplier unrestricted white box" category, as
 * described in the 20160322-1_SW-amendment TCAM.pdf agreement
 * between Volvo Cars and Continental Automotive GmbH.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *  \file     voc.cc
 *  \brief    VOC Service main thread.
 *  \author   Piotr Tomaszewski, Axel Fagerstedt, et al.
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

//include all relevant features

#include "fsm.h"
#include "voc_framework/features/feature.h"

#include "features/basic_car_control_feature.h"
#include "features/car_access_feature.h"
#include "voc.h"
#include "voc_utils.h"
#ifdef ENABLE_SIGNAL_INJECTION
#include "voc_dbus.h"
#include "voc_inject_signal.h"
#endif
#include "keystore.h"

#include <chrono>
#include <cstdlib>
#include <thread>
#include <vector>

#include "dlt/dlt.h"


#ifndef VOC_TESTS
extern "C"
{
#include "tpsys.h"
}
#endif

extern "C"
{
#include "powermgr.h"
}

DLT_DECLARE_CONTEXT(dlt_voc);

//TODO: this is a hack until transfer manager drops coma mock
DLT_DECLARE_CONTEXT(dlt_fssdd);


namespace volvo_on_call
{

fsm::VehicleCommTransactionId default_ihu_transaction_id;

// vector containing all functions
std::vector<fsm::Feature*> feature_list;

#ifndef VOC_TESTS
GMainLoop *main_loop = nullptr;

static void SignalHandler(int sig)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s: got signal %i (%i, %i)", __FUNCTION__, sig, SIGINT, SIGTERM);
    if ((main_loop != nullptr) && g_main_loop_is_running(main_loop))
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s: Quit main loop...", __FUNCTION__);
        signal(SIGINT, SIG_DFL);
        signal(SIGTERM, SIG_DFL);
        g_main_loop_quit(main_loop);
    }
}

static int Start()
{
    DLT_REGISTER_APP("VOC", "Volvo On Call");
    DLT_REGISTER_CONTEXT(dlt_voc, "VOC","Volvo On Call");

    DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "Starting...");

    signal(SIGINT, SignalHandler);
    signal(SIGTERM, SignalHandler);

    time_t cur_time = time(nullptr);
    struct tm  cur_time_struct;
    gmtime_r(&cur_time, &cur_time_struct);
    if (cur_time_struct.tm_year  < 2017 - 1900)  // check whether year is reasonable, otherwise an outdated year (1970) is used
    {
        printf("voc failed: system date (%04i-%02i-%02i) is invalid! Set time manually on command line: 'date -s \"2017-08-01 00:01\"'\n",\
            cur_time_struct.tm_year + 1900, cur_time_struct.tm_mon + 1, cur_time_struct.tm_mday);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "voc failed: system date (%04i-%02i-%02i) is invalid! Set time manually on command line,: 'date -s \"2017-08-01 00:01\"'",\
            cur_time_struct.tm_year + 1900, cur_time_struct.tm_mon + 1, cur_time_struct.tm_mday);
        exit(EXIT_FAILURE);
    }

    // Initialize global libs
    InitVocCustom();

    tpLCM_ReturnCode_t ret_val = tpLCM_init();
    if (ret_val != E_LCM_RET_SUCCESS) {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "%s: tpLCM_init failed! ret value : %d", __FUNCTION__, ret_val);
         exit(EXIT_FAILURE);
    }

    tpPM_ReturnType_t ret_pm_val = tpPM_init();
    if (ret_pm_val != E_PM_RET_SUCCESS) {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "%s: tpPM_init failed! ret value : %d", __FUNCTION__, ret_pm_val);
        exit(EXIT_FAILURE);
    }

    // Create relevant functions and push them to the vector
    CarAccessFeature *ca  = new CarAccessFeature();
    feature_list.push_back(ca);
    feature_list.push_back(new BasicCarControlFeature());

#ifdef ENABLE_SIGNAL_INJECTION
    VolvoOnCallTestInjectSignal::GetInstance().SetCarAccessFeature(ca);
    VolvoOnCallDBus::GetInstance();
#endif

    DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "Started");

    // main loop
    main_loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(main_loop);

    while(!feature_list.empty()) {
        delete feature_list.back();
        feature_list.pop_back();
    }

    ret_pm_val = tpPM_deinit();
    if( E_PM_RET_SUCCESS != ret_pm_val ){
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "AppsLcm::%s, Failed! tpPM_deinit ret value: %d ", __FUNCTION__, ret_pm_val);
    }

    ret_val = tpLCM_deinit();
    if( E_LCM_RET_SUCCESS != ret_val ){
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "AppsLcm::%s, Failed! ret value: %d ", __FUNCTION__, ret_val);
    }
        
    // Clean up openssl
    CleanupVocCustom();

    g_main_loop_unref(main_loop);

    DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "Stopped");

    DLT_UNREGISTER_CONTEXT(dlt_voc);
    DLT_UNREGISTER_APP();

    return 0;
}
#endif

} // namespace volvo_on_call

#ifndef VOC_TESTS
int main()
{
    volvo_on_call::Start();
    return 0;
}
#endif

/** \}    end of addtogroup */
