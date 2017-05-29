#pragma once

#include "Misc.h"

/*! */
namespace razaron::core::component
{
	enum class ComponentType { DEFAULT, FOO, BAR, RENDER, MESH, TRANSFORM, LOCOMOTION, COLLIDER, CONTROLLER, STEERING, ENUM_SIZE};

	extern AtomicCounter<unsigned> g_nextID;

	using ComponentHandle = std::pair<ComponentType, Handle>;

	class Component {
	public:
		Component();
		Component(Component&& other);
		Component(const Component& other);
		~Component();

		Component& operator=(const Component& other);
		Component& operator=(Component&& other);

		unsigned getID() { return m_id; }

	private:
		unsigned m_id;
	};
}