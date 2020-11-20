#ifndef VEHICLE_COMM_INTERFACE_HPP
#define VEHICLE_COMM_INTERFACE_HPP

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

/** @file ts_comm_lib.hpp
 * This file implements a library to talk to vehiclecomm (tscomm) over dbus
 *
 * @project     VCC
 * @subsystem   VehicleComm
 * @version     00.01
 * @author      Johan Bohlin & Ola Lilja
 * @date        07-Dec-2016
 ***************************************************************************/

#include <functional>

extern "C"
{
//#include "VehicleComm-ifc-generated.h"
}

#include "vc_common.hpp"
#include "vc_general_interface.hpp"
#include "vc_ble_interface.hpp"
#include "vc_diagnostics_interface.hpp"

namespace vc {

#define VC_DBUS_SERVICE              "com.contiautomotive.tcam.VehicleComm"
#define VC_DBUS_OBJECT               "/com/contiautomotive/tcam/VehicleComm/iface"

#define CREATE_DATA(x)  reinterpret_cast<const unsigned char*>(&x)

/**
    @brief Main interface for external users.

    This class is the main object that is instantiated by a VehicleComm-client wanting to use
    the functionality of VehicleComm.
    
    Usage:
    1. Include the main hpp-file: 
        >> #include "vehicle_comm_interface.hpp"
    2. Create and gets the main VehicleComm interface-object:
        >> vc::IVehicleComm& ivc = vc::IVehicleComm::GetInstance();
    3. Register callbacks for responses and events:
    4. Get a reference to area-specific methods:
        >> vc::IGeneral& igen = vc::IVehicleComm::GetGeneralInterface();
        >> vc::IBLE& ible = vc::IVehicleComm::GetBLEInterface();
        >> vc::IDiagnostics& idiag = vc::IVehicleComm::GetDiagnosticsInterface();
    5. Enable events:
        >> vc::ReturnValue ret;
        >> ret = ivc.EnableGeneralEvents();
        >> ret = ivc.EnableBLEEvents();
        >> ret = ivc.EnableDiagnosticsEvents();
    6. Start using the interfaces.
*/
class IVehicleComm : IGeneral, IBLE, IDiagnostics {
  protected:
    // Prevent direct initialization
    IVehicleComm() {};
    IVehicleComm(const IVehicleComm&) = delete;
    IVehicleComm& operator=(const IVehicleComm&) = delete;
    ~IVehicleComm() {};

  public:
    static IVehicleComm& GetInstance();

    // Initialization
    static IGeneral& GetGeneralInterface();
    static IBLE& GetBLEInterface();
    static IDiagnostics& GetDiagnosticsInterface();
    
    // Events
    virtual ReturnValue EnableGeneralEvents(void) = 0;
    virtual ReturnValue DisableGeneralEvents(void) = 0;
    virtual ReturnValue EnableBLEEvents(void) = 0;
    virtual ReturnValue DisableBLEEvents(void) = 0;
    virtual ReturnValue EnableDiagnosticsEvents(void) = 0;
    virtual ReturnValue DisableDiagnosticsEvents(void) = 0;

    virtual ReturnValue Init(ResponseCallback response_callback, EventCallback event_callback) = 0;
};

} // namespace vc

#endif //VEHICLE_COMM_INTERFACE_HPP
