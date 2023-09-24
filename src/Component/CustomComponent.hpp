#pragma once

#include <Aka/Scene/Component.hpp>
#include <Aka/Scene/Node.hpp>

namespace app {

class CustomComponent : public aka::Component 
{
public:
	CustomComponent() : aka::Component(aka::Component::generateID<CustomComponent>()), CustomData("Yoyoyo") {}

	aka::String CustomData;
};

}