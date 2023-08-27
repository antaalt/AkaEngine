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

class AssetLibrary {
public:
	// parse library.json & store AssetInfo with AssetID
	void parse();
	// serialize library.json 
	void serialize();
public:
	// For importer.
	ResourceID registerAsset(const ArchivePath& _path, AssetType _assetType) {
		AssetID assetID = generateAssetIDFromAssetPath(AssetPath{ _path.m_path });
		auto itAsset = m_assets.insert(std::make_pair(assetID, AssetInfo{ AssetPath{ _path.m_path }, _assetType }));
		if (!itAsset.second)
			return ResourceID(-1);

		auto itAsset2 = m_assets.insert(std::make_pair(assetID, AssetInfo{ AssetPath{ _path.m_path }, _assetType }));
		ResourceID resourceID = generateResourceIDFromAssetID(assetID);

		auto itResource = m_resources.insert(std::make_pair(resourceID, assetID));
		if (!itResource.second)
			return ResourceID(-1);

		// TODO add check for type which read file (to check it exist, & check type is valid)

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