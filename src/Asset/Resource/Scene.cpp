#include "Scene.hpp"

#include "../Archive/ArchiveScene.hpp"
#include "../AssetLibrary.hpp"

namespace app {

struct InstanceUniformBuffer
{
	mat4f model;
	mat4f normal;
};

mat4f getParentTransform(Entity e)
{
	if (e == Entity::null())
	{
		return mat4f::identity();
	}
	else
	{
		return getParentTransform(e.get<Hierarchy3DComponent>().parent) * e.get<Transform3DComponent>().transform;
	}
}

void Scene::create_internal(AssetLibrary* _library, gfx::GraphicDevice* _device, const Archive& _archive)
{
	AKA_ASSERT(_archive.type() == AssetType::Scene, "Invalid archive");
	const ArchiveScene& scene = reinterpret_cast<const ArchiveScene&>(_archive);

	m_bounds = scene.bounds;
	Vector<Entity> entities;
	for (const ArchiveSceneEntity& entity : scene.entities)
	{
		aka::Entity e = world.createEntity(entity.name);
		if (asBool(SceneComponentMask::Hierarchy & entity.components))
		{
			ArchiveSceneID id = entity.id[EnumToIndex(SceneComponent::Hierarchy)];
			Hierarchy3DComponent& t = e.add<Hierarchy3DComponent>();
			if (id == InvalidArchiveSceneID)
				t.parent = Entity::null();
			else
				t.parent = entities[toIntegral(id)];
			t.inverseTransform = mat4f::identity();
		}
		if (asBool(SceneComponentMask::Transform & entity.components))
		{
			ArchiveSceneID id = entity.id[EnumToIndex(SceneComponent::Transform)];
			Transform3DComponent& t = e.add<Transform3DComponent>();
			t.transform = scene.transforms[toIntegral(id)].matrix;
		}
		if (asBool(SceneComponentMask::StaticMesh & entity.components))
		{
			ArchiveSceneID id = entity.id[EnumToIndex(SceneComponent::StaticMesh)];
			StaticMeshComponent& s = e.add<StaticMeshComponent>();
			const ArchiveStaticMesh& mesh = scene.meshes[toIntegral(id)];
			s.mesh = _library->load<app::StaticMesh>(_library->getResourceID(mesh.id()), mesh, _device);
		}
		/*if (asBool(SceneComponentMask::PointLight & e.components))
		{
			const ArchiveStaticMesh& mesh = scene.meshes[uint32_t(e.id)];
			mesh.getPath(); // TODO read cache with this somehow.
		}*/
		entities.append(e);
	}
	// Compute hierarchy after all entities are loaded
	auto transformView = world.registry().view<Hierarchy3DComponent, Transform3DComponent, StaticMeshComponent>();
	for (entt::entity entity : transformView)
	{
		Hierarchy3DComponent& h = world.registry().get<Hierarchy3DComponent>(entity);
		Transform3DComponent& t = world.registry().get<Transform3DComponent>(entity);
		StaticMeshComponent&  s = world.registry().get<StaticMeshComponent>(entity);
		mat4f parentTransform = getParentTransform(Entity(entity, &world));
		t.transform = parentTransform * t.transform;
		h.inverseTransform = mat4f::inverse(parentTransform);

		// TODO retrieve from elsewhere
		gfx::ShaderBindingState bindings{};
		bindings.add(gfx::ShaderBindingType::UniformBuffer, gfx::ShaderMask::Vertex, 1);
		InstanceUniformBuffer ubo;
		ubo.model = world.registry().get<Transform3DComponent>(entity).transform;
		ubo.normal = mat4f::transpose(mat4f::inverse(ubo.model));
		s.instanceBuffer = _device->createBuffer("InstanceBuffer", gfx::BufferType::Uniform, sizeof(InstanceUniformBuffer), gfx::BufferUsage::Default, gfx::BufferCPUAccess::None, &ubo);;
		s.descriptorSet = _device->createDescriptorSet("InstanceDescriptorSet", bindings);

		gfx::DescriptorSetData data{};
		data.addUniformBuffer(s.instanceBuffer);
		_device->update(s.descriptorSet, data);
	}
}

void Scene::save_internal(AssetLibrary* library, gfx::GraphicDevice* _device, Archive& _archive)
{
	AKA_NOT_IMPLEMENTED;
}

void Scene::destroy_internal(AssetLibrary* _library, gfx::GraphicDevice* _device)
{
	world.registry().view<StaticMeshComponent>().each([_device](entt::entity entity, StaticMeshComponent& meshComp) {
		_device->destroy(meshComp.descriptorSet);
		_device->destroy(meshComp.instanceBuffer);
		});
	world.clear();
}

void Scene::update(aka::Time time, gfx::GraphicDevice* _device)
{
	// --- Update hierarchy transfom.
	// TODO only update if a hierarchy node or transform node has been updated. use dirtyTransform ?
	entt::registry& r = world.registry();
	// Sort hierarchy to ensure correct order.
	// https://skypjack.github.io/2019-08-20-ecs-baf-part-4-insights/
	// https://wickedengine.net/2019/09/29/entity-component-system/
	r.sort<Hierarchy3DComponent>([&r](const entt::entity lhs, entt::entity rhs) {
		const Hierarchy3DComponent& clhs = r.get<Hierarchy3DComponent>(lhs);
		const Hierarchy3DComponent& crhs = r.get<Hierarchy3DComponent>(rhs);
		return lhs < rhs&& clhs.parent.handle() != rhs;
	});
	// Compute transforms
	auto transformView = world.registry().view<Hierarchy3DComponent, Transform3DComponent>();
	for (entt::entity entity : transformView)
	{
		Transform3DComponent& t = r.get<Transform3DComponent>(entity);
		Hierarchy3DComponent& h = r.get<Hierarchy3DComponent>(entity);
		if (h.parent.valid())
		{
			mat4f localTransform = h.inverseTransform * t.transform;
			t.transform = h.parent.get<Transform3DComponent>().transform * localTransform;
			h.inverseTransform = mat4f::inverse(h.parent.get<Transform3DComponent>().transform);
		}
		else
		{
			h.inverseTransform = mat4f::identity();
		}
	}
	// TODO update matrix if change detected.

}

void Scene::render(gfx::GraphicDevice* _device, gfx::Frame* _frame)
{
}

}