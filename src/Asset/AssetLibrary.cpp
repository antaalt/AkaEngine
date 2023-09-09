#include "AssetLibrary.hpp"

#include "../json.hpp"

#include <fstream>

namespace app {

ResourceID generateResourceIDFromAssetID(AssetID id)
{
	return ResourceID(id);
}

AssetID generateAssetIDFromAssetPath(const AssetPath& path)
{
	// With an AssetID depending on path, moving this asset will break all references...
	size_t hash = aka::hash(path.cstr(), path.size());
	return AssetID(hash);
}

ResourceType getResourceType(AssetType _type)
{
	switch (_type)
	{
	default:
	case AssetType::Batch:
	case AssetType::Material:
	case AssetType::Geometry:
	case AssetType::DynamicMesh:
	case AssetType::Image:
	case AssetType::Font:
	case AssetType::Audio:
		return ResourceType::Unknown;
	case AssetType::StaticMesh:
		return ResourceType::StaticMesh;
	case AssetType::Scene:
		return ResourceType::Scene;
	}
	return ResourceType::Unknown;
}

void AssetLibrary::parse()
{
	using json = nlohmann::json;

	Path path = AssetPath::getAssetPath() + "library.json";
	std::ifstream f(path.cstr());

	json data = json::parse(f);

	{
		json& dataMetadata = data["metadata"];
	}

	{
		json& dataAssets = data["assets"];
		for (json& dataAsset : dataAssets)
		{
			uint64_t assetID = dataAsset["id"].get<uint64_t>();
			std::string path = dataAsset["path"].get<std::string>();
			AssetType type = dataAsset["type"].get<AssetType>();
			AssetID id = registerAsset(AssetPath(path.c_str()), type);
			AKA_ASSERT(id == AssetID(assetID), "Invalid assetID");
		}
	}
}

void AssetLibrary::serialize()
{
	using json = nlohmann::json;

	json data;

	{
		json dataMetadata = json::object();
		data["metadata"] = dataMetadata;
	}
	{
		json dataAssets = json::array();
		for (auto& pair : m_assets)
		{
			json dataAsset = json::object();
			dataAsset["id"] = (uint64_t)pair.first;
			dataAsset["path"] = pair.second.path.cstr();
			dataAsset["type"] = (uint32_t)pair.second.type;
			dataAssets.push_back(dataAsset);
		}
		data["assets"] = dataAssets;
	}

	Path path = AssetPath::getAssetPath() + "library.json";
	std::ofstream f(path.cstr());
	f << data.dump(4);
}

bool validate(AssetLibrary* _library, AssetID id, AssetType _type)
{
	switch (_type)
	{
	case AssetType::Geometry: return ArchiveGeometry(id).validate(_library);
	case AssetType::Material: return ArchiveMaterial(id).validate(_library);
	case AssetType::Batch: return ArchiveBatch(id).validate(_library);
	case AssetType::StaticMesh: return ArchiveStaticMesh(id).validate(_library);
	case AssetType::DynamicMesh: return false;
	case AssetType::Image: return ArchiveImage(id).validate(_library);
	case AssetType::Font: return false;
	case AssetType::Audio: return false;
	case AssetType::Scene: return ArchiveScene(id).validate(_library);
	default:
	case AssetType::Unknown:
		return false;
	}
}

AssetID AssetLibrary::registerAsset(const AssetPath& _path, AssetType _assetType)
{
	AssetID assetID = generateAssetIDFromAssetPath(_path);
	auto itAsset = m_assets.insert(std::make_pair(assetID, AssetInfo{ assetID, _path, _assetType, 0, 0 }));
	if (!itAsset.second)
	{
		// Check if the file already exist & is valid, if so, use it.
		bool samePath = itAsset.first->second.path == _path;
		bool fileExist = OS::File::exist(_path.getAbsolutePath());
		if (samePath && (!fileExist || validate(this, assetID, _assetType)))
			return assetID;
		else
			return AssetID::Invalid; // Avoid overwriting an asset. There might be hash conflict.
	}

	auto itAsset2 = m_assets.insert(std::make_pair(assetID, AssetInfo{ assetID, _path, _assetType, 0 , 0 }));
	ResourceID resourceID = generateResourceIDFromAssetID(assetID);

	ResourceType resourceType = getResourceType(_assetType);
	if (resourceType != ResourceType::Unknown)
	{
		auto itResource = m_resources.insert(std::make_pair(resourceID, assetID));
		if (!itResource.second)
			return AssetID::Invalid;
	}
	// Could check file if correct type but might not be created yet...

	return assetID;
}

ResourceID AssetLibrary::getResourceID(AssetID _assetID) const
{
	return generateResourceIDFromAssetID(_assetID);
}

AssetInfo& AssetLibrary::getAssetInfo(AssetID _id)
{
	auto it = m_assets.find(_id);
	if (it == m_assets.end())
		return AssetInfo::invalid();
	return it->second;
}

template <typename T, typename A>
ResourceHandle<T> getResource(ResourceID _resourceID, AssetLibrary* library, gfx::GraphicDevice* _device, std::map<ResourceID, ResourceHandle<T>>& _map, std::map<AssetID, AssetInfo>& _assets, std::map<ResourceID, AssetID>& _resources)
{
	static_assert(std::is_base_of<Resource, T>::value, "Invalid resource type");
	static_assert(std::is_base_of<Archive, A>::value, "Invalid archive type");
	// Check if resource already exist.
	auto itScene = _map.find(_resourceID);
	if (itScene != _map.end())
	{
		return itScene->second;
	}
	// Get assetID corresponding to resource.
	auto itResource = _resources.find(_resourceID);
	if (itResource == _resources.end())
	{
		return ResourceHandle<T>::invalid();
	}
	AssetID assetID = itResource->second;
	// Get assetInfo
	auto itAsset = _assets.find(itResource->second);
	if (itAsset == _assets.end())
	{
		return ResourceHandle<T>::invalid();
	}
	AssetInfo assetInfo = itAsset->second;
	auto scene = std::make_shared<T>(_resourceID, assetInfo.path.cstr());
	A archive(itResource->second);
	ArchiveLoadResult res = archive.load(ArchiveLoadContext(library), assetInfo.path);
	AKA_ASSERT(res == ArchiveLoadResult::Success, "Failed loading");

	auto it = _map.insert(std::make_pair(_resourceID, ResourceHandle<T>(ResourceState::Loaded)));
	if (it.second)
	{
		AKA_ASSERT(_device != nullptr, "Creating resource without passing device");
		ResourceHandle<T> handle = it.first->second;
		handle.get().create(library, _device, archive);
		return handle;
	}
	else
	{
		// If we reach this code, it means the resource does not exist & could not be added.
		AKA_UNREACHABLE;
		return ResourceHandle<T>::invalid();
	}
}

ResourceHandle<Scene> AssetLibrary::getScene(ResourceID _resourceID, gfx::GraphicDevice* _device)
{
	return getResource<Scene, ArchiveScene>(_resourceID, this, _device, m_scenes, m_assets, m_resources);
}

ResourceHandle<StaticMesh> AssetLibrary::getStaticMesh(ResourceID _resourceID, gfx::GraphicDevice* _device)
{
	return getResource<StaticMesh, ArchiveStaticMesh>(_resourceID, this, _device, m_staticMeshes, m_assets, m_resources);
}

void AssetLibrary::destroy(gfx::GraphicDevice* _device)
{
	for (auto it : m_scenes)
	{
		if (it.second.isLoaded())
		{
			it.second.get().destroy(this, _device);
		}
	}
	for (auto it : m_staticMeshes)
	{
		if (it.second.isLoaded())
		{
			it.second.get().destroy(this, _device);
		}
	}
}

}