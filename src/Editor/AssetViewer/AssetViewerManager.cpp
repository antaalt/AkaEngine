#include "AssetViewerManager.hpp"

#include <Aka/Core/Application.h>
#include <Aka/Layer/ImGuiLayer.h>

namespace app {
AssetViewerManager::~AssetViewerManager()
{
	for (AssetViewerBase*& viewer : m_assetViewers)
	{
		delete viewer;
	}
	AKA_ASSERT(m_assetViewersToCreate.size() == 0, "");
	AKA_ASSERT(m_assetViewersToDestroy.size() == 0, "");
}
void AssetViewerManager::onCreate(aka::gfx::GraphicDevice* _device)
{
	for (AssetViewerBase*& viewer : m_assetViewers)
	{
		viewer->create(_device);
	}
}

void AssetViewerManager::onDestroy(aka::gfx::GraphicDevice* _device)
{
	for (AssetViewerBase*& viewer : m_assetViewers)
	{
		viewer->destroy(_device);
	}
}

void AssetViewerManager::onUpdate(aka::Time deltaTime)
{
	for (AssetViewerBase*& viewer : m_assetViewers)
	{
		viewer->update(deltaTime);
	}
}

void AssetViewerManager::onRender(aka::gfx::GraphicDevice* _device, aka::gfx::Frame* frame)
{
	// Destroy viewer that where removed recently.
	for (AssetViewerBase*& viewer : m_assetViewersToDestroy)
	{
		viewer->destroy(_device);
		delete viewer;
	}
	m_assetViewersToDestroy.clear();
	// Create viewer that where added recently.
	for (AssetViewerBase*& viewer : m_assetViewersToCreate)
	{
		viewer->create(_device);
	}
	m_assetViewers.append(m_assetViewersToCreate.begin(), m_assetViewersToCreate.end());
	m_assetViewersToCreate.clear();
	for (AssetViewerBase*& viewer : m_assetViewers)
	{
		viewer->render(_device, frame);
	}
}

void AssetViewerManager::render(aka::gfx::Frame* frame)
{
	aka::Application* app = aka::Application::app();
	bool opened = true;
	if (ImGui::Begin("AssetViewer", &opened, ImGuiWindowFlags_MenuBar))
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				// TODO check if dirty
				if (ImGui::MenuItem("Save"))
				{
					AKA_NOT_IMPLEMENTED;
				}
				if (ImGui::MenuItem("Reload"))
				{
					AKA_NOT_IMPLEMENTED;
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
		if (ImGui::BeginTabBar("AssetViewer"))
		{
			if (ImGui::TabItemButton("?", ImGuiTabItemFlags_Leading | ImGuiTabItemFlags_NoTooltip))
				ImGui::OpenPopup("MyHelpMenu");

			if (ImGui::BeginPopup("MyHelpMenu"))
			{
				ImGui::Selectable("Hello!");
				ImGui::EndPopup();
			}

			if (ImGui::TabItemButton("+", ImGuiTabItemFlags_Trailing | ImGuiTabItemFlags_NoTooltip))
			{
				// Add new tab
			}

			for (size_t i = 0; i < m_assetViewers.size(); i++)
			{
				AssetViewerBase* viewer = m_assetViewers[i];

				bool open = true;
				ResourceType type = viewer->getResourceType();
				// TODO use custom font 
				const char* typeLogo[EnumCount<ResourceType>()] = {
					"🖼️",//Scene
					"🌴",//StaticMesh
					"M",//Material
					"🧱",//Texture
					"🎧",//AudioSource
				};
				String tabName = String::format("[%s] %s", typeLogo, viewer->getResourceName());
				if (ImGui::BeginTabItem(tabName.cstr(), &open, ImGuiTabItemFlags_None))
				{
					
					viewer->drawUI();
					/*ImGui::Separator();
					if (ImGui::TreeNode("Asset"))
					{
						m_library-> m_resource.get()->getID()
						ImGui::Text("Path : %s", m_resource.path.cstr());
						ImGui::TreePop();
					}*/
					ImGui::EndTabItem();
				}
				if (!open) // User pressed close.
				{
					m_assetViewers.remove(m_assetViewers.begin() + i);
					m_assetViewersToDestroy.append(viewer);
					i--;
				}
			}
			ImGui::EndTabBar();
		}
	}
	ImGui::End();
}

template<> 
void AssetViewerManager::open(ResourceID _resourceID, ResourceHandle<Scene>& _resourceHandle)
{
	for (AssetViewerBase*& viewer : m_assetViewers)
	{
		if (_resourceID == viewer->getResourceID())
		{
			// Resource already opened.
			return;
		}
	}
	m_assetViewersToCreate.append(new SceneViewer(_resourceID, _resourceHandle));
}

template<>
void AssetViewerManager::open(ResourceID _resourceID, ResourceHandle<StaticMesh>& _resourceHandle)
{
	for (AssetViewerBase*& viewer : m_assetViewers)
	{
		if (_resourceID == viewer->getResourceID())
		{
			// Resource already opened.
			return;
		}
	}
	m_assetViewersToCreate.append(new StaticMeshViewer(_resourceID, _resourceHandle));
}

template<>
void AssetViewerManager::open(ResourceID _resourceID, ResourceHandle<Texture>& _resourceHandle)
{
	for (AssetViewerBase*& viewer : m_assetViewers)
	{
		if (_resourceID == viewer->getResourceID())
		{
			// Resource already opened.
			return;
		}
	}
	m_assetViewersToCreate.append(new TextureViewer(_resourceID, _resourceHandle));
}

};