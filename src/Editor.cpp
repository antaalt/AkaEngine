#include "Editor.h"

#include <Aka/Core/Container/String.h>
#include <Aka/OS/OS.h>
#include <Aka/Layer/ImGuiLayer.h>
#include <Aka/Scene/Node.hpp>

#include <imgui.h>
#include <imguizmo.h>
#include <map>
#include <vector>

#include "Importer/AssimpImporter.hpp"
#include "Editor/AssetBrowserEditorLayer.hpp"
#include "Editor/SceneEditorLayer.hpp"
#include "Editor/InfoEditorLayer.hpp"
#include "Component/CustomComponent.hpp"
#include "Component/RotatorComponent.hpp"

using namespace aka;

Editor::Editor(const Config& cfg) :
	Application(cfg)
{
	ImGuiLayer* imgui = getRoot().addLayer<ImGuiLayer>();
	// Editor are child of imgui to support frame
	app::SceneEditorLayer*			sceneEditor			= imgui->addLayer<app::SceneEditorLayer>();
	app::AssetViewerEditorLayer*	assetViewerEditor	= imgui->addLayer<app::AssetViewerEditorLayer>();
	app::AssetBrowserEditorLayer*	assetBrowserEditor	= imgui->addLayer<app::AssetBrowserEditorLayer>();
	app::InfoEditorLayer*			infoEditor			= imgui->addLayer<app::InfoEditorLayer>();
	// Set dependencies
	sceneEditor->setCurrentScene(m_scene);
	sceneEditor->setCurrentCameraController(&m_cameraController);
	sceneEditor->setCurrentCameraProjection(&m_cameraProjection);
	sceneEditor->setLibrary(assets());
	assetBrowserEditor->setLibrary(assets());
	assetBrowserEditor->setAssetViewer(assetViewerEditor);
	infoEditor->setEditorLayer(app::EditorLayerType::AssetBrowser, assetBrowserEditor);
	infoEditor->setEditorLayer(app::EditorLayerType::AssetViewer, assetViewerEditor);
	infoEditor->setEditorLayer(app::EditorLayerType::SceneEditor, sceneEditor);
	AKA_REGISTER_COMPONENT(CustomComponent);
	AKA_REGISTER_COMPONENT(RotatorComponent);
}

Editor::~Editor()
{
	AKA_UNREGISTER_COMPONENT(CustomComponent);
	AKA_UNREGISTER_COMPONENT(RotatorComponent);
}

void Editor::onCreate(int argc, char* argv[])
{
	m_cameraController.set(aabbox(point3(-1.f), point3(1.f)));
	m_cameraProjection.hFov = anglef::degree(60.f);
	m_cameraProjection.ratio = width() / (float)height();
	m_cameraProjection.nearZ = 0.1f;
	m_cameraProjection.farZ = 10000.f;

	m_view = renderer()->createView(ViewType::Color);
	assets()->parse();
}

void Editor::onDestroy()
{
	gfx::GraphicDevice* device = graphic();
	renderer()->destroyView(m_view);
}

void Editor::onFixedUpdate(aka::Time time)
{
	if (platform()->keyboard().down(KeyboardKey::Escape))
		EventDispatcher<QuitEvent>::emit();

	if (m_scene.isLoaded())
	{
		m_scene.get().getRoot().fixedUpdate(time);
	}
}

void Editor::onUpdate(aka::Time time)
{
	program()->reloadIfChanged(graphic());

	const ImGuiIO& io = ImGui::GetIO();
	if (!io.WantCaptureMouse && !io.WantCaptureKeyboard)
	{
		if (m_cameraController.update(time))
		{
			m_dirty = true;
		}
	}
	if (m_scene.isLoaded())
	{
		m_scene.get().getRoot().update(time);
	}
}


void Editor::onRender(gfx::GraphicDevice* device, gfx::FrameHandle _frame)
{
	if (m_scene.isLoaded())
	{
		m_scene.get().getRoot().update(assets(), renderer());
	}
	gfx::CommandList* cmd = device->getGraphicCommandList(_frame);

	if (m_dirty)
	{
		renderer()->updateView(
			m_view,
			m_cameraController.view(),
			m_cameraProjection.projection()
		);
		m_dirty = false;
	}
}

void Editor::onResize(uint32_t width, uint32_t height)
{
}

void Editor::onReceive(const app::SceneSwitchEvent& event)
{
	m_scene = event.scene;
	if (event.scene.isLoaded())
	{
		const Scene& scene = event.scene.get();
		m_sceneID = scene.getID();
		m_cameraController.set(scene.getBounds());
		m_dirty = true;
	}
}

