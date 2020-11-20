/*!
 * \file xECDBusStuctures.h
 * \author Mikhail Maslyukov
 * \date 02/27/2017
 * \brief Contains data structures for xevcdm and PHEV parst of FordApp
 */

#ifndef PRJ_SOC_FORD_PACKAGE_XEVCDM_COMMON_DBUS_INTERFACE_XEVDBUSSTUCTURES_H_
#define PRJ_SOC_FORD_PACKAGE_XEVCDM_COMMON_DBUS_INTERFACE_XEVDBUSSTUCTURES_H_

#include <rms/gbt/gbt32960_3_simple.h>

#ifndef DIAG_DTC_MAX
#define DIAG_DTC_MAX 22///value from 'IPC_DIAG_IDs.h'
#endif

#define XEV_ECU_MAX (5)

//! \enum XEV_ECU_ID
//! \brief ECU list which DTCs should be provided in case of fault
typedef enum
{
    PHEV_BECM_ECU_ID                   = 0x7E4,
    PHEV_HPCM_ECU_ID                   = 0x7E6,
    PHEV_PCM_ECU_ID                    = 0x7E0,
    PHEV_DCDC_ECU_ID                   = 0x746,
    PHEV_ABS_ECU_ID                    = 0x760,
}XEV_ECU_ID;

//! \enum XEVStringLength
//! \brief List of settings string length
enum XEVStringLength {
    XEVSL_SERVER_URL_LENGTH = 40,
    XEVSL_USER_NAME_LENGTH = 40,
    XEVSL_PASSWORD_LENGTH = 20,
};

#pragma pack(push, 1)

//! \struct TemprCESSDBUSData
//! \brief store temperature information elemnent of chargeable energy-storage
typedef struct {
    // TCU Hardcode
    uint8_t code;      /// Code of chargeable energy-storage subsystem
    uint16_t size;     /// Quantity of chargeable energy-storage
                       /// temperature probes TCL_CESS_SIZE_REAL
    uint8_t data[TCL_CESS_SIZE_REAL];  /// Temperature value detected by each
                                       /// temperature probe of chargeable
                                       /// energy-storage subsystem
} TemprCESSDBUSData;

//! \struct TemprCESSDBUSDataCollection
//! \brief store temperature information of chargeable energy-storage
typedef struct {
    // TCU Hardcode always 1
    uint8_t size;           /// Number chargeable energy-storage subsystems
    TemprCESSDBUSData data;     /// Chargeable energy-storage subsystems data
} TemprCESSDBUSDataCollection;

//! \struct VoltageCESSDBUSData
//! \brief store voltage information of element in chargeable energy-storage
typedef struct {
    // TCU Hardcode
    uint8_t code;             /// Code of chargeable energy-storage subsystem
    uint16_t voltage;         /// Voltage of chargeable energy-storage device
    uint16_t current;         /// Current of chargeable energy-storage device
    uint16_t totalCells;      /// Total number of cell
    uint16_t snCell;          /// SN of the starting battery
    /// TCU Hardcode always  VCL_CESS_VOLTAGE_CELLS_REAL
    uint8_t frameCells;       /// Total cell number of this frame
    uint16_t voltageCells[VCL_CESS_VOLTAGE_CELLS_REAL]; /// Voltage of cells
} VoltageCESSDBUSData;

//! \struct VoltageCESSDBUSData
//! \brief store voltage information of chargeable energy-storage
typedef struct {
    // TCU Hardcode always 1
    uint8_t size;             /// Number chargeable energy-storage subsystems
    VoltageCESSDBUSData data;     /// Chargeable energy-storage subsystems data
} VoltageCESSDBUSDataCollection;

//! \struct PeriodicDBUSData
//! \brief periodic(1sec) vehicle CAN data
typedef struct {
    uint8_t sampleId;
    Time time;
    VehicleData vehicleData;
    ElectricalMachineData electrmMachineData;
    EngineData engineData;
    GPSData gpsData;
    LimitValueData limitValueData;
    WarningData warningData;
    VoltageCESSDBUSDataCollection voltCESSData;
    TemprCESSDBUSDataCollection tempCESSData;
}PeriodicDBUSData;

//typedef struct {
//    uint8_t sampleId;
//    Time time;
//    VehicleData vehicleData;
//    ElectricalMachineData electrmMachineData;
//    EngineData engineData;
//    GPSData gpsData;
//    LimitValueData limitValueData;
//    WarningData warningData;
//    VoltageCESSDBUSDataCollection voltCESSData;
//    TemprCESSDBUSDataCollection tempCESSData;
//}PeriodicDBUSData;

//! \struct DtcDBusData
//! \brief DTC information it is copy of App_DiagDTCResponse_T struct from 'ipc_diagnostic.h' in 'conti-dcm'
typedef struct {
    uint16_t    t_ecu_id;
    uint8_t     t_status;               ///0=OK, 1=conditions_not_OK, 2=tool connected , 3=other failure 4=timeout, 5=negative response,6=filtered
    uint8_t     t_dtc_actual_count;     /// received from CAN
    uint8_t     t_dtc_included_count;   /// sent in this message (limit based on requirements 19)
    uint8_t     t_dtc[(DIAG_DTC_MAX) * 4];  ///always send full buffer because it is simplier.
                                            /// GGDS => byte0(first byte transmitted)=DTC_HIGH, byte1=DTC_LOW, byte2=Failure_Type,byte3=Status
                                            /// GDS => byte0(first byte transmitted)=DTC_HIGH, byte1=DTC_LOW, byte2=0x00,byte3=Status
    uint32_t    t_track_id;
}DtcDBusData;

//! \struct WarningDbusData
//! \brief Contains vehicle CAN data in warning mode
typedef struct {
    PeriodicDBUSData periodicData;
    DtcDBusData dtc[XEV_ECU_MAX];
} WarningDbusData;

//! \struct ServerConnectionParams
//! \brief Contains connection parameter to the government server
typedef struct {
    char serverURL[XEVSL_SERVER_URL_LENGTH];
    char ICCID[VLL_ICCID_LENGTH];
    char VIN[GSL_VIN_LENGTH];
    Time time;
}ServerConnectionParams;

//! \struct xEVConfigurations
//! \brief Contains xevcdm application parameters
typedef struct {
    bool directConnectionEnable;
    uint8_t dataPublishingPeriod; ///1-120 sec
    uint8_t heartBeatPeriod; ///1-240 sec
    uint8_t previousDataBufferingPeriod; ///30-60
    bool warningModeDetectionDuringIgnitionOn;
    bool warningModeDetectionDuringCharging;
    uint8_t warningModeLastingPeriod; ///30-60
    uint16_t periodOfDataCollectionAfterChargingCompletion;
}xEVConfigurations;

#pragma pack(pop)

#endif /* PRJ_SOC_FORD_PACKAGE_XEVCDM_COMMON_DBUS_INTERFACE_XEVDBUSSTUCTURES_H_ */
