#include "MotionComponent.hpp"

using namespace razaron::physics;

MotionComponent::MotionComponent()
    : _maxVelocity{ 1.f }, _maxAcceleration{ 0.1f }, _mass{ 1.f }
{
}

MotionComponent::MotionComponent(float maxVelocity, float maxAcceleration, float mass)
    : _maxVelocity{ maxVelocity }, _maxAcceleration{ maxAcceleration }, _mass{ mass }
{
}

MotionComponent::~MotionComponent()
{
}

glm::vec2 MotionComponent::applyForce(glm::vec2 force)
{
    auto acc = (force / _mass);

    if (glm::length(acc) >= _maxAcceleration)
    {
        acc = acc / (glm::length(acc) / _maxAcceleration);
    }

    return _acceleration = acc;
}

glm::vec2 MotionComponent::updateVelocity(float delta)
{
    auto vel = _velocity + _acceleration * delta;

    if (glm::length(vel) >= _maxVelocity)
    {
        vel = vel / (glm::length(vel) / _maxVelocity);
    }

    return _velocity = vel;
}
