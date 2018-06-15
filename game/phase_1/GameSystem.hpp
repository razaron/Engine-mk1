#pragma once

#pragma once

#include "LuaHooks.hpp"
#include "System.hpp"
#include "Planner.hpp"

namespace rz::game::systems
{
	using namespace rz::core::system;
	using namespace rz::core::component;
	using namespace rz::core::entity;
	using namespace rz::graph;
	using namespace rz::planner;

	class GameSystem : public System
	{
	public:
		GameSystem(sol::state_view lua);

		~GameSystem();

		Task update(EntityMap &entities, double delta);
		ComponentHandle createComponent(ComponentType type, std::shared_ptr<void> tuplePtr);
		bool removeComponent(ComponentHandle ch);

	private:
		sol::state_view _lua;
	};
}