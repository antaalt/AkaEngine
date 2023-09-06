#pragma once 

#include <Aka/Scene/World.h>

#include "StaticMesh.hpp"
#include "Resource.hpp"

namespace app {

struct ArchiveScene;
class AssetLibrary;

using namespace aka;

struct Scene : Resource {
public:
	// Should be load / save instead and take library + id instead of archive directly for cache.
	void create(AssetLibrary* library, const ArchiveScene& _archive);
	void destroy(AssetLibrary* library);
//private:
	// ECS hierarchy here...
	//entt::registry registry;
	World world;
};

struct Entity {
	entt::entity entity;
};

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