#pragma once

#include <Aka/Core/Layer.h>
#include <Aka/Graphic/GraphicDevice.h>
#include <Aka/Resource/AssetLibrary.hpp>

#include "EditorLayer.hpp"
#include "AssetViewerEditorLayer.hpp"

#include <functional>

namespace app {

struct SceneSwitchEvent
{
	aka::ResourceHandle<aka::Scene> scene;
};

struct AssetNode;

class AssetBrowserEditorLayer : 
	public EditorLayer,
	aka::EventListener<aka::AssetAddedEvent>
{
public:
	AssetBrowserEditorLayer();
	~AssetBrowserEditorLayer();

	void onCreate(aka::gfx::GraphicDevice* _device) override;
	void onDestroy(aka::gfx::GraphicDevice* _device) override;

	void onUpdate(aka::Time deltaTime) override;
	void onFrame() override;
	void onRender(aka::gfx::GraphicDevice* _device, aka::gfx::Frame* frame) override;
	void onPresent() override;
	void onResize(uint32_t width, uint32_t height) override;
	void onReceive(const aka::AssetAddedEvent& event) override;
	void onDrawUI() override;
public:
	void setLibrary(aka::AssetLibrary* _library);
	void setAssetViewer(AssetViewerEditorLayer* _viewer);
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
	AssetViewerEditorLayer* m_viewerEditor;
	aka::AssetLibrary* m_library;
};

};