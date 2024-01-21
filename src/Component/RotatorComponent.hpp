#pragma once

#include <Aka/Scene/Component.hpp>
#include <Aka/Scene/Node.hpp>
#include <Aka/Resource/Archive/ArchiveScene.hpp>

namespace aka {

struct ArchiveRotatorComponent : ArchiveComponent
{
	using ArchiveComponent::ArchiveComponent;

	float speed;
	vec3f axis;

	void parse(BinaryArchive& archive) override;
};

class RotatorComponent : public Component<RotatorComponent, ArchiveRotatorComponent>
{
public:
	RotatorComponent(Node* node);

	vec3f& getAxis() { return m_axis; }
	float& getSpeed() { return m_speed; }

	void onUpdate(Time deltaTime) override;

	void fromArchive(const ArchiveComponent& archive) override;
	void toArchive(ArchiveComponent& archive) override;
private:
	vec3f m_axis;
	float m_speed;
};

AKA_DECL_COMPONENT(RotatorComponent)

};