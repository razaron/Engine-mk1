#include "PhysicsSystem.hpp"
#include "RenderSystem.hpp"
#include "GameSystem.hpp"
#include "InputSystem.hpp"
#include "Space.hpp"
#include "LuaHooks.hpp"

using namespace razaron::core::space;
using namespace razaron::core;
using namespace razaron::game::systems;
using namespace razaron::game::components;

int main()
{
	std::clog.setstate(std::ios_base::failbit);

	sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Phase 2: C++ Component-System");

	// Create a new Lue state and load libraries
	sol::state lua;
	lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::table, sol::lib::math, sol::lib::debug);
	razaron::lua::maths::hook(lua);
	razaron::lua::planner::hook(lua);

	// Entity creation events
	std::vector<Event> events;

	lua["deleteEntity"] = [&events, &lua](const UUID64 &id) {
		Event e{
			id, // Entity ID.
			EventType::SPACE_DELETE_ENTITY, // Event type enum
			std::make_shared<eventdata::SPACE_DELETE_ENTITY>()
		};

		events.push_back(e);
	};

	lua["newAgent"] = [&events, &lua](sol::table obj, glm::vec2 pos, int sides, glm::u8vec3 col) {
		Event e{
			UUID64{ 0 }, // Entity ID. 0 because unneeded
			EventType::SPACE_NEW_ENTITY, // Event type enum
			std::make_shared<eventdata::SPACE_NEW_ENTITY>(std::list<ComponentArgs>{
				ComponentArgs{ ComponentType::TRANSFORM, std::make_shared<TransformArgs>(obj, pos, glm::vec2{6.4f,6.4f}, 0.f) },
				ComponentArgs{ ComponentType::SHAPE, std::make_shared<ShapeArgs>(obj, sides, col) }
			})
		};

		events.push_back(e);
	};

	lua["newBase"] = [&events, &lua](sol::table obj, glm::vec2 pos, glm::u8vec3 col) {
		Event e{
			UUID64{ 0 }, // Entity ID. 0 because unneeded
			EventType::SPACE_NEW_ENTITY, // Event type enum
			std::make_shared<eventdata::SPACE_NEW_ENTITY>(std::list<ComponentArgs>{
			ComponentArgs{ ComponentType::TRANSFORM, std::make_shared<TransformArgs>(obj, pos, glm::vec2{ 25.6f,25.6f }, -3.14159 / 4) },
				ComponentArgs{ ComponentType::SHAPE, std::make_shared<ShapeArgs>(obj, 4, col) }
		})
		};

		events.push_back(e);
	};

	lua["newDeposit"] = [&events, &lua](sol::table obj, glm::vec2 pos, glm::u8vec3 col) {
		Event e{
			UUID64{ 0 }, // Entity ID. 0 because unneeded
			EventType::SPACE_NEW_ENTITY, // Event type enum
			std::make_shared<eventdata::SPACE_NEW_ENTITY>(std::list<ComponentArgs>{
			ComponentArgs{ ComponentType::TRANSFORM, std::make_shared<TransformArgs>(obj, pos, glm::vec2{ 12.8f,12.8f }, 0.f) },
				ComponentArgs{ ComponentType::SHAPE, std::make_shared<ShapeArgs>(obj, 4, col) }
		})
		};

		events.push_back(e);
	};

	lua["newBullet"] = [&events, &lua](sol::table obj, glm::vec2 pos, glm::u8vec3 col) {
		Event e{
			UUID64{ 0 }, // Entity ID. 0 because unneeded
			EventType::SPACE_NEW_ENTITY, // Event type enum
			std::make_shared<eventdata::SPACE_NEW_ENTITY>(std::list<ComponentArgs>{
			ComponentArgs{ ComponentType::TRANSFORM, std::make_shared<TransformArgs>(obj, pos, glm::vec2{ 1.6f,1.6f }, 0.f) },
				ComponentArgs{ ComponentType::SHAPE, std::make_shared<ShapeArgs>(obj, 16, col) }
		})
		};

		events.push_back(e);
	};

	// Create a new space
	SystemGraph g;
	g.addEdge(0, 1);
	g.addEdge(1, 2);
	g.addEdge(2, 3);

	g[0].data = std::make_shared<GameSystem>(lua);
	g[1].data = std::make_shared<PhysicsSystem>(lua);
	g[2].data = std::make_shared<RenderSystem>(lua, &window);
	g[3].data = std::make_shared<InputSystem>(lua, &window);

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
	unsigned frames{};
	while (window.isOpen())
	{
		auto start = std::chrono::high_resolution_clock::now();

		s.pushEvents(events, StreamType::INCOMING);
		events.clear();

		s.update(delta);

		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> diff = end - start;

		delta = diff.count();
		elapsed += delta;
		++frames;

		if (elapsed >= 1.0)
		{
			std::cout << "FPS: " << frames / 1 << std::endl;
			elapsed = 0.0;
			frames = 0;
		}
	}

	std::cin.get();

	return 0;
}
