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
	auto itAsset = m_assets.insert(std::make_pair(assetID, AssetInfo{ assetID, _path, _assetType }));
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

	auto itAsset2 = m_assets.insert(std::make_pair(assetID, AssetInfo{ assetID, _path, _assetType }));
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

AssetID AssetLibrary::getAssetID(ResourceID _resourceID) const
{
	auto it = m_resources.find(_resourceID);
	if (it == m_resources.end())
		return AssetID::Invalid;
	return it->second;
}

AssetInfo AssetLibrary::getAssetInfo(AssetID _id)
{
	auto it = m_assets.find(_id);
	if (it == m_assets.end())
		return AssetInfo::invalid();
	return it->second;
}

template<typename T>
ResourceHandle<T> get_internal(ResourceID _resourceID, std::map<ResourceID, ResourceHandle<T>>& _map)
{
	static_assert(std::is_base_of<Resource, T>::value, "Invalid resource type");
	auto itResource = _map.find(_resourceID);
	if (itResource != _map.end())
	{
		return itResource->second;
	}
	return ResourceHandle<T>::invalid();
}
template<typename T>
ResourceHandle<T> load_internal(ResourceID _resourceID, const typename ArchiveTrait<T>::Archive& _archive, gfx::GraphicDevice* _device, AssetLibrary* _library, std::map<ResourceID, ResourceHandle<T>>& _map, std::map<AssetID, AssetInfo>& _assets, std::map<ResourceID, AssetID>& _resources)
{
	static_assert(std::is_base_of<Resource, T>::value, "Invalid resource type");
	static_assert(std::is_base_of<Archive, ArchiveTrait<T>::Archive>::value, "Invalid archive type");
	// Check if resource already exist.
	auto itResource = _map.find(_resourceID);
	if (itResource != _map.end())
	{
		Logger::warn("Trying to load a resource that is already loaded.");
		return itResource->second;
	}
	// Get assetID corresponding to resource.
	auto itAssetID = _resources.find(_resourceID);
	if (itAssetID == _resources.end())
	{
		return ResourceHandle<T>::invalid();
	}
	AssetID assetID = itAssetID->second;
	// Get assetInfo
	auto itAssetInfo = _assets.find(assetID);
	if (itAssetInfo == _assets.end())
	{
		return ResourceHandle<T>::invalid();
	}
	const AssetInfo& assetInfo = itAssetInfo->second;
	auto scene = std::make_shared<T>(_resourceID, assetInfo.path.cstr());
	auto it = _map.insert(std::make_pair(_resourceID, ResourceHandle<T>(ResourceState::Loaded)));
	if (it.second)
	{
		AKA_ASSERT(_device != nullptr, "Invalid device");
		ResourceHandle<T> handle = it.first->second;
		handle.get().create(_library, _device, _archive);
		return handle;
	}
	else
	{
		// If we reach this code, it means the resource does not exist & could not be added.
		AKA_UNREACHABLE;
		return ResourceHandle<T>::invalid();
	}
}
template<> 
ResourceHandle<Scene> AssetLibrary::get(ResourceID _resourceID)
{
	return get_internal<Scene>(_resourceID, m_scenes);
}
template<> 
ResourceHandle<StaticMesh> AssetLibrary::get(ResourceID _resourceID)
{
	return get_internal<StaticMesh>(_resourceID, m_staticMeshes);
}
template<>
ResourceHandle<Scene> AssetLibrary::load(ResourceID _resourceID, const ArchiveScene& _archive, gfx::GraphicDevice* _device)
{
	return load_internal<Scene>(_resourceID, _archive, _device, this, m_scenes, m_assets, m_resources);
}
template<>
ResourceHandle<StaticMesh> AssetLibrary::load(ResourceID _resourceID, const ArchiveStaticMesh& _archive, gfx::GraphicDevice* _device)
{
	return load_internal<StaticMesh>(_resourceID, _archive, _device, this, m_staticMeshes, m_assets, m_resources);
}
template<> 
ResourceRange<Scene> AssetLibrary::getRange()
{
	return ResourceRange<Scene>(m_scenes);
}
template<> 
ResourceRange<StaticMesh> AssetLibrary::getRange()
{
	return ResourceRange<StaticMesh>(m_staticMeshes);
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