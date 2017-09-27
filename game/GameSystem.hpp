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

    class GameSystem : public System
    {
      public:
        GameSystem();

        ~GameSystem();

        Task update(EntityMap &entities, double delta);
        ComponentHandle createComponent(ComponentType type, std::shared_ptr<void> tuplePtr);
        bool removeComponent(ComponentHandle ch);

        void initGame();

      private:
        std::map<unsigned short, glm::mat4> _models;
    };
}
