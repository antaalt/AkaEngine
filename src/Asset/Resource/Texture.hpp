#pragma once 

#include "../Archive/Archive.hpp"
#include "../Archive/ArchiveImage.hpp"
#include "Resource.hpp"

namespace app {

class Texture : public Resource {
public:
	Texture();
	Texture(ResourceID _id, const String& _name);

	void create(AssetLibrary* _library, gfx::GraphicDevice* _device, const Archive& _archive) override;
	void save(AssetLibrary* _library, gfx::GraphicDevice* _device, Archive& _archive) override;
	void destroy(AssetLibrary* _library, gfx::GraphicDevice* _device) override;

	uint32_t getWidth() const { return m_width; }
	uint32_t getHeight() const { return m_height; }
	gfx::TextureHandle getGfxHandle() const { return m_textureHandle; }
	gfx::TextureType getTextureType() const { return m_textureType; }
	gfx::TextureUsage getTextureUsage() const { return m_textureUsage; }
	gfx::TextureFormat getTextureFormat() const { return m_textureFormat; }
private:
	uint32_t m_width, m_height;
	gfx::TextureHandle m_textureHandle;
	gfx::TextureType m_textureType;
	gfx::TextureUsage m_textureUsage;
	gfx::TextureFormat m_textureFormat;
};


}