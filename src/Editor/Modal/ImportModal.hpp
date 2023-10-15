#pragma once

#include <Aka/Layer/ImGuiLayer.h>
#include <Aka/OS/Path.h>
#include <Aka/Resource/Asset.hpp>

#include "FileModal.hpp"
#include <functional>

namespace app {

struct ImGuiImportModal
{
	ImGuiImportModal(const char* windowName);
	void RequestImportType(aka::AssetType type);
	void Open();
	void Close();
	void Render();
	bool Import(aka::AssetLibrary* library);
private:
	aka::String m_windowName;
	aka::AssetType m_assetType;
	bool m_importRequested = false;
	aka::String m_name;
	aka::Path m_inputPath;
	aka::Path m_outputPath;
	ImGuiFileModal m_inputModal;
	ImGuiFileModal m_outputModal;
};

};