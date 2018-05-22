#pragma once

#include "Component.hpp"

#include <glm/glm.hpp>

namespace razaron::game::components
{
	using namespace razaron::core::component;

	class MotionComponent : public Component
	{
	public:
		MotionComponent();
		MotionComponent(glm::vec2 velocity, glm::vec2 acceleration, float maxVelocity, float maxAcceleration, float mass);
		~MotionComponent();

		glm::vec2 velocity, acceleration;
		float maxVelocity, maxAcceleration, mass, wanderAngle;
	};
}