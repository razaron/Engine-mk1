#pragma once

#include <initializer_list>

#include <SFML/Window.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "System.hpp"
#include "config.hpp"

namespace razaron::input
{
    using namespace razaron::core::system;
    using namespace razaron::core::component;
    using namespace razaron::core::entity;
    using namespace razaron::graph;

    class InputSystem : public System
    {
      public:
        InputSystem(sf::Window* window);

        ~InputSystem();

        Task update(EntityMap &entities, double delta);
        ComponentHandle createComponent(ComponentType type, std::shared_ptr<void> tuplePtr);
        bool removeComponent(ComponentHandle ch);

        Context createContext(ContextType type, std::set<InputType> inputTypes);
        std::list<Input> mapInputs(double delta);

      private:
        std::list<Context> _contexts;
        sf::Window* _windowPtr;
        const double _polling;
    };
}
