#include "RotatorComponent.hpp"

namespace aka {

ArchiveRotatorComponent::ArchiveRotatorComponent() :
	ArchiveComponent(generateComponentID<RotatorComponent>(), 0)
{
}

void ArchiveRotatorComponent::parse(BinaryArchive& archive)
{
	archive.parse<float>(speed);
	archive.parse<float>(axis.x);
	archive.parse<float>(axis.y);
	archive.parse<float>(axis.z);
}

RotatorComponent::RotatorComponent(Node* node) :
	Component(node, generateComponentID<RotatorComponent>()),
	m_axis(vec3f(0, 0, 1)),
	m_speed(45.f)
{
}

void RotatorComponent::onUpdate(Time deltaTime)
{
	anglef rotation = anglef::degree(m_speed * deltaTime.seconds());
	getNode()->setLocalTransform(getNode()->getLocalTransform() * mat4f::rotate(m_axis, rotation));
}

void RotatorComponent::fromArchive(const ArchiveComponent& archive)
{
	const ArchiveRotatorComponent& a = reinterpret_cast<const ArchiveRotatorComponent&>(archive);
	m_speed = a.speed;
	m_axis = a.axis;
}

void RotatorComponent::toArchive(ArchiveComponent& archive)
{
	ArchiveRotatorComponent& a = reinterpret_cast<ArchiveRotatorComponent&>(archive);
	a.speed = m_speed;
	a.axis = m_axis;
}

}