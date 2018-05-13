#include "PhysicsSystem.hpp"

using namespace razaron::game::systems;
using namespace razaron::game::components;

PhysicsSystem::PhysicsSystem(sol::state_view lua)
	:_lua{ lua }
{
	_interval = 0.01;

	_componentTypes.insert(ComponentType::TRANSFORM);

	_lua.new_usertype<TransformComponent>("TransformComponent",
		sol::constructors<TransformComponent()>(),
		"translation", &TransformComponent::translation,
		"scale", &TransformComponent::scale,
		"rotation", sol::property(&TransformComponent::getRotation, &TransformComponent::setRotation)
		);
}

PhysicsSystem::~PhysicsSystem()
{
}

Task PhysicsSystem::update(EntityMap &entities, double)
{
	std::vector<Event> events;

	for (auto &[id, components] : entities)
	{
		try
		{
			auto transform = getObject<TransformComponent>(components[ComponentType::TRANSFORM]);

			events.push_back(Event{
				id,
				EVENTTYPE_MODEL,
				std::make_shared<EVENTDATA_MODEL>(transform->getModel()),
				1U}
			);
		}
		catch(const std::exception &e)
		{
		}
	}

	if (events.size())
		pushEvents(events, StreamType::OUTGOING);

	return Task{};
}

ComponentHandle PhysicsSystem::createComponent(ComponentType type, std::shared_ptr<void> tuplePtr)
{
	Handle h;

	switch (type)
	{
		case ComponentType::TRANSFORM:
		{
			TransformArgs args = *(std::static_pointer_cast<TransformArgs>(tuplePtr));

			h = emplaceObject<TransformComponent>(std::get<1>(args), std::get<2>(args), std::get<3>(args));

			sol::table obj = std::get<0>(args);
			obj["transform"] = getObject<TransformComponent>(h);

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

bool PhysicsSystem::removeComponent(ComponentHandle ch)
{
	Handle h;

	switch (ch.first)
	{
		case ComponentType::TRANSFORM:
			removeObject<TransformComponent>(ch.second);
			break;
		default:
			return false;
			break;
	}

	return true;
}
