#include "Component.hpp"

using namespace razaron::core::component;

std::atomic<unsigned> g_nextID;

Component::Component()
	:_id(g_nextID++)
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
	_id = rhs._id;

	return *this;
}

Component & Component::operator=(Component && rhs)
{
	if (this != &rhs)
	{
		_id = rhs._id;

		rhs._id = std::numeric_limits<unsigned int>::max();
	}

	return *this;
}
