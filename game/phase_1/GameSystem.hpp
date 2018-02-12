#pragma once

#pragma once

#include "LuaHooks.hpp"
#include "System.hpp"
#include "Planner.hpp"

namespace razaron::game::systems
{
	using namespace razaron::core::system;
	using namespace razaron::core::component;
	using namespace razaron::core::entity;
	using namespace razaron::graph;
	using namespace razaron::planner;

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