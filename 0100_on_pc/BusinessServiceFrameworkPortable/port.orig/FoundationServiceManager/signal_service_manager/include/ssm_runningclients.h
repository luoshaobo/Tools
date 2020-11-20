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
 *  \file     ssm_runningclients.h
 *  \brief    Signal Service Manager
 *  \author   Maksym Mozok & Gustav Evertsson
 *
 *  \addtogroup signalservicemanager
 *  \{
 */

#ifndef FSM_SSM_RUNNINGCLIENTS_H
#define FSM_SSM_RUNNINGCLIENTS_H

#include <cstdint>

namespace fsm
{

namespace signalservicemanager
{

typedef struct SSMClient {
    uint16_t client_id;
    char const ending[1];
}SSMClient_t;

class RunningClients
{
    public:
     RunningClients();
     ~RunningClients();
     int32_t AddClient(SSMClient_t ssmclient_to_add);
     int32_t RemoveClient(uint16_t ui_client_id);
     SSMClient* GetClient(uint16_t ui_client_id);
};

} // namespace signalservicemanager
} // namespace fsm

#endif // FSM_SSM_RUNNINGCLIENTS_H

/** \}    end of addtogroup */
