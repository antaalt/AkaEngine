#include "SceneEditorLayer.hpp"

#include <Aka/Layer/ImGuiLayer.h>

#include <Aka/Scene/World.h>

#include "../Asset/AssetLibrary.hpp"

namespace app {

using namespace aka;

SceneEditorLayer::SceneEditorLayer()
{
}

void SceneEditorLayer::onLayerCreate(aka::gfx::GraphicDevice* _device)
{
}

void SceneEditorLayer::onLayerDestroy(aka::gfx::GraphicDevice* _device)
{
}

void SceneEditorLayer::onLayerFrame()
{
}

template <typename T>
struct ComponentNode {
	static const char* name() { return "Unknown"; }
	//static const char* icon() { return ""; }
	static bool draw(T& component) { Logger::error("Trying to draw an undefined component"); return false; }
};

template <> const char* ComponentNode<aka::TagComponent>::name() { return "Tag"; }
template <> bool ComponentNode<aka::TagComponent>::draw(aka::TagComponent& tag)
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
		if (hierarchy.parent.has<aka::TagComponent>())
			ImGui::Text("Parent : %s", hierarchy.parent.get<aka::TagComponent>().name.cstr());
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
	ImGuizmo::DecomposeMatrixToComponents(transform.transform.cols[0].data, translation, rotation, scale);
	updated |= ImGui::InputFloat3("Translation", translation);
	updated |= ImGui::InputFloat3("Rotation", rotation);
	updated |= ImGui::InputFloat3("Scale", scale);
	if (updated)
		ImGuizmo::RecomposeMatrixFromComponents(translation, rotation, scale, transform.transform.cols[0].data);
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

void SceneEditorLayer::onLayerRender(aka::gfx::Frame* frame)
{
	if (m_scene->isLoaded())
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

		app::Scene& scene = m_scene->get();
		static entt::entity m_currentEntity;
		static char m_newEntityName[256];
		if (ImGui::Begin("Scene", nullptr, ImGuiWindowFlags_MenuBar))
		{
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
						/*if (ImGui::MenuItem("Camera", nullptr, nullptr, !e.has<app:: >()))
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
			ImGui::TextColored(ImGuiLayer::Color::red, "Graph");
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
			ImGui::TextColored(ImGuiLayer::Color::red, "Entity");
			if (m_currentEntity != entt::null && scene.world.registry().valid(m_currentEntity))
			{
				if (scene.world.registry().orphan(m_currentEntity))
				{
					ImGui::Text("Add a component to the entity.");
				}
				else
				{
					// Draw every component.
					component<aka::TagComponent>(scene.world, m_currentEntity);
					component<app::Transform3DComponent>(scene.world, m_currentEntity);
					component<app::Hierarchy3DComponent>(scene.world, m_currentEntity);
					component<app::StaticMeshComponent>(scene.world, m_currentEntity);
				}
			}
		}
		ImGui::End();
	}
}

void SceneEditorLayer::onLayerPresent()
{
}

void SceneEditorLayer::onLayerResize(uint32_t width, uint32_t height)
{
}

void SceneEditorLayer::setCurrentScene(ResourceHandle<Scene>* _scene)
{
	m_scene = _scene;
}

};