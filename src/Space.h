#pragma once

#include "System.h"
#include "Graph.h"

/*! A space is an abstract partition for a group of entities and their related systems. */
namespace razaron::core::space
{
	using namespace razaron::graph;
	using namespace razaron::core::system;
	using namespace razaron::core::component;
	using namespace razaron::core::entity;

	struct SystemGraphData;

	using SystemGraph = Graph<System*, char, SystemGraphData>;
	using SystemGraphVertex = Vertex<System*, char>;
	using SystemGraphEdge = Edge<char>;

	/*! The data to be held by the SystemGraph. */
	struct SystemGraphData
	{
		std::vector<std::pair<System*, System*>> orderedSystems; /*!< A std::vector containing ordered std::pairs of src/dst System pointers. */
	};

	/*! The Space class handles processing of System updates. */
	class Space {
	public:
		Space(SystemGraph p_systemGraph); /*!< Constructs a Space from the passed SystemGraph. */
		~Space(); /*!< Default destructor. */

		void update(double delta); /*!< Updates the related System%s with respect to <tt>delta</tt>. */
		//TODO System* addSystem(System* p_system, System* p_root); /*!< Adds a System to the Space. */

		//TODO Entity* createEntity(std::vector<ComponentHandle> p_handles); /*! Constructs an Entity from the passed ComponentHandle%s. */
		void addEntity(Entity&& p_entity); /*!< Moves a pre-constructed Entity into the Space. */
		//TODO Entity* popEntity(); /*!< Deletes an Entity from the Space. */
		//TODO Entity* moveEntity(); /*!< Moves an Entity into another Space. */
		//TODO std::vector<Entity>* getEntities() { return &m_entities; } /*!<  */

	private:
		unsigned m_id;

		SystemGraph m_systemGraph;

		std::vector<Entity> m_entities;
		EventStream m_eventStream;
	};
}