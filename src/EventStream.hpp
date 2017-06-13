#pragma once

#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <queue>
#include <vector>
#include <mutex>

/*! %Event streams are used to queue up events to be consumed at a later time. */
namespace razaron::eventstream
{
    struct Event;

    using EventHandler = std::function<void(Event &event)>;

    /*! Denotes the direction of an EventStream. */
    enum class StreamType
    {
        INCOMING, /*!< will stay in this EventStream. */
        OUTGOING  /*!< will leave this EventStream. */
    };

    /*! The EventType is used to determine how to process an Event. */
    enum class EventType : unsigned short
    {
        DEFAULT,          /*!< Default value. */
        CREATE_ENTITY,    /*!< Lets a Space know to create an Entity. */
        CREATE_COMPONENT, /*!< Lets a System know to create a Component. */
        ADD_COMPONENT,    /*!< Lets a Space know it can add a Component to an Entity. */
        REMOVE_COMPONENT  /*!< Tells a System to remove a Component or tells a Space a Component has been removed. */
    };

    /*! Contains the data required to receive and process an Event. */
    struct Event
    {
        unsigned int recipient;     /*!< The unique id of the recipient. */
        EventType type;             /*!< The EventType. */
        std::shared_ptr<void> data; /*!< A pointer to the data being sent. */

        /*! Evaluates deep equality between two Event structs. */
        bool operator==(const Event &rhs)
        {
            return (recipient == rhs.recipient) && (type == rhs.type) && (data == rhs.data);
        }

        /*! Evaluates deep inequality between two Event structs. */
        bool operator!=(const Event &rhs)
        {
            return !(*this == rhs);
        }
    };

    /*! Handles receiving and sending Event%s to and from EventStream%s. */
    class EventStream
    {
      public:
        EventStream();  /*!< Default constructor. */
        ~EventStream(); /*!< Default destructor. */

        /*! Pushes an Event onto this EventStream.
        *
        *   @remark Thread-safe, blocking. Allows for 1 thread to access incoming Event%s and a 2nd thread to
        *   access incoming events simultaneasly.
        */
        void pushEvent(Event p_event, StreamType p_streamType);

        /*! Pushes a vector of Events onto this EventStream.
        *
        *   @remark Thread-safe, blocking. Allows for 1 thread to access incoming Event%s and a 2nd thread to
        *   access incoming events simultaneasly.
        */
        void pushEvents(const std::vector<Event> &p_events, StreamType p_streamType);

        /*! Pops an Event from this EventStream.
        *
        *   @remark Thread-safe, blocking. Allows for 1 thread to access incoming Event%s and a 2nd thread to
        *   access incoming events simultaneasly.
        */
        Event popEvent(StreamType p_streamType);

        /*! Pops a std::vector of Event%s from this EventStream.
        *
        *   @remark Thread-safe, blocking. Allows for 1 thread to access incoming Event%s and a 2nd thread to
        *   access incoming events simultaneasly.
        */
        std::vector<Event> popEvents(StreamType p_streamType);

        /*!< Register an EventHandler to the passed EventType.
        *
        *   @remark Thread-safe, blocking. Allows for 1 thread to registed an EventHandler at a time.
        */
        void registerHandler(EventType p_type, EventHandler p_handler);

        /*!< Processes all incoming Event%s with their respective handlers.
        *
        *   @remark Thread-safe, blocking. Allows for 1 thread to process events at a time.
        */
        void processEvents();

        /*!< Moves all of this EventStream%s outgoing Event%s to another EventStream.
        *
        *   @remark Thread-safe, blocking. Internally makes use of EventStream::popEvents and
        *   EventStream::pushEvents, so comes with the same caveats.
        */
        void propogateEvents(EventStream &p_dst);

      private:
        std::queue<Event> m_incomingEvents;
        std::queue<Event> m_outgoingEvents;
        std::map<EventType, EventHandler> m_eventHandlers;

        std::mutex m_incomingEventsMutex;
        std::mutex m_outgoingEventsMutex;
        std::mutex m_eventHandlersMutex;
    };
}
