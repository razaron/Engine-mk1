#pragma once

#include "System.hpp"
#include "TransformComponent.hpp"

namespace razaron::physics
{
	using namespace razaron::core::system;
	using namespace razaron::core::component;
	using namespace razaron::core::entity;
	using namespace razaron::graph;

	class PhysicsSystem :public System
	{
	public:
		PhysicsSystem() {}
		~PhysicsSystem() {}

		Graph<Task, char, TaskGraphData>* update(std::vector<Entity>* p_entities, double delta);

	private:
		std::string sysName{ "Physics System" };
	};
}
