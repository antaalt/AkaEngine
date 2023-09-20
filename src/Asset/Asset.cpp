#include "Asset.hpp"

namespace app {

const char* getAssetTypeString(AssetType type)
{
	switch (type)
	{
	case app::AssetType::Unknown: return "Unknown";
	case app::AssetType::Geometry: return "Geometry";
	case app::AssetType::Material: return "Material";
	case app::AssetType::Batch: return "Batch";
	case app::AssetType::StaticMesh: return "StaticMesh";
	case app::AssetType::DynamicMesh: return "DynamicMesh";
	case app::AssetType::Image:return "Image";
	case app::AssetType::Font:return "Font";
	case app::AssetType::Audio:return "Audio";
	case app::AssetType::Scene:return "Scene";
	default:
		AKA_ASSERT(false, "Invalid asset type");
		return nullptr;
	}
}
}