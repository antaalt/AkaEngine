#include "Editor.h"

#include <Aka/Core/Container/String.h>
#include <Aka/OS/OS.h>
#include <Aka/Layer/ImGuiLayer.h>

#include <imgui.h>

using namespace aka;

// ------------ RENDERER ----------------
struct CameraUniformBuffer
{
	mat4f view;
	mat4f projection;
};

struct InstanceUniformBuffer
{
	mat4f model;
};

struct MaterialUniformBuffer
{
	color4f color;
};
// ------------ / RENDERER ----------------

static const uint32_t s_vertexCount = 36;

static const float s_vertices[s_vertexCount * 5] = {
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
gfx::BufferHandle getCubeVertices(gfx::GraphicDevice* device)
{
	return device->createBuffer("CubeVertexBuffer", gfx::BufferType::Vertex, sizeof(s_vertices), gfx::BufferUsage::Default, gfx::BufferCPUAccess::None, s_vertices);
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
		gfx::DepthStateLessEqual,
		gfx::StencilStateDisabled,
		gfx::CullStateDisabled,
		gfx::BlendStateDisabled,
		gfx::FillStateFill
	);
	m_vertices = getCubeVertices(device);

	// TODO retrieve this from somewhere generated from shader ?
	// Or pass it as input of program instead
	gfx::ShaderBindingState bindings[3]{}; // camera, instance, material
	{
		// Camera
		bindings[0].add(gfx::ShaderBindingType::UniformBuffer, gfx::ShaderMask::Vertex, 1);

		// Instance
		bindings[1].add(gfx::ShaderBindingType::UniformBuffer, gfx::ShaderMask::Vertex, 1);

		// Material
		bindings[2].add(gfx::ShaderBindingType::UniformBuffer, gfx::ShaderMask::Vertex, 1);
		bindings[2].add(gfx::ShaderBindingType::SampledImage, gfx::ShaderMask::Fragment, 1);
		bindings[2].add(gfx::ShaderBindingType::SampledImage, gfx::ShaderMask::Fragment, 1);
	}

	{ // CAMERA UBO
		m_cameraDescriptorSet = device->createDescriptorSet("CameraDescriptorSet", bindings[0]);

		m_cameraController.set(aabbox(point3(-1.f), point3(1.f)));
		m_cameraProjection.hFov = anglef::degree(60.f);
		m_cameraProjection.ratio = width() / (float)height();
		m_cameraProjection.nearZ = 0.1f;
		m_cameraProjection.farZ = 100.f;

		CameraUniformBuffer ubo;
		ubo.view = m_cameraController.view();
		ubo.projection = m_cameraProjection.projection();
		m_cameraUniformBuffer = device->createBuffer("CameraBuffer", gfx::BufferType::Uniform, sizeof(CameraUniformBuffer), gfx::BufferUsage::Default, gfx::BufferCPUAccess::None, &ubo);
		
		gfx::DescriptorSetData data{};
		data.addUniformBuffer(m_cameraUniformBuffer);
		device->update(m_cameraDescriptorSet, data);
	}
	{ // INSTANCE UBO
		m_instanceDescriptorSet = device->createDescriptorSet("InstanceDescriptorSet", bindings[1]);

		InstanceUniformBuffer ubo;
		ubo.model = mat4f::rotate(vec3f(0, 0, 1), m_rotation);
		m_instanceUniformBuffer = device->createBuffer("InstanceBuffer", gfx::BufferType::Uniform, sizeof(InstanceUniformBuffer), gfx::BufferUsage::Default, gfx::BufferCPUAccess::None, &ubo);

		gfx::DescriptorSetData data{};
		data.addUniformBuffer(m_instanceUniformBuffer);
		device->update(m_instanceDescriptorSet, data);
	}


	{ // Create a static mesh & archive it
		using namespace app;
		ArchivePath smeshPath = ArchivePath("../../../asset/library/mesh.smesh");
		{ // Hardcoded import for now
			app::ArchiveStaticMesh mesh(ArchivePath("../../../asset/library/mesh.smesh"));
			{
				app::ArchiveBatch batch(ArchivePath("../../../asset/library/mesh.batch"));
				batch.geometry = ArchiveGeometry(ArchivePath("../../../asset/library/mesh.geo"));
				// indices
				batch.geometry.indices.resize(s_vertexCount);
				for (uint32_t i = 0; i < s_vertexCount; i++)
					batch.geometry.indices[i] = i;
				// Vertices
				batch.geometry.vertices.resize(s_vertexCount);
				Memory::copy(batch.geometry.vertices.data(), s_vertices, sizeof(s_vertices));
				batch.geometry.bounds = aabbox(point3f(-1.f), point3f(1.f));


				// Material
				batch.material = ArchiveMaterial(ArchivePath("../../../asset/library/mesh.mat"));
				batch.material.color = color4f(0.0, 0.0, 1.0, 1.0);

				Image img = Image::load("../../../asset/textures/skyscraper.jpg");
				batch.material.albedo = ArchiveImage(ArchivePath("../../../asset/library/albedo.tex"));
				batch.material.albedo.width = img.width();
				batch.material.albedo.height = img.height();
				batch.material.albedo.channels = img.components();
				batch.material.albedo.data.append(img.data(), img.data() + img.size());

				Image imgNormal = Image::load("../../../asset/textures/skyscraper-normal.jpg");
				batch.material.normal = ArchiveImage(ArchivePath("../../../asset/library/normal.tex"));
				batch.material.normal.width = imgNormal.width();
				batch.material.normal.height = imgNormal.height();
				batch.material.normal.channels = imgNormal.components();
				batch.material.normal.data.append(imgNormal.data(), imgNormal.data() + imgNormal.size());

				mesh.batches.append(batch);
			}
			ArchiveSaveResult res = mesh.save(mesh.getPath());
		}
		{ // Test if everything went well
			app::ArchiveStaticMesh mesh(ArchivePath("../../../asset/library/mesh.smesh"));
			ArchiveLoadResult res = mesh.load(mesh.getPath());
			mesh.batches;
		}
		// Add to library & load it.
		AssetLibrary library;
		m_resourceID = library.registerStaticMesh(smeshPath);
		m_resource = library.getStaticMesh(m_resourceID);
		if (m_resource.isLoaded())
		{
			app::StaticMesh mesh = m_resource.get();
		}
	}
}

void Editor::onDestroy()
{
	gfx::GraphicDevice* device = graphic();
	device->destroy(m_vertices);
	device->destroy(m_cameraUniformBuffer);
	device->destroy(m_instanceUniformBuffer);
	device->destroy(m_cameraDescriptorSet);
	device->destroy(m_instanceDescriptorSet);
	device->destroy(m_renderPipeline);
	device->destroy(m_renderPass);
	device->destroy(m_backbuffer);

	m_resource.get().destroy(device);
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

	if (m_dirty)
	{
		CameraUniformBuffer ubo;
		ubo.view = m_cameraController.view();
		ubo.projection = m_cameraProjection.projection();
		device->upload(m_cameraUniformBuffer, &ubo, 0, sizeof(CameraUniformBuffer));
		m_dirty = false;
	}
	{
		InstanceUniformBuffer ubo;
		ubo.model = mat4f::rotate(vec3f(0, 0, 1), m_rotation);
		device->upload(m_instanceUniformBuffer, &ubo, 0, sizeof(InstanceUniformBuffer));
	}

	gfx::FramebufferHandle backbuffer = device->get(m_backbuffer, _frame);


	cmd->bindPipeline(m_renderPipeline);
	cmd->bindDescriptorSet(0, m_cameraDescriptorSet);
	cmd->bindDescriptorSet(1, m_instanceDescriptorSet); // Should be by resource, within the scene hierarchy component

	if (m_resource.isLoaded())
	{
		app::StaticMesh& mesh = m_resource.get();
		cmd->bindVertexBuffer(mesh.gfxVertexBuffer, 0);
		cmd->bindIndexBuffer(mesh.gfxIndexBuffer, gfx::IndexFormat::UnsignedInt, 0);

		cmd->beginRenderPass(m_renderPass, backbuffer, gfx::ClearState{ gfx::ClearMask::All, { 0.1f, 0.1f, 0.1f, 1.f }, 1.f, 0 });
		for (const auto& batch : mesh.batches)
		{
			cmd->bindDescriptorSet(2, batch.gfxDescriptorSet);
			cmd->drawIndexed(batch.indexCount, batch.indexOffset, batch.vertexOffset, 1);
		}
		cmd->endRenderPass();
	}


	{
		if (ImGui::Begin("Window"))
		{
		}
		ImGui::End();
	}
}
