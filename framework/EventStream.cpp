#include "EventStream.hpp"

using namespace razaron::eventstream;

unsigned Event::uid{ 0 };

EventStream::EventStream() noexcept
	:_incomingEvents{}, _outgoingEvents{}, _eventHandlers{}, _incomingEventsMutex{}, _outgoingEventsMutex{}, _eventHandlersMutex{}
{
}

void EventStream::pushEvent(Event event, StreamType streamType)
{
	switch (streamType)
	{
	case StreamType::INCOMING:
	{
		std::lock_guard<std::mutex> lk{ _incomingEventsMutex };

		_incomingEvents.push_back(event);
		break;
	}
	case StreamType::OUTGOING:
	{
		std::lock_guard<std::mutex> lk{ _outgoingEventsMutex };

		_outgoingEvents.push_back(event);
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

		_incomingEvents.insert(_incomingEvents.end(), events.begin(), events.end());

		break;
	}
	case StreamType::OUTGOING:
	{
		std::lock_guard<std::mutex> lk{ _outgoingEventsMutex };

		_outgoingEvents.insert(_outgoingEvents.end(), events.begin(), events.end());

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

		if (!_incomingEvents.size())
			return Event{};
		else
		{
			Event e = _incomingEvents.front();
			_incomingEvents.erase(_incomingEvents.begin());

			return e;
		}
	}
	case StreamType::OUTGOING:
	{
		std::lock_guard<std::mutex> lk{ _outgoingEventsMutex };

		if (!_outgoingEvents.size())
			return Event{};
		else
		{
			Event e = _outgoingEvents.front();
			_outgoingEvents.erase(_incomingEvents.begin());

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

		events.swap(_incomingEvents);

		return events;
	}
	case StreamType::OUTGOING:
	{
		std::lock_guard<std::mutex> lk{ _outgoingEventsMutex };

		std::vector<Event> events = {};

		events.swap(_outgoingEvents);

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
	events.erase(std::unique(events.begin(), events.end()), events.end());

	for (auto &[type, func] : _eventHandlers)
	{
		for (auto &e : events)
		{
			if (e.type == type)
				func(e);
		}
	}
	/*
	for (auto &e : events)
	{
		auto it = _eventHandlers.find(e.type);

		if (it != _eventHandlers.end())
			it->second(e);
	}
	*/
}