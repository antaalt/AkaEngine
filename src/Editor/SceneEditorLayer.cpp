#include "SceneEditorLayer.hpp"

#include <Aka/Layer/ImGuiLayer.h>
#include <Aka/Scene/Component/CameraComponent.hpp>
#include <Aka/Scene/Component/StaticMeshComponent.hpp>
#include <Aka/Resource/AssetLibrary.hpp>

#include "../Component/CustomComponent.hpp"

namespace app {

using namespace aka;

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

Vector<StaticVertex> getSphereVertices(float radius, uint32_t segmentCount, uint32_t ringCount)
{
	// http://www.songho.ca/opengl/gl_sphere.html
	Vector<StaticVertex> vertices;

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
			StaticVertex v;
			ringAngle = (float)j * sectorStep; // starting from 0 to 2pi

			v.position.x = xy * cos(ringAngle); // r * cos(u) * cos(v)
			v.position.y = xy * sin(ringAngle); // r * cos(u) * sin(v)
			v.position.z = z;

			v.normal = norm3f(v.position / radius);

			v.uv.u = (float)j / ringCount;
			v.uv.v = (float)i / segmentCount;
			v.color = color4f(1.f);
			vertices.append(v);
			bounds.include(v.position);
		}
	}
	return vertices;
}

Vector<uint32_t> getSphereIndices(float radius, uint32_t segmentCount, uint32_t ringCount)
{
	Vector<uint32_t> indices;
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
				indices.append(k1);
				indices.append(k2);
				indices.append(k1 + 1);
			}
			// k1+1 => k2 => k2+1
			if (i != (segmentCount - 1))
			{
				indices.append(k1 + 1);
				indices.append(k2);
				indices.append(k2 + 1);
			}
		}
	}
	return indices;
}

AssetID createSphereMesh(AssetLibrary* _library, Renderer* _renderer)
{
	AssetPath meshPath = AssetPath("shapes/sphere/sphere.smesh");
	AssetPath batchPath = AssetPath("shapes/sphere/sphere.batch");
	AssetPath geoPath = AssetPath("shapes/sphere/sphere.geo");
	AssetPath materialPath = AssetPath("shapes/sphere/sphere.mat");
	AssetPath imageAlbedoPath = AssetPath("shapes/sphere/albedo.img");
	AssetPath imageNormalPath = AssetPath("shapes/sphere/normal.img");
	OS::Directory::create(AssetPath("shapes/sphere/").getAbsolutePath());

	// Add to library & load it.
	AssetID meshID = _library->registerAsset(meshPath, AssetType::StaticMesh);
	AssetID batchID = _library->registerAsset(batchPath, AssetType::Batch);
	AssetID geometryID = _library->registerAsset(geoPath, AssetType::Geometry);
	AssetID materialID = _library->registerAsset(materialPath, AssetType::Material);
	AssetID imageAlbedoID = _library->registerAsset(imageAlbedoPath, AssetType::Image);
	AssetID imageNormalID = _library->registerAsset(imageNormalPath, AssetType::Image);
	
	ArchiveBatch batch(batchID);
	batch.geometry = ArchiveGeometry(geometryID);
	batch.geometry.indices = getSphereIndices(1.0, 32, 16);
	batch.geometry.vertices = getSphereVertices(1.0, 32, 16);
	for (uint32_t i = 0; i < batch.geometry.vertices.size(); i++)
		batch.geometry.bounds.include(batch.geometry.vertices[i].position);

	// Material
	batch.material = ArchiveMaterial(materialID);
	batch.material.color = color4f(0.0, 0.0, 1.0, 1.0);

	Image img = ImageDecoder::fromDisk("../../../asset/textures/skyscraper.jpg");
	batch.material.albedo = ArchiveImage(imageAlbedoID);
	batch.material.albedo.width = img.width;
	batch.material.albedo.height = img.height;
	batch.material.albedo.channels = img.getComponents();
	batch.material.albedo.data = std::move(img.bytes);

	Image imgNormal = ImageDecoder::fromDisk("../../../asset/textures/skyscraper-normal.jpg");
	batch.material.normal = ArchiveImage(imageNormalID);
	batch.material.normal.width = imgNormal.width;
	batch.material.normal.height = imgNormal.height;
	batch.material.normal.channels = imgNormal.getComponents();
	batch.material.normal.data = std::move(imgNormal.bytes);

	ArchiveStaticMesh sphereMesh(meshID);
	sphereMesh.batches.append(batch);
	ArchiveSaveResult res = sphereMesh.save(ArchiveSaveContext(_library));

	_library->load<StaticMesh>(meshID, sphereMesh, _renderer);
	return meshID;
}

AssetID createCubeMesh(AssetLibrary* _library, Renderer* _renderer)
{
	AssetPath meshPath = AssetPath("shapes/cube/cube.smesh");
	AssetPath batchPath = AssetPath("shapes/cube/cube.batch");
	AssetPath geoPath = AssetPath("shapes/cube/cube.geo");
	AssetPath materialPath = AssetPath("shapes/cube/cube.mat");
	AssetPath imageAlbedoPath = AssetPath("shapes/cube/albedo.img");
	AssetPath imageNormalPath = AssetPath("shapes/cube/normal.img");
	OS::Directory::create(AssetPath("shapes/cube/").getAbsolutePath());

	// Add to library & load it.
	AssetID meshID = _library->registerAsset(meshPath, AssetType::StaticMesh);
	AssetID batchID = _library->registerAsset(batchPath, AssetType::Batch);
	AssetID geometryID = _library->registerAsset(geoPath, AssetType::Geometry);
	AssetID materialID = _library->registerAsset(materialPath, AssetType::Material);
	AssetID imageAlbedoID = _library->registerAsset(imageAlbedoPath, AssetType::Image);
	AssetID imageNormalID = _library->registerAsset(imageNormalPath, AssetType::Image);

	ArchiveBatch batch(batchID);
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
	batch.material.albedo = ArchiveImage(imageAlbedoID);
	batch.material.albedo.width = img.width;
	batch.material.albedo.height = img.height;
	batch.material.albedo.channels = img.getComponents();
	batch.material.albedo.data = std::move(img.bytes);

	Image imgNormal = ImageDecoder::fromDisk("../../../asset/textures/skyscraper-normal.jpg");
	batch.material.normal = ArchiveImage(imageNormalID);
	batch.material.normal.width = imgNormal.width;
	batch.material.normal.height = imgNormal.height;
	batch.material.normal.channels = imgNormal.getComponents();
	batch.material.normal.data = std::move(imgNormal.bytes);

	ArchiveStaticMesh sphereMesh(meshID);
	sphereMesh.batches.append(batch);
	ArchiveSaveResult res = sphereMesh.save(ArchiveSaveContext(_library));
	_library->load<StaticMesh>(meshID, sphereMesh, _renderer);
	return meshID;
}

SceneEditorLayer::SceneEditorLayer() :
	EditorLayer("Scene Editor")
{
}

void SceneEditorLayer::onCreate(aka::gfx::GraphicDevice* _device)
{
}

void SceneEditorLayer::onDestroy(aka::gfx::GraphicDevice* _device)
{
}

void SceneEditorLayer::onFrame()
{
}

template <typename T>
struct ComponentNode 
{
	static const char* name() { return "Unknown"; }
	//static const char* icon() { return ""; }
	static bool draw(T& component) { AKA_ASSERT(false, "Trying to draw an undefined component"); return false; }
};

template <> const char* ComponentNode<StaticMeshComponent>::name() { return "MeshComponent"; }
template <> bool ComponentNode<StaticMeshComponent>::draw(StaticMeshComponent& mesh)
{
	if (mesh.getMesh().isLoaded())
	{
		StaticMesh& m = mesh.getMesh().get();
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


template <> const char* ComponentNode<CameraComponent>::name() { return "CameraComponent"; }
template <> bool ComponentNode<CameraComponent>::draw(CameraComponent& mesh)
{
	ImGui::Text("tis a camera");
	return false;
}

template <> const char* ComponentNode<CustomComponent>::name() { return "CustomComponent"; }
template <> bool ComponentNode<CustomComponent>::draw(CustomComponent& mesh)
{
	ImGui::Text(mesh.CustomData.cstr());
	return false;
}

template <typename T>
void component(Node* current)
{
	static char buffer[256];
	if (current->has<T>())
	{
		T& component = current->get<T>();
		int res = snprintf(buffer, 256, "%s##%p", ComponentNode<T>::name(), &component);
		if (ImGui::TreeNodeEx(buffer, ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_DefaultOpen))
		{
			snprintf(buffer, 256, "ClosePopUp##%p", &component);
			if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1))
				ImGui::OpenPopup(buffer);
			if (ComponentNode<T>::draw(component))
			{
				current->setDirty<T>();
			}
			if (ImGui::BeginPopupContextItem(buffer))
			{
				if (ImGui::MenuItem("Remove"))
					current->detach<T>();
				ImGui::EndPopup();
			}
			ImGui::TreePop();
		}
	}
}

void SceneEditorLayer::onRender(aka::gfx::GraphicDevice* _device, aka::gfx::Frame* frame)
{
	if (m_nodeToDestroy)
	{
		//AKA_ASSERT(false, "");
		m_nodeToDestroy->unlink();
		m_nodeToDestroy->destroy(m_library, Application::app()->renderer());
		m_scene.get().destroyChild(m_nodeToDestroy);
		m_nodeToDestroy = nullptr;
	}
}

void SceneEditorLayer::onDrawUI()
{
	std::function<void(Node* parent, Node*& current)> recurse;
	recurse = [&recurse, this](Node* parent, Node*& current)
		{
			char buffer[256];
			uint32_t childCount = parent->getChildCount();
			if (childCount > 0)
			{
				int err = snprintf(buffer, 256, "%s##%p", parent->getName().cstr(), parent);
				ImGuiTreeNodeFlags flags = 0;
				if (parent == current)
					flags |= ImGuiTreeNodeFlags_Selected;
				if (ImGui::TreeNodeEx(buffer, flags))
				{
					err = snprintf(buffer, 256, "ClosePopUp##%p", parent);
					if (ImGui::IsItemClicked())
						current = parent;
					if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(1))
						ImGui::OpenPopup(buffer);
					if (ImGui::BeginPopupContextItem(buffer))
					{
						if (ImGui::MenuItem("Delete"))
							m_nodeToDestroy = parent;
						ImGui::EndPopup();
					}
					for (uint32_t iChild = 0; iChild < childCount; iChild++)
						recurse(parent->getChild(iChild), current);
					ImGui::TreePop();
				}
			}
			else
			{
				int err = snprintf(buffer, 256, "ClosePopUp##%p", parent);
				ImGui::Bullet();
				bool isSelected = current == parent;
				if (ImGui::Selectable(parent->getName().cstr(), &isSelected))
					current = parent;
				if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(1))
					ImGui::OpenPopup(buffer);
				if (ImGui::BeginPopupContextItem(buffer))
				{
					if (ImGui::MenuItem("Delete"))
						m_nodeToDestroy = parent;
					ImGui::EndPopup();
				}
			}
		};
	bool resetScene = false;
	bool newScene = false;
	const bool isLoaded = m_scene.isLoaded();
	{
		// --- Menu
		Node* rootNode = isLoaded ? &m_scene.get().getRoot() : nullptr;
		const bool isValid = m_currentNode != nullptr;
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New"))
				{
					newScene = true;
				}
				if (ImGui::MenuItem("Close"))
				{
					resetScene = true;
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Save", nullptr, nullptr, isLoaded))
				{
					if (isLoaded)
					{
						ArchiveScene archive(m_scene.get().getID());
						m_scene.get().save(m_library, Application::app()->renderer(), archive);
						ArchiveSaveResult res = archive.save(ArchiveSaveContext(m_library));
					}
				}
				if (ImGui::MenuItem("Load"))
				{
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Node"))
			{
				if (ImGui::BeginMenu("Create"))
				{
					if (ImGui::BeginMenu("Mesh"))
					{
						if (ImGui::MenuItem("Cube", nullptr, nullptr, isLoaded))
						{
							m_currentNode = m_scene.get().createChild(m_currentNode, "Sphere node");
							m_currentNode->attach<StaticMeshComponent>(createCubeMesh(m_library, Application::app()->renderer()));
						}
						if (ImGui::MenuItem("UV Sphere", nullptr, nullptr, isLoaded))
						{
							m_currentNode = m_scene.get().createChild(m_currentNode, "Sphere node");
							m_currentNode->attach<StaticMeshComponent>(createSphereMesh(m_library, Application::app()->renderer()));
						}
						ImGui::EndMenu();
					}
					if (ImGui::BeginMenu("Light"))
					{
						if (ImGui::MenuItem("Point light", nullptr, nullptr, isLoaded))
						{
							//m_currentNode = Scene::createPointLightNode(world).handle();
						}
						if (ImGui::MenuItem("Directional light", nullptr, nullptr, isLoaded))
						{
							//m_currentNode = Scene::createDirectionalLightNode(world).handle();
						}
						ImGui::EndMenu();
					}

					if (ImGui::MenuItem("Camera", nullptr, nullptr, isLoaded))
					{
						m_currentNode = m_scene.get().createChild(m_currentNode, "Camera");
						m_currentNode->attach<CameraComponent>();
					}
					if (ImGui::MenuItem("Empty", nullptr, nullptr, isLoaded))
					{
						m_currentNode = m_scene.get().createChild(m_currentNode, "Empty");
					}
					ImGui::EndMenu();
				}
				if (ImGui::MenuItem("Destroy", nullptr, nullptr, isLoaded && isValid))
					m_nodeToDestroy = m_currentNode;
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Component", isLoaded && isValid))
			{
				if (ImGui::BeginMenu("Add", isLoaded && isValid))
				{
					if (ImGui::MenuItem("Mesh", nullptr, nullptr, isLoaded && isValid && !m_currentNode->has<StaticMeshComponent>()))
						m_currentNode->attach<StaticMeshComponent>(AssetID::Invalid);
					if (ImGui::MenuItem("Camera", nullptr, nullptr, isLoaded && isValid && !m_currentNode->has<CameraComponent>()))
						m_currentNode->attach<CameraComponent>();
					if (ImGui::MenuItem("CustomComponent", nullptr, nullptr, isLoaded && isValid && !m_currentNode->has<CustomComponent>()))
						m_currentNode->attach<CustomComponent>();
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Remove", isLoaded && isValid))
				{
					if (ImGui::MenuItem("Mesh", nullptr, nullptr, isLoaded && m_currentNode->has<StaticMeshComponent>()))
						m_currentNode->detach<StaticMeshComponent>();
					if (ImGui::MenuItem("Camera", nullptr, nullptr, isLoaded && m_currentNode->has<CameraComponent>()))
						m_currentNode->detach<CameraComponent>();
					if (ImGui::MenuItem("CustomComponent", nullptr, nullptr, isLoaded && m_currentNode->has<CustomComponent>()))
						m_currentNode->detach<CustomComponent>();
					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Transform operation"))
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
			}
			ImGui::EndMenuBar();
		}
		if (ImGui::BeginChild("##list", ImVec2(0, 200), true))
		{
			if (isLoaded)
			{
				for (uint32_t iChild = 0; iChild < rootNode->getChildCount(); iChild++)
				{
					recurse(rootNode->getChild(iChild), m_currentNode);
				}
			}
		}
		ImGui::EndChild();

		// --- Add entity
		static char m_newEntityName[256];
		ImGui::InputTextWithHint("##entityName", "Entity name", m_newEntityName, 256);
		ImGui::SameLine();
		if (ImGui::Button("Create entity") && isLoaded)
		{
			Node* parentNode = m_currentNode ? m_currentNode : rootNode;
			m_currentNode = m_scene.get().createChild(parentNode, m_newEntityName);
		}
		ImGui::Separator();

		// --- Entity info
		if (isLoaded)
		{
			Scene& scene = m_scene.get();
			if (m_currentNode)
			{
				ImGui::TextColored(ImGuiLayer::Color::red, "%s", m_currentNode->getName().cstr());
				if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_DefaultOpen))
				{
					bool updatedTransform = false;
					mat4f& transform = m_currentNode->getLocalTransform();
					float translation[3];
					float rotation[3];
					float scale[3];
					ImGuizmo::DecomposeMatrixToComponents(transform.cols[0].data, translation, rotation, scale);
					updatedTransform |= ImGui::InputFloat3("Translation", translation);
					updatedTransform |= ImGui::InputFloat3("Rotation", rotation);
					updatedTransform |= ImGui::InputFloat3("Scale", scale);
					if (updatedTransform)
						ImGuizmo::RecomposeMatrixFromComponents(translation, rotation, scale, transform.cols[0].data);

					mat4f view = m_cameraController->view();
					mat4f projection = m_cameraProjection->projection();
					// Draw gizmo axis
					ImGuiIO& io = ImGui::GetIO();
					ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
					updatedTransform |= ImGuizmo::Manipulate(view[0].data, projection[0].data, m_gizmoOperation, ImGuizmo::MODE::LOCAL, transform[0].data);

					if (updatedTransform)
						m_currentNode->setFlag(NodeUpdateFlag::Transform);

					ImGui::TreePop();
				}
				if (m_currentNode->isOrphan())
				{
					ImGui::Text("Add a component to the entity.");
				}
				else
				{
					// Draw every component.
					component<StaticMeshComponent>(m_currentNode);
					component<CameraComponent>(m_currentNode);
					component<CustomComponent>(m_currentNode);
				}
			}
			else
			{
				ImGui::TextColored(ImGuiLayer::Color::red, "No node selected");
			}
		}
	}
	if (resetScene && m_scene.isLoaded())
	{
		AssetID assetID = m_scene.get().getID();
		m_scene = ResourceHandle<Scene>::invalid();
		m_library->unload<Scene>(assetID, Application::app()->renderer());
		m_currentNode = nullptr;
		m_nodeToDestroy = nullptr;
	}
	if (newScene)
	{
		// If we want to create a new scene, letsss go.
		// Asset ID based on path, so...
		// Should have an asset ID based on something else
	}
}

void SceneEditorLayer::onPresent()
{
}

void SceneEditorLayer::onResize(uint32_t width, uint32_t height)
{
}

void SceneEditorLayer::onReceive(const aka::ResourceLoadedEvent& event)
{
	if (event.type == ResourceType::Scene)
	{
		setCurrentScene(Application::app()->assets()->get<Scene>(event.resource));
	}
}

void SceneEditorLayer::setLibrary(aka::AssetLibrary* library)
{
	m_library = library;
}

void SceneEditorLayer::setCurrentScene(ResourceHandle<Scene> _scene)
{
	// TODO handle multiple scene with tabs ?
	setVisible(true);
	setEnabled(true);
	m_scene = _scene;
	m_currentNode = nullptr;
	m_nodeToDestroy = nullptr;
}

};