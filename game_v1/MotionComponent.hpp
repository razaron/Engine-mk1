#pragma once

#include <glm/glm.hpp>

#include "Component.hpp"

namespace razaron::physics
{
    using namespace razaron::core::component;

    using MotionArgs = std::tuple<float, float, float>;

    class MotionComponent : public Component
    {
      public:
        MotionComponent();
        MotionComponent(float maxVelocity, float maxAcceleration, float mass);
        ~MotionComponent();

        glm::vec2 applyForce(glm::vec2 force);
        glm::vec2 updateVelocity(float delta);

        glm::vec2 getVelocity() { return _velocity; }
        float getMaxVelocity() { return _maxVelocity; }
        glm::vec2 getAcceleration() { return _acceleration; }
        float getMaxAcceleration() { return _maxAcceleration; }

      private:
        glm::vec2 _velocity, _acceleration;
        float _maxVelocity, _maxAcceleration, _mass;
    };
}
