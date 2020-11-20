/**
 * Copyright (C) 2017 Continental Automotive GmbH
 *
 * Licensed under the "Supplier unrestricted white box" category, as
 * described in the 20160322-1_SW-amendment TCAM.pdf agreement
 * between Volvo Cars and Continental Automotive GmbH.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *  \file     ble_manager_mock.h
 *  \brief    Volvo On Call Test Classes
 *  \author   Dariusz Szymczak
 *
 *  \addtogroup TestVolvoOnCall
 *  \{
 */

#include <gmock/gmock.h>

#include "ble_manager_interface.hpp"
#include "voc_base_frame_interface.hpp"

#ifndef BLE_MANAGER_MOCK_H_
#define BLE_MANAGER_MOCK_H_

namespace BLEM
{

// Copied from voc_base_frame.hpp
#define VOCBASEFRAME_ID 0xFE
#define VOCBASEFRAME_SIZE_DATA_MAX 65535
#define VOCBASEFRAME_SIZE_OVERHEAD 4
#define VOCBASEFRAME_POS_LENGTH 1
#define VOCBASEFRAME_POS_TYPE 3
#define VOCBASEFRAME_POS_DATA VOCBASEFRAME_SIZE_OVERHEAD

class VoCBaseFrameMock : public IVoCBaseFrame
{
  private:
    uint32_t idx_buffer_;
    VoCBaseFrameType type_;

  public:
    char buffer_[VOCBASEFRAME_SIZE_DATA_MAX + VOCBASEFRAME_SIZE_OVERHEAD];

    VoCBaseFrameMock(VoCBaseFrameType type);

    bool AddBytes(const char *data, uint16_t sz);
    bool AddByte(const char val);

    // IVoCBaseFrame
    VoCBaseFrameType Type();
    uint16_t Length();
    uint16_t LengthAdded();
    const char* GetData();
    const char* GetBuffer();
    bool AddData(const char *data, uint16_t sz);
    bool IsValid();
    void PrintFrame(bool include_header);
    void PrintFrame(bool include_header, bool direction_out);
};

class PairingServiceFake
{
public:
    ConnectionStateCallback conn_callback_;
    DataCallback data_callback_;
    ConnectionID conn_id_;
    ConnectionState conn_state_ = CONNECTION_STATE_UNKNOWN;
    IVoCBaseFrame *voc_base_frame_ = nullptr;

    void RegisterConnectionStateCallback(ConnectionStateCallback callback);
    void RegisterDataCallback(DataCallback callback);
    void SendData(ConnectionID id, IVoCBaseFrame* frame);
};

class PairingServiceMock : public IPairingService
{
public:
    // IPairingService methods
    // Events
    MOCK_METHOD1(RegisterConnectionStateCallback, BLEMRetVal(ConnectionStateCallback callback));
    MOCK_METHOD1(RegisterDataCallback, BLEMRetVal(DataCallback callback));
    MOCK_METHOD0(DeregisterConnectionStateCallback, BLEMRetVal());
    MOCK_METHOD0(DeregisterDataCallback, BLEMRetVal());
    // Requests
    MOCK_METHOD1(Disconnect, BLEMRetVal(uint16_t));
    MOCK_METHOD0(GenerateBDAK, BLEMRetVal());
    MOCK_METHOD1(GetBDAK, BLEMRetVal(BDAK* bdak));
    MOCK_METHOD0(StartPairingService, BLEMRetVal());
    MOCK_METHOD0(StopPairingService, BLEMRetVal());
    MOCK_METHOD2(SendData, BLEMRetVal(ConnectionID connection_id, IVoCBaseFrame* frame));
    MOCK_METHOD1(SetChunkSize, void(uint8_t));

    // PairingServiceMock methods
    void DelegateToFake();
    void SetConnectionState(ConnectionID id, ConnectionState state);
    void SendConnectionState();
    void SetFrame(IVoCBaseFrame *frame);
    void SendFrame();

private:
    PairingServiceFake pairing_service_fake_;
};

};  // namspace BLEM

#endif  // #ifndef BLE_MANAGER_MOCK_H_
