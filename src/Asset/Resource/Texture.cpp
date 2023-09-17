#include "Texture.hpp"

namespace app {

Texture::Texture() : 
	Resource(ResourceType::Texture),
	m_textureHandle(gfx::TextureHandle::null)
{
}

Texture::Texture(ResourceID _id, const String& _name) : 
	Resource(ResourceType::Texture, _id, _name),
	m_textureHandle(gfx::TextureHandle::null)
{
}

void Texture::create(AssetLibrary* _library, gfx::GraphicDevice* _device, const Archive& _archive)
{
	AKA_ASSERT(_archive.type() == AssetType::Image, "Invalid archive");
	const ArchiveImage& imageArchive = reinterpret_cast<const ArchiveImage&>(_archive);
	
	// TODO custom mips
	const void* data = imageArchive.data.data();
	m_width = imageArchive.width;
	m_height = imageArchive.height;
	m_textureType = gfx::TextureType::Texture2D;
	m_textureFormat = gfx::TextureFormat::RGBA8;
	m_textureUsage = gfx::TextureUsage::ShaderResource | gfx::TextureUsage::GenerateMips;
	m_textureHandle = _device->createTexture(
		"AlbedoTexture", 
		imageArchive.width, imageArchive.height, 1, 
		m_textureType,
		gfx::Sampler::mipLevelCount(imageArchive.width, imageArchive.height), 1,
		m_textureFormat,
		m_textureUsage,
		&data
	);
}

void Texture::save(AssetLibrary* _library, gfx::GraphicDevice* _device, Archive& _archive)
{
	AKA_NOT_IMPLEMENTED;
}

void Texture::destroy(AssetLibrary* _library, gfx::GraphicDevice* _device)
{
	_device->destroy(m_textureHandle);
}

}