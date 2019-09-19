#include "Space.hpp"

using namespace rz::eventstream;
using namespace rz::graph;
using namespace rz::core;
using namespace rz::taskscheduler;

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
    registerHandler(core::event::type::SPACE_NEW_ENTITY, [&](const Event &e) {
        auto data = std::static_pointer_cast<event::data::SPACE_NEW_ENTITY>(e.data);

        auto entity = createEntity();

        std::vector<Event> events;

        for (auto args : data->components)
        {
            events.push_back(Event{
                entity.getID(),
                core::event::type::SYSTEM_NEW_COMPONENT,
                std::make_shared<event::data::SYSTEM_NEW_COMPONENT>(args) });
        }

        pushEvents(events, StreamType::OUTGOING);
    });

    // Adds a created Component to the correct entity
    registerHandler(core::event::type::ENTITY_ADD_COMPONENT, [&](const Event &e) {
        auto data = std::static_pointer_cast<event::data::ENTITY_ADD_COMPONENT>(e.data);

        (*this)[e.recipient].addComponent(data->ch);
    });

    // Starts the chain of events that will delete an entity
    registerHandler(core::event::type::SPACE_DELETE_ENTITY, [&](const Event &e) {
        auto data = std::static_pointer_cast<event::data::SPACE_DELETE_ENTITY>(e.data);

        if(!_entities.count(e.recipient)) return;

        // IF Entity is not being tracked for deletion, track it. ELSE return
        if (std::find(_deletingEntities.begin(), _deletingEntities.end(), e.recipient) == _deletingEntities.end())
            _deletingEntities.push_back(e.recipient);
        else
            return;

        // Creates a SYSTEM_DELETE_COMPONENT Event for each Component in the Entity
        auto entity = (*this)[e.recipient];

        std::vector<Event> events;
        for (auto & [ type, handle ] : entity.getComponents())
        {
            events.emplace_back(
                e.recipient,
                core::event::type::SYSTEM_DELETE_COMPONENT,
                std::make_shared<event::data::SYSTEM_DELETE_COMPONENT>(ComponentHandle{ type, handle }));
        }

        pushEvents(events, StreamType::OUTGOING);

        // Create a followup SPACE_REMOVE_ENTITY event to check up on progress
        _eventStream.pushEvent(Event{
                                   e.recipient,
                                   core::event::type::SPACE_REMOVE_ENTITY,
                                   std::make_shared<event::data::SPACE_REMOVE_ENTITY>() },
                               StreamType::OUTGOING);
    });

    // Remove an empty Entity from _entities
    registerHandler(core::event::type::SPACE_REMOVE_ENTITY, [&](const Event &e) {
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
                                       core::event::type::SPACE_REMOVE_ENTITY,
                                       std::make_shared<event::data::SPACE_REMOVE_ENTITY>() },
                                   StreamType::OUTGOING);
        }
    });

    // On succesful Component deletion, removes it's Handle from relevant Entity
    registerHandler(core::event::type::ENTITY_REMOVE_COMPONENT, [&](const Event &e) {
        auto data = std::static_pointer_cast<event::data::ENTITY_REMOVE_COMPONENT>(e.data);

        (*this)[e.recipient].removeComponent(data->ch);
    });
}

Entity &Space::operator[](UUID64 id)
{
    if (_entities.count(id))
        return _entities[id];
    else
        throw std::invalid_argument("Entity: " + std::to_string(id.uuid) + " is not in Space: " + std::to_string(_id.uuid));
}

void Space::update(double delta)
{
    double remaining = delta;

    // Updates the SystemGraph N+1 times where N is the number of times _intervalMax goes into delta
    while (remaining >= 0)
    {
        double elapsed = std::min(remaining, _intervalMax);

        updateSystems(elapsed);

        // Wait for System Tasks to finish before continuing
        if(_taskScheduler)
            _taskScheduler->helpWorkers();

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
    _systemGraph.vertexFuncs[State::WHITE] = [&events](SystemGraphVertex &v, SystemGraph &) {
        auto temp = v.data->popEvents(StreamType::OUTGOING);
        events.insert(events.end(), temp.begin(), temp.end());
    };
    _systemGraph.breadthFirstTraversal(0);

    // Delete duplicate gathered events
    auto last = std::unique(events.begin(), events.end());
    events.erase(last, events.end());

    // Publish gathered events to systems incoming streams
    _systemGraph.reset();
    _systemGraph.vertexFuncs[State::WHITE] = [&events](SystemGraphVertex &v, SystemGraph &) {
        v.data->pushEvents(events, StreamType::INCOMING);
    };
    _systemGraph.breadthFirstTraversal(0);

    // Publish gathered events to the spaces incoming stream
    _eventStream.pushEvents(events, StreamType::INCOMING);
}

void Space::updateSystems(double delta)
{
    auto entities = _entities;

    for (const auto &id : _deletingEntities)
    {
        entities.erase(id);
    }

    // Reset SystemGraph and update all Systems
    _systemGraph.reset();

    // TODO Account for Systems with multiple child Systems
    _systemGraph.edgeFuncs[State::WHITE] = [](SystemGraphEdge &e, SystemGraph &g){
        g[e.target].data->setParentTask(g[e.source].data->getParentTask());
    };

    _systemGraph.vertexFuncs[State::WHITE] = [&entities, delta](SystemGraphVertex &v, SystemGraph &) {
        double remaining = delta;

        while (remaining >= 0)
        {
            double elapsed = std::min(remaining, v.data->getInterval());

            // TODO Make entities die instantly to remove isDead hack. E.g. Have Space loop ingest events every SystemVertex
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

void Space::registerHandler(rz::eventstream::EventType type, EventHandler handler)
{
    _eventStream.registerHandler(type, handler);
}

void Space::extendHandler(rz::eventstream::EventType type, EventHandler handler)
{
    _eventStream.extendHandler(type, handler);
}

Entity &Space::createEntity()
{
    Entity e{{}};
    return _entities[e.getID()] = e;
}

std::size_t Space::removeEntity(UUID64 id)
{
    _entities.erase(id);

    return _entities.size();
}
