#include "CustomComponent.hpp"

namespace aka {

ArchiveCustomComponent::ArchiveCustomComponent(ArchiveComponentVersionType _version) :
	ArchiveComponent(Component<CustomComponent, ArchiveCustomComponent>::getComponentID(), _version)
{
}

CustomComponent::CustomComponent(Node* node) :
	Component(node),
	m_customData("Yoyoyo")
{
}

void ArchiveCustomComponent::parse(BinaryArchive& archive)
{
	archive.parse(customData);
}

}