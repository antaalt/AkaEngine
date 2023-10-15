#pragma once

#include <Aka/Layer/ImGuiLayer.h>
#include <Aka/OS/Path.h>
#include <Aka/Resource/Asset.hpp>

#include <functional>

namespace app {

struct ImGuiFileModal
{
	ImGuiFileModal(const char* name, const aka::Path& path, bool assetPath);

	void SetupCallback(std::function<bool(const aka::Path&)>&& callback);
	void SetupPath(const aka::Path& path);
	void Open();
	void Render();
private:
	std::function<bool(const aka::Path&)> m_callback;
	bool m_isAssetPath;
	aka::String m_name;
	aka::Path m_currentPath;
	aka::Path m_pathHeader;
	aka::Path* m_selectedPath;
	std::vector<aka::Path> m_paths;
};
};