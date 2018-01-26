#include "EventStream.hpp"

using namespace razaron::eventstream;

unsigned Event::uid{0};

EventStream::EventStream()
{
}

EventStream::~EventStream()
{
}

void EventStream::pushEvent(Event event, StreamType streamType)
{
    switch (streamType)
    {
    case StreamType::INCOMING:
    {
        std::lock_guard<std::mutex> lk{ _incomingEventsMutex };

        _incomingEvents.push(event);
        break;
    }
    case StreamType::OUTGOING:
    {
        std::lock_guard<std::mutex> lk{ _outgoingEventsMutex };

        _outgoingEvents.push(event);
        break;
    }
    }
}

void EventStream::pushEvents(const std::vector<Event> &events, StreamType streamType)
{
    switch (streamType)
    {
    case StreamType::INCOMING:
    {
        std::lock_guard<std::mutex> lk{ _incomingEventsMutex };

        for (auto e : events)
        {
            _incomingEvents.push(e);
        }

        break;
    }
    case StreamType::OUTGOING:
    {
        std::lock_guard<std::mutex> lk{ _outgoingEventsMutex };

        for (auto e : events)
        {
            _outgoingEvents.push(e);
        }

        break;
    }
    }
}

Event EventStream::popEvent(StreamType streamType)
{
    switch (streamType)
    {
    case StreamType::INCOMING:
    {
        std::lock_guard<std::mutex> lk{ _incomingEventsMutex };

        if (_incomingEvents.size() == 0)
            return Event{};
        else
        {
            Event e = _incomingEvents.front();
            _incomingEvents.pop();

            return e;
        }
    }
    case StreamType::OUTGOING:
    {
        std::lock_guard<std::mutex> lk{ _outgoingEventsMutex };

        if (_outgoingEvents.size() == 0)
            return Event{};
        else
        {
            Event e = _outgoingEvents.front();
            _outgoingEvents.pop();

            return e;
        }
    }
    default:
        return Event{};
    }
}

std::vector<Event> EventStream::popEvents(StreamType streamType)
{
    switch (streamType)
    {
    case StreamType::INCOMING:
    {
        std::lock_guard<std::mutex> lk{ _incomingEventsMutex };

        std::vector<Event> events = {};

		const std::size_t max = _incomingEvents.size();
        for (std::size_t i = 0; i < max; i++)
        {
            Event e = _incomingEvents.front();
            _incomingEvents.pop();

            events.push_back(e);
        }

        return events;
    }
    case StreamType::OUTGOING:
    {
        std::lock_guard<std::mutex> lk{ _outgoingEventsMutex };

        std::vector<Event> events = {};

		const std::size_t max = _outgoingEvents.size();
        for (std::size_t i = 0; i < max; i++)
        {
            Event e = _outgoingEvents.front();
            _outgoingEvents.pop();

            events.push_back(e);
        }

        return events;
    }
    default:
        return std::vector<Event>{};
    }
}

void EventStream::registerHandler(EventType type, EventHandler handler)
{
    std::lock_guard<std::mutex> lk{ _eventHandlersMutex };

    _eventHandlers[type] = handler;
}

void EventStream::extendHandler(EventType type, EventHandler extensionHandler)
{
    std::lock_guard<std::mutex> lk{ _eventHandlersMutex };

    if (_eventHandlers.find(type) != _eventHandlers.end())
    {
        EventHandler originalHandler = _eventHandlers[type];
        _eventHandlers[type] = [originalHandler, extensionHandler](const Event &e) { originalHandler(e); extensionHandler(e); };
    }
    else
    {
        _eventHandlers[type] = extensionHandler;
    }
}

void EventStream::processEvents()
{
    std::lock_guard<std::mutex> lk{ _eventHandlersMutex };

    auto events = popEvents(StreamType::INCOMING);

    // Delete duplicate incoming Events
    auto lastEvent = std::unique(events.begin(), events.end());
    events.erase(lastEvent, events.end());

    for (auto &e : events)
    {
        auto it = _eventHandlers.find(e.type);

        if (it != _eventHandlers.end())
            it->second(e);
    }
}

void EventStream::propogateEvents(EventStream &dst)
{
    std::vector<Event> events = popEvents(StreamType::OUTGOING);

    dst.pushEvents(events, StreamType::INCOMING);
    dst.pushEvents(events, StreamType::OUTGOING);
}
