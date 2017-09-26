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
    enum class EventType
    {
        DEFAULT,          /*!< Default value. */
        CREATE_ENTITY,    /*!< Lets a Space know to create an Entity. */
        CREATE_COMPONENT, /*!< Lets a System know to create a Component. */
        REMOVE_ENTITY,    /*!< Tells Systems to remove relevant Components, then removes Entity. */
        REMOVE_COMPONENT,  /*!< Tells a System to remove a Component or tells a Space a Component has been removed. */
        EVENT_1,
        EVENT_2,
        EVENT_3,
        EVENT_4
    };

    /*! Contains the data required to receive and process an Event. */
    struct Event
    {
        unsigned recipient;         /*!< The unique id of the recipient. */
        EventType type;             /*!< The EventType. */
        std::shared_ptr<void> data; /*!< A pointer to the data being sent. */
        unsigned lifetime{0};       /*!< How long the Event will live for. */

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
        *   access outgoing events simultaneasly.
        */
        void pushEvent(Event event, StreamType streamType);

        /*! Pushes a vector of Events onto this EventStream.
        *
        *   @remark Thread-safe, blocking. Allows for 1 thread to access incoming Event%s and a 2nd thread to
        *   access outgoing events simultaneasly.
        */
        void pushEvents(const std::vector<Event> &events, StreamType streamType);

        /*! Pops an Event from this EventStream.
        *
        *   @remark Thread-safe, blocking. Allows for 1 thread to access incoming Event%s and a 2nd thread to
        *   access outgoing events simultaneasly.
        */
        Event popEvent(StreamType streamType);

        /*! Pops a std::vector of Event%s from this EventStream.
        *
        *   @remark Thread-safe, blocking. Allows for 1 thread to access incoming Event%s and a 2nd thread to
        *   access outgoing events simultaneasly.
        */
        std::vector<Event> popEvents(StreamType streamType);

        /*!< Register an EventHandler to the passed EventType.
        *
        *   @remark Thread-safe, blocking. Allows for 1 thread to registed an EventHandler at a time.
        */
        void registerHandler(EventType type, EventHandler handler);

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
        void propogateEvents(EventStream &dst);

      private:
        std::queue<Event> _incomingEvents;
        std::queue<Event> _outgoingEvents;
        std::map<EventType, EventHandler> _eventHandlers;

        std::mutex _incomingEventsMutex;
        std::mutex _outgoingEventsMutex;
        std::mutex _eventHandlersMutex;
    };
}
