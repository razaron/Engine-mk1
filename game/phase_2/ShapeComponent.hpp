#pragma once

#include <glm/glm.hpp>

#include "Component.hpp"

namespace rz::game::components
{
	using namespace rz::core::component;

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
