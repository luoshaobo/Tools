/**
 * Copyright (C) 2017 Continental Automotive GmbH
 *
 * Licensed under the "Supplier unrestricted white box" category, as
 * described in the 20160322-1_SW-amendment TCAM.pdf agreement
 * between Volvo Cars and Continental Automotive GmbH.
 *
 * Unless required by applicable law or agreed to in writing, software
% * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *  \file     persistence_stub.h
 *  \brief    OTP Persistence API stub control interface
 *  \author   Iulian Sirghi
 *
 *  \addtogroup unittest
 *  \{
 */

#ifndef PERSISTENCE_STUB_H
#define PERSISTENCE_STUB_H

#include <persistence_client_library.h>

#include <cstdint>

void tpPCL_stub_setErrorCode(tpPCL_Error_t err_code);

uint32_t tpPCL_stub_getCallCount();

void tpPCL_stub_setCallCount(uint32_t call_count);

void tpPCL_stub_clean();


/** \}    end of addtogroup */

#endif // PERSISTENCE_STUB_H
