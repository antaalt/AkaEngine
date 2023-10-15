#include "ImportModal.hpp"

#include <Aka/Resource/Importer/TextureImporter.hpp>
#include "../../Importer/AssimpImporter.hpp"

namespace app {

using namespace aka;

ImGuiImportModal::ImGuiImportModal(const char* windowName) :
	m_windowName(windowName),
	m_assetType(AssetType::Unknown),
	m_name("New scene"),
	m_inputModal("Source", OS::normalize(aka::OS::cwd() + AssetPath::getAssetPath().up()), false),
	m_outputModal("Destination", "/", true)
{
	m_name.resize(256);
	m_inputPath.resize(256);
	m_outputPath.resize(256);
	m_inputModal.SetupCallback([&](const Path& path) {
		m_inputPath = path;
		return true;
		});
	m_outputModal.SetupCallback([&](const Path& path) {
		m_outputPath = path;
		return true;
		});
}
void ImGuiImportModal::RequestImportType(aka::AssetType type)
{
	m_assetType = type;
	Open();
}
void ImGuiImportModal::Open()
{
	ImGui::OpenPopup(m_windowName.cstr());
}
void ImGuiImportModal::Close()
{
	ImGui::CloseCurrentPopup();
}
void ImGuiImportModal::Render()
{
	bool openFlag = true;
	if (ImGui::BeginPopupModal(m_windowName.cstr(), &openFlag, ImGuiWindowFlags_AlwaysAutoResize))
	{
		{
			if (ImGui::Button("Explore##Source"))
			{
				m_inputModal.Open();
			}
			ImGui::SameLine();
			if (ImGui::InputText("Source", m_inputPath.cstr(), 256))
			{
				m_inputModal.SetupPath(m_inputPath);
			}
			m_inputModal.Render();
		}
		{
			if (ImGui::Button("Explore##Destination"))
			{
				m_outputModal.Open();
			}
			ImGui::SameLine();
			if (ImGui::InputText("Destination", m_outputPath.cstr(), 256))
			{
				m_outputModal.SetupPath(m_outputPath);
			}
			m_outputModal.Render();
		}
		ImGui::InputText("Name", m_name.cstr(), 256);

		bool overwrite = false;
		ImGui::Checkbox("Overwrite", &overwrite);

		ImGui::Separator();
		if (ImGui::Button("Import"))
		{
			m_importRequested = true;
		}
		ImGui::SameLine();
		if (ImGui::Button("Close"))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}
bool ImGuiImportModal::Import(AssetLibrary* library)
{
	if (!m_importRequested)
		return false;
	AssetPath assetPath;
	assetPath.fromRawPath(m_outputPath);
	m_importRequested = false;
	switch (m_assetType)
	{
	case aka::AssetType::Sprite: {
		//SpriteImporter;
		return false;
	}
	case aka::AssetType::Image: {
		aka::Logger::info("Importing texture : ", m_inputPath);
		TextureImporter importer(library);
		importer.setAssetPath(assetPath);
		importer.setName(m_name);
		importer.setFlag(ImportFlag::Overwrite);
		ImportResult res = importer.import(m_inputPath);
		return res == ImportResult::Succeed;
	}
	case aka::AssetType::Scene: {
		aka::Logger::info("Importing scene : ", m_inputPath);
		AssimpImporter importer(library);
		importer.setAssetPath(assetPath);
		importer.setName(m_name);
		importer.setFlag(ImportFlag::Overwrite);
		ImportResult res = importer.import(m_inputPath);
		return res == ImportResult::Succeed;
	}
	default:
		return false;
	}
	return false;
}

};