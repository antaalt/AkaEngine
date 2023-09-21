#pragma once

#include <Aka/Core/Layer.h>
#include <Aka/Graphic/GraphicDevice.h>
#include <Aka/Resource/AssetLibrary.hpp>

#include "AssetViewer/AssetViewerManager.hpp"

#include <functional>

namespace app {

struct SceneSwitchEvent
{
	aka::ResourceHandle<aka::SceneAvecUnNomChelou> scene;
};

struct AssetNode;

class AssetEditorLayer : public aka::Layer, aka::EventListener<aka::AssetAddedEvent>
{
public:
	AssetEditorLayer();
	~AssetEditorLayer();

	void onLayerCreate(aka::gfx::GraphicDevice* _device) override;
	void onLayerDestroy(aka::gfx::GraphicDevice* _device) override;

	void onLayerUpdate(aka::Time deltaTime) override;
	void onLayerFrame() override;
	void onLayerRender(aka::gfx::Frame* frame) override;
	void onLayerPresent() override;
	void onLayerResize(uint32_t width, uint32_t height) override;
	void onReceive(const aka::AssetAddedEvent& event) override;
public:
	void setLibrary(aka::AssetLibrary* _library);
private:
	void importDeferred(std::function<bool(const aka::Path&)> callback);
private:
	aka::Path m_currentPath;
	aka::Path* m_selectedPath;
	std::vector<aka::Path> m_paths;
	std::function<bool(const aka::Path& path)> m_importCallback;
private:
	AssetNode* m_rootNode;
	bool m_assetUpdated = false;
	AssetViewerManager m_viewerManager;
	aka::AssetLibrary* m_library;
};

};