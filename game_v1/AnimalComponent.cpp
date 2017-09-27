#include "AnimalComponent.hpp"

using namespace razaron::game;

AnimalComponent::AnimalComponent()
    : _diet{ AnimalDiet::CARNIVORE }
{
}

AnimalComponent::AnimalComponent(AnimalDiet diet)
    : _diet{ diet }
{
}

AnimalComponent::~AnimalComponent()
{
}
