#pragma once

#include <Aka/Scene/Component.hpp>
#include <Aka/Scene/Node.hpp>
#include <Aka/Resource/Archive/ArchiveScene.hpp>

namespace aka {

struct ArchiveCustomComponent : ArchiveComponent
{
	ArchiveCustomComponent(ArchiveComponentVersionType _version);

	aka::String customData;

	void parse(BinaryArchive& archive) override;
};

class CustomComponent : public Component<CustomComponent, ArchiveCustomComponent>
{
public:
	CustomComponent(Node* node);

	const String& getCustomData() const { return m_customData; }

	void fromArchive(const ArchiveCustomComponent& archive) override {}
	void toArchive(ArchiveCustomComponent& archive) override {}
private:
	String m_customData;
};

AKA_DECL_COMPONENT(CustomComponent)

};