#include "Component.h"

using namespace razaron::core::component;

AtomicCounter<unsigned> razaron::core::component::g_nextID;

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
	std::clog << "Component Copy Constructor. ID = " << m_id << std::endl;
}

Component::~Component()
{
	std::clog << "Component Destructor. ID = " << m_id << std::endl;
}

Component & Component::operator=(const Component & other)
{
	std::clog << "Component Copy Operator. Old ID = " << m_id << " New ID = " << other.m_id << std::endl;

	m_id = other.m_id;

	return *this;
}

Component & Component::operator=(Component && other)
{
	if (this != &other)
	{
		std::clog << "Component Move Operator. ID = " << other.m_id << std::endl;

		m_id = other.m_id;

		other.m_id = std::numeric_limits<unsigned int>::max();
	}

	return *this;
}
