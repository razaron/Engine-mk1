#pragma once

#include <vector>
#include <set>

#include "ObjectPool.hpp"
#include "EventStream.hpp"
#include "Entity.hpp"
#include "TaskScheduler.hpp"

/*! Systems manage the memory, events and logic for related components. */
namespace razaron::core::system
{
	using namespace razaron::eventstream;
	using namespace razaron::objectpool;
	using namespace razaron::taskscheduler;
	using namespace razaron::graph;
	using namespace razaron::core::entity;

	/*! Denotes the type of a derived System. */
	enum class SystemType { CONSOLE, RENDER, PHYSICS, AI, UI, CONTROLLER, GAMEPLAY, ENUM_SIZE };

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
		virtual TaskGraph& update(std::vector<Entity>* p_entities, double delta) = 0;

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
		Handle emplaceobject(Args... p_args) { return m_pool.emplace<T>(p_args...); }

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
		T* getobject(Handle p_handle) { return m_pool.getObject<T>(p_handle); }

		/*! Deletes the desired object from System managed memory.
		*
		*	@tparam	T	The type of the object to remove.
		*/
		template <class T>
		void removeObject(Handle p_handle) { m_pool.removeObject<T>(p_handle); };

		/*! Moves queued up Event objects to the dst System. */
		void propogateEvents(System &dst);

		/*! Pops all Event objects from the desired StreamType of this System. */
		std::vector<Event> popEvents(StreamType p_type);

		/*! Gets the set interval (in ms) between updates for this System. */
		double getInterval() { return static_cast<double>(m_interval) / 1000; }

		/*! Gets the current TaskGraph of this System. */
		double getTaskGraph() { return static_cast<double>(m_interval) / 1000; }

	protected:
		ObjectPool m_pool; /*!< The ObjectPool used to manage the memory of this System. */
		unsigned short m_interval = 50; /*!< The interval (in ms) between updates for this System. */
		EventStream m_eventStream; /*!< The EventStream belonging to this System. */
		std::set<ComponentType> m_types; /*!< The set of ComponentType%s supported by this System. */
		TaskGraph m_taskGraph; /*!< The last generated TaskGraph. */
	};

	inline System::~System()
	{

	}

	inline void System::propogateEvents(System &dst)
	{
		m_eventStream.propogateEvents(dst.m_eventStream);
	}

	inline std::vector<Event> System::popEvents(StreamType p_type)
	{
		return m_eventStream.popEvents(p_type);
	}
}
