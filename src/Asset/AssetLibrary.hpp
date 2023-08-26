#pragma once

#include "Resource/Resource.hpp"
#include "Asset.hpp"

#include <map>

namespace app {

template <typename T>
struct ResourceIterator {

private:
	using Iterator = std::map<ResourceID, T>::iterator;
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

class AssetLibrary {
public:
	// parse library.json & store AssetInfo with AssetID
	void parse();
	// serialize library.json 
	void serialize();
public:
	// For importer.
	void addImage(const AssetPath& path);
	void addStaticMesh(const AssetPath& path);
public:
	// Function will check if asset exist
	ResourceHandle<Scene> getScene(ResourceID _asset);
	ResourceHandle<StaticMesh> getStaticMesh(ResourceID _asset);

	ResourceIterator<Image> getImageIterator();
	ResourceIterator<Scene> getSceneIterator();
	//AssetIterator getAssetIterator();

private:
	std::map<AssetID, AssetInfo> m_assets;
	std::map<ResourceID, AssetID> m_resources;
private:
	std::map<ResourceID, ResourceHandle<StaticMesh>> m_staticMeshes;
	std::map<ResourceID, ResourceHandle<Scene>> m_scenes;
};


void test() {
	AssetLibrary library;
	{
		ResourceID scene = ResourceID{}; // This is hardcoded cause we cant retrieve it somewhere
		ResourceHandle<Scene> sceneHandle = library.getScene(scene);
		if (sceneHandle.isLoaded())
		{
			Scene& scene = sceneHandle.get();
		}
	}

	{
		// IMPORTER
		struct Importer {
			// Might need path for GLTF
			AssetPath import(const Path& path);
			// Might need this for memory (or could write temp file to use up, but redundant operation
			AssetPath import(const Blob& blob);
		};
		Importer importer;
		AssetPath path = importer.import("mypath/to/desitionatn.png");
		library.addImage(path);
	}
}


}