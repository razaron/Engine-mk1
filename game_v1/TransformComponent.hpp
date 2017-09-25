#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "Component.hpp"

namespace razaron::physics
{
    using namespace razaron::core::component;

    using TransformArgs = std::tuple<glm::vec2, glm::vec2, float>;

    class TransformComponent : public Component
    {
      public:
        TransformComponent();
        TransformComponent(glm::vec2 translation, glm::vec2 scale, float rotation);
        ~TransformComponent();

        glm::mat4 getModel();

        glm::vec2 translation;
        glm::vec2 scale;
        float getRotation() { return _rotation; }
        float setRotation(float rotation);

      private:
        float _rotation; // in radians
    };
}
