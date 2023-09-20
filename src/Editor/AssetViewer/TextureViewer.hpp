#pragma once

#include "AssetViewer.hpp"
#include "../../Asset/Resource/Texture.hpp"

namespace app {

class TextureViewer : public AssetViewer<Texture>
{
public:
	TextureViewer(ResourceID _resourceID, ResourceHandle<Texture> _resourceHandle);
	~TextureViewer();

	void onCreate(gfx::GraphicDevice* _device) override;
	void onDestroy(gfx::GraphicDevice* _device) override;
	void onUpdate(aka::Time deltaTime) override;
	void onRender(gfx::GraphicDevice* _device, aka::gfx::Frame* frame) override;
	void onLoad(const Texture& texture) override;
protected:
	void drawUIResource(const Texture& texture) override;
private:
	aka::gfx::DescriptorSetHandle m_descriptorSet;
	aka::gfx::SamplerHandle m_sampler;
	bool m_needUpdate = false;
	int m_layerSelected = 0;
	int m_mipSelected = 0;
	gfx::ColorMask m_mask = gfx::ColorMask::Rgba;
	float m_zoom = 1.f;
};

};