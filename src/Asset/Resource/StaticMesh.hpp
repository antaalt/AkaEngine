#pragma once 

#include "../Archive/Archive.hpp"
#include "Resource.hpp"

namespace app {



// All these will depend on renderer interface.
// .geo   -> File holding raw batch geometry
// .mat   -> File holding material data
// .batch -> File holding reference to geo & mat
// .mesh  -> File holding reference to a mesh containing multiple batches.
// .smesh -> File holding reference to multiple mesh depending on lod count 
// .skel  -> File holding animation & bones data
// .dmesh -> Same as smesh but with animation data (bones, rigging...)

struct ArchiveDynamicMesh : Archive {
	std::vector<ArchiveBatch> batches;
	ArchiveLoadResult load(const ArchivePath& path) override {}
	ArchiveSaveResult save(const ArchivePath& path) override {}
};

// This could be directly component for ECS
struct StaticMesh : Resource {
public:
	void create(gfx::GraphicDevice* _device, const ArchiveStaticMesh& _archive);
	void destroy(gfx::GraphicDevice* _device, const ArchiveStaticMesh& _archive);

public: // Optionnal data for runtime operations
	gfx::VertexAttributeState attributes;
public: // Mandatory data for rendering & co
	// Should be mutualized in a big single geometry buffer with pages.
	gfx::BufferHandle gfxVertexBuffer;
	gfx::BufferHandle gfxIndexBuffer;
};

struct StaticMeshComponent {
	ResourceHandle<StaticMesh> mesh;
};


}