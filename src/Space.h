#pragma once

#include "System.h"
#include "Graph.h"

namespace razaron::core::space
{
	using namespace razaron::graph;
	using namespace razaron::core::system;
	using namespace razaron::core::component;
	using namespace razaron::core::entity;

	struct SpaceGraphData;

	typedef Graph<System, char, SpaceGraphData> SpaceGraph;
	typedef Vertex<System> SpaceGraphVertex;
	typedef Edge SpaceGraphEdge;

	struct SpaceGraphData
	{
		std::vector<std::pair<System*, System*>> orderedSystems;
	};

	class Space {
	public:
		Space(SpaceGraph p_systemGraph);
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

		SpaceGraph m_systemGraph;

		std::vector<Entity> m_entities;
		EventStream m_eventStream;
	};
}