#include "System.hpp"

using namespace rz::core;
using namespace rz::eventstream;

System::System() noexcept
    : _id{}, _pool{}, _interval{ 1.0 }, _eventStream{}, _componentTypes{}
{
    registerHandler(EventType::SYSTEM_NEW_COMPONENT, [&](const Event &e) {
        auto data = std::static_pointer_cast<SYSTEM_NEW_COMPONENT>(e.data);

        // If ComponentType is valid, do something
        if (_componentTypes.count(data->type))
        {
            auto ch = createComponent(data->type, data->argsPtr);

            auto ptr = std::make_shared<ENTITY_ADD_COMPONENT>(ch);

            Event out{
                e.recipient,
                EventType::ENTITY_ADD_COMPONENT,
                ptr
            };

            _eventStream.pushEvent(out, StreamType::OUTGOING);
        }
    });

    registerHandler(EventType::SYSTEM_DELETE_COMPONENT, [&](const Event &e) {
        auto data = std::static_pointer_cast<SYSTEM_DELETE_COMPONENT>(e.data);

        // If ComponentType is valid, do something
        if (_componentTypes.count(data->ch.first))
        {
            if (removeComponent(data->ch))
            {
                Event out{
                    e.recipient,
                    EventType::ENTITY_REMOVE_COMPONENT,
                    std::make_shared<ENTITY_REMOVE_COMPONENT>(data->ch)
                };

                _eventStream.pushEvent(out, StreamType::OUTGOING);
            }
        }
    });
}

void System::registerHandler(rz::eventstream::EventType type, EventHandler handler)
{
    _eventStream.registerHandler(type, handler);
}

void System::extendHandler(rz::eventstream::EventType type, EventHandler handler)
{
    _eventStream.extendHandler(type, handler);
}

void System::processEvents()
{
    if(_taskScheduler)
        _parentTask = _taskScheduler->push([this](){ _eventStream.processEvents(); }, _parentTask);
    else
        _eventStream.processEvents();
}

void System::pushEvents(const std::vector<Event> &events, StreamType streamType)
{
    _eventStream.pushEvents(events, streamType);
}

std::vector<Event> System::popEvents(StreamType streamType)
{
    return _eventStream.popEvents(streamType);
}