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
 *  \file     utils.cc
 *  \brief    FSM, voc_framework, unittest, common utils
 *  \author   Dariusz Szymczak
 *
 *  \addtogroup unittest
 *  \{
 */

/******************************************************************************
 * INCLUDE FILES
 ******************************************************************************/

#include "utils.h"

#include <gtest/gtest.h>

#include <memory>

#include "voc_framework/signals/signal.h"


void TestSignalReceiverInterface::ProcessSignal(std::shared_ptr<fsm::Signal> signal)
{
    ++process_signal_count_;
}


/** \}    end of addtogroup */
