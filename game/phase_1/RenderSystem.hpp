#pragma once

#include "LuaHooks.hpp"
#include "System.hpp"

#include <SFML/Graphics.hpp>

namespace razaron::game::systems
{
	using namespace razaron::core::system;
	using namespace razaron::core::component;
	using namespace razaron::core::entity;
	using namespace razaron::graph;

	class RenderSystem : public System
	{
	public:
		RenderSystem(sol::state_view lua, sf::RenderWindow *window);

		~RenderSystem();

		Task update(EntityMap &entities, double delta);
		ComponentHandle createComponent(ComponentType type, std::shared_ptr<void> tuplePtr);
		bool removeComponent(ComponentHandle ch);

	private:
		sol::state_view _lua;
		sf::RenderWindow *_window;
	};
}