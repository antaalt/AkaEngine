#pragma once 

#include "../Archive/ArchiveStaticMesh.hpp"

namespace app {

using namespace aka;


enum class ArchiveSceneVersion : uint32_t
{
	ArchiveCreation = 0,

	Latest = ArchiveCreation
};

struct ArchiveSceneTransform {
	mat4f matrix;
};

enum class ArchiveSceneID : uint32_t {};

// TODO move to scene
enum class SceneComponent {
	Unknown,

	Transform,
	Hierarchy,
	StaticMesh,
	PointLight,
	SunLight,

	First = Transform,
	Last = SunLight,
};
enum class SceneComponentMask {
	Transform  = 1 << aka::EnumToIndex(SceneComponent::Transform),
	Hierarchy  = 1 << aka::EnumToIndex(SceneComponent::Hierarchy),
	StaticMesh = 1 << aka::EnumToIndex(SceneComponent::StaticMesh),

	PointLight = 1 << aka::EnumToIndex(SceneComponent::PointLight),
	SunLight   = 1 << aka::EnumToIndex(SceneComponent::SunLight),
};

template<typename T>
struct BitMask
{
	// Check is enum, Use First & Last
	void add(T value) {}
	void remove(T value) {}
	void test(T value) {}
};

AKA_IMPLEMENT_BITMASK_OPERATOR(SceneComponent)


struct ArchiveSceneEntity {
	SceneComponent components;
	ArchiveSceneID id[EnumCount<SceneComponent>()];
};

struct ArchiveScene : Archive {
	Vector<ArchiveStaticMesh> meshes;
	Vector<ArchiveSceneTransform> transforms;
	Vector<ArchiveSceneEntity> entities;
	// TODO: add lights, envmap, cameras, gameplay struct


	ArchiveLoadResult load(const ArchivePath& path) override;
	ArchiveSaveResult save(const ArchivePath& path) override;
};

};