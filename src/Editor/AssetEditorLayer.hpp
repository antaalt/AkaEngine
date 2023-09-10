#pragma once

#include <Aka/Core/Layer.h>
#include <Aka/Graphic/GraphicDevice.h>

#include "AssetViewer.hpp"

namespace app {

class AssetLibrary;

class AssetEditorLayer : public aka::Layer {
public:
	AssetEditorLayer();

	void onLayerCreate() override;
	void onLayerDestroy() override;

	void onLayerUpdate(Time deltaTime) override;
	void onLayerFrame() override;
	void onLayerRender(aka::gfx::Frame* frame) override;
	void onLayerPresent() override;
	void onLayerResize(uint32_t width, uint32_t height) override;
public:
	void setLibrary(AssetLibrary* _library);
private:
	void import(std::function<bool(const aka::Path&)> callback);
private:
	aka::Path m_currentPath;
	aka::Path* m_selectedPath;
	std::vector<aka::Path> m_paths;
	std::function<bool(const aka::Path& path)> m_importCallback;
private:
	MeshViewer m_meshViewer;
	Vector<AssetViewerBase*> m_viewers;
	AssetLibrary* m_library;
};

};