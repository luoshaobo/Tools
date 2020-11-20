///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental AG and subsidiaries
// Unless required by applicable law or agreed to in writing, 
// software distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  @file dr_sensor.c
//  This file handles the DR sensor data
// @project     GLY_TCAM
// @subsystem   VolvoPositioningManager
// @author      Jiaojiao Shen
// @Init date   27-Dec-2018
///////////////////////////////////////////////////////////////////

#include <pthread.h>
#include "dr_sensor.h"
#include "dr_whlinfo.h"
#include "dr_log.h"



#define NANO_SECOND_MULTIPLIER  (1000000)
static const long CB_INTERVAL_MS = 100 * NANO_SECOND_MULTIPLIER;

////////////////////////////////////////////////////////////
// Static Variable definitions
////////////////////////////////////////////////////////////
static tpDR_vehicleDataCallback sensor_cb = NULL;
static pthread_t sensor_thread;
static bool test_mode = false;
static bool cb_registered = false;
static pthread_mutex_t mutex_cb  = PTHREAD_MUTEX_INITIALIZER;

////////////////////////////////////////////////////////////
// Function Definitions
////////////////////////////////////////////////////////////
static bool DR_vehicleDataRegisterCallback(tpDR_vehicleDataCallback callback);
static bool DR_vehicleDataDeregisterCallback(void);
static void DR_vehicleDataEnableTestMode(void);
static void* thread_func(void *args);
static uint32_t get_timestamp(void);

////////////////////////////////////////////////////////////
// @brief : Registers a call back function used by the location service Sensor task to report sensor data
// @param[in]  callback         The callback which should be registered
// @return     True if callback has been registered successfully
// @author     Jiaojiao Shen, Dec 27, 2018
////////////////////////////////////////////////////////////
static bool DR_vehicleDataRegisterCallback(tpDR_vehicleDataCallback callback)
{
    DR_LOG_INFO("DR_SENSOR: %s", __FUNCTION__);

    bool rc = true;

    if (cb_registered == true)
    {
        rc = false;
    }
    else
    {
        pthread_mutex_lock(&mutex_cb);
        sensor_cb = callback;
        cb_registered = true;
        pthread_mutex_unlock(&mutex_cb);

        // start sensor thread
        if (pthread_create(&sensor_thread, NULL, &thread_func, NULL) != 0)
        {
            rc = false;
        }
    }

    if(rc)
    {
        rc = DrWhlInfoInit();
    }

    return(rc);
}

////////////////////////////////////////////////////////////
// @brief : Deregisters a call back function used by the location service Sensor task to report sensor data
// @param[in]  None
// @return     return True if callback has been deregistered successfully
// @author     Jiaojiao Shen, Dec 27, 2018
////////////////////////////////////////////////////////////
static bool DR_vehicleDataDeregisterCallback(void)
{
    DR_LOG_INFO("DR_SENSOR: %s", __FUNCTION__);

    bool rc = true;

    if (cb_registered == false)
    {
        rc = false;
    }
    else
    {
        pthread_mutex_lock(&mutex_cb);
        sensor_cb = 0;
        cb_registered = false;
        pthread_mutex_unlock(&mutex_cb);
        pthread_join(sensor_thread, NULL);
    }

    DrWhlInfoDeInit();
    return(rc);
}

////////////////////////////////////////////////////////////
// @brief : Enable test mode. Normal sensor data message sending is suspended
// @param[in]  None
// @return     None
// @author     Jiaojiao Shen, Dec 27, 2018
////////////////////////////////////////////////////////////
static void DR_vehicleDataEnableTestMode(void)
{
    DR_LOG_INFO("DR_SENSOR: %s", __FUNCTION__);

    test_mode = true;
}

////////////////////////////////////////////////////////////
// @brief : Thread to send ASCII debug data to location manager clients
// @param[in]  None
// @return     None
// @author     Jiaojiao Shen, Dec 27, 2018
////////////////////////////////////////////////////////////
static void* thread_func(void* args)
{
    (void)args;

    DR_LOG_INFO("DR_SENSOR: %s", __FUNCTION__);

    struct timespec cb_sleep = { 0 };
    cb_sleep.tv_nsec = CB_INTERVAL_MS;
    tpDR_vehData cb_data = { 0 };
    
#if 0
    cb_data.yaw =  2;
    cb_data.wheel_data.RR_dir = DR_WHEEL_FORWARD;
    cb_data.wheel_data.RL_dir = DR_WHEEL_FORWARD;
    cb_data.wheel_data.RR_cnt = 80; //speed of 10m/s for ticks rev = 160, width of wheel = 637 cm
    cb_data.wheel_data.RL_cnt = 80; //speed of 10m/s for ticks rev = 160, width of wheel = 637 cm
    cb_data.sensor_data_status |= DR_YAW_AVAILABLE | DR_WHEEL_AVAILABLE | DR_WHEEL_VALID | DR_YAW_VALID;
#endif

    bool threadRunning = true;

    while(threadRunning)
    {
        pthread_mutex_lock(&mutex_cb);
        threadRunning = cb_registered;
        if( sensor_cb != NULL )
        {
            cb_data = *DrGetDrWhlInfo();

            cb_data.vucTimestamp = get_timestamp();

            DR_LOG_DEBUG("DR_SENSOR: Sending vehicle data at timestamp... %u", cb_data.vucTimestamp);
            DR_LOG_DEBUG("DR_SENSOR: vehicle data sensor_data_status:%u, speed:%u, yaw:%d, FR_cnt:%u, FL_cnt:%u, RR_cnt:%u, RL_cnt:%u, FR_dir:%d, FL_dir:%d, RR_dir:%d, RL_dir:%d",
                    cb_data.sensor_data_status,
                    cb_data.speed,
                    cb_data.yaw,
                    cb_data.wheel_data.FR_cnt,
                    cb_data.wheel_data.FL_cnt,
                    cb_data.wheel_data.RR_cnt,
                    cb_data.wheel_data.RL_cnt,
                    cb_data.wheel_data.FR_dir,
                    cb_data.wheel_data.FL_dir,
                    cb_data.wheel_data.RR_dir,
                    cb_data.wheel_data.RL_dir);
            
            (sensor_cb)(&cb_data, 1);
        }
        else
        {
            DR_LOG_INFO("DR_SENSOR: No callback registered");
        }
        pthread_mutex_unlock(&mutex_cb);

        nanosleep(&cb_sleep, NULL);
    }

    return NULL;
}

////////////////////////////////////////////////////////////
// @brief : Get pointers to API functions
// @param[in]  None
// @return     None
// @author     Jiaojiao Shen, Dec 27, 2018
////////////////////////////////////////////////////////////
void DRSensorInit(drsensor_handle_t handle)
{
    DR_LOG_INFO("DR_SENSOR: %s", __FUNCTION__);

    if (handle == NULL)
    {
        DR_LOG_ERR("DR_SENSOR: DRSensorInit - null handle\n");
    }
    else
    {
        handle->vehicleDataRegisterCallback = DR_vehicleDataRegisterCallback;
        handle->vehicleDataDeregisterCallback = DR_vehicleDataDeregisterCallback;
        handle->vehicleDataEnableTestMode = DR_vehicleDataEnableTestMode;
    }
}

////////////////////////////////////////////////////////////
// @brief : Gets system time in milliseconds
// @param[in]  None
// @return     system times in milliseconds
// @author     Jiaojiao Shen, Dec 27, 2018
////////////////////////////////////////////////////////////
static uint32_t get_timestamp(void)
{
    struct timespec time_value = { 0 };

    if (clock_gettime(CLOCK_MONOTONIC, &time_value) != -1)
    {
        return (time_value.tv_sec * 1000 + time_value.tv_nsec / 1000000);
    }
    else
    {
        return 0;
    }
}