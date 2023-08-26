#include "Editor.h"

#include <Aka/Core/Container/String.h>
#include <Aka/OS/OS.h>
#include <Aka/Layer/ImGuiLayer.h>

#include <imgui.h>

using namespace aka;

struct UniformBuffer
{
	mat4f model;
	mat4f view;
	mat4f projection;
};

mat3f getTransform(uint32_t width, uint32_t height, anglef rotation)
{
	aka::vec2f size = aka::vec2f(300.f);
	aka::vec2f position = aka::vec2f(
		width / 2.f - size.x / 2.f,
		height / 2.f - size.y / 2.f
	);
	aka::mat3f transform = aka::mat3f::identity();
	transform *= aka::mat3f::translate(position);
	transform *= aka::mat3f::translate(0.5f * size);
	transform *= aka::mat3f::rotate(rotation);
	transform *= aka::mat3f::translate(-0.5f * size);
	transform *= aka::mat3f::scale(size);
	return transform;
}

struct Vertex
{
	float position[3];
	float texcoord[2];
};

static const uint32_t s_vertexCount = 36;

gfx::BufferHandle getCubeVertices(gfx::GraphicDevice* device)
{
	float vertices[s_vertexCount * 5] = {
		-1.0f,  1.0f, -1.0f,	0.f, 1.f,
		-1.0f, -1.0f, -1.0f,	0.f, 0.f,
		 1.0f, -1.0f, -1.0f,	1.f, 0.f,
		 1.0f, -1.0f, -1.0f,	1.f, 0.f,
		 1.0f,  1.0f, -1.0f,	1.f, 1.f,
		-1.0f,  1.0f, -1.0f,	0.f, 1.f,

		-1.0f, -1.0f,  1.0f,	0.f, 1.f,
		-1.0f, -1.0f, -1.0f,	0.f, 0.f,
		-1.0f,  1.0f, -1.0f,	1.f, 0.f,
		-1.0f,  1.0f, -1.0f,	1.f, 0.f,
		-1.0f,  1.0f,  1.0f,	1.f, 1.f,
		-1.0f, -1.0f,  1.0f,	0.f, 1.f,

		 1.0f, -1.0f, -1.0f,	0.f, 0.f,
		 1.0f, -1.0f,  1.0f,	0.f, 1.f,
		 1.0f,  1.0f,  1.0f,	1.f, 1.f,
		 1.0f,  1.0f,  1.0f,	1.f, 1.f,
		 1.0f,  1.0f, -1.0f,	1.f, 0.f,
		 1.0f, -1.0f, -1.0f,	0.f, 0.f,

		-1.0f, -1.0f,  1.0f,	0.f, 0.f,
		-1.0f,  1.0f,  1.0f,	0.f, 1.f,
		 1.0f,  1.0f,  1.0f,	1.f, 1.f,
		 1.0f,  1.0f,  1.0f,	1.f, 1.f,
		 1.0f, -1.0f,  1.0f,	1.f, 0.f,
		-1.0f, -1.0f,  1.0f,	0.f, 0.f,

		-1.0f,  1.0f, -1.0f,	0.f, 0.f,
		 1.0f,  1.0f, -1.0f,	1.f, 0.f,
		 1.0f,  1.0f,  1.0f,	1.f, 1.f,
		 1.0f,  1.0f,  1.0f,	1.f, 1.f,
		-1.0f,  1.0f,  1.0f,	0.f, 1.f,
		-1.0f,  1.0f, -1.0f,	0.f, 0.f,

		-1.0f, -1.0f, -1.0f,	0.f, 0.f,
		-1.0f, -1.0f,  1.0f,	0.f, 1.f,
		 1.0f, -1.0f, -1.0f,	1.f, 0.f,
		 1.0f, -1.0f, -1.0f,	1.f, 0.f,
		-1.0f, -1.0f,  1.0f,	0.f, 1.f,
		 1.0f, -1.0f,  1.0f,	1.f, 1.f,
	};
	return device->createBuffer("CubeVertexBuffer", gfx::BufferType::Vertex, sizeof(vertices), gfx::BufferUsage::Default, gfx::BufferCPUAccess::None, vertices);
}

// TODO: Should use JSON for this ? JSON that can be generated with a script reading all files in shaders folder (generating DB)
const aka::Path ShaderVertexPath = aka::OS::cwd() + "../../../asset/shaders/shader.vert";
const aka::Path ShaderFragmentPath = aka::OS::cwd() + "../../../asset/shaders/shader.frag";

const aka::ShaderKey ShaderVertex{ ShaderVertexPath, {  }, aka::ShaderType::Vertex, "main" };
const aka::ShaderKey ShaderFragment{ ShaderFragmentPath, {  }, aka::ShaderType::Fragment, "main"};

const aka::ProgramKey ProgramGraphic{ {{ShaderVertex, ShaderFragment}} };

Editor::Editor() :
	Application({ new ImGuiLayer() })
{
}

void Editor::onCreate(int argc, char* argv[])
{
	gfx::GraphicDevice* device = graphic();
	ShaderRegistry* registry = program();

	registry->add(ProgramGraphic, device);

	gfx::ProgramHandle program = registry->get(ProgramGraphic);

	m_renderPass = device->createBackbufferRenderPass();
	m_backbuffer = device->createBackbuffer(m_renderPass);

	m_renderPipeline = device->createGraphicPipeline(
		"RenderPipeline",
		program,
		gfx::PrimitiveType::Triangles,
		device->get(m_renderPass)->state,
		gfx::VertexAttributeState {}.add(gfx::VertexSemantic::Position, gfx::VertexFormat::Float, gfx::VertexType::Vec3).add(gfx::VertexSemantic::Normal, gfx::VertexFormat::Float, gfx::VertexType::Vec2),
		gfx::ViewportState{}.size(width(), height()),
		gfx::DepthStateDisabled,
		gfx::StencilStateDisabled,
		gfx::CullStateDisabled,
		gfx::BlendStateDisabled,
		gfx::FillStateFill
	);

	m_vertices = getCubeVertices(device);
	m_descriptorSet = device->createDescriptorSet("DescriptorSet", device->get(program)->sets[0]);

	m_cameraController.set(aabbox(point3(-1.f), point3(1.f)));
	m_cameraProjection.hFov = anglef::degree(60.f);
	m_cameraProjection.ratio = width() / (float)height();
	m_cameraProjection.nearZ = 0.1f;
	m_cameraProjection.farZ = 100.f;

	UniformBuffer ubo;
	ubo.model = mat4f::identity();
	ubo.view = m_cameraController.view();
	ubo.projection = m_cameraProjection.projection();
	m_uniformBuffer = device->createBuffer("CameraBuffer", gfx::BufferType::Uniform, sizeof(UniformBuffer), gfx::BufferUsage::Default, gfx::BufferCPUAccess::None, &ubo);

	const uint8_t dataTexture[4] = { 0,255,0,255 };
	const void* dataVoid = dataTexture;
	m_texture = device->createTexture("Texture", 1, 1, 1, gfx::TextureType::Texture2D, 1, 1, gfx::TextureFormat::RGBA8, gfx::TextureUsage::ShaderResource, &dataVoid);
	m_sampler = device->createSampler(
		"Sampler",
		gfx::Filter::Linear, gfx::Filter::Linear,
		gfx::SamplerMipMapMode::Linear,
		gfx::SamplerAddressMode::Repeat, gfx::SamplerAddressMode::Repeat, gfx::SamplerAddressMode::Repeat,
		1.0
	);

	gfx::DescriptorSetData data{};
	data.addUniformBuffer(m_uniformBuffer);
	data.addSampledImage(m_texture, m_sampler);
	device->update(m_descriptorSet, data);
}

void Editor::onDestroy()
{
	gfx::GraphicDevice* device = graphic();
	device->destroy(m_texture);
	device->destroy(m_sampler);
	device->destroy(m_vertices);
	device->destroy(m_uniformBuffer);
	device->destroy(m_descriptorSet);
	device->destroy(m_renderPipeline);
	device->destroy(m_renderPass);
	device->destroy(m_backbuffer);
}

void Editor::onFixedUpdate(aka::Time time)
{
	if (platform()->keyboard().down(KeyboardKey::Escape))
		EventDispatcher<QuitEvent>::emit();
}

void Editor::onUpdate(aka::Time time)
{
	program()->reloadIfChanged(graphic());

	const ImGuiIO& io = ImGui::GetIO();
	if (!io.WantCaptureMouse && !io.WantCaptureKeyboard)
	{
		if (m_cameraController.update(time))
		{
			m_dirty = true;
		}
	}
	m_rotation += aka::anglef::radian(time.seconds());
}

void Editor::onRender(gfx::Frame* _frame)
{
	gfx::GraphicDevice* device = graphic();
	gfx::CommandList* cmd = device->getGraphicCommandList(_frame);

	//if (m_dirty)
	{
		UniformBuffer ubo;
		ubo.model = mat4f::rotate(vec3f(0,0,1), m_rotation);
		ubo.view = m_cameraController.view();
		ubo.projection = m_cameraProjection.projection();
		device->upload(m_uniformBuffer, &ubo, 0, sizeof(UniformBuffer));
		m_dirty = false;
	}

	gfx::FramebufferHandle backbuffer = device->get(m_backbuffer, _frame);

	cmd->bindPipeline(m_renderPipeline);
	cmd->bindDescriptorSet(0, m_descriptorSet);
	cmd->bindVertexBuffer(m_vertices, 0);

	cmd->beginRenderPass(m_renderPass, backbuffer, gfx::ClearState{ gfx::ClearMask::All, { 0.1f, 0.1f, 0.1f, 1.f }, 1.f, 0 });
	cmd->draw(s_vertexCount, 0);
	cmd->endRenderPass();

	{
		if (ImGui::Begin("Window"))
		{
		}
		ImGui::End();
	}
}
