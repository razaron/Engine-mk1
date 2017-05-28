#pragma once

#include <vector>

#include "Misc.h"

/** Contains classes, enums and structs related to event streaming.
*/
namespace razaron::eventstream
{
	/*! @brief Denotes the direction of an EventStream. */
	enum class StreamType { 
		INCOMING,	/*!< will stay in this EventStream. */ 
		OUTGOING	/*!< will leave this EventStream. */ 
	};

	/*! @brief The EventType is used to determine how to process an Event. */
	enum class EventType : unsigned short { 
		TYPE_1,	/*!< example, TODO REMOVE THIS SHIT. */
		TYPE_2	/*!< example, TODO REMOVE THIS SHIT. */
	};
	
	/*! @brief Contains the data required to receive and process an Event. */
	struct Event {
		unsigned int recipient;	/*!< The Component id of the recipient. */
		EventType type;			/*!< The EventType. */
		void* data;				/*!< The Component id of the recipient. */

		/*! @brief Evaluates deep equality between two Event structs. */
		bool operator==(const Event& rhs)
		{
			return (recipient == rhs.recipient) && (type == rhs.type) && (data == rhs.data);
		}

		/*! @brief Evaluates deep inequality between two Event structs. */
		bool operator!=(const Event& rhs)
		{
			return !(*this==rhs);
		}
	};

	/*! @brief Handles receiving and sending Event%s to and from EventStream%s. */
	class EventStream{
	public:
		EventStream();	/*!< @brief default constructor. */
		~EventStream(); /*!< @brief default destructor. */

		void pushEvent(Event p_event, StreamType p_streamType);					/*!< @brief Pushes an Event onto this EventStream. */
		void pushEvents(std::vector<Event>& p_events, StreamType p_streamType);	/*!< @brief Pushes a vector of Events onto this EventStream. */
		Event popEvent(StreamType p_streamType);								/*!< @brief Pops an Event from this EventStream. */
		std::vector<Event> popEvents(StreamType p_streamType);					/*!< @brief Pops a vector of Event%s from this EventStream. */

		void bubbleEvents(EventStream* dst);									/*!< @brief Sends all of this EventStream%s outgoing Event%s to another EventStream. */
		void captureEvents(EventStream* src);									/*!< @brief Takes all of another EventStream%s outgoing Event%s into this EventStream. */

	private:
		std::vector<Event> m_incomingEvents;
		std::vector<Event> m_outgoingEvents;
	};
}

