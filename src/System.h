#pragma once

#include "ObjectPool.h"
#include "EventStream.h"
#include "Entity.h"
#include "TaskScheduler.h"

#include <vector>
#include <set>

/*! Systems manage the memory and events for related components, as well as run logic on their data. */
namespace razaron::core::system
{
	using namespace razaron::eventstream;
	using namespace razaron::objectpool;
	using namespace razaron::taskscheduler;
	using namespace razaron::graph;
	using namespace razaron::core::entity;

	struct TaskGraphData;

	using TaskGraph = Graph<Task, char, TaskGraphData>;
	using TaskGraphVertex = Vertex<Task, char>;
	using TaskGraphEdge = Edge<char>;

	/*! Denotes the type of a derived System. */
	enum class SystemType { CONSOLE, RENDER, PHYSICS, AI, UI, CONTROLLER, GAMEPLAY, ENUM_SIZE };

	/*! The data to be held by the TaskGraph. */
	struct TaskGraphData
	{

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
		virtual TaskGraph* update(std::vector<Entity>* p_entities, double delta) = 0;

		/*! Constructs a Component into System managed memory.
		*
		*	@tparam		C		The class of the Component to construct.
		*	@tparam		Args	The parameter pack for the constructor arguments of C.<sup>[1]</sup>
		*
		*	@param		p_args	The constructor arguments for C.
		*
		*	@returns	Returns the TaskGraph needed to run update logic for the Component objects.
		*	<small><sup>[1]</sup> Don't enter this. It <a title="cppreference" href="http://en.cppreference.com/w/cpp/language/template_argument_deduction">deduced</a> by the compiler.</small>
		*/
		template <class C, typename... Args> Handle constructComponent(Args... p_args) { return m_pool.emplace<C>(p_args...); }

		/*! Constructs a Component into System managed memory.
		*
		*	@tparam	C		The class of the Component to construct.
		*	@tparam	Args	The parameter pack for the constructor arguments of C.<sup>[1]</sup>
		*
		*	@param	p_args	The constructor arguments for C.
		*
		*	@retval	C*		On success, a pointer to the desired Component.
		*	@retval	nullptr	On failure, a nullptr.
		*/
		template<class C> C* getComponent(Handle p_handle) { return m_pool.getObject<C>(p_handle); }

		/*! Deletes the desired Component from System managed memory. */
		void removeComponent(Handle p_handle) { m_pool.removeObject(p_handle); };

		/*! Moves queued up Event objects to the dst System. */
		void bubbleEvents(System* dst);

		/*! Copies queued up Event objects from the src System. */
		void captureEvents(System* src);

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

	inline void System::bubbleEvents(System* dst)
	{
		m_eventStream.bubbleEvents(&dst->m_eventStream);
	}

	inline void System::captureEvents(System* src)
	{
		m_eventStream.captureEvents(&src->m_eventStream);
	}

	inline std::vector<Event> System::popEvents(StreamType p_type)
	{
		return m_eventStream.popEvents(p_type);
	}
}



