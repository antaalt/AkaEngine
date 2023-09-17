#include "Editor.h"

#include <Aka/Core/Container/String.h>
#include <Aka/OS/OS.h>
#include <Aka/Layer/ImGuiLayer.h>

#include <imgui.h>
#include <imguizmo.h>
#include <map>
#include <vector>

#include "Importer/AssimpImporter.hpp"
#include "Editor/AssetEditorLayer.hpp"
#include "Editor/SceneEditorLayer.hpp"
#include "Editor/InfoEditorLayer.hpp"

using namespace aka;

// ------------ RENDERER ----------------
struct CameraUniformBuffer
{
	mat4f view;
	mat4f projection;
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

std::vector<app::Vertex> getSphereVertices(float radius, uint32_t segmentCount, uint32_t ringCount)
{
	// http://www.songho.ca/opengl/gl_sphere.html
	std::vector<app::Vertex> vertices;

	float length = 1.f / radius;
	anglef sectorStep = 2.f * pi<float> / (float)ringCount;
	anglef stackStep = pi<float> / (float)segmentCount;
	anglef ringAngle, segmentAngle;
	aabbox<> bounds;

	for (uint32_t i = 0; i <= segmentCount; ++i)
	{
		segmentAngle = pi<float> / 2.f - (float)i * stackStep; // starting from pi/2 to -pi/2
		float xy = radius * cos(segmentAngle); // r * cos(u)
		float z = radius * sin(segmentAngle); // r * sin(u)

		// add (ringCount+1) vertices per segment
		// the first and last vertices have same position and normal, but different uv
		for (uint32_t j = 0; j <= ringCount; ++j)
		{
			app::Vertex v;
			ringAngle = (float)j * sectorStep; // starting from 0 to 2pi

			v.position.x = xy * cos(ringAngle); // r * cos(u) * cos(v)
			v.position.y = xy * sin(ringAngle); // r * cos(u) * sin(v)
			v.position.z = z;

			//v.normal = norm3f(v.position / radius);

			v.uv.u = (float)j / ringCount;
			v.uv.v = (float)i / segmentCount;
			//v.color = color4f(1.f);
			vertices.push_back(v);
			bounds.include(v.position);
		}
	}
	return vertices;
}

std::vector<uint32_t> getSphereIndices(float radius, uint32_t segmentCount, uint32_t ringCount)
{
	std::vector<uint32_t> indices;
	for (uint32_t i = 0; i < segmentCount; ++i)
	{
		uint32_t k1 = i * (ringCount + 1);     // beginning of current stack
		uint32_t k2 = k1 + ringCount + 1;      // beginning of next stack

		for (uint32_t j = 0; j < ringCount; ++j, ++k1, ++k2)
		{
			// 2 triangles per sector excluding first and last stacks
			// k1 => k2 => k1+1
			if (i != 0)
			{
				indices.push_back(k1);
				indices.push_back(k2);
				indices.push_back(k1 + 1);
			}
			// k1+1 => k2 => k2+1
			if (i != (segmentCount - 1))
			{
				indices.push_back(k1 + 1);
				indices.push_back(k2);
				indices.push_back(k2 + 1);
			}
		}
	}
	return indices;
}


Editor::Editor()
{
	ImGuiLayer* imgui = getRoot().addLayer<ImGuiLayer>();
	// Editor are child of imgui to support frame
	imgui->addLayer<app::SceneEditorLayer>()->setCurrentScene(&m_scene);
	imgui->addLayer<app::AssetEditorLayer>()->setLibrary(&m_library);
	imgui->addLayer<app::InfoEditorLayer>();
}

void Editor::onCreate(int argc, char* argv[])
{
	gfx::GraphicDevice* device = graphic();
	ShaderRegistry* registry = program();

	// TODO: Should use JSON for this ? JSON that can be generated with a script reading all files in shaders folder (generating DB)
	const aka::Path ShaderVertexPath = aka::OS::cwd() + "../../../asset/shaders/shader.vert";
	const aka::Path ShaderFragmentPath = aka::OS::cwd() + "../../../asset/shaders/shader.frag";

	const aka::ShaderKey ShaderVertex = aka::ShaderKey().setPath(ShaderVertexPath).setType(aka::ShaderType::Vertex);
	const aka::ShaderKey ShaderFragment = aka::ShaderKey().setPath(ShaderFragmentPath).setType(aka::ShaderType::Fragment);

	m_programKey = aka::ProgramKey();
	m_programKey.add(ShaderVertex).add(ShaderFragment);

	registry->add(m_programKey, device);
	m_program = registry->get(m_programKey);

	createRenderPass();

	// TODO retrieve this from somewhere generated from shader ?
	// Or pass it as input of program instead
	gfx::ShaderBindingState bindings[3]{}; // camera, instance, material
	{
		// Camera
		bindings[0].add(gfx::ShaderBindingType::UniformBuffer, gfx::ShaderMask::Vertex, 1);
		AKA_ASSERT(device->get(m_program)->sets[0] == bindings[0], "Invalid bindings");

		// Instance
		bindings[1].add(gfx::ShaderBindingType::UniformBuffer, gfx::ShaderMask::Vertex, 1);
		AKA_ASSERT(device->get(m_program)->sets[1] == bindings[1], "Invalid bindings");

		// Material
		bindings[2].add(gfx::ShaderBindingType::UniformBuffer, gfx::ShaderMask::Vertex | gfx::ShaderMask::Fragment, 1);
		bindings[2].add(gfx::ShaderBindingType::SampledImage, gfx::ShaderMask::Fragment, 1);
		bindings[2].add(gfx::ShaderBindingType::SampledImage, gfx::ShaderMask::Fragment, 1);
		AKA_ASSERT(device->get(m_program)->sets[2] == bindings[2], "Invalid bindings");
	}

	{ // CAMERA UBO
		m_cameraDescriptorSet = device->createDescriptorSet("CameraDescriptorSet", bindings[0]);

		m_cameraController.set(aabbox(point3(-1.f), point3(1.f)));
		m_cameraProjection.hFov = anglef::degree(60.f);
		m_cameraProjection.ratio = width() / (float)height();
		m_cameraProjection.nearZ = 0.1f;
		m_cameraProjection.farZ = 10000.f;

		CameraUniformBuffer ubo;
		ubo.view = m_cameraController.view();
		ubo.projection = m_cameraProjection.projection();
		m_cameraUniformBuffer = device->createBuffer("CameraBuffer", gfx::BufferType::Uniform, sizeof(CameraUniformBuffer), gfx::BufferUsage::Default, gfx::BufferCPUAccess::None, &ubo);
		
		gfx::DescriptorSetData data{};
		data.addUniformBuffer(m_cameraUniformBuffer);
		device->update(m_cameraDescriptorSet, data);
	}


	/*{ // Create a static mesh & archive it
#if 0
		using namespace app;
		AssetPath scenePath			= AssetPath("demo/scene.scene");
		AssetPath smeshPath			= AssetPath("demo/cube.smesh");
		AssetPath smeshSpherePath	= AssetPath("demo/sphere.smesh");
		AssetPath batchPath			= AssetPath("demo/cube.batch");
		AssetPath batchSpherePath	= AssetPath("demo/sphere.batch");
		AssetPath geoPath			= AssetPath("demo/cube.geo");
		AssetPath geoSpherePath		= AssetPath("demo/sphere.geo");
		AssetPath materialPath		= AssetPath("demo/mesh.mat");
		AssetPath image0Path		= AssetPath("demo/albedo.img");
		AssetPath image1Path		= AssetPath("demo/normal.img");
		OS::Directory::create(AssetPath("demo/").getAbsolutePath());

		// Add to library & load it.
		AssetID meshID = m_library.registerAsset(smeshPath, AssetType::StaticMesh);
		AssetID meshSphereID = m_library.registerAsset(smeshSpherePath, AssetType::StaticMesh);
		AssetID batchID = m_library.registerAsset(batchPath, AssetType::Batch);
		AssetID batchSphereID = m_library.registerAsset(batchSpherePath, AssetType::Batch);
		AssetID geometryID = m_library.registerAsset(geoPath, AssetType::Geometry);
		AssetID geometrySphereID = m_library.registerAsset(geoSpherePath, AssetType::Geometry);
		AssetID materialID = m_library.registerAsset(materialPath, AssetType::Material);
		AssetID image0ID = m_library.registerAsset(image0Path, AssetType::Image);
		AssetID image1ID = m_library.registerAsset(image1Path, AssetType::Image);
		AssetID sceneID = m_library.registerAsset(scenePath, AssetType::Scene);
		m_sceneID = m_library.getResourceID(sceneID);
		aka::StopWatch watch;
		{ // Hardcoded import for now
			app::ArchiveStaticMesh mesh(meshID);
			{ // Cube Mesh
				app::ArchiveBatch batch(batchID);
				batch.geometry = ArchiveGeometry(geometryID);
				// indices
				batch.geometry.indices.resize(s_vertexCount);
				for (uint32_t i = 0; i < s_vertexCount; i++)
					batch.geometry.indices[i] = i;
				// Vertices
				batch.geometry.vertices.resize(s_vertexCount);
				Memory::copy(batch.geometry.vertices.data(), s_vertices, sizeof(s_vertices));
				batch.geometry.bounds = aabbox(point3f(-1.f), point3f(1.f));


				// Material
				batch.material = ArchiveMaterial(materialID);
				batch.material.color = color4f(0.0, 0.0, 1.0, 1.0);

				Image img = ImageDecoder::fromDisk("../../../asset/textures/skyscraper.jpg");
				batch.material.albedo = ArchiveImage(image0ID);
				batch.material.albedo.width = img.width;
				batch.material.albedo.height = img.height;
				batch.material.albedo.channels = img.getComponents();
				batch.material.albedo.data.append(img.data(), img.data() + img.size());
				batch.material.albedo.data = std::move(img.bytes);

				Image imgNormal = ImageDecoder::fromDisk("../../../asset/textures/skyscraper-normal.jpg");
				batch.material.normal = ArchiveImage(image1ID);
				batch.material.normal.width = imgNormal.width;
				batch.material.normal.height = imgNormal.height;
				batch.material.normal.channels = imgNormal.getComponents();
				batch.material.normal.data = std::move(imgNormal.bytes);

				mesh.batches.append(batch);
			}
			app::ArchiveStaticMesh sphereMesh(meshSphereID);
			{ // Cube Mesh
				app::ArchiveBatch batch(batchSphereID);
				batch.geometry = ArchiveGeometry(geometrySphereID);
				batch.geometry.indices = getSphereIndices(1.0, 32, 16);
				batch.geometry.vertices = getSphereVertices(1.0, 32, 16);
				for (uint32_t i = 0; i < batch.geometry.vertices.size(); i++)
					batch.geometry.bounds.include(batch.geometry.vertices[i].position);

				// Material
				batch.material = ArchiveMaterial(materialID);
				batch.material.color = color4f(0.0, 0.0, 1.0, 1.0);

				Image img = ImageDecoder::fromDisk("../../../asset/textures/skyscraper.jpg");
				batch.material.albedo = ArchiveImage(image0ID);
				batch.material.albedo.width = img.width;
				batch.material.albedo.height = img.height;
				batch.material.albedo.channels = img.getComponents();
				batch.material.albedo.data = std::move(img.bytes);

				Image imgNormal = ImageDecoder::fromDisk("../../../asset/textures/skyscraper-normal.jpg");
				batch.material.normal = ArchiveImage(image1ID);
				batch.material.normal.width = imgNormal.width;
				batch.material.normal.height = imgNormal.height;
				batch.material.normal.channels = imgNormal.getComponents();
				batch.material.normal.data = std::move(imgNormal.bytes);

				sphereMesh.batches.append(batch);
			}
			app::ArchiveScene scene(sceneID);
			{ // Scene
				scene.meshes.append(mesh);
				scene.meshes.append(sphereMesh);
				scene.transforms.append(ArchiveSceneTransform{ mat4f::identity() });
				scene.transforms.append(ArchiveSceneTransform{ mat4f::TRS(vec3f(10.f, 0.f, 0.f), quatf::identity(), vec3f(1.f)) });
				scene.transforms.append(ArchiveSceneTransform{ mat4f::TRS(vec3f(5.f, 2.f, 0.f), quatf::identity(), vec3f(1.f)) });
				scene.transforms.append(ArchiveSceneTransform{ mat4f::TRS(vec3f(-5.f, 2.f, 0.f), quatf::identity(), vec3f(2.f)) });
				scene.transforms.append(ArchiveSceneTransform{ mat4f::TRS(vec3f(-5.f, 2.f, 0.f), quatf::identity(), vec3f(1.5f)) });
				scene.entities.append(ArchiveSceneEntity{ 
					"Cube",
					SceneComponentMask::Transform | SceneComponentMask::Hierarchy | SceneComponentMask::StaticMesh,
					{ 
						ArchiveSceneID(0), // t
						InvalidArchiveSceneID, // h
						ArchiveSceneID(0), // sm
						InvalidArchiveSceneID, //pl
						InvalidArchiveSceneID // sl
					}
					});
				scene.entities.append(ArchiveSceneEntity{
					"Cube2",
					SceneComponentMask::Transform | SceneComponentMask::Hierarchy | SceneComponentMask::StaticMesh,
					{
						ArchiveSceneID(1), // t
						InvalidArchiveSceneID, // h
						ArchiveSceneID(0), // sm
						InvalidArchiveSceneID, //pl
						InvalidArchiveSceneID // sl
					}
					});
				scene.entities.append(ArchiveSceneEntity{
					"Sphere1",
					SceneComponentMask::Transform | SceneComponentMask::Hierarchy | SceneComponentMask::StaticMesh,
					{
						ArchiveSceneID(2), // t
						InvalidArchiveSceneID, // h
						ArchiveSceneID(1), // sm
						InvalidArchiveSceneID, //pl
						InvalidArchiveSceneID // sl
					}
					});
				scene.entities.append(ArchiveSceneEntity{
					"Sphere2",
					SceneComponentMask::Transform | SceneComponentMask::Hierarchy | SceneComponentMask::StaticMesh,
					{
						ArchiveSceneID(3), // t
						InvalidArchiveSceneID, // h
						ArchiveSceneID(1), // sm
						InvalidArchiveSceneID, //pl
						InvalidArchiveSceneID // sl
					}
					});
				scene.entities.append(ArchiveSceneEntity{
					"Sphere3",
					SceneComponentMask::Transform | SceneComponentMask::Hierarchy | SceneComponentMask::StaticMesh,
					{
						ArchiveSceneID(4), // t
						ArchiveSceneID(0), // h
						ArchiveSceneID(1), // sm
						InvalidArchiveSceneID, //pl
						InvalidArchiveSceneID // sl
					}
					});
				for (ArchiveSceneEntity& entity : scene.entities)
				{
					SceneComponentMask mask = SceneComponentMask::Transform | SceneComponentMask::StaticMesh;
					if ((entity.components & mask) == mask)
					{
						ArchiveSceneID transformID = entity.id[EnumToIndex(SceneComponent::Transform)];
						ArchiveSceneID meshID = entity.id[EnumToIndex(SceneComponent::StaticMesh)];
						const mat4f& transform = scene.transforms[toIntegral(transformID)].matrix;
						const ArchiveStaticMesh& mesh = scene.meshes[toIntegral(meshID)];
						for (const ArchiveBatch& batch : mesh.batches)
						{
							scene.bounds.include(transform * batch.geometry.bounds);
						}
					}
				}
			}
			watch.start();
			ArchiveSaveResult res = mesh.save(ArchiveSaveContext(&m_library), smeshPath);
			AKA_ASSERT(res == ArchiveSaveResult::Success, "Failed to load mesh.");
			res = scene.save(ArchiveSaveContext(&m_library), scenePath);
			AKA_ASSERT(res == ArchiveSaveResult::Success, "Failed to load scene.");
			Logger::info("Demo save time : ", watch.elapsed(), "ms");
		}
		{ // Test if everything went well
			aka::StopWatch watch;
			app::ArchiveStaticMesh mesh(meshID);
			ArchiveLoadResult res = mesh.load(ArchiveLoadContext(&m_library), smeshPath);
			AKA_ASSERT(res == ArchiveLoadResult::Success, "Failed to load scene.");

			app::ArchiveStaticMesh spheremesh(meshSphereID);
			res = spheremesh.load(ArchiveLoadContext(&m_library), smeshSpherePath);
			AKA_ASSERT(res == ArchiveLoadResult::Success, "Failed to load scene.");

			app::ArchiveScene scene(sceneID);
			res = scene.load(ArchiveLoadContext(&m_library), scenePath);
			AKA_ASSERT(res == ArchiveLoadResult::Success, "Failed to load scene.");
			Logger::info("Demo test load time : ", watch.elapsed(), "ms");
		}
		watch.start();
		m_library.load<app::Scene>(m_sceneID, graphic());
		m_scene = m_library.get<app::Scene>(m_sceneID);
		Logger::info("Demo load time : ", watch.elapsed(), "ms");
#if 1
		watch.start();
		app::AssimpImporter importer;
		importer.import(&m_library, Path("../../../asset/Sponza/sponza.obj"));
		Logger::info("Import time : ", watch.elapsed(), "ms");
#endif
		watch.start();
		m_library.serialize();
		Logger::info("Serialize time : ", watch.elapsed(), "ms");
#else
		aka::StopWatch watch;
		m_library.parse();
		Logger::info("Parsing time : ", watch.elapsed(), "ms");

		app::ResourceID resourceID = app::ResourceID::Invalid;
		app::AssetID assetID = app::AssetID::Invalid;
		for (auto it : m_library.getAssetRange())
		{
			if (it.second.type == app::AssetType::Scene)
			{
				assetID = it.first;
				resourceID = m_library.getResourceID(assetID);
				break;
			}
		}
		watch.start();
		app::ArchiveScene archive(assetID);
		archive.load(app::ArchiveLoadContext(&m_library));
		Logger::info("Loading archive time : ", watch.elapsed(), "ms");

		watch.start();
		m_library.load<app::Scene>(resourceID, archive, graphic());
		m_scene = m_library.get<app::Scene>(resourceID);
		if (m_scene.isLoaded())
		{
			m_cameraController.set(m_scene.get().getBounds());
			m_dirty = true;
		}
		Logger::info("Loading resource time : ", watch.elapsed(), "ms");
#endif
	}*/
	m_library.parse();
}

void Editor::onDestroy()
{
	gfx::GraphicDevice* device = graphic();
	device->destroy(m_cameraUniformBuffer);
	device->destroy(m_cameraDescriptorSet);
	device->destroy(m_renderPipeline);
	device->destroy(m_renderPass);
	device->destroy(m_backbuffer);

	m_library.destroy(device);
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
	EventDispatcher<app::AssetAddedEvent>::dispatch();
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

	gfx::FramebufferHandle backbuffer = device->get(m_backbuffer, _frame);


	cmd->bindPipeline(m_renderPipeline);
	cmd->bindDescriptorSet(0, m_cameraDescriptorSet);
	cmd->bindDescriptorSet(0, m_cameraDescriptorSet);

	cmd->beginRenderPass(m_renderPass, backbuffer, gfx::ClearState{ gfx::ClearMask::All, { 0.1f, 0.1f, 0.1f, 1.f }, 1.f, 0 });
	if (m_scene.isLoaded())
	{
		app::Scene& scene = m_scene.get();
		scene.world.registry().view<app::Transform3DComponent, app::StaticMeshComponent>().each([&](entt::entity entity, app::Transform3DComponent& transformComp, app::StaticMeshComponent& meshComp) {

			if (meshComp.mesh.isLoaded())
			{
				app::StaticMesh& mesh = meshComp.mesh.get();
				//Logger::info("Rendering", mesh.getName());
				cmd->bindVertexBuffer(mesh.gfxVertexBuffer, 0);
				cmd->bindIndexBuffer(mesh.gfxIndexBuffer, gfx::IndexFormat::UnsignedInt, 0);

				// Should use vertex buffer instead
				cmd->bindDescriptorSet(1, meshComp.descriptorSet);
				for (const auto& batch : mesh.batches)
				{
					cmd->bindDescriptorSet(2, batch.gfxDescriptorSet);
					cmd->drawIndexed(batch.indexCount, batch.indexOffset, batch.vertexOffset, 1);
				}
			}
		});
	}
	cmd->endRenderPass();
}

void Editor::onResize(uint32_t width, uint32_t height)
{
	destroyRenderPass();
	createRenderPass();
}

void Editor::onReceive(const app::SceneSwitchEvent& event)
{
	m_scene = event.scene;
	if (event.scene.isLoaded())
	{
		const app::Scene& scene = event.scene.get();
		m_sceneID = scene.getID();
		m_cameraController.set(scene.getBounds());
		m_dirty = true;
	}
}

void Editor::onReceive(const ShaderReloadedEvent& event)
{
	graphic()->wait();
	destroyRenderPass();
	createRenderPass();
}

void Editor::createRenderPass()
{
	gfx::GraphicDevice* device = graphic();
	ShaderRegistry* registry = program();

	m_renderPass = device->createBackbufferRenderPass();
	m_backbuffer = device->createBackbuffer(m_renderPass);
	m_program = registry->get(m_programKey);

	m_renderPipeline = device->createGraphicPipeline(
		"RenderPipeline",
		m_program,
		gfx::PrimitiveType::Triangles,
		device->get(m_renderPass)->state,
		app::Vertex::getState(),
		gfx::ViewportState{}.size(width(), height()),
		gfx::DepthStateLessEqual,
		gfx::StencilStateDisabled,
		gfx::CullStateDisabled,
		gfx::BlendStateDisabled,
		gfx::FillStateFill
	);
}

void Editor::destroyRenderPass()
{
	gfx::GraphicDevice* device = graphic();

	device->destroy(m_renderPass);
	device->destroy(m_backbuffer);
	device->destroy(m_renderPipeline);
}
