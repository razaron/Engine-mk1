#include "RenderComponent.h"

using namespace razaron::render::component;

RenderComponent::RenderComponent()
	:m_colour(glm::vec3(1.0f)), m_alpha(1.0f)
{
}

RenderComponent::~RenderComponent()
{
}