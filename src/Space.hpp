#pragma once

#include <algorithm>

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

    /*! The Space class handles processing of System updates.
    *   For more information and examples, see page \ref space.
    */
    class Space
    {
      public:
        Space(SystemGraph &systemGraph); /*!< Constructs a Space from the passed SystemGraph. */
        ~Space();                          /*!< Default destructor. */

        /*! Updates the related System%s with respect to `delta`.
		*
		*	If `delta` is greater than the greatest interval of the System objects in the
		*	SystemGraph then the systems are updated with the value of the greatest interval, decreasing
		*	the value of `delta` eachtime until `delta` is smaller than the greatest inteval.
		*/
        void update(double delta);

        //TODO System* addSystem(System* system, System* root); /*!< Adds a System to the Space. */

        /*!	Creates a new empty Entity.
		*
		*	@returns    A reference to the created Entity.
		*/
        Entity &createEntity();

        /*!	Copies an object of type T into the ObjectPool.
		*
		*	@returns    The number of Component%s remaining in the Entity.
		*/
        std::size_t removeEntity(unsigned int id); /*!< Deletes an Entity from the Space. */

        //TODO Entity* moveEntity(); /*!< Moves an Entity into another Space. */
        //TODO std::vector<Entity>* getEntities() { return &_entities; } /*!<  */

        /*! Calls `registerHandler` on the member EventStream with the given arguments. */
        void registerHandler(EventType type, EventHandler handler);

        /*! Calls `pushEvents` on the member EventStream with the given arguments. */
        void pushEvents(std::vector<Event> &events, StreamType streamType);

        /*!	Gets the Entity mapped to the passed ID.
		*
		*	@param      id                    The ID of the Entity to search for.
		*
        *   @exception  std::invalid_argument   Throws if the ID maps to no Entity.
        *
		*	@returns    A reference to the mapped Entity.
		*/
        Entity &operator[](unsigned short id);

        EntityMap &getEntities() { return _entities; };

      private:
        void updateSystems(double delta);
        void propagateEvents();

        unsigned _id;
        SystemGraph _systemGraph;
        double _intervalMax{};
        EntityMap _entities;
        EventStream _eventStream;
        std::vector<unsigned> _deletingEntities;
    };
}
