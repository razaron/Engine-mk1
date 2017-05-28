#include "TransformComponent.h"

using namespace razaron::physics::component;

TransformComponent::TransformComponent(glm::vec3 position)
	:position(position), scale(1.0f), deepScale(1.0f), orientation(glm::quat())
{
}

TransformComponent::TransformComponent(glm::vec3 position, glm::vec3 scale)
	: position(position), scale(scale), deepScale(1.0f), orientation(glm::quat())
{
}

TransformComponent::TransformComponent(glm::vec3 position, glm::vec3 scale, glm::vec3 deepScale)
	: position(position), scale(scale), deepScale(deepScale), orientation(glm::quat())
{
}


TransformComponent::~TransformComponent()
{
}