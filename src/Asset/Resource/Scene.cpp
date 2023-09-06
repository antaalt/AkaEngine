#include "Scene.hpp"

#include "../Archive/ArchiveScene.hpp"
#include "../AssetLibrary.hpp"

namespace app {

// http://graphics.stanford.edu/%7Eseander/bithacks.html
uint32_t countbits32(uint32_t x)
{
#if defined(AKA_PLATFORM_WINDOWS)
	return ::__popcnt(x);
#else
	uint32_t s = 0;
	for (uint32_t i = 0; i < 32; i++)
		if (x & (1U << i))
			s++;
	return s;
#endif
}

uint32_t firstbithigh(uint32_t x)
{
	if (x < 0)
		x &= 0x7fffffff;
	return ::__lzcnt(x) + 1;
}
uint32_t firstbitlow(uint32_t x)
{
	return countbits32((~x) & (x - 1));
}

void Scene::create(AssetLibrary* _library, const ArchiveScene& _archive)
{
	//Vector<aka::Entity> entities;
	for (const ArchiveSceneEntity& entity : _archive.entities)
	{
		aka::Entity e = world.createEntity("UnNamed");
		if (asBool(SceneComponentMask::Hierarchy & entity.components))
		{
			ArchiveSceneID id = entity.id[EnumToIndex(SceneComponent::Hierarchy)];
			Hierarchy3DComponent& t = e.add<Hierarchy3DComponent>();
			t.parent = aka::Entity::null();// entities[toIntegral(id)];
			t.inverseTransform = mat4f::identity(); // computed at runtime
		}
		if (asBool(SceneComponentMask::Transform & entity.components))
		{
			ArchiveSceneID id = entity.id[EnumToIndex(SceneComponent::Transform)];
			Transform3DComponent& t = e.add<Transform3DComponent>();
			t.transform = _archive.transforms[toIntegral(id)].matrix;
		}
		if (asBool(SceneComponentMask::StaticMesh & entity.components))
		{
			ArchiveSceneID id = entity.id[EnumToIndex(SceneComponent::StaticMesh)];
			StaticMeshComponent& s = e.add<StaticMeshComponent>();
			const ArchiveStaticMesh& mesh = _archive.meshes[toIntegral(id)];
			s.mesh = _library->getStaticMesh(_library->getResourceID(mesh.id()));
		}
		/*if (asBool(SceneComponentMask::PointLight & e.components))
		{
			const ArchiveStaticMesh& mesh = _archive.meshes[uint32_t(e.id)];
			mesh.getPath(); // TODO read cache with this somehow.
		}*/
		//entities.append(e);
	}
}

void Scene::destroy(AssetLibrary* _library)
{
	world.clear();
}

}