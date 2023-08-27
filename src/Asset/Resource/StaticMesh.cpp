#include "StaticMesh.hpp"

namespace app {



void StaticMesh::create(gfx::GraphicDevice* _device, const ArchiveStaticMesh& _archive)
{
	Vector<Vertex> vertices;
	Vector<uint32_t> indices;
	for (const ArchiveBatch& batch : _archive.batches)
	{
		this->batches.append(DrawCallIndexed{
			(uint32_t)vertices.size(),
			(uint32_t)indices.size(),
			(uint32_t)batch.geometry.indices.size()
		});
		this->materials.append(Material{});
		vertices.append(batch.geometry.vertices);
		indices.append(batch.geometry.indices);
	}
	this->attributes.add(gfx::VertexSemantic::Position, gfx::VertexFormat::Float, gfx::VertexType::Vec3).add(gfx::VertexSemantic::TexCoord0, gfx::VertexFormat::Float, gfx::VertexType::Vec2);
	this->gfxVertexBuffer = _device->createBuffer("VertexBuffer", gfx::BufferType::Vertex, sizeof(Vertex) * vertices.size(), gfx::BufferUsage::Default, gfx::BufferCPUAccess::None, vertices.data());
	this->gfxIndexBuffer = _device->createBuffer("IndexBuffer", gfx::BufferType::Index, sizeof(uint32_t) * indices.size(), gfx::BufferUsage::Default, gfx::BufferCPUAccess::None, indices.data());;
}

void StaticMesh::destroy(gfx::GraphicDevice* _device)
{
	_device->destroy(this->gfxIndexBuffer);
	_device->destroy(this->gfxVertexBuffer);
	this->attributes = gfx::VertexAttributeState{};
	this->batches.clear();
	this->materials.clear();
}

}