#pragma once

#include "AssetViewer.hpp"
#include <Aka/Resource/Resource/Texture.hpp>

namespace app {

class TextureViewer : public AssetViewer<aka::Texture>
{
public:
	TextureViewer(aka::AssetID _assetID, aka::ResourceHandle<aka::Texture> _resourceHandle);
	~TextureViewer();

	void onCreate(aka::gfx::GraphicDevice* _device) override;
	void onDestroy(aka::gfx::GraphicDevice* _device) override;
	void onUpdate(aka::Time deltaTime) override;
	void onRender(aka::gfx::GraphicDevice* _device, aka::gfx::Frame* frame) override;
	void onLoad(const aka::Texture& texture) override;
protected:
	void drawUIResource(const aka::Texture& texture) override;
private:
	aka::gfx::DescriptorSetHandle m_descriptorSet;
	aka::gfx::SamplerHandle m_sampler;
	bool m_needUpdate = false;
	int m_layerSelected = 0;
	int m_mipSelected = 0;
	aka::gfx::ColorMask m_mask = aka::gfx::ColorMask::Rgba;
	float m_zoom = 1.f;
};

};