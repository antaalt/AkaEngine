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

enum class ResourceState {
	Unknown,
	Disk,
	Pending,
	Loaded,
};

template <typename T>
struct ResourceHandle {
	static_assert(std::is_base_of<Resource, T>::value, "Type should inherit Resource");

	bool isLoaded() const { return m_state == ResourceState::Loaded; }
	ResourceState getState() const { return m_state; }

	const T& get() const { return *m_resource.get(); }
	T& get() { return *m_resource.get(); }

private:
	std::shared_ptr<T> m_resource;
	ResourceState m_state; // Should be a pointer shared between instance as well as ref count.
};

}