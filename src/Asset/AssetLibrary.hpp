#pragma once

#include "Resource/Resource.hpp"
#include "Resource/StaticMesh.hpp"
#include "Resource/Scene.hpp"
#include "Asset.hpp"

#include <map>

namespace app {

inline ResourceID generateResourceIDFromAssetID(AssetID id)
{
	return ResourceID(id);
}

template <typename T>
struct ResourceIterator {

private:
	using Iterator = typename std::map<ResourceID, T>::iterator;
	static_assert(std::is_base_of<T, Resource>::value, "This should inherit Resource");
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
	std::pair<ResourceID, T> operator*() { return *m_value; }
	bool operator==(const ResourceIterator<T>& value) { return value.m_value == m_value; }
	bool operator!=(const ResourceIterator<T>& value) { return value.m_value != m_value; }
private:
	Iterator m_value;
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

class AssetLibrary {
public:
	// parse library.json & store AssetInfo with AssetID
	void parse();
	// serialize library.json 
	void serialize();
public:
	// For importer.
	ResourceID registerScene(const ArchivePath& path);
	ResourceID registerStaticMesh(const ArchivePath& path) {
		AssetID assetID = generateAssetIDFromAssetPath(AssetPath{ path.m_path });
		auto itAsset = m_assets.insert(std::make_pair(assetID, AssetInfo{ AssetPath{ path.m_path }, AssetType::StaticMesh }));
		ResourceID resourceID = generateResourceIDFromAssetID(assetID);
		auto itResource = m_resources.insert(std::make_pair(resourceID, assetID));

		return resourceID;
	}
public:
	// Function will check if asset exist
	ResourceHandle<Scene> getScene(ResourceID _asset);
	ResourceHandle<StaticMesh> getStaticMesh(ResourceID _resourceID) {
		auto itResource = m_resources.find(_resourceID);
		if (itResource == m_resources.end())
		{
			return ResourceHandle<StaticMesh>::invalid();
		}
		auto itAsset = m_assets.find(itResource->second);
		if (itAsset == m_assets.end())
		{
			return ResourceHandle<StaticMesh>::invalid();
		}
		auto mesh = std::make_shared<StaticMesh>();
		ArchiveStaticMesh archive(ArchivePath(itAsset->second.path.path));
		ArchiveLoadResult res = archive.load(archive.getPath());
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
			AKA_NOT_IMPLEMENTED;
			return ResourceHandle<StaticMesh>::invalid();
		}
	}

	ResourceIterator<Scene> getSceneIterator();
	ResourceIterator<StaticMesh> getStaticMeshIterator();
	AssetIterator getAssetIterator();

private:
	std::map<AssetID, AssetInfo> m_assets;
	std::map<ResourceID, AssetID> m_resources;
private:
	std::map<ResourceID, ResourceHandle<StaticMesh>> m_staticMeshes;
	std::map<ResourceID, ResourceHandle<Scene>> m_scenes;
};


}