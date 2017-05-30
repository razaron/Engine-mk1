#include "EventStream.h"

using namespace razaron::eventstream;

EventStream::EventStream()
{
	std::clog << "EventStream Constructed" << std::endl;
}

EventStream::~EventStream()
{
	std::clog << "EventStream Destroyed" << std::endl;
}

void EventStream::pushEvent(Event p_event, StreamType p_streamType)
{
	switch (p_streamType) 
	{
	case StreamType::INCOMING:
	{
		m_incomingEvents.push_back(p_event);
		break;
	}
	case StreamType::OUTGOING:
	{
		m_outgoingEvents.push_back(p_event);
		break;
	}
	}
}

void EventStream::pushEvents(std::vector<Event>& p_events, StreamType p_streamType)
{
	switch (p_streamType)
	{
	case StreamType::INCOMING:
	{
		m_incomingEvents.reserve(m_incomingEvents.size() + p_events.size());
		m_incomingEvents.insert(m_incomingEvents.end(), p_events.begin(), p_events.end());

		break;
	}
	case StreamType::OUTGOING:
	{
		m_outgoingEvents.reserve(m_outgoingEvents.size() + p_events.size());
		m_outgoingEvents.insert(m_outgoingEvents.end(), p_events.begin(), p_events.end());
		
		break;
	}
	}
}

Event EventStream::popEvent(StreamType p_streamType)
{
	switch (p_streamType)
	{
	case StreamType::INCOMING:
	{
		if (m_incomingEvents.size() == 0)
			return Event{};
		else
		{
			Event e = m_incomingEvents.back();
			m_incomingEvents.pop_back();

			return e;
		}
	}
	case StreamType::OUTGOING:
	{
		if (m_outgoingEvents.size() == 0)
			return Event{};
		else
		{
			Event e = m_outgoingEvents.back();
			m_outgoingEvents.pop_back();

			return e;
		}
	}
	default:
		return Event{};
	}
}

std::vector<Event> EventStream::popEvents(StreamType p_streamType)
{
	switch (p_streamType)
	{
	case StreamType::INCOMING:
	{
		std::vector<Event> e = {};
		e.swap(m_incomingEvents);

		return e;
	}
	case StreamType::OUTGOING:
	{
		std::vector<Event> e = {};
		e.swap(m_outgoingEvents);

		return e;
	}
	default:
		return std::vector<Event>{};
	}
}

void EventStream::bubbleEvents(EventStream * dst)
{
	std::vector<Event> events = popEvents(StreamType::OUTGOING);

	dst->pushEvents(events, StreamType::INCOMING);
	dst->pushEvents(events, StreamType::OUTGOING);
}

void EventStream::captureEvents(EventStream * src)
{
	std::vector<Event> events = src->m_outgoingEvents;

	pushEvents(events, StreamType::INCOMING);
}