#include "EventStream.hpp"

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
        m_incomingEvents.push(p_event);
        break;
    }
    case StreamType::OUTGOING:
    {
        m_outgoingEvents.push(p_event);
        break;
    }
    }
}

void EventStream::pushEvents(std::vector<Event> &p_events, StreamType p_streamType)
{
    switch (p_streamType)
    {
    case StreamType::INCOMING:
    {
        for (auto e : p_events)
        {
            m_incomingEvents.push(e);
        }

        break;
    }
    case StreamType::OUTGOING:
    {
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

void EventStream::propogateEvents(EventStream &dst)
{
    std::vector<Event> events = popEvents(StreamType::OUTGOING);

    dst.pushEvents(events, StreamType::INCOMING);
}
