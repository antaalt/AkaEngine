#pragma once

#include "AssetViewer.hpp"
#include <Aka/Resource/Resource/StaticMesh.hpp>
#include <Aka/Scene/Component/CameraComponent.hpp>

namespace app {
class StaticMeshViewer : public AssetViewer<aka::StaticMesh>
{
public:
	StaticMeshViewer(aka::AssetID _assetID, aka::ResourceHandle<aka::StaticMesh> _resourceHandle);
	~StaticMeshViewer();

	void onCreate(aka::gfx::GraphicDevice* _device) override;
	void onDestroy(aka::gfx::GraphicDevice* _device) override;
	void onUpdate(aka::Time deltaTime) override;
	void onRender(aka::gfx::GraphicDevice* _device, aka::gfx::FrameHandle frame) override;
	void onLoad(const aka::StaticMesh& mesh) override;
protected:
	void drawUIResource(const aka::StaticMesh& mesh) override;
	void renderMesh(aka::gfx::FrameHandle frame, const aka::StaticMesh& mesh);
private:
	const uint32_t m_width = 512;
	const uint32_t m_height = 512;
	aka::gfx::TextureHandle m_renderTarget;
	aka::gfx::TextureHandle m_depthTarget;
	aka::gfx::FramebufferHandle m_target;
	aka::gfx::RenderPassHandle m_renderPass;
	aka::gfx::GraphicPipelineHandle m_pipeline;
	aka::gfx::DescriptorPoolHandle m_descriptorPool;
	aka::gfx::DescriptorSetHandle m_descriptorSet[aka::gfx::MaxFrameInFlight];
	aka::gfx::DescriptorPoolHandle m_imguiDescriptorPool;
	aka::gfx::DescriptorSetHandle m_imguiDescriptorSet;
	aka::gfx::SamplerHandle m_imguiSampler;
	aka::gfx::BufferHandle m_uniform[aka::gfx::MaxFrameInFlight];
	aka::CameraArcball m_arcball;
	aka::CameraPerspective m_projection;
	bool m_needCameraUpdate[aka::gfx::MaxFrameInFlight] = { false };
};

};