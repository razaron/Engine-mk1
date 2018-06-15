#pragma once

#include "System.hpp"
#include "LuaHooks.hpp"
#include "config.hpp"

namespace rz::game::systems
{
	using namespace rz::core::system;

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
		std::vector<std::pair<UUID64, EVENTDATA_COLLISION>> _collisions;
	};
}