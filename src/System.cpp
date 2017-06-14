#include "System.hpp"

using namespace razaron::core::system;


System::System(){
    registerHandler(EventType::CREATE_COMPONENT, [system = this](Event & e) {
        auto data = std::static_pointer_cast<eventdata::CREATE_COMPONENT>(e.data);

        // If ComponentType is valid, do something
        if (system->m_componentTypes.count(data->type))
        {
            auto ch = system->createComponent(data->type);

            system->pushEvent(Event{
                e.recipient,
                EventType::CREATE_COMPONENT,
                std::make_shared<eventdata::CREATE_COMPONENT>(ch, true)
            });
        }
    });

    registerHandler(EventType::REMOVE_COMPONENT, [system = this](Event & e) {
        auto data = std::static_pointer_cast<eventdata::REMOVE_COMPONENT>(e.data);

        // If ComponentType is valid, do something
        if (system->m_componentTypes.count(data->ch.first))
        {
            bool result = system->removeComponent(data->ch);

            system->pushEvent(Event{
                e.recipient,
                EventType::REMOVE_COMPONENT,
                std::make_shared<eventdata::REMOVE_COMPONENT>(data->ch, result)
            });
        }
    });
}

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
