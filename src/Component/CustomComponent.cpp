#include "CustomComponent.hpp"

namespace aka {

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