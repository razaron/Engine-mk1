#pragma once

#include "System.h"
#include "TransformComponent.h"

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

		Graph<Task, char, SystemGraphData>* update(std::vector<Entity>* p_entities, double delta);

	private:
		char* sysName{ "Physics System" };
	};
}