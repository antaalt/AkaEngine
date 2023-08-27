#pragma once 

#include "../Archive/Archive.hpp"
#include "../Archive/ArchiveStaticMesh.hpp"
#include "Resource.hpp"

namespace app {

struct StaticMesh : Resource {
public:
	void create(gfx::GraphicDevice* _device, const ArchiveStaticMesh& _archive);
	void destroy(gfx::GraphicDevice* _device);

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
		gfx::TextureHandle gfxAlbedoTexture;
		gfx::TextureHandle gfxNormalTexture;
		gfx::BufferHandle gfxUniformBuffer;
		gfx::DescriptorSetHandle gfxDescriptorSet;
	};
	Vector<DrawCallIndexed> batches; // TODO indirect buffer. Require bindless for material
};

struct StaticMeshComponent {
	ResourceHandle<StaticMesh> mesh;
};


}