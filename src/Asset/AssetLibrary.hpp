#pragma once

#include "Resource/Resource.hpp"
#include "Resource/StaticMesh.hpp"
#include "Resource/Scene.hpp"
#include "Asset.hpp"

#include "Archive/ArchiveScene.hpp"

#include <map>

namespace app {

inline ResourceID generateResourceIDFromAssetID(AssetID id)
{
	return ResourceID(id);
}

template <typename T>
struct ResourceIterator {

private:
	using Iterator = typename std::map<ResourceID, ResourceHandle<T>>::iterator;
	static_assert(std::is_base_of<Resource, T>::value, "This should inherit Resource");
public:
	explicit ResourceIterator(Iterator& value) : m_value(value) {}
	ResourceIterator& operator++()
	{
		m_value++;
		return *this;
	}
	ResourceIterator operator++(int)
	{
		ResourceIterator old = *this;
		++(*this);
		return old;
	}
	std::pair<ResourceID, ResourceHandle<T>> operator*() { return *m_value; }
	bool operator==(const ResourceIterator<T>& value) { return value.m_value == m_value; }
	bool operator!=(const ResourceIterator<T>& value) { return value.m_value != m_value; }
private:
	Iterator m_value;
};

template <typename T>
struct ResourceRange {
private:
	static_assert(std::is_base_of<Resource, T>::value, "This should inherit Resource");
public:
	ResourceRange(std::map<ResourceID, ResourceHandle<T>>& resources) : m_resources(resources) {}

	ResourceIterator<T> begin() { return ResourceIterator<T>(m_resources.begin()); }
	ResourceIterator<T> end() { return ResourceIterator<T>(m_resources.end()); }
private:
	std::map<ResourceID, ResourceHandle<T>>& m_resources;
};

struct AssetIterator {
private:
	using Iterator = std::map<AssetID, AssetInfo>::iterator;
public:
	explicit AssetIterator(Iterator& value) : m_value(value) {}
	AssetIterator& operator++()
	{
		m_value++;
		return *this;
	}
	AssetIterator operator++(int)
	{
		AssetIterator old = *this;
		++(*this);
		return old;
	}
	std::pair<AssetID, AssetInfo> operator*() { return *m_value; }
	bool operator==(const AssetIterator& value) { return value.m_value == m_value; }
	bool operator!=(const AssetIterator& value) { return value.m_value != m_value; }
private:
	Iterator m_value;
};

struct AssetRange {
	AssetRange(std::map<AssetID, AssetInfo>& assets) : m_assets(assets) {}

	AssetIterator begin() { return AssetIterator(m_assets.begin()); }
	AssetIterator end() { return AssetIterator(m_assets.end()); }
private:
	std::map<AssetID, AssetInfo>& m_assets;
};

inline ResourceType getResourceType(AssetType _type)
{
	switch (_type)
	{
	default:
	case AssetType::Mesh:
	case AssetType::MeshBatch:
	case AssetType::MeshMaterial:
	case AssetType::MeshGeometry:
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

class AssetLibrary {
public:
	// parse library.json & store AssetInfo with AssetID
	// Or parse whole folder looking for asset, with headers, we can id them
	void parse();
	// serialize library.json 
	void serialize();
public:
	// For importer.
	AssetID registerAsset(const AssetPath& _path, AssetType _assetType) {
		AssetID assetID = generateAssetIDFromAssetPath(_path);
		auto itAsset = m_assets.insert(std::make_pair(assetID, AssetInfo{ _path, _assetType }));
		if (!itAsset.second)
			return AssetID::Invalid;

		auto itAsset2 = m_assets.insert(std::make_pair(assetID, AssetInfo{ _path, _assetType }));
		ResourceID resourceID = generateResourceIDFromAssetID(assetID);

		ResourceType resourceType = getResourceType(_assetType);
		if (resourceType != ResourceType::Unknown)
		{
			auto itResource = m_resources.insert(std::make_pair(resourceID, assetID));
			if (!itResource.second)
				return AssetID::Invalid;
		}

		// TODO add check for type which read file (to check it exist, & check type is valid)
		// switch type, read using archive (only header), check result.

		return assetID;
	}
	ResourceID getResourceID(AssetID _assetID)
	{
		return generateResourceIDFromAssetID(_assetID);
	}
public:
	AssetInfo getAssetInfo(AssetID _id)
	{
		return m_assets.find(_id)->second;
	}
	// Function will check if asset exist
	ResourceHandle<Scene> getScene(ResourceID _resourceID)
	{
		// Check if resource already exist.
		auto itScene = m_scenes.find(_resourceID);
		if (itScene != m_scenes.end())
		{
			return itScene->second;
		}
		// Get assetID corresponding to resource.
		auto itResource = m_resources.find(_resourceID);
		if (itResource == m_resources.end())
		{
			return ResourceHandle<Scene>::invalid();
		}
		AssetID assetID = itResource->second;
		// Get assetInfo
		auto itAsset = m_assets.find(itResource->second);
		if (itAsset == m_assets.end())
		{
			return ResourceHandle<Scene>::invalid();
		}
		AssetInfo assetInfo = itAsset->second;
		auto scene = std::make_shared<Scene>();
		ArchiveScene archive(itResource->second);
		ArchiveLoadResult res = archive.load(this, assetInfo.path);
		AKA_ASSERT(res == ArchiveLoadResult::Success, "Failed loading");

		auto it = m_scenes.insert(std::make_pair(_resourceID, ResourceHandle<Scene>(ResourceState::Loaded)));
		if (it.second)
		{
			ResourceHandle<Scene> handle = it.first->second;
			handle.get().create(this, archive);
			return handle;
		}
		else
		{
			AKA_UNREACHABLE;
			return ResourceHandle<Scene>::invalid();
		}
	}
	ResourceHandle<StaticMesh> getStaticMesh(ResourceID _resourceID) 
	{
		// Check if resource already exist.
		auto itMesh = m_staticMeshes.find(_resourceID);
		if (itMesh != m_staticMeshes.end())
		{
			return itMesh->second;
		}
		// Get assetID corresponding to resource.
		auto itResource = m_resources.find(_resourceID);
		if (itResource == m_resources.end())
		{
			return ResourceHandle<StaticMesh>::invalid();
		}
		AssetID assetID = itResource->second;
		// Get assetInfo
		auto itAsset = m_assets.find(itResource->second);
		if (itAsset == m_assets.end())
		{
			return ResourceHandle<StaticMesh>::invalid();
		}
		AssetInfo assetInfo = itAsset->second;
		auto mesh = std::make_shared<StaticMesh>();
		ArchiveStaticMesh archive(assetID);
		ArchiveLoadResult res = archive.load(this, assetInfo.path);
		AKA_ASSERT(res == ArchiveLoadResult::Success, "Failed loading");

		auto it = m_staticMeshes.insert(std::make_pair(_resourceID, ResourceHandle<StaticMesh>(ResourceState::Loaded)));
		if (it.second)
		{
			ResourceHandle<StaticMesh> handle = it.first->second;
			handle.get().create(Application::app()->graphic(), archive);
			return handle;
		}
		else
		{
			AKA_UNREACHABLE;
			return ResourceHandle<StaticMesh>::invalid();
		}
	}

	ResourceRange<Scene> getSceneRange() { return ResourceRange<Scene>(m_scenes); }
	ResourceRange<StaticMesh> getStaticMeshRange() { return ResourceRange<StaticMesh>(m_staticMeshes); }
	AssetRange getAssetRange() { return AssetRange(m_assets); }

private:
	std::map<AssetID, AssetInfo> m_assets;
	std::map<ResourceID, AssetID> m_resources;
private:
	std::map<ResourceID, ResourceHandle<StaticMesh>> m_staticMeshes;
	std::map<ResourceID, ResourceHandle<Scene>> m_scenes;
};


}