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
        InputSystem(sf::Window* p_window);

        ~InputSystem();

        Task update(EntityMap &p_entities, double delta);
        ComponentHandle createComponent(ComponentType p_type);
        bool removeComponent(ComponentHandle p_ch);

        Context createContext(ContextType p_type, std::set<InputType> p_inputTypes);
        std::list<Input> mapInputs();

      private:
        std::list<Context> m_contexts;
        sf::Window* m_windowPtr;
    };
}
