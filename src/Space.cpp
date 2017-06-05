#include "Space.hpp"

using namespace razaron::core::space;
using namespace razaron::core::entity;

Space::Space(SystemGraph p_systemGraph)
	:m_systemGraph(std::move(p_systemGraph))
{
	std::clog << "Space Constructor" << std::endl;
}

Space::~Space()
{
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

	std::clog << "Space Destructor" << std::endl;
}

void Space::update(double delta)
{
	/*TODO
		Make big TaskGraph from results of system->update(...)
		make TaskQueue from graph
		process queue here
	*/

	m_systemGraph.reset();

	UNUSED(delta);
}

void razaron::core::space::Space::addEntity(Entity && p_entity)
{
	m_entities.push_back(p_entity);
}
