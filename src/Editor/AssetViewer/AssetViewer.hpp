#pragma once

#include <Aka/Graphic/GraphicDevice.h>

#include <Aka/Resource/Resource/Resource.hpp>
#include <Aka/Resource/AssetLibrary.hpp>
#include <Aka/Core/Event.h>

namespace app {

class AssetViewerBase : aka::EventListener<aka::ResourceLoadedEvent>
{
public:
	AssetViewerBase() {}
	virtual ~AssetViewerBase() {}

	void create(aka::gfx::GraphicDevice* _device);
	void destroy(aka::gfx::GraphicDevice* _device);
	void update(aka::Time _deltaTime);
	void render(aka::gfx::GraphicDevice* _device, aka::gfx::Frame* _frame);
	void onReceive(const aka::ResourceLoadedEvent& event);
protected:
	virtual void onCreate(aka::gfx::GraphicDevice * _device) {}
	virtual void onDestroy(aka::gfx::GraphicDevice* _device) {}
	virtual void onUpdate(aka::Time deltaTime) {}
	virtual void onRender(aka::gfx::GraphicDevice* _device, aka::gfx::Frame* frame) {}
	virtual void onLoad() = 0;
public:
	virtual const char* getResourceName() const = 0;
	virtual aka::ResourceType getResourceType() const = 0;
	virtual aka::AssetID getAssetID() const = 0;
	virtual const aka::Resource* getResource() const = 0;
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
	AssetViewer(aka::ResourceType _type, aka::AssetID _resourceID, aka::ResourceHandle<T> _resource);
	virtual ~AssetViewer() {}

protected:
	const char* getResourceName() const override final;
	aka::ResourceType getResourceType() const override final;
	aka::AssetID getAssetID() const override final;
	const aka::Resource* getResource() const override final;
	bool isLoaded() const override final;
protected:
	void drawUI() override final;
	virtual void drawUIResource(const T& _resource) = 0;
protected:
	void onLoad() override final;
	virtual void onLoad(const T& res) {};
protected:
	aka::ResourceType m_type;
	aka::AssetID m_id;
	aka::ResourceHandle<T> m_resource;
};


template<typename T>
inline AssetViewer<T>::AssetViewer(aka::ResourceType _type, aka::AssetID _resourceID, aka::ResourceHandle<T> _resource) :
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
inline aka::ResourceType AssetViewer<T>::getResourceType() const
{
	return m_type;
}

template<typename T>
inline aka::AssetID AssetViewer<T>::getAssetID() const
{
	return m_id;
}

template<typename T>
inline const aka::Resource* AssetViewer<T>::getResource() const
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
	AKA_ASSERT(m_resource.getState() == aka::ResourceState::Loaded, "Resource not loaded when calling \"onLoad\"");
	onLoad(m_resource.get());
}

};