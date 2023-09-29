#include "RotatorComponent.hpp"

namespace aka {

ArchiveRotatorComponent::ArchiveRotatorComponent() :
	ArchiveComponent(generateComponentID<RotatorComponent>(), 0)
{
}

void ArchiveRotatorComponent::load_internal(BinaryArchive& archive)
{
	speed = archive.read<float>();
	axis = archive.read<vec3f>();
}

void ArchiveRotatorComponent::save_internal(BinaryArchive& archive)
{
	archive.write<float>(speed);
	archive.write<vec3f>(axis);
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

void RotatorComponent::load(const ArchiveComponent& archive)
{
	m_speed = reinterpret_cast<const ArchiveRotatorComponent&>(archive).speed;
	m_axis = reinterpret_cast<const ArchiveRotatorComponent&>(archive).axis;
}

void RotatorComponent::save(ArchiveComponent& archive)
{
	reinterpret_cast<ArchiveRotatorComponent&>(archive).speed = m_speed;
	reinterpret_cast<ArchiveRotatorComponent&>(archive).axis = m_axis;
}

}