#include "SceneEditorLayer.hpp"

#include <Aka/Layer/ImGuiLayer.h>
#include <Aka/Scene/Component/CameraComponent.hpp>
#include <Aka/Scene/Component/StaticMeshComponent.hpp>
#include <Aka/Resource/AssetLibrary.hpp>

namespace app {

using namespace aka;

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
	static bool draw(T& component) { Logger::error("Trying to draw an undefined component"); return false; }
};

template <> const char* ComponentNode<StaticMeshComponent>::name() { return "Mesh"; }
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

template <typename T>
void component(Node3D* current)
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
				current->setDirty(ComponentTrait<T>::type);
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
		m_nodeToDestroy->destroy(_device);
		m_nodeToDestroy = nullptr;
	}
}

void SceneEditorLayer::onDrawUI()
{
	std::function<void(Node3D* parent, Node3D*& current)> recurse;
	recurse = [&recurse, this](Node3D* parent, Node3D*& current)
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
	const bool isLoaded = m_scene.isLoaded();
	{
		// --- Menu
		Node3D* rootNode = isLoaded ? &m_scene.get().getRoot() : nullptr;
		const bool isValid = m_currentNode != nullptr;
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("World"))
			{
				if (ImGui::MenuItem("Save"))
				{
					//m_library->save();
					//Scene::save("library/scene.json", world);
				}
				if (ImGui::MenuItem("Load"))
				{
					//Scene::load(world, "library/scene.json");
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
							//m_currentNode = Scene::createCubeNode(world).handle();
						}
						if (ImGui::MenuItem("UV Sphere", nullptr, nullptr, isLoaded))
						{
							//m_currentNode = Scene::createSphereNode(world, 32, 16).handle();
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
						//m_currentNode = Scene::createArcballCameraNode(world).handle();
					}
					if (ImGui::MenuItem("Empty", nullptr, nullptr, isLoaded))
					{
						//m_currentNode = world.createNode("New empty").handle();
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
					if (ImGui::MenuItem("Camera", nullptr, nullptr, isLoaded && isValid && !m_currentNode->has<CameraComponent>()))
						m_currentNode->attach<CameraComponent>();
					if (ImGui::MenuItem("Mesh", nullptr, nullptr, isLoaded && isValid && !m_currentNode->has<StaticMeshComponent>()))
						m_currentNode->attach<StaticMeshComponent>();
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Remove", isLoaded && isValid))
				{
					if (ImGui::MenuItem("Mesh", nullptr, nullptr, isLoaded && m_currentNode->has<StaticMeshComponent>()))
						m_currentNode->detach<StaticMeshComponent>();
					if (ImGui::MenuItem("Camera", nullptr, nullptr, isLoaded && m_currentNode->has<CameraComponent>()))
						m_currentNode->detach<CameraComponent>();
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
				recurse(rootNode, m_currentNode);
			}
		}
		ImGui::EndChild();

		// --- Add entity
		static char m_newEntityName[256];
		ImGui::InputTextWithHint("##entityName", "Entity name", m_newEntityName, 256);
		ImGui::SameLine();
		if (ImGui::Button("Create entity") && isLoaded)
		{
			Node3D* parentNode = m_currentNode ? m_currentNode : rootNode;
			m_currentNode = parentNode->addChild(m_newEntityName);
		}
		ImGui::Separator();

		// --- Entity info
		if (isLoaded)
		{
			Scene& scene = m_scene.get();
			ImGui::TextColored(ImGuiLayer::Color::red, "Entity");
			if (m_currentNode)
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
				updatedTransform |= ImGuizmo::Manipulate(view[0].data, projection[0].data, (ImGuizmo::OPERATION)m_gizmoOperation, ImGuizmo::MODE::WORLD, transform[0].data);
				

				if (m_currentNode->isOrphan())
				{
					ImGui::Text("Add a component to the entity.");
				}
				else
				{
					// Draw every component.
					component<StaticMeshComponent>(m_currentNode);
					component<CameraComponent>(m_currentNode);
				}
			}
		}
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

void SceneEditorLayer::setCurrentScene(ResourceHandle<Scene> _scene)
{
	// TODO handle multiple scene with tabs ?
	setVisible(_scene.isValid());
	setEnabled(_scene.isValid());
	m_scene = _scene;
}

};