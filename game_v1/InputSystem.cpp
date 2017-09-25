#include "InputSystem.hpp"

using namespace razaron::input;

InputSystem::InputSystem(sf::Window *window)
    : _windowPtr(window)
{
    createContext(ContextType::DEFAULT, std::set<InputType>{ InputType::MOUSE_LEFT, InputType::MOUSE_RIGHT, InputType::KEY_ESCAPE, InputType::KEY_W, InputType::KEY_A, InputType::KEY_S, InputType::KEY_D });
}

InputSystem::~InputSystem()
{
}

Task InputSystem::update(EntityMap &entities, double delta)
{
    UNUSED(entities);

    static double delay{0};

    if((delay += delta) < _interval)
        return {};
    else
        delay = 0;

    // Get list of all Inputs this frame
    auto inputs = mapInputs();

    if (inputs.size())
    {
        int ads = 2;
        ads++;
    }

    // Create event for each sublist of Inputs that apply to a context
    std::vector<Event> events;
    for (auto &context : _contexts)
    {
        decltype(inputs) eventInputs;

        // Loop through main Input list, extracting Inputs relevant to the context
        auto it = inputs.begin();
        while (it != inputs.end())
        {
            auto temp = it++;

            if (context.inputTypes.find((*temp).type) != context.inputTypes.end())
                eventInputs.splice(eventInputs.begin(), inputs, temp);
        }

        // Create new input event with extracted inputs
        events.push_back(Event{
            0,
            EVENT_INPUT,
            std::make_shared<EVENTDATA_INPUT>(
                context.type,
                eventInputs) });
    }

    _eventStream.pushEvents(events, StreamType::OUTGOING);

    return Task{};
}

ComponentHandle InputSystem::createComponent(ComponentType type, std::shared_ptr<void> tuplePtr)
{
    UNUSED(tuplePtr);

    Handle h;

    switch (type)
    {
    default:
    {
        h = Handle{};
        break;
    }
    }

    return ComponentHandle{ type, h };
}

bool InputSystem::removeComponent(ComponentHandle ch)
{
    UNUSED(ch);
    return false;
}

Context InputSystem::createContext(ContextType type, std::set<InputType> inputTypes)
{
    Context context{ type, inputTypes };
    _contexts.emplace_back(context);

    return context;
}

std::list<Input> InputSystem::mapInputs()
{
    std::list<Input> inputs;

    if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
    {
        auto pos = sf::Mouse::getPosition(*_windowPtr);

        inputs.push_back(Input{
            InputType::MOUSE_LEFT,
            true,
            static_cast<float>(pos.x)/SCREEN_WIDTH,
            static_cast<float>(pos.y)/SCREEN_HEIGHT
        });
    }

    if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
    {
        auto pos = sf::Mouse::getPosition(*_windowPtr);

        inputs.push_back(Input{
            InputType::MOUSE_RIGHT,
            true,
            static_cast<float>(pos.x)/_windowPtr->getSize().x,
            static_cast<float>(pos.y)/_windowPtr->getSize().y
        });
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
    {
        inputs.push_back(Input{ InputType::KEY_ESCAPE });
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
    {
        inputs.push_back(Input{ InputType::KEY_W });
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        inputs.push_back(Input{ InputType::KEY_A });
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    {
        inputs.push_back(Input{ InputType::KEY_S });
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        inputs.push_back(Input{ InputType::KEY_D });
    }

    return inputs;
}
