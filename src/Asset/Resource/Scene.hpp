#pragma once 

#include <Aka/Scene/World.h>

#include "StaticMesh.hpp"
#include "Resource.hpp"

namespace app {

struct ArchiveScene;
class AssetLibrary;

using namespace aka;

class Scene : public Resource {
public:
	Scene() : Resource(ResourceType::Scene) {}
	Scene(ResourceID _id, const String& _name) : Resource(ResourceType::Scene, _id, _name) {}

	// Should be load / save instead and take library + id instead of archive directly for cache.
	void create(AssetLibrary* library, gfx::GraphicDevice* _device, const Archive& _archive) override;
	void save(AssetLibrary* library, gfx::GraphicDevice* _device, Archive& _archive) override;
	void destroy(AssetLibrary* library, gfx::GraphicDevice* _device) override;

	void update(aka::Time time, gfx::GraphicDevice* _device);
	void render(gfx::GraphicDevice* _device, gfx::Frame* _frame);

	aabbox<> getBounds() const { return m_bounds; }
//private:
	// ECS hierarchy here...
	//entt::registry registry;
	// TODO scene is now world.
	aabbox<> m_bounds;
	World world;
};

/*struct Entity {
	entt::entity entity;
};*/

// ------------------------------------
// Systems
// ------------------------------------
struct System {

};

// ------------------------------------
// Components
// ------------------------------------
struct TagComponent
{
	String name;
};

struct Transform3DComponent {
	mat4f transform;
};

struct Hierarchy3DComponent {
	aka::Entity parent;
	mat4f inverseTransform;
};

// ------------------------------------
// World
// ------------------------------------
/*struct World {
	// Each scene has a single main camera
	
	std::vector<Scene> scenes;
	std::vector<System*> systems; // Run systems for whole world ? We might want to ignore some scene though...

	entt::dispatcher dispatcher;
};*/


};