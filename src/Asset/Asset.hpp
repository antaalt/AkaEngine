#pragma once

#include <Aka/Aka.h>

#include <map>

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
	ImageCube,
	ImageArray,
	Font,
	Audio,
	Scene,
};
struct AssetPath {
	aka::Path path; // might be a path to am agglomerated buffer for perf
};
struct AssetInfo {
	AssetPath path;
	AssetType type; // Let us know how to load it.
	// some infos such as creation & type ?
};

// Strict type generated from path ideally & storing asset type.
enum class AssetID {}; // A single resource might use multiple assets.

inline AssetID generateAssetIDFromAssetPath(const AssetPath& path)
{
	return AssetID(0);
}


}