#include "AssetViewerEditorLayer.hpp"

#include <Aka/Core/Application.h>
#include <Aka/Layer/ImGuiLayer.h>
#include <IconsFontAwesome6.h>

namespace app {

using namespace aka;

AssetViewerEditorLayer::AssetViewerEditorLayer() :
	EditorLayer("Asset viewer")
{
}

AssetViewerEditorLayer::~AssetViewerEditorLayer()
{
	for (AssetViewerBase*& viewer : m_assetViewers)
	{
		delete viewer;
	}
	AKA_ASSERT(m_assetViewersToCreate.size() == 0, "");
	AKA_ASSERT(m_assetViewersToDestroy.size() == 0, "");
}
void AssetViewerEditorLayer::onCreate(aka::Renderer* _renderer)
{
	for (AssetViewerBase*& viewer : m_assetViewers)
	{
		viewer->create(_renderer->getDevice());
	}
}

void AssetViewerEditorLayer::onDestroy(aka::Renderer* _renderer)
{
	for (AssetViewerBase*& viewer : m_assetViewers)
	{
		viewer->destroy(_renderer->getDevice());
	}
}

void AssetViewerEditorLayer::onUpdate(aka::Time deltaTime)
{
	for (AssetViewerBase*& viewer : m_assetViewers)
	{
		viewer->update(deltaTime);
	}
}

void AssetViewerEditorLayer::onRender(aka::Renderer* _renderer, aka::gfx::FrameHandle frame)
{
	// Destroy viewer that where removed recently.
	for (AssetViewerBase*& viewer : m_assetViewersToDestroy)
	{
		viewer->destroy(_renderer->getDevice());
		mem::akaDelete(viewer);
	}
	m_assetViewersToDestroy.clear();
	// Create viewer that where added recently.
	for (AssetViewerBase*& viewer : m_assetViewersToCreate)
	{
		viewer->create(_renderer->getDevice());
	}
	m_assetViewers.append(m_assetViewersToCreate.begin(), m_assetViewersToCreate.end());
	m_assetViewersToCreate.clear();
	for (AssetViewerBase*& viewer : m_assetViewers)
	{
		viewer->render(_renderer->getDevice(), frame);
	}
}

void AssetViewerEditorLayer::onDrawUI(aka::DebugDrawList& debugDrawList)
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
				ICON_FA_FILM, // Scene
				ICON_FA_TREE_CITY, // StaticMesh
				ICON_FA_IMAGES,// Material
				ICON_FA_IMAGE,// Texture
				ICON_FA_HEADPHONES,// AudioSource
			};
			String tabName = String::format("%s %s##%p", typeLogo[EnumToIndex(viewer->getResourceType())], viewer->getResourceName(), viewer);
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

template<> 
void AssetViewerEditorLayer::open(aka::AssetID _assetID, aka::ResourceHandle<aka::Scene>& _resourceHandle)
{
	setVisible(true);
	for (AssetViewerBase*& viewer : m_assetViewers)
	{
		if (_assetID == viewer->getAssetID())
		{
			// Resource already opened.
			return;
		}
	}
	m_assetViewersToCreate.append(mem::akaNew<SceneViewer>(AllocatorMemoryType::Object, AllocatorCategory::Editor, _assetID, _resourceHandle));
}

template<>
void AssetViewerEditorLayer::open(aka::AssetID _assetID, aka::ResourceHandle<aka::StaticMesh>& _resourceHandle)
{
	setVisible(true);
	for (AssetViewerBase*& viewer : m_assetViewers)
	{
		if (_assetID == viewer->getAssetID())
		{
			// Resource already opened.
			return;
		}
	}
	m_assetViewersToCreate.append(mem::akaNew<StaticMeshViewer>(AllocatorMemoryType::Object, AllocatorCategory::Editor, _assetID, _resourceHandle));
}

template<>
void AssetViewerEditorLayer::open(aka::AssetID _assetID, aka::ResourceHandle<aka::Texture>& _resourceHandle)
{
	setVisible(true);
	for (AssetViewerBase*& viewer : m_assetViewers)
	{
		if (_assetID == viewer->getAssetID())
		{
			// Resource already opened.
			return;
		}
	}
	m_assetViewersToCreate.append(mem::akaNew<TextureViewer>(AllocatorMemoryType::Object, AllocatorCategory::Editor, _assetID, _resourceHandle));
}

};