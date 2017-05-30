#pragma once

#include "ObjectPool.h"
#include "EventStream.h"
#include "Entity.h"
#include "TaskScheduler.h"

#include <vector>

/*! Things related to */
namespace razaron::core::system
{
	using namespace razaron::eventstream;
	using namespace razaron::objectpool;
	using namespace razaron::taskscheduler;
	using namespace razaron::graph;
	using namespace razaron::core::entity;

	enum class SystemType {CONSOLE, RENDER, PHYSICS, AI, UI, CONTROLLER, GAMEPLAY, ENUM_SIZE};

	struct TaskGraphData
	{

	};

	class System
	{
	public:
		virtual ~System() = 0;
	
		virtual Graph<Task, char, TaskGraphData>* update(std::vector<Entity>* p_entities, double delta) = 0;
		template <class C, typename... Args> Handle constructComponent(Args... p_args) { return m_pool.emplace<C>(p_args...); }
		template<class T> T* getComponent(Handle p_handle) { return m_pool.getObject<T>(p_handle); }
		void removeComponent(Handle p_handle) { m_pool.removeObject(p_handle); };
		void bubbleEvents(System* dst);
		void captureEvents(System* src);
		std::vector<Event> popEvents(StreamType p_type);
		double getInterval() { return static_cast<double>(m_interval) / 1000; }
		double getTaskGraph() { return static_cast<double>(m_interval) / 1000; }

	protected:
		ObjectPool m_pool;
		unsigned short m_interval = 50; //Loop interval in ms
		EventStream m_eventStream;
		std::vector<ComponentType> m_types;
		Graph<Task, char, TaskGraphData> m_taskGraph;
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



