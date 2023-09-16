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
	m_textureHandle = _device->createTexture(
		"AlbedoTexture", 
		imageArchive.width, imageArchive.height, 1, 
		gfx::TextureType::Texture2D,
		gfx::Sampler::mipLevelCount(imageArchive.width, imageArchive.height), 1,
		gfx::TextureFormat::RGBA8, 
		gfx::TextureUsage::ShaderResource | gfx::TextureUsage::GenerateMips,
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