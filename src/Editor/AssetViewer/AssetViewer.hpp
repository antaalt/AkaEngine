#pragma once

#include <Aka/Graphic/GraphicDevice.h>

#include "../../Asset/Resource/Resource.hpp"
#include "../../Asset/AssetLibrary.hpp"

namespace app {

class AssetViewerBase : EventListener<ResourceLoadedEvent>
{
public:
	AssetViewerBase() {}
	virtual ~AssetViewerBase() {}

	void create(aka::gfx::GraphicDevice* _device);
	void destroy(aka::gfx::GraphicDevice* _device);
	void update(aka::Time _deltaTime);
	void render(aka::gfx::GraphicDevice* _device, aka::gfx::Frame* _frame);
	void onReceive(const ResourceLoadedEvent& event);
protected:
	virtual void onCreate(aka::gfx::GraphicDevice * _device) {}
	virtual void onDestroy(aka::gfx::GraphicDevice* _device) {}
	virtual void onUpdate(aka::Time deltaTime) {}
	virtual void onRender(aka::gfx::GraphicDevice* _device, aka::gfx::Frame* frame) {}
	virtual void onLoad() = 0;
public:
	virtual const char* getResourceName() const = 0;
	virtual ResourceType getResourceType() const = 0;
	virtual ResourceID getResourceID() const = 0;
	virtual const Resource* getResource() const = 0;
	virtual bool isLoaded() const = 0;
	virtual void drawUI() = 0;
	void drawUIMissingResource();
protected:
	bool m_loaded = false;
	bool m_created = false;
};

template <typename T>
class AssetViewer : public AssetViewerBase
{
public:
	AssetViewer(ResourceType _type, ResourceID _resourceID, ResourceHandle<T> _resource);
	virtual ~AssetViewer() {}

protected:
	const char* getResourceName() const override final;
	ResourceType getResourceType() const override final;
	ResourceID getResourceID() const override final;
	const Resource* getResource() const override final;
	bool isLoaded() const override final;
protected:
	void drawUI() override final;
	virtual void drawUIResource(const T& _resource) = 0;
protected:
	void onLoad() override final;
	virtual void onLoad(const T& res) {};
protected:
	ResourceType m_type;
	ResourceID m_id;
	ResourceHandle<T> m_resource;
};


template<typename T>
inline AssetViewer<T>::AssetViewer(ResourceType _type, ResourceID _resourceID, ResourceHandle<T> _resource) :
	m_type(_type),
	m_id(_resourceID),
	m_resource(_resource)
{
}

template<typename T>
inline const char* AssetViewer<T>::getResourceName() const
{
	if (m_resource.isLoaded())
		return m_resource.get().getName().cstr();
	return "Unknown";
}

template<typename T>
inline ResourceType AssetViewer<T>::getResourceType() const
{
	return m_type;
}

template<typename T>
inline ResourceID AssetViewer<T>::getResourceID() const
{
	return m_id;
}

template<typename T>
inline const Resource* AssetViewer<T>::getResource() const
{
	if (m_resource.isLoaded())
		return &m_resource.get();
	return nullptr;
}

template<typename T>
inline bool AssetViewer<T>::isLoaded() const
{
	return m_resource.isLoaded();
}

template<typename T>
inline void AssetViewer<T>::drawUI()
{
	if (m_resource.isLoaded())
	{
		drawUIResource(m_resource.get());
	}
	else
	{
		drawUIMissingResource();
	}
}

template<typename T>
inline void AssetViewer<T>::onLoad()
{
	AKA_ASSERT(m_loaded == false, "Already called \"onLoad\"");
	AKA_ASSERT(m_resource.getState() == ResourceState::Loaded, "Resource not loaded when calling \"onLoad\"");
	onLoad(m_resource.get());
}

};