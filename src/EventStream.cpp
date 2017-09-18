#include "EventStream.hpp"

using namespace razaron::eventstream;

EventStream::EventStream()
{
}

EventStream::~EventStream()
{
}

void EventStream::pushEvent(Event p_event, StreamType p_streamType)
{
    switch (p_streamType)
    {
    case StreamType::INCOMING:
    {
        std::lock_guard<std::mutex> lk{m_incomingEventsMutex};

        m_incomingEvents.push(p_event);
        break;
    }
    case StreamType::OUTGOING:
    {
        std::lock_guard<std::mutex> lk{m_outgoingEventsMutex};

        m_outgoingEvents.push(p_event);
        break;
    }
    }
}

void EventStream::pushEvents(const std::vector<Event> &p_events, StreamType p_streamType)
{
    switch (p_streamType)
    {
    case StreamType::INCOMING:
    {
        std::lock_guard<std::mutex> lk{m_incomingEventsMutex};

        for (auto e : p_events)
        {
            m_incomingEvents.push(e);
        }

        break;
    }
    case StreamType::OUTGOING:
    {
        std::lock_guard<std::mutex> lk{m_outgoingEventsMutex};

        for (auto e : p_events)
        {
            m_outgoingEvents.push(e);
        }

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
        std::lock_guard<std::mutex> lk{m_incomingEventsMutex};

        if (m_incomingEvents.size() == 0)
            return Event{};
        else
        {
            Event e = m_incomingEvents.front();
            m_incomingEvents.pop();

            return e;
        }
    }
    case StreamType::OUTGOING:
    {
        std::lock_guard<std::mutex> lk{m_outgoingEventsMutex};

        if (m_outgoingEvents.size() == 0)
            return Event{};
        else
        {
            Event e = m_outgoingEvents.front();
            m_outgoingEvents.pop();

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
        std::lock_guard<std::mutex> lk{m_incomingEventsMutex};

        std::vector<Event> events = {};

        for (std::size_t i = 0, max = m_incomingEvents.size(); i < max; i++)
        {
            Event e = m_incomingEvents.front();
            m_incomingEvents.pop();

            events.push_back(e);
        }

        return events;
    }
    case StreamType::OUTGOING:
    {
        std::lock_guard<std::mutex> lk{m_outgoingEventsMutex};

        std::vector<Event> events = {};

        for (std::size_t i = 0, max = m_outgoingEvents.size(); i < max; i++)
        {
            Event e = m_outgoingEvents.front();
            m_outgoingEvents.pop();

            events.push_back(e);
        }

        return events;
    }
    default:
        return std::vector<Event>{};
    }
}

void EventStream::registerHandler(EventType p_type, EventHandler p_handler)
{
    std::lock_guard<std::mutex> lk{m_eventHandlersMutex};

    m_eventHandlers[p_type] = p_handler;
}

void EventStream::processEvents()
{
    std::lock_guard<std::mutex> lk{m_eventHandlersMutex};

    auto events = popEvents(StreamType::INCOMING);

	for(auto &e: events)
	{
        auto it = m_eventHandlers.find(e.type);

        if(it != m_eventHandlers.end())
		      it->second(e);
	}
}

void EventStream::propogateEvents(EventStream &dst)
{
    std::vector<Event> events = popEvents(StreamType::OUTGOING);

    dst.pushEvents(events, StreamType::INCOMING);
    dst.pushEvents(events, StreamType::OUTGOING);
}
