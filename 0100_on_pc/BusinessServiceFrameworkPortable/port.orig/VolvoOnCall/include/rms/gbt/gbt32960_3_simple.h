////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  @file gbt32960_3_simple.h
//  This file handles the communication with the VuC, using the OTP VCS-interface
 
// @project     GLY_TCAM
// @subsystem   VolvoOnCall
// @author      Shen Jiaojiao
// @Init date   21-Sep-2018
////////////////////////////////////////////////////////////////////////////


#ifndef XEVCDM_INCLUDE_PROTO_GBT32960_3_H_
#define XEVCDM_INCLUDE_PROTO_GBT32960_3_H_

#include <stdint.h>

#pragma pack(push, 1)

//#if defined(__cplusplus)
//namespace gbt {
//#endif

/******************************************************************************
 *                       VehicleLoginData
 ******************************************************************************/

#define ELECTRICAL_MACHINE_MAX (2)

enum IntLimits {
    IL_BYTE_MAX = 252,
    IL_WORD_MAX = 65531,
    IL_BYTE_ANOMALY = 0xFE,
    IL_BYTE_INVALID = 0xFF,
    IL_WORD_ANOMALY = 0xFFFE,
    IL_WORD_INVALID = 0xFFFF,
    IL_DWORD_ANOMALY = 0xFFFFFFFE,
    IL_DWORD_INVALID = 0xFFFFFFFF,
};

enum TimeLimits {
    TL_YEAR_MIN = 0,
    TL_YEAR_MAX = 99,
    TL_MONTH_MIN = 1,
    TL_MONTH_MAX = 12,
    TL_DAY_MIN = 1,
    TL_DAY_MAX = 31,
    TL_HOUR_MIN = 0,
    TL_HOUR_MAX = 23,
    TL_MINUTE_MIN = 0,
    TL_MINUTE_MAX = 59,
    TL_SECOND_MIN = 0,
    TL_SECOND_MAX = 59,
};

typedef struct {
    uint8_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
} Time;

enum GBTSringLength {
    GSL_VIN_LENGTH = 17,
};

enum VehicleLoginLimits {
    VLL_LOGIN_SN_MIN = 1,
    VLL_LOGIN_SN_MAX = IL_WORD_MAX,
    VLL_ICCID_LENGTH = 20,
    VLL_QCESS_MAX = 250,
    VLL_QCESS_REAL = 1,
    VLL_LCESS_MAX = 50,
    VLL_LCESS_REAL = 0, //????
};

typedef struct {
    Time time;          // Data acquisition time
    uint16_t sn;        // Login serial number 0 ~ 65531 (IL_WORD_MAX).
                        // Increment at each login
    char iccid[VLL_ICCID_LENGTH];

    // TCU Hardcode
    uint8_t qcess;      // Quantity of chargeable energy-storage subsystem (n);
                        // range of valid values: 0~250, where "0" means not
    // TCU Hardcode
    uint8_t lcess;      // Code length of chargeable energy-storage system (m);
                        // range of valid values: 0~50,
                        // where "0" means not uploading this code.
    // TCU Hardcode
    // Code of chargeable energy-storage system should be
    // value obtained from the vehicle by the terminal.
    // ?????????????????
    char ccess[VLL_QCESS_REAL*VLL_LCESS_REAL];//[qcess * lcess];
} VehicleLoginData;


/******************************************************************************
 *                       RealtimeInfo
 ******************************************************************************/

//                       VehicleData
//------------------------------------------------------------------------------

typedef enum {
    VS_STARTUP = 0x01,               // the start-up state of vehicle
    VS_SHUTDOWN = 0x02,              // the shut-down state of vehicle
    VS_OTHER = 0x03,                 // other state
    VS_ANOMALY = IL_BYTE_ANOMALY,
    VS_INVALID = IL_BYTE_INVALID,
} VehicleState;

typedef enum {
    CS_CHARGING_PARKING = 0x01,   // charging in parking state;
    CS_CHARGING_DRIVING = 0x02,   // charging in driving state
    CS_UNCHARGED = 0x03,          // uncharged
    CS_CHARGING_COMPETED = 0x04,  // charging competed
    CS_ANOMALY = IL_BYTE_ANOMALY,
    CS_INVALID = IL_BYTE_INVALID,
} ChargingState;

typedef enum {
    OM_ELECTRIC = 0x01,           // All-electric
    OM_HYBRID = 0x02,             // Hybrid electric
    OM_FUEL = 0x03,               // Fuel
    OM_ANOMALY = IL_BYTE_ANOMALY,
    OM_INVALID = IL_BYTE_INVALID,
} OperationMode;

typedef enum {
    DCS_IN_OPERATION = 0x01,      // In operation
    DCS_CUT_OFF = 0x02,           // Cut-off
    DCS_ANOMALY = IL_BYTE_ANOMALY,
    DCS_INVALID = IL_BYTE_INVALID,
} DCDCState;


enum VehicleDataLimits {
    VDL_VEHILCES_SPEED_MAX = 2200,  // (denoting 0km/h～220km/h);
                                    // minimum measuring unit: 0.1km/h;
    VDL_VEHILCES_SPEED_ANOMALY = IL_WORD_ANOMALY,
    VDL_VEHILCES_SPEED_INVALID = IL_WORD_INVALID,

    VDL_ACCUM_MILEAGE_MAX = 9999999,  // (denoting 0km～999999.9km);
                                      // minimum measuring unit: 0.1km;
    VDL_ACCUM_MILEAGE_ANOMALY = IL_DWORD_ANOMALY,
    VDL_ACCUM_MILEAGE_INVALID = IL_DWORD_INVALID,

    VDL_TOTAL_VOLTAGE_MAX = 10000,   // (denoting 0V～1,000V);
                                     // minimum measuring unit: 0.1V
    VDL_TOTAL_VOLTAGE_ANOMALY = IL_WORD_ANOMALY,
    VDL_TOTAL_VOLTAGE_INVALID = IL_WORD_INVALID,

    VDL_TOTAL_CURRENT_MAX = 20000,   // (denoting -1000A～+1000A);
                                     // minimum measuring unit: 0.1A
    VDL_TOTAL_CURRENT_ANOMALY = IL_WORD_ANOMALY,
    VDL_TOTAL_CURRENT_INVALID = IL_WORD_INVALID,

    VDL_SOC_MAX = 100,   //(denoting 0%～100%); minimum measuring unit: 1%
    VDL_SOC_ANOMALY = IL_BYTE_ANOMALY,
    VDL_SOC_INVALID = IL_BYTE_INVALID,

    VDL_INSULATION_RESISTANCE_MAX = 60000,   // (denoting 0kΩ～60000kΩ);
                                             // minimum measuring unit: 1KΩ;
    VDL_INSULATION_INVALID = IL_WORD_INVALID,//TODO: clarify on using this value

    VDL_STROKE_ACCELERATOR_PEDAL_MAX = 100,   //(denoting 0%~100%); minimum measuring unit: 1%
    VDL_STROKE_ACCELERATOR_PEDAL_ANOMALY = IL_BYTE_ANOMALY,
    VDL_STROKE_ACCELERATOR_PEDAL_INVALID = IL_BYTE_INVALID,

    VDL_BRAKE_PEDAL_STATE_MAX = 100, //(denoting 0%~100%); minimum measuring unit: 1%
    VDL_BRAKE_PEDAL_STATE_ANOMALY = IL_BYTE_ANOMALY,
    VDL_BRAKE_PEDAL_STATE_INVALID = IL_BYTE_INVALID,


};
typedef union {
    struct {
        unsigned gear :4;  // [0-3] bits:
                           //  0000 neutral gear
                           //  0001 Gear 1
                           //  0010 Gear 2
                           //  0011 Gear 3
                           //  0100 Gear 4
                           //  0101 Gear 5
                           //  0110 Gear 6
                           //  ...
                           //  1101  reverse gear
                           //  1110  Gear D of AT
                           //  1111  Gear P (parking)
        unsigned breakForce :1;     // 1: with braking force
                                    // 0: without braking force
        unsigned drivingForce :1;   // 1: with driving force
                                    // 0: without driving force
        unsigned reserved :2;
    } bit;
    uint8_t byte;
} GearPosition;

typedef union {
    struct {
        uint8_t strokeAccelPedal;
        uint8_t brakePedalState;
    } pedals;
    uint16_t reserved;
} VehicleDataReserved;

typedef struct {
    VehicleState vehicleState: 8;    // Vehicle state
    ChargingState chargingState: 8;  // Charging state
    OperationMode operationMode: 8;  // Operation mode
    uint16_t vehicleSpeed;           // Vehicle speed
    uint32_t accumMileage;           // Accumulated mileage
    uint16_t totalVoltage;           // Total voltage
    uint16_t totalCurrent;           // Total current
    uint8_t soc;                     // SOC
    DCDCState dcdcState: 8;          // DC-DC state
    GearPosition gearPosition;       // Gear position
    uint16_t insulationResistance;   // Insulation resistance
    VehicleDataReserved reserved;    // Reserved
} VehicleData;


//                       ElectricalMachineData
//------------------------------------------------------------------------------

enum ElectricalMachineLimits {
    EML_SIZE_MIN = 1,
    EML_SIZE_REAL = 1,
    EML_SIZE_MAX = 253,
    EML_SN_MIN = 1,
    EML_SN_MAX = 253,
    EML_TEMPR_CTRLER_MAX = 250,   // (deviation is 40℃, denoting -40℃～+210℃);
    EML_TEMPR_CTRLER_ANOMALY = IL_BYTE_ANOMALY,
    EML_TEMPR_CTRLER_INVALID = IL_BYTE_INVALID,
    EML_SPEED_MAX = IL_WORD_MAX,  // (value deviation is 20,000,
                                  //  denoting -20,000r/min ~ 45,531r/min);
    EML_SPEED_ANOMALY = IL_WORD_ANOMALY,
    EML_SPEED_INVALID = IL_WORD_INVALID,

    EML_TORQUE_MAX = IL_WORD_MAX,  // (value deviation is 20,000,
                                   // denoting -2000N*m～4553.1N*m);
    EML_TORQUE_ANOMALY = IL_WORD_ANOMALY,
    EML_TORQUE_INVALID = IL_WORD_INVALID,

    EML_TEMPR_MAX = 250,         // (deviation is 40℃, denoting -40℃～+210℃);
    EML_TEMPR_ANOMALY = IL_BYTE_ANOMALY,
    EML_TEMPR_INVALID = IL_BYTE_INVALID,

    EML_INVOLTAGE_MAX = 60000,  // (denoting 0V～6,000V);
    EML_INVOLTAGE_ANOMALY = IL_WORD_ANOMALY,
    EML_INVOLTAGE_INVALID = IL_WORD_INVALID,

    EML_CURRENTDCBUS_MAX = 20000,  // (value deviation is 1000A,
                                   // denoting -1000A～+1000A);
    EML_CURRENTDCBUS_ANOMALY = IL_WORD_ANOMALY,
    EML_CURRENTDCBUS_INVALID = IL_WORD_INVALID,
};

typedef enum  {
    EMS_POWER_CONSUMTION = 0x01,
    EMS_POWER_GENERATION = 0x02,
    EMS_DEACTIVATION = 0x03,
    EMS_READINESS = 0x04,
    EMS_ANOMALY = IL_BYTE_ANOMALY,
    EMS_INVALID = IL_BYTE_INVALID,
} ElectricalMachineState;

typedef struct {
    uint8_t sn;             // SN of electrical machine;
                            // one day constitutes one cycle.
    ElectricalMachineState ems: 8;  // State of electrical machine
    uint8_t temperatureController;  // Temperature of electrical machine
                                    // controller
    uint16_t speed;         // Speed of electrical machine
    uint16_t torque;        // Torque of electrical machine
    uint8_t temperature;    // Temperature of electrical machine
    uint16_t inVoltage;     // Input voltage of electrical machine controller
    uint16_t dcCurrent;     // Current of DC bus of electrical machine
                            // controller
} ElectricalMachineDataElement;

typedef struct {
    uint8_t count;          // total electrical machine;
    ElectricalMachineDataElement elecMachineDataArray[ELECTRICAL_MACHINE_MAX];
} ElectricalMachineData;

//typedef struct {
//   uint8_t sn;             // SN of electrical machine;
//                            // one day constitutes one cycle.
//    ElectricalMachineState ems: 8;  // State of electrical machine
//    uint8_t temperatureController;  // Temperature of electrical machine
//                                    // controller
//    uint16_t speed;         // Speed of electrical machine
//    uint16_t torque;        // Torque of electrical machine
//    uint8_t temperature;    // Temperature of electrical machine
//    uint16_t inVoltage;     // Input voltage of electrical machine controller
//    uint16_t dcCurrent;     // Current of DC bus of electrical machine
//                            // controller
//} ElectricalMachineData;

typedef struct {
    uint8_t size;                   // always 2
    ElectricalMachineDataElement data[ELECTRICAL_MACHINE_MAX];
} ElectricalMachineDataCollection;

//typedef struct {
//    uint8_t size;                   // always 1
//    ElectricalMachineData data;
//} ElectricalMachineDataCollection;


//                       EngineData
//------------------------------------------------------------------------------

typedef enum {
    ES_ON = 0x01,       // Engine is On
    ES_OFF = 0x02,      // Engine is Off
    ES_ANOMALY = IL_BYTE_ANOMALY,
    ES_INVALID = IL_BYTE_INVALID,
} EngineState;

enum EngineLimits {
    EL_CRANGSHAFT_SPEED_MAX = 60000,  // (denoting 0rpm～60000rpm);
    EL_CRANGSHAFT_SPEED_ANOMALY = IL_WORD_ANOMALY,
    EL_CRANGSHAFT_SPEED_INVALID = IL_WORD_INVALID,
    EL_FUEL_CONSUMPTION_MAX = 60000,  // (denoting 0L/100km～600L/100km);
    EL_FUEL_CONSUMPTION_ANOMALY = IL_WORD_ANOMALY,
    EL_FUEL_CONSUMPTION_INVALID = IL_WORD_INVALID,
};

typedef struct {
    EngineState engineState: 8;  // Engine state
    uint16_t crankshaftSpeed;    // Crankshaft speed
    uint16_t fuelConsumption;    // Fuel consumption
} EngineData;


//                       GPSData
//------------------------------------------------------------------------------

typedef union {
    struct {
        unsigned invalid :1;    // 0: Valid positioning
                                // 1: Invalid positioning;
        unsigned latitude :1;   // 0: North latitude;
                                // 1: South latitude
        unsigned longitude :1;  // 0: East longitude;
                                // 1: West longitude
        unsigned reserved :5;
    } bit;
    uint8_t byte;
} GPSPositionState;

typedef struct {
    GPSPositionState positionState;   // Positioning state
    uint32_t longitude;  // Longitude is multiplied by 106,
                         // with precision up to 0.000001 (in degree).
    uint32_t latitude;   // Latitude is multiplied by 106,
                         // with precision up to 0.000001 (in degree).
} GPSData;


//                       LimitValueData
//------------------------------------------------------------------------------

enum LimitValueLimits {
    LVL_ID_BATT_MAX = 250,
    LVL_ID_BATT_ANOMALY = IL_BYTE_ANOMALY,
    LVL_ID_BATT_INVALID = IL_BYTE_INVALID,
    LVL_ID_CELL_MAX = 250,
    LVL_ID_CELL_ANOMALY = IL_BYTE_ANOMALY,
    LVL_ID_CELL_INVALID = IL_BYTE_INVALID,
    LVL_CELL_VOLTAGE_MAX = 15000,
    LVL_CELL_VOLTAGE_ANOMALY = IL_WORD_ANOMALY,
    LVL_CELL_VOLTAGE_INVALID = IL_WORD_INVALID,
    LVL_ID_TEMPR_MAX = 250,
    LVL_ID_TEMPR_ANOMALY = IL_BYTE_ANOMALY,
    LVL_ID_TEMPR_INVALID = IL_BYTE_INVALID,
    LVL_CODE_TEMPR_MAX = 250,
    LVL_CODE_TEMPR_ANOMALY = IL_BYTE_ANOMALY,
    LVL_CODE_TEMPR_INVALID = IL_BYTE_INVALID,
    LVL_TEMPR_MAX = 250,
    LVL_TEMPR_ANOMALY = IL_BYTE_ANOMALY,
    LVL_TEMPR_INVALID = IL_BYTE_INVALID,
};

typedef struct {
    uint8_t idBattMaxVoltage;  // ID No. of battery subsystem with max. voltage
    uint8_t idCellMaxVoltage;  // ID No. of cell with max. voltage
    uint16_t maxCellVoltage;   // Max. cell voltage
    uint8_t idBattMinVoltage;  // ID No. of battery subsystem with min. voltage
    uint8_t idCellMinVoltage;  // ID No. of cell with min. voltage
    uint16_t minCellVoltage;   // Min. cell voltage
    uint8_t idMaxTemp;         // ID No. of subsystem with max. temperature
    uint8_t codeMaxTemprProbe; // Code of single probe with max. temperature
    uint8_t maxTemp;           // Max. temperature value
    uint8_t idMinTemp;         // ID No. of subsystem with min. temperature
    uint8_t codeMinTemprProbe; // Code of single probe with min. temperature
    uint8_t minTemp;           // Min. temperature value
} LimitValueData;


//                       WarningData
//------------------------------------------------------------------------------

typedef enum {
    WL_LEVEL0 = 0x00,  // 0 - denotes fault- free;
    WL_LEVEL1 = 0x01,  // 1 - denotes fault Level 1, representing fault
                       //     not affecting normal driving of vehicle;
    WL_LEVEL2 = 0x02,  // 2 - denotes fault Level 2, representing fault
                       //     affecting vehicle performance and requiring the
                       //     driver to limit driving;
    WL_LEVEL3 = 0x03,  // 3 - denotes fault Level 3, the fault of the highest
                       //     level, representing fault requiring the driver to
                       //     park for treatment or ask for help;
    WL_LEVEL_ANOMALY = IL_BYTE_ANOMALY,
    WL_LEVEL_INVALID = IL_BYTE_INVALID,
} WarningLevel;

typedef union {
    struct {
        unsigned temprDiff :1;  // 1: Temperature difference warning;
                                // 0: Normal
        unsigned temprBatt :1;  // 1: Battery high-temperature warning;
                                // 0: Normal
        unsigned overVoltageVehicle :1;   // 1: Vehicle energy-storage device
                                          //    type over-voltage warning;
                                          // 0: Normal
        unsigned underVoltageVehicle :1;  // 1: Vehicle energy-storage device
                                          //    type under-voltage warning;
                                          // 0: Normal
        unsigned lowSOC :1;            // 1: Low SOC warning;
                                       // 0: Normal
        unsigned overVoltageCell :1;   // 1: Vehicle energy-storage device type
                                       //    over-voltage warning;
                                       // 0: Normal
        unsigned underVoltageCell :1;  // 1: Vehicle energy-storage device type
                                       //    under-voltage warning;
                                       // 0: Normal
        unsigned highSOC :1;    // 1: Excessively-high SOC warning;
                                // 0: Normal
        unsigned jumpSOC :1;    // 1: SOC jump warning;
                                // 0: Normal
        unsigned unmatched: 1;  // 1: Chargeable energy-storage system unmatched
                                //    warning;
                                // 0: Normal
        unsigned poorConsCell :1;  // 1: Cell poor-consistency warning;
                                   // 0: Normal
        unsigned insulation :1;    // 1: Insulation warning;
                                   // 0: Normal
        unsigned dcdcTempr :1;  // 1: DC-DC temperature warning;
                                // 0: Normal
        unsigned breaks :1;     // 1: Brake system warning
                                // 0: Normal
        unsigned dcdcState :1;  // 1: DC-DC state warning
                                // 0: Normal
        unsigned emcTempr :1;   // 1: Electrical machine controller temperature
                                //    warning;
                                // 0: Normal

        unsigned highVIlock :1;  // 1: High-voltage interlocking state warning
                                 // 0: Normal
        unsigned emTempr :1;     // 1: Electrical machine temperature warning
                                 // 0: Normal
        unsigned overCharge :1;  // 1: Vehicle energy-storage device type
                                 //    over-charging
                                 // 0: Normal
        unsigned reserved :13;
    } bit;
    uint32_t dword;
} WarningMark;



enum WarninLimits {
    WL_CES_SIZE_MAX = IL_BYTE_MAX,
    WL_CES_SIZE_ANOMALY = IL_BYTE_ANOMALY,
    WL_CES_SIZE_INVALID = IL_BYTE_INVALID,
};

typedef struct {
    uint8_t cesSize;                 // Number of chargeable energy-storage
                                     // device fault
    uint32_t cesData[0];             // List of codes of chargeable
                                     // energy-storage device faults
} CesWarningData;

typedef struct {
    uint8_t emfSize;                 // Total number of electrical machine
    uint32_t emfData[0];             // List of codes of electrical machine
                                     // faults
} EmfWarningData;

typedef struct {
    uint8_t efSize;                  // Total number of engine faults
    uint32_t efData[0];              // List of engine faults
} EfWarningData;

typedef struct {
    uint8_t ofSize;                  // Total number of other faults
    uint32_t ofData[0];              // List of other faults
} OfWarningData;

typedef struct {
    WarningLevel level: 8;           // Highest warning level
    WarningMark mark;                // General warning mark

    CesWarningData ceswd;
    EmfWarningData emfwd;
    EfWarningData  efwd;
    OfWarningData  ofwd;
} WarningData;


//                       VoltageCESSData
//------------------------------------------------------------------------------

enum VoltageCESSLimits {
    VCL_CESS_SIZE_MAX = 250,
    VCL_CESS_SIZE_ANOMALY = IL_BYTE_ANOMALY,
    VCL_CESS_SIZE_INVALID = IL_BYTE_INVALID,
    VCL_CESS_CODE_MAX = 250,
    VCL_CESS_CODE_ANOMALY = IL_BYTE_ANOMALY,
    VCL_CESS_CODE_INVALID = IL_BYTE_INVALID,
    VCL_CESS_VOLTAGE_MAX = 10000,   // 0～10,000 (denoting 0V～1,000V);
                                    // minimum measuring unit: 0.1V;
    VCL_CESS_VOLTAGE_ANOMALY = IL_WORD_ANOMALY,
    VCL_CESS_VOLTAGE_INVALID = IL_WORD_INVALID,
    VCL_CESS_CURRENT_MAX = 20000,   // 0～20,000 (denoting -1000A～+1000A);
                                    // minimum measuring unit: 0.1A;
    VCL_CESS_CURRENT_ANOMALY = IL_WORD_ANOMALY,
    VCL_CESS_CURRENT_INVALID = IL_WORD_INVALID,
    VCL_CESS_TOTAL_CELLS_MAX = IL_WORD_MAX,
    VCL_CESS_TOTAL_CELLS_ANOMALY = IL_WORD_ANOMALY,
    VCL_CESS_TOTAL_CELLS_INVALID = IL_WORD_INVALID,
    VCL_CESS_SN_MAX = IL_WORD_MAX,
    VCL_CESS_SN_ANOMALY = IL_WORD_ANOMALY,
    VCL_CESS_SN_INVALID = IL_WORD_INVALID,
    VCL_CESS_FRAME_CELLS_MAX = 200,
    VCL_CESS_FRAME_CELLS_ANOMALY = IL_BYTE_ANOMALY,
    VCL_CESS_FRAME_CELLS_INVALID = IL_BYTE_INVALID,
    VCL_CESS_VOLTAGE_CELLS_MAX = 60000,
    VCL_CESS_VOLTAGE_CELLS_REAL = 96,
    VCL_CESS_VOLTAGE_CELLS_ANOMALY = IL_WORD_ANOMALY,
    VCL_CESS_VOLTAGE_CELLS_INVALID = IL_WORD_INVALID,
};


typedef struct {
    // TCU Hardcode
    uint8_t code;             // Code of chargeable energy-storage subsystem
    uint16_t voltage;         // Voltage of chargeable energy-storage device
    uint16_t current;         // Current of chargeable energy-storage device
    uint16_t totalCells;      // Total number of cell
    uint16_t snCell;          // SN of the starting battery
    // TCU Hardcode always  VCL_CESS_VOLTAGE_CELLS_REAL
    uint8_t frameCells;       // Total cell number of this frame
    uint16_t voltageCells[0]; // Voltage of cells
} VoltageCESSData;

typedef struct {
    // TCU Hardcode always 1
    uint8_t size;             // Number chargeable energy-storage subsystems
    VoltageCESSData data;     // Chargeable energy-storage subsystems data
} VoltageCESSDataCollection;



//                       TemprCESSData
//------------------------------------------------------------------------------

enum TemprCESSLimits {
    TCL_CESS_SIZE_MAX = 250,
    TCL_CESS_SIZE_REAL = 19,
    TCL_CESS_SIZE_ANOMALY = IL_BYTE_ANOMALY,
    TCL_CESS_SIZE_INVALID = IL_BYTE_INVALID,
    TCL_CESS_TEMPR_SIZE_MAX = IL_WORD_MAX,
    TCL_CESS_TEMPR_SIZE_ANOMALY = IL_WORD_ANOMALY,
    TCL_CESS_TEMPR_SIZE_INVALID = IL_WORD_INVALID,
    TCL_CESS_TEMPR_MAX = 250,   // 0~250 value deviation is 40℃,
                                // denoting -40℃～+210℃)
    TCL_CESS_TEMPR_ANOMALY = IL_BYTE_ANOMALY,
    TCL_CESS_TEMPR_INVALID = IL_BYTE_INVALID,
};

typedef struct {
    // TCU Hardcode
    uint8_t code;      // Code of chargeable energy-storage subsystem
    uint16_t size;     // Quantity of chargeable energy-storage
                       // temperature probes TCL_CESS_SIZE_REAL
    uint8_t data[0];   // Temperature value detected by each
                                       // temperature probe of chargeable
                                       // energy-storage subsystem
} TemprCESSData;

typedef struct {
    // TCU Hardcode always 1
    uint8_t size;           // Number chargeable energy-storage subsystems
    TemprCESSData data;     // Chargeable energy-storage subsystems data
} TemprCESSDataCollection;

typedef enum InformationMarkId {
    IM_VEHICLE_DATA = 0x01,
    IM_EMACHINE_DATA = 0x02,
    IM_FUELCELL_DATA = 0x03,
    IM_ENGINE_DATA = 0x04,
    IM_GPS_DATA = 0x05,
    IM_LIMIT_VALUE_DATA = 0x06,
    IM_WARNING_DATA = 0x07,
    IM_VOLTAGE_DATA = 0x08,
    IM_TEMPR_DATA = 0x09,
} InformationMark;

typedef struct {
    InformationMark infoMark: 8;
    union {
        VehicleData vehicleData;
        ElectricalMachineDataCollection electricalDataCollection;
        EngineData engineData;
        GPSData gpsData;
        LimitValueData limitData;
        WarningData warningData;
        VoltageCESSDataCollection voltageCESSCollection;
        TemprCESSDataCollection temprCESSCollection;
    } carrier;
} RealtimeInfoData;

typedef struct {
    Time time;          // Data acquisition time
    RealtimeInfoData info[0];
} RealtimeInfo;

/******************************************************************************
 *                       SupplementaryInfo
 ******************************************************************************/
typedef RealtimeInfo SupplementaryInfo;

/******************************************************************************
 *                       VehicleLogoutnData
 ******************************************************************************/
typedef struct {
    Time time;          // Logout time
    uint16_t sn;        // Logout SN is consistent with the current login SN.
} VehicleLogoutData;

/******************************************************************************
 *                       HeartBeat
 ******************************************************************************/
typedef struct {
    uint8_t empty[0];   // Because of sizeof(HeartBeat) = 0
} HeartBeat;

typedef enum EncryptId {
    ENCRYPT_NOT = 0x01,
    ENCRYPT_RSA = 0x02,
    ENCRYPT_AES = 0x03,
    ENCRYPT_ANOMALY = IL_BYTE_ANOMALY,
    ENCRYPT_INVALID = IL_BYTE_INVALID,
} Encrypt;

typedef struct {
    Time time;          // Data acquisition time
    uint16_t sn;        // Login serial number 0 ~ 65531 (IL_WORD_MAX).
                        // Increment at each login
    char name[12];
    char pass[20];
    Encrypt encrypt: 8;
} PlatformLoginData;



/******************************************************************************
 *                       DataPacket
 ******************************************************************************/


typedef enum ResponseMarkId {
    RM_SUCCESS = 0x01,
    RM_ERROR = 0x02,
    RM_VIN_REPETITION = 0x03,
    RM_BCC_ERROR = 0x04, //CRC
    RM_COMMAND = 0xFE,
    RM_INVALID = IL_BYTE_INVALID
} ResponseMark;

typedef enum CommandUintId {
    CU_VEHICLE_LOGIN = 0x01,
    CU_REALTIME_REPORT = 0x02,
    CU_SUPPLEMENTARY_REPORT = 0x03,
    CU_VEHICLE_LOGOUT = 0x04,
    CU_PLATFORM_LOGIN = 0x05,
    CU_HEARTBEAT = 0x07,
    CU_INVALID = IL_BYTE_INVALID,
} CommandId;

typedef union {
     VehicleLoginData login;
     RealtimeInfo rtinfo;
     SupplementaryInfo sinfo;
     VehicleLogoutData logout;
     HeartBeat heartBeat;
     PlatformLoginData plogin;
} UnitData;

typedef struct {
    char start[2];
    struct {
        CommandId cmd: 8;
        ResponseMark resp: 8;
    } unit;
    char vin[GSL_VIN_LENGTH];
    Encrypt encrypt: 8;
    uint16_t unitLength;  // 0 ~ 65531 (IL_WORD_MAX). 0 for Heartbeat
    UnitData unitData[0]; // Can be one of UnitData or EMPTY for Heartbeat
} DataPacketPart1;

typedef struct {
    uint8_t crc;
} DataPacketPart2;


#pragma pack(pop)


#define GBT_START_CHAR_DATA "##"
#define GBT_START_CHAR_SIZE (sizeof(GBT_START_CHAR_DATA)-1)
#define GBT_DATA_PKT_MIN_SIZE (sizeof(DataPacketPart1) + sizeof(DataPacketPart2))
#define GBT_DATA_CRC_MIN_SIZE (sizeof(DataPacketPart1) - GBT_START_CHAR_SIZE)

//#ifdef __cplusplus
//};
//#endif

#endif /* XEVCDM_INCLUDE_PROTO_GBT32960_3_H_ */
