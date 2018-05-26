#pragma once

#include "Component.hpp"

#include <glm/glm.hpp>

namespace razaron::game::components
{
	using namespace razaron::core::component;

	class ColliderComponent : public Component
	{
	public:
		ColliderComponent(float radius, int group) :radius{ radius }, group{ group } {}
		ColliderComponent() {}
		~ColliderComponent() {}

		float radius;
		int group;
	};
}
