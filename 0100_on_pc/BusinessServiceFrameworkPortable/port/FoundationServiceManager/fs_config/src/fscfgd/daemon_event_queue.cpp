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
 *  \file     daemon_event_queue.cpp
 *  \brief    Foundation Services Daemon Event Queue GSource wrapper implementation
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#include <unistd.h>

#include <cstring>

#include <fscfgd/daemon_event_queue.h>

namespace fsm
{

DaemonEventQueue::DaemonEventQueue()
 : local_sync_(),
   event_queue_source_(nullptr),
   event_queue_functions_(),
   event_queue_(),
   listeners_(),
   event_descriptor_(-1)
{
    // Prepare the event queue functions.
    memset(&event_queue_functions_, 0, sizeof(event_queue_functions_));

    event_queue_functions_.prepare = EventQueueGSourcePrepare;
    event_queue_functions_.dispatch = EventQueueGSourceDispatch;

    // Allocate our representation of GSource. (specialized version that internally hold pointer to this).
    // We will be able to access the event queue instance from the Glib call.
    event_queue_source_ = reinterpret_cast<EventQueueGSource*>(g_source_new(&event_queue_functions_,
                                                                            sizeof(EventQueueGSource)));

    event_descriptor_ = eventfd(0, EFD_SEMAPHORE);

    if (event_descriptor_ == -1)
    {
        DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_INFO, "DaemonEventQueue: eventfd call failed!");
    }
    else
    {
        g_source_add_unix_fd(reinterpret_cast<GSource*>(event_queue_source_), event_descriptor_, G_IO_IN);
    }
}

DaemonEventQueue::~DaemonEventQueue()
{
    // When destroyed, GSource will be automatically detached.
    g_source_destroy(reinterpret_cast<GSource*>(event_queue_source_));
}

fscfg_ReturnCode DaemonEventQueue::Attach(GMainContext* main_context)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!main_context)
    {
        rc = fscfg_kRcBadParam;
    }
    else if (event_descriptor_ == -1)
    {
        rc = fscfg_kRcError;
    }
    else
    {
        // Attach the event queue to the context.
        // This way, the context will always check all attached sources for events.
        // When the context is destroyed, all sources are deleted.
        event_queue_source_->context = main_context;
        event_queue_source_->daemon_event_queue = this;

        g_source_attach(reinterpret_cast<GSource*>(event_queue_source_), main_context);
    }

    return rc;
}

fscfg_ReturnCode DaemonEventQueue::ExecuteEvent(Event event)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    // iterate over all listeners and check which one is registered as being able to
    // execute events from the event's class.
    for (const ListenerEntry& listener_entry : listeners_)
    {
        if (listener_entry.second == event.event_class)
        {
            fscfg_ReturnCode listener_rc = listener_entry.first->ExecuteEvent(event);

            if (listener_rc != fscfg_kRcSuccess)
            {
                // We will just log. No matter what, we would still wish to continue processing the event
                // for other listeners as well.
                DLT_LOG_STRING(dlt_fscfgd,
                               DLT_LOG_WARN,
                               "DaemonEventQueue::ExecuteEvent: Listener failed to execute event");
            }
        }
    }

    return rc;
}

fscfg_ReturnCode DaemonEventQueue::PostEvent(Event event)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    local_sync_.lock();
    event_queue_.push(event);

    uint64_t lock_count = 1;

    write(event_descriptor_, &lock_count, sizeof(lock_count));

    local_sync_.unlock();

    return rc;
}

fscfg_ReturnCode DaemonEventQueue::IsEmpty(bool& is_empty)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    local_sync_.lock();
    is_empty = event_queue_.empty();
    local_sync_.unlock();

    return rc;
}

fscfg_ReturnCode DaemonEventQueue::RegisterListener(EventListenerInterface* event_listener, EventClass event_class)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!event_listener || event_class >= EventClass::kMax || event_class == EventClass::kUndefined)
    {
        // null event listener or event class is out of bounds.
        rc = fscfg_kRcBadParam;
    }
    else
    {
        listeners_.push_back(ListenerEntry(event_listener, event_class));
    }

    return rc;
}

fscfg_ReturnCode DaemonEventQueue::UnregisterListener(EventListenerInterface* event_listener)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!event_listener)
    {
        rc = fscfg_kRcBadParam;
    }
    else
    {
        std::vector<ListenerEntry>::iterator it = std::find_if(listeners_.begin(), listeners_.end(),
                                                    [event_listener](const ListenerEntry& listener_entry)
                                                    {
                                                        return event_listener == listener_entry.first;
                                                    });

        if (it == listeners_.end())
        {
            // There's no such listener. (it was never registered).
            rc = fscfg_kRcNotFound;
        }
        else
        {
            listeners_.erase(it);
        }
    }

    return rc;
}

gboolean  DaemonEventQueue::EventQueueGSourceIsReady(GSource* source)
{
    gboolean source_ready = FALSE;

    // If the event queue is not empty, we say that the event queue is ready to be polled.
    // Because there are actual events in the queue. (Duh!).
    DaemonEventQueue* daemon_event_queue = reinterpret_cast<EventQueueGSource*>(source)->daemon_event_queue;
    bool event_queue_empty;

    daemon_event_queue->IsEmpty(event_queue_empty);

    // source is ready if event queue is not empty.
    source_ready = !event_queue_empty;
    return source_ready;
}

gboolean DaemonEventQueue::EventQueueGSourcePrepare(GSource* source, gint* timeout_)
{
    gboolean source_ready = FALSE;

    if (!source|| !timeout_)
    {
        DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_ERROR, "Daemon::EventQGSourcePrepare: bad params");
    }
    else
    {
        *timeout_ = -1;
        source_ready = EventQueueGSourceIsReady(source);
    }

    return source_ready;
}

gboolean DaemonEventQueue::EventQueueGSourceCheck(GSource* source)
{
    gboolean source_ready = FALSE;

    if (!source)
    {
        DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_ERROR, "Daemon::EventQueueGSourceCheck: bad params");
    }
    else
    {
        source_ready = EventQueueGSourceIsReady(source);
    }

    return source_ready;
}

gboolean DaemonEventQueue::EventQueueGSourceDispatch(GSource* source, GSourceFunc callback, gpointer user_data)
{
    if (!source)
    {
        DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_ERROR, "Daemon::EventQueueGSourceDispatch: bad source");
    }
    else
    {
        EventQueueGSource* event_queue_source = reinterpret_cast<EventQueueGSource*>(source);
        DaemonEventQueue* daemon_event_queue = event_queue_source->daemon_event_queue;
        bool event_queue_empty;

        fscfg_ReturnCode rc = daemon_event_queue->IsEmpty(event_queue_empty);

        if (rc != fscfg_kRcSuccess)
        {
            DLT_LOG_STRING(dlt_fscfgd,
                           DLT_LOG_ERROR,
                           "Daemon::EventQueueGSourceDispatch:DaemonEventQueue::IsEmpty failed");
        }
        else if (!event_queue_empty)
        {
            // there are actual events in the queue.
            daemon_event_queue->local_sync_.lock();
            Event top_event = daemon_event_queue->event_queue_.front();
            daemon_event_queue->event_queue_.pop();
            daemon_event_queue->local_sync_.unlock();

            daemon_event_queue->ExecuteEvent(top_event);

            // if there are no more events, lock our event descriptor.
            daemon_event_queue->local_sync_.lock();

            if (daemon_event_queue->event_queue_.empty())
            {
                // lock the eventfd. Reading causes lock count to become zero. We are waiting on G_IO_IN, and by reading,
                // this becomes G_IO_OUT, basically.
                uint64_t lock_count;
                read(daemon_event_queue->event_descriptor_, &lock_count, sizeof(lock_count));
            }

            daemon_event_queue->local_sync_.unlock();
        }
    }

    return G_SOURCE_CONTINUE;
}


} // namespace fsm

/** \}    end of addtogroup */
