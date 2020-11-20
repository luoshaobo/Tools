#ifndef VGM_SERVER_HPP
#define VGM_SERVER_HPP

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

/** @file vgm_server.hpp
 * This file handles the simulation of the VGM.
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

#define VGM_SERVICE_ID      0x0666
#define VGM_EVENTGROUP_ID   0x4456

/**
    @brief Simulator of VGM.

    This class is the simulation of the VGM, and inherits from
    LANServer.
*/
class VGMServer : public SomeIPServer {
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
    VGMServer();
    ~VGMServer();

    ReturnValue Init();
    ReturnValue Start();
    ReturnValue Stop();
};

} // namespace vc

#endif // VGM_SERVER_HPP
