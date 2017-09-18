#include "Component.hpp"

using namespace razaron::core::component;

std::atomic<unsigned> razaron::core::component::g_nextID;

Component::Component()
	:m_id(g_nextID++)
{
}

Component::Component(Component && other)
{
	*this = std::move(other);
}

Component::Component(const Component & other)
{
	*this = other;
}

Component::~Component()
{
}

Component & Component::operator=(const Component & rhs)
{
	m_id = rhs.m_id;

	return *this;
}

Component & Component::operator=(Component && rhs)
{
	if (this != &rhs)
	{
		m_id = rhs.m_id;

		rhs.m_id = std::numeric_limits<unsigned int>::max();
	}

	return *this;
}
