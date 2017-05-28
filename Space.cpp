#include "Space.h"

using namespace razaron::core::space;
using namespace razaron::core::entity;

Space::Space(SpaceGraph p_systemGraph)
	:m_systemGraph(std::move(p_systemGraph))
{
	std::clog << "Space Constructor" << std::endl;
	g_constructed++;
}

Space::~Space()
{
	m_systemGraph.onVertexDiscoverFunc = [](Vertex<System>& v, SpaceGraph& g) { delete v.data; };
	m_systemGraph.breadthFirstSearch(0);

	std::clog << "Space Destructor" << std::endl;
	g_destroyed++;
}

void Space::update(double delta)
{
	/*TODO
		Make big TaskGraph from results of system->update(...)
		make TaskQueue from graph
		process queue here
	*/

	m_systemGraph.reset();

	//Creates a breadth first ordered vector of {*system, parent *system} pairs
	m_systemGraph.data.orderedSystems.clear();
	m_systemGraph.onEdgeDiscoverFunc = [](SpaceGraphEdge& e, SpaceGraph& g) {
		g.data.orderedSystems.push_back({ g[e.target].data, g[e.source].data });
	};
	m_systemGraph.breadthFirstSearch(0);

	//Bubble events up the graph to the root system
	auto vec = m_systemGraph.data.orderedSystems;
	for (auto sPair : reverse(vec))
	{
		sPair.first->bubbleEvents(sPair.second);
	}

	std::vector<System*> closedList{};

	//Systems capture events from root system
	for (auto sPair : vec)
	{
		if (std::find(closedList.begin(), closedList.end(), sPair.first) != closedList.end()) continue;

		closedList.push_back(sPair.first);

		sPair.first->popEvents(StreamType::OUTGOING);
		sPair.first->captureEvents(m_systemGraph[0].data);
	}

	m_systemGraph[0].data->popEvents(StreamType::OUTGOING);

	//call system->update(...)
	m_systemGraph.reset();

	std::vector<Entity>* vE = &m_entities;
	m_systemGraph.onVertexDiscoverFunc = [vE, delta](SpaceGraphVertex& v, SpaceGraph& g) { v.data->update(vE, (delta <= 0) ? 0 : delta); };

	m_systemGraph.breadthFirstSearch(0);
}

void razaron::core::space::Space::addEntity(Entity && p_entity)
{
	m_entities.push_back(p_entity);
}
