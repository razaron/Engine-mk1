#pragma once

#include <vector>

#include "Misc.h"

/*! %Event streams are used to queue up events to be consumed at a later time. */
namespace razaron::eventstream
{
	/*! Denotes the direction of an EventStream. */
	enum class StreamType { 
		INCOMING,	/*!< will stay in this EventStream. */ 
		OUTGOING	/*!< will leave this EventStream. */ 
	};

	/*! The EventType is used to determine how to process an Event. */
	enum class EventType : unsigned short {
		TYPE_1,	/*!< example, TODO REMOVE THIS SHIT. */
		TYPE_2	/*!< example, TODO REMOVE THIS SHIT. */
	};
	
	/*! Contains the data required to receive and process an Event. */
	struct Event {
		unsigned int recipient;	/*!< The unique id of the recipient. */
		EventType type;			/*!< The EventType. */
		void* data;				/*!< A pointer to the data being sent. */

		/*! Evaluates deep equality between two Event structs. */
		bool operator==(const Event& rhs)
		{
			return (recipient == rhs.recipient) && (type == rhs.type) && (data == rhs.data);
		}

		/*! Evaluates deep inequality between two Event structs. */
		bool operator!=(const Event& rhs)
		{
			return !(*this==rhs);
		}
	};

	/*! Handles receiving and sending Event%s to and from EventStream%s. */
	class EventStream{
	public:
		EventStream();	/*!< Default constructor. */
		~EventStream(); /*!< Default destructor. */

		void pushEvent(Event p_event, StreamType p_streamType);					/*!< Pushes an Event onto this EventStream. */
		void pushEvents(std::vector<Event>& p_events, StreamType p_streamType);	/*!< Pushes a vector of Events onto this EventStream. */
		Event popEvent(StreamType p_streamType);								/*!< Pops an Event from this EventStream. */
		std::vector<Event> popEvents(StreamType p_streamType);					/*!< Pops a std::vector of Event%s from this EventStream. */

		void bubbleEvents(EventStream* p_dst);									/*!< Moves all of this EventStream%s outgoing Event%s to another EventStream. */
		void captureEvents(EventStream* p_src);									/*!< Copies all of another EventStream%s outgoing Event%s into this EventStream. */

	private:
		std::vector<Event> m_incomingEvents;
		std::vector<Event> m_outgoingEvents;
	};
}

