/**
* Copyright (C) 2016 Continental Automotive GmbH
*
* Licensed under the "Supplier unrestricted white box" category, as
* described in the 20160322-1_SW-amendment TCAM.pdf agreement
* between Volvo Cars and Continental Automotive GmbH.
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*/

/** @file vehicle_comm.hpp
 * This file handles add-on test-functions for VehicleComm.
 *
 * @project     VCC
 * @subsystem   VehicleComm
 * @version     00.01
 * @author      Ola Lilja & Johan Bohlin
 * @date        07-Dec-2016
 ***************************************************************************/

#include "vehicle_comm_test.hpp"

namespace vc {


#define MESSAGE_INTERVAL 2000
#define MESSAGE_SOURCE ENDPOINT_TS
#define FORCE_DESTINATION 1
#define MESSAGE_DEST ENDPOINT_VUC

std::thread VehicleCommTest::loopThread;
std::atomic<bool> VehicleCommTest::exit_;
MessageQueue *VehicleCommTest::mq_;

void VehicleCommTest::SendMessage()
{
    LOG(LOG_DEBUG, "VehicleCommTest: %s.", __FUNCTION__);

    // Create a test-message
    CryptoTelmLockgCenReq mCryptoTelmLockgCenReq;
    mCryptoTelmLockgCenReq.ReqTyp = TelmNoReq;
    mCryptoTelmLockgCenReq.TiVld = 2;
    mCryptoTelmLockgCenReq.Cntr = 12;
    mCryptoTelmLockgCenReq.AuthentCod = 666;
    MessageBase *m = InitMessage(VCMSG_CRYPTODOORLOCKREQ, MESSAGE_SOURCE, reinterpret_cast<const unsigned char*>(&mCryptoTelmLockgCenReq));
    if (FORCE_DESTINATION)
        m->dst_ = MESSAGE_DEST;

    (void)mq_->AddMessage(m);
}

void VehicleCommTest::StartTestThread(MessageQueue *mq)
{
    LOG(LOG_DEBUG, "%s.", __FUNCTION__);

    mq_ = mq;
    exit_ = false;

    loopThread = std::thread(&VehicleCommTest::Loop);
}

void VehicleCommTest::StopTestThread()
{
    LOG(LOG_DEBUG, "%s.", __FUNCTION__);
    exit_ = true;
    loopThread.join();
}

void VehicleCommTest::Loop()
{
    LOG(LOG_DEBUG, "%s.", __FUNCTION__);
    while (!exit_) {
        SendMessage();
        std::this_thread::sleep_for(std::chrono::milliseconds(MESSAGE_INTERVAL));
    }
    LOG(LOG_DEBUG, "%s Exiting message-loop....", __FUNCTION__);
}

} // namespace vc
