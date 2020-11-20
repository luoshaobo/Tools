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
 *  \file     event_listener_interface.h
 *  \brief    Foundation Services Daemon Event Listener interface
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#ifndef FSM_FSCFGD_EVENT_LISTENER_INTERFACE_H_INC_
#define FSM_FSCFGD_EVENT_LISTENER_INTERFACE_H_INC_

#include <fscfg/fscfg_types.h>

namespace fsm
{

struct Event;

/**
 * \brief EventListener interface.
 * Implementors allow de-coupled execution of a given scheduled event.
 */
class EventListenerInterface
{
public:
    /**
     * \brief EventListenerInterface destructor.
     */
    virtual ~EventListenerInterface() { }

    /**
     * \brief Execute the given event.
     *
     * \param[in] event event that is scheduled for execution.
     *
     * \return fscfg_kRcSuccess on success.
     */
    virtual fscfg_ReturnCode ExecuteEvent(fsm::Event event) = 0;
};

}

#endif // FSM_FSCFGD_EVENT_LISTENER_INTERFACE_H_INC_

/** \}    end of addtogroup */
