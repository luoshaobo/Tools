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
/**
 *
 * @file vpom_common_service.hpp
 * @brief Common Interface
 *
 */

#ifndef VPOM_COMMON_SERVICE_HPP
#define VPOM_COMMON_SERVICE_HPP

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <cstdint>
#include <list>

#define VPOM_DBUS_SERVICE              "com.contiautomotive.tcam.vpom"           ///< VPOM dbus service
#define VPOM_DBUS_SERVER_NAME          "/com/contiautomotive/tcam/vpom"          ///< VPOM dbus server name
#define VPOM_DBUS_OBJECT               "/com/contiautomotive/tcam/vpom/Manager"  ///< VPOM dbus object

namespace vpom
{
typedef enum {
    SubscribeStart = 0x05,
    SubscribeStop = 0xFE,
    SubscribeAllStop = 0xFF
} SubscribeType;

typedef void (*fgen)(void);

typedef struct {
    fgen cb;
    uint32_t requestPending;
} VpomCallbackType;

typedef enum {
    RegItemGNSSPositionDataRawResponse,
    VpomNrOfRegisteredItems,
} VpomRegisteredItem;

} //vpom

#endif //VPOM_COMMON_SERVICE_HPP
