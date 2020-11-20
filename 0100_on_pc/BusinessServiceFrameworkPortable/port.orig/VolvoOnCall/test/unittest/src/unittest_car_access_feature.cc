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
 *  \file     unittest_car_access_feature.cc
 *  \brief    Volvo On Call, unit tests, car access feature
 *  \author   Dariusz Szymczak
 *
 *  \addtogroup TestVolvoOnCall
 *  \{
 */

/******************************************************************************
 * INCLUDE FILES
 ******************************************************************************/
#include "voc_framework/signal_sources/car_access_signal_source.h"
#include "voc_framework/signals/signal.h"
#include "voc_framework/signals/signal_factory.h"
#include "test_classes.h"

#include <fstream>
#include <vector>

#include <gtest/gtest.h>

namespace voc_feature_test
{

using namespace volvo_on_call;
using namespace voc_test_classes;

static const std::string uplink_topic_low = "uplink topic_low";
static const std::string uplink_topic_normal = "uplink topic_normal";
static const std::string uplink_topic_high = "uplink topic_high";
static const std::string downlink_topic_low = "downlink topic_low";
static const std::string downlink_topic_normal = "downlink topic_normal";
static const std::string downlink_topic_high = "downlink topic_high";
static const int high_prio = 80;
static const int normal_prio = 50;
static const int low_prio = 20;

static void add_topics(std::shared_ptr<fsm::CarAccessSignal> signal)
{
    // Uplink topics
    fsm::CarAccessSignal::MqttTopicInfo uplink_topic_1;
    uplink_topic_1.topic = uplink_topic_high;
    uplink_topic_1.priority = high_prio;
    fsm::CarAccessSignal::MqttServiceInfo service_info;
    service_info.oid = BccCl002Signal::oid;
    uplink_topic_1.services.push_back(service_info);
    service_info.oid = BccCl011Signal::oid;
    uplink_topic_1.services.push_back(service_info);
    signal->AddUplinkTopic(uplink_topic_1);

    fsm::CarAccessSignal::MqttTopicInfo uplink_topic_2;
    uplink_topic_2.topic = uplink_topic_normal;
    uplink_topic_2.priority = normal_prio;
    // services are optional, dont set any for normal
    signal->AddUplinkTopic(uplink_topic_2);

    fsm::CarAccessSignal::MqttTopicInfo uplink_topic_3;
    uplink_topic_3.topic = uplink_topic_low;
    uplink_topic_3.priority = low_prio;
    service_info.oid = BasicCarControlSignal::oid;
    uplink_topic_3.services.push_back(service_info);
    service_info.oid = BccCl002Signal::oid;
    uplink_topic_3.services.push_back(service_info);
    signal->AddUplinkTopic(uplink_topic_3);


    // Downlink topics
    fsm::CarAccessSignal::MqttTopicInfo downlink_topic_1;
    downlink_topic_1.topic = downlink_topic_high;
    downlink_topic_1.priority = high_prio;
    signal->AddDownlinkTopic(downlink_topic_1);

    fsm::CarAccessSignal::MqttTopicInfo downlink_topic_2;
    downlink_topic_2.topic = downlink_topic_normal;
    downlink_topic_2.priority = normal_prio;
    signal->AddDownlinkTopic(downlink_topic_2);

    fsm::CarAccessSignal::MqttTopicInfo downlink_topic_3;
    downlink_topic_3.topic = downlink_topic_low;
    downlink_topic_3.priority = low_prio;
    signal->AddDownlinkTopic(downlink_topic_3);
}


/******************************************************************************
 * TESTS
 ******************************************************************************/
TEST (CarAccessFeatureTest, GetUplinkTopic)
{

    fsm::CarAccessSignalSource& car_access_feature = fsm::CarAccessSignalSource::GetInstance();

    fsm::SetCertsForDecode();

    // Should return topic with normal priority if requested OID not found
    std::string topic = car_access_feature.GetUplinkTopic("not_supported_OID");
    EXPECT_TRUE(topic == uplink_topic_normal);

    // Should return topic with high priority if requested OID present on that prio
    topic = car_access_feature.GetUplinkTopic(BccCl002Signal::oid);
    EXPECT_TRUE(topic == uplink_topic_high);

    // Should return topic with high priority if requested OID present on that prio
    // and another prio requested (i.e. prio should be ignored if OID found)
    topic = car_access_feature.GetUplinkTopic(BccCl002Signal::oid, low_prio);
    EXPECT_TRUE(topic == uplink_topic_high);

    // Should return topic with highest priority out of topics that support same OID
    topic = car_access_feature.GetUplinkTopic(BccCl002Signal::oid);
    EXPECT_TRUE(topic == uplink_topic_high);

    // Should return topic with >= priority than the requested priority
    topic = car_access_feature.GetUplinkTopic("not_supported_OID", high_prio-20);
    EXPECT_TRUE(topic == uplink_topic_high);

    // Should return topic with requested priority
    topic = car_access_feature.GetUplinkTopic("not_supported_OID", low_prio);
    EXPECT_TRUE(topic == uplink_topic_low);
}

TEST (CarAccessFeatureTest, GetDownlinkTopicPriority)
{

    fsm::CarAccessSignalSource& car_access_feature = fsm::CarAccessSignalSource::GetInstance();

    fsm::SetCertsForDecode();

    // Should return kPriorityUndefined if requested topic not found
    int priority = car_access_feature.GetDownlinkTopicPriority("not_supported_topic");
    EXPECT_TRUE(priority == fsm::Signal::kPriorityUndefined);

    // Should return uplink_topic_low_prio if uplink_topic_low requested
    priority = car_access_feature.GetDownlinkTopicPriority(downlink_topic_low);
    EXPECT_TRUE(priority == low_prio);
}

}  // namespace voc_feature_test
