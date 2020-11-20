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
 *  \file     utils.h
 *  \brief    FSM, voc_framework, unittest, common utils
 *  \author   Dariusz Szymczak
 *
 *  \addtogroup unittest
 *  \{
 */

/******************************************************************************
 * INCLUDE FILES
 ******************************************************************************/

#include "voc_framework/signals/ccm_encodable.h"
#include "voc_framework/signal_sources/signal_receiver_interface.h"
#include "voc_framework/signals/ccm_encodable.h"

#ifndef UNITTEST_VOC_FRAMEWORK_UTILS_H_
#define UNITTEST_VOC_FRAMEWORK_UTILS_H_

const std::string uplink_topic_low = "uplink topic_low";
const std::string uplink_topic_normal = "uplink topic_normal";
const std::string uplink_topic_high = "uplink topic_high";
const std::string downlink_topic_low = "downlink topic_low";
const std::string downlink_topic_normal = "downlink topic_normal";
const std::string downlink_topic_high = "downlink topic_high";
const int high_prio = 80;
const int normal_prio = 50;
const int low_prio = 20;

const std::string test_oid("1.2.3.4.5");


/**
 * \brief This class is to override SignalReceiverInterface::ProcessSignal for testing purpose
 */
class TestSignalReceiverInterface : public fsm::SignalReceiverInterface
{
public:
    /**
     * \brief Incremented in ProcessSignal to indicate a signal has been received
     */
    uint32_t process_signal_count_ = 0;

    /**
     * \brief Override parent class function to test that signals are delivered
     */
    void ProcessSignal(std::shared_ptr<fsm::Signal> signal) override;
};


#endif  // UNITTEST_VOC_FRAMEWORK_UTILS_H_

/** \}    end of addtogroup */
