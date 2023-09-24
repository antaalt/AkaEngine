#pragma once

#include <Aka/Graphic/GraphicDevice.h>
#include <Aka/Scene/Component/CameraComponent.hpp>
#include <Aka/Core/Container/Vector.h>
#include <Aka/Core/Layer.h>

#include "EditorLayer.hpp"
#include "AssetViewer/SceneViewer.hpp"
#include "AssetViewer/StaticMeshViewer.hpp"
#include "AssetViewer/TextureViewer.hpp"

namespace app {

class AssetViewerEditorLayer : public EditorLayer
{
public:
	AssetViewerEditorLayer();
	~AssetViewerEditorLayer();

	void onCreate(aka::gfx::GraphicDevice* _device) override;
	void onDestroy(aka::gfx::GraphicDevice* _device) override;
	void onUpdate(aka::Time deltaTime) override;
	void onRender(aka::gfx::GraphicDevice* _device, aka::gfx::Frame* frame) override;
	void onDrawUI();

	template <typename T>
	void open(aka::AssetID _assetID, aka::ResourceHandle<T>& _resourceHandle);
private:
	aka::Vector<AssetViewerBase*> m_assetViewersToCreate;
	aka::Vector<AssetViewerBase*> m_assetViewersToDestroy;
	aka::Vector<AssetViewerBase*> m_assetViewers;
};

template<> void AssetViewerEditorLayer::open(aka::AssetID _assetID, aka::ResourceHandle<aka::Scene>& _resourceHandle);
template<> void AssetViewerEditorLayer::open(aka::AssetID _assetID, aka::ResourceHandle<aka::Texture>& _resourceHandle);
template<> void AssetViewerEditorLayer::open(aka::AssetID _assetID, aka::ResourceHandle<aka::StaticMesh>& _resourceHandle);


};