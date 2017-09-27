#include "InputSystem.hpp"

using namespace razaron::input;

InputSystem::InputSystem(sf::Window *window)
    : _windowPtr(window), _polling{ 0.1 }
{
    createContext(ContextType::DEFAULT, std::set<InputType>{ InputType::MOUSE_LEFT, InputType::MOUSE_RIGHT, InputType::KEY_ESCAPE, InputType::KEY_W, InputType::KEY_A, InputType::KEY_S, InputType::KEY_D });
}

InputSystem::~InputSystem()
{
}

Task InputSystem::update(EntityMap &entities, double delta)
{
    UNUSED(entities);

    // Get list of all Inputs this frame
    auto inputs = mapInputs(delta);

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

std::list<Input> InputSystem::mapInputs(double delta)
{
    std::list<Input> inputs;

    static double elapsed1{ 0 };
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && (elapsed1 += delta) > _polling)
    {
        elapsed1 = 0;

        auto pos = sf::Mouse::getPosition(*_windowPtr);

        inputs.push_back(Input{
            InputType::MOUSE_LEFT,
            true,
            static_cast<float>(pos.x) / SCREEN_WIDTH,
            static_cast<float>(pos.y) / SCREEN_HEIGHT });
    }

    static double elapsed2{ 0 };
    if (sf::Mouse::isButtonPressed(sf::Mouse::Right) && (elapsed2 += delta) > _polling)
    {
        elapsed2 = 0;

        auto pos = sf::Mouse::getPosition(*_windowPtr);

        inputs.push_back(Input{
            InputType::MOUSE_RIGHT,
            true,
            static_cast<float>(pos.x) / _windowPtr->getSize().x,
            static_cast<float>(pos.y) / _windowPtr->getSize().y });
    }

    static double elapsed3{ 0 };
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape) && (elapsed3 += delta) > _polling)
    {
        elapsed3 = 0;

        inputs.push_back(Input{ InputType::KEY_ESCAPE });
    }

    static double elapsed4{ 0 };
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && (elapsed4 += delta) > _polling)
    {
        elapsed4 = 0;

        inputs.push_back(Input{ InputType::KEY_W });
    }

    static double elapsed5{ 0 };
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) && (elapsed5 += delta) > _polling)
    {
        elapsed5 = 0;

        inputs.push_back(Input{ InputType::KEY_A });
    }

    static double elapsed6{ 0 };
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && (elapsed6 += delta) > _polling)
    {
        elapsed6 = 0;

        inputs.push_back(Input{ InputType::KEY_S });
    }

    static double elapsed7{ 0 };
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) && (elapsed7 += delta) > _polling)
    {
        elapsed7 = 0;

        inputs.push_back(Input{ InputType::KEY_D });
    }

    return inputs;
}
