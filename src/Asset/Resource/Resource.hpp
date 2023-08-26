#pragma once 

#include <type_traits>
#include <memory>


namespace app {

enum class ResourceID {};

// A resource is something we can drop in engine, aka a component. we plug it gameplay component, and we have an actor
enum class ResourceType {
	Scene, // scene is not a component though... its a list of entity with component that we can drop or open ?
	StaticMesh,
	DynamicMesh,
	SpriteMesh, // 2D mesh, could be staticmesh2D instead ?
	FontMesh,
	AudioSource,
	AmbientAudioSource,
};

// This could be Component directly for ECS.
// Might cause issue with component which are not directly droppable
struct Resource {
	ResourceID id;

	// We need to be able to clone a resource
	//virtual Resource* clone() = 0;
};


template <typename T>
struct ResourceHandle {
	static_assert(std::is_base_of<T, Resource>::value, "Type should inherit Resource");

	std::shared_ptr<T> resource;

	bool isLoaded() const;

	const T& get() const;
	T& get();
};

}