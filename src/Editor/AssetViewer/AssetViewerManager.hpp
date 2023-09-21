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
	void open(aka::ResourceID _resourceID, aka::ResourceHandle<T>& _resourceHandle);
private:
	aka::Vector<AssetViewerBase*> m_assetViewersToCreate;
	aka::Vector<AssetViewerBase*> m_assetViewersToDestroy;
	aka::Vector<AssetViewerBase*> m_assetViewers;
};

template<> void AssetViewerManager::open(aka::ResourceID _resourceID, aka::ResourceHandle<aka::Scene>& _resourceHandle);
template<> void AssetViewerManager::open(aka::ResourceID _resourceID, aka::ResourceHandle<aka::Texture>& _resourceHandle);
template<> void AssetViewerManager::open(aka::ResourceID _resourceID, aka::ResourceHandle<aka::StaticMesh>& _resourceHandle);


};