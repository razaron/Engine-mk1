#include "TransformComponent.hpp"

using namespace razaron::physics;

TransformComponent::TransformComponent()
:translation{0.f,0.f}, scale{0.1f, 0.1f}, m_rotation{0}
{
}

TransformComponent::~TransformComponent()
{
}

glm::mat4 TransformComponent::getModel()
{
    glm::mat4 translationMatrix = glm::translate(glm::mat4{}, glm::vec3{ translation, 0.f });
    glm::mat4 scaleMatrix = glm::scale(glm::vec3{ scale, 1.f });
    glm::mat4 rotationMatrix = glm::rotate(m_rotation, glm::vec3{ 0.f, 0.f, 1.f });

    return translationMatrix * rotationMatrix * scaleMatrix;
}

float TransformComponent::setRotation(float p_rotation)
{
    m_rotation = p_rotation;

    if(m_rotation > 2*glm::pi<float>())
        m_rotation -= 2*glm::pi<float>();
    else if (m_rotation < 0)
        m_rotation += 2*glm::pi<float>();

    return m_rotation;
}
