#include "Editor.h"

#include <Aka/Core/Container/String.h>
#include <Aka/OS/OS.h>
#include <Aka/Layer/ImGuiLayer.h>

#include <imgui.h>
#include <map>
#include <vector>

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
	mat3f normal;
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
		AKA_ASSERT(device->get(program)->sets[0] == bindings[0], "Invalid bindings");

		// Instance
		bindings[1].add(gfx::ShaderBindingType::UniformBuffer, gfx::ShaderMask::Vertex, 1);
		AKA_ASSERT(device->get(program)->sets[1] == bindings[1], "Invalid bindings");

		// Material
		bindings[2].add(gfx::ShaderBindingType::UniformBuffer, gfx::ShaderMask::Vertex | gfx::ShaderMask::Fragment, 1);
		bindings[2].add(gfx::ShaderBindingType::SampledImage, gfx::ShaderMask::Fragment, 1);
		bindings[2].add(gfx::ShaderBindingType::SampledImage, gfx::ShaderMask::Fragment, 1);
		AKA_ASSERT(device->get(program)->sets[2] == bindings[2], "Invalid bindings");
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
		ubo.normal = mat3f(mat4f::transpose(mat4f::inverse(ubo.model)));
		m_instanceUniformBuffer = device->createBuffer("InstanceBuffer", gfx::BufferType::Uniform, sizeof(InstanceUniformBuffer), gfx::BufferUsage::Default, gfx::BufferCPUAccess::None, &ubo);

		gfx::DescriptorSetData data{};
		data.addUniformBuffer(m_instanceUniformBuffer);
		device->update(m_instanceDescriptorSet, data);
	}


	{ // Create a static mesh & archive it
		using namespace app;
		AssetPath scenePath		= AssetPath("../../../asset/library/scene.scene");
		AssetPath smeshPath		= AssetPath("../../../asset/library/mesh.smesh");
		AssetPath batchPath		= AssetPath("../../../asset/library/mesh.batch");
		AssetPath geoPath		= AssetPath("../../../asset/library/mesh.geo");
		AssetPath materialPath	= AssetPath("../../../asset/library/mesh.mat");
		AssetPath image0Path	= AssetPath("../../../asset/library/albedo.img");
		AssetPath image1Path	= AssetPath("../../../asset/library/normal.img");

		// Add to library & load it.
		AssetID meshID = m_library.registerAsset(smeshPath, AssetType::StaticMesh);
		AssetID batchID = m_library.registerAsset(batchPath, AssetType::MeshBatch);
		AssetID geometryID = m_library.registerAsset(geoPath, AssetType::MeshGeometry);
		AssetID materialID = m_library.registerAsset(materialPath, AssetType::MeshMaterial);
		AssetID image0ID = m_library.registerAsset(image0Path, AssetType::Image);
		AssetID image1ID = m_library.registerAsset(image1Path, AssetType::Image);
		AssetID sceneID = m_library.registerAsset(scenePath, AssetType::Scene);
		m_resourceID = m_library.getResourceID(meshID);
		m_sceneID = m_library.getResourceID(sceneID);
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

				Image img = Image::load("../../../asset/textures/skyscraper.jpg");
				batch.material.albedo = ArchiveImage(image0ID);
				batch.material.albedo.width = img.width();
				batch.material.albedo.height = img.height();
				batch.material.albedo.channels = img.components();
				batch.material.albedo.data.append(img.data(), img.data() + img.size());

				Image imgNormal = Image::load("../../../asset/textures/skyscraper-normal.jpg");
				batch.material.normal = ArchiveImage(image1ID);
				batch.material.normal.width = imgNormal.width();
				batch.material.normal.height = imgNormal.height();
				batch.material.normal.channels = imgNormal.components();
				batch.material.normal.data.append(imgNormal.data(), imgNormal.data() + imgNormal.size());

				mesh.batches.append(batch);
			}
			app::ArchiveScene scene(sceneID);
			{ // Scene
				scene.meshes.append(mesh);
				scene.transforms.append(ArchiveSceneTransform{ mat4f::identity() });
				scene.transforms.append(ArchiveSceneTransform{ mat4f::TRS(vec3f(10.f, 0.f, 0.f), quatf::identity(), vec3f(1.f)) });
				scene.entities.append(ArchiveSceneEntity{ 
					SceneComponentMask::Transform | SceneComponentMask::Hierarchy | SceneComponentMask::StaticMesh,
					{ 
						ArchiveSceneID(0), // t
						ArchiveSceneID(0), // h
						ArchiveSceneID(0), // sm
						ArchiveSceneID(0), //pl
						ArchiveSceneID(0) // sl
					}
					});
				scene.entities.append(ArchiveSceneEntity{
					SceneComponentMask::Transform | SceneComponentMask::Hierarchy | SceneComponentMask::StaticMesh,
					{
						ArchiveSceneID(1), // t
						ArchiveSceneID(0), // h
						ArchiveSceneID(0), // sm
						ArchiveSceneID(0), //pl
						ArchiveSceneID(0) // sl
					}
					});
			}
			ArchiveSaveResult res = mesh.save(&m_library, smeshPath);
			AKA_ASSERT(res == ArchiveSaveResult::Success, "Failed to load mesh.");
			res = scene.save(&m_library, scenePath);
			AKA_ASSERT(res == ArchiveSaveResult::Success, "Failed to load scene.");
		}
		{ // Test if everything went well
			app::ArchiveStaticMesh mesh(meshID);
			ArchiveLoadResult res = mesh.load(&m_library, smeshPath);
			AKA_ASSERT(res == ArchiveLoadResult::Success, "Failed to load scene.");

			app::ArchiveScene scene(sceneID);
			res = scene.load(&m_library, scenePath);
			AKA_ASSERT(res == ArchiveLoadResult::Success, "Failed to load scene.");
		}
		m_resource = m_library.getStaticMesh(m_resourceID);
		m_scene = m_library.getScene(m_sceneID);
		if (m_resource.isLoaded())
		{
			app::StaticMesh& mesh = m_resource.get();
		}
		if (m_scene.isLoaded())
		{
			app::Scene& scene = m_scene.get();
			scene.world.registry().view<app::Transform3DComponent, app::StaticMeshComponent>().each([&](entt::entity entity, app::Transform3DComponent& transformComp, app::StaticMeshComponent& meshComp) {

				m_resource = m_library.getStaticMesh(m_resourceID);
				meshComp.mesh.get();
			});
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

template <typename T>
struct ComponentNode {
	static const char* name() { return "Unknown"; }
	//static const char* icon() { return ""; }
	static bool draw(T& component) { Logger::error("Trying to draw an undefined component"); return false; }
};

template <> const char* ComponentNode<TagComponent>::name() { return "Tag"; }
template <> bool ComponentNode<TagComponent>::draw(TagComponent& tag)
{
	char buffer[256];
	String::copy(buffer, 256, tag.name.cstr());
	if (ImGui::InputText("Name", buffer, 256))
	{
		tag.name = buffer;
		return true;
	}
	return false;
}

template <> const char* ComponentNode<app::Hierarchy3DComponent>::name() { return "Hierarchy"; }
template <> bool ComponentNode<app::Hierarchy3DComponent>::draw(app::Hierarchy3DComponent& hierarchy)
{
	if (hierarchy.parent.handle() == entt::null || !hierarchy.parent.valid())
	{
		ImGui::Text("Parent : None");
	}
	else
	{
		if (hierarchy.parent.has<TagComponent>())
			ImGui::Text("Parent : %s", hierarchy.parent.get<TagComponent>().name.cstr());
		else
			ImGui::Text("Parent : Unknown");
	}
	return false;
}

template <> const char* ComponentNode<app::Transform3DComponent>::name() { return "Transform"; }
template <> bool ComponentNode<app::Transform3DComponent>::draw(app::Transform3DComponent& transform)
{
	bool updated = false;
	float translation[3];
	float rotation[3];
	float scale[3];
	/*ImGuizmo::DecomposeMatrixToComponents(transform.transform.cols[0].data, translation, rotation, scale);
	updated |= ImGui::InputFloat3("Translation", translation);
	updated |= ImGui::InputFloat3("Rotation", rotation);
	updated |= ImGui::InputFloat3("Scale", scale);
	if (updated)
		ImGuizmo::RecomposeMatrixFromComponents(translation, rotation, scale, transform.transform.cols[0].data);*/
	return updated;
}

template <> const char* ComponentNode<app::StaticMeshComponent>::name() { return "Mesh"; }
template <> bool ComponentNode<app::StaticMeshComponent>::draw(app::StaticMeshComponent& mesh)
{
	if (mesh.mesh.isLoaded())
	{
		app::StaticMesh& m = mesh.mesh.get();
		uint32_t sizeOfVertex = 0;
		
		//ImGui::Text("Vertices : %u", mesh.mesh->vertices[0].data->size / sizeOfVertex);
		//ImGui::Text("Index count : %u", mesh.mesh->count);
		//ImGui::Text("Index offset : %u", 0);// mesh.mesh->offset);
		/*String type = "Undefined";
		switch (mesh.mesh->type)
		{
		case PrimitiveType::Lines:
			type = "Lines";
			break;
		case PrimitiveType::Triangles:
			type = "Triangles";
			break;
		case PrimitiveType::Points:
			type = "Points";
			break;
		}
		ImGui::Text("Primitive : %s", type.cstr());*/
		//ImGui::Text("Bounds min : (%f, %f, %f)", mesh.bounds.min.x, mesh.bounds.min.y, mesh.bounds.min.z);
		//ImGui::Text("Bounds max : (%f, %f, %f)", mesh.bounds.max.x, mesh.bounds.max.y, mesh.bounds.max.z);
	}
	else
	{
		ImGui::Text("No mesh data");
	}
	return false;
}

template <typename T>
void component(World& world, entt::entity entity)
{
	static char buffer[256];
	if (world.registry().has<T>(entity))
	{
		T& component = world.registry().get<T>(entity);
		snprintf(buffer, 256, "%s##%p", ComponentNode<T>::name(), &component);
		if (ImGui::TreeNodeEx(buffer, ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_DefaultOpen))
		{
			snprintf(buffer, 256, "ClosePopUp##%p", &component);
			if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1))
				ImGui::OpenPopup(buffer);
			if (ComponentNode<T>::draw(component))
			{
				world.registry().patch<T>(entity);
			}
			if (ImGui::BeginPopupContextItem(buffer))
			{
				if (ImGui::MenuItem("Remove"))
					world.registry().remove<T>(entity);
				ImGui::EndPopup();
			}
			ImGui::TreePop();
		}
	}
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

	if (m_scene.isLoaded())
	{
		app::Scene& scene = m_scene.get();
		scene.world.registry().view<app::Transform3DComponent, app::StaticMeshComponent>().each([&](entt::entity entity, app::Transform3DComponent& transformComp, app::StaticMeshComponent& meshComp) {

			if (meshComp.mesh.isLoaded())
			{
				app::StaticMesh& mesh = meshComp.mesh.get();
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
		});
		/*app::StaticMesh& mesh = m_resource.get();
		cmd->bindVertexBuffer(mesh.gfxVertexBuffer, 0);
		cmd->bindIndexBuffer(mesh.gfxIndexBuffer, gfx::IndexFormat::UnsignedInt, 0);

		cmd->beginRenderPass(m_renderPass, backbuffer, gfx::ClearState{ gfx::ClearMask::All, { 0.1f, 0.1f, 0.1f, 1.f }, 1.f, 0 });
		for (const auto& batch : mesh.batches)
		{
			cmd->bindDescriptorSet(2, batch.gfxDescriptorSet);
			cmd->drawIndexed(batch.indexCount, batch.indexOffset, batch.vertexOffset, 1);
		}
		cmd->endRenderPass();*/
	}


	{
		if (ImGui::Begin("ArchiveEditor"))
		{
			// We should list all archive from library here.
			// We could open them in separate tab to edit their content & save them.
			ImGui::TextColored(ImVec4(0.0, 0.0, 1.0, 1.0), "Assets");
			for (auto asset : m_library.getAssetRange()) // TODO sort them by type ?
			{
				//asset.second.type
				ImGui::Text("AssetID: %3u | Path: %s", (uint32_t)asset.first, asset.second.path.getPath().cstr());
			}
			ImGui::Separator();
			auto getStatusString = [](app::ResourceState state) -> String {
				switch (state)
				{
				case app::ResourceState::Disk: return "Disk";
				case app::ResourceState::Loaded: return "Loaded";
				case app::ResourceState::Pending: return "Pending";
				case app::ResourceState::Unknown: return "Unknown";
				default:
					break;
				}
				};
			ImGui::TextColored(ImVec4(0.0, 0.0, 1.0, 1.0), "Resources");
			ImGui::TextColored(ImVec4(0.0, 1.0, 0.0, 1.0), "Meshes");
			for (auto resPair : m_library.getStaticMeshRange())
			{
				app::ResourceID resID = resPair.first;
				app::ResourceHandle<app::StaticMesh>& resHandle = resPair.second;
				uint32_t batchCount;
				String status;
				if (resHandle.isLoaded())
				{
					const app::StaticMesh& mesh = resHandle.get();
					batchCount = mesh.batches.size();
					mesh.attributes;
					status = getStatusString(resHandle.getState());
				}
				else
				{
					batchCount = 0;
					status = getStatusString(app::ResourceState::Unknown);
				}
				ImGui::Text("ResourceID: %3u | Batches: %2u | Status: %s", (uint32_t)resID, batchCount, status.cstr());
			}
			ImGui::TextColored(ImVec4(0.0, 1.0, 0.0, 1.0), "Scenes");
			for (auto resPair : m_library.getSceneRange())
			{
				app::ResourceID resID = resPair.first;
				app::ResourceHandle<app::Scene>& resHandle = resPair.second;
				uint32_t entityCount;
				String status;
				if (resHandle.isLoaded())
				{
					const app::Scene& scene = resHandle.get();
					entityCount = (uint32_t)scene.world.registry().size();
					status = getStatusString(resHandle.getState());
				}
				else
				{
					entityCount = 0;
					status = getStatusString(app::ResourceState::Unknown);
				}
				ImGui::Text("ResourceID: %3u | Entities: %2u | Status: %s", (uint32_t)resID, entityCount, status.cstr());
			}
		}
		ImGui::End();
	}
	if (m_scene.isLoaded())
	{
		std::function<void(World&, entt::entity, const std::map<entt::entity, std::vector<entt::entity>>&, entt::entity&)> recurse;
		recurse = [&recurse](World& world, entt::entity entity, const std::map<entt::entity, std::vector<entt::entity>>& childrens, entt::entity& current)
			{
				char buffer[256];
				const TagComponent& tag = world.registry().get<TagComponent>(entity);

				auto it = childrens.find(entity);
				if (it != childrens.end())
				{
					int err = snprintf(buffer, 256, "%s##%p", tag.name.cstr(), &tag);
					ImGuiTreeNodeFlags flags = 0;
					if (entity == current)
						flags |= ImGuiTreeNodeFlags_Selected;
					if (ImGui::TreeNodeEx(buffer, flags))
					{
						err = snprintf(buffer, 256, "ClosePopUp##%p", &tag);
						if (ImGui::IsItemClicked())
							current = entity;
						if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(1))
							ImGui::OpenPopup(buffer);
						if (ImGui::BeginPopupContextItem(buffer))
						{
							if (ImGui::MenuItem("Delete"))
								world.registry().destroy(entity);
							ImGui::EndPopup();
						}
						for (entt::entity e : it->second)
							recurse(world, e, childrens, current);
						ImGui::TreePop();
					}
				}
				else
				{
					int err = snprintf(buffer, 256, "ClosePopUp##%p", &tag);
					ImGui::Bullet();
					bool isSelected = current == entity;
					if (ImGui::Selectable(tag.name.cstr(), &isSelected))
						current = entity;
					if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(1))
						ImGui::OpenPopup(buffer);
					if (ImGui::BeginPopupContextItem(buffer))
					{
						if (ImGui::MenuItem("Delete"))
							world.registry().destroy(entity);
						ImGui::EndPopup();
					}
				}
			};

		app::Scene& scene = m_scene.get();
		static entt::entity m_currentEntity;
		static char m_newEntityName[256];
		if (ImGui::Begin("Scene", nullptr, ImGuiWindowFlags_MenuBar))
		{
			static const ImVec4 color = ImVec4(0.93f, 0.04f, 0.26f, 1.f);
			// --- Menu
			Entity e = Entity(m_currentEntity, &scene.world);
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("World"))
				{
					if (ImGui::MenuItem("Save"))
					{
						//Scene::save("library/scene.json", world);
					}
					if (ImGui::MenuItem("Load"))
					{
						//Scene::load(world, "library/scene.json");
					}
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Entity"))
				{
					if (ImGui::BeginMenu("Create"))
					{
						mat4f id = mat4f::identity();
						if (ImGui::BeginMenu("Mesh"))
						{
							if (ImGui::MenuItem("Cube"))
							{
								//m_currentEntity = Scene::createCubeEntity(world).handle();
							}
							if (ImGui::MenuItem("UV Sphere"))
							{
								//m_currentEntity = Scene::createSphereEntity(world, 32, 16).handle();
							}
							ImGui::EndMenu();
						}
						if (ImGui::BeginMenu("Light"))
						{
							if (ImGui::MenuItem("Point light"))
							{
								//m_currentEntity = Scene::createPointLightEntity(world).handle();
							}
							if (ImGui::MenuItem("Directional light"))
							{
								//m_currentEntity = Scene::createDirectionalLightEntity(world).handle();
							}
							ImGui::EndMenu();
						}

						if (ImGui::MenuItem("Camera"))
						{
							//m_currentEntity = Scene::createArcballCameraEntity(world).handle();
						}
						if (ImGui::MenuItem("Empty"))
						{
							//m_currentEntity = world.createEntity("New empty").handle();
						}
						ImGui::EndMenu();
					}
					if (ImGui::MenuItem("Destroy", nullptr, nullptr, e.valid()))
						e.destroy();
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Component", e.valid()))
				{
					if (ImGui::BeginMenu("Add", e.valid()))
					{
						if (ImGui::MenuItem("Transform", nullptr, nullptr, !e.has<app::Transform3DComponent>()))
							e.add<app::Transform3DComponent>(app::Transform3DComponent{ mat4f::identity() });
						if (ImGui::MenuItem("Hierarchy", nullptr, nullptr, !e.has<app::Hierarchy3DComponent>()))
							e.add<app::Hierarchy3DComponent>(app::Hierarchy3DComponent{ Entity::null(), mat4f::identity() });
						/*if (ImGui::MenuItem("Camera", nullptr, nullptr, !e.has<app::Camera3DComponent>()))
						{
							auto p = std::make_unique<CameraPerspective>();
							p->hFov = anglef::degree(60.f);
							p->nearZ = 0.01f;
							p->farZ = 100.f;
							p->ratio = 1.f;
							auto c = std::make_unique<CameraArcball>();
							c->set(aabbox<>(point3f(0.f), point3f(1.f)));
							e.add<Camera3DComponent>(Camera3DComponent{
								mat4f::identity(),
								std::move(p),
								std::move(c)
								});
						}*/
						if (ImGui::MenuItem("Mesh", nullptr, nullptr, !e.has<app::StaticMeshComponent>()))
							e.add<app::StaticMeshComponent>(app::StaticMeshComponent{});
						//if (ImGui::MenuItem("Material", nullptr, nullptr, !e.has<MaterialComponent>()))
						//	e.add<MaterialComponent>(MaterialComponent{ color4f(1.f), false, { nullptr, TextureSampler::nearest}, { nullptr, TextureSampler::nearest}, { nullptr, TextureSampler::nearest} });
						/*if (ImGui::MenuItem("Point light", nullptr, nullptr, !e.has<PointLightComponent>()))
							e.add<PointLightComponent>(PointLightComponent{
								color3f(1.f), 1.f, {}
								});
						if (ImGui::MenuItem("Directional light", nullptr, nullptr, !e.has<DirectionalLightComponent>()))
							e.add<DirectionalLightComponent>(DirectionalLightComponent{
								vec3f(0,1,0),
								color3f(1.f), 1.f, {}
								});
						if (ImGui::BeginMenu("Text", !e.has<TextComponent>()))
						{
							FontAllocator& allocator = resources->allocator<Font>();
							for (auto& r : allocator)
							{
								//if (ImGui::MenuItem(r.first.cstr(), nullptr, nullptr, !e.has<TextComponent>()))
								//	e.add<TextComponent>(TextComponent{ r.second.resource.get(), TextureSampler::nearest, "", color4f(1.f) });
							}
							ImGui::EndMenu();
						}*/
						ImGui::EndMenu();
					}
					if (ImGui::BeginMenu("Remove", e.valid()))
					{
						if (ImGui::MenuItem("Transform", nullptr, nullptr, e.has<app::Transform3DComponent>()))
							e.remove<app::Transform3DComponent>();
						if (ImGui::MenuItem("Hierarchy", nullptr, nullptr, e.has<app::Hierarchy3DComponent>()))
							e.remove<app::Hierarchy3DComponent>();
						//if (ImGui::MenuItem("Camera", nullptr, nullptr, e.has<Camera3DComponent>()))
						//	e.remove<Camera3DComponent>();
						if (ImGui::MenuItem("Mesh", nullptr, nullptr, e.has<app::StaticMeshComponent>()))
							e.remove<app::StaticMeshComponent>();
						ImGui::EndMenu();
					}
					ImGui::EndMenu();
				}
				/*if (ImGui::BeginMenu("Transform operation"))
				{
					bool enabled = m_gizmoOperation == ImGuizmo::TRANSLATE;
					if (ImGui::MenuItem("Translate", nullptr, &enabled))
						m_gizmoOperation = ImGuizmo::TRANSLATE;
					enabled = m_gizmoOperation == ImGuizmo::ROTATE;
					if (ImGui::MenuItem("Rotate", nullptr, &enabled))
						m_gizmoOperation = ImGuizmo::ROTATE;
					enabled = m_gizmoOperation == ImGuizmo::SCALE;
					if (ImGui::MenuItem("Scale", nullptr, &enabled))
						m_gizmoOperation = ImGuizmo::SCALE;
					ImGui::Separator();
					enabled = m_gizmoOperation == ImGuizmo::TRANSLATE_X;
					if (ImGui::MenuItem("TranslateX", nullptr, &enabled))
						m_gizmoOperation = ImGuizmo::TRANSLATE_X;
					enabled = m_gizmoOperation == ImGuizmo::TRANSLATE_Y;
					if (ImGui::MenuItem("TranslateY", nullptr, &enabled))
						m_gizmoOperation = ImGuizmo::TRANSLATE_Y;
					enabled = m_gizmoOperation == ImGuizmo::TRANSLATE_Z;
					if (ImGui::MenuItem("TranslateZ", nullptr, &enabled))
						m_gizmoOperation = ImGuizmo::TRANSLATE_Z;
					ImGui::Separator();
					enabled = m_gizmoOperation == ImGuizmo::ROTATE_X;
					if (ImGui::MenuItem("RotateX", nullptr, &enabled))
						m_gizmoOperation = ImGuizmo::ROTATE_X;
					enabled = m_gizmoOperation == ImGuizmo::ROTATE_Y;
					if (ImGui::MenuItem("RotateY", nullptr, &enabled))
						m_gizmoOperation = ImGuizmo::ROTATE_Y;
					enabled = m_gizmoOperation == ImGuizmo::ROTATE_Z;
					if (ImGui::MenuItem("RotateZ", nullptr, &enabled))
						m_gizmoOperation = ImGuizmo::ROTATE_Z;
					ImGui::Separator();
					enabled = m_gizmoOperation == ImGuizmo::SCALE_X;
					if (ImGui::MenuItem("ScaleX", nullptr, &enabled))
						m_gizmoOperation = ImGuizmo::SCALE_X;
					enabled = m_gizmoOperation == ImGuizmo::SCALE_Y;
					if (ImGui::MenuItem("ScaleY", nullptr, &enabled))
						m_gizmoOperation = ImGuizmo::SCALE_Y;
					enabled = m_gizmoOperation == ImGuizmo::SCALE_Z;
					if (ImGui::MenuItem("ScaleZ", nullptr, &enabled))
						m_gizmoOperation = ImGuizmo::SCALE_Z;
					ImGui::EndMenu();
				}*/
				ImGui::EndMenuBar();
			}
			// --- Graph
			// TODO do not compute child map every cycle
			// listen to event ?
			std::map<entt::entity, std::vector<entt::entity>> childrens;
			std::vector<entt::entity> roots;
			scene.world.registry().each([&](entt::entity entity) {
				if (scene.world.registry().has<app::Hierarchy3DComponent>(entity))
				{
					const app::Hierarchy3DComponent& h = scene.world.registry().get<app::Hierarchy3DComponent>(entity);
					if (!h.parent.valid())
						roots.push_back(entity);
					else
						childrens[h.parent.handle()].push_back(entity);
				}
				else
					roots.push_back(entity);
				});
			ImGui::TextColored(color, "Graph");
			if (ImGui::BeginChild("##list", ImVec2(0, 200), true))
			{
				for (entt::entity e : roots)
					recurse(scene.world, e, childrens, m_currentEntity);
			}
			ImGui::EndChild();

			// --- Add entity
			ImGui::InputTextWithHint("##entityName", "Entity name", m_newEntityName, 256);
			ImGui::SameLine();
			if (ImGui::Button("Create entity"))
			{
				m_currentEntity = scene.world.createEntity(m_newEntityName).handle();
			}
			ImGui::Separator();

			// --- Entity info
			ImGui::TextColored(color, "Entity");
			if (m_currentEntity != entt::null && scene.world.registry().valid(m_currentEntity))
			{
				if (scene.world.registry().orphan(m_currentEntity))
				{
					ImGui::Text("Add a component to the entity.");
				}
				else
				{
					// Draw every component.
					component<TagComponent>(scene.world, m_currentEntity);
					component<app::Transform3DComponent>(scene.world, m_currentEntity);
					component<app::Hierarchy3DComponent>(scene.world, m_currentEntity);
					component<app::StaticMeshComponent>(scene.world, m_currentEntity);
				}
			}
		}
		ImGui::End();
	}
}
