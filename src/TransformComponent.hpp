#pragma once
#include "Component.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

namespace razaron::physics::component
{
	using namespace razaron::core::component;

	class TransformComponent : public Component
	{
	public:
		TransformComponent(glm::vec3 position);
		TransformComponent(glm::vec3 position, glm::vec3 scale);
		TransformComponent(glm::vec3 position, glm::vec3 scale, glm::vec3 deepScale);
		~TransformComponent();

		glm::vec3 getPosition() { return position; };
		glm::vec3 getScale() { return scale; };
		glm::vec3 getDeepScale() { return deepScale; };
		void setPosition(glm::vec3 position) { this->position = position; };
		void setScale(glm::vec3 scale) { this->scale = scale; };
		void setDeepScale(glm::vec3 deepScale) { this->deepScale = deepScale; };

		void rotate(glm::quat rotation) { orientation = rotation * orientation; };
		glm::mat4 getModel() { return glm::translate(position) * glm::scale(scale) * glm::toMat4(orientation); };

	private:
		glm::vec3 position, scale, deepScale;
		glm::quat orientation;
	};
}
