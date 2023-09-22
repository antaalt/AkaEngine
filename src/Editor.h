#pragma once

#include <Aka/Core/Application.h>
#include <Aka/Scene/Camera.h>
#include <Aka/Graphic/GraphicDevice.h>

#include <Aka/Resource/Asset.hpp>
#include <Aka/Resource/Archive/ArchiveStaticMesh.hpp>
#include <Aka/Resource/Archive/ArchiveScene.hpp>
#include <Aka/Resource/Resource/StaticMesh.hpp>
#include <Aka/Resource/Resource/Scene.hpp>
#include <Aka/Resource/AssetLibrary.hpp>

#include "Editor/AssetBrowserEditorLayer.hpp"

using namespace aka;

class Editor : public aka::Application, 
	EventListener<app::SceneSwitchEvent>,
	EventListener<ShaderReloadedEvent>
{
public:
	Editor();
	void onCreate(int argc, char* argv[]) override;
	void onDestroy() override;
	void onFixedUpdate(aka::Time time) override;
	void onUpdate(aka::Time time) override;
	void onRender(gfx::GraphicDevice* _device, aka::gfx::Frame* frame) override;
	void onResize(uint32_t width, uint32_t height) override;
	void onReceive(const app::SceneSwitchEvent& event) override;
	void onReceive(const ShaderReloadedEvent& event) override;
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
	ProgramKey m_programKey;
private:
	bool m_dirty = false;
	anglef m_rotation = anglef::radian(0.f);
	CameraArcball m_cameraController = {};
	CameraPerspective m_cameraProjection = {};

private:
	ResourceID m_sceneID;
	ResourceHandle<Scene> m_scene;
};
