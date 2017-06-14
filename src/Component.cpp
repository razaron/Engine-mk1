#include "Component.hpp"

using namespace razaron::core::component;

std::atomic<unsigned> razaron::core::component::g_nextID;

Component::Component()
	:m_id(g_nextID++)
{
	std::clog << "Component Default Constructor. ID = " << m_id << std::endl;
}

Component::Component(Component && other)
{
	std::clog << "Component Move Constructor. ID = " << other.m_id << std::endl;

	*this = std::move(other);
}

Component::Component(const Component & other)
{
	std::clog << "Component Copy Constructor. ID = " << other.m_id << std::endl;

	*this = other;
}

Component::~Component()
{
	std::clog << "Component Destructor. ID = " << m_id << std::endl;
}

Component & Component::operator=(const Component & rhs)
{
	std::clog << "Component Copy Operator. Old ID = " << m_id << " New ID = " << rhs.m_id << std::endl;

	m_id = rhs.m_id;

	return *this;
}

Component & Component::operator=(Component && rhs)
{
	if (this != &rhs)
	{
		std::clog << "Component Move Operator. ID = " << rhs.m_id << std::endl;

		m_id = rhs.m_id;

		rhs.m_id = std::numeric_limits<unsigned int>::max();
	}

	return *this;
}
