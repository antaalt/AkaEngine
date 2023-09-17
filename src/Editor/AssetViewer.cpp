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

MeshViewer::MeshViewer() :
	AssetViewer("Mesh")
{
}

void MeshViewer::create()
{
	gfx::GraphicDevice* device = Application::app()->graphic();
	// TODO if no data, bug ?
	std::vector<uint8_t> data(m_width * m_height * 4, 0xff);
	void* dataVoid = data.data();
	m_renderTarget = device->createTexture("MeshViewerRT", m_width, m_height, 1, gfx::TextureType::Texture2D, 1, 1, gfx::TextureFormat::RGBA8, gfx::TextureUsage::RenderTarget | gfx::TextureUsage::ShaderResource, &dataVoid);
	m_depthTarget = device->createTexture("MeshViewerDepthRT", m_width, m_height, 1, gfx::TextureType::Texture2D, 1, 1, gfx::TextureFormat::Depth, gfx::TextureUsage::RenderTarget);
	gfx::Attachment color = { m_renderTarget, gfx::AttachmentFlag::None, 0, 0 };
	gfx::Attachment depth = { m_depthTarget, gfx::AttachmentFlag::None, 0, 0 };
	m_renderPass = device->createRenderPass("MeshViewerRenderPass", gfx::RenderPassState{}.addColor(gfx::TextureFormat::RGBA8).setDepth(gfx::TextureFormat::Depth));
	m_target = device->createFramebuffer("MeshViewerFramebuffer", m_renderPass, &color, 1, &depth);


	const aka::ShaderKey ShaderVertex = aka::ShaderKey().setPath(app::AssetPath("../shaders/editor/basic.vert").getAbsolutePath()).setType(aka::ShaderType::Vertex);
	const aka::ShaderKey ShaderFragment = aka::ShaderKey().setPath(app::AssetPath("../shaders/editor/basic.frag").getAbsolutePath()).setType(aka::ShaderType::Fragment);

	const aka::ProgramKey ProgramGraphic = aka::ProgramKey().add(ShaderVertex).add(ShaderFragment);
	ShaderRegistry* program = Application::app()->program();
	program->add(ProgramGraphic, device);
	gfx::ProgramHandle p = program->get(ProgramGraphic);
	m_descriptorSet = device->createDescriptorSet("MeshViewerDescriptorSet", device->get(p)->sets[0]);
	m_uniform = device->createBuffer("MeshViewerUBO", gfx::BufferType::Uniform, sizeof(mat4f), gfx::BufferUsage::Default, gfx::BufferCPUAccess::None);
	//m_descriptorSet->setUniformBuffer(0, m_uniform);
	// TODO update set
	m_arcball.set(aabbox<>(point3f(-20.f), point3f(20.f)));
	m_projection = mat4f::perspective(anglef::degree(90.f), m_width / (float)m_height, 0.1f, 100.f);
}
void MeshViewer::destroy()
{
	gfx::GraphicDevice* device = Application::app()->graphic();
	device->destroy(m_descriptorSet);
	device->destroy(m_uniform);
	device->destroy(m_target);
	device->destroy(m_renderTarget);
	device->destroy(m_depthTarget);
	device->destroy(m_renderPass);
}
void MeshViewer::update(aka::Time deltaTime)
{

}
void MeshViewer::onResourceChange()
{
	// TODO compute mesh bounds from mesh
	m_arcball.set(aabbox<>(point3f(-20.f), point3f(20.f)));
}

void MeshViewer::drawMesh(const StaticMesh* mesh)
{
}

void MeshViewer::draw(const String& name, ResourceHandle<StaticMesh>& resource)
{
	ImGui::TextColored(ImGuiLayer::Color::red, name.cstr());
	/*auto mesh = resource.resource;
	ImGui::Text("Vertices");

	for (uint32_t i = 0; i < mesh->bindings.count; i++)
	{
		char buffer[256];
		snprintf(buffer, 256, "Attribute %u", i);
		if (ImGui::TreeNode(buffer))
		{
			Buffer b{ mesh->vertices[i] };
			ImGui::BulletText("Format : %s", toString(mesh->bindings.attributes[i].format));
			ImGui::BulletText("Semantic : %s", toString(mesh->bindings.attributes[i].semantic));
			ImGui::BulletText("Type : %s", toString(mesh->bindings.attributes[i].type));
			ImGui::BulletText("Count : %u", 0);
			ImGui::BulletText("Offset : %u", mesh->bindings.offsets[i]);
			ImGui::BulletText("Buffer : %s", resources->name<Buffer>(&b).cstr());
			ImGui::TreePop();
		}
	}
	Buffer b{ mesh->indices };
	ImGui::Separator();
	ImGui::Text("Indices");
	ImGui::BulletText("Format : %s", toString(mesh->format));
	ImGui::BulletText("Count : %u", mesh->count);
	ImGui::BulletText("Buffer : %s", resources->name<Buffer>(&b).cstr());
	ImGui::Separator();*/

	// Mesh viewer
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	if (ImGui::BeginChild("MeshDisplayChild", ImVec2(0.f, (float)m_height + 5.f), false, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize))
	{
		if (ImGui::IsWindowHovered() && (ImGui::IsMouseDragging(0, 0.0f) || ImGui::IsMouseDragging(1, 0.0f) || !ImGui::IsAnyItemActive()))
		{
			// TODO use real deltatime
			m_arcball.update(Time::milliseconds(10));
		}
		if (resource.isLoaded())
		{
			drawMesh(&resource.get());
		}
		else
		{
			ImGui::Text("Mesh not loaded");
		}

		//ImGui::Image((ImTextureID)(uintptr_t)m_renderTarget->handle().value(), ImVec2((float)m_width, (float)m_height), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1));
	}
	ImGui::EndChild();
	ImGui::PopStyleVar();
}

void TextureViewer::draw(const String& name, ResourceHandle<Texture>& resource)
{
	ImGui::TextColored(ImGuiLayer::Color::red, name.cstr());
	if (!resource.isLoaded() || m_needUpdate)
	{
		ImGui::Text("Texture not loaded");
		return;// TODO: prevent opening if not loaded
	}
	Texture& texture = resource.get();
	gfx::TextureHandle handle = texture.getGfxHandle();
	bool isRenderTarget = (texture.getTextureUsage() & gfx::TextureUsage::RenderTarget) == gfx::TextureUsage::RenderTarget;
	bool isShaderResource = (texture.getTextureUsage() & gfx::TextureUsage::ShaderResource) == gfx::TextureUsage::ShaderResource;
	bool hasMips = (texture.getTextureUsage() & gfx::TextureUsage::GenerateMips) == gfx::TextureUsage::GenerateMips;
	ImGui::Text("%s - %u x %u (%s)", toString(texture.getTextureType()), texture.getWidth(), texture.getHeight(), toString(texture.getTextureFormat()));
	ImGui::Checkbox("Render target", &isRenderTarget); ImGui::SameLine();
	ImGui::Checkbox("Shader resource", &isShaderResource); ImGui::SameLine();
	ImGui::Checkbox("Mips", &hasMips);

	// TODO add zoom and mip viewer
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
	ImGui::DragInt("##Zoom", &zoomInt, 1, minZoom, maxZoom, "%d %%");

	gfx::GraphicDevice* device = Application::app()->graphic();
	ImTextureID textureID = ImGuiLayer::getTextureID(device, m_descriptorSet);
	ImVec4 mask = ImVec4(
		red ? 1.f : 0.f,
		green ? 1.f : 0.f,
		blue ? 1.f : 0.f,
		alpha ? 1.f : 0.f
	);
	// Image explorer
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	if (ImGui::BeginChild("TextureDisplayChild", ImVec2(0, 0), true, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar))
	{
		if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemActive())
		{
			zoomInt = min(max(zoomInt + (int)(ImGui::GetIO().MouseWheel * 2.f), minZoom), maxZoom);
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
	m_needUpdate(false)
{
}

void TextureViewer::create()
{
	gfx::GraphicDevice* device = Application::app()->graphic();
	gfx::ShaderBindingState state{};
	state.add(gfx::ShaderBindingType::SampledImage, gfx::ShaderMask::Fragment);
	// TODO: add layer & mip selector
	// There might be an issue cuz of pImmutableSamplers not being set

	//ImGuiLayer::
	m_descriptorSet = device->createDescriptorSet("ImGuiTextureViewerDescriptorSet", state);
	m_sampler = device->createSampler("ImGuiTextureViewerSampler", 
		gfx::Filter::Nearest, gfx::Filter::Nearest,
		gfx::SamplerMipMapMode::None,
		gfx::SamplerAddressMode::Repeat, gfx::SamplerAddressMode::Repeat, gfx::SamplerAddressMode::Repeat,
		1.0);
	if (m_resource.isLoaded())
	{
		gfx::DescriptorSetData data;
		data.addSampledImage(m_resource.get().getGfxHandle(), m_sampler);
		device->update(m_descriptorSet, data);
		m_needUpdate = false;
	}
	else
	{
		m_needUpdate = true; // wait for a texture to be available
	}
}

void TextureViewer::destroy()
{
	gfx::GraphicDevice* device = Application::app()->graphic();
	device->destroy(m_descriptorSet);
	device->destroy(m_sampler);
}

void TextureViewer::update(aka::Time deltaTime)
{
	gfx::GraphicDevice* device = Application::app()->graphic();
	if (m_resource.isLoaded() && m_needUpdate)
	{
		gfx::DescriptorSetData data;
		data.addSampledImage(m_resource.get().getGfxHandle(), m_sampler);
		device->update(m_descriptorSet, data);
		m_needUpdate = false;
	}
}

};
