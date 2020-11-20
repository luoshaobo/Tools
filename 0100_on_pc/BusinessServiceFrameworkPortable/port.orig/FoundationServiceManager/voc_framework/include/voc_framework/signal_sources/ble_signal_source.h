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
 *  \file     ble_signal_source.h
 *  \brief    BLE signal source.
 *  \author   Dariusz Szymczak
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_FRAMEWORK_SIGNAL_SOURCES_BLE_SIGNAL_SOURCE_H_
#define VOC_FRAMEWORK_SIGNAL_SOURCES_BLE_SIGNAL_SOURCE_H_

/******************************************************************************
 * INCLUDE FILES
 ******************************************************************************/

#include <condition_variable>
#include <memory>

#include <icommunicationmanagerproxy.h>
#include <icommunicatorproxy.h>

#include "signal_source.h"

namespace fsm
{

/**
 * \brief BLE signal source class that provides interface to BLE data communication services.
 */
class BleSignalSource : public SignalSource, public ICommunicationManagerProxyEvents
{

 public:

    static const uint32_t kComaTimeout; // Number of seconds to wait until coma activation time-out

    /**
     * \brief Constructor
     */
    BleSignalSource();

    /**
     * \brief Destructor
     */
    ~BleSignalSource();

    /**
     * \brief Callback interface from Coma for incoming BLE messages.
     * \param[in] event Data about the event.
     */
    void handleEvent(const NotificationBluetoothLEIncomingMessage* event) override;

 private:

    /**
     * \brief BLE signal source error codes.
     */
    typedef enum
    {
        kUndefined,                 ///< Uninitialized value.

        kSuccess,                   ///< Operation was executed with success.
        kFailed,                    ///< Operation failed without specific reason.
        kBadState,                  ///< Object is in bad state and cannot satifsy operation.
        kBadValue,                  ///< Unexpected or disallowed value.
        kTimeout,                   ///< Operation wait-time expired.

        kMax                        ///< Maximum value type should hold. Keep this last.
    } ResultCode;

    // Callbacks for Coma
    void ComaInitCommunicationMgrProxyCallback(ResponseComaInitClient& response,
                          std::shared_ptr<std::condition_variable> cv);

    ResultCode ComaGetCommunicationMgrProxy();
    ResultCode ComaInitCommunicationMgrProxy();
    void ComaAttachBleEvents();
    void ComaDetachBleEvents();

    std::mutex global_sync_;
    ICommunicationManagerProxy* coma_com_mgr_ = nullptr;
    bool coma_init_callback_received_ = false;
};

} // namespace fsm

#endif //VOC_FRAMEWORK_SIGNAL_SOURCES_BLE_SIGNAL_SOURCE_H_

/** \}    end of addtogroup */
