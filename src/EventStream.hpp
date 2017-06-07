#pragma once

#include <vector>
#include <queue>
#include <functional>
#include <map>
#include <iostream>

#include "Misc.hpp"

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
		DEFAULT,/*!< Default value. */
        TYPE_1, /*!< example, TODO REMOVE THIS SHIT. */
        TYPE_2,  /*!< example, TODO REMOVE THIS SHIT. */
        CREATE_ENTITY,  /*!< Lets a Space know to create an Entity. */
        CREATE_COMPONENT,  /*!< Lets a System know to create a Component. */
        ADD_COMPONENT  /*!< Lets a Space know it can add a Component to an Entity. */
    };

    /*! Contains the data required to receive and process an Event. */
    struct Event
    {
        unsigned int recipient; /*!< The unique id of the recipient. */
        EventType type;         /*!< The EventType. */
        void *data;             /*!< A pointer to the data being sent. */

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
        EventStream();                  /*!< Default constructor. */
        ~EventStream();                 /*!< Default destructor. */

        void pushEvent(Event p_event, StreamType p_streamType);                 /*!< Pushes an Event onto this EventStream. */
        void pushEvents(std::vector<Event> &p_events, StreamType p_streamType); /*!< Pushes a vector of Events onto this EventStream. */
        Event popEvent(StreamType p_streamType);                                /*!< Pops an Event from this EventStream. */
        std::vector<Event> popEvents(StreamType p_streamType);                  /*!< Pops a std::vector of Event%s from this EventStream. */

        void registerHandler(EventType p_type, EventHandler p_handler);         /*!< Register an EventHandler to the passed EventType. */
        void processEvents();                                                   /*!< Processes all incoming Event%s with their respective handlers. */
        void propogateEvents(EventStream &p_dst);                               /*!< Moves all of this EventStream%s outgoing Event%s to another EventStream. */

      private:
        std::queue<Event> m_incomingEvents;
        std::queue<Event> m_outgoingEvents;

		std::map<EventType, EventHandler> m_eventHandlers;
    };
}
