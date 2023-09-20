#pragma once

#include <Aka/Graphic/GraphicDevice.h>
#include <Aka/Scene/Camera.h>
#include <Aka/Core/Container/Vector.h>

#include "SceneViewer.hpp"
#include "StaticMeshViewer.hpp"
#include "TextureViewer.hpp"

namespace app {

class AssetViewerManager
{
public:
	AssetViewerManager() {}
	~AssetViewerManager();
	void onCreate(aka::gfx::GraphicDevice* _device);
	void onDestroy(aka::gfx::GraphicDevice* _device);
	void onUpdate(aka::Time deltaTime);
	void onRender(aka::gfx::GraphicDevice* _device, aka::gfx::Frame* frame);
	void render(aka::gfx::Frame* frame);

	template <typename T>
	void open(ResourceID _resourceID, ResourceHandle<T>& _resourceHandle);
private:
	Vector<AssetViewerBase*> m_assetViewersToCreate;
	Vector<AssetViewerBase*> m_assetViewersToDestroy;
	Vector<AssetViewerBase*> m_assetViewers;
};

template<> void AssetViewerManager::open(ResourceID _resourceID, ResourceHandle<Scene>& _resourceHandle);
template<> void AssetViewerManager::open(ResourceID _resourceID, ResourceHandle<Texture>& _resourceHandle);
template<> void AssetViewerManager::open(ResourceID _resourceID, ResourceHandle<StaticMesh>& _resourceHandle);


};