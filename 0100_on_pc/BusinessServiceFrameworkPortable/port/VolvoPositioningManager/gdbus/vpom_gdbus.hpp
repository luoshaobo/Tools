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
 * @file vpom_gdbus.hpp
 * @brief This file is used for gdbus handling in VPOM
 *
 */

#ifndef VPOM_GDBUS_HPP
#define VPOM_GDBUS_HPP

#include <functional>
#include <dbus/dbus.h>
#include <dbus/dbus-shared.h>
#include "vpom-ifc-generated.h"
#include <vpom.hpp>
#include "vpom_IPositioningService.hpp"
#include <condition_variable>

#define VPOM_GDBUS_ACQUIRED_TIMEOUT_MS 3000            ///< VPOM Gdbus aquired timeout in milliseconds

namespace vpom
{

struct RequestResponse {
    vpomManager *vpomm;
    GDBusMethodInvocation *invocation;
    uint64_t request_id;
    bool setrequest;
};

} // namespace vpom

class VPOM_GDBus : public std::thread {
public:

    /**
     * @brief    Constructor
     */
    VPOM_GDBus(VolvoPositioningManager *volvopositioningmanager);

    /**
     * @brief    Constructor
     */
    VPOM_GDBus();

    /**
     * @brief   Destructor
     */
    ~VPOM_GDBus();

    /**
     * @brief   Instance getter
     */
    static VPOM_GDBus &getInstance();

    /**
     * @brief Thread for gdbus support
     *
     * @return void
     */
    void run();

    /**
     * @brief Function for waiting on dbus acquired
     */
    bool WaitForDbusAcquired(void);

    /**
     * @brief Function for sending GNSSPositionData Response gdbus signal
     *
     * @param[in] gnssData                      All gnss data params
     *
     * @return int
     */
    int GNSSPositionDataRawResponse(GNSSData *gnssData, uint64_t resuest_id);

private:
    static VolvoPositioningManager *volvopositioningmanager_;
    static GDBusConnection *vpom_dbus_conn_;
    static vpomManager *ivpom_;
    guint32 owner_id_;

    //Methods begin
    static gboolean handleGNSSPositionDataRawRequest_(vpomManager *vpomm,
                                                      GDBusMethodInvocation *invocation,
                                                      guint64 request_id);
    //Methods end

    static void RegisterDbusCallbacks(vpomManager *vpomm);
    static void OnBusAcquired(GDBusConnection *connection, const gchar *name, gpointer user_data);
    static void OnNameAcquired(GDBusConnection *connection, const gchar *name, gpointer user_data);
    static void OnNameLost(GDBusConnection *connection, const gchar *name, gpointer user_data);
    bool Initialize();
};

#endif // VPOM_GDBUS_HPP
