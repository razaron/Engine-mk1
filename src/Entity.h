#pragma once

#include <vector>
#include <map>

#include "Component.h"

namespace razaron::core::entity
{
	using namespace razaron::core::component;

	class Entity {
	public:
		Entity() {}

		Entity(std::map<ComponentType, Handle> p_components)
			:m_components(p_components), m_id(g_nextID++) {}

		~Entity() {}

		std::pair<ComponentType, Handle> addComponent(std::pair<ComponentType, Handle> p_component)
		{
			m_components[p_component.first] = p_component.second;
			return p_component;
		}

		std::map<ComponentType, Handle> getComponents()
		{ 
			return m_components; 
		};

		unsigned int getID() { return m_id; }

		Handle& operator[](ComponentType p_type)
		{
			return m_components[p_type];
		}

	private:
		unsigned int m_id;
		std::map<ComponentType, Handle> m_components;
	};
}