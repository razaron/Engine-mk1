#pragma once

#include <glm/glm.hpp>

#include "Component.hpp"

namespace razaron::game
{
    enum class AnimalDiet;

    using namespace razaron::core::component;
    using AnimalArgs = std::tuple<AnimalDiet>;

    enum class AnimalDiet
    {
        CARNIVORE,
        HERBIVORE
    };

    class AnimalComponent : public Component
    {
      public:
        AnimalComponent();
        AnimalComponent(AnimalDiet diet);
        ~AnimalComponent();

        AnimalDiet getDiet() { return _diet; }

      private:
        AnimalDiet _diet;
    };
}
