/*
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
 *  \file     ssm_runningclients.cpp
 *  \brief    Signal Service Manager
 *  \author   Maksym Mozok & Gustav Evertsson
 *
 *  \addtogroup signalservicemanager
 *  \{
 */

#include "ssm_runningclients.h"

namespace fsm
{

namespace signalservicemanager
{

RunningClients::RunningClients()
{
    return;
};
RunningClients::~RunningClients()
{
    return;
}

int32_t RunningClients::AddClient(SSMClient_t ssmclient_to_add)
{
    int32_t rc=0;
    return rc;
}

int32_t RunningClients::RemoveClient(uint16_t ui_client_id)
{
    int32_t rc=0;
    return rc;
}

SSMClient* RunningClients::GetClient(uint16_t ui_client_id)
{
    SSMClient* pclient=nullptr;
    return pclient;
}

} // namespace signalservicemanager
} // namespace fsm

/** \}    end of addtogroup */
