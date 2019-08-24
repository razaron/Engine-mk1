#ifndef RZ_CORE_SPACE_HPP
#define RZ_CORE_SPACE_HPP

#include "Graph.hpp"
#include "System.hpp"

#include <algorithm>
#include <memory>

/*! A space is an abstract partition for a group of entities and their related systems. */
namespace rz::core
{
    struct SystemGraphData;

    using SystemGraph = rz::graph::Graph<std::shared_ptr<System>, char, SystemGraphData>;
    using SystemGraphVertex = rz::graph::Vertex<std::shared_ptr<System>, char>;
    using SystemGraphEdge = rz::graph::Edge<char>;

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

        /*!	Removes an Entity from the Space.
        *
        *   @param      id      The UUID64 of the Entity to be deleted.
		*
		*	@returns    The number of Entity%s remaining in the Space.
		*/
        std::size_t removeEntity(UUID64 id);

        //TODO Entity* moveEntity(); /*!< Moves an Entity into another Space. */
        //TODO std::vector<Entity>* getEntities() { return &_entities; } /*!<  */

        /*! Calls `registerHandler` on the member EventStream with the given arguments. */
        void registerHandler(rz::eventstream::EventType type, rz::eventstream::EventHandler handler);

        /*! Calls `registerHandler` on the member EventStream with the given arguments. */
        void extendHandler(rz::eventstream::EventType type, rz::eventstream::EventHandler handler);

        /*! Calls `pushEvents` on the member EventStream. */
        void pushEvents(const std::vector<rz::eventstream::Event> &events, rz::eventstream::StreamType streamType);

        EntityMap &getEntities() noexcept { return _entities; };

        /*! Gets the unique ID of this Space.
		*
		*	@returns	The unique id of this Space.
		*/
        UUID64 getID() noexcept { return _id; }

        std::shared_ptr<rz::taskscheduler::TaskScheduler> _taskScheduler;
      
      private:
        void updateSystems(double delta);
        void publishEvents();

        UUID64 _id;
        SystemGraph _systemGraph;
        double _intervalMax;
        EntityMap _entities;
        rz::eventstream::EventStream _eventStream;
        std::vector<UUID64> _deletingEntities;  
    };
}

#endif //RZ_CORE_SPACE_HPP