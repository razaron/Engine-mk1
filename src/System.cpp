#include "System.hpp"

using namespace razaron::core::system;

System::~System()
{
}

void System::registerHandler(razaron::eventstream::EventType p_type, EventHandler p_handler)
{
    m_eventStream.registerHandler(p_type, p_handler);
}

void System::processEvents()
{
    m_eventStream.processEvents();
}

void System::propogateEvents(System &dst)
{
    m_eventStream.propogateEvents(dst.m_eventStream);
}

void System::pushEvent(Event p_event)
{
    m_eventStream.pushEvent(p_event, StreamType::OUTGOING);
}

void System::pushEvents(std::vector<Event> &p_events)
{
    m_eventStream.pushEvents(p_events, StreamType::INCOMING);
    m_eventStream.pushEvents(p_events, StreamType::OUTGOING);
}

std::vector<Event> System::popEvents()
{
    return m_eventStream.popEvents(StreamType::INCOMING);
}
