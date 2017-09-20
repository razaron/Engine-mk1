#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "Component.hpp"

namespace razaron::physics
{
    using namespace razaron::core::component;

    class TransformComponent : public Component
    {
      public:
        TransformComponent();
        ~TransformComponent();

        glm::mat4 getModel();

        glm::vec2 translation;
        glm::vec2 scale;
        float getRotation() { return m_rotation; }
        float setRotation(float p_rotation);

      private:
        float m_rotation; // in radians
    };
}
