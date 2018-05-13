#pragma once

#include <glm/glm.hpp>

#include "Component.hpp"

namespace razaron::game::components
{
	using namespace razaron::core::component;

	class ShapeComponent : public Component
	{
	public:
		ShapeComponent();
		ShapeComponent(int sides, glm::u8vec3 colour);
		~ShapeComponent();

		int sides;
		glm::u8vec3 colour;
	};
}
