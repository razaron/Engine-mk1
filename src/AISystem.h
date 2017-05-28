#pragma once

#include "System.h"

namespace razaron::ai
{
	using namespace razaron::core::system;
	using namespace razaron::core::component;
	using namespace razaron::core::entity;
	using namespace razaron::graph;

	class AISystem :public System
	{
	public:
		AISystem() {}
		~AISystem() {}

		Graph<Task, char, SystemGraphData>* update(std::vector<Entity>* p_entities, double delta) { std::cout << sysName << std::endl; return{}; }
		Handle addComponent(Component* p_component) { return{}; }
		void removeComponent(Handle p_handle) {}

	private:
		char* sysName{ "AI System" };
	};
}