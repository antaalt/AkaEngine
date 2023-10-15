#pragma once

#include <Aka/Core/Application.h>
#include <Aka/Scene/Component/CameraComponent.hpp>
#include <Aka/Graphic/GraphicDevice.h>

#include <Aka/Resource/Asset.hpp>
#include <Aka/Resource/Archive/ArchiveStaticMesh.hpp>
#include <Aka/Resource/Archive/ArchiveScene.hpp>
#include <Aka/Resource/Resource/StaticMesh.hpp>
#include <Aka/Resource/Resource/Scene.hpp>
#include <Aka/Resource/AssetLibrary.hpp>
#include <Aka/Renderer/Renderer.hpp>

#include "Editor/AssetBrowserEditorLayer.hpp"

using namespace aka;

class Editor : public aka::Application, 
	EventListener<app::SceneSwitchEvent>
{
public:
	Editor(const Config& cfg);
	~Editor();
	void onCreate(int argc, char* argv[]) override;
	void onDestroy() override;
	void onFixedUpdate(aka::Time time) override;
	void onUpdate(aka::Time time) override;
	void onRender(gfx::GraphicDevice* _device, aka::gfx::FrameHandle frame) override;
	void onResize(uint32_t width, uint32_t height) override;
	void onReceive(const app::SceneSwitchEvent& event) override;
private:
	Node* m_editorCameraNode;
private:
	AssetID m_sceneID;
	ResourceHandle<Scene> m_scene;
};
