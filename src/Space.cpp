#include "Space.hpp"

using namespace razaron::core::space;
using namespace razaron::core::entity;

Space::Space(SystemGraph &p_systemGraph)
	:m_systemGraph(std::move(p_systemGraph))
{
	std::clog << "Space Constructor" << std::endl;

	m_systemGraph.vertexFuncs[State::WHITE] = [this](SystemGraphVertex& v, SystemGraph& g) {
		UNUSED(g);

		if(this->m_intervalMax < v.data->getInterval())
			this->m_intervalMax = v.data->getInterval();
	};

	m_systemGraph.breadthFirstTraversal(0);
	m_systemGraph.reset();
}

Space::~Space()
{
	std::clog << "Space Destructor" << std::endl;

	m_systemGraph.reset();

	m_systemGraph.vertexFuncs[State::WHITE] = [](SystemGraphVertex& v, SystemGraph& g) {
		UNUSED(g);

		delete v.data;
	};

	m_systemGraph.edgeFuncs[State::WHITE] = [](SystemGraphEdge& e, SystemGraph& g) {
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
	while(remaining >= 0)
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
	m_systemGraph.edgeFuncs[State::WHITE] = [space = this](SystemGraphEdge& e, SystemGraph& g) {
		g[e.source].data->propogateEvents(*g[e.target].data);

		// If e.target is at the bottom of the graph, propogate it's events to the Space
		if(!g[e.target].adjacencyList.size())
		{
			auto events = g[e.target].data->popEvents();

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
	m_systemGraph.vertexFuncs[State::WHITE] = [entities = &this->m_entities, delta](SystemGraphVertex& v, SystemGraph& g) {
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
