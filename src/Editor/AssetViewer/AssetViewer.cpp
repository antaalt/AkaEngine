#include "AssetViewer.hpp"

#include <Aka/Resource/Resource/Texture.hpp>
#include <Aka/Resource/Resource/StaticMesh.hpp>
#include <Aka/Layer/ImGuiLayer.h>

namespace app {

template class AssetViewer<aka::Texture>;
template class AssetViewer<aka::StaticMesh>;

void AssetViewerBase::drawUIMissingResource()
{
	ImGui::Text("Resource loading...");
}

void AssetViewerBase::create(aka::gfx::GraphicDevice* _device)
{
	onCreate(_device);
	if (isLoaded())
	{
		onLoad();
		m_loaded = true;
	}
	m_created = true;
}

void AssetViewerBase::destroy(aka::gfx::GraphicDevice* _device)
{
	if (m_created)
		onDestroy(_device);
}

void AssetViewerBase::update(aka::Time _deltaTime)
{
	if (m_created)
		onUpdate(_deltaTime);
}

void AssetViewerBase::render(aka::gfx::GraphicDevice* _device, aka::gfx::Frame* _frame)
{
	if (m_created)
		onRender(_device, _frame);
}

void AssetViewerBase::onReceive(const aka::ResourceLoadedEvent& event)
{
	if (event.resource == getAssetID())
	{
		onLoad();
		m_loaded = true;
	}
}

};