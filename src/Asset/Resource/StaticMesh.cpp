#include "StaticMesh.hpp"

namespace app {

struct MaterialUniformBuffer {
	color4f color;
};


void StaticMesh::create(gfx::GraphicDevice* _device, const ArchiveStaticMesh& _archive)
{
	// TODO mutualize sampler in Renderer class (& pass renderer instead of device as argument, or renderer create class)
	this->gfxAlbedoSampler = _device->createSampler(
		"Sampler",
		gfx::Filter::Linear, gfx::Filter::Linear,
		gfx::SamplerMipMapMode::Linear,
		gfx::SamplerAddressMode::Repeat, gfx::SamplerAddressMode::Repeat, gfx::SamplerAddressMode::Repeat,
		1.0
	);
	this->gfxNormalSampler = _device->createSampler(
		"Sampler",
		gfx::Filter::Linear, gfx::Filter::Linear,
		gfx::SamplerMipMapMode::Linear,
		gfx::SamplerAddressMode::Repeat, gfx::SamplerAddressMode::Repeat, gfx::SamplerAddressMode::Repeat,
		1.0
	);
	Vector<Vertex> vertices;
	Vector<uint32_t> indices;
	for (const ArchiveBatch& batch : _archive.batches)
	{
		// TODO should retrieve this from shader somehow...
		gfx::ShaderBindingState bindings{};
		bindings.add(gfx::ShaderBindingType::UniformBuffer, gfx::ShaderMask::Vertex | gfx::ShaderMask::Fragment, 1);
		bindings.add(gfx::ShaderBindingType::SampledImage, gfx::ShaderMask::Fragment, 1);
		bindings.add(gfx::ShaderBindingType::SampledImage, gfx::ShaderMask::Fragment, 1);
		// Material
		// TODO mips
		const void* albedoData = batch.material.albedo.data.data();
		gfx::TextureHandle gfxAlbedoTexture = _device->createTexture("AlbedoTexture", batch.material.albedo.width, batch.material.albedo.height, 1, gfx::TextureType::Texture2D, 1, 1, gfx::TextureFormat::RGBA8, gfx::TextureUsage::ShaderResource, &albedoData);
		const void* normalData = batch.material.normal.data.data();
		gfx::TextureHandle gfxNormalTexture = _device->createTexture("AlbedoTexture", batch.material.normal.width, batch.material.normal.height, 1, gfx::TextureType::Texture2D, 1, 1, gfx::TextureFormat::RGBA8, gfx::TextureUsage::ShaderResource, &normalData);

		MaterialUniformBuffer ubo{};
		ubo.color = batch.material.color;
		gfx::BufferHandle gfxUniformBuffer = _device->createBuffer("MaterialUniformBuffer", gfx::BufferType::Uniform, sizeof(MaterialUniformBuffer), gfx::BufferUsage::Default, gfx::BufferCPUAccess::None, &ubo);
		
		gfx::DescriptorSetHandle gfxDescriptorSet = _device->createDescriptorSet("DescriptorSetMaterial", bindings);

		gfx::DescriptorSetData data;
		data.addUniformBuffer(gfxUniformBuffer);
		data.addSampledImage(gfxAlbedoTexture, this->gfxAlbedoSampler);
		data.addSampledImage(gfxNormalTexture, this->gfxNormalSampler);
		_device->update(gfxDescriptorSet, data);
		
		this->batches.append(DrawCallIndexed{
			(uint32_t)vertices.size(),
			(uint32_t)indices.size(),
			(uint32_t)batch.geometry.indices.size(),
			gfxAlbedoTexture,
			gfxNormalTexture,
			gfxUniformBuffer,
			gfxDescriptorSet,
		});

		vertices.append(batch.geometry.vertices);
		indices.append(batch.geometry.indices);
	}
	this->attributes.add(gfx::VertexSemantic::Position, gfx::VertexFormat::Float, gfx::VertexType::Vec3).add(gfx::VertexSemantic::TexCoord0, gfx::VertexFormat::Float, gfx::VertexType::Vec2);
	this->gfxVertexBuffer = _device->createBuffer("VertexBuffer", gfx::BufferType::Vertex, (uint32_t)(sizeof(Vertex) * vertices.size()), gfx::BufferUsage::Default, gfx::BufferCPUAccess::None, vertices.data());
	this->gfxIndexBuffer = _device->createBuffer("IndexBuffer", gfx::BufferType::Index, (uint32_t)(sizeof(uint32_t) * indices.size()), gfx::BufferUsage::Default, gfx::BufferCPUAccess::None, indices.data());;
}

void StaticMesh::destroy(gfx::GraphicDevice* _device)
{
	_device->destroy(this->gfxAlbedoSampler);
	_device->destroy(this->gfxNormalSampler);
	_device->destroy(this->gfxIndexBuffer);
	_device->destroy(this->gfxVertexBuffer);
	for (const DrawCallIndexed& batch : batches)
	{
		_device->destroy(batch.gfxAlbedoTexture);
		_device->destroy(batch.gfxNormalTexture);
		_device->destroy(batch.gfxDescriptorSet);
		_device->destroy(batch.gfxUniformBuffer);
	}
	this->attributes = gfx::VertexAttributeState{};
	this->batches.clear();
}

}