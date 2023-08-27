#pragma once

#include "Resource/Resource.hpp"
#include "Resource/StaticMesh.hpp"
#include "Resource/Scene.hpp"
#include "Asset.hpp"

#include <map>

namespace app {

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
	ResourceID addScene(const AssetPath& path);
	ResourceID addStaticMesh(const AssetPath& path);
public:
	// Function will check if asset exist
	ResourceHandle<Scene> getScene(ResourceID _asset);
	ResourceHandle<StaticMesh> getStaticMesh(ResourceID _asset);

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