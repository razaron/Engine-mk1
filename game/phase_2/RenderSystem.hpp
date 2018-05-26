#pragma once

#include "System.hpp"
#include "ShapeComponent.hpp"
#include "config.hpp"

#include <SFML/Graphics.hpp>
#include <glm/gtc/matrix_transform.hpp>
#pragma warning(push)
#pragma warning(disable: 4996)
#include <sol.hpp>
#pragma warning(pop)

namespace razaron::game::systems
{
	using namespace razaron::core::system;
	using namespace razaron::game::components;

	using ShapeArgs = std::tuple<sol::object, int, glm::u8vec3>;

	struct Camera
	{
		glm::vec2 pos;
		float zoom;

		glm::mat4 getViewMatrix()
		{
			return glm::translate(glm::mat4{}, glm::vec3{});
		}
	};

	class RenderSystem : public System
	{
	public:
		RenderSystem(sol::state_view lua, sf::RenderWindow *window);
		~RenderSystem();

		Task update(EntityMap &entities, double delta);
		ComponentHandle createComponent(ComponentType type, std::shared_ptr<void> tuplePtr);
		bool removeComponent(ComponentHandle ch);

		void render();

	private:
		sol::state_view _lua;
		sf::RenderWindow *_window;

		std::vector<std::pair<glm::mat4, ShapeComponent>> _data;
		std::map<UUID64, glm::mat4, UUID64Cmp> _models;
	};
}