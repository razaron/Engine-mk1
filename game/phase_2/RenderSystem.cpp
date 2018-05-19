#include "RenderSystem.hpp"

using namespace razaron::game::systems;
using namespace razaron::game::components;

RenderSystem::RenderSystem(sol::state_view lua, sf::RenderWindow *window)
	:_lua{ lua }, _window{ window }
{
	_interval = 0.01;

	_componentTypes.insert(ComponentType::SHAPE);

	_lua.new_usertype<ShapeComponent>("ShapeComponent",
		sol::constructors<ShapeComponent()>(),
		"sides", &ShapeComponent::sides,
		"colour", &ShapeComponent::colour
		);

	registerHandler(EVENTTYPE_MODEL, [&](const Event &e) {
		auto data = std::static_pointer_cast<EVENTDATA_MODEL>(e.data);

		_models[e.recipient] = data->model;
		});
}

RenderSystem::~RenderSystem()
{
}

Task RenderSystem::update(EntityMap &entities, double)
{
	_window->clear();

	std::vector<std::pair<glm::mat4, ShapeComponent>> data;
	for (auto &[id, entity] : entities)
	{
		if (!entity.has(ComponentType::SHAPE)) continue;
		if (_models.find(id) == _models.end()) continue;

		auto model = _models[id];
		auto shape = *getObject<ShapeComponent>(entity[ComponentType::SHAPE]);

		data.push_back(std::make_pair(model, shape));
	}

	std::vector<sf::Vertex> va;

	for (auto &[model, shape] : data)
	{
		float theta = .0f;
		float delta = 2 * 3.14159 / shape.sides;
		for (int i = 0; i < shape.sides; i++)
		{
			float x = std::cos(theta);
			float y = std::sin(theta);

			glm::vec4 v1{ std::cos(theta), std::sin(theta), 0.f, 1.f };
			glm::vec4 v2{ std::cos(theta + delta), std::sin(theta + delta), 0.f, 1.f };
			glm::vec4 v3{ 0.f,0.f,0.f,1.f };

			v1 = model * v1;
			v2 = model * v2;
			v3 = model * v3;

			va.emplace_back(sf::Vertex(sf::Vector2f{ v1.x, v1.y }, sf::Color(shape.colour.r, shape.colour.g, shape.colour.b)));
			va.emplace_back(sf::Vertex(sf::Vector2f{ v2.x, v2.y }, sf::Color(shape.colour.r, shape.colour.g, shape.colour.b)));
			va.emplace_back(sf::Vertex(sf::Vector2f{ v3.x, v3.y }, sf::Color(shape.colour.r, shape.colour.g, shape.colour.b)));

			theta += delta;
		}
	}

	if (va.size())
		_window->draw(&va[0], va.size(), sf::Triangles);

	_window->display();

	// Cleanup
	_models.clear();

	return Task{};
}

ComponentHandle RenderSystem::createComponent(ComponentType type, std::shared_ptr<void> tuplePtr)
{
	Handle h;

	switch (type)
	{
		case ComponentType::SHAPE:
		{
			ShapeArgs args = *(std::static_pointer_cast<ShapeArgs>(tuplePtr));

			h = emplaceObject<ShapeComponent>(std::get<1>(args), std::get<2>(args));

			sol::table obj = std::get<0>(args);
			obj["shape"] = getObject<ShapeComponent>(h);

			break;
		}
		default:
		{
			h = Handle{};
			break;
		}
	}

	return ComponentHandle{ type, h };
}

bool RenderSystem::removeComponent(ComponentHandle ch)
{
	Handle h;

	switch (ch.first)
	{
		case ComponentType::SHAPE:
			removeObject<ShapeComponent>(ch.second);
			break;
		default:
			return false;
			break;
	}

	return true;
}
