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
 *  \file     ble_manager_mock.cc
 *  \brief    Volvo On Call Test Classes
 *  \author   Dariusz Szymczak
 *
 *  \addtogroup TestVolvoOnCall
 *  \{
 */

#include "ble_manager_mock.h"
#include "dlt/dlt.h"
#include <cstring>

DLT_DECLARE_CONTEXT(ut);

namespace BLEM
{

/*class VoCBaseFrameMock : public IVoCBaseFrame
{
public:

};*/

// VoCbaseFrameMock implementation
VoCBaseFrameMock::VoCBaseFrameMock(VoCBaseFrameType type)
{
    idx_buffer_ = 0;
    type_ = type;
}

bool VoCBaseFrameMock::AddBytes(const char *data, uint16_t sz)
{
    bool result = true;

    if (sz + idx_buffer_ > VOCBASEFRAME_SIZE_DATA_MAX + VOCBASEFRAME_SIZE_OVERHEAD)
    {
        result = false;
    }
    else
    {
        std::memcpy(&buffer_[idx_buffer_], data, sz);
        idx_buffer_ += sz;
    }

    return result;
}

bool VoCBaseFrameMock::AddByte(const char val)
{
    buffer_[idx_buffer_++] = val;

    return true;
}


// VoCbaseFrameMock IVoCBaseFrame implementation

VoCBaseFrameType VoCBaseFrameMock::Type()
{
    return type_;
}

uint16_t VoCBaseFrameMock::Length()
{
    return buffer_[VOCBASEFRAME_POS_LENGTH] << 8 | buffer_[VOCBASEFRAME_POS_LENGTH+1];
}

uint16_t VoCBaseFrameMock::LengthAdded()
{
    DLT_LOG_STRINGF(ut, DLT_LOG_WARN, "VoCBaseFrameMock::%s() NOT IMPLEMENTED", __func__);
    return 0;
}

bool VoCBaseFrameMock::AddData(const char *data, uint16_t sz)
{
    return AddBytes(data, sz);
}

bool VoCBaseFrameMock::IsValid()
{
    DLT_LOG_STRINGF(ut, DLT_LOG_WARN, "VoCBaseFrameMock::%s() NOT IMPLEMENTED", __func__);
    return true;
}

const char* VoCBaseFrameMock::GetData()
{
    DLT_LOG_STRINGF(ut, DLT_LOG_WARN, "VoCBaseFrameMock::%s() NOT IMPLEMENTED", __func__);
    return nullptr;
}

const char* VoCBaseFrameMock::GetBuffer()
{
    return buffer_;
}

void VoCBaseFrameMock::PrintFrame(bool include_header)
{
    DLT_LOG_STRINGF(ut, DLT_LOG_WARN, "VoCBaseFrameMock::%s() NOT IMPLEMENTED", __func__);
}

void VoCBaseFrameMock::PrintFrame(bool include_header, bool direction_out)
{
    DLT_LOG_STRINGF(ut, DLT_LOG_WARN, "VoCBaseFrameMock::%s() NOT IMPLEMENTED", __func__);
}

// PairingServiceFake implementation

void PairingServiceFake::RegisterConnectionStateCallback(ConnectionStateCallback callback)
{
    conn_callback_ = callback;
}

void PairingServiceFake::RegisterDataCallback(DataCallback callback)
{
    data_callback_ = callback;
}

void PairingServiceFake::SendData(ConnectionID id, IVoCBaseFrame* frame)
{
    if (frame)
    {
        delete frame;
    }
}


// PairingServiceMock implementation

void PairingServiceMock::DelegateToFake()
{
    ON_CALL(*this, RegisterConnectionStateCallback(::testing::_)).
            WillByDefault(DoAll(::testing::Invoke(&pairing_service_fake_, &PairingServiceFake::RegisterConnectionStateCallback),
                                ::testing::Return(BLEM_OK)));

    ON_CALL(*this, RegisterDataCallback(::testing::_)).
            WillByDefault(DoAll(::testing::Invoke(&pairing_service_fake_, &PairingServiceFake::RegisterDataCallback),
                                ::testing::Return(BLEM_OK)));

    ON_CALL(*this, SendData(::testing::_, ::testing::_)).
            WillByDefault(DoAll(::testing::Invoke(&pairing_service_fake_, &PairingServiceFake::SendData),
                                ::testing::Return(BLEM_OK)));
}

void PairingServiceMock::SetConnectionState(ConnectionID id, ConnectionState state)
{
    pairing_service_fake_.conn_id_ = id;
    pairing_service_fake_.conn_state_ = state;
}

void PairingServiceMock::SendConnectionState()
{
    pairing_service_fake_.conn_callback_(pairing_service_fake_.conn_id_,
                                         pairing_service_fake_.conn_state_);
}

void PairingServiceMock::SetFrame(IVoCBaseFrame *frame)
{
    pairing_service_fake_.voc_base_frame_ = frame;
}

void PairingServiceMock::SendFrame()
{
    pairing_service_fake_.data_callback_(pairing_service_fake_.conn_id_,
            pairing_service_fake_.voc_base_frame_);
}

IPairingService* IBLEManager::GetPairingServiceInterface()
{
    static PairingServiceMock pairing_service_mock;
    return &pairing_service_mock;
}

IVoCBaseFrame* IVoCBaseFrame::CreateVoCBaseFrame(VoCBaseFrameType type, uint16_t length)
{
    VoCBaseFrameMock *bframe = new VoCBaseFrameMock(type);
    bframe->AddByte(VOCBASEFRAME_ID);
    bframe->AddByte(length >> 8);
    bframe->AddByte(length & 0xFF);
    bframe->AddByte((uint8_t)type);

    return (IVoCBaseFrame*)bframe;
}

}
