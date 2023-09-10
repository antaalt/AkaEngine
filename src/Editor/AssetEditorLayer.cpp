#include "AssetEditorLayer.hpp"

#include <imgui.h>
#include <imguizmo.h>

#include <Aka/Scene/World.h>

#include "../Asset/AssetLibrary.hpp"
#include "../Importer/AssimpImporter.hpp"
#include "AssetViewer.hpp"

namespace app {

using namespace aka;

AssetEditorLayer::AssetEditorLayer()
{
	m_viewers.append(&m_meshViewer);
}

void AssetEditorLayer::onLayerCreate()
{
	for (AssetViewerBase* viewer : m_viewers)
		viewer->create();
}

void AssetEditorLayer::onLayerDestroy()
{
	for (AssetViewerBase* viewer : m_viewers)
		viewer->destroy();
}

void AssetEditorLayer::onLayerUpdate(Time deltaTime)
{
	for (AssetViewerBase* viewer : m_viewers)
		viewer->update(deltaTime);
}

void AssetEditorLayer::onLayerFrame()
{
}

// Make the UI compact because there are so many fields
static void PushStyleCompact()
{
	ImGuiStyle& style = ImGui::GetStyle();
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, (float)(int)(style.FramePadding.y * 0.60f)));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(style.ItemSpacing.x, (float)(int)(style.ItemSpacing.y * 0.60f)));
}

static void PopStyleCompact()
{
	ImGui::PopStyleVar(2);
}


template <typename T>
static void drawResource(const char* type, AssetLibrary* library, AssetViewer<T>* viewer)
{
	Application* app = Application::app();

	bool opened = false;
	std::pair<ResourceID, ResourceHandle<T>> pair;
	ImGui::TableNextRow();
	ImGui::TableNextColumn();
	bool open = ImGui::TreeNodeEx(type, ImGuiTreeNodeFlags_SpanFullWidth);
	ImGui::TableNextColumn();
	ImGui::TextDisabled("--");
	ImGui::TableNextColumn();
	ImGui::TextDisabled("--");
	ImGui::TableNextColumn();
	ImGui::TextDisabled("--"); // TODO compute total size ?
	ImGui::TableNextColumn();
	ImGui::TextDisabled("--");
	ImGui::TableNextColumn();
	ImGui::TextDisabled("--");
	if (open)
	{
		auto getStatusString = [](app::ResourceState state) -> const char* {
			switch (state)
			{
			case app::ResourceState::Disk: return "Disk";
			case app::ResourceState::Loaded: return "Loaded";
			case app::ResourceState::Pending: return "Pending";
			default:
			case app::ResourceState::Unknown: return "Unknown";
			}
			};
		for (std::pair<ResourceID, ResourceHandle<T>>& element : library->getRange<T>())
		{
			if (element.second.isLoaded())
			{
				T& e = element.second.get();
				AssetID assetID = library->getAssetID(element.first);
				AssetInfo assetInfo = library->getAssetInfo(assetID);
				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::TreeNodeEx(e.getName().cstr(), ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanFullWidth);
				if (ImGui::IsItemClicked() && ImGui::IsMouseDoubleClicked(0))
				{
					opened = true;
					pair = element;
				}
				ImGui::TableNextColumn();
				ImGui::Text("%s", assetInfo.path.cstr());
				ImGui::TableNextColumn();
				ImGui::Text("%ld", element.second.getCount() - 1); // Remove self use (within allocator)
				ImGui::TableNextColumn();
				//ImGui::Text("%ld bytes", assetInfo.size); // TODO add size
				ImGui::Text("-- bytes");
				ImGui::TableNextColumn();
				ImGui::TextUnformatted(type);
				ImGui::TableNextColumn();
				ImGui::TextUnformatted(getStatusString(element.second.getState()));
			}
		}
		ImGui::TreePop();
	}
	if (opened && viewer != nullptr)
		viewer->set(pair.first, pair.second);
}

void AssetEditorLayer::onLayerRender(aka::gfx::Frame* frame)
{
	ImGui::ShowDemoWindow();
	if (ImGui::Begin("ArchiveEditor", nullptr, ImGuiWindowFlags_MenuBar))
	{
		bool openImportWindow = false;
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Load"))
				{
					m_library->parse();
				}
				if (ImGui::MenuItem("Save"))
				{
					m_library->serialize();
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Import"))
			{
				if (ImGui::MenuItem("Scene"))
				{
					openImportWindow = true;
					import([&](const aka::Path& path) -> bool{
						aka::Logger::info("Importing scene : ", path);

						AssimpImporter importer;
						ImportResult res = importer.import(m_library, path);
						return ImportResult::Succeed == res;
					});
				}
				if (ImGui::MenuItem("Mesh"))
				{
				}
				if (ImGui::MenuItem("Texture"))
				{
				}
				if (ImGui::MenuItem("Audio"))
				{
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		static const ImVec4 color = ImVec4(0.93f, 0.04f, 0.26f, 1.f);
		// We should list all archive from library here.
		// We could open them in separate tab to edit their content & save them.

		// -------------------------
		// -------- ASSETS ---------
		// -------------------------
		ImGui::TextColored(color, "Assets");
		static int s_flags = (1 << EnumCount<app::AssetType>()) - 1;
		PushStyleCompact();
		for (app::AssetType type : EnumRange<app::AssetType>())
		{
			// TODO handle window size.
			ImGui::CheckboxFlags(getAssetTypeString(type), &s_flags, 1 << EnumToIndex(type));
			if (type != app::AssetType::Last)
				ImGui::SameLine();
		}
		PopStyleCompact();

		if (ImGui::BeginTable("Assets", 4, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_ScrollY, ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 20)))
		{
			ImGui::TableSetupColumn("AssetID", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn("Load", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn("Path", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupScrollFreeze(0, 1); // Make row always visible
			ImGui::TableHeadersRow();
			for (auto asset : m_library->getAssetRange())
			{
				if ((s_flags & (1 << EnumToIndex(asset.second.type))) == 0)
					continue;
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::Text("%llu", (uint64_t)asset.second.id);
				ImGui::TableSetColumnIndex(1);
				ImGui::Text("%s", getAssetTypeString(asset.second.type));
				ImGui::TableSetColumnIndex(2);
				app::ResourceType res = app::getResourceType(asset.second.type);
				if (res != app::ResourceType::Unknown)
				{
					app::ResourceID id = m_library->getResourceID(asset.second.id);
					if (ImGui::SmallButton("load"))
					{

						// Load if resource
					}
				}
				ImGui::TableSetColumnIndex(3);
				ImGui::Text("%s", (uint64_t)asset.second.path.cstr());
			}
			ImGui::EndTable();
		}

		ImGui::Separator();
		// -------------------------
		// ------- RESOURCES -------
		// -------------------------
		ImGui::TextColored(color, "Resources");
		
		static ImGuiTableFlags flags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody;

		if (ImGui::BeginTable("Resources", 6, flags))
		{
			ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn("Path", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn("Count", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn("Status", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableHeadersRow();
			drawResource<Scene>("scenes", m_library, nullptr);
			drawResource<StaticMesh>("meshes", m_library, &m_meshViewer);
			ImGui::EndTable();
		}

	ImGui::End();
	// -------------------------
	// ------- IMPORTER --------
	// -------------------------
	{
		if (openImportWindow)
			ImGui::OpenPopup("Import##Popup");
		bool openFlag = true;
		if (ImGui::BeginPopupModal("Import##Popup", &openFlag, ImGuiWindowFlags_AlwaysAutoResize))
		{
			bool updated = false;
			if (ImGui::Button("^"))
			{
				m_currentPath = m_currentPath.up();
				updated = true;
			}
			// Refresh directory
			ImGui::SameLine();
			if (ImGui::Button("*"))
			{
				updated = true;
			}
			ImGui::SameLine();
			// Path
			char currentPathBuffer[256];
			aka::String::copy(currentPathBuffer, 256, m_currentPath.cstr());
			if (ImGui::InputText("##Path", currentPathBuffer, 256))
			{
				m_currentPath = currentPathBuffer;
				updated = true;
			}
			if (ImGui::BeginChild("##files", ImVec2(0, 200), true))
			{
				char buffer[256];
				for (aka::Path& path : m_paths)
				{
					bool selected = (&path == m_selectedPath);
					bool isFolder = OS::Directory::exist(m_currentPath + path);
					if (isFolder)
					{
						int err = snprintf(buffer, 256, "%s %s", "D", path.cstr());
						if (ImGui::Selectable(buffer, &selected))
						{
							m_selectedPath = &path;
						}
						if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
						{
							m_currentPath += path;
							updated = true;
						}
					}
					else
					{
						int err = snprintf(buffer, 256, "%s %s", "F", path.cstr());
						if (ImGui::Selectable(buffer, &selected))
						{
							m_selectedPath = &path;
						}
						if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
						{
							if (m_importCallback(m_currentPath + path))
								ImGui::CloseCurrentPopup();
						}
					}
				}
			}
			ImGui::EndChild();
			if (updated)
			{
				m_paths = OS::enumerate(m_currentPath);
			}
			if (ImGui::Button("Import"))
			{
				if (m_selectedPath != nullptr)
				{
					if (m_importCallback(m_currentPath + *m_selectedPath))
						ImGui::CloseCurrentPopup();
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Close"))
			{
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}

	for (AssetViewerBase* viewer : m_viewers)
		viewer->render(frame);
	}
}

void AssetEditorLayer::onLayerPresent()
{
}

void AssetEditorLayer::onLayerResize(uint32_t width, uint32_t height)
{
}
void AssetEditorLayer::setLibrary(AssetLibrary* _library)
{
	m_library = _library;
}

void AssetEditorLayer::import(std::function<bool(const aka::Path&)> callback)
{
	m_currentPath = AssetPath().getAbsolutePath();
	m_selectedPath = nullptr;
	m_paths = aka::OS::enumerate(m_currentPath);
	m_importCallback = callback;
}

};