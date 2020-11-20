/*
* Copyright (C) 2018 Continental Automotive GmbH
*
* Licensed under the "Supplier unrestricted white box" category, as
* described in the 20160322-1_SW-amendment TCAM.pdf agreement
* between Volvo Cars and Continental Automotive GmbH.
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*/

#ifndef VPOM_GDBUS_CLIENT_HPP
#define VPOM_GDBUS_CLIENT_HPP

#include <chrono>
#include <condition_variable>
#include <iomanip>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <getopt.h>
#include <thread>
#include <iostream>
#include <list>
extern "C"
{
#include "tpsys.h"
#include "tplog.h"
}
#include "vpom_IClient.hpp"

class VpomGdbusClient {
private:

public:
    static VpomGdbusClient &getInstance();
    static void SignalHandler(int signum);
    static void GNSSPositionDataRawResponseCb(vpom::GNSSData *param, uint64_t request_id);

    VpomIPositioningService & vpomPositioningProxy_;

    void helpText();

    VpomGdbusClient();

    /**
     * @brief                                   Initialization of test client
     *
     * @return                                  True if initialization was ok.
     *                                          False otherwise.
     */
    bool Init();

    /**
     * @brief                                   De-initialization of test client
     *
     * @return                                  True if de-initialization was ok.
     *                                          False otherwise.
     */
    void Deinit();
};

#endif //VPOM_GDBUS_CLIENT_HPP
