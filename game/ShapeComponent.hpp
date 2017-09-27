#pragma once

#include <glm/glm.hpp>

#include "Component.hpp"

namespace razaron::render
{
    using namespace razaron::core::component;
    using ShapeArgs = std::tuple<glm::vec4>;

    class ShapeComponent : public Component
    {
      public:
        ShapeComponent();
        ShapeComponent(glm::vec4 colour);
        ~ShapeComponent();

        glm::vec4 getColour() { return _colour; }

      private:
        glm::vec4 _colour;
    };
}
