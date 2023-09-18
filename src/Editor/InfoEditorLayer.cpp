#include "InfoEditorLayer.hpp"

#include <imgui.h>
#include <imguizmo.h>

namespace app {

using namespace aka;

InfoEditorLayer::InfoEditorLayer()
{
}

void InfoEditorLayer::onLayerCreate(gfx::GraphicDevice* _device)
{
	_device->getBackbufferSize(m_width, m_height);
}

void InfoEditorLayer::onLayerDestroy(gfx::GraphicDevice* _device)
{
}

void InfoEditorLayer::onLayerFrame()
{
}

void InfoEditorLayer::onLayerRender(aka::gfx::Frame* frame)
{
	aka::Application* app = aka::Application::app();
	aka::gfx::GraphicDevice* graphic = app->graphic();
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;
	ImGui::SetNextWindowPos(ImVec2((float)(m_width - 5), 25.f), ImGuiCond_Always, ImVec2(1.f, 0.f));
	if (ImGui::Begin("Info", nullptr, flags))
	{
		ImGuiIO& io = ImGui::GetIO();
		ImGui::Text("Resolution : %ux%u", m_width, m_height);
		ImGui::Text("%.3f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
		ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
		ImGui::Separator();
		const char* apiName[] = {
			"Vulkan",
			"DirectX12"
		};
		ImGui::Text("Api : %s", apiName[EnumToIndex(graphic->api())]);
	}
	ImGui::End();
}

void InfoEditorLayer::onLayerPresent()
{
}

void InfoEditorLayer::onLayerResize(uint32_t width, uint32_t height)
{
	m_width = width;
	m_height = height;
}

};