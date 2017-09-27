#include "ShapeComponent.hpp"

using namespace razaron::render;

ShapeComponent::ShapeComponent()
    : _colour{glm::vec4{1.f,1.f,1.f,1.f}}
{
}

ShapeComponent::ShapeComponent(glm::vec4 colour)
    : _colour{colour}
{
}

ShapeComponent::~ShapeComponent()
{
}
