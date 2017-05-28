#pragma once

#include "Component.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

namespace razaron::render::component
{
	class RenderComponent : public razaron::core::component::Component
	{
	public:
		RenderComponent();
		~RenderComponent();

		glm::vec3 getOvColour() { return m_colour; };
		void setOvColour(glm::vec3 p_colour) { this->m_colour = p_colour; };
		float getAlpha() { return m_alpha; };
		void setAlpha(float p_alpha) { this->m_alpha = p_alpha; };
		float getBoundingRadius() { return m_boundingRadius; };
		void setBoundingRadius(float p_boundingRadius) { this->m_boundingRadius = p_boundingRadius; };

		std::array<glm::vec3, 3> m_vertices;
		std::array<unsigned short, 3> m_indices;
		glm::mat4 m_modelMatrix;

	private:
		glm::vec3 m_colour;
		float m_alpha, m_boundingRadius;
	};
}
