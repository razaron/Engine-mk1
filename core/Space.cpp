#include "Space.hpp"

using namespace razaron::core::space;
using namespace razaron::core::entity;

Space::Space(const SystemGraph &systemGraph)
	: _id{}, _systemGraph{ systemGraph }, _intervalMax{}, _entities{}, _eventStream{}, _deletingEntities{}
{
	// Finds the highest interval for given Systems
	_systemGraph.vertexFuncs[State::WHITE] = [&](SystemGraphVertex &v, SystemGraph &) {
		if (_intervalMax < v.data->getInterval())
			_intervalMax = v.data->getInterval();
	};

	_systemGraph.breadthFirstTraversal(0);
	_systemGraph.reset();

	// Starts the chain of events that will create a new Entity from a list of components
	registerHandler(EventType::SPACE_NEW_ENTITY, [&](const Event & e) {
		auto data = std::static_pointer_cast<eventdata::SPACE_NEW_ENTITY>(e.data);

		auto entity = createEntity();

		std::vector<Event> events;

		for (auto args : data->components)
		{
			events.push_back(Event{
				entity.getID(),
				EventType::SYSTEM_NEW_COMPONENT,
				std::make_shared<eventdata::SYSTEM_NEW_COMPONENT>(args)
				});
		}

		pushEvents(events, StreamType::OUTGOING);
		}
	);

	// Adds a created Component to the correct entity
	registerHandler(EventType::ENTITY_ADD_COMPONENT, [&](const Event & e) {
		auto data = std::static_pointer_cast<eventdata::ENTITY_ADD_COMPONENT>(e.data);

		(*this)[e.recipient].addComponent(data->ch);
		}
	);

	// Starts the chain of events that will delete an entity
	registerHandler(EventType::SPACE_DELETE_ENTITY, [&](const Event & e) {
		auto data = std::static_pointer_cast<eventdata::SPACE_DELETE_ENTITY>(e.data);

		// IF Entity is not being tracked for deletion, track it. ELSE return
		if (std::find(_deletingEntities.begin(), _deletingEntities.end(), e.recipient) == _deletingEntities.end())
			_deletingEntities.push_back(e.recipient);
		else
			return;

		// Creates a SYSTEM_DELETE_COMPONENT Event for each Component in the Entity
		auto entity = (*this)[e.recipient];

		std::vector<Event> events;
		for (auto&[type, handle] : entity.getComponents())
		{
			events.emplace_back(
				e.recipient,
				EventType::SYSTEM_DELETE_COMPONENT,
				std::make_shared<eventdata::SYSTEM_DELETE_COMPONENT>(ComponentHandle{ type, handle })
			);
		}

		pushEvents(events, StreamType::OUTGOING);

		// Create a followup SPACE_REMOVE_ENTITY event to check up on progress
		_eventStream.pushEvent(Event{
			e.recipient,
			EventType::SPACE_REMOVE_ENTITY,
			std::make_shared<eventdata::SPACE_REMOVE_ENTITY>()
			}
		, StreamType::INCOMING);
		}
	);

	// Remove an empty Entity from _entities
	registerHandler(EventType::SPACE_REMOVE_ENTITY, [&](const Event & e) {
		// If Entity is empty, delete it
		if (!(*this)[e.recipient].getComponents().size())
		{
			removeEntity(e.recipient);

			// Stop tracking the Entity
			_deletingEntities.erase(std::find(_deletingEntities.begin(), _deletingEntities.end(), e.recipient));
		}
		// Else resend the event
		else
		{
			_eventStream.pushEvent(Event{
				e.recipient,
				EventType::SPACE_REMOVE_ENTITY,
				std::make_shared<eventdata::SPACE_REMOVE_ENTITY>()
				}
			, StreamType::INCOMING);
		}
		}
	);

	// On succesful Component deletion, removes it's Handle from relevant Entity
	registerHandler(EventType::ENTITY_REMOVE_COMPONENT, [&](const Event & e) {
		auto data = std::static_pointer_cast<eventdata::ENTITY_REMOVE_COMPONENT>(e.data);

		(*this)[e.recipient].removeComponent(data->ch);	
		}
	);

}

Entity &Space::operator[](UUID64 id)
{
	if (_entities.count(id))
		return _entities[id];
	else
		throw std::invalid_argument("Entity: " + std::to_string(id.uuid.to_ullong()) + " is not in Space: " + std::to_string(_id.uuid.to_ullong()));
}

void Space::update(double delta)
{
	double remaining = delta;

	// Updates the SystemGraph N+1 times where N is the number of times _intervalMax goes into delta
	while (remaining >= 0)
	{
		double elapsed = std::min(remaining, _intervalMax);

		updateSystems(elapsed);

		_eventStream.processEvents();
		publishEvents();

		remaining -= _intervalMax;
	}
}

void Space::publishEvents()
{
	auto events = _eventStream.popEvents(StreamType::OUTGOING);

	// Gather all outgoing events
	_systemGraph.reset();
	_systemGraph.vertexFuncs[State::WHITE] = [&events](SystemGraphVertex & v, SystemGraph & g) {
		auto temp = v.data->popEvents(StreamType::OUTGOING);
		events.insert(events.end(), temp.begin(), temp.end());
	};
	_systemGraph.breadthFirstTraversal(0);

	// Delete duplicate gathered events
	auto last = std::unique(events.begin(), events.end());
	events.erase(last, events.end());

	// Publish gathered events to systems incoming streams
	_systemGraph.reset();
	_systemGraph.vertexFuncs[State::WHITE] = [&events](SystemGraphVertex & v, SystemGraph & g) {
		v.data->pushEvents(events, StreamType::INCOMING);
	};
	_systemGraph.breadthFirstTraversal(0);

	// Publish gathered events to the spaces incoming stream
	_eventStream.pushEvents(events, StreamType::INCOMING);
}

void Space::updateSystems(double delta)
{
	auto entities = _entities;

	for (const auto& id : _deletingEntities)
	{
		entities.erase(id);
	}

	// Reset SystemGraph and update all Systems
	_systemGraph.reset();
	_systemGraph.vertexFuncs[State::WHITE] = [&entities, delta](SystemGraphVertex & v, SystemGraph &) {
		double remaining = delta;

		while (remaining >= 0)
		{
			double elapsed = std::min(remaining, v.data->getInterval());

			v.data->processEvents();
			v.data->update(entities, elapsed);

			remaining -= v.data->getInterval();
			if (remaining == 0) break; // For instant frame updates (e.g. initial setup)
		}
	};
	_systemGraph.breadthFirstTraversal(0);
}

void Space::pushEvents(const std::vector<Event> &events, StreamType streamType)
{
	_eventStream.pushEvents(events, streamType);
}

void Space::registerHandler(razaron::eventstream::EventType type, EventHandler handler)
{
	_eventStream.registerHandler(type, handler);
}

void Space::extendHandler(razaron::eventstream::EventType type, EventHandler handler)
{
	_eventStream.extendHandler(type, handler);
}

Entity &Space::createEntity()
{
	Entity e{};
	return _entities[e.getID()] = e;
}

std::size_t Space::removeEntity(UUID64 id)
{
	_entities.erase(id);

	return _entities.size();
}
