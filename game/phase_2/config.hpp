#pragma once

#include <glm/glm.hpp>

#include "EventStream.hpp"
#include "TaskScheduler.hpp"

using namespace rz::eventstream;

const EventType EVENTTYPE_MODEL = EventType::EVENT_1;
const EventType EVENTTYPE_STEERING = EventType::EVENT_2;
const EventType EVENTTYPE_COLLISION = EventType::EVENT_3;

const unsigned SCREEN_WIDTH = 1024u;
const unsigned SCREEN_HEIGHT = 1024u;

const int GROUP_ALL = 0;
const int GROUP_RED = 1;
const int GROUP_BLUE = 2;
const int GROUP_BULLETS = 3;
const int GROUP_DEPOSITS = 4;


enum class SteeringBehaviour
{
	SEEK,
	ARRIVE,
	MAINTAIN,
	WANDER,
	STOP
};

struct EVENTDATA_MODEL
{
	glm::mat4 model;

	EVENTDATA_MODEL(glm::mat4 model) : model{ model } {}
};

struct EVENTDATA_STEERING
{
	UUID64 target;
	SteeringBehaviour behaviour;

	EVENTDATA_STEERING(UUID64 target, SteeringBehaviour behaviour) :target{ target }, behaviour{ behaviour } {}
};

struct EVENTDATA_COLLISION
{
	UUID64 target;
	float distance;
	int group;

	EVENTDATA_COLLISION(UUID64 target, float distance, int group) :target{ target }, distance{ distance }, group{ group } {}
};