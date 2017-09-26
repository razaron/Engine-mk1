#pragma once

#include <list>
#include <set>

#include <glm/glm.hpp>

#include "EventStream.hpp"
#include "Component.hpp"

using namespace razaron::eventstream;
using namespace razaron::core::component;

const EventType EVENT_INPUT = EventType::EVENT_1;
const EventType EVENT_MODEL = EventType::EVENT_2;
const EventType EVENT_STEERING = EventType::EVENT_3;

const ComponentType COMPONENT_ANIMAL = ComponentType::COMPONENT_1;

const unsigned SCREEN_WIDTH = 800u;
const unsigned SCREEN_HEIGHT = 800u;

extern float g_cameraZoom;
extern glm::vec2 g_cameraPos;

namespace razaron::input
{
    enum class ContextType
    {
        DEFAULT,
        CONTEXT_1,
        CONTEXT_2,
        CONTEXT_3,
        CONTEXT_4
    };

    enum class InputType
    {
        KEY_ESCAPE,
        KEY_W,
        KEY_A,
        KEY_S,
        KEY_D,
        MOUSE_LEFT,
        MOUSE_RIGHT
    };

    struct Input
    {
        InputType type;
        bool state{ false };
        float rangeX{ 0 };
        float rangeY{ 0 };
    };

    struct Context
    {
        ContextType type;
        std::set<InputType> inputTypes;
    };
}

namespace razaron::eventdata
{
    using namespace razaron::input;

    enum class SteeringBehaviour
    {
        SEEK,
        FLEE
    };

    struct EVENTDATA_INPUT
    {
        ContextType context;
        std::list<Input> inputs;

        EVENTDATA_INPUT(ContextType context, std::list<Input> inputs) : context{ context }, inputs{ inputs } {}
    };

    struct EVENTDATA_MODEL
    {
        glm::mat4 model;

        EVENTDATA_MODEL(glm::mat4 model) : model{ model } {}
    };

    struct EVENTDATA_STEERING
    {
        SteeringBehaviour behaviour;
        unsigned targetID;

        EVENTDATA_STEERING(SteeringBehaviour behaviour, unsigned targetID) : behaviour{ behaviour }, targetID{ targetID } {}
    };
}
