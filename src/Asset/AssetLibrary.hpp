#pragma once

#include "Resource/Resource.hpp"
#include "Resource/StaticMesh.hpp"
#include "Resource/Scene.hpp"
#include "Asset.hpp"

#include "Archive/ArchiveScene.hpp"

#include <map>

namespace app {

template <typename T>
class ResourceIterator
{
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
class ResourceRange
{
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


ResourceType getResourceType(AssetType _type);

class AssetLibrary {
public:
	// parse library.json & store AssetInfo with AssetID
	// Or parse whole folder looking for asset, with headers, we can id them
	void parse();
	// serialize library.json 
	void serialize();
public:
	AssetID registerAsset(const AssetPath& _path, AssetType _assetType);
	ResourceID getResourceID(AssetID _assetID) const;
	AssetInfo& getAssetInfo(AssetID _id);

public:
	template <typename T>
	ResourceHandle<T> get(ResourceID _resourceID);
	ResourceHandle<Scene> getScene(ResourceID _resourceID, gfx::GraphicDevice* _device = nullptr);
	ResourceHandle<StaticMesh> getStaticMesh(ResourceID _resourceID, gfx::GraphicDevice* _device = nullptr);

	void destroy(gfx::GraphicDevice* _device);

public:
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


template<typename T>
inline ResourceHandle<T> AssetLibrary::get(ResourceID _resourceID)
{
	return ResourceHandle<T>();
}

}