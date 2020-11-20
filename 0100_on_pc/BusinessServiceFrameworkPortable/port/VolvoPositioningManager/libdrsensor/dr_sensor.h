#ifndef DR_SENSOR
#define DR_SENSOR
/* ************************************************************************
 * Copyright (C) 2016 Continental AG and subsidiaries
 * The copyright notice above does not evidence any actual
 * or intended publication of such source code.
 * The code contains Continental Confidential Proprietary Information.
 * ************************************************************************/

/** @file dr_sensor.h
*
* @brief API providing vehicle data and status for the Dead Reckoning (DR) library
* in telematics products.
*
* A data structure is defined and a callback mechanism to provide this data.
* The library providing this interface will be provided by the telematics
* product/platform team.
* This API will be used by the product specific wrapper to feed the generic DR library.
*/

/* **************************************************************************
 * HEADER FILE INCLUDES
 * *************************************************************************/
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup  Positioning
 ** @{
 ** @defgroup DR Dead Reckoning
 ** @{
*/

/*
 * Defines bits to indicate the status of the TCU sensor data.
 * The defined bits are to be set to tpDR_vehData::sensor_data_status
 */

/***************************************************************************
 * TYPE DEFINITIONS
***************************************************************************/
#define DR_VEHICLESPEED_VALID    0x01  /**< This bit is set when the speed data is valid. */
#define DR_REVERSE_VALID         0x02  /**< This bit is set when the reverse data is valid. */
#define DR_YAW_VALID             0x04  /**< This bit is set when the yaw data is valid. */
#define DR_WHEEL_VALID           0x08  /**< This bit is set when the wheel data is valid. */

/**
* @brief This bit is set when no data is received from the selected sensor for the interval (timeout).
* When a timeout occurs, the callback function will be called with timeout set.
*/
#define DR_TIMEOUT        0x10

/*
* These bits are set to reflect the selected sensor for the system. For example,
* the Ford system will only use the yaw sensor. Reception of data from the selected sensor is the
* trigger to send all sensor data for the interval. It is possible that a system will use both yaw and wheel sensors.
* In this case, yaw will be the trigger.
*/
#define DR_USE_YAW        0x20  /**< Use yaw rate as the selected sensor for DR. */
#define DR_USE_WHEEL      0x40  /**< use wheel data as the selected sensor for DR */

/**
 * @brief This bit is set to true if the vehicle data has been buffered at the VuC before transmission to the SoC.
 * This may be done at start-up to allow provision of early vehicle data.
 * Note: In this case, the timestamp cannot be used for sensor synchronization.
 * Assumption: The flag is only set for early messages at start-up,
 *   i.e. after once un-buffered (real-time) messages have been provided
 *   there will be no buffered data anymore.
 * Note:
 */
#define DR_BUFFERED        0x80  /**< Multiple elements can be reported by the callback only for buffered data.  */

#define DR_REVERSE_ACTIVE  0x100 /**< Reverse gear indication. This bit is set when reverse is active. */
#define DR_YAW_FAULT       0x200 /**< This bit is set when the yaw sensor reports fault. */
#define DR_WHEEL_FAULT     0x400 /**< This bit is set when the wheel sensor reports fault. */

/**
 * @brief These bits were added to avoid the case in which there is an asynchronous event based of collecting sensor data.
 * Note: In BSRF project the structure is partialy set with either yaw or wheel data.
 */
#define DR_YAW_AVAILABLE    0x1000 /**< This bit is set when yaw sensor data is available in tpDR_vehData structure. */
#define DR_WHEEL_AVAILABLE  0x2000 /**< This bit is set when wheel sensor data is available in tpDR_vehData structure. */
#define DR_SPEED_AVAILABLE  0x4000 /**< This bit is set when speed sensor data is available in tpDR_vehData structure. */

/**
 * @brief DR library needs degrees per second instead of 0.0002 rad/sec
 */
#define DR_YAW_OFFSET       5000.0 /**< This macro is defined to be used by DR library in converting from 0.0002 rad/sec => deg/sec */

/** @brief Wheel direction */
typedef enum e_tpDR_wheelDir
{
    DR_WHEEL_FORWARD,               /**< Wheel direction is forward. */
    DR_WHEEL_BACKWARD,              /**< Wheel direction is backward. */
    DR_WHEEL_UNKNOWN,               /**< Wheel direction is unknown. */
    DR_WHEEL_ERROR                  /**< Wheel direction is in error state. */
} tpDR_wheelDir_t;

/** @brief The tpDR_wheelData_t struct delivers wheel data received from the vehicle bus */
typedef struct s_tpDR_wheelData_t
{
    uint16_t FR_cnt;       /**< Front right wheel tick count */
    uint16_t FL_cnt;       /**< Front left wheel tick count */
    uint16_t RR_cnt;       /**< Rear right wheel tick count */
    uint16_t RL_cnt;       /**< Rear left wheel tick count */
    tpDR_wheelDir_t FR_dir;  /**< Front right wheel direction */
    tpDR_wheelDir_t FL_dir;  /**< Front left wheel direction */
    tpDR_wheelDir_t RR_dir;  /**< Rear right wheel direction */
    tpDR_wheelDir_t RL_dir;  /**< Rear left wheel direction */
} tpDR_wheelData_t;

/** @brief The tpDR_vehData struct delivers DR sensor data received from the vehicle bus plus some status indications. */
typedef struct s_tpDR_vehData {
    /*
     *  Assumption: VuC and SoC clocks are not synchronized.
     *  Assumption: VuC and SoC clocks never jump and are never reset.
     */
    uint32_t vucTimestamp;        /**< Timestamp of the _reception_ of the vehicle data at the VuC [ms]. */
    uint16_t sensor_data_status;  /**< Bit mask providing sensor data status information [bitwise or of sensor data status defines] */
    uint16_t speed;               /**< speed data (.01 KPH) */
    int32_t  yaw;                 /**< yaw data (0.0002 rad/sec) */
    tpDR_wheelData_t wheel_data;  /**< wheel data */
} tpDR_vehData;

/**
* @brief Callback type for DR vehicle data.
* Use this type of callback if you want to register for vehicle data.
* This callback may return buffered data (numElements >1) for different reasons
*   for (large) portions of data buffered at startup
*   for data buffered during regular operation e.g. for performance optimization (reduction of callback invocation frequency)
* If the array contains (numElements >1), the elements will be ordered with rising timestamps
* @param vehData pointer to an array of tpDR_vehData with size numElements
* @param numElements: allowed range: >=1. If numElements >1, buffered data are provided.
*/
typedef void (*tpDR_vehicleDataCallback)(const tpDR_vehData *vehData, const uint16_t numElements);

/**
* @brief This structure contains function pointers to DR sensor library
*/
typedef struct s_tpDR_Sensor_t {
/**
* @brief Register vehicle data callback.
* @param callback - The callback which should be registered.
* @return True if callback has been registered successfully.
*/
bool (*vehicleDataRegisterCallback) (tpDR_vehicleDataCallback callback);

/**
* @brief Deregister vehicle data callback.
* After calling this method no new vehicle data data will be delivered to the client.
* @return True if callback has been deregistered successfully.
*/
bool (*vehicleDataDeregisterCallback) (void);

/**
* @brief Enable test mode.
* After calling this method test mode will be enabled.
*/
void (*vehicleDataEnableTestMode) (void);
} tpDR_Sensor_t;

typedef tpDR_Sensor_t* drsensor_handle_t;

/**
* @brief Initialize DR sensor library.
* Initializes the tpDR_Sensor_t with DR sensor function pointers.
*/
void DRSensorInit(drsensor_handle_t handle);

#ifdef __cplusplus
}
#endif

 /** @} */ /* End of group (DR) */
 /** @} */ /* End of group (Positioning) */
#endif /* DR_SENSOR */
