#include "StaticMeshViewer.hpp"

#include <Aka/Resource/Shader/ShaderRegistry.h>
#include <Aka/Layer/ImGuiLayer.h>

namespace app {

using namespace aka;


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
	case gfx::VertexSemantic::WorldMatrix:
		return "worldmatrix";
	case gfx::VertexSemantic::NormalMatrix:
		return "normalmatrix";
	default:
		return "unknown";
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

struct StaticMeshViewerUBO {
	mat4f model;
	mat4f view;
	mat4f proj;
	mat4f normal;
};

StaticMeshViewer::StaticMeshViewer(AssetID _assetID, ResourceHandle<app::StaticMesh> _resourceHandle) :
	AssetViewer(ResourceType::StaticMesh, _assetID, _resourceHandle),
	m_needCameraUpdate(false)
{
}

StaticMeshViewer::~StaticMeshViewer()
{
}

void StaticMeshViewer::onCreate(gfx::GraphicDevice* _device)
{
	// TODO if no data, bug ?
	std::vector<uint8_t> data(m_width * m_height * 4, 0xff);
	void* dataVoid = data.data();
	m_renderTarget = _device->createTexture("StaticMeshViewerRT", m_width, m_height, 1, gfx::TextureType::Texture2D, 1, 1, gfx::TextureFormat::RGBA8, gfx::TextureUsage::RenderTarget | gfx::TextureUsage::ShaderResource, &dataVoid);
	m_depthTarget = _device->createTexture("StaticMeshViewerDepthRT", m_width, m_height, 1, gfx::TextureType::Texture2D, 1, 1, gfx::TextureFormat::Depth, gfx::TextureUsage::RenderTarget);
	gfx::Attachment color = { m_renderTarget, gfx::AttachmentFlag::None, 0, 0 };
	gfx::Attachment depth = { m_depthTarget, gfx::AttachmentFlag::None, 0, 0 };
	gfx::RenderPassState rpState{};
	rpState.addColor(gfx::TextureFormat::RGBA8, gfx::AttachmentLoadOp::Clear, gfx::AttachmentStoreOp::Store, gfx::ResourceAccessType::Attachment, gfx::ResourceAccessType::Resource);
	rpState.setDepth(gfx::TextureFormat::Depth);
	m_renderPass = _device->createRenderPass("StaticMeshViewerRenderPass", rpState);
	m_target = _device->createFramebuffer("StaticMeshViewerFramebuffer", m_renderPass, &color, 1, &depth);

	const ShaderKey ShaderVertex = ShaderKey().setPath(app::AssetPath("../shaders/editor/basic.vert").getAbsolutePath()).setType(aka::ShaderType::Vertex);
	const ShaderKey ShaderFragment = ShaderKey().setPath(app::AssetPath("../shaders/editor/basic.frag").getAbsolutePath()).setType(aka::ShaderType::Fragment);

	const ProgramKey ProgramGraphic = ProgramKey().add(ShaderVertex).add(ShaderFragment);
	ShaderRegistry* program = Application::app()->program();
	program->add(ProgramGraphic, _device);
	gfx::ProgramHandle p = program->get(ProgramGraphic);
	m_pipeline = _device->createGraphicPipeline(
		"StaticMeshViewerPipeline",
		p,
		gfx::PrimitiveType::Triangles,
		rpState,
		gfx::VertexState{}.add(StaticVertex::getState()),
		gfx::ViewportState{}.size(m_width, m_height),
		gfx::DepthStateDefault,
		gfx::StencilStateDefault,
		gfx::CullStateDefault,
		gfx::BlendStateDefault,
		gfx::FillStateLine
	);
	// TODO 1 by gfx::MaxFrameInFlight
	m_descriptorPool = _device->createDescriptorPool("StaticMeshViewerDescriptorPool", _device->get(p)->sets[0], 1);
	m_descriptorSet = _device->allocateDescriptorSet("StaticMeshViewerDescriptorSet", _device->get(p)->sets[0], m_descriptorPool);
	{
		m_arcball.set(aabbox<>(point3f(-20.f), point3f(20.f)));
		m_projection.nearZ = 0.1f;
		m_projection.farZ = 1000.f;
		m_projection.ratio = m_width / (float)m_height;
		m_projection.hFov = anglef::degree(90.f);

		StaticMeshViewerUBO ubo{};
		ubo.model = mat4f::identity();
		ubo.view = m_arcball.view();
		ubo.proj = m_projection.projection();
		ubo.normal = mat4f::transpose(mat4f::inverse(ubo.view * ubo.model));
		m_uniform = _device->createBuffer("StaticMeshViewerUBO", gfx::BufferType::Uniform, sizeof(StaticMeshViewerUBO), gfx::BufferUsage::Default, gfx::BufferCPUAccess::None, &ubo);

		Vector<gfx::DescriptorUpdate> desc;
		desc.append(gfx::DescriptorUpdate::uniformBuffer(0, 0, m_uniform));
		_device->update(m_descriptorSet, desc.data(), desc.size());
	}

	{
		gfx::ShaderBindingState state{};
		state.add(gfx::ShaderBindingType::SampledImage, gfx::ShaderMask::Fragment);
		m_imguiDescriptorPool = _device->createDescriptorPool("StaticMeshViewerDescriptorPool", _device->get(p)->sets[0], 1);
		m_imguiDescriptorSet = _device->allocateDescriptorSet("ImGuiStaticMeshViewerDescriptorSet", state, m_imguiDescriptorPool);
		m_imguiSampler = _device->createSampler("ImGuiTextureViewerSampler",
			gfx::Filter::Nearest, gfx::Filter::Nearest,
			gfx::SamplerMipMapMode::None,
			gfx::SamplerAddressMode::Repeat, gfx::SamplerAddressMode::Repeat, gfx::SamplerAddressMode::Repeat,
			1.0);
		Vector<gfx::DescriptorUpdate> desc;
		desc.append(gfx::DescriptorUpdate::sampledTexture2D(0, 0, m_renderTarget, m_imguiSampler));
		_device->update(m_imguiDescriptorSet, desc.data(), desc.size());
	}
}
void StaticMeshViewer::onDestroy(gfx::GraphicDevice* _device)
{
	_device->destroy(m_pipeline);
	_device->destroy(m_imguiSampler);
	_device->free(m_imguiDescriptorSet);
	_device->destroy(m_imguiDescriptorPool);
	_device->free(m_descriptorSet);
	_device->destroy(m_descriptorPool);
	_device->destroy(m_uniform);
	_device->destroy(m_target);
	_device->destroy(m_renderTarget);
	_device->destroy(m_depthTarget);
	_device->destroy(m_renderPass);
}
void StaticMeshViewer::onUpdate(aka::Time deltaTime)
{
	// This is run one frame late though...
	if (m_needCameraUpdate)
	{
		m_arcball.update(deltaTime);
	}
}
void StaticMeshViewer::onRender(gfx::GraphicDevice* _device, aka::gfx::Frame* frame)
{
	// This is run one frame late though...
	if (m_needCameraUpdate)
	{
		_device->wait();
		StaticMeshViewerUBO ubo{};
		ubo.model = mat4f::identity();
		ubo.view = m_arcball.view();
		ubo.proj = m_projection.projection();
		ubo.normal = mat4f::transpose(mat4f::inverse(ubo.view * ubo.model));
		_device->upload(m_uniform, &ubo, 0, sizeof(StaticMeshViewerUBO));
	}
	if (m_resource.isLoaded())
	{
		renderMesh(frame, m_resource.get());
	}
}

void StaticMeshViewer::onLoad(const StaticMesh& mesh)
{
	m_arcball.set(mesh.getBounds());
}

void StaticMeshViewer::renderMesh(gfx::Frame* frame, const StaticMesh& mesh)
{
	gfx::GraphicDevice* device = Application::app()->graphic();
	Renderer* renderer = Application::app()->renderer();
	gfx::CommandList* cmd = device->getGraphicCommandList(frame);

	cmd->transition(m_renderTarget, gfx::ResourceAccessType::Resource, gfx::ResourceAccessType::Attachment);
	cmd->beginRenderPass(m_renderPass, m_target, gfx::ClearState{ gfx::ClearMask::All, { 0.1f, 0.1f, 0.1f, 1.f}, 1.f, 0 });


	cmd->bindPipeline(m_pipeline);
	cmd->bindIndexBuffer(renderer->getGeometryBuffer(mesh.getIndexBufferHandle()), mesh.getIndexFormat(), renderer->getGeometryBufferOffset(mesh.getIndexBufferHandle()));
	cmd->bindVertexBuffer(0, renderer->getGeometryBuffer(mesh.getVertexBufferHandle()), renderer->getGeometryBufferOffset(mesh.getVertexBufferHandle()));
	cmd->bindDescriptorSet(0, m_descriptorSet);

	for (uint32_t i = 0; i < mesh.getBatchCount(); i++)
	{
		const StaticMeshBatch& batch = mesh.getBatch(i);
		cmd->drawIndexed(batch.indexCount, batch.indexOffset, batch.vertexOffset, 1);
	}
	cmd->endRenderPass();
}

void StaticMeshViewer::drawUIResource(const app::StaticMesh& mesh)
{
	m_resource.get();
	ImGui::TextColored(aka::ImGuiLayer::Color::red, mesh.getName().cstr());
	ImGui::Text("Vertices");
	gfx::VertexBufferLayout layout = StaticVertex::getState();
	for (uint32_t i = 0; i < layout.count; i++)
	{
		char buffer[256];
		snprintf(buffer, 256, "Attribute %u", i);
		if (ImGui::TreeNode(buffer))
		{
			ImGui::BulletText("Format : %s", toString(layout.attributes[i].format));
			ImGui::BulletText("Semantic : %s", toString(layout.attributes[i].semantic));
			ImGui::BulletText("Type : %s", toString(layout.attributes[i].type));
			ImGui::BulletText("Offset : %u", layout.offsets[i]);
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
		else
		{
			m_needCameraUpdate = false;
		}
		gfx::GraphicDevice* device = Application::app()->graphic();
		ImTextureID texID = ImGuiLayer::getTextureID(device, m_imguiDescriptorSet);
		ImGui::Image(texID, ImVec2((float)m_width, (float)m_height), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1));
	}
	ImGui::EndChild();
	ImGui::PopStyleVar();
}


};