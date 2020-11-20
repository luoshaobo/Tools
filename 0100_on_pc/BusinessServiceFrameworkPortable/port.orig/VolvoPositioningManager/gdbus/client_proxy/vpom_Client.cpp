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

#include "vpom_IClient.hpp"
#include "vpom_gdbus_client_proxy.hpp"

VpomGdbusClientProxy VpomIClient::m_VpomGdbusClientProxy;  // instance of actual implementation (GDbus proxy)

/**
* @brief  Function called by client; needed mainly to check the pre-conditions and initialize underlying IPC initialization
*
* @return True if initialization was ok - the client can be used
*         False otherwise - the client should NOT be used
*/
bool VpomIClient::Init()
{
    return m_VpomGdbusClientProxy.Init();
}

/**
* @brief   Function called by client to de-initialize the underlying IPC initialization
*
* @return  True if de-initialization was ok.
*          False otherwise.
*/
bool VpomIClient::Deinit()
{
    return m_VpomGdbusClientProxy.Deinit();
}

/**
* @brief   Provides access to Positioning functions offerred by VolvoPositionManager
*
* @return  Reference to GnssService
*/
VpomIPositioningService & VpomIClient::GetPositioningService()
{
    return m_VpomGdbusClientProxy;
}
