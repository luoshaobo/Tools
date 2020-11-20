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
 *  \file     daemon_event_queue.h
 *  \brief    Foundation Services Daemon Event Queue interface
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#ifndef FSM_DAEMON_EVENT_QUEUE_H_INC_
#define FSM_DAEMON_EVENT_QUEUE_H_INC_

#include <sys/eventfd.h>

#include <queue>
#include <mutex>
#include <memory>

#include <fscfgd/events.h>
#include <fscfgd/event_listener_interface.h>


namespace fsm
{

/**
 * \brief GSource Object-Oriented event queue wrapper.
 */

class DaemonEventQueue
{
public:

    /**
     * \brief DaemonEventQueue constructor.
     */
    DaemonEventQueue();

    /**
     * \brief DaemonEventQueue destructor.
     */
    ~DaemonEventQueue();

    /**
     * \brief Attach the message queue to a given context of execution.
     *
     * \param[in] main_context context of execution
     *
     * \return fscfg_kRcSuccess on success,\n
     *         fscfg_kRcBadParam if main_context is null.
     */
    fscfg_ReturnCode Attach(GMainContext* main_context);

    /**
     * \brief Executes a given event on the calling thread.
     *
     * \param[in] event daemon event.
     *
     * \return fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode ExecuteEvent(Event event);

    /**
     * \brief Posts an event for execution on the daemon's thread with added possibility to wait for event execution.
     *
     * \param[in] event Daemon event.
     * \param[in] blocking true if calling thread should wait for execution, false otherwise.
     *
     * \return fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode PostEvent(Event event);

    /**
     * \brief Deduces if the event queue is empty or not.
     *
     * \param[out] is_empty true if event queue is empty, false otherwise.
     *
     * \return fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode IsEmpty(bool& is_empty);

    /**
     * \brief Register an event execution listener for a given event class.
     * When an event for the specified event class is being dispatched, the execution of the event is passed to the
     * listener.
     *
     * \param[in] event_listener event listener.
     * \param[in] event_class class of events listener is interested in.
     *
     * \return fscfg_kRcSuccess on success,\n
     *         fscfg_kRcBadParam if event_listener is null or event_class is out of bounds.
     */
    fscfg_ReturnCode RegisterListener(EventListenerInterface* event_listener, EventClass event_class);

    /**
     * \brief Unregister a previously registered event listener.
     *
     * \param[in] event_listener event listener.
     *
     * \return fscfg_kRcSuccess on success,\n
     *         fscfg_kRcBadParam if event_listener is null or event_class is out of bounds.
     */
    fscfg_ReturnCode UnregisterListener(EventListenerInterface* event_listener);

private:

    /**
     * \brief Specialized glib GSource event queue.
     */
    struct EventQueueGSource
    {
        GSource source;                             ///< Glib's GSource.
        GMainContext* context;                      ///< Glib's associated context.
        DaemonEventQueue* daemon_event_queue;       ///< Daemon's event queue.
    };

    ///! Event listener entry.
    typedef std::pair<EventListenerInterface*, EventClass> ListenerEntry;


    /**
     * \brief Wrapper function checking if the GSource has any events in the queue.
     *
     * \param[in] source specialized GSource
     *
     * \return TRUE if GSource is to be dispatched, FALSE otherwise.
     */
    static gboolean EventQueueGSourceIsReady(GSource* source);

    /**
     * \brief Specialized GSource prepare callback.
     * Determines if the GSource should be dispatched prior to poll. If there are events in our GSource representation,
     * we deduce that the GSource can be dispatched already, before poll.
     *
     * \param[in] source specialized GSource
     * \param[out] timeout_ polling timeout, if any polling should be done.
     *
     * \return TRUE if GSource is to be dispatched, FALSE otherwise.
     */
    static gboolean EventQueueGSourcePrepare(GSource* source, gint* timeout_);

    /**
     * \brief Specialized GSource check callback.
     * Checks if the GSource still has events within after poll time expired.
     *
     * \param[in] source specialized GSource
     *
     * \return TRUE if GSource is to be dispatched, FALSE otherwise.
     */
    static gboolean EventQueueGSourceCheck(GSource* source);

    /**
     * \brief Specialized GSource dispatch callback.
     *
     * \param[in] source specialized GSource.
     * \param[in] callback not used.
     * \param[in] user_data not used.
     *
     * \return G_SOURCE_CONTINUE always.
     */
    static gboolean EventQueueGSourceDispatch(GSource* source, GSourceFunc callback, gpointer user_data);

    /**
     * \brief disabled copy constructor.
     */
    DaemonEventQueue(DaemonEventQueue& other);

    /**
     * \brief disabled assignment constructor.
     */
    DaemonEventQueue& operator=(const DaemonEventQueue& other);


    std::mutex local_sync_;                         ///< Local synchronization primitive.
    EventQueueGSource* event_queue_source_;         ///< GSource for the event queue.
    GSourceFuncs event_queue_functions_;            ///< GSource functions.
    std::queue<Event> event_queue_;                 ///< Event queue.
    std::vector<ListenerEntry> listeners_;          ///< Listeners.
    int event_descriptor_;                           ///< Event descriptor.
};

} // namespace fsm

#endif // FSM_DAEMON_EVENT_QUEUE_H_INC_

/** \}    end of addtogroup */
