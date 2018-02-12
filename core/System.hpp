#pragma once

#include <set>
#include <vector>
#include <utility>

#include "Entity.hpp"
#include "EventStream.hpp"
#include "EventData.hpp"
#include "ObjectPool.hpp"
#include "TaskScheduler.hpp"

/*! Systems manage the memory, events and logic for related components.
*   For more information and examples, see page \ref core.
*/
namespace razaron::core::system
{
    using namespace razaron::eventstream;
    using namespace razaron::objectpool;
    using namespace razaron::taskscheduler;
    using namespace razaron::graph;
    using namespace razaron::core::entity;
    using namespace razaron::core::eventdata;

    /*! The abstract base class for a System. */
    class System
    {
      public:
        /*! Generates a TaskGraph used to update all relevant Component objects.
		*
		*	@param		entities	A vector of all Entity objects to access relevant Component objects from.
		*	@param		delta		The time
		*
		*	@returns	Returns the TaskGraph needed to run update logic for the Component objects.
		*/
        virtual Task update(EntityMap &entities, double delta) = 0;

        /*! Creates a new Component in the ObjectPool. */
        virtual ComponentHandle createComponent(ComponentType type, std::shared_ptr<void> tuplePtr) = 0;

        /*! Removes a Component from the ObjectPool. */
        virtual bool removeComponent(ComponentHandle ch) = 0;

        /*! Calls `emplace` on the member ObjectPool with the given arguments. */
        template <class T, typename... Args>
        Handle emplaceObject(Args... args) { return _pool.emplace<T>(args...); }

        /*! Calls `get` on the member ObjectPool with the given arguments. */
        template <class T>
        T *getObject(Handle handle) { return _pool.get<T>(handle); }

        /*! Calls `erase` on the member ObjectPool with the given arguments. */
        template <class T>
        void removeObject(Handle handle) { _pool.erase<T>(handle); };

        /*! Calls `registerHandler` on the member EventStream with the given arguments. */
        void registerHandler(EventType type, EventHandler handler);

        /*! Calls `processEvents` on the member EventStream. */
        void processEvents();

        /*! Calls `propogateEvents` on the member EventStream with the given argument. */
        void propogateEvents(System &dst);
        void propogateEvents(EventStream &stream);

        /*! Pushes an Event onto the outgoing stream. */
        void pushEvent(Event event);

        /*! Pushes Events onto the incoming stream. */
        void pushEvents(const std::vector<Event> &event);

        /*! Pops all Event objects from the incoming stream. */
        std::vector<Event> popEvents();

        /*! Gets the set interval (in ms) between updates for this System. */
        double getInterval() noexcept { return _interval; }

        /*! Gets the current TaskGraph of this System. */
        double getTaskGraph() noexcept { return static_cast<double>(_interval) / 1000; }

      protected:
        System() noexcept;

		UUID64 _id;									/*!< The unique id of this system. */
        ObjectPool _pool;							/*!< The ObjectPool used to manage the memory of this System. */
        double _interval;							/*!< The max interval (in seconds) between updates for this System. */
        EventStream _eventStream;					/*!< The EventStream belonging to this System. */
        std::set<ComponentType> _componentTypes;	/*!< The set of ComponentType%s supported by this System. */
    };
}
