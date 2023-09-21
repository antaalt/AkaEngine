#include "AssetEditorLayer.hpp"

#include <Aka/Layer/ImGuiLayer.h>
#include <Aka/Scene/World.h>
#include <Aka/OS/OS.h>
#include <Aka/Resource/Importer/Importer.hpp>

#include "../Importer/AssimpImporter.hpp"

namespace app {

using namespace aka;

struct AssetNode
{
	AssetID id = AssetID::Invalid;
	String name = "";
	AssetType type = AssetType::Unknown;
	size_t size = 0;

	// Using std vector cuz aka::Vector cause stack overflow:
	// Should not call constructor for reserved memory.
	std::vector<AssetNode> childrens = {};
	static void draw(AssetLibrary* library, const AssetNode& node)
	{
		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		const bool isFolder = (node.childrens.size() > 0);
		if (isFolder)
		{
			bool open = ImGui::TreeNodeEx(node.name.cstr(), ImGuiTreeNodeFlags_SpanFullWidth);
			ImGui::TableNextColumn();
			ImGui::TextDisabled("--");
			ImGui::TableNextColumn();
			ImGui::TextUnformatted("");
			if (open)
			{
				for (int iChild = 0; iChild < node.childrens.size(); iChild++)
					AssetNode::draw(library, node.childrens[iChild]);
				ImGui::TreePop();
			}
		}
		else // isFile
		{
			const char* popUpName = node.name.cstr(); // TODO: avoid duplicated name, should use path...
			if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1))
				ImGui::OpenPopup(popUpName);

			ImGui::TreeNodeEx(node.name.cstr(), ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanFullWidth);
			ImGui::TableNextColumn();
			ImGui::Text("%u", node.size);
			ImGui::TableNextColumn();
			ImGui::TextUnformatted(getAssetTypeString(node.type));

			if (ImGui::BeginPopupContextItem(popUpName))
			{
				if (ImGui::MenuItem("Load"))
				{
					switch (node.type)
					{
					case AssetType::Scene:
						EventDispatcher<SceneSwitchEvent>::trigger(SceneSwitchEvent{
							library->load<Scene>(library->getResourceID(node.id), Application::app()->graphic())
							});
						break;
					case AssetType::StaticMesh:
						library->load<StaticMesh>(library->getResourceID(node.id), Application::app()->graphic());
						break;
					case AssetType::Image:
						library->load<Texture>(library->getResourceID(node.id), Application::app()->graphic());
						break;
					default:
						Logger::warn("Trying to load unimplemented type.");
						break;
					}
				}
				ImGui::EndPopup();
			}
		}
	}
	static AssetNode* addNodeToTree(AssetNode& node, const AssetInfo& assetInfo, const String* directories, size_t count)
	{
		// TODO should not compute this every time, only when asset added / removed. event ?
		if (count == 0)
			return &node;
		for (AssetNode& node : node.childrens)
		{
			if (node.name == directories[0])
			{
				return addNodeToTree(node, assetInfo, &directories[1], --count);
			}
		}
		// Not found if we reach here. Create a folder & inspect it
		AssetType type = (count == 1) ? assetInfo.type : AssetType::Unknown;
		node.childrens.push_back(AssetNode{ assetInfo.id, directories[0], type, 0, {} });
		return addNodeToTree(node.childrens.back(), assetInfo, &directories[1], --count);
	}
};

AssetEditorLayer::AssetEditorLayer()
{
	m_rootNode = new AssetNode();
}

AssetEditorLayer::~AssetEditorLayer()
{
	delete m_rootNode;
}

void AssetEditorLayer::onLayerCreate(gfx::GraphicDevice* _device)
{
	m_viewerManager.onCreate(_device);
}

void AssetEditorLayer::onLayerDestroy(gfx::GraphicDevice* _device)
{
	m_viewerManager.onDestroy(_device);
}

void AssetEditorLayer::onLayerUpdate(Time deltaTime)
{
	m_viewerManager.onUpdate(deltaTime);
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
static void drawResource(const char* type, AssetLibrary* library, AssetViewerManager& viewerManager)
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
		auto getStatusString = [](ResourceState state) -> const char* {
			switch (state)
			{
			case ResourceState::Disk: return "Disk";
			case ResourceState::Loaded: return "Loaded";
			case ResourceState::Pending: return "Pending";
			default:
			case ResourceState::Unknown: return "Unknown";
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
	if (opened)
		viewerManager.open<T>(pair.first, pair.second);
}

void AssetEditorLayer::onLayerRender(aka::gfx::Frame* frame)
{
	ImGui::ShowDemoWindow();
	bool openImportWindow = false;
	if (ImGui::Begin("Asset Browser", nullptr, ImGuiWindowFlags_MenuBar))
	{
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
					importDeferred([&](const aka::Path& path) -> bool{
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

		// We should list all archive from library here.
		// We could open them in separate tab to edit their content & save them.

		// -------------------------
		// -------- ASSETS ---------
		// -------------------------
		ImGui::TextColored(ImGuiLayer::Color::red, "Assets");
		// Generate asset tree.
		if (m_assetUpdated)
		{
			for (const auto& asset : m_library->getAssetRange())
			{
				const AssetID& assetID = asset.first;
				const AssetInfo& assetInfo = asset.second;
				String path = assetInfo.path.cstr();
				Vector<String> directories = path.split('/');
				AssetNode::addNodeToTree(*m_rootNode, assetInfo, directories.data(), directories.size());
				// if not found, add it.
			
			}
			m_assetUpdated = false;
		}
		// First we need to iterate all assets & sort them by path & iterate on this.
		if (ImGui::BeginTable("Assets", 3, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_ScrollY, ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 20)))
		{
			ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableHeadersRow();
			for (const AssetNode& node : m_rootNode->childrens)
				AssetNode::draw(m_library, node);
			ImGui::EndTable();
		}

		ImGui::Separator();
		// -------------------------
		// ------- RESOURCES -------
		// -------------------------
		ImGui::TextColored(ImGuiLayer::Color::red, "Resources");

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
			drawResource<Scene>("scenes", m_library, m_viewerManager);
			drawResource<StaticMesh>("meshes", m_library, m_viewerManager);
			drawResource<Texture>("textures", m_library, m_viewerManager);
			ImGui::EndTable();
		}
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

	m_viewerManager.onRender(Application::app()->graphic(), frame);
	m_viewerManager.render(frame);
}

void AssetEditorLayer::onLayerPresent()
{
}

void AssetEditorLayer::onLayerResize(uint32_t width, uint32_t height)
{
}
void AssetEditorLayer::onReceive(const AssetAddedEvent& event)
{
	m_assetUpdated = true;
}
void AssetEditorLayer::setLibrary(AssetLibrary* _library)
{
	m_library = _library;
}

void AssetEditorLayer::importDeferred(std::function<bool(const aka::Path&)> callback)
{
	m_currentPath = AssetPath().getAbsolutePath();
	m_selectedPath = nullptr;
	m_paths = aka::OS::enumerate(m_currentPath);
	m_importCallback = callback;
}

};