#include "CustomComponent.hpp"

namespace aka {

CustomComponent::CustomComponent(Node* node) :
	Component(node, generateComponentID<CustomComponent>()), 
	m_customData("Yoyoyo")
{
}

ArchiveCustomComponent::ArchiveCustomComponent() :
	ArchiveComponent(generateComponentID<CustomComponent>(), 0)
{
}

void ArchiveCustomComponent::parse(BinaryArchive& archive)
{
	archive.parse(customData);
}

}