///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// @file L3g4200d.h
//	This file handles the Gyro chip L3G4200D.
// @project 	GLY_TCAM
// @subsystem	Application
// @author		Lu Yang
// @Init date	18-Feb-2019
///////////////////////////////////////////////////////////////////
#ifndef L3G4200D_DRIVER_H__
#define L3G4200D_DRIVER_H__

/** Device Identification (Who am I) **/
#define L3G4200D_WHO_AM_I                0x0F

/** Control registers **/
#define L3G4200D_CTRL_REG1               0x20
#define L3G4200D_CTRL_REG2               0x21
#define L3G4200D_CTRL_REG3               0x22
#define L3G4200D_CTRL_REG4               0x23
#define L3G4200D_CTRL_REG5               0x24
#define L3G4200D_OUT_TEMP                0x26
#define L3G4200D_STATUS_REG            0x27

/** Control registers **/
#define L3G4200D_OUT_X_L                   0x28
#define L3G4200D_OUT_X_H                  0x29
#define L3G4200D_OUT_Y_L                   0x2A
#define L3G4200D_OUT_Y_H                  0x2B
#define L3G4200D_OUT_Z_L                   0x2C
#define L3G4200D_OUT_Z_H                   0x2D


#define L3G4200D_FIFO_CTRL_REG       0x2E

typedef union{
	int16_t i16bit[3];
	uint8_t u8bit[6];
} axis3bit16_t;

typedef enum{
    GRYO_YAW_X = 1,
    GRYO_YAW_Y,
    GRYO_YAW_Z,
}GRYO_YAW_DIRECTION;

/************************************************************/
// @brief : init device
// @param[in]  initial_signal,fsm::Signal, which caused transaction to be created.
// @return
// @author     
/************************************************************/
void gyro_dev_init();


/************************************************************/
// @brief : Read YAW data
// @param[in]  direction,  specify the direction of YAW
//             [out] yaw,  yaw data
// @return
// @author     
/************************************************************/
void gyro_read_yaw(int16_t*  yaw, GRYO_YAW_DIRECTION direction);
uint8_t get_device_id();

#endif

