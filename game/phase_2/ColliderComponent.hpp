#pragma once

#include "Component.hpp"

#include <glm/glm.hpp>

namespace rz::game::components
{
	using namespace rz::core::component;

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
