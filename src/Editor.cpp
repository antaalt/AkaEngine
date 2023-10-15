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
	assets()->parse();
}

void Editor::onDestroy()
{
}

void Editor::onFixedUpdate(aka::Time time)
{
	if (platform()->keyboard().pressed(KeyboardKey::Escape))
		EventDispatcher<QuitEvent>::emit();

	if (m_scene.isLoaded())
	{
		m_scene.get().getRootNode().fixedUpdate(time);
	}
}

void Editor::onUpdate(aka::Time time)
{
	program()->reloadIfChanged(graphic());

	
	if (m_scene.isLoaded())
	{
		// Disable camera inputs if ImGui uses them.
		const ImGuiIO& io = ImGui::GetIO();
		CameraComponent& camera = m_editorCameraNode->get<CameraComponent>();
		camera.setUpdateEnabled(!io.WantCaptureMouse && !io.WantCaptureKeyboard);

		// Update scene
		m_scene.get().getRootNode().update(time);
	}
}


void Editor::onRender(gfx::GraphicDevice* device, gfx::FrameHandle _frame)
{
	if (m_scene.isLoaded())
	{
		m_scene.get().getRootNode().update(assets(), renderer());
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
		Scene& scene = m_scene.get();
		m_sceneID = scene.getID();
		// Setup editor camera.
		m_editorCameraNode = scene.createChild(nullptr, "EditorCamera");
		ArchiveCameraComponent component{};
		component.projectionType = CameraProjectionType::Perpective;
		component.controllerType = CameraControllerType::Arcball;
		CameraComponent& camera = m_editorCameraNode->attach<CameraComponent>();
		camera.fromArchive(component);
		camera.getController()->set(scene.getBounds());
	}
}

