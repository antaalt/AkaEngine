#include "EditorLayer.hpp"

#include <Aka/Layer/ImGuiLayer.h>

namespace app {

using namespace aka;


void EditorLayer::onLayerCreate(gfx::GraphicDevice* _device)
{
	onCreate(_device);
}

void EditorLayer::onLayerDestroy(gfx::GraphicDevice* _device)
{
	onDestroy(_device);
}

void EditorLayer::onLayerUpdate(Time deltaTime)
{
	onUpdate(deltaTime);
}

void EditorLayer::onLayerFixedUpdate(Time deltaTime)
{
	onFixedUpdate(deltaTime);
}

void EditorLayer::onLayerFrame()
{
	onFrame();
}

void EditorLayer::onLayerRender(aka::gfx::GraphicDevice* _device, gfx::Frame* frame)
{
	onRender(_device, frame);
	if (isVisible() && isEnabled())
	{
		bool open = true;
		if (ImGui::Begin(getName(), &open, ImGuiWindowFlags_MenuBar))
		{
			onDrawUI();
		}
		setVisible(open);
		ImGui::End();
	}
}

void EditorLayer::onLayerPresent()
{
	onPresent();
}

void EditorLayer::onLayerResize(uint32_t width, uint32_t height)
{
	onResize(width, height);
}

};