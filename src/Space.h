#pragma once

#include "System.h"
#include "Graph.h"

namespace razaron::core::space
{
	using namespace razaron::graph;
	using namespace razaron::core::system;
	using namespace razaron::core::component;
	using namespace razaron::core::entity;

	struct SystemGraphData;

	typedef Graph<System*, char, SystemGraphData> SystemGraph;
	typedef Vertex<System*, char> SystemGraphVertex;
	typedef Edge<char> SystemGraphEdge;

	struct SystemGraphData
	{
		std::vector<std::pair<System*, System*>> orderedSystems;
	};

	class Space {
	public:
		Space(SystemGraph p_systemGraph);
		~Space();

		void update(double delta);
		System* addSystem(System* p_system);
	
		Entity* createEntity(std::vector<ComponentHandle> p_handles);
		void addEntity(Entity&& p_entity);
		Entity* popEntity();
		Entity* moveEntity();
		//std::vector<Entity>* getEntities() { return &m_entities; }

	private:
		unsigned m_id;

		SystemGraph m_systemGraph;

		std::vector<Entity> m_entities;
		EventStream m_eventStream;
	};
}