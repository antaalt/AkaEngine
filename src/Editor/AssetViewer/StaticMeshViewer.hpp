#pragma once

#include "AssetViewer.hpp"
#include "../../Asset/Resource/StaticMesh.hpp"

namespace app {
class StaticMeshViewer : public AssetViewer<StaticMesh>
{
public:
	StaticMeshViewer(ResourceID _resourceID, ResourceHandle<StaticMesh> _resourceHandle);
	~StaticMeshViewer();

	void onCreate(gfx::GraphicDevice* _device) override;
	void onDestroy(gfx::GraphicDevice* _device) override;
	void onUpdate(aka::Time deltaTime) override;
	void onRender(gfx::GraphicDevice* _device, aka::gfx::Frame* frame) override;
	void onLoad(const StaticMesh& mesh) override;
protected:
	void drawUIResource(const StaticMesh& mesh) override;
	void renderMesh(gfx::Frame* frame, const StaticMesh& mesh);
private:
	const uint32_t m_width = 512;
	const uint32_t m_height = 512;
	aka::gfx::TextureHandle m_renderTarget;
	aka::gfx::TextureHandle m_depthTarget;
	aka::gfx::FramebufferHandle m_target;
	aka::gfx::RenderPassHandle m_renderPass;
	aka::gfx::GraphicPipelineHandle m_pipeline;
	aka::gfx::DescriptorSetHandle m_descriptorSet;
	aka::gfx::DescriptorSetHandle m_imguiDescriptorSet;
	aka::gfx::SamplerHandle m_imguiSampler;
	aka::gfx::BufferHandle m_uniform;
	aka::CameraArcball m_arcball;
	aka::CameraPerspective m_projection;
	bool m_needCameraUpdate;
};

};