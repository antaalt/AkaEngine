#pragma once

#include <Aka/Core/Application.h>
#include <Aka/Scene/Camera.h>
#include <Aka/Graphic/GraphicDevice.h>

#include "Asset/Asset.hpp"
#include "Asset/Archive/ArchiveStaticMesh.hpp"
#include "Asset/Archive/ArchiveScene.hpp"
#include "Asset/Resource/StaticMesh.hpp"
#include "Asset/Resource/Scene.hpp"
#include "Asset/AssetLibrary.hpp"
#include "Editor/AssetEditorLayer.hpp"

using namespace aka;

class Editor : public aka::Application, EventListener<app::SceneSwitchEvent>
{
public:
	Editor();
	void onCreate(int argc, char* argv[]) override;
	void onDestroy() override;
	void onFixedUpdate(aka::Time time) override;
	void onUpdate(aka::Time time) override;
	void onRender(aka::gfx::Frame* frame) override;
	void onResize(uint32_t width, uint32_t height) override;
	void onReceive(const app::SceneSwitchEvent& event) override;
private:
	void createRenderPass();
	void destroyRenderPass();
private:
	gfx::RenderPassHandle m_renderPass;
	gfx::BackbufferHandle m_backbuffer;
	gfx::GraphicPipelineHandle m_renderPipeline;
	gfx::BufferHandle m_cameraUniformBuffer;
	gfx::DescriptorSetHandle m_cameraDescriptorSet;
	gfx::ProgramHandle m_program;
private:
	bool m_dirty = false;
	anglef m_rotation = anglef::radian(0.f);
	CameraArcball m_cameraController = {};
	CameraPerspective m_cameraProjection = {};

private:
	app::AssetLibrary m_library;
	app::ResourceID m_sceneID;
	app::ResourceHandle<app::Scene> m_scene;
};
