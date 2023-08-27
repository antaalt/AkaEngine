#pragma once 

#include <Aka/Scene/World.h>

#include "StaticMesh.hpp"
#include "Resource.hpp"
#include "../Archive/Archive.hpp"

namespace app {

using namespace aka;

struct ArchiveScene : Archive {
	std::vector<ArchiveStaticMesh> meshes;

	ArchiveLoadResult load(const ArchivePath& path) override;
	ArchiveSaveResult save(const ArchivePath& path) override;
};

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
// Zorld
// ------------------------------------
struct World {
	// Each scene has a single main camera
	
	std::vector<Scene> scenes;
	std::vector<System*> systems; // Run systems for whole world ? We might want to ignore some scene though...

	entt::dispatcher dispatcher;
};


};