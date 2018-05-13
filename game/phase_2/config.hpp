#pragma once

#include <glm/glm.hpp>

#include "EventStream.hpp"

using namespace razaron::eventstream;

const EventType EVENTTYPE_MODEL = EventType::EVENT_1;

const unsigned SCREEN_WIDTH = 1024u;
const unsigned SCREEN_HEIGHT = 1024u;

struct EVENTDATA_MODEL
{
    glm::mat4 model;

    EVENTDATA_MODEL(glm::mat4 model) : model{ model } {}
};
