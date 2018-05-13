#pragma once

#include "System.hpp"
#include "TransformComponent.hpp"
#include "config.hpp"

#include <sol.hpp>

namespace razaron::game::systems
{
	using namespace razaron::core::system;

	using TransformArgs = std::tuple<sol::table, glm::vec2, glm::vec2, float>;

	class PhysicsSystem : public System
	{
	public:
		PhysicsSystem(sol::state_view lua);
		~PhysicsSystem();

		Task update(EntityMap &, double);
		ComponentHandle createComponent(ComponentType type, std::shared_ptr<void>);
		bool removeComponent(ComponentHandle ch);

	private:
		sol::state_view _lua;
	};
}