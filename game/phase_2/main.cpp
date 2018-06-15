#include "PhysicsSystem.hpp"
#include "RenderSystem.hpp"
#include "GameSystem.hpp"
#include "InputSystem.hpp"
#include "Space.hpp"
#include "LuaHooks.hpp"

using namespace rz::core::space;
using namespace rz::core;
using namespace rz::game::systems;
using namespace rz::game::components;
using namespace rz::taskscheduler;

int main()
{
	std::clog.setstate(std::ios_base::failbit);

	sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Phase 2: C++ Component-System");

	// Create a new Lue state and load libraries
	sol::state lua;
	lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::table, sol::lib::math, sol::lib::os, sol::lib::debug, sol::lib::string, sol::lib::io);
	rz::lua::maths::hook(lua);
	rz::lua::planner::hook(lua);

	std::vector<Event> events;
	rz::lua::entities::hook(lua, events);
	
	// Create a new space
	SystemGraph g;
	g.addEdge(0, 1);
	g.addEdge(1, 2);
	g.addEdge(2, 3);

	g[0].data = std::make_shared<InputSystem>(lua, &window);
	g[1].data = std::make_shared<GameSystem>(lua);
	g[2].data = std::make_shared<PhysicsSystem>(lua);
	auto r = std::make_shared<RenderSystem>(lua, &window);
	g[3].data = r;

	Space s{ g };
	s.registerHandler(EventType::SPACE_NEW_ENTITY, [&s, &lua](const Event & e) {
		auto data = std::static_pointer_cast<eventdata::SPACE_NEW_ENTITY>(e.data);

		auto entity = s.createEntity();

		// Adds uuid to lua object (e.g. Agent)
		TransformArgs args = *(std::static_pointer_cast<TransformArgs>(data->components.begin()->second));
		sol::table obj = std::get<0>(args);
		obj["uuid"] = entity.getID();

		std::vector<Event> events;
		for (auto args : data->components)
		{
			events.push_back(Event{
				entity.getID(),
				EventType::SYSTEM_NEW_COMPONENT,
				std::make_shared<eventdata::SYSTEM_NEW_COMPONENT>(args)
				});
		}

		s.pushEvents(events, StreamType::OUTGOING);
		}
	);

	double delta{};
	double elapsed{};
	double lastRender{};
	unsigned frames{};

	while (window.isOpen())
	{
		auto start = std::chrono::high_resolution_clock::now();

		s.pushEvents(events, StreamType::INCOMING);
		events.clear();

		s.update(delta);

		if (lastRender > 0.016667)
		{
			lastRender = 0;
			r->render();
		}

		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> diff = end - start;

		delta = diff.count();
		elapsed += delta;
		lastRender += delta;
		++frames;

		if (elapsed >= 10.0)
		{
			std::cout << "FPS: " << frames / 10.f << "\tFrames: " << frames << std::endl;
			elapsed = 0.0;
			frames = 0;
		}
	}

	std::cin.get();

	return 0;
}
