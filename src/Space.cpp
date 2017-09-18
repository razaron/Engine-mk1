#include "Space.hpp"

using namespace razaron::core::space;
using namespace razaron::core::entity;

Space::Space(SystemGraph &p_systemGraph)
    : m_id{g_nextID++}, m_systemGraph{std::move(p_systemGraph)}
{
    // Finds the highest interval for given Systems
    m_systemGraph.vertexFuncs[State::WHITE] = [&](SystemGraphVertex &v, SystemGraph &g) {
        UNUSED(g);

        if (m_intervalMax < v.data->getInterval())
            m_intervalMax = v.data->getInterval();
    };

    m_systemGraph.breadthFirstTraversal(0);
    m_systemGraph.reset();

    // Starts the chain of events that will create a new Entity from a list of components
    registerHandler(EventType::CREATE_ENTITY, [&](Event & e) {
        auto data = std::static_pointer_cast<eventdata::CREATE_ENTITY>(e.data);

        auto entity = createEntity();

        std::vector<Event> events;

        for(auto type : data->components)
        {
            events.push_back(Event{
                entity.getID(),
                EventType::CREATE_COMPONENT,
                std::make_shared<eventdata::CREATE_COMPONENT>(type)}
            );
        }

        pushEvents(events, StreamType::OUTGOING);
    });

    // Adds a created Component to the correct entity
    registerHandler(EventType::CREATE_COMPONENT, [&](Event & e) {
        auto data = std::static_pointer_cast<eventdata::CREATE_COMPONENT>(e.data);

        if (data->isCreated)
        {
            (*this)[e.recipient].addComponent(ComponentHandle{data->type, data->handle});
        }
    });

    // Starts the chain of events that will create a new Entity from a list of components
    registerHandler(EventType::REMOVE_ENTITY, [&](Event & e) {
        auto data = std::static_pointer_cast<eventdata::REMOVE_ENTITY>(e.data);

        // If this is the initial request
        if(data->initial)
        {
            // Creates a REMOVE_COMPONENT Event for each Component in the Entity
            auto entity = (*this)[e.recipient];

            std::vector<Event> events;
            for(auto& [type, handle] : entity.getComponents())
            {
                Event event{
                    e.recipient,
                    EventType::REMOVE_COMPONENT,
                    std::make_shared<eventdata::REMOVE_COMPONENT>(ComponentHandle{type, handle})
                };

                events.push_back(event);
            }

            pushEvents(events, StreamType::OUTGOING);

            // Create a followup REMOVE_ENTITY event to check up on progress
            std::vector<Event> event{{
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
            if(!(*this)[e.recipient].getComponents().size())
            {
                removeEntity(e.recipient);
            }
            // Else resend the event
            else
            {
                std::vector<Event> event{{
                    e.recipient,
                    EventType::REMOVE_ENTITY,
                    std::make_shared<eventdata::REMOVE_ENTITY>(false)
                }};

                pushEvents(event, StreamType::INCOMING);
            }
        }
    });

    // On succesful Component deletion, removes it's Handle from relevant Entity
    registerHandler(EventType::REMOVE_COMPONENT, [&](Event & e) {
        auto data = std::static_pointer_cast<eventdata::REMOVE_COMPONENT>(e.data);

        if (data->isRemoved)
        {
            (*this)[e.recipient].removeComponent(data->ch);
        }
    });

}

Space::~Space()
{
    m_systemGraph.reset();

    m_systemGraph.vertexFuncs[State::WHITE] = [](SystemGraphVertex &v, SystemGraph &g) {
        UNUSED(g);

        delete v.data;
    };

    m_systemGraph.edgeFuncs[State::WHITE] = [](SystemGraphEdge &e, SystemGraph &g) {
        UNUSED(g);

        UNUSED(e); // delete e if necessary
    };

    UNUSED(m_systemGraph.data); // delete data if necessary

    m_systemGraph.breadthFirstTraversal(0);
}

void Space::update(double delta)
{
    double remaining = delta;

    // Updates the SystemGraph N+1 times where N is the number of times m_intervalMax goes into delta
    while (remaining >= 0)
    {
        double elapsed = std::min(remaining, m_intervalMax);

        m_eventStream.processEvents();

        updateSystems(elapsed);

        propagateEvents();

        remaining -= m_intervalMax;
    }
}

void Space::propagateEvents()
{
    auto events = m_eventStream.popEvents(StreamType::OUTGOING);

    m_systemGraph[0].data->pushEvents(events);

    // Reset SystemGraph and propogate Events down the graph
    m_systemGraph.reset();
    m_systemGraph.edgeFuncs[State::WHITE] = [space = this](SystemGraphEdge & e, SystemGraph & g)
    {
        g[e.source].data->propogateEvents(*g[e.target].data);

        // If e.target is at the bottom of the graph, propogate it's events to the Space
        if (!g[e.target].adjacencyList.size())
        {
            auto events = g[e.target].data->popEvents();

            // Push copy of events back onto e.target
            g[e.target].data->pushEvents(events);

            // Only pushing to incoming to stop Events looping around SystemGraph
            space->pushEvents(events, StreamType::INCOMING);
        }
    };
    m_systemGraph.breadthFirstTraversal(0);
}

void Space::updateSystems(double delta)
{
    // Reset SystemGraph and update all Systems
    m_systemGraph.reset();
    m_systemGraph.vertexFuncs[State::WHITE] = [ entities = &this->m_entities, delta ](SystemGraphVertex & v, SystemGraph & g)
    {
        UNUSED(g);

        v.data->processEvents();
        v.data->update(*entities, delta);
    };
    m_systemGraph.breadthFirstTraversal(0);
}

void Space::pushEvents(std::vector<Event> &p_events, StreamType p_streamType)
{
    m_eventStream.pushEvents(p_events, p_streamType);
}

void Space::registerHandler(razaron::eventstream::EventType p_type, EventHandler p_handler)
{
    m_eventStream.registerHandler(p_type, p_handler);
}

Entity &Space::createEntity()
{
    Entity e{};
    return m_entities[e.getID()] = e;
}

std::size_t Space::removeEntity(unsigned int p_id)
{
    m_entities.erase(p_id);

    return m_entities.size();
}

Entity &Space::operator[](unsigned short p_id)
{
    if (m_entities.count(p_id))
        return m_entities[p_id];
    else
        throw std::invalid_argument("Entity: " + std::to_string(p_id) + " is not in Space: " + std::to_string(m_id));
}
