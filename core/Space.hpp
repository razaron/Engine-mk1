#pragma once

#include "Graph.hpp"
#include "System.hpp"

#include <algorithm>
#include <memory>

/*! A space is an abstract partition for a group of entities and their related systems. */
namespace razaron::core::space
{
    using namespace razaron::core::system;
    using namespace razaron::core::component;
    using namespace razaron::core::entity;

    struct SystemGraphData;

    using SystemGraph = razaron::graph::Graph<std::shared_ptr<System>, char, SystemGraphData>;
    using SystemGraphVertex = razaron::graph::Vertex<std::shared_ptr<System>, char>;
    using SystemGraphEdge = razaron::graph::Edge<char>;

    /*! The data to be held by the SystemGraph. */
    struct SystemGraphData
    {
        std::vector<std::pair<System *, System *>> orderedSystems; /*!< A std::vector containing ordered std::pairs of src/dst System pointers. */
    };

    /*! The Space class handles processing of System updates.
    *   For more information and examples, see page \ref core.
    */
    class Space
    {
      public:
        Space(const SystemGraph &systemGraph); /*!< Constructs a Space from the passed SystemGraph. */

        /*!	Gets the Entity mapped to the passed ID.
		*
		*	@param      id                    The ID of the Entity to search for.
		*
        *   @exception  std::invalid_argument   Throws if the ID maps to no Entity.
        *
		*	@returns    A reference to the mapped Entity.
		*/
        Entity &operator[](UUID64 id);

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
        std::size_t removeEntity(UUID64 id); /*!< Deletes an Entity from the Space. */

        //TODO Entity* moveEntity(); /*!< Moves an Entity into another Space. */
        //TODO std::vector<Entity>* getEntities() { return &_entities; } /*!<  */

        /*! Calls `registerHandler` on the member EventStream with the given arguments. */
        void registerHandler(EventType type, EventHandler handler);

		/*! Calls `registerHandler` on the member EventStream with the given arguments. */
		void extendHandler(EventType type, EventHandler handler);

        /*! Calls `pushEvents` on the member EventStream. */
        void pushEvents(const std::vector<Event> &events, StreamType streamType);

        EntityMap &getEntities() noexcept { return _entities; };

      private:
        void updateSystems(double delta);
        void publishEvents();

		UUID64 _id;
        SystemGraph _systemGraph;
        double _intervalMax;
        EntityMap _entities;
        EventStream _eventStream;
        std::vector<UUID64> _deletingEntities;
    };
}
