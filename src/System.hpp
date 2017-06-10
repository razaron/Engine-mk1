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

        /*! Constructs a object into System managed memory.
		*
		*	@tparam		C		The class of the object to construct.
		*	@tparam		Args	The parameter pack for the constructor arguments of C.<sup>[1]</sup>
		*
		*	@param		p_args	The constructor arguments for C.
		*
		*	@returns	Returns the TaskGraph needed to run update logic for the object objects.
		*	<small><sup>[1]</sup> Don't enter this. It <a title="cppreference" href="http://en.cppreference.com/w/cpp/language/template_argument_deduction">deduced</a> by the compiler.</small>
		*/
        template <class T, typename... Args>
        Handle emplaceObject(Args... p_args) { return m_pool.emplace<T>(p_args...); }

        /*! Constructs a object into System managed memory.
		*
		*	@tparam	C		The class of the object to construct.
		*	@tparam	Args	The parameter pack for the constructor arguments of C.<sup>[1]</sup>
		*
		*	@param	p_args	The constructor arguments for C.
		*
		*	@retval	C*		On success, a pointer to the desired object.
		*	@retval	nullptr	On failure, a nullptr.
		*/
        template <class T>
        T *getobject(Handle p_handle) { return m_pool.getObject<T>(p_handle); }

        /*! Deletes the desired object from System managed memory.
		*
		*	@tparam	T	The type of the object to remove.
		*/
        template <class T>
        void removeObject(Handle p_handle) { m_pool.removeObject<T>(p_handle); };

        /*! Register an EventHandler to the passed EventType. */
        void registerHandler(EventType p_type, EventHandler p_handler);

        /*! Processes all incoming Event%s with their respective handlers. */
        void processEvents();

        /*! Moves queued up Event objects to the dst System. */
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
        System()
        {
            registerHandler(EventType::CREATE_COMPONENT, [system = this](Event & e) {
                auto data = std::static_pointer_cast<eventdata::CREATE_COMPONENT>(e.data);

                // If ComponentType is valid, do something
                if (system->m_componentTypes.count(data->type))
                {
                    auto ch = system->createComponent(data->type);

                    system->pushEvent(Event{
                        e.recipient,
                        EventType::CREATE_COMPONENT,
                        std::make_shared<eventdata::CREATE_COMPONENT>(ch, true)
                    });
                }
            });

            registerHandler(EventType::REMOVE_COMPONENT, [system = this](Event & e) {
                auto data = std::static_pointer_cast<eventdata::REMOVE_COMPONENT>(e.data);

                // If ComponentType is valid, do something
                if (system->m_componentTypes.count(data->ch.first))
                {
                    bool result = system->removeComponent(data->ch);

                    system->pushEvent(Event{
                        e.recipient,
                        EventType::REMOVE_COMPONENT,
                        std::make_shared<eventdata::REMOVE_COMPONENT>(data->ch, result)
                    });
                }
            });
        }

        ObjectPool m_pool;               /*!< The ObjectPool used to manage the memory of this System. */
        double m_interval = 0.05;        /*!< The interval (in seconds) between updates for this System. */
        EventStream m_eventStream;       /*!< The EventStream belonging to this System. */
        std::set<ComponentType> m_componentTypes; /*!< The set of ComponentType%s supported by this System. */
    };

    inline System::~System()
    {
    }

    inline void System::registerHandler(razaron::eventstream::EventType p_type, EventHandler p_handler)
    {
        m_eventStream.registerHandler(p_type, p_handler);
    }

    inline void System::processEvents()
    {
        m_eventStream.processEvents();
    }

    inline void System::propogateEvents(System &dst)
    {
        m_eventStream.propogateEvents(dst.m_eventStream);
    }

    inline void System::pushEvent(Event p_event)
    {
        m_eventStream.pushEvent(p_event, StreamType::OUTGOING);
    }

    inline void System::pushEvents(std::vector<Event> &p_events)
    {
        m_eventStream.pushEvents(p_events, StreamType::INCOMING);
        m_eventStream.pushEvents(p_events, StreamType::OUTGOING);
    }

    inline std::vector<Event> System::popEvents()
    {
        return m_eventStream.popEvents(StreamType::INCOMING);
    }
}
