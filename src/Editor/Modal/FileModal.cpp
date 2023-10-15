#include "ImportModal.hpp"

#include <Aka/Resource/Importer/TextureImporter.hpp>
#include "../../Importer/AssimpImporter.hpp"

namespace app {

using namespace aka;

ImGuiFileModal::ImGuiFileModal(const char* name, const Path& path, bool assetPath) :
	m_name(name),
	m_currentPath(path),
	m_selectedPath(nullptr),
	m_isAssetPath(assetPath),
	m_pathHeader(m_isAssetPath ? AssetPath::getAssetPath() + "/" : "")
{
	m_paths = OS::enumerate(m_pathHeader + m_currentPath);
}
void ImGuiFileModal::SetupCallback(std::function<bool(const aka::Path&)>&& callback)
{
	m_callback = callback;
}
void ImGuiFileModal::SetupPath(const Path& path)
{
	m_currentPath = path;
}
void ImGuiFileModal::Open()
{
	ImGui::OpenPopup(m_name.cstr());
}
void ImGuiFileModal::Render()
{
	bool openFlag = true;
	if (ImGui::BeginPopupModal(m_name.cstr(), &openFlag, ImGuiWindowFlags_AlwaysAutoResize))
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
				bool isFolder = OS::Directory::exist(m_pathHeader + m_currentPath + path);
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
						if (m_callback(m_currentPath + path))
							ImGui::CloseCurrentPopup();
					}
				}
			}
		}
		ImGui::EndChild();
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Create folder"))
			{
				OS::Directory::create(m_pathHeader + m_currentPath + "new folder/");
				updated = true;
			}
			if (ImGui::MenuItem("Delete folder", nullptr, nullptr, m_selectedPath != nullptr))
			{
				if (m_selectedPath != nullptr)
					OS::Directory::remove(m_pathHeader + m_currentPath + *m_selectedPath);
				updated = true;
			}
			ImGui::EndPopup();
		}
		if (updated)
		{
			m_paths = OS::enumerate(m_pathHeader + m_currentPath);
			m_selectedPath = nullptr;
		}
		if (ImGui::Button("Select"))
		{
			Path path = m_selectedPath ? *m_selectedPath : "";

			if (m_callback(m_currentPath + path))
				ImGui::CloseCurrentPopup();

		}
		ImGui::SameLine();
		if (ImGui::Button("Close"))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

};