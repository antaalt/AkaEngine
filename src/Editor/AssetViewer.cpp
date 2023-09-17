#include "AssetViewer.hpp"

#include <Aka/Aka.h>

#include <Aka/Layer/ImGuiLayer.h>

namespace app {

using namespace aka;

//template class AssetViewer<Texture>;
template class AssetViewer<StaticMesh>;

const char* toString(gfx::VertexFormat format)
{
	switch (format)
	{
	case gfx::VertexFormat::Float:
		return "float";
	case gfx::VertexFormat::Double:
		return "double";
	case gfx::VertexFormat::Byte:
		return "byte";
	case gfx::VertexFormat::UnsignedByte:
		return "unsigned byte";
	case gfx::VertexFormat::Short:
		return "short";
	case gfx::VertexFormat::UnsignedShort:
		return "unsigned short";
	case gfx::VertexFormat::Int:
		return "int";
	case gfx::VertexFormat::UnsignedInt:
		return "unsigned int";
	default:
		return "unknown";
	}
}

const char* toString(gfx::IndexFormat format)
{
	switch (format)
	{
	case gfx::IndexFormat::UnsignedByte:
		return "unsigned byte";
	case gfx::IndexFormat::UnsignedShort:
		return "unsigned short";
	case gfx::IndexFormat::UnsignedInt:
		return "unsigned int";
	default:
		return "unknown";
	}
}

const char* toString(gfx::VertexType type)
{
	switch (type)
	{
	case gfx::VertexType::Vec2:
		return "vec2";
	case gfx::VertexType::Vec3:
		return "vec3";
	case gfx::VertexType::Vec4:
		return "vec4";
	case gfx::VertexType::Mat2:
		return "mat2";
	case gfx::VertexType::Mat3:
		return "mat3";
	case gfx::VertexType::Mat4:
		return "mat4";
	case gfx::VertexType::Scalar:
		return "scalar";
	default:
		return "unknown";
	}
}

const char* toString(gfx::VertexSemantic semantic)
{
	switch (semantic)
	{
	case gfx::VertexSemantic::Position:
		return "position";
	case gfx::VertexSemantic::Normal:
		return "normal";
	case gfx::VertexSemantic::Tangent:
		return "tangent";
	case gfx::VertexSemantic::TexCoord0:
		return "texcoord0";
	case gfx::VertexSemantic::TexCoord1:
		return "texcoord1";
	case gfx::VertexSemantic::TexCoord2:
		return "texcoord2";
	case gfx::VertexSemantic::TexCoord3:
		return "texcoord3";
	case gfx::VertexSemantic::Color0:
		return "color0";
	case gfx::VertexSemantic::Color1:
		return "color1";
	case gfx::VertexSemantic::Color2:
		return "color2";
	case gfx::VertexSemantic::Color3:
		return "color3";
	default:
		return "unknown";
	}
}

const char* toString(gfx::TextureFormat format)
{
	switch (format)
	{
	case gfx::TextureFormat::R8:
		return "R8";
	case gfx::TextureFormat::R8U:
		return "R8U";
	case gfx::TextureFormat::R16:
		return "R16";
	case gfx::TextureFormat::R16U:
		return "R16U";
	case gfx::TextureFormat::R16F:
		return "R16F";
	case gfx::TextureFormat::R32F:
		return "R32F";
	case gfx::TextureFormat::RG8:
		return "RG8";
	case gfx::TextureFormat::RG8U:
		return "RG8U";
	case gfx::TextureFormat::RG16U:
		return "RG16U";
	case gfx::TextureFormat::RG16:
		return "RG16";
	case gfx::TextureFormat::RG16F:
		return "RG16F";
	case gfx::TextureFormat::RG32F:
		return "RG32F";
	case gfx::TextureFormat::RGB8:
		return "RGB8";
	case gfx::TextureFormat::RGB8U:
		return "RGB8U";
	case gfx::TextureFormat::RGB16:
		return "RGB16";
	case gfx::TextureFormat::RGB16U:
		return "RGB16U";
	case gfx::TextureFormat::RGB16F:
		return "RGB16F";
	case gfx::TextureFormat::RGB32F:
		return "RGB32F";
	case gfx::TextureFormat::RGBA8:
		return "RGBA8";
	case gfx::TextureFormat::RGBA8U:
		return "RGBA8U";
	case gfx::TextureFormat::RGBA16:
		return "RGBA16";
	case gfx::TextureFormat::RGBA16U:
		return "RGBA16U";
	case gfx::TextureFormat::RGBA16F:
		return "RGBA16F";
	case gfx::TextureFormat::RGBA32F:
		return "RGBA32F";
	case gfx::TextureFormat::Depth:
		return "Depth";
	case gfx::TextureFormat::Depth16:
		return "Depth16";
	case gfx::TextureFormat::Depth24:
		return "Depth24";
	case gfx::TextureFormat::Depth32:
		return "Depth32";
	case gfx::TextureFormat::Depth32F:
		return "Depth32F";
	case gfx::TextureFormat::DepthStencil:
		return "DepthStencil";
	case gfx::TextureFormat::Depth0Stencil8:
		return "Depth0Stencil8";
	case gfx::TextureFormat::Depth24Stencil8:
		return "Depth24Stencil8";
	case gfx::TextureFormat::Depth32FStencil8:
		return "Depth32FStencil8";
	default:
		return "Unknown";
	}
}

const char* toString(gfx::TextureType type)
{
	switch (type)
	{
	case gfx::TextureType::Texture2D:
		return "Texture2D";
	case gfx::TextureType::Texture2DMultisample:
		return "Texture2DMultisample";
	case gfx::TextureType::TextureCubeMap:
		return "TextureCubemap";
	default:
		return "Unknown";
	}
}

const char* toString(gfx::BufferType type)
{
	switch (type)
	{
	case gfx::BufferType::Vertex:
		return "VertexBuffer";
	case gfx::BufferType::Index:
		return "IndexBuffer";
	default:
		return "Unknown";
	}
}

const char* toString(gfx::BufferUsage usage)
{
	switch (usage)
	{
	case gfx::BufferUsage::Staging:
		return "Staging";
	case gfx::BufferUsage::Immutable:
		return "Immutable";
	case gfx::BufferUsage::Dynamic:
		return "Dynamic";
	case gfx::BufferUsage::Default:
		return "Default";
	default:
		return "Unknown";
	}
}

const char* toString(gfx::BufferCPUAccess access)
{
	switch (access)
	{
	case gfx::BufferCPUAccess::Read:
		return "Read";
	case gfx::BufferCPUAccess::ReadWrite:
		return "ReadWrite";
	case gfx::BufferCPUAccess::Write:
		return "Write";
	case gfx::BufferCPUAccess::None:
		return "None";
	default:
		return "Unknown";
	}
}
struct MeshViewerUBO {
	mat4f model;
	mat4f view;
	mat4f proj;
	mat4f normal;
};

MeshViewer::MeshViewer() :
	AssetViewer("Mesh"),
	m_needCameraUpdate(false)
{
}

void MeshViewer::create(gfx::GraphicDevice* _device)
{
	// TODO if no data, bug ?
	std::vector<uint8_t> data(m_width * m_height * 4, 0xff);
	void* dataVoid = data.data();
	m_renderTarget = _device->createTexture("MeshViewerRT", m_width, m_height, 1, gfx::TextureType::Texture2D, 1, 1, gfx::TextureFormat::RGBA8, gfx::TextureUsage::RenderTarget | gfx::TextureUsage::ShaderResource, &dataVoid);
	m_depthTarget = _device->createTexture("MeshViewerDepthRT", m_width, m_height, 1, gfx::TextureType::Texture2D, 1, 1, gfx::TextureFormat::Depth, gfx::TextureUsage::RenderTarget);
	gfx::Attachment color = { m_renderTarget, gfx::AttachmentFlag::None, 0, 0 };
	gfx::Attachment depth = { m_depthTarget, gfx::AttachmentFlag::None, 0, 0 };
	gfx::RenderPassState rpState{};
	rpState.addColor(gfx::TextureFormat::RGBA8, gfx::AttachmentLoadOp::Clear, gfx::AttachmentStoreOp::Store, gfx::ResourceAccessType::Attachment, gfx::ResourceAccessType::Resource);
	rpState.setDepth(gfx::TextureFormat::Depth);
	m_renderPass = _device->createRenderPass("MeshViewerRenderPass", rpState);
	m_target = _device->createFramebuffer("MeshViewerFramebuffer", m_renderPass, &color, 1, &depth);

	const aka::ShaderKey ShaderVertex = aka::ShaderKey().setPath(app::AssetPath("../shaders/editor/basic.vert").getAbsolutePath()).setType(aka::ShaderType::Vertex);
	const aka::ShaderKey ShaderFragment = aka::ShaderKey().setPath(app::AssetPath("../shaders/editor/basic.frag").getAbsolutePath()).setType(aka::ShaderType::Fragment);

	const aka::ProgramKey ProgramGraphic = aka::ProgramKey().add(ShaderVertex).add(ShaderFragment);
	ShaderRegistry* program = Application::app()->program();
	program->add(ProgramGraphic, _device);
	gfx::ProgramHandle p = program->get(ProgramGraphic);
	m_pipeline = _device->createGraphicPipeline(
		"MeshViewerPipeline",
		p,
		gfx::PrimitiveType::Triangles,
		rpState,
		Vertex::getState(),
		gfx::ViewportState{}.size(m_width, m_height),
		gfx::DepthStateDefault,
		gfx::StencilStateDefault,
		gfx::CullStateDefault,
		gfx::BlendStateDefault,
		gfx::FillStateLine
	);

	m_descriptorSet = _device->createDescriptorSet("MeshViewerDescriptorSet", _device->get(p)->sets[0]);
	{
		m_arcball.set(aabbox<>(point3f(-20.f), point3f(20.f)));
		m_projection.nearZ = 0.1f;
		m_projection.farZ = 1000.f;
		m_projection.ratio = m_width / (float)m_height;
		m_projection.hFov = anglef::degree(90.f);

		MeshViewerUBO ubo{};
		ubo.model = mat4f::identity();
		ubo.view = m_arcball.view();
		ubo.proj = m_projection.projection();
		ubo.normal = mat4f::transpose(mat4f::inverse(ubo.view * ubo.model));
		m_uniform = _device->createBuffer("MeshViewerUBO", gfx::BufferType::Uniform, sizeof(MeshViewerUBO), gfx::BufferUsage::Default, gfx::BufferCPUAccess::None, &ubo);

		gfx::DescriptorSetData desc;
		desc.addUniformBuffer(m_uniform);
		_device->update(m_descriptorSet, desc);
	}

	{
		gfx::ShaderBindingState state{};
		state.add(gfx::ShaderBindingType::SampledImage, gfx::ShaderMask::Fragment);
		m_imguiDescriptorSet = _device->createDescriptorSet("ImGuiMeshViewerDescriptorSet", state);
		m_imguiSampler = _device->createSampler("ImGuiTextureViewerSampler",
			gfx::Filter::Nearest, gfx::Filter::Nearest,
			gfx::SamplerMipMapMode::None,
			gfx::SamplerAddressMode::Repeat, gfx::SamplerAddressMode::Repeat, gfx::SamplerAddressMode::Repeat,
			1.0);
		gfx::DescriptorSetData desc;
		desc.addSampledTexture2D(m_renderTarget, m_imguiSampler);
		_device->update(m_imguiDescriptorSet, desc);
	}
}
void MeshViewer::destroy(gfx::GraphicDevice* _device)
{
	_device->destroy(m_imguiDescriptorSet);
	_device->destroy(m_descriptorSet);
	_device->destroy(m_uniform);
	_device->destroy(m_target);
	_device->destroy(m_renderTarget);
	_device->destroy(m_depthTarget);
	_device->destroy(m_renderPass);
}
void MeshViewer::update(gfx::GraphicDevice* _device, aka::Time deltaTime)
{
	// This is run one frame late though...
	if (m_needCameraUpdate)
	{
		_device->wait();
		m_arcball.update(deltaTime);
		MeshViewerUBO ubo{};
		ubo.model = mat4f::identity();
		ubo.view = m_arcball.view();
		ubo.proj = m_projection.projection();
		ubo.normal = mat4f::transpose(mat4f::inverse(ubo.view*ubo.model));
		_device->upload(m_uniform, &ubo, 0, sizeof(MeshViewerUBO));
		m_needCameraUpdate = false;
	}
}
void MeshViewer::onResourceChange()
{	
	m_arcball.set(m_resource.get().getBounds());
}

void MeshViewer::drawMesh(const StaticMesh& mesh)
{
	gfx::GraphicDevice* device = Application::app()->graphic();

	gfx::CommandList* cmd = device->acquireCommandList(gfx::QueueType::Graphic); // TODO use standard queue...
	cmd->begin();

	cmd->transition(m_renderTarget, gfx::ResourceAccessType::Resource, gfx::ResourceAccessType::Attachment);
	cmd->beginRenderPass(m_renderPass, m_target, gfx::ClearState{ gfx::ClearMask::All, { 0.1f, 0.1f, 0.1f, 1.f}, 1.f, 0 });

	cmd->bindPipeline(m_pipeline);
	cmd->bindIndexBuffer(mesh.gfxIndexBuffer, mesh.getIndexFormat());
	cmd->bindVertexBuffer(mesh.gfxVertexBuffer, 0);
	cmd->bindDescriptorSet(0, m_descriptorSet);

	for (const auto& batch : mesh.batches)
	{
		// TODO material
		cmd->drawIndexed(batch.indexCount, batch.indexOffset, batch.vertexOffset, 1);
	}
	cmd->endRenderPass();

	cmd->end();
	device->submit(cmd);
}

void MeshViewer::draw(const String& name, const StaticMesh& mesh)
{
	ImGui::TextColored(ImGuiLayer::Color::red, name.cstr());
	ImGui::Text("Vertices");
	for (uint32_t i = 0; i < mesh.attributes.count; i++)
	{
		char buffer[256];
		snprintf(buffer, 256, "Attribute %u", i);
		if (ImGui::TreeNode(buffer))
		{
			ImGui::BulletText("Format : %s", toString(mesh.attributes.attributes[i].format));
			ImGui::BulletText("Semantic : %s", toString(mesh.attributes.attributes[i].semantic));
			ImGui::BulletText("Type : %s", toString(mesh.attributes.attributes[i].type));
			ImGui::BulletText("Offset : %u", mesh.attributes.offsets[i]);
			ImGui::TreePop();
		}
	}
	ImGui::Separator();
	ImGui::Text("Indices");
	ImGui::BulletText("Format : %s", toString(mesh.getIndexFormat()));
	ImGui::Separator();

	// Mesh viewer
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	if (ImGui::BeginChild("MeshDisplayChild", ImVec2(0.f, (float)m_height + 5.f), false, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize))
	{
		if (ImGui::IsWindowHovered() && (ImGui::IsMouseDragging(0, 0.0f) || ImGui::IsMouseDragging(1, 0.0f) || !ImGui::IsAnyItemActive()))
		{
			m_needCameraUpdate = true;
		}
		drawMesh(mesh);
		gfx::GraphicDevice* device = Application::app()->graphic();
		ImTextureID texID = ImGuiLayer::getTextureID(device, m_imguiDescriptorSet);
		ImGui::Image(texID, ImVec2((float)m_width, (float)m_height), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1));
	}
	ImGui::EndChild();
	ImGui::PopStyleVar();
}

void TextureViewer::draw(const String& name, const Texture& texture)
{
	ImGui::TextColored(ImGuiLayer::Color::red, name.cstr());
	if (m_needUpdate)
	{
		ImGui::Text("Texture need update");
		return;
	}
	gfx::TextureHandle handle = texture.getGfxHandle();
	bool isRenderTarget = (texture.getTextureUsage() & gfx::TextureUsage::RenderTarget) == gfx::TextureUsage::RenderTarget;
	bool isShaderResource = (texture.getTextureUsage() & gfx::TextureUsage::ShaderResource) == gfx::TextureUsage::ShaderResource;
	bool hasMips = (texture.getTextureUsage() & gfx::TextureUsage::GenerateMips) == gfx::TextureUsage::GenerateMips;
	ImGui::Text("%s - %u x %u (%s)", toString(texture.getTextureType()), texture.getWidth() >> m_mipSelected, texture.getHeight() >> m_mipSelected, toString(texture.getTextureFormat()));
	ImGui::Checkbox("Render target", &isRenderTarget); ImGui::SameLine();
	ImGui::Checkbox("Shader resource", &isShaderResource); ImGui::SameLine();
	ImGui::Checkbox("Mips", &hasMips);

	ImGui::Separator();
	static bool red = true;
	static bool green = true;
	static bool blue = true;
	static bool alpha = true;
	static int zoomInt = 100;
	static const int minZoom = 1;
	static const int maxZoom = 500;
	static ImVec2 scrollPosition = ImVec2(0.f, 0.f);
	ImGui::Checkbox("R", &red); ImGui::SameLine();
	ImGui::Checkbox("G", &green); ImGui::SameLine();
	ImGui::Checkbox("B", &blue); ImGui::SameLine();
	ImGui::Checkbox("A", &alpha); ImGui::SameLine();
	bool zoomChanged = ImGui::DragInt("##Zoom", &zoomInt, 1, minZoom, maxZoom, "%d %%");

	ImGui::Separator();
	m_needUpdate |= ImGui::SliderInt("Layer", &m_layerSelected, 0, texture.getLayerCount() - 1);
	m_needUpdate |= ImGui::SliderInt("Mip", &m_mipSelected, 0, texture.getMipCount() - 1);

	// Image explorer
	gfx::GraphicDevice* device = Application::app()->graphic();
	ImTextureID textureID = ImGuiLayer::getTextureID(device, m_descriptorSet);
	ImVec4 mask = ImVec4(
		red ? 1.f : 0.f,
		green ? 1.f : 0.f,
		blue ? 1.f : 0.f,
		alpha ? 1.f : 0.f
	);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	if (ImGui::BeginChild("TextureDisplayChild", ImVec2(0, 0), true, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar))
	{
		if (!zoomChanged && ImGui::IsWindowHovered() && !ImGui::IsAnyItemActive() && ImGui::GetIO().MouseWheel != 0.f)
		{
			zoomInt = clamp(zoomInt + (int)(ImGui::GetIO().MouseWheel * 2.f), minZoom, maxZoom);
		}
		float zoom = zoomInt / 100.f;
		if (ImGui::IsWindowHovered() && (ImGui::IsMouseDragging(0, 0.0f) || ImGui::IsMouseDragging(1, 0.0f)))
		{
			ImVec2 windowSize = ImGui::GetWindowSize();
			ImVec2 maxScroll(
				zoom * texture.getWidth() - windowSize.x,
				zoom * texture.getHeight() - windowSize.y
			);
			scrollPosition.x = fminf(fmaxf(scrollPosition.x - ImGui::GetIO().MouseDelta.x, 0.f), maxScroll.x);
			scrollPosition.y = fminf(fmaxf(scrollPosition.y - ImGui::GetIO().MouseDelta.y, 0.f), maxScroll.y);
		}
		ImGui::SetScrollX(scrollPosition.x);
		ImGui::SetScrollY(scrollPosition.y);
		ImGui::Image(textureID, ImVec2(zoom * texture.getWidth(), zoom * texture.getHeight()), ImVec2(0, 0), ImVec2(1, 1), mask);
	}
	ImGui::EndChild();
	ImGui::PopStyleVar();
}

void TextureViewer::onResourceChange()
{
	m_needUpdate = true;
}

TextureViewer::TextureViewer() :
	AssetViewer("Texture"),
	m_descriptorSet(gfx::DescriptorSetHandle::null),
	m_sampler(gfx::SamplerHandle::null),
	m_needUpdate(false),
	m_layerSelected(0),
	m_mipSelected(0)
{
}

void TextureViewer::create(gfx::GraphicDevice* _device)
{
	gfx::ShaderBindingState state{};
	state.add(gfx::ShaderBindingType::SampledImage, gfx::ShaderMask::Fragment);

	m_descriptorSet = _device->createDescriptorSet("ImGuiTextureViewerDescriptorSet", state);
	m_sampler = _device->createSampler("ImGuiTextureViewerSampler",
		gfx::Filter::Nearest, gfx::Filter::Nearest,
		gfx::SamplerMipMapMode::None,
		gfx::SamplerAddressMode::Repeat, gfx::SamplerAddressMode::Repeat, gfx::SamplerAddressMode::Repeat,
		1.0);
	if (m_resource.isLoaded())
	{
		gfx::DescriptorSetData data;
		data.addSampledTexture2D(m_resource.get().getGfxHandle(), m_sampler, m_layerSelected, m_mipSelected);
		_device->update(m_descriptorSet, data);
		m_needUpdate = false;
	}
	else
	{
		m_needUpdate = true; // wait for a texture to be available
	}
}

void TextureViewer::destroy(gfx::GraphicDevice* _device)
{
	_device->destroy(m_descriptorSet);
	_device->destroy(m_sampler);
}

void TextureViewer::update(gfx::GraphicDevice* _device, aka::Time deltaTime)
{
	if (m_resource.isLoaded() && m_needUpdate)
	{
		_device->wait();
		gfx::DescriptorSetData data;
		data.addSampledTexture2D(m_resource.get().getGfxHandle(), m_sampler, m_layerSelected, m_mipSelected);
		_device->update(m_descriptorSet, data);
		m_needUpdate = false;
	}
}

};
