#pragma once

#include <Aka/Core/Application.h>
#include <Aka/Scene/Camera.h>
#include <Aka/Graphic/GraphicDevice.h>

using namespace aka;

class Editor : public aka::Application
{
public:
	Editor();
	void onCreate(int argc, char* argv[]) override;
	void onDestroy() override;
	void onFixedUpdate(aka::Time time) override;
	void onUpdate(aka::Time time) override;
	void onRender(aka::gfx::Frame* frame) override;
private:
	gfx::RenderPassHandle m_renderPass;
	gfx::BackbufferHandle m_backbuffer;
	gfx::GraphicPipelineHandle m_renderPipeline;
	gfx::BufferHandle m_vertices;
	gfx::BufferHandle m_uniformBuffer;
	gfx::TextureHandle m_texture;
	gfx::SamplerHandle m_sampler;
	gfx::DescriptorSetHandle m_descriptorSet;

private:
	bool m_dirty = false;
	anglef m_rotation = anglef::radian(0.f);
	CameraArcball m_cameraController = {};
	CameraPerspective m_cameraProjection = {};
};
