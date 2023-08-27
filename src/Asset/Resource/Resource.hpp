#pragma once 

#include <type_traits>
#include <memory>
#include <atomic>


namespace app {

enum class ResourceID : uint32_t {};

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
private:
	struct ResourceHandleInner {
		T resource;
		std::atomic<ResourceState> state; // Tracking
	};
public:
	ResourceHandle() : m_resource(nullptr) {

	}
	ResourceHandle(ResourceState _state) : m_resource(std::make_shared<ResourceHandleInner>()) 
	{
		m_resource->state = _state;
	}
	bool isValid() const { return m_resource != nullptr; }
	bool isLoaded() const { return isValid() && m_resource->state == ResourceState::Loaded; }
	ResourceState getState() const { return !isValid() ? ResourceState::Unknown : m_resource->state.load(); }

	const T& get() const { return m_resource->resource; }
	T& get() { return m_resource->resource; }

	static ResourceHandle<T> invalid() { return ResourceHandle<T>(); }

private:
	std::shared_ptr<ResourceHandleInner> m_resource;
};

}