#pragma once

#include <Aka/Aka.h>


namespace app {

// An asset is the result of an import.
// Contain an ID and a path (ID depend on path ?) and a type
// Whats an asset ? something we can drop in engine ?
enum class AssetType {
	MeshGeometry,
	MeshMaterial,
	MeshBatch,
	Mesh,
	StaticMesh,
	DynamicMesh,
	Image,
	Font,
	Audio,
	Scene,
};
// Not an absolute path, but a relative path
struct AssetPath {
	AssetPath(const char* _path) : m_path(_path) {}

	aka::Path getPath() const { return m_path; }
private:
	aka::Path m_path; // might be a path to am agglomerated buffer for perf
};
struct AssetInfo {
	AssetPath path;
	AssetType type; // Let us know how to load it.
	// some infos such as creation & type ?
};

// Strict type generated from path ideally & storing asset type.
enum class AssetID : uint64_t {
	Invalid = -1
}; // A single resource might use multiple assets.

inline AssetID generateAssetIDFromAssetPath(const AssetPath& path)
{
	size_t hash = aka::hash(path.getPath().cstr(), path.getPath().length());
	return AssetID(hash);
}


}