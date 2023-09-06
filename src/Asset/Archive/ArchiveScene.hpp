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
constexpr ArchiveSceneID InvalidArchiveSceneID = static_cast<ArchiveSceneID>(-1);
inline std::underlying_type<ArchiveSceneID>::type toIntegral(ArchiveSceneID value) { return static_cast<std::underlying_type<ArchiveSceneID>::type>(value); }
//ArchiveSceneID operator+(ArchiveSceneID lhs, ArchiveSceneID rhs) { return static_cast<ArchiveSceneID>(toIntegral(lhs) + toIntegral(rhs)); }
//ArchiveSceneID operator-(ArchiveSceneID lhs, ArchiveSceneID rhs) { return static_cast<ArchiveSceneID>(toIntegral(lhs) - toIntegral(rhs)); }

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
AKA_IMPLEMENT_BITMASK_OPERATOR(SceneComponentMask)

template<typename T>
struct BitMask
{
	// Check is enum, Use First & Last
	void add(T value) {}
	void remove(T value) {}
	void test(T value) {}

	static BitMask<T> get(T value) { return 1U << EnumToIndex(value); }
private:
	T m_value;
};



struct ArchiveSceneEntity {
	SceneComponentMask components;
	ArchiveSceneID id[EnumCount<SceneComponent>()];
};

struct ArchiveScene : Archive {
	ArchiveScene() {}
	ArchiveScene(AssetID id) : Archive(id) {}

	Vector<ArchiveStaticMesh> meshes;
	Vector<ArchiveSceneTransform> transforms;
	Vector<ArchiveSceneEntity> entities;
	// TODO: add lights, envmap, cameras, gameplay struct


	ArchiveLoadResult load(AssetLibrary* _library, const AssetPath& path) override;
	ArchiveSaveResult save(AssetLibrary* _library, const AssetPath& path) override;
};

};