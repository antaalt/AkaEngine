#pragma once 

#include <type_traits>
#include <memory>
#include <atomic>

#include <Aka/Core/Container/String.h>
#include <Aka/Graphic/GraphicDevice.h>

namespace app {

class AssetLibrary;
struct Archive;

enum class ResourceID : uint64_t { Invalid = (uint64_t)-1 };
// TODO define DECL_STRICT_TYPE(ResourceID)

enum class ResourceType : uint32_t
{
	Unknown,

	Scene, // scene is not a component though... its a list of entity with component that we can drop or open ?
	StaticMesh,
	DynamicMesh,
	SpriteMesh, // 2D mesh, could be staticmesh2D instead ?
	FontMesh,
	AudioSource,
	AmbientAudioSource,

	First = Scene,
	Last = AmbientAudioSource,
};

// This could be Component directly for ECS.
// Might cause issue with component which are not directly droppable
class Resource
{
public:
	Resource(ResourceType _type) : Resource(_type, ResourceID::Invalid, "") {}
	Resource(ResourceType _type, ResourceID id, const aka::String& _name) : m_type(_type), m_id(id), m_name(_name) {}

	const aka::String& getName() const { return m_name; }
	ResourceID getID() const { return m_id; }
	ResourceType getType() const { return m_type; }

	virtual void create(AssetLibrary* _library, aka::gfx::GraphicDevice* _device, const Archive& _archive) = 0;
	virtual void save(AssetLibrary* _library, aka::gfx::GraphicDevice* _device, Archive& _archive) = 0;
	virtual void destroy(AssetLibrary* _library, aka::gfx::GraphicDevice* _device) = 0;

	// We need to be able to clone a resource ?
	//virtual Resource* clone() = 0;
private:
	ResourceType m_type;
	ResourceID m_id;
	aka::String m_name;
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

	size_t getCount() const { return m_resource.use_count(); }

	const T& get() const { return m_resource->resource; }
	T& get() { return m_resource->resource; }

	static ResourceHandle<T> invalid() { return ResourceHandle<T>(); }

private:
	std::shared_ptr<ResourceHandleInner> m_resource;
};

}