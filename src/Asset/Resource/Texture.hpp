#pragma once 

#include "../Archive/Archive.hpp"
#include "../Archive/ArchiveImage.hpp"
#include "Resource.hpp"

namespace app {

struct TextureDescription
{

};

class Texture : public Resource {
public:
	Texture();
	Texture(ResourceID _id, const String& _name);

	void create(AssetLibrary* _library, gfx::GraphicDevice* _device, const Archive& _archive) override;
	void save(AssetLibrary* _library, gfx::GraphicDevice* _device, Archive& _archive) override;
	void destroy(AssetLibrary* _library, gfx::GraphicDevice* _device) override;

	gfx::TextureHandle getGfxHandle() const { return m_textureHandle; }
private:
	gfx::TextureHandle m_textureHandle;
};


}