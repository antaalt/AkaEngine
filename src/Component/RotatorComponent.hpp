#pragma once

#include <Aka/Scene/Component.hpp>
#include <Aka/Scene/Node.hpp>
#include <Aka/Resource/Archive/ArchiveScene.hpp>

namespace aka {

struct ArchiveRotatorComponent : ArchiveComponent
{
	ArchiveRotatorComponent(ArchiveComponentVersionType _version);

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

	void fromArchive(const ArchiveRotatorComponent& archive) override;
	void toArchive(ArchiveRotatorComponent& archive) override;
private:
	vec3f m_axis;
	float m_speed;
};

AKA_DECL_COMPONENT(RotatorComponent)

};