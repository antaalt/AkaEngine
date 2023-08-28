#include "Scene.hpp"

#include "../Archive/ArchiveScene.hpp"

namespace app {

uint32_t firstbithigh(uint32_t mask)
{

}
uint32_t firstbitlow(uint32_t mask)
{

}

void Scene::create(const ArchiveScene& _archive)
{
	entt::entity e = registry.create();
	TransformComponent& t = registry.emplace<TransformComponent>(e);

	for (const ArchiveSceneEntity& e : _archive.entities)
	{
		if (asBool(SceneComponent::Hierarchy & e.components))
		{
			e.id;
		}
		if (asBool(SceneComponent::Transform & e.components))
		{
			const ArchiveSceneTransform& mesh = _archive.transforms[uint32_t(e.id[firstbit])];
			mesh.getPath(); // TODO read cache with this somehow.
			e.id;
		}
		if (asBool(SceneComponent::StaticMesh & e.components))
		{
			const ArchiveStaticMesh& mesh = _archive.meshes[uint32_t(e.id)];
			mesh.getPath(); // TODO read cache with this somehow.
		}
	}
}

void Scene::destroy(const ArchiveScene& _archive)
{
}

}