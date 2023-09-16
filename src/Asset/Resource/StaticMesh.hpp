#pragma once 

#include "../Archive/Archive.hpp"
#include "../Archive/ArchiveStaticMesh.hpp"
#include "Resource.hpp"
#include "Texture.hpp"

namespace app {

class StaticMesh : public Resource {
public:
	StaticMesh() : Resource(ResourceType::StaticMesh) {}
	StaticMesh(ResourceID _id, const String& _name) : Resource(ResourceType::StaticMesh, _id, _name) {}


	void create(AssetLibrary* _library, gfx::GraphicDevice* _device, const Archive& _archive) override;
	void save(AssetLibrary* _library, gfx::GraphicDevice* _device, Archive& _archive) override;
	void destroy(AssetLibrary* _library, gfx::GraphicDevice* _device) override;

public: // Optionnal data for runtime operations
	gfx::VertexAttributeState attributes;
public: // Mandatory data for rendering & co
	// Should be mutualized in a big single geometry buffer with pages.
	gfx::BufferHandle gfxVertexBuffer;
	gfx::BufferHandle gfxIndexBuffer;
	gfx::SamplerHandle gfxAlbedoSampler;
	gfx::SamplerHandle gfxNormalSampler;

	struct DrawCallIndexed {
		uint32_t vertexOffset;
		uint32_t indexOffset;
		uint32_t indexCount;
		// TODO mutualize texture to avoid copies
		ResourceHandle<Texture> m_albedo;
		ResourceHandle<Texture> m_normal;
		gfx::BufferHandle gfxUniformBuffer;
		gfx::DescriptorSetHandle gfxDescriptorSet;
	};
	Vector<DrawCallIndexed> batches; // TODO indirect buffer. Require bindless for material
};


// Component could inherit from StaticMesh directly ?
struct StaticMeshComponent {
	ResourceHandle<StaticMesh> mesh;
	// Instance data.
	gfx::BufferHandle instanceBuffer;
	gfx::DescriptorSetHandle descriptorSet;
};


}