#include "CustomComponent.hpp"

namespace aka {

CustomComponent::CustomComponent() : 
	Component(generateComponentID<CustomComponent>()), 
	m_customData("Yoyoyo")
{
}

ArchiveCustomComponent::ArchiveCustomComponent() :
	ArchiveComponent(generateComponentID<CustomComponent>(), 0)
{
}

void ArchiveCustomComponent::load_internal(BinaryArchive& archive)
{
	uint32_t length = archive.read<uint32_t>();
	customData.resize(length);
	archive.read(customData.data(), customData.size());
	customData[length] = '\0';
}

void ArchiveCustomComponent::save_internal(BinaryArchive& archive)
{
	archive.write<uint32_t>(customData.length());
	archive.write(customData.data(), customData.size());
}

}