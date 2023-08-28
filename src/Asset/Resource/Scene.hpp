#pragma once 

#include <Aka/Scene/World.h>

#include "StaticMesh.hpp"
#include "Resource.hpp"

namespace app {

struct ArchiveScene;

using namespace aka;

struct Scene : Resource {
public:
	void create(const ArchiveScene& _archive);
	void destroy(const ArchiveScene& _archive);
private:
	// ECS hierarchy here...
	entt::registry registry;
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

struct TransformComponent {
	mat4f transform;
};

struct HierarchyComponent {
	Entity parent;
	mat4f inverseTransform;
};

// ------------------------------------
// World
// ------------------------------------
struct World {
	// Each scene has a single main camera
	
	std::vector<Scene> scenes;
	std::vector<System*> systems; // Run systems for whole world ? We might want to ignore some scene though...

	entt::dispatcher dispatcher;
};


};