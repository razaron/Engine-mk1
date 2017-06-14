#pragma once

#include <set>
#include <vector>

#include "Entity.hpp"
#include "EventStream.hpp"
#include "EventData.hpp"
#include "ObjectPool.hpp"
#include "TaskScheduler.hpp"

/*! Systems manage the memory, events and logic for related components. */
namespace razaron::core::system
{
    using namespace razaron::eventstream;
    using namespace razaron::objectpool;
    using namespace razaron::taskscheduler;
    using namespace razaron::graph;
    using namespace razaron::core::entity;
    using namespace razaron::eventdata;

    /*! Denotes the type of a derived System. */
    enum class SystemType
    {
        CONSOLE,
        RENDER,
        PHYSICS,
        AI,
        UI,
        CONTROLLER,
        GAMEPLAY,
        ENUM_SIZE
    };

    /*! The abstract base class for a System. */
    class System
    {
      public:
        virtual ~System() = 0;

        /*! Generates a TaskGraph used to update all relevant Component objects.
		*
		*	@param		p_entities	A vector of all Entity objects to access relevant Component objects from.
		*	@param		delta		The time
		*
		*	@returns	Returns the TaskGraph needed to run update logic for the Component objects.
		*/
        virtual Task update(EntityMap &p_entities, double delta) = 0;

        /*! Creates a new Component in the ObjectPool. */
        virtual ComponentHandle createComponent(ComponentType p_type) = 0;

        /*! Removes a Component from the ObjectPool. */
        virtual bool removeComponent(ComponentHandle p_ch) = 0;

        /*! Calls `emplace` on the member ObjectPool with the given arguments. */
        template <class T, typename... Args>
        Handle emplaceObject(Args... p_args) { return m_pool.emplace<T>(p_args...); }

        /*! Calls `get` on the member ObjectPool with the given arguments. */
        template <class T>
        T *getObject(Handle p_handle) { return m_pool.get<T>(p_handle); }

        /*! Calls `erase` on the member ObjectPool with the given arguments. */
        template <class T>
        void removeObject(Handle p_handle) { m_pool.erase<T>(p_handle); };

        /*! Calls `registerHandler` on the member EventStream with the given arguments. */
        void registerHandler(EventType p_type, EventHandler p_handler);

        /*! Calls `processEvents` on the member EventStream. */
        void processEvents();

        /*! Calls `propogateEvents` on the member EventStream with the given argument. */
        void propogateEvents(System &dst);

        /*! Pushes an Event onto the outgoing stream. */
        void pushEvent(Event p_event);

        /*! Pushes Events onto the incoming stream. */
        void pushEvents(std::vector<Event> &p_event);

        /*! Pops all Event objects from the incoming stream. */
        std::vector<Event> popEvents();

        /*! Gets the set interval (in ms) between updates for this System. */
        double getInterval() { return m_interval; }

        /*! Gets the current TaskGraph of this System. */
        double getTaskGraph() { return static_cast<double>(m_interval) / 1000; }

      protected:
        System();

        ObjectPool m_pool;               /*!< The ObjectPool used to manage the memory of this System. */
        double m_interval = 0.05;        /*!< The interval (in seconds) between updates for this System. */
        EventStream m_eventStream;       /*!< The EventStream belonging to this System. */
        std::set<ComponentType> m_componentTypes; /*!< The set of ComponentType%s supported by this System. */
    };
}
