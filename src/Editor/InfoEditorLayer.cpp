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

void InfoEditorLayer::onLayerPreRender()
{
}

void InfoEditorLayer::onLayerRender(aka::gfx::GraphicDevice* _device, aka::gfx::FrameHandle frame)
{
	aka::Application* app = aka::Application::app();
	aka::gfx::GraphicDevice* graphic = app->graphic();

	if (ImGui::BeginMainMenuBar())
	{
		bool enableUI = false;
		if (ImGui::MenuItem(enableUI ? "Hide" : "Show"))
		{
		}
		ImGui::Separator();
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New scene", nullptr)) {

			}
			ImGui::Separator();
			if (ImGui::MenuItem("Open scene", nullptr)) {

			}
			if (ImGui::MenuItem("Save scene", nullptr)) {

			}
			ImGui::Separator();
			if (ImGui::MenuItem("Exit", nullptr)) {
				EventDispatcher<QuitEvent>::emit();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Undo", "Ctrl+Z")) {

			}
			if (ImGui::MenuItem("Redo", "Ctrl+Y")) {

			}
			ImGui::Separator();
			if (ImGui::MenuItem("Copy", "Ctrl+C")) {

			}
			if (ImGui::MenuItem("Paste", "Ctrl+V")) {

			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Display"))
		{
			static const char* s_name[EnumCount<EditorLayerType>()] = {
				"AssetViewer",
				"AssetBrowser",
				"SceneEditor",
			};
			for (EditorLayerType type : EnumRange<EditorLayerType>())
			{
				uint32_t id = EnumToIndex(type);
				if (m_editorLayer[id] == nullptr)
					continue;
				bool isViewerVisible = m_editorLayer[id]->isVisible();
				bool isEnabled = m_editorLayer[id]->isEnabled();
				if (ImGui::MenuItem(s_name[id], nullptr, &isViewerVisible, isEnabled))
				{
					m_editorLayer[id]->setVisible(isViewerVisible);
				}
			}
			ImGui::EndMenu();
		}
		ImGui::Separator();
		ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
		ImGui::Separator();
		ImGui::Text("%.3f ms", 1000.0f / ImGui::GetIO().Framerate);
		ImGui::Separator();
		ImGui::Text("%ux%u ", m_width, m_height);
		ImGui::Separator();
		if (ImGui::SmallButton("Capture"))
		{
			graphic->capture();
		}
		if (ImGui::SmallButton("Screenshot"))
		{
			void* data = nullptr;
			graphic->screenshot(data);
		}
		ImGui::EndMainMenuBar();
	}
}

void InfoEditorLayer::onLayerPostRender()
{
}

void InfoEditorLayer::onLayerResize(uint32_t width, uint32_t height)
{
	m_width = width;
	m_height = height;
}

void InfoEditorLayer::setEditorLayer(EditorLayerType type, EditorLayer* layer)
{
	m_editorLayer[EnumToIndex(type)] = layer;
}

};