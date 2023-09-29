#pragma once

#include <Aka/Scene/Component.hpp>
#include <Aka/Scene/Node.hpp>
#include <Aka/Resource/Archive/ArchiveScene.hpp>

namespace aka {

struct ArchiveRotatorComponent : ArchiveComponent
{
	ArchiveRotatorComponent();

	float speed;
	vec3f axis;

	void load_internal(BinaryArchive& archive) override;
	void save_internal(BinaryArchive& archive) override;
};

class RotatorComponent : public Component
{
public:
	RotatorComponent(Node* node);

	vec3f& getAxis() { return m_axis; }
	float& getSpeed() { return m_speed; }

	void onUpdate(Time deltaTime) override;

	void load(const ArchiveComponent& archive) override;
	void save(ArchiveComponent& archive) override;
private:
	vec3f m_axis;
	float m_speed;
};

AKA_DECL_COMPONENT(RotatorComponent)

};