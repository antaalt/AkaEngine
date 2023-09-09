#pragma once

#include <Aka/Aka.h>

#include "AssetPath.hpp"

namespace app {

enum class AssetID : uint64_t 
{
	Invalid = -1
};

enum class AssetType : uint32_t
{
	Unknown,

	Geometry,
	Material,
	Batch,
	StaticMesh,
	DynamicMesh,
	Image,
	Font,
	Audio,
	Scene,

	First = Geometry,
	Last = Scene,
};


const char* getAssetTypeString(AssetType type);

struct AssetInfo 
{
	AssetID id;
	AssetPath path;
	AssetType type;
	uint32_t writeCounter = 0;
	uint32_t readCounter = 0;

	static AssetInfo invalid() {
		return AssetInfo{
			AssetID::Invalid,
			"",
			AssetType::Unknown,
			0,
			0
		};
	}
};


}