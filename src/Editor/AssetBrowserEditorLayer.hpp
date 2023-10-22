#pragma once

#include <Aka/Core/Layer.h>
#include <Aka/Graphic/GraphicDevice.h>
#include <Aka/Resource/AssetLibrary.hpp>

#include "EditorLayer.hpp"
#include "Modal/ImportModal.hpp"
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

	void onCreate(aka::Renderer* _renderer) override;
	void onDestroy(aka::Renderer* _renderer) override;

	void onUpdate(aka::Time deltaTime) override;
	void onPreRender() override;
	void onRender(aka::Renderer* _renderer, aka::gfx::FrameHandle frame) override;
	void onPostRender() override;
	void onResize(uint32_t width, uint32_t height) override;
	void onReceive(const aka::AssetAddedEvent& event) override;
	void onDrawUI(aka::DebugDrawList& debugDrawList) override;
public:
	void setLibrary(aka::AssetLibrary* _library);
	void setAssetViewer(AssetViewerEditorLayer* _viewer);
private:
	ImGuiImportModal m_importer;
	AssetNode* m_rootNode;
	const AssetNode* m_currentNode = nullptr;
	bool m_assetUpdated = false;
	AssetViewerEditorLayer* m_viewerEditor;
	aka::AssetLibrary* m_library;
};

};