#pragma once

#include "Misc.hpp"

#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <vector>

/*! %Event streams are used to queue up events to be consumed at a later time. */
namespace razaron::eventstream
{
	struct Event;

	using EventHandler = std::function<void(const Event &event)>;

	/*! Denotes the direction of an EventStream. */
	enum class StreamType
	{
		INCOMING, /*!< will stay in this EventStream. */
		OUTGOING  /*!< will leave this EventStream. */
	};

	/*! The EventType is used to determine how to process an Event. 
		Events have the format "SCOPE_ACTION".
	*/
	enum class EventType
	{
		DEFAULT,			/*!< Default value. */
		SPACE_NEW_ENTITY,			/*!< Request new Entity from Space. */
		SPACE_DELETE_ENTITY,		/*!< Request deletion of Entity from Space. */
		SPACE_REMOVE_ENTITY,		/*!< Deregister empty Entity from Space. */
		SYSTEM_NEW_COMPONENT,		/*!< Request new Component from System. */
		SYSTEM_DELETE_COMPONENT,	/*!< Request deletion of Component from System. */
		ENTITY_ADD_COMPONENT,		/*!< Register newly created Component with Entity. */
		ENTITY_REMOVE_COMPONENT,	/*!< Deregister deleted Component from Entity. */
		EVENT_1,
		EVENT_2,
		EVENT_3,
		EVENT_4,
		ENUM_SIZE
	};

	/*! Contains the data required to receive and process an Event. */
	struct Event
	{
		UUID64 recipient;         /*!< The unique id of the recipient. */
		EventType type;             /*!< The EventType. */
		std::shared_ptr<void> data; /*!< A pointer to the data being sent. */
		unsigned lifetime;			/*!< How long the Event will live for. */
		unsigned id{ uid++ };       /*!< This Events unique id. */

		Event(UUID64 recipient = UUID64{ 0 }, EventType type = EventType{}, std::shared_ptr<void> data = nullptr, unsigned lifetime = 0) noexcept
			: recipient{ recipient }, type{ type }, data{ data }, lifetime{ lifetime }
		{
		}

			  /*! Evaluates equality between two Event structs. */
		bool operator==(const Event &rhs) noexcept
		{
			return (recipient == rhs.recipient) && (type == rhs.type) && (data == rhs.data);
		}

		/*! Evaluates inequality between two Event structs. */
		bool operator!=(const Event &rhs) noexcept
		{
			return !(*this == rhs);
		}

	private:
		static unsigned uid;
	};

	/*! Handles receiving and sending Event%s to and from EventStream%s. */
	class EventStream
	{
	public:
		EventStream() noexcept;  /*!< Default constructor. */

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
		*   @remark Thread-safe, blocking. Allows for 1 thread to register an EventHandler at a time.
		*/
		void registerHandler(EventType type, EventHandler handler);

		/*!< Extend the EventHandler of the passed EventType by calling
		*
		*   @remark Thread-safe, blocking. Allows for 1 thread to register an EventHandler at a time.
		*/
		void extendHandler(EventType type, EventHandler extensionHandler);

		/*!< Processes all incoming Event%s with their respective handlers.
		*
		*   @remark Thread-safe, blocking. Allows for 1 thread to process events at a time.
		*/
		void processEvents();

	private:
		std::vector<Event> _incomingEvents;
		std::vector<Event> _outgoingEvents;
		std::map<EventType, EventHandler> _eventHandlers;

		std::mutex _incomingEventsMutex;
		std::mutex _outgoingEventsMutex;
		std::mutex _eventHandlersMutex;
	};
}
