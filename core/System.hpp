#ifndef RZ_CORE_SYSTEM_HPP
#define RZ_CORE_SYSTEM_HPP

#include "Entity.hpp"
#include "EventData.hpp"
#include "EventStream.hpp"
#include "ObjectPool.hpp"
#include "TaskScheduler.hpp"

#include <set>
#include <utility>
#include <vector>

/*! Systems manage the memory, events and logic for related components.
*   For more information and examples, see page \ref core.
*/
namespace rz::core
{
    /*! The abstract base class for a System. */
    class System
    {
      public:
        /*! Generates a TaskGraph used to update all relevant Component objects.
        *
        *	  @param		entities	A vector of all Entity objects to access relevant Component objects from.
        *	  @param		delta		The time
        * 
        *	  @return		Returns the TaskGraph needed to run update logic for the Component objects.
        */
        virtual rz::taskscheduler::Task update(EntityMap &entities, double delta) = 0;

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
        void registerHandler(rz::eventstream::EventType type, rz::eventstream::EventHandler handler);

        /*! Calls `extendHandler` on the member EventStream with the given arguments. */
        void extendHandler(rz::eventstream::EventType type, rz::eventstream::EventHandler handler);

        /*! Calls `processEvents` on the member EventStream. */
        void processEvents();

        /*! Calls `pushEvents` on the member EventStream. */
        void pushEvents(const std::vector<rz::eventstream::Event> &events, rz::eventstream::StreamType streamType);

        /*! Calls `popEvents` on the member EventStream. */
        std::vector<rz::eventstream::Event> popEvents(rz::eventstream::StreamType streamType);

        /*! Gets the set interval (in ms) between updates for this System. */
        double getInterval() noexcept { return _interval; }

        /*! Gets the current TaskGraph of this System. */
        double getTaskGraph() noexcept { return static_cast<double>(_interval) / 1000; }

        /*! Sets the TaskScheduler used by this System. */
        void setTaskScheduler(std::shared_ptr<rz::taskscheduler::TaskScheduler> taskScheduler) { _taskScheduler = taskScheduler; }

        /*! Gets a reference to the TaskScheduler used by this System. */
        rz::taskscheduler::TaskScheduler &getTaskScheduler() { return *_taskScheduler; }

        /*! Sets the parent Task of this System. */
        void setParentTask(rz::taskscheduler::Task parentTask) { _parentTask = parentTask; }

        /*! Gets the parent Task of this System. */
        rz::taskscheduler::Task &getParentTask() { return _parentTask; }

      protected:
        System() noexcept;

        UUID64 _id;                                /*!< The unique id of this system. */
        rz::objectpool::ObjectPool _pool;          /*!< The ObjectPool used to manage the memory of this System. */
        double _interval;                          /*!< The max interval (in seconds) between updates for this System. */
        rz::eventstream::EventStream _eventStream; /*!< The EventStream belonging to this System. */
        std::set<ComponentType> _componentTypes;   /*!< The set of ComponentType%s supported by this System. */

        std::shared_ptr<rz::taskscheduler::TaskScheduler> _taskScheduler;
        rz::taskscheduler::Task _parentTask;

        EntityMap _entities;
    };
} // namespace rz::core

#endif //RZ_CORE_SYSTEM_HPP
