#include "AssetBrowserEditorLayer.hpp"

#include <Aka/Layer/ImGuiLayer.h>
#include <Aka/OS/OS.h>
#include <Aka/Resource/Importer/Importer.hpp>
#include <Aka/Resource/Importer/TextureImporter.hpp>
#include <Aka/Resource/AssetLibrary.hpp>

#include "../Importer/AssimpImporter.hpp"

namespace app {

using namespace aka;

ResourceState getResourceState(AssetID id, AssetType type, AssetLibrary* library)
{
	switch (type)
	{
	case AssetType::Scene: return library->getState<Scene>(id);
	case AssetType::StaticMesh: return library->getState<StaticMesh>(id);
	case AssetType::Image: return library->getState<Texture>(id);
	default: return ResourceState::Unknown;
	}
}

const char* getStatusString(ResourceState state)
{
	switch (state)
	{
	case ResourceState::Disk: return "Disk";
	case ResourceState::Loaded: return "Loaded";
	case ResourceState::Pending: return "Pending";
	case ResourceState::Unknown: return "--";
	default:
		AKA_ASSERT(false, "");
		return "Unknown";
	}
};

void load(AssetID id, AssetType type, AssetLibrary* library)
{
	switch (type)
	{
	case AssetType::Scene:
		library->load<Scene>(id, Application::app()->renderer());
		break;
	case AssetType::StaticMesh:
		library->load<StaticMesh>(id, Application::app()->renderer());
		break;
	case AssetType::Image:
		library->load<Texture>(id, Application::app()->renderer());
		break;
	default:
		// Might try to open unimplemented type.
		Logger::warn("Trying to load unimplemented type.");
		break;
	}
}

struct AssetNode
{
	AssetID id = AssetID::Invalid;
	String name = "";
	AssetPath path = "";
	AssetType type = AssetType::Unknown;
	size_t size = 0;

	Vector<AssetNode> childrens = {};
	static const uint32_t ColumnCount = 4;
	static void drawHeader()
	{
		// Should add
		// - name (hard)
		// - size (cache)
		// - count (hard, might not be required, only when selecting item)
		ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn("Status", ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableHeadersRow();
	}
	static void draw(AssetLibrary* library, const AssetNode& node, const AssetNode*& currentNode, AssetViewerEditorLayer& viewerManager)
	{
		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		const bool isFolder = (node.childrens.size() > 0);
		if (isFolder)
		{
			bool open = ImGui::TreeNodeEx(node.name.cstr(), ImGuiTreeNodeFlags_SpanFullWidth);
			ImGui::TableNextColumn();
			ImGui::TextDisabled("--"); // Size
			ImGui::TableNextColumn();
			ImGui::TextUnformatted(""); // Type
			ImGui::TableNextColumn();
			ImGui::TextUnformatted(""); // Status
			if (open)
			{
				for (int iChild = 0; iChild < node.childrens.size(); iChild++)
					AssetNode::draw(library, node.childrens[iChild], currentNode, viewerManager);
				ImGui::TreePop();
			}
		}
		else // isFile
		{
			const ResourceState state = getResourceState(node.id, node.type, library);
			const bool isLoaded = state == ResourceState::Loaded;
			const bool isResource = state != ResourceState::Unknown;
			const bool current = currentNode && (currentNode->id == node.id);
			const char* popUpName = node.path.cstr();

			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanFullWidth;

			ImU32 textColor = ImColor(0.4f, 0.4f, 0.4f);
			if (isLoaded)
				textColor = ImColor(1.f, 1.f, 1.f);
			else if (isResource)
				textColor = ImColor(0.6f, 0.6f, 0.6f);
			if (current)
			{
				textColor = ImColor(1.f, 1.f, 1.f);
				flags |= ImGuiTreeNodeFlags_Selected;
			}
			ImGui::PushStyleColor(ImGuiCol_Text, textColor);
			bool selected = currentNode == &node;
			if (ImGui::Selectable(node.name.cstr(), &selected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowDoubleClick))
			{
				currentNode = &node;
				if (isResource && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				{
					if (node.type == AssetType::Scene)
					{
						// This will load or simply get the scene if it already exist.
						EventDispatcher<SceneSwitchEvent>::trigger(SceneSwitchEvent{
							library->load<Scene>(node.id, Application::app()->renderer())
						});
					}
					else
					{
						load(node.id, node.type, library);
					}
				}
			}
			ImGui::PopStyleColor();
			if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1))
				ImGui::OpenPopup(popUpName);
			ImGui::TableNextColumn();
			ImGui::Text("%u bytes", node.size); // Size
			ImGui::TableNextColumn();
			ImGui::TextUnformatted(getAssetTypeString(node.type)); // Type
			ImGui::TableNextColumn();
			ImGui::TextUnformatted(getStatusString(state)); // Status

			if (ImGui::BeginPopupContextItem(popUpName))
			{
				if (ImGui::MenuItem("Open", nullptr, nullptr, (node.type == AssetType::Scene)))
				{
					// This will load or simply get the scene if it already exist.
					EventDispatcher<SceneSwitchEvent>::trigger(SceneSwitchEvent{
						library->load<Scene>(node.id, Application::app()->renderer())
					});
				}
				if (ImGui::MenuItem("Load", nullptr, nullptr, state == ResourceState::Disk))
				{
					load(node.id, node.type, library);
				}
				if (ImGui::MenuItem("View", nullptr, nullptr, state == ResourceState::Loaded))
				{
					switch (node.type)
					{
					case AssetType::Scene:
						viewerManager.open<Scene>(node.id, library->get<Scene>(node.id));
						break;
					case AssetType::StaticMesh:
						viewerManager.open<StaticMesh>(node.id, library->get<StaticMesh>(node.id));
						break;
					case AssetType::Image:
						viewerManager.open<Texture>(node.id, library->get<Texture>(node.id));
						break;
					default:
						AKA_NOT_IMPLEMENTED;
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
		const bool isFolder = (count > 1);
		AssetNode newNode;
		newNode.type = AssetType::Unknown;
		newNode.path = "";
		newNode.size = 0;
		newNode.name = directories[0];
		newNode.id = AssetID::Invalid;
		if (!isFolder)
		{
			newNode.id = assetInfo.id;
			newNode.type = assetInfo.type;
			newNode.path = assetInfo.path;
			newNode.size = OS::File::size(assetInfo.path.getAbsolutePath());
		}
		node.childrens.append(newNode);
		return addNodeToTree(node.childrens.last(), assetInfo, &directories[1], --count);
	}
};

AssetBrowserEditorLayer::AssetBrowserEditorLayer() :
	EditorLayer("Asset browser")
{
	m_rootNode = new AssetNode();
}

AssetBrowserEditorLayer::~AssetBrowserEditorLayer()
{
	delete m_rootNode;
}

void AssetBrowserEditorLayer::onCreate(gfx::GraphicDevice* _device)
{
}

void AssetBrowserEditorLayer::onDestroy(gfx::GraphicDevice* _device)
{
}

void AssetBrowserEditorLayer::onUpdate(Time deltaTime)
{
}

void AssetBrowserEditorLayer::onPreRender()
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

void AssetBrowserEditorLayer::onRender(aka::gfx::GraphicDevice* _device, aka::gfx::Frame* frame)
{
}

void AssetBrowserEditorLayer::onDrawUI()
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
				importDeferred([&](const aka::Path& path) -> bool{
					aka::Logger::info("Importing scene : ", path);
					AssimpImporter importer(m_library);
					ImportResult res = importer.import(path);
					return ImportResult::Succeed == res;
				});
			}
			if (ImGui::MenuItem("Mesh"))
			{
			}
			if (ImGui::MenuItem("Texture"))
			{
				openImportWindow = true;
				importDeferred([&](const aka::Path& path) -> bool {
					aka::Logger::info("Importing texture : ", path);
					TextureImporter importer(m_library);
					ImportResult res = importer.import(path);
					return ImportResult::Succeed == res;
				});
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
		m_currentNode = nullptr;
		m_assetUpdated = false;
	}
	// First we need to iterate all assets & sort them by path & iterate on this.
	if (ImGui::BeginTable("Assets", AssetNode::ColumnCount, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_ScrollY, ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 20)))
	{
		AssetNode::drawHeader();
		for (const AssetNode& node : m_rootNode->childrens)
			AssetNode::draw(m_library, node, m_currentNode, *m_viewerEditor);
		ImGui::EndTable();
	}
	ImGui::Separator();
	if (m_currentNode != nullptr)
	{
		ImGui::TextColored(ImGuiLayer::Color::red, "%s", m_currentNode->name.cstr());
		ImGui::Text("Path: %s", m_currentNode->path.cstr());
		ImGui::Text("Size: %llu bytes", m_currentNode->size);
		ImGui::Text("ID: %llu", (uint64_t)m_currentNode->id);
		ImGui::Text("Type: %s", getAssetTypeString(m_currentNode->type));
	}

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
			if (ImGui::Button(ICON_FA_ARROW_UP_LONG))
			{
				m_currentPath = m_currentPath.up();
				updated = true;
			}
			// Refresh directory
			ImGui::SameLine();
			if (ImGui::Button(ICON_FA_ROTATE_RIGHT))
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
						int err = snprintf(buffer, 256, "%s %s", ICON_FA_FOLDER, path.cstr());
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
						int err = snprintf(buffer, 256, "%s %s", ICON_FA_FILE, path.cstr());
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
}

void AssetBrowserEditorLayer::onPostRender()
{
}

void AssetBrowserEditorLayer::onResize(uint32_t width, uint32_t height)
{
}
void AssetBrowserEditorLayer::onReceive(const AssetAddedEvent& event)
{
	m_assetUpdated = true;
}
void AssetBrowserEditorLayer::setLibrary(AssetLibrary* _library)
{
	m_library = _library;
}

void AssetBrowserEditorLayer::setAssetViewer(AssetViewerEditorLayer* _viewer)
{
	m_viewerEditor = _viewer;
}

void AssetBrowserEditorLayer::importDeferred(std::function<bool(const aka::Path&)> callback)
{
	m_currentPath = AssetPath().getAbsolutePath();
	m_selectedPath = nullptr;
	m_paths = aka::OS::enumerate(m_currentPath);
	m_importCallback = callback;
}

};