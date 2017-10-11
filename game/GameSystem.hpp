#pragma once

#include <glm/glm.hpp>

#include "MotionComponent.hpp"
#include "System.hpp"
#include "TransformComponent.hpp"
#include "ShapeComponent.hpp"
#include "AnimalComponent.hpp"
#include "config.hpp"

namespace razaron::game
{
    using namespace razaron::core::system;
    using namespace razaron::core::component;
    using namespace razaron::core::entity;
    using namespace razaron::graph;

    using Animal = std::pair<unsigned, AnimalComponent *>;

    class GameSystem : public System
    {
      public:
        GameSystem();

        ~GameSystem();

        Task update(EntityMap &entities, double delta);
        ComponentHandle createComponent(ComponentType type, std::shared_ptr<void> tuplePtr);
        bool removeComponent(ComponentHandle ch);

        void initGame();
        Event createAnimal(glm::vec2 pos, glm::vec2 scale, AnimalDiet diet, glm::vec4 colour);

        void updateSteering(std::vector<Animal> herbivores, std::vector<Animal> carnivores);
        Animal findNearestAnimal(const Animal &me, std::vector<Animal> others, float maxDist = std::numeric_limits<float>::max());

      private:
        std::map<unsigned short, glm::mat4> _models;
    };
}
