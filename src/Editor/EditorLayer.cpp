#include "EditorLayer.hpp"

#include <Aka/Layer/ImGuiLayer.h>

namespace app {

using namespace aka;


void EditorLayer::onLayerCreate(Renderer* _renderer)
{
	onCreate(_renderer);
}

void EditorLayer::onLayerDestroy(Renderer* _renderer)
{
	onDestroy(_renderer);
}

void EditorLayer::onLayerUpdate(Time deltaTime)
{
	onUpdate(deltaTime);
}

void EditorLayer::onLayerFixedUpdate(Time deltaTime)
{
	onFixedUpdate(deltaTime);
}

void EditorLayer::onLayerPreRender()
{
	onPreRender();
}

void EditorLayer::onLayerRender(aka::Renderer* _renderer, gfx::FrameHandle frame)
{
	onRender(_renderer, frame);
	if (isVisible() && isEnabled())
	{
		bool open = true;
		if (ImGui::Begin(getName(), &open, ImGuiWindowFlags_MenuBar))
		{
			onDrawUI(_renderer->getDebugDrawList());
		}
		setVisible(open);
		ImGui::End();
	}
}

void EditorLayer::onLayerPostRender()
{
	onPostRender();
}

void EditorLayer::onLayerResize(uint32_t width, uint32_t height)
{
	onResize(width, height);
}

};