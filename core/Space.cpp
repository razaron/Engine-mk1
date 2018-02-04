#include "Space.hpp"

using namespace razaron::core::space;
using namespace razaron::core::entity;

Space::Space(const SystemGraph &systemGraph)
	: _id{}, _systemGraph{ systemGraph }, _intervalMax{}, _entities{}, _eventStream{}, _deletingEntities{}
{
	// Finds the highest interval for given Systems
	_systemGraph.vertexFuncs[State::WHITE] = [&](SystemGraphVertex &v, SystemGraph &g) {
		UNUSED(g);

		if (_intervalMax < v.data->getInterval())
			_intervalMax = v.data->getInterval();
	};

	_systemGraph.breadthFirstTraversal(0);
	_systemGraph.reset();

	// Starts the chain of events that will create a new Entity from a list of components
	registerHandler(EventType::CREATE_ENTITY, [&](const Event & e) {
		auto data = std::static_pointer_cast<eventdata::CREATE_ENTITY>(e.data);

		auto entity = createEntity();

		std::vector<Event> events;

		for (auto args : data->components)
		{
			events.push_back(Event{
				entity.getID(),
				EventType::CREATE_COMPONENT,
				std::make_shared<eventdata::CREATE_COMPONENT>(args)
				});
		}

		pushEvents(events, StreamType::OUTGOING);
	});

	// Adds a created Component to the correct entity
	registerHandler(EventType::CREATE_COMPONENT, [&](const Event & e) {
		auto data = std::static_pointer_cast<eventdata::CREATE_COMPONENT>(e.data);

		if (data->isCreated)
		{
			(*this)[e.recipient].addComponent(ComponentHandle{ data->type, data->handle });
		}
	});

	// Starts the chain of events that will create a new Entity from a list of components
	registerHandler(EventType::REMOVE_ENTITY, [&](const Event & e) {
		auto data = std::static_pointer_cast<eventdata::REMOVE_ENTITY>(e.data);

		// If this is the initial request
		if (data->initial)
		{
			// IF Entity is not being tracked for deletion, track it. ELSE return
			if (std::find(_deletingEntities.begin(), _deletingEntities.end(), e.recipient) == _deletingEntities.end())
				_deletingEntities.push_back(e.recipient);
			else
				return;

			// Creates a REMOVE_COMPONENT Event for each Component in the Entity
			auto entity = (*this)[e.recipient];

			std::vector<Event> events;
			for (auto&[type, handle] : entity.getComponents())
			{
				Event event {
					e.recipient,
						EventType::REMOVE_COMPONENT,
						std::make_shared<eventdata::REMOVE_COMPONENT>(ComponentHandle{ type, handle })
				};

				events.push_back(event);
			}

			pushEvents(events, StreamType::OUTGOING);

			// Create a followup REMOVE_ENTITY event to check up on progress
			std::vector<Event> event { {
					e.recipient,
						EventType::REMOVE_ENTITY,
						std::make_shared<eventdata::REMOVE_ENTITY>(false)
				}};

			pushEvents(event, StreamType::INCOMING);
		}
		// If this is a followup request
		else
		{
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
				std::vector<Event> event { {
						e.recipient,
							EventType::REMOVE_ENTITY,
							std::make_shared<eventdata::REMOVE_ENTITY>(false)
					}};

				pushEvents(event, StreamType::INCOMING);
			}
		}
	});

	// On succesful Component deletion, removes it's Handle from relevant Entity
	registerHandler(EventType::REMOVE_COMPONENT, [&](const Event & e) {
		auto data = std::static_pointer_cast<eventdata::REMOVE_COMPONENT>(e.data);

		if (data->isRemoved)
		{
			(*this)[e.recipient].removeComponent(data->ch);
		}
	});

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

		_eventStream.processEvents();

		updateSystems(elapsed);

		propagateEvents();

		remaining -= _intervalMax;
	}
}

void Space::propagateEvents()
{
	auto events = _eventStream.popEvents(StreamType::OUTGOING);

	// Delete duplicate outgoing Events
	auto last = std::unique(events.begin(), events.end());
	events.erase(last, events.end());
	_eventStream.pushEvents(events, StreamType::OUTGOING);

	_systemGraph[0].data->pushEvents(events);

	// Reset SystemGraph and propogate Events down the graph
	_systemGraph.reset();
	_systemGraph.edgeFuncs[State::WHITE] = [&](SystemGraphEdge & e, SystemGraph & g)
	{
		g[e.source].data->propogateEvents(*g[e.target].data);

		// If e.target is at the bottom of the graph, propogate it's events to the Space
		if (!g[e.target].adjacencyList.size())
		{
			g[e.target].data->propogateEvents(_eventStream);

			// TODO Whitelist repeat events. For now I'm just deleting everything.
			std::vector<Event> events = _eventStream.popEvents(StreamType::OUTGOING);
			std::vector<Event> liveEvents;

			for (auto &event : events)
			{
				if (event.lifetime)
				{
					event.lifetime--;
					liveEvents.push_back(event);
				}
			}

			_eventStream.pushEvents(liveEvents, StreamType::OUTGOING);
		}
	};
	_systemGraph.breadthFirstTraversal(0);
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
	_systemGraph.vertexFuncs[State::WHITE] = [&entities, delta](SystemGraphVertex & v, SystemGraph & g)
	{
		UNUSED(g);

		v.data->processEvents();
		v.data->update(entities, delta);
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
