#include "RotatorComponent.hpp"

namespace aka {

ArchiveRotatorComponent::ArchiveRotatorComponent(ArchiveComponentVersionType _version) :
	ArchiveComponent(Component<RotatorComponent, ArchiveRotatorComponent>::getComponentID(), _version)
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
	Component(node),
	m_axis(vec3f(0, 0, 1)),
	m_speed(45.f)
{
}

void RotatorComponent::onUpdate(Time deltaTime)
{
	anglef rotation = anglef::degree(m_speed * deltaTime.seconds());
	getNode()->setLocalTransform(getNode()->getLocalTransform() * mat4f::rotate(m_axis, rotation));
}

void RotatorComponent::fromArchive(const ArchiveRotatorComponent& archive)
{
	m_speed = archive.speed;
	m_axis = archive.axis;
}

void RotatorComponent::toArchive(ArchiveRotatorComponent& archive)
{
	archive.speed = m_speed;
	archive.axis = m_axis;
}

}