#ifndef IHU_SERVER_HPP
#define IHU_SERVER_HPP

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

/** @file ihu_server.hpp
 * This file handles the simulation of the IHU.
 *
 * @project     VCC
 * @subsystem   VehicleComm
 * @version     00.01
 * @author      Ola Lilja & Johan Bohlin
 * @date        07-Dec-2016
 ***************************************************************************/

#include <vsomeip/vsomeip.hpp>

#include "vc_common.hpp"

#include "someip_server.hpp"

namespace vc {

#define IHU_SERVICE_ID      0x0888
#define IHU_EVENTGROUP_ID   0x4455

/**
    @brief Simulator of IHU.

    This class is the simulation of the IHU, and inherits from
    LANServer.
*/
class IHUServer : public SomeIPServer {
  private:
    // Overrides - SomeIPServers

    // Methods for the client-part
    void HandleAvailability(bool _is_available);
    void HandleResponse(const std::shared_ptr<vsomeip::message> &msg);
    void HandleRequestAck(const std::shared_ptr<vsomeip::message> &msg);
    void HandleEvent(const std::shared_ptr<vsomeip::message> &msg);

    // Methods for the server-part
    void HandleRequest(const std::shared_ptr<vsomeip::message> &msg);

  public:
    IHUServer();
    ~IHUServer();

    ReturnValue Init();
    ReturnValue Start();
    ReturnValue Stop();
};

} // namespace vc

#endif // IHU_SERVER_HPP
