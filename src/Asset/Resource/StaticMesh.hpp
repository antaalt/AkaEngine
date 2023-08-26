#pragma once 

namespace app {

struct Vertex {
	float position[3];
	float uv[2];
	float color[4];
};


// All these will depend on renderer interface.
// .geo   -> File holding raw batch geometry
// .mat   -> File holding material data
// .batch -> File holding reference to geo & mat
// .mesh  -> File holding reference to a mesh containing multiple batches.
// .smesh -> File holding reference to multiple mesh depending on lod count 
// .skel  -> File holding animation & bones data
// .dmesh -> Same as smesh but with animation data (bones, rigging...)
struct ArchiveGeometry : Archive {
	void load(const Path& path) override;
	void save(const Path& path) override;
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	aabbox<> bounds;
};
struct ArchiveMaterial : Archive {
	void load(const Path& path) override;
	void save(const Path& path) override;
};
struct ArchiveBatch : Archive {
	ArchiveMaterial material;
	ArchiveGeometry geometry;
	void load(const Path& path) override;
	void save(const Path& path) override;
};
struct ArchiveMesh : Archive {
	std::vector<ArchiveBatch> batches;
	void load(const Path& path) override;
	void save(const Path& path) override;
};
struct ArchiveStaticMesh : Archive {
	// LOD will be generated at runtime, maybe cache it somewhere for perf...
	// Use simplygon or mesh opt...
	ArchiveMesh mesh;
	void load(const Path& path) override;
	void save(const Path& path) override;
};
struct ArchiveDynamicMesh : Archive {
	ArchiveMesh mesh;
	void load(const Path& path) override;
	void save(const Path& path) override;
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
	// Could be the resource directly ?
	StaticMesh mesh;
};


}