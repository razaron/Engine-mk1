#pragma once

#include "Graph.hpp"
#include "System.hpp"

/*! A space is an abstract partition for a group of entities and their related systems. */
namespace razaron::core::space
{
    using namespace razaron::graph;
    using namespace razaron::core::system;
    using namespace razaron::core::component;
    using namespace razaron::core::entity;

    struct SystemGraphData;

    using SystemGraph = Graph<System *, char, SystemGraphData>;
    using SystemGraphVertex = Vertex<System *, char>;
    using SystemGraphEdge = Edge<char>;

    /*! The data to be held by the SystemGraph. */
    struct SystemGraphData
    {
        std::vector<std::pair<System *, System *>> orderedSystems; /*!< A std::vector containing ordered std::pairs of src/dst System pointers. */
    };

    /*! The Space class handles processing of System updates. */
    class Space
    {
      public:
        Space(SystemGraph &p_systemGraph); /*!< Constructs a Space from the passed SystemGraph. */
        ~Space();                          /*!< Default destructor. */

        /*! Updates the related System%s with respect to <tt>delta</tt>.
		*
		*	If <tt>delta</tt> is greater than the greatest interval of the System objects in the
		*	SystemGraph then the systems are updated with the value of the greatest interval, decreasing
		*	the value of <tt>delta</tt> eachtime until <tt>delta</tt> is smaller than the greatest inteval.
		*/
        void update(double delta);

        //TODO System* addSystem(System* p_system, System* p_root); /*!< Adds a System to the Space. */

        Entity &createEntity(); /*! Constructs an a new empty Entity. */
        std::size_t removeEntity(unsigned int p_id); /*!< Deletes an Entity from the Space. */
        //TODO Entity* moveEntity(); /*!< Moves an Entity into another Space. */
        //TODO std::vector<Entity>* getEntities() { return &m_entities; } /*!<  */

        void registerHandler(EventType p_type, EventHandler p_handler);
        void pushEvents(std::vector<Event> &p_events, StreamType p_streamType);

        Entity &operator[](unsigned short p_id){ if(m_entities.count(p_id)) return m_entities[p_id]; }

        EntityMap m_entities;
      private:
		void updateSystems(double delta);
		void propagateEvents();

		SystemGraph m_systemGraph;
        double m_intervalMax{};
        EventStream m_eventStream;
    };
}
