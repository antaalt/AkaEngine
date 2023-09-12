#pragma once

#include <Aka/Core/Layer.h>
#include <Aka/Graphic/GraphicDevice.h>

#include "AssetViewer.hpp"
#include "../Asset/AssetLibrary.hpp"

namespace app {

class AssetLibrary;

struct SceneSwitchEvent
{
	ResourceHandle<Scene> scene;
};

struct AssetNode;

class AssetEditorLayer : public aka::Layer, EventListener<AssetAddedEvent>
{
public:
	AssetEditorLayer();
	~AssetEditorLayer();

	void onLayerCreate() override;
	void onLayerDestroy() override;

	void onLayerUpdate(Time deltaTime) override;
	void onLayerFrame() override;
	void onLayerRender(aka::gfx::Frame* frame) override;
	void onLayerPresent() override;
	void onLayerResize(uint32_t width, uint32_t height) override;
	void onReceive(const AssetAddedEvent& event) override;
public:
	void setLibrary(AssetLibrary* _library);
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
	MeshViewer m_meshViewer;
	Vector<AssetViewerBase*> m_viewers;
	AssetLibrary* m_library;
};

};