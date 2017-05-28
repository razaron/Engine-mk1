#pragma once

#include "Component.h"

namespace razaron::physics::component
{
	using namespace razaron::core::component;

	class LocomotionComponent :public Component
	{
	public:
		LocomotionComponent();
		~LocomotionComponent();

	private:
	};
}