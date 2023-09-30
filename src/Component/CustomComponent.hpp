#pragma once

#include <Aka/Scene/Component.hpp>
#include <Aka/Scene/Node.hpp>
#include <Aka/Resource/Archive/ArchiveScene.hpp>

namespace aka {

struct ArchiveCustomComponent : ArchiveComponent
{
	ArchiveCustomComponent();

	aka::String customData;

	void parse(BinaryArchive& archive) override;
};

class CustomComponent : public Component
{
public:
	CustomComponent(Node* node);

	const String& getCustomData() const { return m_customData; }

	void fromArchive(const ArchiveComponent& archive) override {}
	void toArchive(ArchiveComponent& archive) override {}
private:
	String m_customData;
};

AKA_DECL_COMPONENT(CustomComponent)

};