#pragma once

#include "Component.h"

namespace razaron::physics::component
{
	using namespace razaron::core::component;

	class ColliderComponent :public Component
	{
	public:
		ColliderComponent();
		~ColliderComponent();

	private:
	};
}