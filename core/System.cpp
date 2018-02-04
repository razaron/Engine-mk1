#include "System.hpp"

using namespace razaron::core::system;


System::System() noexcept
	:_id{}, _pool{}, _interval{0.05}, _eventStream{}, _componentTypes{}
{
    registerHandler(EventType::CREATE_COMPONENT, [system = this](const Event & e) {
        auto data = std::static_pointer_cast<eventdata::CREATE_COMPONENT>(e.data);

        // If ComponentType is valid, do something
        if (system->_componentTypes.count(data->type))
        {
            auto ch = system->createComponent(data->type, data->argsPtr);

            auto ptr = std::make_shared<eventdata::CREATE_COMPONENT>(ch, true);

            system->pushEvent(Event{
                e.recipient,
                EventType::CREATE_COMPONENT,
                ptr
            });
        }
    });

    registerHandler(EventType::REMOVE_COMPONENT, [system = this](const Event & e) {
        auto data = std::static_pointer_cast<eventdata::REMOVE_COMPONENT>(e.data);

        // If ComponentType is valid, do something
        if (system->_componentTypes.count(data->ch.first))
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

void System::registerHandler(razaron::eventstream::EventType type, EventHandler handler)
{
    _eventStream.registerHandler(type, handler);
}

void System::processEvents()
{
    _eventStream.processEvents();
}

void System::propogateEvents(System &dst)
{
    _eventStream.propogateEvents(dst._eventStream);
}

void System::propogateEvents(EventStream &stream)
{
    _eventStream.propogateEvents(stream);
}

void System::pushEvent(Event event)
{
    _eventStream.pushEvent(event, StreamType::OUTGOING);
}

void System::pushEvents(const std::vector<Event> &events)
{
    _eventStream.pushEvents(events, StreamType::INCOMING);
    _eventStream.pushEvents(events, StreamType::OUTGOING);
}

std::vector<Event> System::popEvents()
{
    return _eventStream.popEvents(StreamType::INCOMING);
}
