#include "MotionComponent.hpp"

using namespace razaron::game::components;


MotionComponent::MotionComponent()
	: velocity{}, acceleration{}, maxVelocity{}, maxAcceleration{}, mass{}, wanderAngle{}
{
}

razaron::game::components::MotionComponent::MotionComponent(glm::vec2 velocity, glm::vec2 acceleration, float maxVelocity, float maxAcceleration, float mass)
	: velocity{ velocity }, acceleration{ acceleration }, maxVelocity{ maxVelocity }, maxAcceleration{ maxAcceleration }, mass{ mass }, wanderAngle{}
{
}

MotionComponent::~MotionComponent()
{
}